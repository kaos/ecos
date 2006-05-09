//==========================================================================
//
//      devs/serial/coldfire/mcf5272/mcf5272_serial.c
//
//      ColdFire MCF5272 UART Serial I/O Interface Module (interrupt driven)
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Ltd.
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
// Author(s):    Wade Jensen, Enrico Piria
// Contributors: 
// Date:         2005-06-25
// Purpose:      MCF5272 Serial I/O module (interrupt driven version).
// Description: 
//
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/io_serial.h>
#include <pkgconf/io.h>
#include <cyg/io/io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/io/devtab.h>
#include <cyg/io/serial.h>
#include <cyg/infra/diag.h>
#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>
#include <string.h> // memset, strcmp

#include "mcf5272_serial.h"


// Use this macro to determine if at least one of the ports uses
// autobaud detection.
#if defined(CYGOPT_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL0_AUTOBAUD) || \
    defined(CYGOPT_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL1_AUTOBAUD)
#define REQUESTED_AUTOBAUD
#endif

// Autobaud states
typedef enum autobaud_states_t
{
    AB_IDLE = 0,      // Normal state. Autobaud process hasn't been initiated yet.
    AB_BEGIN_BREAK,   // Detected a start of the break.
    AB_BEGIN,         // Detected the end of the break and has set up the autobaud.
    AB_DISABLED       // Autobaud detection disabled for this port.
} autobaud_states_t;

typedef struct MCF5272_uart_info_t
{
    volatile mcf5272_uart_t         *base;                      // Base address of the UART registers
    cyg_uint32                      uart_vector;                // UART interrupt vector number

    cyg_interrupt                   serial_interrupt;           // Interrupt context
    cyg_handle_t                    serial_interrupt_handle;    // Interrupt handle

    volatile cyg_uint8              imr_mirror;                 // Interrupt mask register mirror

    cyg_serial_info_t               config;                     // The channel configuration

    autobaud_states_t               autobaud_state;             // The autobaud state


} MCF5272_uart_info_t;

// Function prototypes for the MCF5272 UART ISR and DSR.
static cyg_uint32 MCF5272_uart_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void MCF5272_uart_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);

// Function prototypes for the serial functions.
static bool MCF5272_uart_init(struct cyg_devtab_entry * tab);
static Cyg_ErrNo MCF5272_uart_lookup(struct cyg_devtab_entry **tab,
    struct cyg_devtab_entry *sub_tab, const char *name);
static bool MCF5272_uart_putc(serial_channel *chan, unsigned char c);
static unsigned char MCF5272_uart_getc(serial_channel *chan);
Cyg_ErrNo MCF5272_uart_set_config(serial_channel *chan, cyg_uint32 key,
    const void *xbuf, cyg_uint32 *len);
static void MCF5272_uart_start_xmit(serial_channel *chan);
static void MCF5272_uart_stop_xmit(serial_channel * chan);

// Declare the serial functions that are called by the common serial
// driver layer.
static SERIAL_FUNS
(
    MCF5272_uart_funs,
    MCF5272_uart_putc,
    MCF5272_uart_getc,
    MCF5272_uart_set_config,
    MCF5272_uart_start_xmit,
    MCF5272_uart_stop_xmit
);


// Definition for channel 0 UART configuration.
//***********************************************
#ifdef CYGPKG_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL0

// Data structure contains channel information.
static MCF5272_uart_info_t MCF5272_uart_channel_info_0;

// If the channel buffer size is zero, do not include interrupt UART processing
#if CYGNUM_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL0_BUFSIZE > 0

// Allocate receive and transmit buffer. The size of the buffer  is
// configured by the configtool.
static unsigned char 
MCF5272_uart_out_buf0[CYGNUM_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL0_BUFSIZE];
static unsigned char 
MCF5272_uart_in_buf0[CYGNUM_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL0_BUFSIZE];

// Channel function table. We register the UART functions here so
// that uppper serial drivers can call the serial driver's routines.
static SERIAL_CHANNEL_USING_INTERRUPTS(
    MCF5272_uart_channel_0,
    MCF5272_uart_funs,
    MCF5272_uart_channel_info_0,
    CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL0_BAUD),
    CYG_SERIAL_STOP_DEFAULT,
    CYG_SERIAL_PARITY_DEFAULT,
    CYG_SERIAL_WORD_LENGTH_DEFAULT,
    CYG_SERIAL_FLAGS_DEFAULT,
    MCF5272_uart_out_buf0, sizeof(MCF5272_uart_out_buf0),
    MCF5272_uart_in_buf0, sizeof(MCF5272_uart_in_buf0)
);

#else

// Don't use interrupt processing for the UART.
static SERIAL_CHANNEL(
    MCF5272_uart_channel_0,
    MCF5272_uart_funs,
    MCF5272_uart_channel_info_0,
    CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL0_BAUD),
    CYG_SERIAL_STOP_DEFAULT,
    CYG_SERIAL_PARITY_DEFAULT,
    CYG_SERIAL_WORD_LENGTH_DEFAULT,
    CYG_SERIAL_FLAGS_DEFAULT
);
#endif

