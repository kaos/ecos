//=============================================================================
//
//      hal_diag.c
//
//      HAL diagnostic output code
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004, 2005, 2006, 2008, 2010
// 2011, Free Software Foundation, Inc.
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
// Author(s):   original: nickg, ccoutand: updated for Stellaris HAL
// Date:        2011-01-18
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include CYGBLD_HAL_PLATFORM_H

#include <cyg/infra/cyg_type.h>        // Base types
#include <cyg/infra/cyg_trac.h>        // Tracing

#include <cyg/hal/hal_arch.h>          // SAVE/RESTORE GP macros
#include <cyg/hal/hal_io.h>            // IO macros
#include <cyg/hal/hal_if.h>            // interface API
#include <cyg/hal/hal_intr.h>          // HAL_ENABLE/MASK/UNMASK_INTERRUPTS
#include <cyg/hal/hal_misc.h>          // Helper functions
#include <cyg/hal/drv_api.h>           // CYG_ISR_HANDLED

#include <cyg/hal/var_io.h>            // USART registers

//-----------------------------------------------------------------------------
typedef struct {
    cyg_uint32      uart;
    CYG_ADDRESS     base;
    cyg_int32       msec_timeout;
    int             isr_vector;
    cyg_uint32      rxpin;
    cyg_uint32      txpin;
    cyg_uint32      baud_rate;
    int             irq_state;
} channel_data_t;

static channel_data_t lm3s_ser_channels[] = {
#if CYGINT_HAL_CORTEXM_LM3S_UART0>0
    {0, CYGHWR_HAL_LM3S_UART0, 1000, CYGNUM_HAL_INTERRUPT_UART0,
     CYGHWR_HAL_LM3S_UART0_RX, CYGHWR_HAL_LM3S_UART0_TX},
#endif
#if CYGINT_HAL_CORTEXM_LM3S_UART1>0
    {1, CYGHWR_HAL_LM3S_UART1, 1000, CYGNUM_HAL_INTERRUPT_UART1,
     CYGHWR_HAL_LM3S_UART1_RX, CYGHWR_HAL_LM3S_UART1_TX},
#endif
};

//-----------------------------------------------------------------------------

static void
hal_lm3s_serial_init_channel(void *__ch_data)
{
    channel_data_t *chan = (channel_data_t *) __ch_data;
    CYG_ADDRESS     base = chan->base;
    cyg_uint32      lcrh,
                    ctl;

    // Disable UART
    HAL_READ_UINT32(base + CYGHWR_HAL_LM3S_UART_CTL, ctl);
    ctl &= ~(CYGHWR_HAL_LM3S_UART_CTL_UARTEN |
             CYGHWR_HAL_LM3S_UART_CTL_TXE | CYGHWR_HAL_LM3S_UART_CTL_RXE);
    HAL_WRITE_UINT32(base + CYGHWR_HAL_LM3S_UART_CTL, ctl);

    // Enable the PIO lines for the serial channel
    CYGHWR_HAL_LM3S_GPIO_SET(chan->rxpin);
    CYGHWR_HAL_LM3S_GPIO_SET(chan->txpin);

    // Set up Baud rate
    chan->baud_rate = CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD;
    hal_lm3s_uart_setbaud(base, chan->baud_rate);

    HAL_WRITE_UINT32(base + CYGHWR_HAL_LM3S_UART_FR, 0);

    // 8 bits - 1 stop - Enable FIFO
    lcrh = CYGHWR_HAL_LM3S_UART_LCRH_WLEN(3) | CYGHWR_HAL_LM3S_UART_LCRH_FEN;
    HAL_WRITE_UINT32(base + CYGHWR_HAL_LM3S_UART_LCRH, lcrh);

    // Enable the UART
    ctl = (CYGHWR_HAL_LM3S_UART_CTL_UARTEN |
           CYGHWR_HAL_LM3S_UART_CTL_TXE | CYGHWR_HAL_LM3S_UART_CTL_RXE);
    HAL_WRITE_UINT32(base + CYGHWR_HAL_LM3S_UART_CTL, ctl);
}

