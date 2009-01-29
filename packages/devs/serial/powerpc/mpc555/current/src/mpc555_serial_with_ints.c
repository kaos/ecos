//==========================================================================
//
//      mpc555_serial_with_ints.c
//
//      PowerPC 5xx MPC555 Serial I/O Interface Module (interrupt driven)
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Bob Koninckx
// Contributors:
// Date:        2002-04-25
// Purpose:     MPC555 Serial I/O module (interrupt driven version)
// Description: 
//
//   
//####DESCRIPTIONEND####
//==========================================================================
//----------------------------------
// Includes and forward declarations
//----------------------------------
#include <pkgconf/io_serial.h>
#include <pkgconf/io.h>

#include <cyg/io/io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_arbiter.h>
#include <cyg/io/devtab.h>
#include <cyg/infra/diag.h>
#include <cyg/io/serial.h>

// Only build this driver for the MPC555 based boards
#if defined (CYGPKG_IO_SERIAL_POWERPC_MPC555) && \
   (defined (CYGPKG_IO_SERIAL_POWERPC_MPC555_SERIAL_A) || \
    defined (CYGPKG_IO_SERIAL_POWERPC_MPC555_SERIAL_B))

#include "mpc555_serial.h"

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------
#define MPC555_SCI_RX_BUFF_SIZE 256
typedef struct st_sci_circbuf {
  cyg_uint8 buf[MPC555_SCI_RX_BUFF_SIZE];
  cyg_uint16 scsr[MPC555_SCI_RX_BUFF_SIZE];
  cyg_uint8 fill_pos;
  cyg_uint8 read_pos;
} mpc555_sci_circbuf_t;

typedef struct mpc555_serial_info {
  CYG_ADDRWORD   base;                 // The base address of the serial port
  CYG_WORD       tx_interrupt_num;     // trivial
  CYG_WORD       rx_interrupt_num;     // trivial
  cyg_priority_t tx_interrupt_priority;// trivial
  cyg_priority_t rx_interrupt_priority;// trivial
  bool           tx_interrupt_enable;  // can the tx interrupt be re-enabled?
  mpc555_sci_circbuf_t* rx_circbuf;    // rx buff for ISR to DSR data exchange
  bool           use_queue;            // Use the queue when available?
  CYG_WORD       rx_last_queue_pointer;// Keep track where queue read is upto
  CYG_WORD       rx_interrupt_idle_line_num;             // trivial
  CYG_WORD       tx_interrupt_queue_top_empty_num;       // trivial
  CYG_WORD       tx_interrupt_queue_bot_empty_num;       // trivial
  CYG_WORD       rx_interrupt_queue_top_full_num;        // trivial
  CYG_WORD       rx_interrupt_queue_bot_full_num;        // trivial
  cyg_priority_t rx_interrupt_idle_line_priority;        // trivial
  cyg_priority_t tx_interrupt_queue_top_empty_priority;  // trivial
  cyg_priority_t tx_interrupt_queue_bot_empty_priority;  // trivial
  cyg_priority_t rx_interrupt_queue_top_full_priority;   // trivial
  cyg_priority_t rx_interrupt_queue_bot_full_priority;   // trivial
  cyg_interrupt  tx_interrupt;                 // the tx interrupt object
  cyg_handle_t   tx_interrupt_handle;          // the tx interrupt handle
  cyg_interrupt  rx_interrupt;                 // the rx interrupt object
  cyg_handle_t   rx_interrupt_handle;          // the rx interrupt handle
  cyg_interrupt  rx_idle_interrupt;            // the rx idle line isr object
  cyg_handle_t   rx_idle_interrupt_handle;     // the rx idle line isr handle
  cyg_interrupt  tx_queue_top_interrupt;       // the tx interrupt object
  cyg_handle_t   tx_queue_top_interrupt_handle;// the tx interrupt handle
  cyg_interrupt  tx_queue_bot_interrupt;       // the tx interrupt object
  cyg_handle_t   tx_queue_bot_interrupt_handle;// the tx interrupt handle
  cyg_interrupt  rx_queue_top_interrupt;       // the tx interrupt object
  cyg_handle_t   rx_queue_top_interrupt_handle;// the tx interrupt handle
  cyg_interrupt  rx_queue_bot_interrupt;       // the tx interrupt object
  cyg_handle_t   rx_queue_bot_interrupt_handle;// the tx interrupt handle
} mpc555_serial_info;

//--------------------
// Function prototypes
//--------------------
static bool mpc555_serial_putc(serial_channel * chan, unsigned char c);
static unsigned char mpc555_serial_getc(serial_channel *chan);
static Cyg_ErrNo mpc555_serial_set_config(serial_channel *chan, cyg_uint32 key,
                                          const void *xbuf, cyg_uint32 *len);
static void mpc555_serial_start_xmit(serial_channel *chan);
static void mpc555_serial_stop_xmit(serial_channel *chan);
static Cyg_ErrNo mpc555_serial_lookup(struct cyg_devtab_entry ** tab, 
                                      struct cyg_devtab_entry * sub_tab,
                                      const char * name);
static bool mpc555_serial_init(struct cyg_devtab_entry * tab);

// The interrupt servers
static cyg_uint32 mpc555_serial_tx_ISR(cyg_vector_t vector, cyg_addrword_t data);
static cyg_uint32 mpc555_serial_rx_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void mpc555_serial_tx_DSR(cyg_vector_t vector, 
                                 cyg_ucount32 count, 
                                 cyg_addrword_t data);
static void mpc555_serial_rx_DSR(cyg_vector_t vector, 
                                 cyg_ucount32 count,
                                 cyg_addrword_t data);

#ifdef CYGPKG_IO_SERIAL_POWERPC_MPC555_SERIAL_A
static cyg_uint32 mpc555_serial_tx_queue_top_ISR(cyg_vector_t vector,
                                                 cyg_addrword_t data);
static cyg_uint32 mpc555_serial_tx_queue_bot_ISR(cyg_vector_t vector,
                                                 cyg_addrword_t data);
static cyg_uint32 mpc555_serial_rx_queue_top_ISR(cyg_vector_t vector,
                                                 cyg_addrword_t data);
static cyg_uint32 mpc555_serial_rx_queue_bot_ISR(cyg_vector_t vector,
                                                 cyg_addrword_t data);
static cyg_uint32 mpc555_serial_rx_idle_line_ISR(cyg_vector_t vector,
                                                 cyg_addrword_t data);

static void mpc555_serial_tx_queue_DSR(cyg_vector_t vector,
                                       cyg_ucount32 count,
                                       cyg_addrword_t data);
static void mpc555_serial_rx_queue_DSR(cyg_vector_t vector,
                                       cyg_ucount32 count,
                                       cyg_addrword_t data);

static int mpc555_serial_read_queue(serial_channel* chan, int start, int end);
#endif

//------------------------------------------------------------------------------
// Register the device driver with the kernel
//------------------------------------------------------------------------------
static SERIAL_FUNS(mpc555_serial_funs, 
                   mpc555_serial_putc, 
                   mpc555_serial_getc,
                   mpc555_serial_set_config,
                   mpc555_serial_start_xmit,
                   mpc555_serial_stop_xmit);

//------------------------------------------------------------------------------
// Device driver data
//------------------------------------------------------------------------------
#ifdef CYGPKG_IO_SERIAL_POWERPC_MPC555_SERIAL_A
#ifdef CYGDAT_IO_SERIAL_POWERPC_MPC555_SERIAL_A_USE_HWARE_QUEUE
//static mpc555_sci_circbuf_t mpc555_serial_isr_to_dsr_buf0;

static mpc555_serial_info mpc555_serial_info0 = {
  MPC555_SERIAL_BASE_A,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_TX,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_RX,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_TX_PRIORITY,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_RX_PRIORITY,
  false,
  NULL, // Don't need software buffer
  true, // Use queue
  0,    // init queue pointer
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_IDLE, 
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_TXQTHE,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_TXQBHE,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_RXQTHF,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_RXQBHF,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_IDLE_PRIORITY,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_TXQTHE_PRIORITY,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_TXQBHE_PRIORITY,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_RXQTHF_PRIORITY,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_RXQBHF_PRIORITY};

