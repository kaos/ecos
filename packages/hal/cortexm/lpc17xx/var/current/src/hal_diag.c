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
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004, 2005, 2006, 2008, 2010  Free Software Foundation, Inc.
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
// Author(s):    nickg, jani, ilijak
// Contributors: jskov, gthomas
// Date:         2010-12-15
// Purpose:      HAL diagnostic output
// Description:  Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include CYGBLD_HAL_PLATFORM_H

#include <cyg/infra/cyg_type.h>        // base types
#include <cyg/infra/cyg_trac.h>        // tracing

#include <cyg/hal/hal_arch.h>          // SAVE/RESTORE GP macros
#include <cyg/hal/hal_io.h>            // IO macros
#include <cyg/hal/hal_if.h>            // interface API
#include <cyg/hal/hal_intr.h>          // HAL_ENABLE/MASK/UNMASK_INTERRUPTS
#include <cyg/hal/hal_misc.h>          // Helper functions
#include <cyg/hal/drv_api.h>           // CYG_ISR_HANDLED

#include <cyg/hal/var_io.h>            // USART registers
#include <cyg/hal/lpc17xx_misc.h>      // peripheral identifiers

//-----------------------------------------------------------------------------

typedef struct {
    cyg_uint32      uart;
    CYG_ADDRESS     base;
    cyg_int32       msec_timeout;
    int             isr_vector;
    int             baud_rate;
    cyg_uint8       periph_id;
    int             irq_state;
} channel_data_t;

static channel_data_t lpc17xx_ser_channels[] = {
#if CYGINT_HAL_LPC17XX_UART0>0
    {0,
     CYGHWR_HAL_LPC17XX_REG_UART0_BASE,
     1000,
     CYGNUM_HAL_INTERRUPT_UART0,
     CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD,
     CYNUM_HAL_LPC17XX_PCLK_UART0},
#endif
#if CYGINT_HAL_LPC17XX_UART1>0
    {1,
     CYGHWR_HAL_LPC17XX_REG_UART1_BASE,
     1000,
     CYGNUM_HAL_INTERRUPT_UART1,
     CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD,
     CYNUM_HAL_LPC17XX_PCLK_UART1}
#endif
};

#define HAL_PLF_SER_CHANNELS lpc17xx_ser_channels


//===========================================================================
// Initialize diagnostic serial channel
//===========================================================================
static void
hal_plf_serial_init_channel(void *__ch_data)
{
    channel_data_t *chan = (channel_data_t *) __ch_data;
    CYG_ADDRESS     base = chan->base;

    hal_plf_uart_setbaud(base, chan->baud_rate);

    // 8-1-no parity.
    HAL_WRITE_UINT32(base + CYGHWR_HAL_LPC17XX_REG_UxLCR,
                     CYGHWR_HAL_LPC17XX_REG_UxLCR_WORD_LENGTH_8 |
                     CYGHWR_HAL_LPC17XX_REG_UxLCR_STOP_1);

    // Reset and enable FIFO
    HAL_WRITE_UINT32(base + CYGHWR_HAL_LPC17XX_REG_UxFCR,
                     CYGHWR_HAL_LPC17XX_REG_UxFCR_FIFO_ENA |
                     CYGHWR_HAL_LPC17XX_REG_UxFCR_RX_FIFO_RESET |
                     CYGHWR_HAL_LPC17XX_REG_UxFCR_TX_FIFO_RESET);
}


//===========================================================================
// Write single character
//===========================================================================
void
hal_plf_serial_putc(void *__ch_data, char c)
{
    CYG_ADDRESS     base = ((channel_data_t *) __ch_data)->base;
    cyg_uint32      sr;

    CYGARC_HAL_SAVE_GP();

    do {
        HAL_READ_UINT32(base + CYGHWR_HAL_LPC17XX_REG_UxLSR, sr);
    } while ((sr & CYGHWR_HAL_LPC17XX_REG_UxLSR_THRE) == 0);

    HAL_WRITE_UINT32(base + CYGHWR_HAL_LPC17XX_REG_UxTHR, c);

    CYGARC_HAL_RESTORE_GP();
}

static cyg_bool
hal_plf_serial_getc_nonblock(void *__ch_data, cyg_uint8 *ch)
{
    CYG_ADDRESS     base = ((channel_data_t *) __ch_data)->base;
    cyg_uint32      sr;
    cyg_uint32      c;

    CYGARC_HAL_SAVE_GP();

    HAL_READ_UINT32(base + CYGHWR_HAL_LPC17XX_REG_UxLSR, sr);

    if ((sr & CYGHWR_HAL_LPC17XX_REG_UxLSR_RDR) == 0)
        return false;

    HAL_READ_UINT32(base + CYGHWR_HAL_LPC17XX_REG_UxRBR, c);

    *ch = (cyg_uint8)c;

    CYGARC_HAL_RESTORE_GP();

    return true;
}

cyg_uint8
hal_plf_serial_getc(void *__ch_data)
{
    cyg_uint8       ch;

    CYGARC_HAL_SAVE_GP();

    while (!hal_plf_serial_getc_nonblock(__ch_data, &ch));

    CYGARC_HAL_RESTORE_GP();

    return ch;
}

//=============================================================================
// Virtual vector HAL diagnostics

#if defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)

static void
hal_plf_serial_write(void *__ch_data, const cyg_uint8 *__buf,
                     cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while (__len-- > 0)
        hal_plf_serial_putc(__ch_data, *__buf++);

    CYGARC_HAL_RESTORE_GP();
}

