//==========================================================================
//
//      io/serial/v85x/v85x_v850_serial.c
//
//      NEC V850 Serial I/O Interface Module (interrupt driven)
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-03-31
// Purpose:      V850 Serial I/O module (interrupt driven version)
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/io_serial.h>
#include <pkgconf/io.h>
#include <pkgconf/kernel.h>

#include <cyg/io/io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/io/devtab.h>
#include <cyg/io/serial.h>
#include <cyg/infra/diag.h>

#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK
#include <cyg/kernel/kapi.h>
#endif

#ifdef CYGPKG_IO_SERIAL_V85X_V850

#include "v85x_v850_serial.h"

typedef struct v850_serial_info {
    CYG_ADDRWORD   base;
    CYG_WORD       int_num;
    cyg_interrupt  serial_interrupt[3];
    cyg_handle_t   serial_interrupt_handle[3];
    cyg_bool       tx_busy;
#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK
    cyg_alarm      tx_timeout;
    cyg_handle_t   tx_timeout_handle;
#endif
} v850_serial_info;

static bool v850_serial_init(struct cyg_devtab_entry *tab);
static bool v850_serial_putc(serial_channel *chan, unsigned char c);
static Cyg_ErrNo v850_serial_lookup(struct cyg_devtab_entry **tab, 
                                    struct cyg_devtab_entry *sub_tab,
                                    const char *name);
static unsigned char v850_serial_getc(serial_channel *chan);
static bool v850_serial_set_config(serial_channel *chan, cyg_serial_info_t *config);
static void v850_serial_start_xmit(serial_channel *chan);
static void v850_serial_stop_xmit(serial_channel *chan);

static cyg_uint32 v850_serial_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       v850_serial_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);

static SERIAL_FUNS(v850_serial_funs, 
                   v850_serial_putc, 
                   v850_serial_getc,
                   v850_serial_set_config,
                   v850_serial_start_xmit,
                   v850_serial_stop_xmit
    );

#ifdef CYGPKG_IO_SERIAL_V85X_V850_SERIAL0
static v850_serial_info v850_serial_info0 = {V850_REG_ASIM0,
                                             CYGNUM_HAL_VECTOR_INTSER0};