static unsigned char mpc555_serial_out_buf0[CYGNUM_IO_SERIAL_POWERPC_MPC555_SERIAL_A_BUFSIZE]; 
static unsigned char mpc555_serial_in_buf0[CYGNUM_IO_SERIAL_POWERPC_MPC555_SERIAL_A_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(
  mpc555_serial_channel0,
  mpc555_serial_funs,
  mpc555_serial_info0,
  CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_POWERPC_MPC555_SERIAL_A_BAUD),
  CYG_SERIAL_STOP_DEFAULT,
  CYG_SERIAL_PARITY_DEFAULT,
  CYG_SERIAL_WORD_LENGTH_DEFAULT,
  CYG_SERIAL_FLAGS_DEFAULT,
  &mpc555_serial_out_buf0[0],
  sizeof(mpc555_serial_out_buf0),
  &mpc555_serial_in_buf0[0],
  sizeof(mpc555_serial_in_buf0));

#elif CYGNUM_IO_SERIAL_POWERPC_MPC555_SERIAL_A_BUFSIZE > 0
static mpc555_sci_circbuf_t mpc555_serial_isr_to_dsr_buf0;

static mpc555_serial_info mpc555_serial_info0 = {
  MPC555_SERIAL_BASE_A,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_TX,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_RX,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_TX_PRIORITY,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_RX_PRIORITY,
  false,
  &mpc555_serial_isr_to_dsr_buf0,
  false};

static unsigned char mpc555_serial_out_buf0[CYGNUM_IO_SERIAL_POWERPC_MPC555_SERIAL_A_BUFSIZE]; 
static unsigned char mpc555_serial_in_buf0[CYGNUM_IO_SERIAL_POWERPC_MPC555_SERIAL_A_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(
  mpc555_serial_channel0,
  mpc555_serial_funs,
  mpc555_serial_info0,
  CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_POWERPC_MPC555_SERIAL_A_BAUD),
  CYG_SERIAL_STOP_DEFAULT,
  CYG_SERIAL_PARITY_DEFAULT,
  CYG_SERIAL_WORD_LENGTH_DEFAULT,
  CYG_SERIAL_FLAGS_DEFAULT,
  &mpc555_serial_out_buf0[0],
  sizeof(mpc555_serial_out_buf0),
  &mpc555_serial_in_buf0[0],
  sizeof(mpc555_serial_in_buf0));
#else
static mpc555_serial_info mpc555_serial_info0 = {
  MPC555_SERIAL_BASE_A,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_TX,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_RX,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_TX_PRIORITY,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_RX_PRIORITY,
  false,
  NULL,
  false};
 
static SERIAL_CHANNEL(
  mpc555_serial_channel0,
  mpc555_serial_funs,
  mpc555_serial_info0,
  CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_POWERPC_MPC555_SERIAL_A_BAUD),
  CYG_SERIAL_STOP_DEFAULT,
  CYG_SERIAL_PARITY_DEFAULT,
  CYG_SERIAL_WORD_LENGTH_DEFAULT,
  CYG_SERIAL_FLAGS_DEFAULT);
#endif
DEVTAB_ENTRY(mpc555_serial_io0,
             CYGDAT_IO_SERIAL_POWERPC_MPC555_SERIAL_A_NAME,
             0, // does not depend on a lower level device driver
             &cyg_io_serial_devio,
             mpc555_serial_init,
             mpc555_serial_lookup,
             &mpc555_serial_channel0);
#endif // ifdef CYGPKG_IO_SERIAL_POWERPC_MPC555_SERIAL_A

#ifdef CYGPKG_IO_SERIAL_POWERPC_MPC555_SERIAL_B

#if CYGNUM_IO_SERIAL_POWERPC_MPC555_SERIAL_B_BUFSIZE > 0
static mpc555_sci_circbuf_t mpc555_serial_isr_to_dsr_buf1;

static mpc555_serial_info mpc555_serial_info1 = {
  MPC555_SERIAL_BASE_B,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI2_TX,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI2_RX,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI2_TX_PRIORITY,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI2_RX_PRIORITY,
  false,
  &mpc555_serial_isr_to_dsr_buf1,
  false};

static unsigned char mpc555_serial_out_buf1[CYGNUM_IO_SERIAL_POWERPC_MPC555_SERIAL_B_BUFSIZE]; 
static unsigned char mpc555_serial_in_buf1[CYGNUM_IO_SERIAL_POWERPC_MPC555_SERIAL_B_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(
  mpc555_serial_channel1,
  mpc555_serial_funs,
  mpc555_serial_info1,
  CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_POWERPC_MPC555_SERIAL_B_BAUD),
  CYG_SERIAL_STOP_DEFAULT,
  CYG_SERIAL_PARITY_DEFAULT,
  CYG_SERIAL_WORD_LENGTH_DEFAULT,
  CYG_SERIAL_FLAGS_DEFAULT,
  &mpc555_serial_out_buf1[0],
  sizeof(mpc555_serial_out_buf1),
  &mpc555_serial_in_buf1[0],
  sizeof(mpc555_serial_in_buf1));
#else
static mpc555_serial_info mpc555_serial_info1 = {
  MPC555_SERIAL_BASE_B,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_TX,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_RX,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_TX_PRIORITY,
  CYGNUM_HAL_INTERRUPT_IMB3_SCI1_RX_PRIORITY,
  false,
  NULL,
  false};
static SERIAL_CHANNEL(
  mpc555_serial_channel1,
  mpc555_serial_funs,
  mpc555_serial_info1,
  CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_POWERPC_MPC555_SERIAL_B_BAUD),
  CYG_SERIAL_STOP_DEFAULT,
  CYG_SERIAL_PARITY_DEFAULT,
  CYG_SERIAL_WORD_LENGTH_DEFAULT,
  CYG_SERIAL_FLAGS_DEFAULT);
#endif
DEVTAB_ENTRY(mpc555_serial_io1,
             CYGDAT_IO_SERIAL_POWERPC_MPC555_SERIAL_B_NAME,
             0, // does not depend on a lower level device driver
             &cyg_io_serial_devio,
             mpc555_serial_init,
             mpc555_serial_lookup,
             &mpc555_serial_channel1);
#endif // ifdef CYGPKG_IO_SERIAL_POWERPC_MPC555_SERIAL_B

//------------------------------------------------------------------------------
// Device driver implementation
//------------------------------------------------------------------------------

