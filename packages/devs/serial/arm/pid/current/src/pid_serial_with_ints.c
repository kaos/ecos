//==========================================================================
//
//      io/serial/arm/pid_serial_with_ints.c
//
//      ARM PID Serial I/O Interface Module (interrupt driven)
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
// Contributors: gthomas, jlarmour
// Date:         1999-02-04
// Purpose:      PID Serial I/O module (interrupt driven version)
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

#ifdef CYGPKG_IO_SERIAL_ARM_PID
#include "pid_serial.h"

typedef struct pid_serial_info {
    CYG_ADDRWORD   base;
    CYG_WORD       int_num;
    cyg_interrupt  serial_interrupt;
    cyg_handle_t   serial_interrupt_handle;
} pid_serial_info;

static bool pid_serial_init(struct cyg_devtab_entry *tab);
static bool pid_serial_putc(serial_channel *chan, unsigned char c);
static Cyg_ErrNo pid_serial_lookup(struct cyg_devtab_entry **tab, 
                                   struct cyg_devtab_entry *sub_tab,
                                   const char *name);
static unsigned char pid_serial_getc(serial_channel *chan);
static Cyg_ErrNo pid_serial_set_config(serial_channel *chan, cyg_uint32 key,
                                       const void *xbuf, cyg_uint32 *len);
static void pid_serial_start_xmit(serial_channel *chan);
static void pid_serial_stop_xmit(serial_channel *chan);

static cyg_uint32 pid_serial_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       pid_serial_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);

static SERIAL_FUNS(pid_serial_funs, 
                   pid_serial_putc, 
                   pid_serial_getc,
                   pid_serial_set_config,
                   pid_serial_start_xmit,
                   pid_serial_stop_xmit
    );