#if CYGNUM_IO_SERIAL_V85X_V850_SERIAL0_BUFSIZE > 0
static unsigned char v850_serial_out_buf0[CYGNUM_IO_SERIAL_V85X_V850_SERIAL0_BUFSIZE];
static unsigned char v850_serial_in_buf0[CYGNUM_IO_SERIAL_V85X_V850_SERIAL0_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(v850_serial_channel0,
                                       v850_serial_funs, 
                                       v850_serial_info0,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_V85X_V850_SERIAL0_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &v850_serial_out_buf0[0], sizeof(v850_serial_out_buf0),
                                       &v850_serial_in_buf0[0], sizeof(v850_serial_in_buf0)
    );
#else
static SERIAL_CHANNEL(v850_serial_channel0,
                      v850_serial_funs, 
                      v850_serial_info0,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_V85X_V850_SERIAL0_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(v850_serial_io0, 
             CYGDAT_IO_SERIAL_V85X_V850_SERIAL0_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             v850_serial_init, 
             v850_serial_lookup,     // Serial driver may need initializing
             &v850_serial_channel0
    );
#endif //  CYGPKG_IO_SERIAL_V85X_V850_SERIAL0

// Internal function to actually configure the hardware to desired baud rate, etc.
static bool
v850_serial_config_port(serial_channel *chan, cyg_serial_info_t *new_config, bool init)
{
    v850_serial_info *v850_chan = (v850_serial_info *)chan->dev_priv;
    volatile struct serial_port *port = (volatile struct serial_port *)v850_chan->base;
    unsigned char parity = select_parity[new_config->parity];
    unsigned char word_length = select_word_length[new_config->word_length-CYGNUM_SERIAL_WORD_LENGTH_5];
    unsigned char stop_bits = select_stop_bits[new_config->stop];
    if ((select_baud[new_config->baud].count == 0) ||
        (word_length == 0xFF) ||
        (parity == 0xFF) ||
        (stop_bits == 0xFF)) {
        return false;  // Unsupported configuration
    }
    port->asim = ASIM_TxRx_Tx | ASIM_TxRx_Rx | parity | word_length | stop_bits;
    port->brgc = select_baud[new_config->baud].count;
    port->brgm = select_baud[new_config->baud].mode + 1;
    if (new_config != &chan->config) {
        chan->config = *new_config;
    }
    return true;
}

#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK
//
// The serial ports on the V850 are incredibly stupid.  There is no
// interface status register which can tell you if it is possible to
// read or write a character!  The only way to discern this is by using
// interrupts [or at least an interrupt register and polling it].  Thus
// the serial transmit code has a problem in that it will be required by
// upper layers to "send until full".  The only way to decide "not full" is
// that an interrupt has happened.  If the serial driver is being mixed
// with diagnostic I/O, then serial transmit interrupts will possibly be
// lost.  
//
// This code attempts to compenstate by using a kernel alarm to reset the
// "device is busy" flag after some timeout.  The timeout period will be
// sufficiently long so as to not interfere with normal interrupt handling.
//
static void
v850_serial_tx_timeout(cyg_addrword_t p)
{
    v850_serial_info *v850_chan = (v850_serial_info *)p;
    v850_chan->tx_busy = false;
}
#endif

// Function to initialize the device.  Called at bootstrap time.
static bool 
v850_serial_init(struct cyg_devtab_entry *tab)
{
    int i;
    serial_channel *chan = (serial_channel *)tab->priv;
    v850_serial_info *v850_chan = (v850_serial_info *)chan->dev_priv;
#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK
    cyg_handle_t h;
#endif
#ifdef CYGDBG_IO_INIT
    diag_printf("V850 SERIAL init - dev: %x.%d\n", v850_chan->base, v850_chan->int_num);
#endif
    (chan->callbacks->serial_init)(chan);  // Really only required for interrupt driven devices
    if (chan->out_cbuf.len != 0) {
        for (i = 0;  i < 3;  i++) {
            cyg_drv_interrupt_create(v850_chan->int_num+i,
                                     99,                     // Priority - unused
                                     (cyg_addrword_t)chan,   // Data item passed to interrupt handler
                                     v850_serial_ISR,
                                     v850_serial_DSR,
                                     &v850_chan->serial_interrupt_handle[i],
                                     &v850_chan->serial_interrupt[i]);
            cyg_drv_interrupt_attach(v850_chan->serial_interrupt_handle[i]);
            cyg_drv_interrupt_unmask(v850_chan->int_num+i);
#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK
            cyg_clock_to_counter(cyg_real_time_clock(), &h);
            cyg_alarm_create(h, v850_serial_tx_timeout, (cyg_addrword_t)v850_chan, 
                             &v850_chan->tx_timeout_handle, &v850_chan->tx_timeout);
#endif
        }
    }
    v850_chan->tx_busy = false;
    return v850_serial_config_port(chan, &chan->config, true);
}

// This routine is called when the device is "looked" up (i.e. attached)
static Cyg_ErrNo 
v850_serial_lookup(struct cyg_devtab_entry **tab, 
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
v850_serial_putc(serial_channel *chan, unsigned char c)
{
    v850_serial_info *v850_chan = (v850_serial_info *)chan->dev_priv;
    volatile struct serial_port *port = (volatile struct serial_port *)v850_chan->base;
    if (!v850_chan->tx_busy) {
        v850_chan->tx_busy = true;
        port->txs = c;
#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK
        cyg_alarm_initialize(v850_chan->tx_timeout_handle, cyg_current_time()+10, 0);
#endif
    } else {
        return false;  // Couldn't send, tx was busy
    }
}

// Fetch a character from the device input buffer, waiting if necessary
static unsigned char 
v850_serial_getc(serial_channel *chan)
{
    v850_serial_info *v850_chan = (v850_serial_info *)chan->dev_priv;
    volatile struct serial_port *port = (volatile struct serial_port *)v850_chan->base;
    return port->rxs;
}

// Set up the device characteristics; baud rate, etc.
static bool 
v850_serial_set_config(serial_channel *chan, cyg_serial_info_t *config)
{
    return v850_serial_config_port(chan, config, false);
}

// Enable the transmitter on the device
static void
v850_serial_start_xmit(serial_channel *chan)
{
    v850_serial_info *v850_chan = (v850_serial_info *)chan->dev_priv;
    (chan->callbacks->xmt_char)(chan);  // Kick transmitter (if necessary)
    cyg_drv_interrupt_unmask(v850_chan->int_num+INT_Tx);  // Enable Tx interrupt
}

// Disable the transmitter on the device
static void 
v850_serial_stop_xmit(serial_channel *chan)
{
    v850_serial_info *v850_chan = (v850_serial_info *)chan->dev_priv;
    cyg_drv_interrupt_mask(v850_chan->int_num+INT_Tx);  // Disable Tx interrupt
}

// Serial I/O - low level interrupt handler (ISR)
static cyg_uint32 
v850_serial_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_drv_interrupt_mask(vector);
    cyg_drv_interrupt_acknowledge(vector);
    return CYG_ISR_CALL_DSR;  // Cause DSR to be run
}

// Serial I/O - high level interrupt handler (DSR)
static void       
v850_serial_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    v850_serial_info *v850_chan = (v850_serial_info *)chan->dev_priv;
    volatile struct serial_port *port = (volatile struct serial_port *)v850_chan->base;
    switch (vector-(v850_chan->int_num)) {
    case INT_ERR:
    case INT_Rx:
        (chan->callbacks->rcv_char)(chan, port->rxs);
        break;
    case INT_Tx:
        v850_chan->tx_busy = false;
#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK
        cyg_alarm_initialize(v850_chan->tx_timeout_handle, 0, 0);
#endif
        (chan->callbacks->xmt_char)(chan);
        break;
    }
    cyg_drv_interrupt_unmask(vector);
}
#endif
