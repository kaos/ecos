//==========================================================================
//
//      hal_diag.c
//
//      HAL diagnostic I/O support routines for the MCF5272
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
// Author(s):    Enrico Piria
// Contributors:
// Date:         2005-25-06
// Purpose:      Code to manage the serial ports for diagnostic output.
// Description:
//
//####DESCRIPTIONEND####
//==========================================================================


#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_intr.h>           // CYGNUM_HAL_INTERRUPT_UART1
#include <cyg/hal/hal_if.h>             // __comm_control_cmd_t
#include <cyg/hal/hal_io.h>             // HAL I/O macros
#include <cyg/hal/hal_misc.h>           // cyg_hal_is_break
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED

#include <cyg/hal/plf_serial.h>


typedef struct {
    CYG_ADDRWORD                base;           // [Pointer] to Port base address
    unsigned int                vector;         // UART interrupt vector
    CYG_WORD                    msec_timeout;   // Timeout in msec
} channel_data_t;

static channel_data_t ports[] = {
    {
        (CYG_ADDRWORD) (&((mcf5272_devs_t *) 0)->uart[0]),
        CYGNUM_HAL_INTERRUPT_UART1,
        1000
    },

    {
        (CYG_ADDRWORD) (&((mcf5272_devs_t *) 0)->uart[1]),
        CYGNUM_HAL_INTERRUPT_UART2,
        1000
    }
};


static cyg_uint8
cyg_hal_plf_serial_getc(void* __ch_data);

static void
cyg_hal_plf_serial_putc(void* __ch_data, cyg_uint8 ch);

static void
cyg_hal_plf_serial_init_channel(channel_data_t *port);

static cyg_bool
cyg_hal_plf_serial_getc_nonblock(void* __ch_data, cyg_uint8* ch);

static cyg_bool
cyg_hal_plf_serial_getc_timeout(void* __ch_data, cyg_uint8* ch);

static void
cyg_hal_plf_serial_write(void* __ch_data, const cyg_uint8* __buf, 
                         cyg_uint32 __len);
static void
cyg_hal_plf_serial_read(void* __ch_data, cyg_uint8* __buf, cyg_uint32 __len);

static int
cyg_hal_plf_serial_isr(void *__ch_data, int* __ctrlc, 
                       CYG_ADDRWORD __vector, CYG_ADDRWORD __data);
static int
cyg_hal_plf_serial_control(void *__ch_data, __comm_control_cmd_t __func, ...);


// ----------------------------------------------------------------------------
// Early initialization of comm. channels.
void
cyg_hal_plf_comms_init(void)
{
    hal_virtual_comm_table_t* comm;
    int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
    cyg_uint32 portcnt;
    static int initialized = 0;

    if (initialized)
        return;
    initialized = 1;

#if (defined(CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL) && \
        CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL == 0) || \
    (defined(CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL) && \
        CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL == 0)
    
    // UART0 pins are multplexed with GPIO port B. Enable them in
    // port B control register.
    HAL_READ_UINT32(&MCF5272_DEVS->gpio.pbcnt, portcnt);
    HAL_WRITE_UINT32(&MCF5272_DEVS->gpio.pbcnt, ((portcnt &
        ~(MCF5272_GPIO_PBCNT_URT0_MSK)) |
        (MCF5272_GPIO_PBCNT_URT0_EN)));

    // Init channel 0
    cyg_hal_plf_serial_init_channel(&ports[0]);

    // Setup procs in the vector table for channel 0
    CYGACC_CALL_IF_SET_CONSOLE_COMM(0);
    comm = CYGACC_CALL_IF_CONSOLE_PROCS();
    CYGACC_COMM_IF_CH_DATA_SET(*comm, &ports[0]);
    CYGACC_COMM_IF_WRITE_SET(*comm, cyg_hal_plf_serial_write);
    CYGACC_COMM_IF_READ_SET(*comm, cyg_hal_plf_serial_read);
    CYGACC_COMM_IF_PUTC_SET(*comm, cyg_hal_plf_serial_putc);
    CYGACC_COMM_IF_GETC_SET(*comm, cyg_hal_plf_serial_getc);
    CYGACC_COMM_IF_CONTROL_SET(*comm, cyg_hal_plf_serial_control);
    CYGACC_COMM_IF_DBG_ISR_SET(*comm, cyg_hal_plf_serial_isr);
    CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, cyg_hal_plf_serial_getc_timeout);
#endif

