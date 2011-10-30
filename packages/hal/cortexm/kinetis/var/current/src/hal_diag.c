/*=============================================================================
//
//      hal_diag.c
//
//      HAL diagnostic output code
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2011 Free Software Foundation, Inc.                        
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Ilija Kocho <ilijak@siva.com.mk>
// Contributors:
// Date:        2011-02-04
// Purpose:     HAL diagnostic input/output
// Description: Implementations of HAL diagnostic input/output support.
//
//####DESCRIPTIONEND####
//
//===========================================================================
 */

#include <pkgconf/hal.h>
#include CYGBLD_HAL_PLATFORM_H

#include <cyg/infra/cyg_type.h>         // base types

#include <cyg/hal/hal_arch.h>           // SAVE/RESTORE GP macros
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_if.h>             // interface API
#include <cyg/hal/hal_intr.h>           // HAL_ENABLE/MASK/UNMASK_INTERRUPTS
#include <cyg/hal/hal_misc.h>           // Helper functions
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED
#include <cyg/hal/hal_diag.h>

#include <cyg/hal/var_io.h>             //
#include <cyg/io/ser_freescale_uart.h>  // UART registers

//-----------------------------------------------------------------------------

typedef struct {
    cyg_uint32 uart;
    CYG_ADDRESS base;
    cyg_int32 msec_timeout;
    cyg_int32 isr_vector;
    cyg_uint32 rx_pin;
    cyg_uint32 tx_pin;
    cyg_int32 baud_rate;
    cyg_int32 irq_state;
} channel_data_t;

channel_data_t plf_ser_channels[] = {
#ifdef CYGINT_HAL_FREESCALE_UART0
    { 0, CYGADDR_IO_SERIAL_FREESCALE_UART0_BASE, 1000,
      CYGNUM_HAL_INTERRUPT_UART0_RX_TX,
      CYGHWR_HAL_FREESCALE_UART0_PIN_RX, CYGHWR_HAL_FREESCALE_UART0_PIN_TX,
      CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD },
#endif
#ifdef CYGINT_HAL_FREESCALE_UART1
    { 1, CYGADDR_IO_SERIAL_FREESCALE_UART1_BASE, 1000,
      CYGNUM_HAL_INTERRUPT_UART1_RX_TX,
      CYGHWR_HAL_FREESCALE_UART1_PIN_RX, CYGHWR_HAL_FREESCALE_UART1_PIN_TX,
      CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD },
#endif
#ifdef CYGINT_HAL_FREESCALE_UART2
    { 2, CYGADDR_IO_SERIAL_FREESCALE_UART2_BASE, 1000,
      CYGNUM_HAL_INTERRUPT_UART2_RX_TX,
      CYGHWR_HAL_FREESCALE_UART2_PIN_RX, CYGHWR_HAL_FREESCALE_UART2_PIN_TX,
      CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD },
#endif
#ifdef CYGINT_HAL_FREESCALE_UART3
    { 3, CYGADDR_IO_SERIAL_FREESCALE_UART3_BASE, 1000,
      CYGNUM_HAL_INTERRUPT_UART3_RX_TX,
      CYGHWR_HAL_FREESCALE_UART3_PIN_RX, CYGHWR_HAL_FREESCALE_UART3_PIN_TX,
      CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD },
#endif
#ifdef CYGINT_HAL_FREESCALE_UART4
    { 4, CYGADDR_IO_SERIAL_FREESCALE_UART4_BASE, 1000,
      CYGNUM_HAL_INTERRUPT_UART4_RX_TX,
      CYGHWR_HAL_FREESCALE_UART4_PIN_RX, CYGHWR_HAL_FREESCALE_UART4_PIN_TX,
      CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD },
#endif
#ifdef CYGINT_HAL_FREESCALE_UART5
    { 5, CYGADDR_IO_SERIAL_FREESCALE_UART5_BASE, 1000,
      CYGNUM_HAL_INTERRUPT_UART5_RX_TX,
      CYGHWR_HAL_FREESCALE_UART5_PIN_RX, CYGHWR_HAL_FREESCALE_UART5_PIN_TX,
      CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD }
#endif
};