// The arbitration isr. 
// I think this is the best place to implement it.
// The device driver is the only place in the code where the knowledge is 
// present about how the hardware is used.
//
// Always check receive interrupts. 
// Some rom monitor might be waiting for CTRL-C
static cyg_uint32 hal_arbitration_isr_qsci(CYG_ADDRWORD a_vector, 
                                           CYG_ADDRWORD a_data)
{
  cyg_uint16 status;
  cyg_uint16 control;

  HAL_READ_UINT16(CYGARC_REG_IMM_SC1SR, status);
  HAL_READ_UINT16(CYGARC_REG_IMM_SCC1R1, control);
  if((status & CYGARC_REG_IMM_SCxSR_RDRF) && 
      (control & CYGARC_REG_IMM_SCCxR1_RIE))
    return hal_call_isr(CYGNUM_HAL_INTERRUPT_IMB3_SCI1_RX);
// Do not waist time on unused hardware
#ifdef CYGPKG_IO_SERIAL_POWERPC_MPC555_SERIAL_A
#ifdef CYGDAT_IO_SERIAL_POWERPC_MPC555_SERIAL_A_USE_HWARE_QUEUE
  // Only one port supports queue mode
  if((status & CYGARC_REG_IMM_SCxSR_IDLE) && 
      (control & CYGARC_REG_IMM_SCCxR1_ILIE))
    return hal_call_isr(CYGNUM_HAL_INTERRUPT_IMB3_SCI1_IDLE);
  HAL_READ_UINT16(CYGARC_REG_IMM_QSCI1SR, status);
  HAL_READ_UINT16(CYGARC_REG_IMM_QSCI1CR, control);
  if((status & CYGARC_REG_IMM_QSCI1SR_QTHF) && 
      (control & CYGARC_REG_IMM_QSCI1CR_QTHFI))
    return hal_call_isr(CYGNUM_HAL_INTERRUPT_IMB3_SCI1_RXQTHF);
  if((status & CYGARC_REG_IMM_QSCI1SR_QBHF) && 
      (control & CYGARC_REG_IMM_QSCI1CR_QBHFI))
    return hal_call_isr(CYGNUM_HAL_INTERRUPT_IMB3_SCI1_RXQBHF);
  if((status & CYGARC_REG_IMM_QSCI1SR_QTHE) && 
      (control & CYGARC_REG_IMM_QSCI1CR_QTHEI))
    return hal_call_isr(CYGNUM_HAL_INTERRUPT_IMB3_SCI1_TXQTHE);
  if((status & CYGARC_REG_IMM_QSCI1SR_QBHE) && 
      (control & CYGARC_REG_IMM_QSCI1CR_QBHEI))
    return hal_call_isr(CYGNUM_HAL_INTERRUPT_IMB3_SCI1_TXQBHE);
// Only for SPI, leave fo future reference
#if 0
  HAL_READ_UINT16(CYGARC_REG_IMM_SPSR, status);
  HAL_READ_UINT16(CYGARC_REG_IMM_SPCR2, control);
  if((status & CYGARC_REG_IMM_SPSR_SPIF) && 
      (control & CYGARC_REG_IMM_SPCR2_SPIFIE))
    return hal_call_isr(CYGNUM_HAL_INTERRUPT_IMB3_SPI_FI);

  HAL_READ_UINT16(CYGARC_REG_IMM_SPCR3, control);
  if((status & CYGARC_REG_IMM_SPSR_MODF) && 
      (control & CYGARC_REG_IMM_SPCR3_HMIE))
    return hal_call_isr(CYGNUM_HAL_INTERRUPT_IMB3_SPI_MODF);

  if((status & CYGARC_REG_IMM_SPSR_HALTA) && 
      (control & CYGARC_REG_IMM_SPCR3_HMIE))
    return hal_call_isr(CYGNUM_HAL_INTERRUPT_IMB3_SPI_HALTA);
#endif
#else  //No HW Queue
  if((status & CYGARC_REG_IMM_SCxSR_TDRE) && 
      (control & CYGARC_REG_IMM_SCCxR1_TIE))
    return hal_call_isr(CYGNUM_HAL_INTERRUPT_IMB3_SCI1_TX);
// Don't waist time on unused interrupts
// Transmit complete interrupt enabled (not used)
//  if((status & CYGARC_REG_IMM_SCxSR_TC) && (control & CYGARC_REG_IMM_SCCxR1_TCIE))
//    return hal_call_isr(CYGNUM_HAL_INTERRUPT_IMB3_SCI1_TXC);
// Don't waist time on unused interrupts
// Idle-line interrupt enabled (not used)
//  if((status & CYGARC_REG_IMM_SCxSR_IDLE) && (control & CYGARC_REG_IMM_SCCxR1_ILIE))
//    return hal_call_isr(CYGNUM_HAL_INTERRUPT_IMB3_SCI1_IDLE);
#endif // HW_QUEUE
#endif // SERIAL_A

  HAL_READ_UINT16(CYGARC_REG_IMM_SC2SR, status);
  HAL_READ_UINT16(CYGARC_REG_IMM_SCC2R1, control);
  if((status & CYGARC_REG_IMM_SCxSR_RDRF) && 
      (control & CYGARC_REG_IMM_SCCxR1_RIE))
    return hal_call_isr(CYGNUM_HAL_INTERRUPT_IMB3_SCI2_RX);
// Do not waist time on unused hardware
#ifdef CYGPKG_IO_SERIAL_POWERPC_MPC555_SERIAL_B
  if((status & CYGARC_REG_IMM_SCxSR_TDRE) && 
      (control & CYGARC_REG_IMM_SCCxR1_TIE))
    return hal_call_isr(CYGNUM_HAL_INTERRUPT_IMB3_SCI2_TX);
// Don't waist time on unused interrupts
// Transmit complete interrupt enabled (not used)
//  if((status & CYGARC_REG_IMM_SCxSR_TC) && (control & CYGARC_REG_IMM_SCCxR1_TCIE))
//    return hal_call_isr(CYGNUM_HAL_INTERRUPT_IMB3_SCI2_TXC);
// Don't waist time on unused interrupts
// Idle-line interrupt enabled (not used)
//  if((status & CYGARC_REG_IMM_SCxSR_IDLE) && (control & CYGARC_REG_IMM_SCCxR1_ILIE))
//    return hal_call_isr(CYGNUM_HAL_INTERRUPT_IMB3_SCI2_IDLE);
#endif

  return 0;
}

//------------------------------------------------------------------------------
// Internal function to configure the hardware to desired baud rate, etc.
//------------------------------------------------------------------------------
static bool mpc555_serial_config_port(serial_channel * chan, 
                                      cyg_serial_info_t * new_config, 
                                      bool init)
{
  mpc555_serial_info * mpc555_chan = (mpc555_serial_info *)(chan->dev_priv);

  cyg_addrword_t port = mpc555_chan->base;
  cyg_uint16 baud_rate = select_baud[new_config->baud];
  unsigned char frame_length = 1; // The start bit

  cyg_uint16 old_isrstate;
  cyg_uint16 sccxr;

  if(!baud_rate)
    return false;    // Invalid baud rate selected

  if((new_config->word_length != CYGNUM_SERIAL_WORD_LENGTH_7) &&
     (new_config->word_length != CYGNUM_SERIAL_WORD_LENGTH_8))
    return false;    // Invalid word length selected

  if((new_config->parity != CYGNUM_SERIAL_PARITY_NONE) &&
     (new_config->parity != CYGNUM_SERIAL_PARITY_EVEN) &&
     (new_config->parity != CYGNUM_SERIAL_PARITY_ODD))
    return false;    // Invalid parity selected

  if((new_config->stop != CYGNUM_SERIAL_STOP_1) &&
     (new_config->stop != CYGNUM_SERIAL_STOP_2))
    return false;    // Invalid stop bits selected

  frame_length += select_word_length[new_config->word_length - 
                                     CYGNUM_SERIAL_WORD_LENGTH_5]; 
  frame_length += select_stop_bits[new_config->stop];
  frame_length += select_parity[new_config->parity];

  if((frame_length != 10) && (frame_length != 11))
    return false;    // Invalid frame format selected

  // Disable port interrupts while changing hardware
  HAL_READ_UINT16(port + MPC555_SERIAL_SCCxR1, sccxr);
  old_isrstate = sccxr;
  old_isrstate &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_LOOPS);
  old_isrstate &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_WOMS);
  old_isrstate &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_ILT);
  old_isrstate &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_PT);
  old_isrstate &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_PE);
  old_isrstate &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_M);
  old_isrstate &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_WAKE);
  old_isrstate &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_TE);
  old_isrstate &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_RE);
  old_isrstate &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_RWU);
  old_isrstate &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_SBK);
  sccxr &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_TIE);
  sccxr &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_TCIE);
  sccxr &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_RIE);
  sccxr &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_ILIE);
  HAL_WRITE_UINT16(port + MPC555_SERIAL_SCCxR1, sccxr);