#if (defined(CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL) && \
        CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL == 1) || \
    (defined(CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL) && \
        CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL == 1)

    // UART1 pins need to be enabled in port D control register.
    HAL_READ_UINT32(&MCF5272_DEVS->gpio.pdcnt, portcnt);
    HAL_WRITE_UINT32(&MCF5272_DEVS->gpio.pdcnt, ((portcnt &
        ~(MCF5272_GPIO_PDCNT_URT1_MSK)) |
        (MCF5272_GPIO_PDCNT_URT1_EN)));

    // Init channel 1
    cyg_hal_plf_serial_init_channel(&ports[1]);

    // Setup procs in the vector table for channel 0
    CYGACC_CALL_IF_SET_CONSOLE_COMM(1);
    comm = CYGACC_CALL_IF_CONSOLE_PROCS();
    CYGACC_COMM_IF_CH_DATA_SET(*comm, &ports[1]);
    CYGACC_COMM_IF_WRITE_SET(*comm, cyg_hal_plf_serial_write);
    CYGACC_COMM_IF_READ_SET(*comm, cyg_hal_plf_serial_read);
    CYGACC_COMM_IF_PUTC_SET(*comm, cyg_hal_plf_serial_putc);
    CYGACC_COMM_IF_GETC_SET(*comm, cyg_hal_plf_serial_getc);
    CYGACC_COMM_IF_CONTROL_SET(*comm, cyg_hal_plf_serial_control);
    CYGACC_COMM_IF_DBG_ISR_SET(*comm, cyg_hal_plf_serial_isr);
    CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, cyg_hal_plf_serial_getc_timeout);
#endif

    // Restore original console
    CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);
}


void cyg_hal_plf_serial_init_channel(channel_data_t *port)
{
    CYG_WORD16 clk_div;
    volatile mcf5272_uart_t *base =
        (volatile mcf5272_uart_t *)((char *)MCF5272_DEVS + port->base);
    // Initialize variable to prevent compiler warnings
    unsigned int baud_rate = 1200;


#ifdef CYGPRI_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL_CONFIGURABLE    
    if( (port-&ports[0]) == CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL )
        baud_rate = CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL_BAUD;
#endif    
#ifdef CYGPRI_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_CONFIGURABLE
    if( (port-&ports[0]) == CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL )
        baud_rate = CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD;
#endif

    // Before we do anything else, make sure we have enabled RTS in case
    // the device we are using relies on hardware flow control.
    // Note that this step is our only attempt at hardware flow control.
    HAL_WRITE_UINT8(&base->uop1, MCF5272_UART_UOP1_RTS);

    // Initialize UART
    
    // Reset Transmitter
    HAL_WRITE_UINT8(&base->ucr, MCF5272_UART_UCR_RTX);

    // Reset Receiver
    HAL_WRITE_UINT8(&base->ucr, MCF5272_UART_UCR_RRX);

    // Reset Mode Register
    HAL_WRITE_UINT8(&base->ucr, MCF5272_UART_UCR_RMR);

    HAL_WRITE_UINT8(&base->ucr, MCF5272_UART_UCR_RES);
    HAL_WRITE_UINT8(&base->ucr, MCF5272_UART_UCR_RBC);

    // Mode register 1 sets the UART to  8 data bits with no parity, and
    // mode register 2 forces 1 stop  bit. Also, interrupt generation
    // on RxRDY signal is enabled by default.
    // Reading or write to the mode register switches it from umr1 to umr2.
    // To set it to umr1, we must write a reset mode register command to the
    // command register.
    HAL_WRITE_UINT8(&base->umr, MCF5272_UART_UMR_8BNP);
    HAL_WRITE_UINT8(&base->umr, MCF5272_UART_UMR_1S);

    // Select a prescaled (by 1/16) CLKIN for the clock source
    HAL_WRITE_UINT8(&base->usr_ucsr, MCF5272_UART_UCSR_CLKIN);

    // Calculate baud settings
    clk_div = (CYG_WORD16)
              ((CYGHWR_HAL_SYSTEM_CLOCK_MHZ*1000000)/
               (baud_rate * 32));
    HAL_WRITE_UINT8(&base->udu, clk_div >> 8);
    HAL_WRITE_UINT8(&base->udl, clk_div & 0x00ff);

    // Enable the transmitter and receiver
    HAL_WRITE_UINT8(&base->ucr, MCF5272_UART_UCR_TXRXEN);

    // Set interrupt priority to highest maskable level
    HAL_INTERRUPT_SET_LEVEL(port->vector, 6);
}


cyg_uint8 cyg_hal_plf_serial_getc(void* __ch_data)
{
    cyg_uint8 ch;


    while(!cyg_hal_plf_serial_getc_nonblock(__ch_data, &ch));

    return ch;
}