#if defined(CYGOPT_HAL_KINETIS_DIAG_IN_MISC_FLASH_SECTION) && \
        CYGOPT_HAL_KINETIS_DIAG_IN_MISC_FLASH_SECTION
#  define CYGOPT_HAL_KINETIS_DIAG_FLASH_SECTION_ATTR         \
        CYGOPT_HAL_KINETIS_MISC_FLASH_SECTION_ATTR
#else
#  define CYGOPT_HAL_KINETIS_DIAG_FLASH_SECTION_ATTR
#endif

//-----------------------------------------------------------------------------

void  CYGOPT_HAL_KINETIS_DIAG_FLASH_SECTION_ATTR
cyg_hal_plf_serial_putc(void *__ch_data, char c);


static void  CYGOPT_HAL_KINETIS_DIAG_FLASH_SECTION_ATTR
cyg_hal_plf_serial_init_channel(void* __ch_data)
{
    channel_data_t* chan = (channel_data_t*)__ch_data;
    CYG_ADDRESS uart_p = chan->base;

    // Configure PORT pins
    hal_set_pin_function(chan->rx_pin);
    hal_set_pin_function(chan->tx_pin);

    // 8-1-no parity.
    HAL_WRITE_UINT8(uart_p + CYGHWR_DEV_FREESCALE_UART_C1, 0);
    CYGHWR_IO_FREESCALE_UART_BAUD_SET(uart_p, chan->baud_rate);
    // Enable RX and TX
    HAL_WRITE_UINT8(uart_p + CYGHWR_DEV_FREESCALE_UART_C2,
                    (CYGHWR_DEV_FREESCALE_UART_C2_TE |
                     CYGHWR_DEV_FREESCALE_UART_C2_RE));
}

void  CYGOPT_HAL_KINETIS_DIAG_FLASH_SECTION_ATTR
cyg_hal_plf_serial_putc(void* __ch_data, char ch_out)
{
    channel_data_t* chan = (channel_data_t*)__ch_data;
    CYG_ADDRESS uart_p = (CYG_ADDRESS) chan->base;
    cyg_uint32 uart_s1;

    CYGARC_HAL_SAVE_GP();

    do {
        HAL_READ_UINT8(uart_p + CYGHWR_DEV_FREESCALE_UART_S1, uart_s1);
    } while (!(uart_s1 & CYGHWR_DEV_FREESCALE_UART_S1_TDRE));

    HAL_WRITE_UINT8(uart_p + CYGHWR_DEV_FREESCALE_UART_D, ch_out);

    CYGARC_HAL_RESTORE_GP();
}

static cyg_bool CYGOPT_HAL_KINETIS_DIAG_FLASH_SECTION_ATTR
cyg_hal_plf_serial_getc_nonblock(void* __ch_data, cyg_uint8* p_ch_in)
{
    channel_data_t* chan = (channel_data_t*)__ch_data;
    CYG_ADDRESS uart_p = (CYG_ADDRESS) chan->base;
    cyg_uint8 uart_s1;
    cyg_uint8 ch_in;

    HAL_READ_UINT8(uart_p + CYGHWR_DEV_FREESCALE_UART_S1, uart_s1);
    if (!(uart_s1 & CYGHWR_DEV_FREESCALE_UART_S1_RDRF))
        return false;

    HAL_READ_UINT8(uart_p + CYGHWR_DEV_FREESCALE_UART_D, ch_in);
    *p_ch_in = ch_in;

    return true;
}

cyg_uint8 CYGOPT_HAL_KINETIS_DIAG_FLASH_SECTION_ATTR
cyg_hal_plf_serial_getc(void* __ch_data)
{
    cyg_uint8 ch;
    CYGARC_HAL_SAVE_GP();

    while(!cyg_hal_plf_serial_getc_nonblock(__ch_data, &ch));

    CYGARC_HAL_RESTORE_GP();
    return ch;
}