#ifdef CYGDAT_IO_SERIAL_POWERPC_MPC555_SERIAL_A_USE_HWARE_QUEUE
  cyg_uint16 qsci1cr = 0;
  if(mpc555_chan->use_queue){
    HAL_READ_UINT16( CYGARC_REG_IMM_QSCI1SR, qsci1cr);
    // disable queue
    qsci1cr &= ~((cyg_uint16)CYGARC_REG_IMM_QSCI1CR_QTE);
    qsci1cr &= ~((cyg_uint16)CYGARC_REG_IMM_QSCI1CR_QRE);
    // disable queue interrupts    
    qsci1cr &= ~((cyg_uint16)CYGARC_REG_IMM_QSCI1CR_QTHFI);
    qsci1cr &= ~((cyg_uint16)CYGARC_REG_IMM_QSCI1CR_QBHFI);
    qsci1cr &= ~((cyg_uint16)CYGARC_REG_IMM_QSCI1CR_QTHEI);
    qsci1cr &= ~((cyg_uint16)CYGARC_REG_IMM_QSCI1CR_QBHEI);
    HAL_WRITE_UINT16( CYGARC_REG_IMM_QSCI1SR, qsci1cr);
  }
#endif
  // Set databits, stopbits and parity.
  HAL_READ_UINT16(port + MPC555_SERIAL_SCCxR1, sccxr);

  if(frame_length == 11)
    sccxr |= (cyg_uint16)MPC555_SERIAL_SCCxR1_M;
  else
    sccxr &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_M);

  switch(new_config->parity){
    case CYGNUM_SERIAL_PARITY_NONE:
      sccxr &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_PE);
      break;
    case CYGNUM_SERIAL_PARITY_EVEN:
      sccxr |= (cyg_uint16)MPC555_SERIAL_SCCxR1_PE;
      sccxr &= ~((cyg_uint16)MPC555_SERIAL_SCCxR1_PT);
      break;
    case CYGNUM_SERIAL_PARITY_ODD:
      sccxr |= (cyg_uint16)MPC555_SERIAL_SCCxR1_PE;
      sccxr |= (cyg_uint16)MPC555_SERIAL_SCCxR1_PT;
      break;
    default:
      break;
  }
  HAL_WRITE_UINT16(port + MPC555_SERIAL_SCCxR1, sccxr);

  // Set baud rate.
  baud_rate &= ~((cyg_uint16)MPC555_SERIAL_SCCxR0_OTHR);
  baud_rate &= ~((cyg_uint16)MPC555_SERIAL_SCCxR0_LINKBD);
  HAL_READ_UINT16(port + MPC555_SERIAL_SCCxR0, sccxr);
  sccxr &= ~(MPC555_SERIAL_SCCxR0_SCxBR);
  sccxr |= baud_rate;
  HAL_WRITE_UINT16(port + MPC555_SERIAL_SCCxR0, sccxr);

  // Enable the device
  HAL_READ_UINT16(port + MPC555_SERIAL_SCCxR1, sccxr);
  sccxr |= MPC555_SERIAL_SCCxR1_TE;
  sccxr |= MPC555_SERIAL_SCCxR1_RE;
  HAL_WRITE_UINT16(port + MPC555_SERIAL_SCCxR1, sccxr);

  if(init){ 
#ifdef CYGDAT_IO_SERIAL_POWERPC_MPC555_SERIAL_A_USE_HWARE_QUEUE
    HAL_READ_UINT16(port + MPC555_SERIAL_SCCxR1, sccxr);
    if(mpc555_chan->use_queue){
      cyg_uint16 qsci1sr;
      // enable read queue
      qsci1cr |= ((cyg_uint16)CYGARC_REG_IMM_QSCI1CR_QRE);
      // enable receive queue interrupts    
      qsci1cr |= ((cyg_uint16)CYGARC_REG_IMM_QSCI1CR_QTHFI);
      qsci1cr |= ((cyg_uint16)CYGARC_REG_IMM_QSCI1CR_QBHFI);
      HAL_WRITE_UINT16( CYGARC_REG_IMM_QSCI1CR, qsci1cr);
      // also enable idle line detect interrupt
      sccxr |= MPC555_SERIAL_SCxSR_IDLE;
      HAL_READ_UINT16( CYGARC_REG_IMM_QSCI1SR, qsci1sr);
      qsci1sr &= ~((cyg_uint16)CYGARC_REG_IMM_QSCI1SR_QBHF);
      qsci1sr &= ~((cyg_uint16)CYGARC_REG_IMM_QSCI1SR_QTHF);
      HAL_WRITE_UINT16(CYGARC_REG_IMM_QSCI1SR, qsci1sr);
    }
    else {
      // enable the receiver interrupt
      sccxr |= MPC555_SERIAL_SCCxR1_RIE;
    }
    HAL_WRITE_UINT16(port + MPC555_SERIAL_SCCxR1, sccxr);
#else
    // enable the receiver interrupt
    HAL_READ_UINT16(port + MPC555_SERIAL_SCCxR1, sccxr);
    sccxr |= MPC555_SERIAL_SCCxR1_RIE;
    HAL_WRITE_UINT16(port + MPC555_SERIAL_SCCxR1, sccxr);
#endif
  } 
  else {// Restore the old interrupt state
  
    HAL_READ_UINT16(port + MPC555_SERIAL_SCCxR1, sccxr);
    sccxr |= old_isrstate;
    HAL_WRITE_UINT16(port + MPC555_SERIAL_SCCxR1, sccxr);
  }

  if(new_config != &chan->config) 
    chan->config = *new_config;

  return true;
}

