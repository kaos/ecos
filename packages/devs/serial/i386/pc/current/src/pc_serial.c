//==========================================================================
//
//      io/serial/i386/pc_serial.c
//
//      i386 PC Serial I/O Interface Module (interrupt driven)
//      for use with 8250s or 16550s.
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Author(s):   gthomas
// Contributors:  gthomas, pjo
// Date:        1999-02-04
// Purpose:     AEB-1 Serial I/O module (interrupt driven version)
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

#ifdef CYGPKG_IO_SERIAL_I386_PC

#include "pc_serial.h"

typedef struct pc_serial_info {
    CYG_ADDRWORD   base;
    CYG_WORD       int_num;
    cyg_interrupt  serial_interrupt;
    cyg_handle_t   serial_interrupt_handle;
	enum		{	sNone = 0,
					s8250,
					s16450,
					s16550,
					s16550a
				}	deviceType ;
	unsigned char	_ier ;
	unsigned char	charsInTransmitter ;
} pc_serial_info;

static bool pc_serial_init(struct cyg_devtab_entry *tab);
static bool pc_serial_putc(serial_channel *chan, unsigned char c);
static Cyg_ErrNo pc_serial_lookup(struct cyg_devtab_entry **tab, 
                                   struct cyg_devtab_entry *sub_tab,
                                   const char *name);
static unsigned char pc_serial_getc(serial_channel *chan);
static bool pc_serial_set_config(serial_channel *chan, cyg_serial_info_t *config);
static void pc_serial_start_xmit(serial_channel *chan);
static void pc_serial_stop_xmit(serial_channel *chan);

static cyg_uint32 pc_serial_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       pc_serial_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);

static SERIAL_FUNS(pc_serial_funs, 
                   pc_serial_putc, 
                   pc_serial_getc,
                   pc_serial_set_config,
                   pc_serial_start_xmit,
                   pc_serial_stop_xmit
    );

#ifdef CYGPKG_IO_SERIAL_I386_PC_SERIAL0
static pc_serial_info pc_serial_info0 = { CYGNUM_IO_SERIAL_I386_PC_SERIAL0_IOBASE, 
                                           CYGNUM_IO_SERIAL_I386_PC_SERIAL0_INT};