#ifdef CYGPKG_IO_SERIAL_ARM_PID_SERIAL0
static pid_serial_info pid_serial_info0 = {0x0D800000, CYGNUM_HAL_INTERRUPT_SERIALA};
#if CYGNUM_IO_SERIAL_ARM_PID_SERIAL0_BUFSIZE > 0
static unsigned char pid_serial_out_buf0[CYGNUM_IO_SERIAL_ARM_PID_SERIAL0_BUFSIZE];
static unsigned char pid_serial_in_buf0[CYGNUM_IO_SERIAL_ARM_PID_SERIAL0_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(pid_serial_channel0,
                                       pid_serial_funs, 
                                       pid_serial_info0,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_PID_SERIAL0_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &pid_serial_out_buf0[0], sizeof(pid_serial_out_buf0),
                                       &pid_serial_in_buf0[0], sizeof(pid_serial_in_buf0)
    );
#else
static SERIAL_CHANNEL(pid_serial_channel0,
                      pid_serial_funs, 
                      pid_serial_info0,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_PID_SERIAL0_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(pid_serial_io0, 
             CYGDAT_IO_SERIAL_ARM_PID_SERIAL0_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             pid_serial_init, 
             pid_serial_lookup,     // Serial driver may need initializing
             &pid_serial_channel0
    );
#endif //  CYGPKG_IO_SERIAL_ARM_PID_SERIAL0

#ifdef CYGPKG_IO_SERIAL_ARM_PID_SERIAL1
static pid_serial_info pid_serial_info1 = {0x0D800020, CYGNUM_HAL_INTERRUPT_SERIALB};
#if CYGNUM_IO_SERIAL_ARM_PID_SERIAL1_BUFSIZE > 0
static unsigned char pid_serial_out_buf1[CYGNUM_IO_SERIAL_ARM_PID_SERIAL1_BUFSIZE];
static unsigned char pid_serial_in_buf1[CYGNUM_IO_SERIAL_ARM_PID_SERIAL1_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(pid_serial_channel1,
                                       pid_serial_funs, 
                                       pid_serial_info1,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_PID_SERIAL1_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &pid_serial_out_buf1[0], sizeof(pid_serial_out_buf1),
                                       &pid_serial_in_buf1[0], sizeof(pid_serial_in_buf1)
    );
#else
static SERIAL_CHANNEL(pid_serial_channel1,
                      pid_serial_funs, 
                      pid_serial_info1,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_PID_SERIAL1_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(pid_serial_io1, 
             CYGDAT_IO_SERIAL_ARM_PID_SERIAL1_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             pid_serial_init, 
             pid_serial_lookup,     // Serial driver may need initializing
             &pid_serial_channel1
    );
#endif //  CYGPKG_IO_SERIAL_ARM_PID_SERIAL1

// Internal function to actually configure the hardware to desired baud rate, etc.
static bool
pid_serial_config_port(serial_channel *chan, cyg_serial_info_t *new_config, bool init)
{
    pid_serial_info *pid_chan = (pid_serial_info *)chan->dev_priv;
    volatile struct serial_port *port = (volatile struct serial_port *)pid_chan->base;
    unsigned short baud_divisor = select_baud[new_config->baud];
    unsigned char _lcr, _ier;
    if (baud_divisor == 0) return false;  // Invalid configuration
    _ier = port->REG_ier;
    port->REG_ier = 0;  // Disable port interrupts while changing hardware
    _lcr = select_word_length[new_config->word_length - CYGNUM_SERIAL_WORD_LENGTH_5] | 
        select_stop_bits[new_config->stop] |
        select_parity[new_config->parity];
    port->REG_lcr = _lcr;
    port->REG_lcr |= LCR_DL;
    port->REG_mdl = baud_divisor >> 8;
    port->REG_ldl = baud_divisor & 0xFF;
    port->REG_lcr &= ~LCR_DL;
    if (init) {
        port->REG_fcr = 0x07;  // Enable and clear FIFO
        if (chan->out_cbuf.len != 0) {
            _ier = IER_RCV;
        } else {
            _ier = 0;
        }
        port->REG_mcr = MCR_INT|MCR_DTR|MCR_RTS;  // Master interrupt enable
    }
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
    _ier |= (IER_LS|IER_MS);
#endif
    port->REG_ier = _ier;
    if (new_config != &chan->config) {
        chan->config = *new_config;
    }
    return true;
}

// Function to initialize the device.  Called at bootstrap time.
static bool 
pid_serial_init(struct cyg_devtab_entry *tab)
{
    serial_channel *chan = (serial_channel *)tab->priv;
    pid_serial_info *pid_chan = (pid_serial_info *)chan->dev_priv;
#ifdef CYGDBG_IO_INIT
    diag_printf("PID SERIAL init - dev: %x.%d\n", pid_chan->base, pid_chan->int_num);
#endif
    (chan->callbacks->serial_init)(chan);  // Really only required for interrupt driven devices
    if (chan->out_cbuf.len != 0) {
        cyg_drv_interrupt_create(pid_chan->int_num,
                                 99,                     // Priority - what goes here?
                                 (cyg_addrword_t)chan,   //  Data item passed to interrupt handler
                                 pid_serial_ISR,
                                 pid_serial_DSR,
                                 &pid_chan->serial_interrupt_handle,
                                 &pid_chan->serial_interrupt);
        cyg_drv_interrupt_attach(pid_chan->serial_interrupt_handle);
        cyg_drv_interrupt_unmask(pid_chan->int_num);
    }
    pid_serial_config_port(chan, &chan->config, true);
    return true;
}

// This routine is called when the device is "looked" up (i.e. attached)
static Cyg_ErrNo 
pid_serial_lookup(struct cyg_devtab_entry **tab, 
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
pid_serial_putc(serial_channel *chan, unsigned char c)
{
    pid_serial_info *pid_chan = (pid_serial_info *)chan->dev_priv;
    volatile struct serial_port *port = (volatile struct serial_port *)pid_chan->base;
    if (port->REG_lsr & LSR_THE) {
// Transmit buffer is empty
        port->REG_thr = c;
        return true;
    } else {
// No space
        return false;
    }
}

// Fetch a character from the device input buffer, waiting if necessary
static unsigned char 
pid_serial_getc(serial_channel *chan)
{
    unsigned char c;
    pid_serial_info *pid_chan = (pid_serial_info *)chan->dev_priv;
    volatile struct serial_port *port = (volatile struct serial_port *)pid_chan->base;
    while ((port->REG_lsr & LSR_RSR) == 0) ;   // Wait for char
    c = port->REG_rhr;
    return c;
}

// Set up the device characteristics; baud rate, etc.
static Cyg_ErrNo
pid_serial_set_config(serial_channel *chan, cyg_uint32 key, const void *xbuf,
                      cyg_uint32 *len)
{
    pid_serial_info *pid_chan = (pid_serial_info *)chan->dev_priv;

    switch (key) {
    case CYG_IO_SET_CONFIG_SERIAL_INFO:
      {
        cyg_serial_info_t *config = (cyg_serial_info_t *)xbuf;
        if ( *len < sizeof(cyg_serial_info_t) ) {
            return -EINVAL;
        }
        *len = sizeof(cyg_serial_info_t);
        if ( true != pid_serial_config_port(chan, config, false) )
            return -EINVAL;
      }
      break;
#ifdef CYGOPT_IO_SERIAL_FLOW_CONTROL_HW
    case CYG_IO_SET_CONFIG_SERIAL_HW_RX_FLOW_THROTTLE:
      {
          volatile struct serial_port *port = (volatile struct serial_port *)pid_chan->base;
          cyg_uint8 *f = (cyg_uint8 *)xbuf;
          unsigned char mask=0;
          if ( *len < *f )
              return -EINVAL;
          
          if ( chan->config.flags & CYGNUM_SERIAL_FLOW_RTSCTS_RX )
              mask = MCR_RTS;
          if ( chan->config.flags & CYGNUM_SERIAL_FLOW_DSRDTR_RX )
              mask |= MCR_DTR;
          if (*f) // we should throttle
              port->REG_mcr &= ~mask;
          else // we should no longer throttle
              port->REG_mcr |= mask;
      }
      break;
    case CYG_IO_SET_CONFIG_SERIAL_HW_FLOW_CONFIG:
        // Nothing to do because we do support both RTSCTS and DSRDTR flow
        // control.
        // Other targets would clear any unsupported flags here.
        // We just return ENOERR.
      break;
#endif
    default:
        return -EINVAL;
    }
    return ENOERR;
}

// Enable the transmitter on the device
static void
pid_serial_start_xmit(serial_channel *chan)
{
    pid_serial_info *pid_chan = (pid_serial_info *)chan->dev_priv;
    volatile struct serial_port *port = (volatile struct serial_port *)pid_chan->base;
    port->REG_ier |= IER_XMT;  // Enable xmit interrupt
}

// Disable the transmitter on the device
static void 
pid_serial_stop_xmit(serial_channel *chan)
{
    pid_serial_info *pid_chan = (pid_serial_info *)chan->dev_priv;
    volatile struct serial_port *port = (volatile struct serial_port *)pid_chan->base;
    port->REG_ier &= ~IER_XMT;  // Disable xmit interrupt
}

// Serial I/O - low level interrupt handler (ISR)
static cyg_uint32 
pid_serial_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    pid_serial_info *pid_chan = (pid_serial_info *)chan->dev_priv;
    cyg_drv_interrupt_mask(pid_chan->int_num);
    cyg_drv_interrupt_acknowledge(pid_chan->int_num);
    return CYG_ISR_CALL_DSR;  // Cause DSR to be run
}

// Serial I/O - high level interrupt handler (DSR)
static void       
pid_serial_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    pid_serial_info *pid_chan = (pid_serial_info *)chan->dev_priv;
    volatile struct serial_port *port = (volatile struct serial_port *)pid_chan->base;
    unsigned char isr;

    // Check if we have an interrupt pending - note that the interrupt
    // is pending of the low bit of the isr is *0*, not 1.
    while (((isr = port->REG_isr) & ISR_nIP) == 0) {
        switch (isr&0x6) {
        case ISR_Rx:
            while(port->REG_lsr & LSR_RSR)
                (chan->callbacks->rcv_char)(chan, port->REG_rhr);
            break;
        case ISR_Tx:
            (chan->callbacks->xmt_char)(chan);
            break;

#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
        case ISR_LS:
            {
                cyg_serial_line_status_t stat;
                unsigned char lsr=port->REG_lsr;
                // this might look expensive, but it is rarely the case that
                // more than one of these is set
                stat.value = 1;
                if ( lsr & LSR_OE ) {
                    stat.which = CYGNUM_SERIAL_STATUS_OVERRUNERR;
                    (chan->callbacks->indicate_status)(chan, &stat );
                }
                if ( lsr & LSR_PE ) {
                    stat.which = CYGNUM_SERIAL_STATUS_PARITYERR;
                    (chan->callbacks->indicate_status)(chan, &stat );
                }
                if ( lsr & LSR_FE ) {
                    stat.which = CYGNUM_SERIAL_STATUS_FRAMEERR;
                    (chan->callbacks->indicate_status)(chan, &stat );
                }
                if ( lsr & LSR_BI ) {
                    stat.which = CYGNUM_SERIAL_STATUS_BREAK;
                    (chan->callbacks->indicate_status)(chan, &stat );
                }
                if ( lsr & LSR_OE ) {
                    stat.which = CYGNUM_SERIAL_STATUS_OVERRUNERR;
                    (chan->callbacks->indicate_status)(chan, &stat );
                }
            }
            break;

        case ISR_MS:
            {
                cyg_serial_line_status_t stat;
                unsigned char msr=port->REG_msr;
#ifdef CYGOPT_IO_SERIAL_FLOW_CONTROL_HW
                if ( msr & MSR_DDSR )
                    if ( chan->config.flags & CYGNUM_SERIAL_FLOW_DSRDTR_TX ) {
                        stat.which = CYGNUM_SERIAL_STATUS_FLOW;
                        stat.value = (0 != (msr & MSR_DSR));
                        (chan->callbacks->indicate_status)(chan, &stat );
                    }
                if ( msr & MSR_DCTS )
                    if ( chan->config.flags & CYGNUM_SERIAL_FLOW_RTSCTS_TX ) {
                        stat.which = CYGNUM_SERIAL_STATUS_FLOW;
                        stat.value = (0 != (msr & MSR_CTS));
                        (chan->callbacks->indicate_status)(chan, &stat );
                    }
#endif
                if ( msr & MSR_DDCD ) {
                    stat.which = CYGNUM_SERIAL_STATUS_CARRIERDETECT;
                    stat.value = (0 != (msr & MSR_CD));
                    (chan->callbacks->indicate_status)(chan, &stat );
                }
                if ( msr & MSR_RI ) {
                    stat.which = CYGNUM_SERIAL_STATUS_RINGINDICATOR;
                    stat.value = 1;
                    (chan->callbacks->indicate_status)(chan, &stat );
                }
                if ( msr & MSR_TERI ) {
                    stat.which = CYGNUM_SERIAL_STATUS_RINGINDICATOR;
                    stat.value = 0;
                    (chan->callbacks->indicate_status)(chan, &stat );
                }
            }
            break;
#endif
        }
    }
    cyg_drv_interrupt_unmask(pid_chan->int_num);
}
#endif

// EOF pid_serial_with_ints.c