//=============================================================================
// Virtual vector HAL diagnostics

#if defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)

static void CYGOPT_HAL_KINETIS_DIAG_FLASH_SECTION_ATTR
cyg_hal_plf_serial_write(void* __ch_data, const cyg_uint8* __buf,
                         cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        cyg_hal_plf_serial_putc(__ch_data, *__buf++);

    CYGARC_HAL_RESTORE_GP();
}

static void CYGOPT_HAL_KINETIS_DIAG_FLASH_SECTION_ATTR
cyg_hal_plf_serial_read(void* __ch_data, cyg_uint8* __buf, cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        *__buf++ = cyg_hal_plf_serial_getc(__ch_data);

    CYGARC_HAL_RESTORE_GP();
}

cyg_bool CYGOPT_HAL_KINETIS_DIAG_FLASH_SECTION_ATTR
cyg_hal_plf_serial_getc_timeout(void* __ch_data, cyg_uint8* p_ch_in)
{
    int delay_count;
    cyg_bool res;
    CYGARC_HAL_SAVE_GP();

    // delay in .1 ms steps
    delay_count = ((channel_data_t*)__ch_data)->msec_timeout * 10;

    for(;;) {
        res = cyg_hal_plf_serial_getc_nonblock(__ch_data, p_ch_in);
        if (res || 0 == delay_count--)
            break;

        CYGACC_CALL_IF_DELAY_US(100);
    }

    CYGARC_HAL_RESTORE_GP();
    return res;
}

static int CYGOPT_HAL_KINETIS_DIAG_FLASH_SECTION_ATTR
cyg_hal_plf_serial_control(void *__ch_data, __comm_control_cmd_t __func, ...)
{
    channel_data_t* chan = (channel_data_t*)__ch_data;
    CYG_ADDRESS uart_p = ((channel_data_t*)__ch_data)->base;
    cyg_uint8 ser_port_reg;
    int ret = 0;
    va_list ap;

    CYGARC_HAL_SAVE_GP();
    va_start(ap, __func);

    switch (__func) {
    case __COMMCTL_IRQ_ENABLE:
        chan->irq_state = 1;
        HAL_INTERRUPT_ACKNOWLEDGE(chan->isr_vector);
        HAL_INTERRUPT_UNMASK(chan->isr_vector);

        HAL_READ_UINT8(uart_p + CYGHWR_DEV_FREESCALE_UART_C2, ser_port_reg);
        ser_port_reg |= CYGHWR_DEV_FREESCALE_UART_C2_RIE;
        HAL_WRITE_UINT8(uart_p + CYGHWR_DEV_FREESCALE_UART_C2, ser_port_reg);

        break;
    case __COMMCTL_IRQ_DISABLE:
        ret = chan->irq_state;
        chan->irq_state = 0;
        HAL_INTERRUPT_MASK(chan->isr_vector);

        HAL_READ_UINT8(uart_p + CYGHWR_DEV_FREESCALE_UART_C2, ser_port_reg);
        ser_port_reg &= ~(cyg_uint8)CYGHWR_DEV_FREESCALE_UART_C2_RIE;
        HAL_WRITE_UINT8(uart_p + CYGHWR_DEV_FREESCALE_UART_C2, ser_port_reg);
        break;
    case __COMMCTL_DBG_ISR_VECTOR:
        ret = chan->isr_vector;
        break;
    case __COMMCTL_SET_TIMEOUT:
        ret = chan->msec_timeout;
        chan->msec_timeout = va_arg(ap, cyg_uint32);
    case __COMMCTL_GETBAUD:
        ret = chan->baud_rate;
        break;
    case __COMMCTL_SETBAUD:
        chan->baud_rate = va_arg(ap, cyg_int32);
        // Should we verify this value here?
        cyg_hal_plf_serial_init_channel(chan);
        ret = 0;
        break;
    default:
        break;
    }

    va_end(ap);
    CYGARC_HAL_RESTORE_GP();
    return ret;
}