//------------------------------------------------------------------------------
// Function to initialize the device.  Called at bootstrap time.
//------------------------------------------------------------------------------
static hal_mpc5xx_arbitration_data arbiter;
static bool mpc555_serial_init(struct cyg_devtab_entry * tab){
   serial_channel * chan = (serial_channel *)tab->priv;
   mpc555_serial_info * mpc555_chan = (mpc555_serial_info *)chan->dev_priv;

   if(!mpc555_serial_config_port(chan, &chan->config, true))
     return false;

   // Really only required for interrupt driven devices
   (chan->callbacks->serial_init)(chan);
   if(chan->out_cbuf.len != 0){ 
     arbiter.priority = CYGNUM_HAL_ISR_SOURCE_PRIORITY_QSCI;
     arbiter.data     = 0;
     arbiter.arbiter  = hal_arbitration_isr_qsci;
     
     // Install the arbitration isr, Make sure that is is not installed twice
     hal_mpc5xx_remove_arbitration_isr(CYGNUM_HAL_ISR_SOURCE_PRIORITY_QSCI);
     hal_mpc5xx_install_arbitration_isr(&arbiter); 

     // if !(Chan_B && using queue) 
     if(!mpc555_chan->use_queue){
       mpc555_chan->rx_circbuf->fill_pos = 0;
       mpc555_chan->rx_circbuf->read_pos = 0;
     
       // Create the Tx interrupt, do not enable it yet
       cyg_drv_interrupt_create(mpc555_chan->tx_interrupt_num,
                                mpc555_chan->tx_interrupt_priority,
                                (cyg_addrword_t)chan,//Data item passed to isr
                                mpc555_serial_tx_ISR,
                                mpc555_serial_tx_DSR,
                                &mpc555_chan->tx_interrupt_handle,
                                &mpc555_chan->tx_interrupt);
       cyg_drv_interrupt_attach(mpc555_chan->tx_interrupt_handle);
  
       // Create the Rx interrupt, this can be safely unmasked now
       cyg_drv_interrupt_create(mpc555_chan->rx_interrupt_num,
                                mpc555_chan->rx_interrupt_priority,
                                (cyg_addrword_t)chan,
                                mpc555_serial_rx_ISR,
                                mpc555_serial_rx_DSR,
                                &mpc555_chan->rx_interrupt_handle,
                                &mpc555_chan->rx_interrupt);
       cyg_drv_interrupt_attach(mpc555_chan->rx_interrupt_handle);
       cyg_drv_interrupt_unmask(mpc555_chan->rx_interrupt_num);
     }
#ifdef CYGDAT_IO_SERIAL_POWERPC_MPC555_SERIAL_A_USE_HWARE_QUEUE
     else {// Use HW queue
       // Create the Tx interrupt, do not enable it yet
       cyg_drv_interrupt_create(mpc555_chan->tx_interrupt_queue_top_empty_num,
                                mpc555_chan->tx_interrupt_queue_top_empty_priority,
                                (cyg_addrword_t)chan,//Data item passed to isr
                                mpc555_serial_tx_queue_top_ISR,
                                mpc555_serial_tx_queue_DSR,
                                &mpc555_chan->tx_queue_top_interrupt_handle,
                                &mpc555_chan->tx_queue_top_interrupt);
       cyg_drv_interrupt_attach(mpc555_chan->tx_queue_top_interrupt_handle);


       cyg_drv_interrupt_create(mpc555_chan->tx_interrupt_queue_bot_empty_num,
                                mpc555_chan->tx_interrupt_queue_bot_empty_priority,
                                (cyg_addrword_t)chan,//Data passed to isr
                                mpc555_serial_tx_queue_bot_ISR,
                                mpc555_serial_tx_queue_DSR,
                                &mpc555_chan->tx_queue_bot_interrupt_handle,
                                &mpc555_chan->tx_queue_bot_interrupt);
       cyg_drv_interrupt_attach(mpc555_chan->tx_queue_bot_interrupt_handle);
       
       // Rx queue interrupts
       cyg_drv_interrupt_create(mpc555_chan->rx_interrupt_queue_top_full_num,
                                mpc555_chan->rx_interrupt_queue_top_full_priority,
                                (cyg_addrword_t)chan,//Data item passed to isr
                                mpc555_serial_rx_queue_top_ISR,
                                mpc555_serial_rx_queue_DSR,
                                &mpc555_chan->rx_queue_top_interrupt_handle,
                                &mpc555_chan->rx_queue_top_interrupt);
       cyg_drv_interrupt_attach(mpc555_chan->rx_queue_top_interrupt_handle);
       
       cyg_drv_interrupt_create(mpc555_chan->rx_interrupt_queue_bot_full_num,
                                mpc555_chan->rx_interrupt_queue_bot_full_priority,
                                (cyg_addrword_t)chan,//Data item passed to isr
                                mpc555_serial_rx_queue_bot_ISR,
                                mpc555_serial_rx_queue_DSR,
                                &mpc555_chan->rx_queue_bot_interrupt_handle,
                                &mpc555_chan->rx_queue_bot_interrupt);
       cyg_drv_interrupt_attach(mpc555_chan->rx_queue_bot_interrupt_handle);
       
       cyg_drv_interrupt_create(mpc555_chan->rx_interrupt_idle_line_num,
                                mpc555_chan->rx_interrupt_idle_line_priority,
                                (cyg_addrword_t)chan,//Data item passed to isr
                                mpc555_serial_rx_idle_line_ISR,
                                mpc555_serial_rx_queue_DSR,
                                &mpc555_chan->rx_idle_interrupt_handle,
                                &mpc555_chan->rx_idle_interrupt);
       cyg_drv_interrupt_attach(mpc555_chan->rx_idle_interrupt_handle);
     }
#endif // use queue
   }
    return true;
}

//----------------------------------------------------------------------------
// This routine is called when the device is "looked" up (i.e. attached)
//----------------------------------------------------------------------------
static Cyg_ErrNo mpc555_serial_lookup(struct cyg_devtab_entry ** tab, 
                                      struct cyg_devtab_entry * sub_tab,
                                      const char * name)
{
  serial_channel * chan = (serial_channel *)(*tab)->priv;
  //Really only required for interrupt driven devices
  (chan->callbacks->serial_init)(chan);

  return ENOERR;
}

//----------------------------------------------------------------------------
// Send a character to the device output buffer.
// Return 'true' if character is sent to device
//----------------------------------------------------------------------------
static bool mpc555_serial_putc(serial_channel * chan, unsigned char c){
  mpc555_serial_info * mpc555_chan = (mpc555_serial_info *)chan->dev_priv;
  cyg_addrword_t port = mpc555_chan->base;

  cyg_uint16 scsr;
  cyg_uint16 scdr;

  HAL_READ_UINT16(port + MPC555_SERIAL_SCxSR, scsr);
  if(scsr & MPC555_SERIAL_SCxSR_TDRE){ 
    // Ok, we have space, write the character and return success
    scdr = (cyg_uint16)c;
    HAL_WRITE_UINT16(port + MPC555_SERIAL_SCxDR, scdr);
    return true;
  }  
  else
    // We cannot write to the transmitter, return failure
    return false;
}

//----------------------------------------------------------------------------
// Fetch a character from the device input buffer, waiting if necessary
//----------------------------------------------------------------------------
static unsigned char mpc555_serial_getc(serial_channel * chan){
  unsigned char c;
  mpc555_serial_info * mpc555_chan = (mpc555_serial_info *)chan->dev_priv;
  cyg_addrword_t port = mpc555_chan->base;

  cyg_uint16 scsr;
  cyg_uint16 scdr;

  do {
    HAL_READ_UINT16(port + MPC555_SERIAL_SCxSR, scsr);
  } while(!(scsr & MPC555_SERIAL_SCxSR_RDRF));

  // Ok, data is received, read it out and return
  HAL_READ_UINT16(port + MPC555_SERIAL_SCxDR, scdr);
  c = (unsigned char)scdr;

  return c;
}

//----------------------------------------------------------------------------
// Set up the device characteristics; baud rate, etc.
//----------------------------------------------------------------------------
static bool mpc555_serial_set_config(serial_channel * chan, cyg_uint32 key,
                                     const void *xbuf, cyg_uint32 * len)
{
  switch(key){
    case CYG_IO_SET_CONFIG_SERIAL_INFO:{
      cyg_serial_info_t *config = (cyg_serial_info_t *)xbuf;
      if(*len < sizeof(cyg_serial_info_t)){
        return -EINVAL;
      }
      *len = sizeof(cyg_serial_info_t);
      if(true != mpc555_serial_config_port(chan, config, false))
        return -EINVAL;
      }
    break;
  default:
    return -EINVAL;
  }
  return ENOERR;
}

//------------------------------------------------------------------------------
// Enable the transmitter on the device
//------------------------------------------------------------------------------
static void mpc555_serial_start_xmit(serial_channel * chan)
{
  mpc555_serial_info * mpc555_chan = (mpc555_serial_info *)chan->dev_priv;
#ifdef CYGDAT_IO_SERIAL_POWERPC_MPC555_SERIAL_A_USE_HWARE_QUEUE
  cyg_addrword_t port = mpc555_chan->base;
  if(mpc555_chan->use_queue){
    cyg_uint16 qscicr;
    cyg_uint16 qscisr;
    cyg_uint16 scsr;
    
    int chars_avail;
    unsigned char* chars;
    int block_index = 0;
    cyg_addrword_t i;
    cyg_uint16 queue_transfer;
 
    if(!(mpc555_chan->tx_interrupt_enable) && 
       (chan->callbacks->data_xmt_req)(chan, 32, &chars_avail, &chars) 
       == CYG_XMT_OK){
      queue_transfer = (chars_avail > 16) ? 16 : chars_avail;

      HAL_READ_UINT16( CYGARC_REG_IMM_QSCI1CR, qscicr);
      // Write QTSZ for first pass through the queue
      qscicr &= ~(CYGARC_REG_IMM_QSCI1CR_QTSZ);
      qscicr |= (CYGARC_REG_IMM_QSCI1CR_QTSZ & (queue_transfer - 1));
      HAL_WRITE_UINT16( CYGARC_REG_IMM_QSCI1CR, qscicr);
      // Read SC1SR to clear TC bit when followed by a write of sctq
      HAL_READ_UINT16(port + MPC555_SERIAL_SCxSR, scsr);
      
      for(i=0; i < queue_transfer; i++){
        HAL_WRITE_UINT16(CYGARC_REG_IMM_SCTQ + (i * 2), chars[block_index]);
        ++block_index;
      }
      chan->callbacks->data_xmt_done(chan, queue_transfer);

      // clear QTHE and QBHE
      HAL_READ_UINT16(CYGARC_REG_IMM_QSCI1SR, qscisr);

      qscisr &= ~(CYGARC_REG_IMM_QSCI1SR_QTHE);
      HAL_WRITE_UINT16(CYGARC_REG_IMM_QSCI1SR, qscisr);
      if(queue_transfer > 8){
        qscisr &= ~(CYGARC_REG_IMM_QSCI1SR_QBHE);
        HAL_WRITE_UINT16(CYGARC_REG_IMM_QSCI1SR, qscisr);
      }

      mpc555_chan->tx_interrupt_enable = true;

      cyg_drv_interrupt_unmask(mpc555_chan->tx_interrupt_queue_top_empty_num);
      if(queue_transfer > 8){
        cyg_drv_interrupt_unmask(mpc555_chan->tx_interrupt_queue_bot_empty_num);
      }

      HAL_READ_UINT16( CYGARC_REG_IMM_QSCI1CR, qscicr);
      qscicr |= ((cyg_uint16)CYGARC_REG_IMM_QSCI1CR_QTE);
      HAL_WRITE_UINT16( CYGARC_REG_IMM_QSCI1CR, qscicr);
    }
  }
  else { // no queue  
   mpc555_chan->tx_interrupt_enable = true;
   cyg_drv_interrupt_unmask(mpc555_chan->tx_interrupt_num);
// No need to call xmt_char, this will generate an interrupt immediately.
  }
#else // No queue
   mpc555_chan->tx_interrupt_enable = true;
   cyg_drv_interrupt_unmask(mpc555_chan->tx_interrupt_num);
// No need to call xmt_char, this will generate an interrupt immediately.
#endif
}

