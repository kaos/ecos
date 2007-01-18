//==========================================================================
//
//      ser_esci.c
//
//      Freescale sSCI Serial I/O Interface Module (interrupt driven)
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2996 eCosCentric Ltd
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Ilija Koco <ilijak@siva.com.mk>
// Contributors:
// Date:        2006-04-20
// Purpose:     eSCI Serial I/O module (interrupt driven version)
// Description: 
//
//   
//####DESCRIPTIONEND####
//==========================================================================
#include <pkgconf/io_serial.h>
#include <pkgconf/io.h>

#include <cyg/io/io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_arbiter.h>
#include <cyg/hal/var_io.h>
#include <cyg/io/devtab.h>
#include <cyg/infra/diag.h>
#include <cyg/io/serial.h>
#include <cyg/devs/ser_esci.h>

// Only build this driver for if ESCI is needed.
#ifdef CYGPKG_IO_SERIAL_FREESCALE_ESCI

typedef struct esci_serial_info {
    CYG_ADDRWORD   esci_base;          // Base address of the esci port
    CYG_WORD       interrupt_num;      // INTC interrupt vector
    cyg_priority_t interrupt_priority; // INTC interupt priority
    cyg_interrupt  interrupt_obj;      // Interrupt object
    cyg_handle_t   interrupt_handle;   // Interrupt handle
} esci_serial_info;

static bool esci_serial_init(struct cyg_devtab_entry * tab);
static bool esci_serial_putc(serial_channel * chan, unsigned char c);
static Cyg_ErrNo esci_serial_lookup(struct cyg_devtab_entry ** tab, 
                                    struct cyg_devtab_entry * sub_tab, 
                                    const char * name);
static unsigned char esci_serial_getc(serial_channel *chan);
static Cyg_ErrNo esci_serial_set_config(serial_channel *chan, cyg_uint32 key, 
                                        const void *xbuf, cyg_uint32 *len);
static void esci_serial_start_xmit(serial_channel *chan);
static void esci_serial_stop_xmit(serial_channel *chan);

// Interrupt servers
static cyg_uint32 esci_serial_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       esci_serial_DSR(cyg_vector_t vector, cyg_ucount32 count, 
                                  cyg_addrword_t data);

static SERIAL_FUNS(esci_serial_funs, 
                   esci_serial_putc, 
                   esci_serial_getc,
                   esci_serial_set_config,
                   esci_serial_start_xmit,
                   esci_serial_stop_xmit);

// Available baud rates
static unsigned short select_baud[] = {
    0,                            // Unused
    0,                            // 50     bps unsupported
    0,                            // 75     bps unsupported
    0,                            // 110    bps unsupported
    0,                            // 134_5  bps unsupported
    0,                            // 150    bps unsupported
    0,                            // 200    bps unsupported
    FREESCALE_ESCI_BAUD(300),     // 300    bps
    FREESCALE_ESCI_BAUD(600),     // 600    bps
    FREESCALE_ESCI_BAUD(1200),    // 1200   bps
    0,                            // 1800   bps unsupported
    FREESCALE_ESCI_BAUD(2400),    // 2400   bps
    0,                            // 3600   bps unsupported
    FREESCALE_ESCI_BAUD(4800),    // 4800   bps
    0,                            // 7200   bps unsupported
    FREESCALE_ESCI_BAUD(9600),    // 9600   bps
    FREESCALE_ESCI_BAUD(14400),   // 14400  bps
    FREESCALE_ESCI_BAUD(19200),   // 19200  bps
    FREESCALE_ESCI_BAUD(38400),   // 38400  bps
    FREESCALE_ESCI_BAUD(57600),   // 57600  bps
    FREESCALE_ESCI_BAUD(115200),  // 115200 bps
    0                             // 230400 bps unsupported
};