DEVTAB_ENTRY(
    MCF5272_uart_io0,
    CYGDAT_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL0_NAME,
    0,                       // Does not depend on a lower level interface
    &cyg_io_serial_devio,    // The table of I/O functions.
    MCF5272_uart_init,       // UART initialization function.
    MCF5272_uart_lookup,     // The UART lookup function. This
                             // function typically sets
                             // up the device for actual use, 
                             // turning on interrupts,
                             // configuring the port, etc.
    &MCF5272_uart_channel_0
);
#endif // ifdef CYGNUM_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL0


// Definition for channel 1 UART configuration.
//***********************************************
#ifdef CYGPKG_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL1

// Data structure contains channel informtion.
static MCF5272_uart_info_t MCF5272_uart_channel_info_1;

// If the channel buffer size is zero, do not include interrupt UART processing
#if CYGNUM_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL1_BUFSIZE > 0

// Allocate receive and transmit buffer. The size of the buffer  is
// configured by the configtool.
static unsigned char 
MCF5272_uart_out_buf1[CYGNUM_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL1_BUFSIZE];
static unsigned char 
MCF5272_uart_in_buf1[CYGNUM_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL1_BUFSIZE];

// Channel function table. We register the UART functions here so
// that uppper serial drivers can call the serial driver's routines.
static SERIAL_CHANNEL_USING_INTERRUPTS(
    MCF5272_uart_channel_1,
    MCF5272_uart_funs,
    MCF5272_uart_channel_info_1,
    CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL1_BAUD),
    CYG_SERIAL_STOP_DEFAULT,
    CYG_SERIAL_PARITY_DEFAULT,
    CYG_SERIAL_WORD_LENGTH_DEFAULT,
    CYG_SERIAL_FLAGS_DEFAULT,
    MCF5272_uart_out_buf1, sizeof(MCF5272_uart_out_buf1),
    MCF5272_uart_in_buf1, sizeof(MCF5272_uart_in_buf1)
);

#else

static SERIAL_CHANNEL(
    MCF5272_uart_channel_1,
    MCF5272_uart_funs,
    MCF5272_uart_channel_info_1,
    CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL1_BAUD),
    CYG_SERIAL_STOP_DEFAULT,
    CYG_SERIAL_PARITY_DEFAULT,
    CYG_SERIAL_WORD_LENGTH_DEFAULT,
    CYG_SERIAL_FLAGS_DEFAULT
);
#endif

DEVTAB_ENTRY(
    MCF5272_uart_io1,
    CYGDAT_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL1_NAME,
    0,                     // Does not depend on a lower level interface
    &cyg_io_serial_devio,  // The table of I/O functions.
    MCF5272_uart_init,     // UART initialization function.
    MCF5272_uart_lookup,   // The UART lookup function. This function 
                           // typically sets up the device for actual use, 
                           // turing on interrupts, configuring the port, etc.
    &MCF5272_uart_channel_1
);
#endif // ifdef CYGNUM_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL1


// Function Prototypes

// Internal function to actually configure the hardware to desired
// baud rate, etc.
static bool MCF5272_uart_config_port(serial_channel*, cyg_serial_info_t*);
static void MCF5272_uart_start_xmit(serial_channel*);


// The table contains  dividers  to  divide  the  clock  to  configure  an
// approppriate baud rate for the UART.

#define DIVIDER(_baudrate_) \
    ((CYGHWR_HAL_SYSTEM_CLOCK_MHZ * 1000000) / ((_baudrate_) * 32))

static unsigned long dividers_table[]=
{
    0,
    DIVIDER(50),        // CYGNUM_SERIAL_BAUD_50 = 1
    DIVIDER(75),        // CYGNUM_SERIAL_BAUD_75
    DIVIDER(110),       // CYGNUM_SERIAL_BAUD_110
    DIVIDER(134.5),     // CYGNUM_SERIAL_BAUD_134_5
    DIVIDER(150),       // CYGNUM_SERIAL_BAUD_150
    DIVIDER(200),       // CYGNUM_SERIAL_BAUD_200
    DIVIDER(300),       // CYGNUM_SERIAL_BAUD_300
    DIVIDER(600),       // CYGNUM_SERIAL_BAUD_600
    DIVIDER(1200),      // CYGNUM_SERIAL_BAUD_1200
    DIVIDER(1800),      // CYGNUM_SERIAL_BAUD_1800
    DIVIDER(2400),      // CYGNUM_SERIAL_BAUD_2400
    DIVIDER(3600),      // CYGNUM_SERIAL_BAUD_3600
    DIVIDER(4800),      // CYGNUM_SERIAL_BAUD_4800
    DIVIDER(7200),      // CYGNUM_SERIAL_BAUD_7200
    DIVIDER(9600),      // CYGNUM_SERIAL_BAUD_9600
    DIVIDER(14400),     // CYGNUM_SERIAL_BAUD_14400
    DIVIDER(19200),     // CYGNUM_SERIAL_BAUD_19200
    DIVIDER(38400),     // CYGNUM_SERIAL_BAUD_38400
    DIVIDER(57600),     // CYGNUM_SERIAL_BAUD_57600
    DIVIDER(115200),    // CYGNUM_SERIAL_BAUD_115200
    DIVIDER(230400)     // CYGNUM_SERIAL_BAUD_230400
};