void cyg_hal_plf_serial_putc(void *__ch_data, cyg_uint8 ch)
{
    channel_data_t *port = (channel_data_t *) __ch_data;
    volatile mcf5272_uart_t *base =
        (volatile mcf5272_uart_t *)((char *)MCF5272_DEVS + port->base);
    cyg_uint8 usr_ucsr, utf;


    // Loop until the transmit data holding register is empty
    do
    {
        HAL_READ_UINT8(&base->usr_ucsr, usr_ucsr);
    } while (!(usr_ucsr & MCF5272_UART_USR_TXRDY));

    // Write the character to the transmit buffer
    HAL_WRITE_UINT8(&base->urb_utb, ch);

    // Loop until the transmit data FIFO and the shift register are empty
    do
    {
        HAL_READ_UINT8(&base->utf, utf);
        HAL_READ_UINT8(&base->usr_ucsr, usr_ucsr);
    } while ((utf & MCF5272_UART_UTF_TXB) ||
        (!(usr_ucsr & MCF5272_UART_USR_TXEMP)));
}

static void cyg_hal_plf_serial_write(void* __ch_data, const cyg_uint8* __buf, 
                                     cyg_uint32 __len)
{
    while(__len-- > 0)
        cyg_hal_plf_serial_putc(__ch_data, *__buf++);
}


static void cyg_hal_plf_serial_read(void* __ch_data, cyg_uint8* __buf,
                                    cyg_uint32 __len)
{
    while(__len-- > 0)
        *__buf++ = cyg_hal_plf_serial_getc(__ch_data);
}


static cyg_bool cyg_hal_plf_serial_getc_nonblock(void* __ch_data, cyg_uint8* ch)
{
    channel_data_t *port = (channel_data_t *) __ch_data;
    volatile mcf5272_uart_t *base =
        (volatile mcf5272_uart_t *)((char *)MCF5272_DEVS + port->base);
    cyg_uint8 usr_ucsr;


    // Read status
    HAL_READ_UINT8(&base->usr_ucsr, usr_ucsr);

    // Check if a character is present
    if (usr_ucsr & MCF5272_UART_USR_RRDY)
    {
        // Read character
        HAL_READ_UINT8(&base->urb_utb, *ch);
        return true;
    }

    return false;
}


static cyg_bool cyg_hal_plf_serial_getc_timeout(void* __ch_data, cyg_uint8* ch)
{
    int delay_count;
    cyg_bool res;


    delay_count = ((channel_data_t *)__ch_data)->msec_timeout;

    for(;;) {
        res = cyg_hal_plf_serial_getc_nonblock(__ch_data, ch);
        if (res || 0 == delay_count--)
            break;
        
        CYGACC_CALL_IF_DELAY_US(100);
    }

    return res;
}


static int cyg_hal_plf_serial_control(void *__ch_data,
                                      __comm_control_cmd_t __func, ...)
{
    static int irq_state = 0;
    channel_data_t *port = (channel_data_t *) __ch_data;
    volatile mcf5272_uart_t *base =
        (volatile mcf5272_uart_t *)((char *)MCF5272_DEVS + port->base);
    int ret = 0;


    switch (__func) {
    case __COMMCTL_IRQ_ENABLE:
        irq_state = 1;

        HAL_WRITE_UINT8(&base->uisr_uimr, MCF5272_UART_UIMR_FFULL);
        HAL_INTERRUPT_UNMASK(port->vector);
        break;
    case __COMMCTL_IRQ_DISABLE:
        ret = irq_state;
        irq_state = 0;

        HAL_INTERRUPT_MASK(port->vector);
        HAL_WRITE_UINT8(&base->uisr_uimr, 0);
        break;
    case __COMMCTL_DBG_ISR_VECTOR:
        ret = port->vector;
        break;
    case __COMMCTL_SET_TIMEOUT:
    {
        va_list ap;

        va_start(ap, __func);

        ret = port->msec_timeout;
        port->msec_timeout = va_arg(ap, cyg_uint32);

        va_end(ap);
    }        
    default:
        break;
    }

    return ret;
}


static int cyg_hal_plf_serial_isr(void *__ch_data, int* __ctrlc, 
                                  CYG_ADDRWORD __vector, CYG_ADDRWORD __data)
{
    channel_data_t *port = (channel_data_t *) __ch_data;
    volatile mcf5272_uart_t *base =
        (volatile mcf5272_uart_t *)((char *)MCF5272_DEVS + port->base);
    char c;


    // Disable the interrupt temporarily
    HAL_WRITE_UINT8(&base->uisr_uimr, 0);

    *__ctrlc = 0;

    // Read character
    HAL_READ_UINT8(&base->urb_utb, c);
    if( cyg_hal_is_break( &c , 1 ) )
    {
        *__ctrlc = 1;
    }

    // Re-enable RxRDY interrupt
    HAL_WRITE_UINT8(&base->uisr_uimr, MCF5272_UART_UIMR_FFULL);
    
    return CYG_ISR_HANDLED;
}
