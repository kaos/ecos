//==========================================================================
//
//      io/serial/arm/cl7211_serial.c
//
//      Cirrus Logic CL7211 Serial I/O Interface Module (interrupt driven)
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   gthomas
// Contributors:  gthomas
// Date:        1999-02-04
// Purpose:     CL7211 Serial I/O module (interrupt driven version)
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/io_serial.h>
#include <pkgconf/io.h>
#include <cyg/io/io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/io/devtab.h>
#include <cyg/io/serial.h>
#include <cyg/infra/diag.h>

#ifdef CYGPKG_IO_SERIAL_ARM_CL7211

#include "cl7211_serial.h"

typedef struct cl7211_serial_info {
    CYG_ADDRWORD   data,                      // Pointer to data register
                   control,                   // Pointer to baud rate/line control register
                   stat,                      // Pointer to system flags for this port
                   syscon;                    // Pointer to system control for this port
    CYG_WORD       tx_int_num,                // Transmit interrupt number
                   rx_int_num,                // Receive interrupt number
                   ms_int_num;                // Modem Status Change interrupt number
    cyg_interrupt  serial_tx_interrupt, 
                   serial_rx_interrupt, 
                   serial_ms_interrupt;
    cyg_handle_t   serial_tx_interrupt_handle, 
                   serial_rx_interrupt_handle, 
                   serial_ms_interrupt_handle;
    bool           tx_enabled;
} cl7211_serial_info;

static bool cl7211_serial_init(struct cyg_devtab_entry *tab);
static bool cl7211_serial_putc(serial_channel *chan, unsigned char c);
static Cyg_ErrNo cl7211_serial_lookup(struct cyg_devtab_entry **tab, 
                                   struct cyg_devtab_entry *sub_tab,
                                   const char *name);
static unsigned char cl7211_serial_getc(serial_channel *chan);
static bool cl7211_serial_set_config(serial_channel *chan, cyg_serial_info_t *config);
static void cl7211_serial_start_xmit(serial_channel *chan);
static void cl7211_serial_stop_xmit(serial_channel *chan);

static cyg_uint32 cl7211_serial_tx_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       cl7211_serial_tx_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
static cyg_uint32 cl7211_serial_rx_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       cl7211_serial_rx_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
static cyg_uint32 cl7211_serial_ms_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       cl7211_serial_ms_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);

static SERIAL_FUNS(cl7211_serial_funs, 
                   cl7211_serial_putc, 
                   cl7211_serial_getc,
                   cl7211_serial_set_config,
                   cl7211_serial_start_xmit,
                   cl7211_serial_stop_xmit
    );

#ifdef CYGPKG_IO_SERIAL_ARM_CL7211_SERIAL1
static cl7211_serial_info cl7211_serial_info1 = {UARTDR1, // Data register
                                                 UBLCR1,  // Port control
                                                 SYSFLG1, // Status
                                                 SYSCON1, // System config
                                                 CYGNUM_HAL_INTERRUPT_UTXINT1, // Tx interrupt
                                                 CYGNUM_HAL_INTERRUPT_URXINT1, // Rx interrupt
                                                 0 /*CYGNUM_HAL_INTERRUPT_UMSINT*/}; // Modem control