// ****************************************************************************
// MCF5272_uart_init() - This routine is called during bootstrap to set up the
//                       UART driver.
//
// INPUT:
//    Pointer to the the device table.
//
// RETURN:
//    Returns true if the initialization is successful. Otherwise, it retuns 
//    false.

static bool MCF5272_uart_init(struct cyg_devtab_entry * tab)
{
    serial_channel *chan = (serial_channel *) tab->priv;
    MCF5272_uart_info_t *port = (MCF5272_uart_info_t *) chan->dev_priv;
    int priority_level = 0;


#ifdef CYGPKG_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL0

    // Instantiation of the UART channel 0 data structure. This data
    // structure contains channel information.
    if (strcmp(tab->name, CYGDAT_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL0_NAME) 
        == 0)
    {

        cyg_uint32 pbcnt;
        
        // A priority makes sense only if interrupts are enabled
#ifdef CYGNUM_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL0_PRIORITY
        priority_level = CYGNUM_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL0_PRIORITY;
#else
        priority_level = 0;
#endif

        // Initialize the UART information data to all zeros
        memset(port, sizeof(MCF5272_uart_info_t), 0);

        // Set the base address of the UART registers to differentiate
        // itself from the different registers for the other UART port.
        port->base = (mcf5272_uart_t *) &MCF5272_DEVS->uart[0];

        // Set the UART interrupt vector number
        port->uart_vector = CYGNUM_HAL_INTERRUPT_UART1;

#ifdef CYGOPT_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL0_AUTOBAUD

        // Set the autobaud state to idle
        port->autobaud_state = AB_IDLE;
#else
        // Disable autobaud detection for this port
        port->autobaud_state = AB_DISABLED;
#endif

        // Initialize the UART 0 output pins
        HAL_READ_UINT32(&MCF5272_DEVS->gpio.pbcnt, pbcnt);
        HAL_WRITE_UINT32(&MCF5272_DEVS->gpio.pbcnt, 
                         MCF5272_GPIO_PBCNT_URT0_EN |
                         (pbcnt & ~MCF5272_GPIO_PBCNT_URT0_MSK));
    }
#endif // CYGPKG_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL0

#ifdef CYGPKG_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL1

    // Instantiation of the UART channel 1 data strucutre. This data
    // structure contains channel information.
    if (strcmp(tab->name, CYGDAT_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL1_NAME) 
        == 0)
    {
        cyg_uint32 pdcnt;
        
        // A priority makes sense only if interrupts are enabled
#ifdef CYGNUM_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL1_PRIORITY
        priority_level = CYGNUM_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL1_PRIORITY;
#else
        priority_level = 0;
#endif

        // Initialize the UART information data to all zeros
        memset(port, sizeof(MCF5272_uart_info_t), 0);

        // Set the base address of the UART registers to differentiate
        // itself from the different regusters for the other UART port.
        port->base = (mcf5272_uart_t *) &MCF5272_DEVS->uart[1];

        // Set the UART interrupt vector number
        port->uart_vector = CYGNUM_HAL_INTERRUPT_UART2;

#ifdef CYGOPT_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL1_AUTOBAUD

        // Set the autobaud state to idle
        port->autobaud_state = AB_IDLE;
#else
        // Disable autobaud detection for this port
        port->autobaud_state = AB_DISABLED;
#endif

        // Initialize the UART 1 output pins
        HAL_READ_UINT32(&MCF5272_DEVS->gpio.pdcnt, pdcnt);
        HAL_WRITE_UINT32(&MCF5272_DEVS->gpio.pdcnt, 
                         MCF5272_GPIO_PDCNT_URT1_EN |
                         (pdcnt & ~MCF5272_GPIO_PDCNT_URT1_MSK));

    }
#endif // CYGPKG_IO_SERIAL_COLDFIRE_MCF5272_CHANNEL1


    if (chan->out_cbuf.len > 0)
    {
        // If the the buffer is greater than zero, then the driver will
        // use interrupt driven  I/O. Hence, the driver creates an
        // interrupt context for the UART device.

        cyg_drv_interrupt_create(port->uart_vector,
                                 priority_level,           // Priority
                                 (cyg_addrword_t)chan,     // Data item passed
                                                       // to interrupt handler
                                 MCF5272_uart_ISR,
                                 MCF5272_uart_DSR,
                                 &port->serial_interrupt_handle,
                                 &port->serial_interrupt);

        cyg_drv_interrupt_attach(port->serial_interrupt_handle);

        cyg_drv_interrupt_unmask(port->uart_vector);
    }

    // Really only required for interrupt driven devices
    (chan->callbacks->serial_init)(chan);

#ifdef CYGDBG_IO_INIT
    diag_printf("MCF5272 UART init - dev: %p.%d\n", port->base,
        port->uart_vector);
#endif
    
    // Configure Serial device
    return (MCF5272_uart_config_port(chan, &chan->config));
}