#if defined CYGPKG_IO_SERIAL_FREESCALE_ESCI_A
static esci_serial_info esci_serial_info0 = {
    esci_base          : CYGADDR_IO_SERIAL_FREESCALE_ESCI_A_BASE,
    interrupt_num      : CYGNUM_IO_SERIAL_FREESCALE_ESCI_A_INT_VECTOR,
    interrupt_priority : CYGNUM_IO_SERIAL_FREESCALE_ESCI_A_INT_PRIORITY
};
#if CYGNUM_IO_SERIAL_FREESCALE_ESCI_A_BUFSIZE > 0
static unsigned char 
    esci_serial_out_buf0[CYGNUM_IO_SERIAL_FREESCALE_ESCI_A_BUFSIZE]; 
static unsigned char 
    esci_serial_in_buf0[CYGNUM_IO_SERIAL_FREESCALE_ESCI_A_BUFSIZE];

static 
SERIAL_CHANNEL_USING_INTERRUPTS(
                                esci_serial_channel0,
                                esci_serial_funs,
                                esci_serial_info0,
                                CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_FREESCALE_ESCI_A_BAUD),
                                CYG_SERIAL_STOP_DEFAULT,
                                CYG_SERIAL_PARITY_DEFAULT,
                                CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                CYG_SERIAL_FLAGS_DEFAULT,
                                &esci_serial_out_buf0[0],
                                sizeof(esci_serial_out_buf0),
                                &esci_serial_in_buf0[0],
                                sizeof(esci_serial_in_buf0));
#else 
static 
SERIAL_CHANNEL(esci_serial_channel0,
               esci_serial_funs,
               esci_serial_info0,
               CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_FREESCALE_ESCI_A_BAUD),
               CYG_SERIAL_STOP_DEFAULT,
               CYG_SERIAL_PARITY_DEFAULT,
               CYG_SERIAL_WORD_LENGTH_DEFAULT,
               CYG_SERIAL_FLAGS_DEFAULT);
#endif
DEVTAB_ENTRY(esci_serial_io0,
             CYGDAT_IO_SERIAL_FREESCALE_ESCI_A_NAME,
             0, // does not depend on a lower level device driver
             &cyg_io_serial_devio,
             esci_serial_init,
             esci_serial_lookup,
             &esci_serial_channel0);
#endif // ifdef CYGPKG_IO_SERIAL_FREESCALE_ESCI_A

#if defined CYGPKG_IO_SERIAL_FREESCALE_ESCI_B
static esci_serial_info esci_serial_info1 = {
    esci_base          : CYGADDR_IO_SERIAL_FREESCALE_ESCI_B_BASE,
    interrupt_num      : CYGNUM_IO_SERIAL_FREESCALE_ESCI_B_INT_VECTOR,
    interrupt_priority : CYGNUM_IO_SERIAL_FREESCALE_ESCI_B_INT_PRIORITY
};
#if CYGNUM_IO_SERIAL_FREESCALE_ESCI_B_BUFSIZE > 0
static unsigned char 
    esci_serial_out_buf1[CYGNUM_IO_SERIAL_FREESCALE_ESCI_B_BUFSIZE]; 
static unsigned char 
    esci_serial_in_buf1[CYGNUM_IO_SERIAL_FREESCALE_ESCI_B_BUFSIZE];

static 
SERIAL_CHANNEL_USING_INTERRUPTS(esci_serial_channel1,
                                esci_serial_funs,
                                esci_serial_info1,
                                CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_FREESCALE_ESCI_B_BAUD),
                                CYG_SERIAL_STOP_DEFAULT,
                                CYG_SERIAL_PARITY_DEFAULT,
                                CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                CYG_SERIAL_FLAGS_DEFAULT,
                                &esci_serial_out_buf1[0],
                                sizeof(esci_serial_out_buf1),
                                &esci_serial_in_buf1[0],
                                sizeof(esci_serial_in_buf1));