//----------------------------------------------------------------------------
// Disable the transmitter on the device
//----------------------------------------------------------------------------
static void mpc555_serial_stop_xmit(serial_channel * chan){
  mpc555_serial_info * mpc555_chan = (mpc555_serial_info *)chan->dev_priv;

  if(!mpc555_chan->use_queue){
    cyg_drv_dsr_lock();
    mpc555_chan->tx_interrupt_enable = false;
    cyg_drv_interrupt_mask(mpc555_chan->tx_interrupt_num);
    cyg_drv_dsr_unlock();
  }
}

//----------------------------------------------------------------------------
// The low level transmit interrupt handler
//----------------------------------------------------------------------------
static cyg_uint32 mpc555_serial_tx_ISR(cyg_vector_t vector,
                                       cyg_addrword_t data){
  serial_channel * chan = (serial_channel *)data;
  mpc555_serial_info * mpc555_chan = (mpc555_serial_info *)chan->dev_priv;

  cyg_drv_interrupt_mask(mpc555_chan->tx_interrupt_num);
  cyg_drv_interrupt_acknowledge(mpc555_chan->tx_interrupt_num);

  return CYG_ISR_CALL_DSR; // cause the DSR to run
}

//----------------------------------------------------------------------------
// The low level receive interrupt handler
//----------------------------------------------------------------------------
static cyg_uint32 mpc555_serial_rx_ISR(cyg_vector_t vector, 
                                       cyg_addrword_t data){
  serial_channel * chan = (serial_channel *)data;
  mpc555_serial_info * mpc555_chan = (mpc555_serial_info *)chan->dev_priv;

  cyg_drv_interrupt_mask(mpc555_chan->rx_interrupt_num);
  cyg_drv_interrupt_acknowledge(mpc555_chan->rx_interrupt_num);

  cyg_addrword_t port = mpc555_chan->base;
  cyg_uint16 scdr;
  cyg_uint16 scsr;

  HAL_READ_UINT16(port + MPC555_SERIAL_SCxSR, scsr);
  // Always read out the received character, in order to clear receiver flags
  HAL_READ_UINT16(port + MPC555_SERIAL_SCxDR, scdr);
  
  mpc555_chan->rx_circbuf->scsr[mpc555_chan->rx_circbuf->fill_pos] = scsr;
  mpc555_chan->rx_circbuf->buf[mpc555_chan->rx_circbuf->fill_pos] = (cyg_uint8)scdr;

  if(mpc555_chan->rx_circbuf->fill_pos < MPC555_SCI_RX_BUFF_SIZE - 1){
    mpc555_chan->rx_circbuf->fill_pos = mpc555_chan->rx_circbuf->fill_pos + 1;
  }
  else {
    mpc555_chan->rx_circbuf->fill_pos = 0;
  }
  cyg_drv_interrupt_unmask(mpc555_chan->rx_interrupt_num);
  return CYG_ISR_CALL_DSR; // cause the DSR to run
}

#ifdef CYGPKG_IO_SERIAL_POWERPC_MPC555_SERIAL_A
//----------------------------------------------------------------------------
// The low level queued receive interrupt handlers
//----------------------------------------------------------------------------
static cyg_uint32 mpc555_serial_rx_queue_top_ISR(cyg_vector_t vector, 
                                                 cyg_addrword_t data){
  serial_channel * chan = (serial_channel *)data;
  mpc555_serial_info * mpc555_chan = (mpc555_serial_info *)chan->dev_priv;

  cyg_drv_interrupt_mask(mpc555_chan->rx_interrupt_queue_top_full_num);
  cyg_drv_interrupt_acknowledge(mpc555_chan->rx_interrupt_queue_top_full_num);

  return CYG_ISR_CALL_DSR; // cause the DSR to run
}

static cyg_uint32 mpc555_serial_rx_queue_bot_ISR(cyg_vector_t vector,
                                                 cyg_addrword_t data){
  serial_channel* chan = (serial_channel *)data;
  mpc555_serial_info* mpc555_chan = (mpc555_serial_info *)chan->dev_priv;

  cyg_drv_interrupt_mask(mpc555_chan->rx_interrupt_queue_bot_full_num);
  cyg_drv_interrupt_acknowledge(mpc555_chan->rx_interrupt_queue_bot_full_num);

  return CYG_ISR_CALL_DSR; // cause the DSR to run
}

// This is used to flush the queue when the line falls idle
static cyg_uint32 mpc555_serial_rx_idle_line_ISR(cyg_vector_t vector,
                                                 cyg_addrword_t data){
  serial_channel* chan = (serial_channel *)data;
  mpc555_serial_info* mpc555_chan = (mpc555_serial_info *)chan->dev_priv;

  cyg_drv_interrupt_mask(mpc555_chan->rx_interrupt_idle_line_num);
  cyg_drv_interrupt_acknowledge(mpc555_chan->rx_interrupt_idle_line_num);

  return CYG_ISR_CALL_DSR; // cause the DSR to run
}

//----------------------------------------------------------------------------
// The low level queued transmit interrupt handlers
//----------------------------------------------------------------------------
static cyg_uint32 mpc555_serial_tx_queue_top_ISR(cyg_vector_t vector,
                                                 cyg_addrword_t data){
  serial_channel * chan = (serial_channel *)data;
  mpc555_serial_info * mpc555_chan = (mpc555_serial_info *)chan->dev_priv;

  cyg_drv_interrupt_mask(mpc555_chan->tx_interrupt_queue_top_empty_num);
  cyg_drv_interrupt_acknowledge(mpc555_chan->tx_interrupt_queue_top_empty_num);

  return CYG_ISR_CALL_DSR; // cause the DSR to run
}

static cyg_uint32 mpc555_serial_tx_queue_bot_ISR(cyg_vector_t vector,
                                                 cyg_addrword_t data){
  serial_channel * chan = (serial_channel *)data;
  mpc555_serial_info * mpc555_chan = (mpc555_serial_info *)chan->dev_priv;

  cyg_drv_interrupt_mask(mpc555_chan->tx_interrupt_queue_bot_empty_num);
  cyg_drv_interrupt_acknowledge(mpc555_chan->tx_interrupt_queue_bot_empty_num);

  return CYG_ISR_CALL_DSR; // cause the DSR to run
}
#endif // SERIAL_A