// ***************************************************************************
// MCF5272_uart_config_port() - Configure the UART port.
//
// Internal function to actually configure the hardware to desired baud rate,
// etc.
//
// INPUT:
//    chan        - The channel information.
//    new_confg   - The port configuration which include the desired
//                  baud rate, etc.
//
// RETURN:
//    Returns true if the port configuration is successful. Otherwise,
//    it retuns false.

static bool MCF5272_uart_config_port(serial_channel *chan,
                                     cyg_serial_info_t *new_config)
{
    MCF5272_uart_info_t *port = (MCF5272_uart_info_t *) chan->dev_priv;
    cyg_uint8 mode_reg = 0;
    cyg_uint32 ubgs;


    // If we are configuring the port once again, disable all interrupts
    HAL_WRITE_UINT8(&port->base->uisr_uimr, 0);

    // If the baud rate is null, we don't configure the port
    if (new_config->baud == 0) return false;
    
    // Get the  divider  from  the  baudrate  table  which  will  use  to
    // configure the port's baud rate.
    ubgs = (cyg_uint16) dividers_table[new_config->baud];

    // Save the configuration value for later use
    port->config = *new_config;

    // We first write the reset values into the device and then configure
    // the device the way we want to use it.
    
    // Reset Transmitter
    HAL_WRITE_UINT8(&port->base->ucr, MCF5272_UART_UCR_RESET_TX);

    // Reset Receiver
    HAL_WRITE_UINT8(&port->base->ucr, MCF5272_UART_UCR_RESET_RX);

    // Reset Mode Register
    HAL_WRITE_UINT8(&port->base->ucr, MCF5272_UART_UCR_RESET_MR);

    // Translate the parity configuration to UART mode bits
    switch(port->config.parity)
    {
        default:
        case CYGNUM_SERIAL_PARITY_NONE:
            mode_reg = 0 | MCF5272_UART_UMR1_PM_NONE;
            break;
            
        case CYGNUM_SERIAL_PARITY_EVEN:
            mode_reg = 0 | MCF5272_UART_UMR1_PM_EVEN;
            break;
        
        case CYGNUM_SERIAL_PARITY_ODD:
            mode_reg = 0 | MCF5272_UART_UMR1_PM_ODD;
            break;
        
        case CYGNUM_SERIAL_PARITY_MARK:
            mode_reg = 0 | MCF5272_UART_UMR1_PM_FORCE_HI;
            break;
        
        case CYGNUM_SERIAL_PARITY_SPACE:
            mode_reg = 0 | MCF5272_UART_UMR1_PM_FORCE_LO;
            break;
    }

    // Translate the number of bits per character configuration to
    // UART mode bits
    switch(port->config.word_length)
    {
        case CYGNUM_SERIAL_WORD_LENGTH_5:
            mode_reg |= MCF5272_UART_UMR1_BC_5;
            break;
            
        case CYGNUM_SERIAL_WORD_LENGTH_6:
            mode_reg |= MCF5272_UART_UMR1_BC_6;
            break;
            
        case CYGNUM_SERIAL_WORD_LENGTH_7:
            mode_reg |= MCF5272_UART_UMR1_BC_7;
            break;
            
        default:
        case CYGNUM_SERIAL_WORD_LENGTH_8:
            mode_reg |= MCF5272_UART_UMR1_BC_8;
            break;
    }

    // Enable HW flow control for receiver
    if(port->config.flags & CYGNUM_SERIAL_FLOW_RTSCTS_RX)
        mode_reg |= MCF5272_UART_UMR1_RXRTS;
    
    // Configure the parity, HW flow control and the bits per character.
    // After this write MR pointer points to mode register 2.
    HAL_WRITE_UINT8(&port->base->umr, mode_reg);

    // Translate the stop bit length to UART mode bits
    switch(port->config.stop)
    {
        default:
        case CYGNUM_SERIAL_STOP_1:
            mode_reg = MCF5272_UART_UMR2_STOP_BITS_1;
            break;
            
        case CYGNUM_SERIAL_STOP_1_5:
            mode_reg = MCF5272_UART_UMR2_STOP_BITS_15;
            break;
            
        case CYGNUM_SERIAL_STOP_2:
            mode_reg = MCF5272_UART_UMR2_STOP_BITS_2;
            break;
    }
    
    // Enable HW flow control for transmitter
    if(port->config.flags & CYGNUM_SERIAL_FLOW_RTSCTS_TX)
        mode_reg |= MCF5272_UART_UMR2_TXCTS;

    // No echo or loopback
    mode_reg |= MCF5272_UART_UMR2_CM_NORMAL;
    
    // Write to mode register 2
    HAL_WRITE_UINT8(&port->base->umr, mode_reg);

    // Set Rx and Tx baud by timer
    HAL_WRITE_UINT8(&port->base->usr_ucsr, 0 | MCF5272_UART_UCSR_RCS(0xD) |
                       MCF5272_UART_UCSR_TCS(0xD));

    // Mask all UART interrupts
    HAL_WRITE_UINT8(&port->base->uisr_uimr, 0);

    // Program the baud settings to the device
    HAL_WRITE_UINT8(&port->base->udu, (cyg_uint8)((ubgs & 0xFF00) >> 8));
    HAL_WRITE_UINT8(&port->base->udl, (cyg_uint8)(ubgs & 0x00FF));

    // Enable receiver and transmitter
    HAL_WRITE_UINT8(&port->base->ucr, 0 | MCF5272_UART_UCR_TXRXEN);

    // Enable both transmit and receive interrupt
    port->imr_mirror = MCF5272_UART_UIMR_TXRDY | MCF5272_UART_UIMR_FFULL_RXRDY;
    
    // Enable break interrupt only if autobaud is enabled
    if (port->autobaud_state != AB_DISABLED)
        port->imr_mirror |= MCF5272_UART_UIMR_DB;
        
    HAL_WRITE_UINT8(&port->base->uisr_uimr, port->imr_mirror);

    // Return true to indicate a successful configuration
    return true;
}