#else 
static 
SERIAL_CHANNEL(esci_serial_channel1,
               esci_serial_funs,
               esci_serial_info1,
               CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_FREESCALE_ESCI_B_BAUD),
               CYG_SERIAL_STOP_DEFAULT,
               CYG_SERIAL_PARITY_DEFAULT,
               CYG_SERIAL_WORD_LENGTH_DEFAULT,
               CYG_SERIAL_FLAGS_DEFAULT);
#endif
DEVTAB_ENTRY(esci_serial_io1,
             CYGDAT_IO_SERIAL_FREESCALE_ESCI_B_NAME,
             0, // does not depend on a lower level device driver
             &cyg_io_serial_devio,
             esci_serial_init,
             esci_serial_lookup,
             &esci_serial_channel1);
#endif // ifdef CYGPKG_IO_SERIAL_FREESCALE_ESCI_B

#if defined CYGPKG_IO_SERIAL_FREESCALE_ESCI_C
static esci_serial_info esci_serial_info2 = {
    esci_base          : CYGADDR_IO_SERIAL_FREESCALE_ESCI_C_BASE,
    interrupt_num      : CYGNUM_IO_SERIAL_FREESCALE_ESCI_C_INT_VECTOR,
    interrupt_priority : CYGNUM_IO_SERIAL_FREESCALE_ESCI_C_INT_PRIORITY
};
#if CYGNUM_IO_SERIAL_FREESCALE_ESCI_C_BUFSIZE > 0
static unsigned char 
    esci_serial_out_buf2[CYGNUM_IO_SERIAL_FREESCALE_ESCI_C_BUFSIZE]; 
static unsigned char 
    esci_serial_in_buf2[CYGNUM_IO_SERIAL_FREESCALE_ESCI_C_BUFSIZE];

static 
SERIAL_CHANNEL_USING_INTERRUPTS(esci_serial_channel2,
                                esci_serial_funs,
                                esci_serial_info2,
                                CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_FREESCALE_ESCI_C_BAUD),
                                CYG_SERIAL_STOP_DEFAULT,
                                CYG_SERIAL_PARITY_DEFAULT,
                                CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                CYG_SERIAL_FLAGS_DEFAULT,
                                &esci_serial_out_buf2[0],
                                sizeof(esci_serial_out_buf2),
                                &esci_serial_in_buf2[0],
                                sizeof(esci_serial_in_buf2));
#else 
static 
SERIAL_CHANNEL(esci_serial_channel2,
               esci_serial_funs,
               esci_serial_info2,
               CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_FREESCALE_ESCI_C_BAUD),
               CYG_SERIAL_STOP_DEFAULT,
               CYG_SERIAL_PARITY_DEFAULT,
               CYG_SERIAL_WORD_LENGTH_DEFAULT,
               CYG_SERIAL_FLAGS_DEFAULT);
#endif
DEVTAB_ENTRY(esci_serial_io2,
             CYGDAT_IO_SERIAL_FREESCALE_ESCI_C_NAME,
             0, // does not depend on a lower level device driver
             &cyg_io_serial_devio,
             esci_serial_init,
             esci_serial_lookup,
             &esci_serial_channel2);
#endif // ifdef CYGPKG_IO_SERIAL_FREESCALE_ESCI_C


#if defined CYGPKG_IO_SERIAL_FREESCALE_ESCI_D
static esci_serial_info esci_serial_info3 = {
    esci_base          : CYGADDR_IO_SERIAL_FREESCALE_ESCI_D_BASE,
    interrupt_num      : CYGNUM_IO_SERIAL_FREESCALE_ESCI_D_INT_VECTOR,
    interrupt_priority : CYGNUM_IO_SERIAL_FREESCALE_ESCI_D_INT_PRIORITY
};
#if CYGNUM_IO_SERIAL_FREESCALE_ESCI_D_BUFSIZE > 0
static unsigned char 
    esci_serial_out_buf3[CYGNUM_IO_SERIAL_FREESCALE_ESCI_D_BUFSIZE]; 