//----------------------------------------------------------------------------
// The high level transmit interrupt handler
//----------------------------------------------------------------------------
static void mpc555_serial_tx_DSR(cyg_vector_t vector, cyg_ucount32 count,
                                 cyg_addrword_t data){
  serial_channel * chan = (serial_channel *)data;
  mpc555_serial_info * mpc555_chan = (mpc555_serial_info *)chan->dev_priv;

  (chan->callbacks->xmt_char)(chan);
  if(mpc555_chan->tx_interrupt_enable)
    cyg_drv_interrupt_unmask(mpc555_chan->tx_interrupt_num);
}

//----------------------------------------------------------------------------
// The high level receive interrupt handler
//----------------------------------------------------------------------------
#define MPC555_SERIAL_SCxSR_ERRORS (MPC555_SERIAL_SCxSR_OR | \
                                    MPC555_SERIAL_SCxSR_NF | \
                                    MPC555_SERIAL_SCxSR_FE | \
                                    MPC555_SERIAL_SCxSR_PF)

static void mpc555_serial_rx_DSR(cyg_vector_t vector, cyg_ucount32 count, 
                                 cyg_addrword_t data){
  serial_channel * chan = (serial_channel *)data;
  mpc555_serial_info * mpc555_chan = (mpc555_serial_info *)chan->dev_priv;
//  cyg_addrword_t port = mpc555_chan->base;
//  cyg_uint16 scdr;
  cyg_uint16 scsr;

#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
  cyg_serial_line_status_t stat;
#endif


  int i = mpc555_chan->rx_circbuf->read_pos;
  while (i < mpc555_chan->rx_circbuf->fill_pos){
     scsr = mpc555_chan->rx_circbuf->scsr[i];
     if(scsr & (cyg_uint16)MPC555_SERIAL_SCxSR_ERRORS){
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
       if(scsr & MPC555_SERIAL_SCxSR_OR){
         stat.which = CYGNUM_SERIAL_STATUS_OVERRUNERR;
         (chan->callbacks->indicate_status)(chan, &stat);
         // The current byte is still valid when OR is set
         (chan->callbacks->rcv_char)(chan, mpc555_chan->rx_circbuf->buf[i]);
       } 
       else { // OR is never set with any other error bits
         if(scsr & MPC555_SERIAL_SCxSR_NF){
           stat.which = CYGNUM_SERIAL_STATUS_NOISEERR;
           (chan->callbacks->indicate_status)(chan, &stat);
         } 
         if(scsr & MPC555_SERIAL_SCxSR_FE){
           stat.which = CYGNUM_SERIAL_STATUS_FRAMEERR;
           (chan->callbacks->indicate_status)(chan, &stat);
         }   
         if(scsr & MPC555_SERIAL_SCxSR_PF){
           stat.which = CYGNUM_SERIAL_STATUS_PARITYERR;
           (chan->callbacks->indicate_status)(chan, &stat);
         }
       }
#endif
     } 
     else {
       (chan->callbacks->rcv_char)(chan, mpc555_chan->rx_circbuf->buf[i]);
     }
     ++i;
  } 

    cyg_drv_isr_lock();
    mpc555_chan->rx_circbuf->fill_pos = 0;
    mpc555_chan->rx_circbuf->read_pos = 0;
    cyg_drv_isr_unlock();
}

#ifdef CYGPKG_IO_SERIAL_POWERPC_MPC555_SERIAL_A
//----------------------------------------------------------------------------
// The high level queued transmit interrupt handler
//----------------------------------------------------------------------------
static void mpc555_serial_tx_queue_DSR(cyg_vector_t vector, cyg_ucount32 count, 
                                       cyg_addrword_t data){
  serial_channel * chan = (serial_channel *)data;
  mpc555_serial_info * mpc555_chan = (mpc555_serial_info *)chan->dev_priv;
  bool QTHE = false;
  bool QBHE = false;
  cyg_uint16 qscisr;
  cyg_uint16 qscicr;
  HAL_READ_UINT16(CYGARC_REG_IMM_QSCI1SR, qscisr);
  QTHE = (qscisr & CYGARC_REG_IMM_QSCI1SR_QTHE) ? true : false;
  QBHE = (qscisr & CYGARC_REG_IMM_QSCI1SR_QBHE) ? true : false;
  
  CYG_ASSERT(QTHE || QBHE,"In tx queue DSR for no reason");

  HAL_READ_UINT16(CYGARC_REG_IMM_QSCI1CR, qscicr);
  int chars_avail;
  unsigned char* chars;
  int block_index = 0;
  cyg_addrword_t i;
  cyg_uint16 queue_transfer;
  xmt_req_reply_t result = (chan->callbacks->data_xmt_req)(chan, 24, &chars_avail, &chars);
  if(CYG_XMT_OK == result){
    queue_transfer = (chars_avail > 8) ? 8 : chars_avail;
    if(QTHE){
      for(i=0; i < queue_transfer; i++){
        HAL_WRITE_UINT16(CYGARC_REG_IMM_SCTQ + (i * 2), chars[block_index]);
        ++block_index;
      }
      chan->callbacks->data_xmt_done(chan, queue_transfer);
      // Clear QTHE
      qscisr &= ~(CYGARC_REG_IMM_QSCI1SR_QTHE);
      HAL_WRITE_UINT16(CYGARC_REG_IMM_QSCI1SR, qscisr);
      
      // Re-enable wrap QTWE
      HAL_READ_UINT16(CYGARC_REG_IMM_QSCI1CR, qscicr);
      qscicr |= ((cyg_uint16)CYGARC_REG_IMM_QSCI1CR_QTWE);
      HAL_WRITE_UINT16(CYGARC_REG_IMM_QSCI1CR, qscicr);
      HAL_READ_UINT16(CYGARC_REG_IMM_QSCI1CR, qscicr);
      // load QTSZ with how many chars *after* the next wrap
      cyg_uint16 next_time = (chars_avail) > 16 ? 15 : chars_avail -1; 
      qscicr &= ~(CYGARC_REG_IMM_QSCI1CR_QTSZ);
      qscicr |= (CYGARC_REG_IMM_QSCI1CR_QTSZ & next_time);
      HAL_WRITE_UINT16(CYGARC_REG_IMM_QSCI1CR, qscicr);
      cyg_drv_interrupt_unmask(mpc555_chan->tx_interrupt_queue_top_empty_num);
    }
    else if(QBHE){
      for(i=8; i < queue_transfer + 8; i++){
        HAL_WRITE_UINT16(CYGARC_REG_IMM_SCTQ + (i * 2), chars[block_index]);
        ++block_index;
      }
      chan->callbacks->data_xmt_done(chan, queue_transfer);
      // Clear QBHE
      qscisr &= ~(CYGARC_REG_IMM_QSCI1SR_QBHE);
      HAL_WRITE_UINT16(CYGARC_REG_IMM_QSCI1SR, qscisr);
      cyg_drv_interrupt_unmask(mpc555_chan->tx_interrupt_queue_bot_empty_num);
    }

  }   
  else if(CYG_XMT_EMPTY== result){
    // No more data
    cyg_drv_interrupt_mask(mpc555_chan->tx_interrupt_queue_top_empty_num);
    cyg_drv_interrupt_mask(mpc555_chan->tx_interrupt_queue_bot_empty_num);
    mpc555_chan->tx_interrupt_enable = false;

    // Clear QTHE
    HAL_READ_UINT16(CYGARC_REG_IMM_QSCI1SR, qscisr);
    qscisr &= ~(CYGARC_REG_IMM_QSCI1SR_QTHE);
    HAL_WRITE_UINT16(CYGARC_REG_IMM_QSCI1SR, qscisr);
  }
}