#if CYGNUM_IO_SERIAL_ARM_CL7211_SERIAL1_BUFSIZE > 0
static unsigned char cl7211_serial_out_buf1[CYGNUM_IO_SERIAL_ARM_CL7211_SERIAL1_BUFSIZE];
static unsigned char cl7211_serial_in_buf1[CYGNUM_IO_SERIAL_ARM_CL7211_SERIAL1_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(cl7211_serial_channel1,
                                       cl7211_serial_funs, 
                                       cl7211_serial_info1,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_CL7211_SERIAL1_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &cl7211_serial_out_buf1[0], sizeof(cl7211_serial_out_buf1),
                                       &cl7211_serial_in_buf1[0], sizeof(cl7211_serial_in_buf1)
    );
#else
static SERIAL_CHANNEL(cl7211_serial_channel1,
                      cl7211_serial_funs, 
                      cl7211_serial_info1,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_CL7211_SERIAL1_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(cl7211_serial_io1, 
             CYGDAT_IO_SERIAL_ARM_CL7211_SERIAL1_NAME,
             0,                     // Does not depend on a lower level interface
             &serial_devio, 
             cl7211_serial_init, 
             cl7211_serial_lookup,     // Serial driver may need initializing
             &cl7211_serial_channel1
    );
#endif //  CYGPKG_IO_SERIAL_ARM_CL7211_SERIAL2

#ifdef CYGPKG_IO_SERIAL_ARM_CL7211_SERIAL2
static cl7211_serial_info cl7211_serial_info2 = {UARTDR2, // Data register
                                                 UBLCR2,  // Port control
                                                 SYSFLG2, // Status
                                                 SYSCON2, // System config
                                                 CYGNUM_HAL_INTERRUPT_UTXINT2, // Tx interrupt
                                                 CYGNUM_HAL_INTERRUPT_URXINT2, // Rx interrupt
                                                 0};      // No modem control
#if CYGNUM_IO_SERIAL_ARM_CL7211_SERIAL2_BUFSIZE > 0
static unsigned char cl7211_serial_out_buf2[CYGNUM_IO_SERIAL_ARM_CL7211_SERIAL2_BUFSIZE];
static unsigned char cl7211_serial_in_buf2[CYGNUM_IO_SERIAL_ARM_CL7211_SERIAL2_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(cl7211_serial_channel2,
                                       cl7211_serial_funs, 
                                       cl7211_serial_info2,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_CL7211_SERIAL2_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &cl7211_serial_out_buf2[0], sizeof(cl7211_serial_out_buf2),
                                       &cl7211_serial_in_buf2[0], sizeof(cl7211_serial_in_buf2)
    );
#else
static SERIAL_CHANNEL(cl7211_serial_channel2,
                      cl7211_serial_funs, 
                      cl7211_serial_info2,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_CL7211_SERIAL2_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(cl7211_serial_io2, 
             CYGDAT_IO_SERIAL_ARM_CL7211_SERIAL2_NAME,
             0,                     // Does not depend on a lower level interface
             &serial_devio, 
             cl7211_serial_init, 
             cl7211_serial_lookup,     // Serial driver may need initializing
             &cl7211_serial_channel2
    );
#endif //  CYGPKG_IO_SERIAL_ARM_CL7211_SERIAL2

// Internal function to actually configure the hardware to desired baud rate, etc.
static bool
cl7211_serial_config_port(serial_channel *chan, cyg_serial_info_t *new_config, bool init)
{
    cl7211_serial_info *cl7211_chan = (cl7211_serial_info *)chan->dev_priv;
    volatile cyg_uint32 *syscon = (volatile cyg_uint32 *)cl7211_chan->syscon;
    volatile cyg_uint32 *blcfg = (volatile cyg_uint32 *)cl7211_chan->control;
    unsigned int baud_divisor = select_baud[new_config->baud];
    cyg_uint32 _lcr;
    if (baud_divisor == 0) return false;
    // Disable port interrupts while changing hardware
    _lcr = select_word_length[new_config->word_length - CYGNUM_SERIAL_WORD_LENGTH_5] | 
        select_stop_bits[new_config->stop] |
        select_parity[new_config->parity] |
        UBLCR_FIFOEN | UART_BITRATE(baud_divisor);
#ifdef CYGDBG_IO_INIT
    diag_printf("Set CTL: %x = %x\n", blcfg, _lcr);
#endif
    *blcfg = _lcr;
    *syscon |= SYSCON1_UART1EN;
    if (new_config != &chan->config) {
        chan->config = *new_config;
    }
    return true;
}

// Function to initialize the device.  Called at bootstrap time.
static bool 
cl7211_serial_init(struct cyg_devtab_entry *tab)
{
    serial_channel *chan = (serial_channel *)tab->priv;
    cl7211_serial_info *cl7211_chan = (cl7211_serial_info *)chan->dev_priv;
#ifdef CYGDBG_IO_INIT
    diag_printf("CL7211 SERIAL init - dev: %x.%d\n", cl7211_chan->control, cl7211_chan->tx_int_num);
#endif
    (chan->callbacks->serial_init)(chan);  // Really only required for interrupt driven devices
    if (chan->out_cbuf.len != 0) {
        cyg_drv_interrupt_create(cl7211_chan->tx_int_num,
                                 99,                     // Priority - unused
                                 (cyg_addrword_t)chan,   //  Data item passed to interrupt handler
                                 cl7211_serial_tx_ISR,
                                 cl7211_serial_tx_DSR,
                                 &cl7211_chan->serial_tx_interrupt_handle,
                                 &cl7211_chan->serial_tx_interrupt);
        cyg_drv_interrupt_attach(cl7211_chan->serial_tx_interrupt_handle);
        cyg_drv_interrupt_mask(cl7211_chan->tx_int_num);
        cl7211_chan->tx_enabled = false;
    }
    if (chan->in_cbuf.len != 0) {
        cyg_drv_interrupt_create(cl7211_chan->rx_int_num,
                                 99,                     // Priority - unused
                                 (cyg_addrword_t)chan,   //  Data item passed to interrupt handler
                                 cl7211_serial_rx_ISR,
                                 cl7211_serial_rx_DSR,
                                 &cl7211_chan->serial_rx_interrupt_handle,
                                 &cl7211_chan->serial_rx_interrupt);
        cyg_drv_interrupt_attach(cl7211_chan->serial_rx_interrupt_handle);
        cyg_drv_interrupt_unmask(cl7211_chan->rx_int_num);
    }
    cl7211_serial_config_port(chan, &chan->config, true);
    return true;
}

// This routine is called when the device is "looked" up (i.e. attached)
static Cyg_ErrNo 
cl7211_serial_lookup(struct cyg_devtab_entry **tab, 
                  struct cyg_devtab_entry *sub_tab,
                  const char *name)
{
    serial_channel *chan = (serial_channel *)(*tab)->priv;
    (chan->callbacks->serial_init)(chan);  // Really only required for interrupt driven devices
    return ENOERR;
}

// Send a character to the device output buffer.
// Return 'true' if character is sent to device
static bool
cl7211_serial_putc(serial_channel *chan, unsigned char c)
{
    cl7211_serial_info *cl7211_chan = (cl7211_serial_info *)chan->dev_priv;
    volatile cyg_uint8  *data = (volatile cyg_uint8  *)cl7211_chan->data;
    volatile cyg_uint32 *stat = (volatile cyg_uint32 *)cl7211_chan->stat;
    if ((*stat & SYSFLG1_UTXFF1) == 0) {
// Transmit buffer/FIFO is not full
        *data = c;
        return true;
    } else {
// No space
        return false;
    }
}

// Fetch a character from the device input buffer, waiting if necessary
static unsigned char 
cl7211_serial_getc(serial_channel *chan)
{
    unsigned char c;
    cl7211_serial_info *cl7211_chan = (cl7211_serial_info *)chan->dev_priv;
    volatile cyg_uint32 *data = (volatile cyg_uint32 *)cl7211_chan->data;
    volatile cyg_uint32 *stat = (volatile cyg_uint32 *)cl7211_chan->stat;
    while (*stat & SYSFLG1_URXFE1) ; // Wait for char
    c = *data;
    return c;
}

// Set up the device characteristics; baud rate, etc.
static bool 
cl7211_serial_set_config(serial_channel *chan, cyg_serial_info_t *config)
{
    return cl7211_serial_config_port(chan, config, false);
}

// Enable the transmitter (interrupt) on the device
static void
cl7211_serial_start_xmit(serial_channel *chan)
{
    cl7211_serial_info *cl7211_chan = (cl7211_serial_info *)chan->dev_priv;
    cl7211_chan->tx_enabled = true;
    cyg_drv_interrupt_unmask(cl7211_chan->tx_int_num);
}

// Disable the transmitter on the device
static void 
cl7211_serial_stop_xmit(serial_channel *chan)
{
    cl7211_serial_info *cl7211_chan = (cl7211_serial_info *)chan->dev_priv;
    cyg_drv_interrupt_mask(cl7211_chan->tx_int_num);
    cl7211_chan->tx_enabled = false;
}

// Serial I/O - low level Tx interrupt handler (ISR)
static cyg_uint32 
cl7211_serial_tx_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    cl7211_serial_info *cl7211_chan = (cl7211_serial_info *)chan->dev_priv;
    cyg_drv_interrupt_mask(cl7211_chan->tx_int_num);
    cyg_drv_interrupt_acknowledge(cl7211_chan->tx_int_num);
    return CYG_ISR_CALL_DSR;  // Cause DSR to be run
}

// Serial I/O - high level Tx interrupt handler (DSR)
static void       
cl7211_serial_tx_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    cl7211_serial_info *cl7211_chan = (cl7211_serial_info *)chan->dev_priv;
    (chan->callbacks->xmt_char)(chan);
    if (cl7211_chan->tx_enabled) {
        cyg_drv_interrupt_unmask(cl7211_chan->tx_int_num);
    }
}

// Serial I/O - low level Rx interrupt handler (ISR)
static cyg_uint32 
cl7211_serial_rx_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    cl7211_serial_info *cl7211_chan = (cl7211_serial_info *)chan->dev_priv;
    cyg_drv_interrupt_mask(cl7211_chan->rx_int_num);
    cyg_drv_interrupt_acknowledge(cl7211_chan->rx_int_num);
    return CYG_ISR_CALL_DSR;  // Cause DSR to be run
}

// Serial I/O - high level Rx interrupt handler (DSR)
static void       
cl7211_serial_rx_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    cl7211_serial_info *cl7211_chan = (cl7211_serial_info *)chan->dev_priv;
    volatile cyg_uint32 *datreg = (volatile cyg_uint32 *)cl7211_chan->data;
    volatile cyg_uint32 *stat = (volatile cyg_uint32 *)cl7211_chan->stat;
    while (!(*stat & SYSFLG1_URXFE1))
        (chan->callbacks->rcv_char)(chan, *datreg);
    cyg_drv_interrupt_unmask(cl7211_chan->rx_int_num);
}

// Serial I/O - low level Ms interrupt handler (ISR)
static cyg_uint32 
cl7211_serial_ms_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    cl7211_serial_info *cl7211_chan = (cl7211_serial_info *)chan->dev_priv;
    cyg_drv_interrupt_mask(cl7211_chan->ms_int_num);
    cyg_drv_interrupt_acknowledge(cl7211_chan->ms_int_num);
    return CYG_ISR_CALL_DSR;  // Cause DSR to be run
}

// Serial I/O - high level Ms interrupt handler (DSR)
static void       
cl7211_serial_ms_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
}
#endif // CYGPKG_IO_SERIAL_ARM_CL7211