// ***************************************************************************
// MCF5272_uart_lookup() - This routine is called when the device is "looked"
//                         up (i.e. attached)
//
// INPUT:
//    tab - pointer to a pointer of the device table.
//    sub_tab - Pointer to the sub device table.
//    name - name of the device.
//
// RETURN:
//    Always return ENOERR.

static Cyg_ErrNo MCF5272_uart_lookup(struct cyg_devtab_entry **tab,
                                     struct cyg_devtab_entry *sub_tab,
                                     const char *name)
{
    serial_channel *chan = (serial_channel *)(*tab)->priv;

    // Really only required for interrupt driven devices
    (chan->callbacks->serial_init)(chan);
    return ENOERR;
}


// ***************************************************************************
// MCF5272_uart_putc() - Send a character to the device output buffer.
//
// INPUT:
//    chan - pointer to the serial private data.
//    c    - the character to output.
//
// RETURN:
//    'true' if character is sent to device, return 'false' when we've
//    ran out of buffer space in the device itself.

static bool MCF5272_uart_putc(serial_channel *chan, unsigned char c)
{
    MCF5272_uart_info_t *port = (MCF5272_uart_info_t *) chan->dev_priv;
    cyg_uint8 usr_ucsr;

    // Make sure the transmitter is not full. If it is full, return false.
    HAL_READ_UINT8(&port->base->usr_ucsr, usr_ucsr);
    if (!(usr_ucsr & MCF5272_UART_USR_TXRDY))
        return false;

    // Send the character
    HAL_WRITE_UINT8(&port->base->urb_utb, c);

    return true ;
}


// ***************************************************************************
// MCF5272_uart_getc() - Fetch a character from the device input buffer and
//                       return it to the calling routine. Wait until there
//                       is a character ready.
//
// INPUT:
//    chan - pointer to the serial private data.
//
// RETURN:
//    the character read from the UART.

static unsigned char MCF5272_uart_getc(serial_channel *chan)
{
    MCF5272_uart_info_t *port = (MCF5272_uart_info_t *) chan->dev_priv;
    cyg_uint8 usr_ucsr, urb_utb;

    // Wait until character has been received
    do
    {
        HAL_READ_UINT8(&port->base->usr_ucsr, usr_ucsr);
    }
    while (!(usr_ucsr & MCF5272_UART_USR_RXRDY)) ;

    // Read the character from the FIFO queue
    HAL_READ_UINT8(&port->base->urb_utb, urb_utb);

    return urb_utb;
}


// ***************************************************************************
// MCF5272_uart_set_config() - Set up the device characteristics; baud rate,
//                             etc.
//
// INPUT:
//    chan - pointer to the serial private data.
//    key  - configuration key (command).
//    xbuf - pointer to the configuration buffer.
//    len  - the length of the configuration buffer.
//
// RETURN:
//    NOERR - If the configuration is successful.
//    EINVAL -  If the argument is invalid.

