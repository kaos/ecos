/*=============================================================================
//
//      hal_diag.c
//
//      ColdFire MCFxxxx HAL diagnostics support
//
//=============================================================================
//###ECOSGPLCOPYRIGHTBEGIN####
//-------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003,2004,2006,2007,2008 Free Software Foundation, Inc.
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   bartv
// Date:        2003-06-04
//
//####DESCRIPTIONEND####
//===========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/hal_m68k_mcfxxxx.h>
#ifdef CYGPKG_REDBOOT
# include <pkgconf/redboot.h>
#endif
#include CYGBLD_HAL_PLATFORM_H

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_misc.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/drv_api.h>

// There are two main possibilities:
// 1) the platform uses the standard MCFxxxx diagnostics support, by
//    enabling CYGINT_HAL_M68K_MCFxxxx_DIAGNOSTICS_USE_DEFAULT, and
//    the user has selected an available uart.
// 2) or the platform may provide its own diagnostics facilities.

#if defined(CYGINT_HAL_M68K_MCFxxxx_DIAGNOSTICS_USE_DEFAULT)

// Output goes via one of the UARTs. hal_diag.h will have provided
// _HAL_MCFxxxx_DIAG_UART_BASE_ and _HAL_MCFxxxx_DIAG_UART_ISRVEC_.
// Optionally the platform HAL can provide an extra INIT macro.

void
hal_mcfxxxx_diag_uart_init(cyg_uint8* base, cyg_uint32 baud)
{
#if defined(CYGSEM_HAL_VIRTUAL_VECTOR_CLAIM_COMMS) || defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
    static int  initialized;
    if (initialized) {
        return;
    }
    initialized = 1;
#endif
    
#ifdef HAL_MCFxxxx_UART_DIAG_PLATFORM_INIT    
    HAL_MCFxxxx_UART_DIAG_PLATFORM_INIT();
#endif
    
    // Various resets to get the UART in a known good state
    HAL_WRITE_UINT8(base + HAL_MCFxxxx_UARTx_UCR, HAL_MCFxxxx_UARTx_UCR_MISC_RMRP);
    HAL_WRITE_UINT8(base + HAL_MCFxxxx_UARTx_UCR, HAL_MCFxxxx_UARTx_UCR_MISC_RR);
    HAL_WRITE_UINT8(base + HAL_MCFxxxx_UARTx_UCR, HAL_MCFxxxx_UARTx_UCR_MISC_RT);
    HAL_WRITE_UINT8(base + HAL_MCFxxxx_UARTx_UCR, HAL_MCFxxxx_UARTx_UCR_MISC_RES);
    HAL_WRITE_UINT8(base + HAL_MCFxxxx_UARTx_UCR, HAL_MCFxxxx_UARTx_UCR_MISC_RBCI);
    
    // Assume that this code will only run during system startup and with
    // interrupts disabled, so that imr1 and imr2 can be poked in succession.
    HAL_WRITE_UINT8(base + HAL_MCFxxxx_UARTx_UMR, HAL_MCFxxxx_UARTx_UMR1_PM_NO     | HAL_MCFxxxx_UARTx_UMR1_BC_8);
    HAL_WRITE_UINT8(base + HAL_MCFxxxx_UARTx_UMR, HAL_MCFxxxx_UARTx_UMR2_CM_NORMAL | HAL_MCFxxxx_UARTx_UMR2_SB_1);

    // Assert RTS, just in case the other side is interested
    HAL_WRITE_UINT8(base + HAL_MCFxxxx_UARTx_UOP1, HAL_MCFxxxx_UARTx_UOP_RTS);

    // No hardware flow control based on fifo contents, no interrupts for change-of-state
    HAL_WRITE_UINT8(base + HAL_MCFxxxx_UARTx_UACR, 0);

    // The IMR is write-only, so it is going to be difficult to set or clear just the
    // rxrdy bit. Instead leave the serial interrupt enabled here but mask/unmask it
    // inside the interrupt controller.
    HAL_WRITE_UINT8(base + HAL_MCFxxxx_UARTx_UIMR, HAL_MCFxxxx_UARTx_UIMR_RXRDY);

    // Baud rate. Always use the internal prescaled CLKIN. The baud rate is
    // determined by the platform.
    HAL_WRITE_UINT8(base + HAL_MCFxxxx_UARTx_UCSR, HAL_MCFxxxx_UARTx_UCSR_RCS_CLKIN | HAL_MCFxxxx_UARTx_UCSR_TCS_CLKIN);
    HAL_MCFxxxx_UARTx_SET_BAUD(base, baud);
    
    // Enable both RX and TX
    HAL_WRITE_UINT8(base + HAL_MCFxxxx_UARTx_UCR, HAL_MCFxxxx_UARTx_UCR_TC_TE | HAL_MCFxxxx_UARTx_UCR_RC_RE);
}

void
hal_mcfxxxx_diag_uart_putc(void* channel_data, char c)
{
    cyg_uint8*  base = (cyg_uint8*)channel_data;
    cyg_uint8   usr;

    do {
        HAL_READ_UINT8(base + HAL_MCFxxxx_UARTx_USR, usr);
    } while (!(usr & HAL_MCFxxxx_UARTx_USR_TXRDY) );
    HAL_WRITE_UINT8(base + HAL_MCFxxxx_UARTx_UTB, c);
}

cyg_uint8
hal_mcfxxxx_diag_uart_getc(void* channel_data)
{
    cyg_uint8*  base = (cyg_uint8*)channel_data;
    cyg_uint8   usr, data;
    do {
        HAL_READ_UINT8(base + HAL_MCFxxxx_UARTx_USR, usr);
    } while (!(usr & HAL_MCFxxxx_UARTx_USR_RXRDY));
    HAL_READ_UINT8(base + HAL_MCFxxxx_UARTx_URB, data);
    return data;
}

// Additional routines needed in virtual vector configurations.
# if defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)

// State manipulated by the _control() function. Support for dynamic
// baud rates is optional. Some platforms may choose to provide this
// by implementing CYGSEM_REDBOOT_VARIABLE_BAUD_RATE
static int          msec_timeout    = 1;
static int          irq_enabled     = 0;
#  ifdef CYGSEM_REDBOOT_VARIABLE_BAUD_RATE
static cyg_uint32   baud_rate       = _HAL_MCFxxxx_DIAG_UART_BAUD_;
#  endif

static void
hal_mcfxxxx_diag_uart_write(void* channel_data, const cyg_uint8* buf, cyg_uint32 len)
{
    while (len-- > 0) {
        hal_mcfxxxx_diag_uart_putc(channel_data, *buf++);
    }
}

static void
hal_mcfxxxx_diag_uart_read(void* channel_data, cyg_uint8* buf, cyg_uint32 len)
{
    while (len-- > 0) {
        *buf++ = hal_mcfxxxx_diag_uart_getc(channel_data);
    }
}

static cyg_bool
hal_mcfxxxx_diag_uart_getc_timeout(void* channel_data, cyg_uint8* ch)
{
    cyg_uint8*  base = (cyg_uint8*)channel_data;
    cyg_uint8   usr;
    cyg_uint8   data;
    int         delay_count = msec_timeout * 10;

    while (delay_count-- > 0) {
        HAL_READ_UINT8(base + HAL_MCFxxxx_UARTx_USR, usr);
        if (usr & HAL_MCFxxxx_UARTx_USR_RXRDY) {
            HAL_READ_UINT8(base + HAL_MCFxxxx_UARTx_URB, data);
            *ch = data;
            return 1;
        }
        HAL_DELAY_US(100);
    }
    return 0;
}

static int
hal_mcfxxxx_diag_uart_isr(void* channel_data, int* ctrl_c, CYG_ADDRWORD isr_vector, CYG_ADDRWORD isr_data)
{
    cyg_uint8*  base = (cyg_uint8*)channel_data;
    cyg_uint8   usr;
    cyg_uint8   data;

    *ctrl_c = 0;
    HAL_READ_UINT8(base + HAL_MCFxxxx_UARTx_USR, usr);
    if (usr & HAL_MCFxxxx_UARTx_USR_RXRDY) {
        HAL_READ_UINT8(base + HAL_MCFxxxx_UARTx_URB, data);
        if (cyg_hal_is_break((char*)&data, 1)) {
            *ctrl_c = 1;
        }
    }
    return CYG_ISR_HANDLED;
}

static int
hal_mcfxxxx_diag_uart_control(void* channel_data, __comm_control_cmd_t func, ...)
{
    int         result  = -1;
    va_list     args;

    va_start(args, func);
    switch(func) {
      case __COMMCTL_IRQ_ENABLE:
        result      = 0;
        irq_enabled = 1;
        HAL_INTERRUPT_UNMASK(_HAL_MCFxxxx_DIAG_UART_ISRVEC_);
        break;
      case __COMMCTL_IRQ_DISABLE:
        result = irq_enabled;
        irq_enabled = 0;
        HAL_INTERRUPT_MASK(_HAL_MCFxxxx_DIAG_UART_ISRVEC_);
        break;
      case __COMMCTL_DBG_ISR_VECTOR:
        result = _HAL_MCFxxxx_DIAG_UART_ISRVEC_;
        break;
      case __COMMCTL_SET_TIMEOUT:
        result = msec_timeout;
        msec_timeout = va_arg(args,cyg_uint32);
        break;
#ifdef CYGSEM_REDBOOT_VARIABLE_BAUD_RATE
      case __COMMCTL_GETBAUD:
        result = baud_rate;
        break;
      case __COMMCTL_SETBAUD:
        baud_rate = va_arg(args, cyg_uint32);
        HAL_MCFxxxx_UARTx_SET_BAUD(_HAL_MCFxxxx_DIAG_UART_BASE_, baud_rate);
        result = 0;
        break;
#endif        
      default:
        break;
    }
    
    CYG_UNUSED_PARAM(void*, channel_data);
    return result;
}

void
cyg_hal_plf_comms_init(void)
{
    hal_virtual_comm_table_t*   comm;
    int                         cur;

    hal_mcfxxxx_diag_uart_init((cyg_uint8*)_HAL_MCFxxxx_DIAG_UART_BASE_, CYGNUM_HAL_M68K_MCFxxxx_DIAGNOSTICS_BAUD);
    // For the diag channel we may want interrupts without explicitly installing
    // an interrupt handler, so the priority has to be set manually.
    HAL_INTERRUPT_SET_LEVEL(_HAL_MCFxxxx_DIAG_UART_ISRVEC_, CYGNUM_HAL_M68K_MCFxxxx_DIAGNOSTICS_ISRPRI);
    
    cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
    
    CYGACC_CALL_IF_SET_CONSOLE_COMM(0);
    comm    = CYGACC_CALL_IF_CONSOLE_PROCS();
    CYGACC_COMM_IF_CH_DATA_SET(*comm, (void*)(_HAL_MCFxxxx_DIAG_UART_BASE_));
    CYGACC_COMM_IF_WRITE_SET(*comm, hal_mcfxxxx_diag_uart_write);
    CYGACC_COMM_IF_READ_SET(*comm, hal_mcfxxxx_diag_uart_read);
    CYGACC_COMM_IF_PUTC_SET(*comm, hal_mcfxxxx_diag_uart_putc);
    CYGACC_COMM_IF_GETC_SET(*comm, hal_mcfxxxx_diag_uart_getc);
    CYGACC_COMM_IF_CONTROL_SET(*comm, hal_mcfxxxx_diag_uart_control);
    CYGACC_COMM_IF_DBG_ISR_SET(*comm, hal_mcfxxxx_diag_uart_isr);
    CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, hal_mcfxxxx_diag_uart_getc_timeout);
    
    CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);
}
# endif // CYGSEM_HAL_VIRTUAL_VECTOR_DIAG

#else
// The platform HAL must provide its own diagnostics routines.
#endif

/* End of hal_diag.c */