void
hal_lm3s_serial_putc(void *__ch_data, char c)
{
    CYG_ADDRESS     base = ((channel_data_t *) __ch_data)->base;
    cyg_uint32      sr;

    CYGARC_HAL_SAVE_GP();

    do {
        HAL_READ_UINT32(base + CYGHWR_HAL_LM3S_UART_FR, sr);
    } while ((sr & CYGHWR_HAL_LM3S_UART_FR_TXFE) == 0);

    HAL_WRITE_UINT32(base + CYGHWR_HAL_LM3S_UART_DR, c);

    CYGARC_HAL_RESTORE_GP();
}

static cyg_bool
hal_lm3s_serial_getc_nonblock(void *__ch_data, cyg_uint8 *ch)
{
    CYG_ADDRESS     base = ((channel_data_t *) __ch_data)->base;
    cyg_uint32      fr;
    cyg_uint32      c;

    CYGARC_HAL_SAVE_GP();

    HAL_READ_UINT32(base + CYGHWR_HAL_LM3S_UART_FR, fr);

    if ((fr & CYGHWR_HAL_LM3S_UART_FR_RXFE))
        return false;

    HAL_READ_UINT32(base + CYGHWR_HAL_LM3S_UART_DR, c);

    *ch = (cyg_uint8)c;

    CYGARC_HAL_RESTORE_GP();

    return true;
}

cyg_uint8
hal_lm3s_serial_getc(void *__ch_data)
{
    cyg_uint8       ch;

    CYGARC_HAL_SAVE_GP();

    while (!hal_lm3s_serial_getc_nonblock(__ch_data, &ch))
        continue;

    CYGARC_HAL_RESTORE_GP();

    return ch;
}

//=============================================================================
// Virtual vector HAL diagnostics

#if defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)

static void
hal_lm3s_serial_write(void *__ch_data, const cyg_uint8 *__buf,
                      cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while (__len-- > 0)
        hal_lm3s_serial_putc(__ch_data, *__buf++);

    CYGARC_HAL_RESTORE_GP();
}

static void
hal_lm3s_serial_read(void *__ch_data, cyg_uint8 *__buf, cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while (__len-- > 0)
        *__buf++ = hal_lm3s_serial_getc(__ch_data);

    CYGARC_HAL_RESTORE_GP();
}

cyg_bool
hal_lm3s_serial_getc_timeout(void *__ch_data, cyg_uint8 *ch)
{
    int             delay_count;
    channel_data_t *chan = (channel_data_t *) __ch_data;
    cyg_bool        res;

    CYGARC_HAL_SAVE_GP();

    delay_count = chan->msec_timeout * 100;     // delay in 10 us steps

    for (;;) {
        res = hal_lm3s_serial_getc_nonblock(__ch_data, ch);
        if (res || 0 == delay_count--)
            break;

        CYGACC_CALL_IF_DELAY_US(10);
    }

    CYGARC_HAL_RESTORE_GP();

    return res;
}

static int
hal_lm3s_serial_control(void *__ch_data, __comm_control_cmd_t __func, ...)
{
    channel_data_t *chan = (channel_data_t *) __ch_data;
    CYG_ADDRESS     base = ((channel_data_t *) __ch_data)->base;
    int             ret = 0;
    cyg_uint32      im;

    va_list         ap;

    CYGARC_HAL_SAVE_GP();

    va_start(ap, __func);

    switch (__func) {
    case __COMMCTL_IRQ_ENABLE:
        chan->irq_state = 1;
        HAL_INTERRUPT_ACKNOWLEDGE(chan->isr_vector);
        HAL_INTERRUPT_UNMASK(chan->isr_vector);
        HAL_READ_UINT32(base + CYGHWR_HAL_LM3S_UART_IM, im);
        im |= CYGHWR_HAL_LM3S_UART_IM_RXIM;
        HAL_WRITE_UINT32(base + CYGHWR_HAL_LM3S_UART_IM, im);
        break;
    case __COMMCTL_IRQ_DISABLE:
        ret = chan->irq_state;
        chan->irq_state = 0;
        HAL_INTERRUPT_MASK(chan->isr_vector);
        HAL_READ_UINT32(base + CYGHWR_HAL_LM3S_UART_IM, im);
        im &= ~CYGHWR_HAL_LM3S_UART_IM_RXIM;
        HAL_WRITE_UINT32(base + CYGHWR_HAL_LM3S_UART_IM, im);
        break;
    case __COMMCTL_DBG_ISR_VECTOR:
        ret = chan->isr_vector;
        break;
    case __COMMCTL_SET_TIMEOUT:
        {
            va_list         ap;

            va_start(ap, __func);

            ret = chan->msec_timeout;
            chan->msec_timeout = va_arg(ap, cyg_uint32);

            va_end(ap);
        }
    case __COMMCTL_GETBAUD:
        ret = chan->baud_rate;
        break;
    case __COMMCTL_SETBAUD:
        chan->baud_rate = va_arg(ap, cyg_int32);
        // Should we verify this value here?
        hal_lm3s_uart_setbaud(base, chan->baud_rate);
        ret = 0;
        break;
    default:
        break;
    }

    va_end(ap);

    CYGARC_HAL_RESTORE_GP();

    return ret;
}