static unsigned char 
    esci_serial_in_buf3[CYGNUM_IO_SERIAL_FREESCALE_ESCI_D_BUFSIZE];

static 
SERIAL_CHANNEL_USING_INTERRUPTS(esci_serial_channel3,
                                esci_serial_funs,
                                esci_serial_info3,
                                CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_FREESCALE_ESCI_D_BAUD),
                                CYG_SERIAL_STOP_DEFAULT,
                                CYG_SERIAL_PARITY_DEFAULT,
                                CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                CYG_SERIAL_FLAGS_DEFAULT,
                                &esci_serial_out_buf3[0],
                                sizeof(esci_serial_out_buf3),
                                &esci_serial_in_buf3[0],
                                sizeof(esci_serial_in_buf3));
#else 
static 
SERIAL_CHANNEL(esci_serial_channel3,
               esci_serial_funs,
               esci_serial_info3,
               CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_FREESCALE_ESCI_D_BAUD),
               CYG_SERIAL_STOP_DEFAULT,
               CYG_SERIAL_PARITY_DEFAULT,
               CYG_SERIAL_WORD_LENGTH_DEFAULT,
               CYG_SERIAL_FLAGS_DEFAULT);
#endif
DEVTAB_ENTRY(esci_serial_io3,
             CYGDAT_IO_SERIAL_FREESCALE_ESCI_D_NAME,
             0, // does not depend on a lower level device driver
             &cyg_io_serial_devio,
             esci_serial_init,
             esci_serial_lookup,
             &esci_serial_channel3);
#endif // ifdef CYGPKG_IO_SERIAL_FREESCALE_ESCI_D

//----------------------------------------------------------------------------
// Internal function to actually configure the hardware to desired
// baud rate, etc.
//----------------------------------------------------------------------------
static bool
esci_serial_config_port(serial_channel * chan, cyg_serial_info_t * new_config,
                        bool init)
{
    esci_serial_info * esci_chan = (esci_serial_info *)(chan->dev_priv);
    cyg_addrword_t esci_base = esci_chan->esci_base;
    cyg_uint16 baud_rate = ((new_config->baud >= 0) && 
                            (new_config->baud < (sizeof(select_baud)/
                                                 sizeof(select_baud[0]))))
      ? select_baud[new_config->baud] : 0;
    
    cyg_uint16 esci_cr12=0, esci_cr12_old;
    
    HAL_WRITE_UINT8(FREESCALE_ESCI_CR3(esci_base), 0);
    HAL_WRITE_UINT16(FREESCALE_ESCI_LINCTRL(esci_base), 0);
    HAL_WRITE_UINT16(FREESCALE_ESCI_CR12(esci_base), 0);
    
    if(!baud_rate) return false;    // Invalid baud rate selected
    
    switch(new_config->word_length){
    case 8: break;
    default: return false;
    }
    
    switch(new_config->parity){
    case CYGNUM_SERIAL_PARITY_ODD:
        esci_cr12 |= FREESCALE_ESCI_CR12_PT;
    case CYGNUM_SERIAL_PARITY_EVEN:
        esci_cr12 |= FREESCALE_ESCI_CR12_PE;        
    case CYGNUM_SERIAL_PARITY_NONE:
        break;
    default: return false;
    }
    
    if(new_config->stop!=CYGNUM_SERIAL_STOP_1) return false;
    
    // Enable the device
    esci_cr12 |= FREESCALE_ESCI_CR12_TE | FREESCALE_ESCI_CR12_RE;
    
    if(init){ // Enable the receiver interrupt
        esci_cr12 |= FREESCALE_ESCI_CR12_RIE;
    }else{    // Restore the old interrupt state
        HAL_READ_UINT16(FREESCALE_ESCI_CR12(esci_base), esci_cr12_old);
        esci_cr12 |= (esci_cr12_old & FREESCALE_ESCI_CR12_RIE);
    }
    HAL_WRITE_UINT16(FREESCALE_ESCI_CR12(esci_base), esci_cr12);
    
    if(new_config != &chan->config)
        chan->config = *new_config;

    return true;
}