Cyg_ErrNo MCF5272_uart_set_config(serial_channel *chan,
                                  cyg_uint32 key,
                                  const void *xbuf,
                                  cyg_uint32 *len)
{
    cyg_serial_info_t *config = (cyg_serial_info_t *) xbuf;
    MCF5272_uart_info_t *port = (MCF5272_uart_info_t *) chan->dev_priv;

    switch (key)
    {
        case CYG_IO_SET_CONFIG_SERIAL_INFO:
        {
            // Set serial configuration
            if (*len < sizeof(cyg_serial_info_t))
                return EINVAL;
          
            *len = sizeof(cyg_serial_info_t);

            if (!MCF5272_uart_config_port(chan, config))
                return EINVAL;
        }
        break;

        case CYG_IO_GET_CONFIG_SERIAL_INFO:
            // Retrieve UART configuration
            *config = port->config;
            break;
            
#ifdef CYGOPT_IO_SERIAL_FLOW_CONTROL_HW
        case CYG_IO_SET_CONFIG_SERIAL_HW_RX_FLOW_THROTTLE:
        {
            cyg_uint32 *f = (cyg_uint32 *)xbuf;

            if (*len < sizeof(*f))
                return -EINVAL;
          
            // we should throttle
            if (*f) HAL_WRITE_UINT8(&port->base->uop0, MCF5272_UART_UOP0_RTS);
            // we should no longer throttle
            else HAL_WRITE_UINT8(&port->base->uop1, MCF5272_UART_UOP1_RTS);
        }
        break;

        case CYG_IO_SET_CONFIG_SERIAL_HW_FLOW_CONFIG:
        // We only support RTSCTS (and software) flow control.
        // We clear any unsupported flags here and
        // then return -ENOSUPP - the higher layer can then query
        // what flags are set and decide what to do.
        {
            unsigned int flags_mask;
            cyg_uint8 umr1_mask, umr2_mask;

            // These are the control flow modes we support
            flags_mask = (CYGNUM_SERIAL_FLOW_RTSCTS_RX | 
                          CYGNUM_SERIAL_FLOW_RTSCTS_RX);
#ifdef CYGOPT_IO_SERIAL_FLOW_CONTROL_SOFTWARE
            flags_mask |= (CYGNUM_SERIAL_FLOW_XONXOFF_RX | 
                           CYGNUM_SERIAL_FLOW_XONXOFF_TX);
#endif          
            if (chan->config.flags & ~flags_mask)
            {
                chan->config.flags &= flags_mask;
                return -ENOSUPP;
            }
          
            // For security, mask UART interrupt while we change configuration
            cyg_drv_interrupt_mask(port->uart_vector);

            // Reset mode register pointer
            HAL_WRITE_UINT8(&port->base->ucr, MCF5272_UART_UCR_RESET_MR);
          
            // Read mode register 1
            HAL_READ_UINT8(&port->base->umr, umr1_mask);

            // Read mode register 2
            HAL_READ_UINT8(&port->base->umr, umr2_mask);

            if (chan->config.flags & CYGNUM_SERIAL_FLOW_RTSCTS_RX)
                umr1_mask |= MCF5272_UART_UMR1_RXRTS;
            else umr1_mask &= ~MCF5272_UART_UMR1_RXRTS;
          
            if (chan->config.flags & CYGNUM_SERIAL_FLOW_RTSCTS_TX)
                umr2_mask |= MCF5272_UART_UMR2_TXCTS;
            else umr2_mask &= ~MCF5272_UART_UMR2_TXCTS;

            // Reset mode register pointer
            HAL_WRITE_UINT8(&port->base->ucr, MCF5272_UART_UCR_RESET_MR);
          
            // Write mode register 1
            HAL_WRITE_UINT8(&port->base->umr, umr1_mask);

            // Write mode register 2
            HAL_WRITE_UINT8(&port->base->umr, umr2_mask);

            // Unmask UART interrupt
            cyg_drv_interrupt_unmask(port->uart_vector);
        }
        break;
#endif // CYGOPT_IO_SERIAL_FLOW_CONTROL_HW

        default:
            return EINVAL;
    }
    
    return ENOERR;
}


// ***************************************************************************
//  MCF5272_uart_start_xmit() - Enable the transmitter on the device.
//
//  INPUT:
//    chan - pointer to the serial private data.

static void MCF5272_uart_start_xmit(serial_channel *chan)
{
    MCF5272_uart_info_t *port = (MCF5272_uart_info_t *) chan->dev_priv;

    
    // Mask UART interrupt to prevent race conditions
    cyg_drv_interrupt_mask(port->uart_vector);
    
    // Enable the UART transmitter.
    // Eventually, preserve the ongoing autobaud calculation.
#ifdef REQUESTED_AUTOBAUD
    if(port->autobaud_state == AB_BEGIN)
      HAL_WRITE_UINT8(&port->base->ucr, (MCF5272_UART_UCR_TX_ENABLED | 
                                         MCF5272_UART_UCR_ENAB));
    else
#endif
    {
        HAL_WRITE_UINT8(&port->base->ucr, MCF5272_UART_UCR_TX_ENABLED);
    }
    
    // Enable transmitter interrupt
    port->imr_mirror |= MCF5272_UART_UIMR_TXRDY;
    HAL_WRITE_UINT8(&port->base->uisr_uimr, port->imr_mirror);

    // Unmask UART interrupt
    cyg_drv_interrupt_unmask(port->uart_vector);
}


// ***************************************************************************
// MCF5272_uart_stop_xmit() - Disable the transmitter on the device
//
// INPUT:
//    chan - pointer to the serial private data.