static int
hal_lm3s_serial_isr(void *__ch_data, int *__ctrlc,
                    CYG_ADDRWORD __vector, CYG_ADDRWORD __data)
{
    channel_data_t *chan = (channel_data_t *) __ch_data;
    cyg_uint8       ch;

    CYGARC_HAL_SAVE_GP();

    *__ctrlc = 0;

    if (hal_lm3s_serial_getc_nonblock(__ch_data, &ch)) {
        if (cyg_hal_is_break((char *)&ch, 1))
            *__ctrlc = 1;
    }

    HAL_INTERRUPT_ACKNOWLEDGE(chan->isr_vector);

    CYGARC_HAL_RESTORE_GP();

    return 1;
}

static void
hal_lm3s_serial_init(void)
{
    hal_virtual_comm_table_t *comm;
    int             cur;
    int             i;

    cur =
        CYGACC_CALL_IF_SET_CONSOLE_COMM
        (CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);

    for (i = 0; i < CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS; i++) {
        hal_lm3s_serial_init_channel(&lm3s_ser_channels[i]);

        CYGACC_CALL_IF_SET_CONSOLE_COMM(i);
        comm = CYGACC_CALL_IF_CONSOLE_PROCS();
        CYGACC_COMM_IF_CH_DATA_SET(*comm, &lm3s_ser_channels[i]);
        CYGACC_COMM_IF_WRITE_SET(*comm, hal_lm3s_serial_write);
        CYGACC_COMM_IF_READ_SET(*comm, hal_lm3s_serial_read);
        CYGACC_COMM_IF_PUTC_SET(*comm, hal_lm3s_serial_putc);
        CYGACC_COMM_IF_GETC_SET(*comm, hal_lm3s_serial_getc);
        CYGACC_COMM_IF_CONTROL_SET(*comm, hal_lm3s_serial_control);
        CYGACC_COMM_IF_DBG_ISR_SET(*comm, hal_lm3s_serial_isr);
        CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, hal_lm3s_serial_getc_timeout);
    }

    // Restore original console
    CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);

# if (CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD != CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL_BAUD)
    // Set debug channel baud rate if different
    lm3s_ser_channels[CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL]->baud_rate =
        CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL_BAUD;
    update_baud_rate(&lm3s_ser_channels
                     [CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL]);
# endif

}

void
cyg_hal_plf_comms_init(void)
{
    static int      initialized = 0;

    if (initialized)
        return;

    initialized = 1;

    hal_lm3s_serial_init();
}

#endif // ifdef CYGSEM_HAL_VIRTUAL_VECTOR_DIAG


//=============================================================================
// Non-Virtual vector HAL diagnostics

#if !defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)

void
hal_lm3s_diag_init(void)
{
    hal_lm3s_serial_init(&lm3s_ser_channels
                         [CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL]);
}

void
hal_lm3s_diag_putc(char c)
{
    hal_lm3s_serial_putc(&lm3s_ser_channels
                         [CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL], c);
}

cyg_uint8
hal_lm3s_diag_getc(void)
{
    return
        hal_lm3s_serial_getc(&lm3s_ser_channels
                             [CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL]);
}

#endif // ifndef CYGSEM_HAL_VIRTUAL_VECTOR_DIAG

//-----------------------------------------------------------------------------
// EOF hal_diag.c