static void
hal_plf_serial_read(void *__ch_data, cyg_uint8 *__buf, cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while (__len-- > 0)
        *__buf++ = hal_plf_serial_getc(__ch_data);

    CYGARC_HAL_RESTORE_GP();
}

cyg_bool
hal_plf_serial_getc_timeout(void *__ch_data, cyg_uint8 *ch)
{
    int             delay_count;
    channel_data_t *chan = (channel_data_t *) __ch_data;
    cyg_bool        res;

    CYGARC_HAL_SAVE_GP();

    // Delay in 10 us steps
    delay_count = chan->msec_timeout * 100;

    while(true) {
        res = hal_plf_serial_getc_nonblock(__ch_data, ch);
        if (res || 0 == delay_count--)
            break;

        CYGACC_CALL_IF_DELAY_US(10);
    }

    CYGARC_HAL_RESTORE_GP();

    return res;
}

static int
hal_plf_serial_control(void *__ch_data, __comm_control_cmd_t __func, ...)
{
    channel_data_t *chan = (channel_data_t *) __ch_data;
    CYG_ADDRESS     base = ((channel_data_t *) __ch_data)->base;
    int             ret = 0;

    va_list         ap;

    CYGARC_HAL_SAVE_GP();

    va_start(ap, __func);

    switch (__func) {
    case __COMMCTL_IRQ_ENABLE:
        chan->irq_state = 1;
        HAL_INTERRUPT_ACKNOWLEDGE(chan->isr_vector);
        HAL_INTERRUPT_UNMASK(chan->isr_vector);
        HAL_WRITE_UINT32(base + CYGHWR_HAL_LPC17XX_REG_UxIER,
                         CYGHWR_HAL_LPC17XX_REG_UxIER_RXDATA_INT);
        break;
    case __COMMCTL_IRQ_DISABLE:
        ret = chan->irq_state;
        chan->irq_state = 0;
        HAL_INTERRUPT_MASK(chan->isr_vector);
        HAL_WRITE_UINT32(base + CYGHWR_HAL_LPC17XX_REG_UxIER,
                         CYGHWR_HAL_LPC17XX_REG_UxIER_RXDATA_INT);
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
        hal_plf_uart_setbaud(base, chan->baud_rate);
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
hal_plf_serial_isr(void *__ch_data, int *__ctrlc,
                   CYG_ADDRWORD __vector, CYG_ADDRWORD __data)
{
    channel_data_t *chan = (channel_data_t *) __ch_data;
    cyg_uint8       ch;

    CYGARC_HAL_SAVE_GP();

    *__ctrlc = 0;

    if (hal_plf_serial_getc_nonblock(__ch_data, &ch)) {
        if (cyg_hal_is_break((char *)&ch, 1))
            *__ctrlc = 1;
    }

    HAL_INTERRUPT_ACKNOWLEDGE(chan->isr_vector);

    CYGARC_HAL_RESTORE_GP();

    return 1;
}

static void
hal_plf_serial_init(void)
{
    hal_virtual_comm_table_t *comm;
    int             cur;
    int             i;

    cur =
        CYGACC_CALL_IF_SET_CONSOLE_COMM
        (CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);

    for (i = 0; i < CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS; i++) {
        hal_plf_serial_init_channel(&lpc17xx_ser_channels[i]);

        CYGACC_CALL_IF_SET_CONSOLE_COMM(i);
        comm = CYGACC_CALL_IF_CONSOLE_PROCS();
        CYGACC_COMM_IF_CH_DATA_SET(*comm, &lpc17xx_ser_channels[i]);
        CYGACC_COMM_IF_WRITE_SET(*comm, hal_plf_serial_write);
        CYGACC_COMM_IF_READ_SET(*comm, hal_plf_serial_read);
        CYGACC_COMM_IF_PUTC_SET(*comm, hal_plf_serial_putc);
        CYGACC_COMM_IF_GETC_SET(*comm, hal_plf_serial_getc);
        CYGACC_COMM_IF_CONTROL_SET(*comm, hal_plf_serial_control);
        CYGACC_COMM_IF_DBG_ISR_SET(*comm, hal_plf_serial_isr);
        CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, hal_plf_serial_getc_timeout);
    }

    // Restore original console
    CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);

    // Set debug channel baud rate if different
#if (CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD != CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL_BAUD)
    lpc17xx_ser_channels[CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL]->baud_rate =
        CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL_BAUD;
    update_baud_rate(&lpc17xx_ser_channels
                     [CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL]);
#endif

}

void
cyg_hal_plf_comms_init(void)
{
    static int      initialized = 0;

    if (initialized)
        return;

    initialized = 1;

    hal_plf_serial_init();
}

#endif // CYGSEM_HAL_VIRTUAL_VECTOR_DIAG

//=============================================================================
// Non-Virtual vector HAL diagnostics

#if !defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)

void
hal_plf_diag_init(void)
{
    hal_plf_serial_init(&lpc17xx_ser_channels
                        [CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL]);
}

void
hal_plf_diag_putc(char c)
{
    hal_plf_serial_putc(&lpc17xx_ser_channels
                        [CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL], c);
}

cyg_uint8
hal_plf_diag_getc(void)
{
    return
        hal_plf_serial_getc(&lpc17xx_ser_channels
                            [CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL]);
}

#endif // if !defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)

//-----------------------------------------------------------------------------
// End of hal_diag.c