//----------------------------------------------------------------------------
// The high level queued receive interrupt handler
//----------------------------------------------------------------------------
static void mpc555_serial_rx_queue_DSR(cyg_vector_t vector, 
                                       cyg_ucount32 count, cyg_addrword_t data){
  serial_channel * chan = (serial_channel *)data;
  mpc555_serial_info * mpc555_chan = (mpc555_serial_info *)chan->dev_priv;
  cyg_addrword_t port = mpc555_chan->base;
  cyg_uint16 scrq;
  cyg_uint16 qscisr;
  cyg_uint16 scsr;
  cyg_uint16 scdr;
  bool QTHF = false;
  bool QBHF = false;
  bool idle = false;
  // Read status reg before reading any data otherwise NE flag will be lost
  HAL_READ_UINT16(port + MPC555_SERIAL_SCxSR, scsr);
  HAL_READ_UINT16(CYGARC_REG_IMM_QSCI1SR, qscisr);
  QTHF = (qscisr & CYGARC_REG_IMM_QSCI1SR_QTHF) ? true : false;
  QBHF = (qscisr & CYGARC_REG_IMM_QSCI1SR_QBHF) ? true : false;
  idle = (scsr & CYGARC_REG_IMM_SCxSR_IDLE)? true : false;
  // The queue pointer is the next place to be filled by incomming data
  cyg_uint16 queue_pointer = (qscisr & CYGARC_REG_IMM_QSCI1SR_QRPNT) >> 4;

  int start;
  int space_req = 0;
  // Idle needs to be handled first as the IDLE bit will be cleared by a read of
  // scsr followed by a read of scrq[0:16]

  if(queue_pointer > mpc555_chan->rx_last_queue_pointer){
    start = mpc555_chan->rx_last_queue_pointer;
    space_req = mpc555_serial_read_queue(chan, start, queue_pointer - 1);
  }
  else {// Its wrapped around
    if(mpc555_chan->rx_last_queue_pointer > queue_pointer){
      space_req = mpc555_serial_read_queue(chan, mpc555_chan->rx_last_queue_pointer,15);
      if(queue_pointer != 0){
        mpc555_serial_read_queue(chan, 0,queue_pointer -1); 
      }
    }
    else // No new data to read, do nothing here
    {
    }
  }

  mpc555_chan->rx_last_queue_pointer = queue_pointer;
    
  if(CYGARC_REG_IMM_QSCI1SR_QOR & qscisr){
    // Need to re-enable the queue
    cyg_uint16 qscicr;
    HAL_READ_UINT16( CYGARC_REG_IMM_QSCI1CR, qscicr);
    qscicr |= ((cyg_uint16)CYGARC_REG_IMM_QSCI1CR_QRE);
    HAL_WRITE_UINT16( CYGARC_REG_IMM_QSCI1CR, qscicr);
    // Queue has overrun but data might not have been lost yet
    if(scsr & MPC555_SERIAL_SCxSR_OR){  
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
      cyg_serial_line_status_t stat;
      stat.which = CYGNUM_SERIAL_STATUS_OVERRUNERR;
      (chan->callbacks->indicate_status)(chan, &stat);    
#endif
    }
  }

  if(scsr & (cyg_uint16)MPC555_SERIAL_SCxSR_ERRORS){
    // Special case for queue overrun handled above.
    // Only data without FE or PF is allowed into the queue.
    // Data with NE is allowed into the queue.
    // If FE or PF have occured then the queue is disabled
    // until they are cleared (by reading scsr then scdr).
    
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
    cyg_serial_line_status_t stat;
    if(scsr & MPC555_SERIAL_SCxSR_NF){
      // Note if there is more than one frame in the queue
      // it is not possible to tell which frame
      // in the queue caused the noise error.
      // The error has already been cleared by reading
      // srsr then scrq[n], so no action is required here.
      stat.which = CYGNUM_SERIAL_STATUS_NOISEERR;
      (chan->callbacks->indicate_status)(chan, &stat);
    } 
#endif 
    if(scsr & (MPC555_SERIAL_SCxSR_FE | MPC555_SERIAL_SCxSR_PF)){
      // This action needs to be taken clear the status bits so that
      // the queue can be re-enabled.
      HAL_READ_UINT16(port + MPC555_SERIAL_SCxDR, scdr);
      // Need to re-enable the queue
      cyg_uint16 qscicr;
      HAL_READ_UINT16(CYGARC_REG_IMM_QSCI1CR, qscicr);
      qscicr |= ((cyg_uint16)CYGARC_REG_IMM_QSCI1CR_QRE);
      HAL_WRITE_UINT16(CYGARC_REG_IMM_QSCI1CR, qscicr);
      
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
      if(scsr & MPC555_SERIAL_SCxSR_FE){
        stat.which = CYGNUM_SERIAL_STATUS_FRAMEERR;
        (chan->callbacks->indicate_status)(chan, &stat);
      }   
      if(scsr & MPC555_SERIAL_SCxSR_PF){
        stat.which = CYGNUM_SERIAL_STATUS_PARITYERR;
        (chan->callbacks->indicate_status)(chan, &stat);
      }
#endif
    }
  }
  if(QTHF){
    qscisr &= ~((cyg_uint16)CYGARC_REG_IMM_QSCI1SR_QTHF);
    HAL_WRITE_UINT16(CYGARC_REG_IMM_QSCI1SR, qscisr);
    //cyg_drv_interrupt_unmask(mpc555_chan->rx_interrupt_queue_top_full_num);
  }
  if(QBHF){
    qscisr &= ~((cyg_uint16)CYGARC_REG_IMM_QSCI1SR_QBHF);
    HAL_WRITE_UINT16(CYGARC_REG_IMM_QSCI1SR, qscisr);
    //cyg_drv_interrupt_unmask(mpc555_chan->rx_interrupt_queue_bot_full_num);
  }
  if(idle){
    if(idle && !space_req){
      // The IDLE flag can be set sometimes when RE is set
      // so a read of scrq is needed to clear it.
      // If this occurs there should be no new data yet otherwise the
      // condition is impossible to detect
      HAL_READ_UINT16(CYGARC_REG_IMM_SCRQ, scrq);
    }
    HAL_READ_UINT16(CYGARC_REG_IMM_SCRQ, scrq);
    //cyg_drv_interrupt_unmask(mpc555_chan->rx_interrupt_idle_line_num);
  }
  // A bit lasy, but we don't know or care what the original ISR source
  // was so to cover all bases re-enble them all
  cyg_drv_interrupt_unmask(mpc555_chan->rx_interrupt_queue_top_full_num);
  cyg_drv_interrupt_unmask(mpc555_chan->rx_interrupt_queue_bot_full_num);
  cyg_drv_interrupt_unmask(mpc555_chan->rx_interrupt_idle_line_num);
}

static int mpc555_serial_read_queue(serial_channel* chan, int start, int end)
{
  int block_index = 0;
  cyg_uint16 scrq;
  cyg_addrword_t i;
  unsigned char* space;
  int space_avail = 0;
  int space_req = end - start + 1;
  if((space_req > 0) && 
     ((chan->callbacks->data_rcv_req)
      (chan, space_req, &space_avail, &space) == CYG_RCV_OK)) {
    CYG_ASSERT((start >= 0) && (start < 16),"rx queue read start point out of range");
    CYG_ASSERT(start <= end,"rx queue read start and end points reversed");
    for(i=start ;i < (start + space_avail); i++){
      CYG_ASSERT((i >= 0) && (i < 16),"rx queue read out of range");
      HAL_READ_UINT16(CYGARC_REG_IMM_SCRQ + (i * 2), scrq);
      space[block_index] = scrq;
      ++block_index;
    } 
    (chan->callbacks->data_rcv_done)(chan,space_avail);
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS    
// If there's not enough room data will be lost.
// There's no point calling rcv_char because the reader is blocked by this DSR.
    if(space_avail < space_req){
      cyg_serial_line_status_t stat;
      stat.which = CYGNUM_SERIAL_STATUS_OVERRUNERR;
      (chan->callbacks->indicate_status)(chan, &stat); 
    }
#endif   
  }
  return space_req;
}
#endif // SERIAL_A
#endif // CYGPKG_IO_SERIAL_POWERPC_MPC555

// EOF mpc555_serial_with_ints.c