//--------------------------------------------------------------
// Function to initialize the device.  Called at bootstrap time.
//--------------------------------------------------------------
static bool
esci_serial_init(struct cyg_devtab_entry * tab)
{
    serial_channel * chan = (serial_channel *)tab->priv;
    esci_serial_info * esci_chan = (esci_serial_info *)chan->dev_priv;

    // Really only required for interrupt driven devices
    (chan->callbacks->serial_init)(chan);  
    if(chan->out_cbuf.len != 0){ 
        cyg_drv_interrupt_create(esci_chan->interrupt_num,
                                 esci_chan->interrupt_priority,
                                 // Data item passed to interrupt handler
                                 (cyg_addrword_t)chan,   
                                 esci_serial_ISR,
                                 esci_serial_DSR,
                                 &esci_chan->interrupt_handle,
                                 &esci_chan->interrupt_obj);
        
        cyg_drv_interrupt_attach(esci_chan->interrupt_handle);
        cyg_drv_interrupt_unmask(esci_chan->interrupt_num);
    }
    return esci_serial_config_port(chan, &chan->config, true);
}

//----------------------------------------------------------------------
// This routine is called when the device is "looked" up (i.e. attached)
//----------------------------------------------------------------------
static Cyg_ErrNo
esci_serial_lookup(struct cyg_devtab_entry ** tab, 
                   struct cyg_devtab_entry * sub_tab, const char * name)
{
    serial_channel * chan = (serial_channel *)(*tab)->priv;
    // Really only required for interrupt driven devices
    (chan->callbacks->serial_init)(chan);  

    return ENOERR;
}

//-----------------------------------------------------------------
// Send a character to Tx
//-----------------------------------------------------------------
static bool
esci_serial_putc(serial_channel * chan, unsigned char ch_out)
{
    esci_serial_info * esci_chan = (esci_serial_info *)chan->dev_priv;
    cyg_addrword_t esci_base = esci_chan->esci_base;
    cyg_uint16 esci_sr;
    
    HAL_READ_UINT16(FREESCALE_ESCI_SR(esci_base), esci_sr);
    if(esci_sr & FREESCALE_ESCI_SR_TDRE){
        HAL_WRITE_UINT16(FREESCALE_ESCI_SR(esci_base), FREESCALE_ESCI_SR_TDRE);
        HAL_WRITE_UINT8(FREESCALE_ESCI_DRL(esci_base), ch_out);
        return true;
    }else
        return false;
}

//---------------------------------------------------------------------
// Fetch a character Rx (for polled operation only)
//---------------------------------------------------------------------
static unsigned char
esci_serial_getc(serial_channel * chan)
{
    cyg_uint8 ch_in;
    esci_serial_info * esci_chan = (esci_serial_info *)chan->dev_priv;
    cyg_addrword_t esci_base = esci_chan->esci_base;
    
    cyg_uint16 esci_sr;
    
    do{
        HAL_READ_UINT16(FREESCALE_ESCI_SR(esci_base), esci_sr);
    }while(esci_sr & FREESCALE_ESCI_SR_RDRF);
    
    HAL_READ_UINT8(FREESCALE_ESCI_DRL(esci_base), ch_in);
    HAL_WRITE_UINT16(FREESCALE_ESCI_SR(esci_base), FREESCALE_ESCI_SR_RDRF);
    
    return ch_in;
}

//---------------------------------------------------
// Set up the device characteristics; baud rate, etc.
//---------------------------------------------------
static bool
esci_serial_set_config(serial_channel * chan, cyg_uint32 key, 
                       const void *xbuf, cyg_uint32 * len)
{
    switch(key) {
    case CYG_IO_SET_CONFIG_SERIAL_INFO:{
            cyg_serial_info_t *config = (cyg_serial_info_t *)xbuf;
            if(*len < sizeof(cyg_serial_info_t)) {
                return -EINVAL;
            }
            *len = sizeof(cyg_serial_info_t);
            if(true != esci_serial_config_port(chan, config, false))
            return -EINVAL;
        }
        break;
    default:
        return -EINVAL;
    }
    return ENOERR;
}