static void MCF5272_uart_stop_xmit(serial_channel * chan)
{   
    MCF5272_uart_info_t *port = (MCF5272_uart_info_t *) chan->dev_priv;

    
    // Mask UART interrupt to prevent race conditions
    cyg_drv_interrupt_mask(port->uart_vector);

    // Disable transmitter interrupt
    port->imr_mirror &= ~MCF5272_UART_UIMR_TXRDY;
    HAL_WRITE_UINT8(&port->base->uisr_uimr, port->imr_mirror);

    // Disable the UART transmitter.
    // Eventually, preserve the ongoing autobaud calculation.
    //   !!!!!!!!!!!!!
    //   !!!WARNING!!!
    //   !!!!!!!!!!!!!
    //   If the transmitter is disabled
    //   the diag_printf routines will poll forever to transmit the
    //   a character. Hence, don't ever disable the transmitter if
    //   you want it to work with diag_printf.
#ifdef REQUESTED_AUTOBAUD
    if(port->autobaud_state == AB_BEGIN)
      HAL_WRITE_UINT8(&port->base->ucr, (MCF5272_UART_UCR_TX_DISABLED | 
                                         MCF5272_UART_UCR_ENAB));
    else
#endif
    {
        HAL_WRITE_UINT8(&port->base->ucr, MCF5272_UART_UCR_TX_DISABLED);
    }
    
    // Unmask UART interrupt
    cyg_drv_interrupt_unmask(port->uart_vector);
}


// ***************************************************************************
// MCF5272_uart_ISR() - UART I/O interrupt interrupt service routine (ISR).
//
// INPUT:
//    vector - the interrupt vector number.
//    data   - user parameter.
//
// RETURN:
//     returns CYG_ISR_CALL_DSR to call the DSR.

static cyg_uint32 MCF5272_uart_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *) data;
    MCF5272_uart_info_t *port = (MCF5272_uart_info_t *) chan->dev_priv;


    // Write the value in the interrupt status register back
    // to the mask register to disable the interrupt temporarily.
    HAL_WRITE_UINT8(&port->base->uisr_uimr, 0);

    // Cause DSR to run
    return CYG_ISR_CALL_DSR;
}


// ***************************************************************************
// MCF5272_uart_DSR() - Defered Service Routine (DSR) - This routine processes
//                      the interrupt from the device.
//
// INPUT:
//    vector - The interrupt vector number.
//    count  - The nunber of DSR requests.
//    data   - Device specific information.
//
// The autobaud feature is implemented by means of a simple finite state
// machine which can take the following states:
// AB_DISABLED: autobaud is disabled.
// AB_IDLE: no autobaud calculation is in progress. If autobaud calculation
//          has completed, retrieve the new baud rate.
// AB_BEGIN_BREAK: the start of a break character was detected.
// AB_BEGIN: the end of a break character was detected. Start autobaud
//           calculation.
//
// The state diagram is the following:
// AB_IDLE --> AB_BEGIN_BREAK --> AB_BEGIN --> Back to AB_IDLE
// The state AB_DISABLED is isolated and means that the autobaud feature is
// not active for that port.