#if CYGNUM_IO_SERIAL_I386_PC_SERIAL0_BUFSIZE > 0
static unsigned char pc_serial_out_buf0[CYGNUM_IO_SERIAL_I386_PC_SERIAL0_BUFSIZE];
static unsigned char pc_serial_in_buf0[CYGNUM_IO_SERIAL_I386_PC_SERIAL0_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(pc_serial_channel0,
                                       pc_serial_funs, 
                                       pc_serial_info0,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_I386_PC_SERIAL0_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &pc_serial_out_buf0[0], sizeof(pc_serial_out_buf0),
                                       &pc_serial_in_buf0[0], sizeof(pc_serial_in_buf0)
    );
#else
static SERIAL_CHANNEL(pc_serial_channel0,
                      pc_serial_funs, 
                      pc_serial_info0,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_I386_PC_SERIAL0_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(pc_serial_io0, 
             CYGDAT_IO_SERIAL_I386_PC_SERIAL0_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             pc_serial_init, 
             pc_serial_lookup,     // Serial driver may need initializing
             &pc_serial_channel0
    );
#endif //  CYGPKG_IO_SERIAL_I386_PC_SERIAL0

#ifdef CYGPKG_IO_SERIAL_I386_PC_SERIAL1
static pc_serial_info pc_serial_info1 = { CYGNUM_IO_SERIAL_I386_PC_SERIAL1_IOBASE, 
                                           CYGNUM_IO_SERIAL_I386_PC_SERIAL1_INT};
#if CYGNUM_IO_SERIAL_I386_PC_SERIAL1_BUFSIZE > 0
static unsigned char pc_serial_out_buf1[CYGNUM_IO_SERIAL_I386_PC_SERIAL1_BUFSIZE];
static unsigned char pc_serial_in_buf1[CYGNUM_IO_SERIAL_I386_PC_SERIAL1_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(pc_serial_channel1,
                                       pc_serial_funs, 
                                       pc_serial_info1,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_I386_PC_SERIAL1_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &pc_serial_out_buf1[0], sizeof(pc_serial_out_buf1),
                                       &pc_serial_in_buf1[0], sizeof(pc_serial_in_buf1)
    );
#else
static SERIAL_CHANNEL(pc_serial_channel1,
                      pc_serial_funs, 
                      pc_serial_info1,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_I386_PC_SERIAL1_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(pc_serial_io1, 
             CYGDAT_IO_SERIAL_I386_PC_SERIAL1_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             pc_serial_init, 
             pc_serial_lookup,     // Serial driver may need initializing
             &pc_serial_channel1
    );
#endif //  CYGPKG_IO_SERIAL_I386_PC_SERIAL1


void pc_outb(int port, int byte) ;
int pc_inb(int port) ;


// Internal function to actually configure the hardware to desired baud rate, etc.
static bool pc_serial_config_port(serial_channel *chan, cyg_serial_info_t *new_config, bool init)
{
    pc_serial_info * port = (pc_serial_info *) chan->dev_priv;
    unsigned short baud_divisor = select_baud[new_config->baud];
    unsigned char _lcr ;
    if (baud_divisor == 0) return false;

// Initialize the device, if necessary.  First, find out what kind of device
//	it is.  Use a while() loop so that we can break to the next statement; 
//	otherwise we'd have an avalanche of if/else, which can become very
//	difficult to understand.
	while (init)
	{	port->deviceType = sNone ;
		pc_outb(port->base + MCR, MCR_LOOP) ;
		if (pc_inb(port->base + MSR) & 0xF0)
			break ;
		pc_outb(port->base + MCR, MCR_LOOP | 0xF) ;
		if ((pc_inb(port->base + MSR) & 0xF0) != 0xF0)
			break;
// So we have at least an 8250.
		pc_outb(port->base + MCR, 0x00) ;
		port->deviceType = s8250 ;
// Check for a scratch register; scratch register indicates 16450 or above.
		pc_outb(port->base + SCR, 0x55) ;
		if (pc_inb(port->base + SCR) != 0x55)
			break ;
		pc_outb(port->base + SCR, 0xAA) ;
		if (pc_inb(port->base + SCR) != 0xAA)
			break;
// Check for a FIFO
		pc_outb(port->base + FCR, FCR_FE) ;
		if ((pc_inb(port->base + ISR) & 0x80) == 0)
			break ;
		port->deviceType = s16550 ;	// Note! 16550 fifo's don't work!
		if ((pc_inb(port->base + ISR) & 0x40) == 0)
			break ;
		port->deviceType = s16550a ; // But 16550A fifos do.
		break;
	}

    port->_ier = pc_inb(port->base + IER);
	pc_outb(port->base + IER, 0) ; // Disable port interrupts while changing hardware
    _lcr = select_word_length[new_config->word_length - CYGNUM_SERIAL_WORD_LENGTH_5] | 
        select_stop_bits[new_config->stop] |
        select_parity[new_config->parity];
	pc_outb(port->base + LCR, _lcr | LCR_DL) ;
	pc_outb(port->base + MDL, baud_divisor >> 8) ;
	pc_outb(port->base + LDL, baud_divisor & 0xFF) ;
	pc_outb(port->base + LCR, _lcr) ;

    if (init)
	{	if (port->deviceType == s16550a)
			pc_outb(port->base + FCR, FCR_FE | FCR_RFR | FCR_TFR | FCR_RT14);
		else
			pc_outb(port->base + FCR, 0);

        if (chan->out_cbuf.len != 0)
			pc_outb(port->base + IER, IER_RCV) ;
		else
			pc_outb(port->base + IER, 0) ;
//		port->_ier = MCR_INT | MCR_DTR | MCR_RTS ;
//		port->_ier = IER_RCV | IER_LS | IER_MS ;
		pc_outb(port->base + MCR, MCR_INT | MCR_DTR | MCR_RTS) ;
		port->_ier = IER_RCV ;
	}

	pc_outb(port->base + IER, port->_ier) ;

    if (new_config != &chan->config)
		chan->config = *new_config;

    return true;
}



// Function to initialize the device.  Called at bootstrap time.
static bool pc_serial_init(struct cyg_devtab_entry * tab)
{
    serial_channel *chan = (serial_channel *)tab->priv;
    pc_serial_info *pc_chan = (pc_serial_info *)chan->dev_priv;
#ifdef CYGDBG_IO_INIT
    diag_printf("I386_PC SERIAL init - dev: %x.%d\n", pc_chan->base, pc_chan->int_num);
#endif
    (chan->callbacks->serial_init)(chan);  // Really only required for interrupt driven devices
    if (chan->out_cbuf.len != 0) {
        cyg_drv_interrupt_create(pc_chan->int_num,
                                 99,                     // Priority - unused
                                 (cyg_addrword_t)chan,   //  Data item passed to interrupt handler
                                 pc_serial_ISR,
                                 pc_serial_DSR,
                                 &pc_chan->serial_interrupt_handle,
                                 &pc_chan->serial_interrupt);
        cyg_drv_interrupt_attach(pc_chan->serial_interrupt_handle);
        cyg_drv_interrupt_unmask(pc_chan->int_num);
    }
    pc_serial_config_port(chan, &chan->config, true);
    return true;
}



// This routine is called when the device is "looked" up (i.e. attached)
static Cyg_ErrNo pc_serial_lookup(struct cyg_devtab_entry **tab, 
                  struct cyg_devtab_entry *sub_tab,
                  const char *name)
{
    serial_channel *chan = (serial_channel *)(*tab)->priv;
    (chan->callbacks->serial_init)(chan);  // Really only required for interrupt driven devices
    return ENOERR;
}





// Send a character to the device output buffer.
// Return 'true' if character is sent to device, return 'false' when
// we've ran out of buffer space in the device itself.
static bool pc_serial_putc(serial_channel *chan, unsigned char c)
{
	int x = 0 ;

    pc_serial_info * port = (pc_serial_info *)chan->dev_priv;

#if 0
// Make sure the transmitter is empty.
	if ((pc_inb(port->base + LSR) & LSR_THE) == 0)
		return false ;
#else
        // There isn't a status for FIFO "not full" from the '550, so we have to
        // keep track of it ourselves.
	if ((pc_inb(port->base + LSR) & LSR_THE) == 0)
	{	if ((port->deviceType == s16550a) &&
			(port->charsInTransmitter++ < PC16550_FIFO_TX_LENGTH))
		{	/* We're cool to keep sending characters. */
		}
		else
			return false ;
	}
#endif
	pc_outb(port->base + THR, c);

	return true ;
}



// Fetch a character from the device input buffer, waiting if necessary
static unsigned char pc_serial_getc(serial_channel *chan)
{
    pc_serial_info * port = (pc_serial_info *)chan->dev_priv;

	while ((pc_inb(port->base + LSR) & LSR_RSR) == 0)
		;

	return pc_inb(port->base + RHR) ;
}



// Set up the device characteristics; baud rate, etc.
static bool pc_serial_set_config(serial_channel *chan, cyg_serial_info_t *config)
{
    return pc_serial_config_port(chan, config, false);
}



// Enable the transmitter on the device
static void pc_serial_start_xmit(serial_channel *chan)
{
    pc_serial_info * port = (pc_serial_info *) chan->dev_priv;
    pc_outb(port->base + IER, port->_ier |= IER_XMT) ;
}



// Disable the transmitter on the device
static void pc_serial_stop_xmit(serial_channel * chan)
{	pc_serial_info * port = (pc_serial_info *) chan->dev_priv;
	pc_outb(port->base + IER, port->_ier &= ~IER_XMT) ;
}



// Serial I/O - low level interrupt handler (ISR)
static cyg_uint32 pc_serial_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *) data;
    pc_serial_info * port = (pc_serial_info *) chan->dev_priv;
    cyg_drv_interrupt_mask(port->int_num);
    cyg_drv_interrupt_acknowledge(port->int_num);
    return CYG_ISR_CALL_DSR;  // Cause DSR to be run
}



// Serial I/O - high level interrupt handler (DSR)
static void pc_serial_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    pc_serial_info *port = (pc_serial_info *)chan->dev_priv;
    unsigned char isr;
    isr = pc_inb(port->base + ISR) & 0x0E;
    if (isr == ISR_Tx)
    {	port->charsInTransmitter = 0 ;
    (chan->callbacks->xmt_char)(chan);
    } else if (isr == ISR_Rx)
    {
        // If we've got s16550a, then read all the characters in the fifo.
        while (pc_inb(port->base + LSR) & LSR_RSR)
        {
            char c = pc_inb(port->base + RHR);
            (chan->callbacks->rcv_char)(chan, c);
        }
    }
    cyg_drv_interrupt_unmask(port->int_num);
}
#endif