static int CYGOPT_HAL_KINETIS_DIAG_FLASH_SECTION_ATTR
cyg_hal_plf_serial_isr(void *__ch_data, int* __ctrlc,
                       CYG_ADDRWORD __vector, CYG_ADDRWORD __data)
{

    channel_data_t* chan = (channel_data_t*)__ch_data;
    CYG_ADDRESS uart_p = (CYG_ADDRESS) chan->base;
    cyg_uint8 uart_s1;
    int res = 0;
    cyg_uint8 ch_in;
    CYGARC_HAL_SAVE_GP();

    *__ctrlc = 0;

    HAL_READ_UINT8(uart_p + CYGHWR_DEV_FREESCALE_UART_S1, uart_s1);
    if (uart_s1 & CYGHWR_DEV_FREESCALE_UART_S1_RDRF) {
        HAL_READ_UINT8(uart_p + CYGHWR_DEV_FREESCALE_UART_D, ch_in);
        if( cyg_hal_is_break( (char *) &ch_in , 1 ) )
            *__ctrlc = 1;

        res = CYG_ISR_HANDLED;
    }

    HAL_INTERRUPT_ACKNOWLEDGE(chan->isr_vector);

    CYGARC_HAL_RESTORE_GP();
    return res;
}

static void CYGOPT_HAL_KINETIS_DIAG_FLASH_SECTION_ATTR
cyg_hal_plf_serial_init(void)
{
    hal_virtual_comm_table_t* comm;
    int cur;
    int chan_i;

    cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);

    // Init channels
    for(chan_i=0; chan_i<CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS; chan_i++) {
        cyg_hal_plf_serial_init_channel(&plf_ser_channels[chan_i]);

        // Setup procs in the vector table
        CYGACC_CALL_IF_SET_CONSOLE_COMM(chan_i);
        comm = CYGACC_CALL_IF_CONSOLE_PROCS();
        CYGACC_COMM_IF_CH_DATA_SET(*comm, &plf_ser_channels[chan_i]);
        CYGACC_COMM_IF_WRITE_SET(*comm, cyg_hal_plf_serial_write);
        CYGACC_COMM_IF_READ_SET(*comm, cyg_hal_plf_serial_read);
        CYGACC_COMM_IF_PUTC_SET(*comm, cyg_hal_plf_serial_putc);
        CYGACC_COMM_IF_GETC_SET(*comm, cyg_hal_plf_serial_getc);
        CYGACC_COMM_IF_CONTROL_SET(*comm, cyg_hal_plf_serial_control);
        CYGACC_COMM_IF_DBG_ISR_SET(*comm, cyg_hal_plf_serial_isr);
        CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, cyg_hal_plf_serial_getc_timeout);
    }
    // Restore original console
    CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);
#if (CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD != CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL_BAUD)
    plf_ser_channels[CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL]->baud_rate =
        CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL_BAUD;
    update_baud_rate( &plf_ser_channels[CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL] );
#endif
}

void CYGOPT_HAL_KINETIS_DIAG_FLASH_SECTION_ATTR
cyg_hal_plf_comms_init(void)
{
    static int initialized = 0;

    if (initialized)
        return;
    initialized = 1;
    cyg_hal_plf_serial_init();
}

#else // !defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)
//=============================================================================
// Non-Virtual vector HAL diagnostics

// #if !defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)

void hal_plf_diag_init(void)
{
    cyg_hal_plf_serial_init( &plf_ser_channels[CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL] );
}

void hal_plf_diag_putc(char c)
{
    cyg_hal_plf_serial_putc( &plf_ser_channels[CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL], c);
}

cyg_uint8 hal_plf_diag_getc(void)
{
    return cyg_hal_plf_serial_getc( &plf_ser_channels[CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL] );
}

#endif // defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)

//-----------------------------------------------------------------------------
// End of hal_diag.c