static void MCF5272_uart_DSR(cyg_vector_t vector, cyg_ucount32 count,
                             cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *) data;
    MCF5272_uart_info_t *port = (MCF5272_uart_info_t *) chan->dev_priv;
    volatile cyg_uint8 isr;
    cyg_uint8 uisr_uimr;


    while (1)
    {
        // First of all, the exit condition
        
        // Retrieve the interrupt status bits. We use these status bits to
        // figure out what process should we perform: read from the UART or
        // inform of a completion of a data transmission.
        HAL_READ_UINT8(&port->base->uisr_uimr, uisr_uimr);
        isr = uisr_uimr & port->imr_mirror;
        
        // If there are no more events pending, exit the loop
        if (!isr) break;
        
#ifdef REQUESTED_AUTOBAUD
        switch (port->autobaud_state)
        {
            case AB_DISABLED:
                // Nothing to check for
                break;

            case AB_BEGIN_BREAK:
                if (isr & MCF5272_UART_UISR_DB)
                {
                    // Detected the end of a break, set the state to
                    // AB_BEGIN, and setup autobaud detection.
                    port->autobaud_state = AB_BEGIN;

                    // Initialize divider
                    HAL_WRITE_UINT8(&port->base->udu, 0);
                    HAL_WRITE_UINT8(&port->base->udl, 0);

                    // Reset the Delta Break bit in the UISR and
                    //Enable autobaud
                    HAL_WRITE_UINT8(&port->base->ucr, 
                                    MCF5272_UART_UCR_RESET_BKCHGINT);

                    HAL_WRITE_UINT8(&port->base->ucr, MCF5272_UART_UCR_ENAB);

                    // Enable autobaud completion interrupt
                    port->imr_mirror |= MCF5272_UART_UIMR_ABC;

                    // Disable the delta break interrupt so we can't receive
                    // anymore break interrupt.
                    port->imr_mirror &= ~MCF5272_UART_UIMR_DB;

                }
                break;

            case AB_BEGIN:
                if (isr & MCF5272_UART_UISR_ABC)
                {
                    int count;
                    unsigned int threshold;
                    cyg_uint8 uabu, uabl;
               
                    // Retrieve the detected baud rate
                    HAL_READ_UINT8(&port->base->uabu, uabu);
                    HAL_READ_UINT8(&port->base->uabl, uabl);
                    
                    cyg_uint16 divider = (uabu << 8) + uabl;

                    // Search in the list to find a match
                    for (count = sizeof(dividers_table)/
                           sizeof(unsigned long) - 1;
                            count > 1; count--)
                    {
                        if (divider < dividers_table[count - 1]) break;
                    }

                    // Modify baud rate only if it is in range
                    if (count > 1)
                    {
                        // Set the baud rate to the nearest standard rate
                        threshold = (dividers_table[count] + 
                                     dividers_table[count - 1]) / 2;
                        port->config.baud = (divider < threshold) ? count : 
                          count - 1;
                    }

                    divider = dividers_table[port->config.baud];
                   
                    // Program the baud settings to the device
                    HAL_WRITE_UINT8(&port->base->udu, 
                                    (cyg_uint8)((divider & 0xFF00) >> 8));
                    HAL_WRITE_UINT8(&port->base->udl, 
                                    (cyg_uint8)(divider & 0x00FF));

                    // Autobaud completion
                    port->autobaud_state = AB_IDLE;

                    // Disable autobaud
                    HAL_WRITE_UINT8(&port->base->ucr, MCF5272_UART_UCR_NONE);

#if 0
                    // In case patch submitted July 11, 2005 gets committed
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
                    // Inform upper layers of the new baud rate
                    {
                        cyg_serial_line_status_t stat;
                        
                        stat.which = CYGNUM_SERIAL_STATUS_NEWBAUDRATE;
                        stat.value = port->config.baud;
                        
                        (chan->callbacks->indicate_status)(chan, &stat);
                    }
#endif
#endif
                    // Ignore autobaud completion interrupt
                    port->imr_mirror &= ~MCF5272_UART_UIMR_ABC;

                    // Reenable delta break interrupt
                    port->imr_mirror |= MCF5272_UART_UIMR_DB;

                }
                break;

            default:
            case AB_IDLE:
                if (isr & MCF5272_UART_UISR_DB)
                {
                    // Detected the begin of a break, set the state to
                    // AB_BEGIN_BREAK
                    port->autobaud_state = AB_BEGIN_BREAK;

                    // Reset the delta break bit in the UISR
                    HAL_WRITE_UINT8(&port->base->ucr, 
                                    MCF5272_UART_UCR_RESET_BKCHGINT);
                }
                break;
        }
#endif // REQUESTED_AUTOBAUD

        // Receive character interrupt
        if ((isr & MCF5272_UART_UISR_RXRDY))
        {
            char c;
            cyg_uint8 usr_ucsr;
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
            cyg_serial_line_status_t stat;
#endif
            
            // Read all the characters in the fifo
            while (1)
            {
                // First of all, the exit condition

                // If there are no more characters waiting, exit the loop
                HAL_READ_UINT8(&port->base->uisr_uimr, uisr_uimr);
                if (!(uisr_uimr & MCF5272_UART_UISR_RXRDY)) break;

                // Read port status
                HAL_READ_UINT8(&port->base->usr_ucsr, usr_ucsr);
                
                // Received break
                if (usr_ucsr & MCF5272_UART_USR_RB)
                {
                    // Ignore break character
                    HAL_READ_UINT8(&port->base->urb_utb, c);
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
                    stat.which = CYGNUM_SERIAL_STATUS_BREAK;
                    (chan->callbacks->indicate_status)(chan, &stat);
#endif
                    continue;
                }

#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
                // Overrun error
                if (usr_ucsr & MCF5272_UART_USR_OE)
                {
                    stat.which = CYGNUM_SERIAL_STATUS_OVERRUNERR;
                    (chan->callbacks->indicate_status)(chan, &stat);
                }
#endif

#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
                // Framing error
                if (usr_ucsr & MCF5272_UART_USR_FE)
                {
                    stat.which = CYGNUM_SERIAL_STATUS_FRAMEERR;
                    (chan->callbacks->indicate_status)(chan, &stat);
                }
#endif

#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
                // Parity error
                if (usr_ucsr & MCF5272_UART_USR_PE)
                {
                    stat.which = CYGNUM_SERIAL_STATUS_PARITYERR;
                    (chan->callbacks->indicate_status)(chan, &stat);
                }
#endif

                // Read the character from the UART
                HAL_READ_UINT8(&port->base->urb_utb, c);

                // Pass the read character to the upper layer
                (chan->callbacks->rcv_char)(chan, c);
            }
        }

        // Transmit complete interrupt
        if ((isr & MCF5272_UART_UISR_TXRDY))
        {
            // Transmit holding register is empty
            (chan->callbacks->xmt_char)(chan);
        }
    }

    // Unmask all the UART interrupts that were masked in the ISR, so
    // that we can receive the next interrupt.
    HAL_WRITE_UINT8(&port->base->uisr_uimr, port->imr_mirror);
}