//-------------------------------------
// Enable the transmitter on the device
//-------------------------------------
static void esci_serial_start_xmit(serial_channel * chan)
{
    esci_serial_info * esci_chan = (esci_serial_info *)chan->dev_priv;
    cyg_addrword_t esci_base = esci_chan->esci_base;
    cyg_uint16 esci_cr12;
    
    HAL_READ_UINT16(FREESCALE_ESCI_CR12(esci_base), esci_cr12);
    esci_cr12 |= FREESCALE_ESCI_CR12_TIE;
    HAL_WRITE_UINT16(FREESCALE_ESCI_CR12(esci_base), esci_cr12);
}

//--------------------------------------
// Disable the transmitter on the device
//--------------------------------------
static void esci_serial_stop_xmit(serial_channel * chan)
{
    esci_serial_info * esci_chan = (esci_serial_info *)chan->dev_priv;

    cyg_addrword_t esci_base = esci_chan->esci_base;
    cyg_uint16 esci_cr12;
    
    HAL_READ_UINT16(FREESCALE_ESCI_CR12(esci_base), esci_cr12);
    esci_cr12 &= ~FREESCALE_ESCI_CR12_TIE;
    HAL_WRITE_UINT16(FREESCALE_ESCI_CR12(esci_base), esci_cr12);
}

//-----------------------------------------
// The low level interrupt handler
//-----------------------------------------
static
cyg_uint32 esci_serial_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel * chan = (serial_channel *)data;
    esci_serial_info * esci_chan = (esci_serial_info *)chan->dev_priv;

    cyg_drv_interrupt_mask(esci_chan->interrupt_num);
    cyg_drv_interrupt_acknowledge(esci_chan->interrupt_num);

    return CYG_ISR_CALL_DSR; // cause the DSR to run
}


//------------------------------------------
// The high level interrupt handler
//------------------------------------------

#define FREESCALE_ESCI_SR_ERRORS (FREESCALE_ESCI_SR_OR | \
                                  FREESCALE_ESCI_SR_NF | \
                                  FREESCALE_ESCI_SR_FE | \
                                  FREESCALE_ESCI_SR_PF)

static void
esci_serial_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel * chan = (serial_channel *)data;
    esci_serial_info * esci_chan = (esci_serial_info *)chan->dev_priv;
    cyg_addrword_t esci_base = esci_chan->esci_base;
    cyg_uint16 esci_sr;
    cyg_uint8 esci_dr;

    HAL_READ_UINT16(FREESCALE_ESCI_SR(esci_base), esci_sr);
    if(esci_sr & FREESCALE_ESCI_SR_RDRF){ // Receiver full
        HAL_READ_UINT8(FREESCALE_ESCI_DRL(esci_base), esci_dr);
        HAL_WRITE_UINT16(FREESCALE_ESCI_SR(esci_base), FREESCALE_ESCI_SR_RDRF);
        if(esci_sr &= (cyg_uint16)FREESCALE_ESCI_SR_ERRORS){
            HAL_WRITE_UINT16(FREESCALE_ESCI_SR(esci_base), esci_sr);
        }else{
            (chan->callbacks->rcv_char)(chan, (cyg_uint8)esci_dr);
        }
    }else if(esci_sr & FREESCALE_ESCI_SR_TDRE){ //Transmitter empty
        (chan->callbacks->xmt_char)(chan);
    }
    
    cyg_drv_interrupt_unmask(esci_chan->interrupt_num);
}

#endif // CYGPKG_IO_SERIAL_FREESCALE_ESCI_[ABCD]
// EOF ser_esci.c
