//==========================================================================
//
//      lm3s_misc.c
//
//      Stellaris Cortex-M3 variant HAL functions
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    ccoutand
// Date:         2011-01-18
// Description:  
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
#endif

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>        // Tracing macros
#include <cyg/infra/cyg_ass.h>         // Assertion macros

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_if.h>

//==========================================================================
// Initialization
//
__externC void  hal_start_clocks(void);

// Clock computation must be done per Variant basis
cyg_uint32      hal_cortexm_systick_clock;
cyg_uint32      hal_lm3s_sysclk;


void
hal_variant_init(void)
{
#if !defined(CYG_HAL_STARTUP_RAM)
    hal_start_clocks();
#endif

    // All LM3S devices use PORTA 0/1 for UART0
#if CYGINT_HAL_CORTEXM_LM3S_UART0>0
    CYGHWR_HAL_LM3S_PERIPH_SET(CYGHWR_HAL_LM3S_P_UART0, 1);
    CYGHWR_HAL_LM3S_PERIPH_SET(CYGHWR_HAL_LM3S_P_UART0_GPIO, 1);
#endif

#if CYGINT_HAL_CORTEXM_LM3S_UART1>0

# ifdef CYGHWR_HAL_LM3S_P_UART1_GPIO
    CYGHWR_HAL_LM3S_PERIPH_SET(CYGHWR_HAL_LM3S_P_UART1, 1);
    CYGHWR_HAL_LM3S_PERIPH_SET(CYGHWR_HAL_LM3S_P_UART1_GPIO, 1);
# else
#  error "Variant/Platform does not specify UART1 GPIO Port"
# endif

#endif

#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    hal_if_init();
#endif
}

//==========================================================================
// GPIO support
//
// These functions provide configuration and IO for GPIO pins.
//

__externC void
hal_lm3s_gpio_set(cyg_uint32 pin)
{
    cyg_uint32      port = CYGHWR_HAL_LM3S_GPIO_PORT(pin);
    cyg_uint32      bit = (1 << CYGHWR_HAL_LM3S_GPIO_BIT(pin));
    cyg_uint32      cm = CYGHWR_HAL_LM3S_GPIO_CFG(pin);
    cyg_uint32      mode = CYGHWR_HAL_LM3S_GPIO_MODE(pin);
    cyg_uint32      irq = CYGHWR_HAL_LM3S_GPIO_IRQ(pin);
    cyg_uint32      st = CYGHWR_HAL_LM3S_GPIO_STRENGTH(pin);
    cyg_uint32      reg,
                    dir,
                    im,
                    dr2r,
                    dr4r,
                    dr8r;

    if (pin == CYGHWR_HAL_LM3S_GPIO_NONE)
        return;

    /*
     *  Handle IO mode settings
     */
    HAL_READ_UINT32(port + CYGHWR_HAL_LM3S_GPIO_AFSEL, reg);
    HAL_READ_UINT32(port + CYGHWR_HAL_LM3S_GPIO_DIR, dir);

    HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_AFSEL, (reg & ~(bit)));
    if (mode == CYGHWR_HAL_LM3S_GPIO_MODE_IN)
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_DIR, (dir & ~(bit)));
    else if (mode == CYGHWR_HAL_LM3S_GPIO_MODE_OUT)
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_DIR, (dir | bit));
    else
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_AFSEL, (reg | bit));

    /*
     *  Handle IO configuration
     */
    HAL_READ_UINT32(port + CYGHWR_HAL_LM3S_GPIO_ODR, reg);
    if (cm == CYGHWR_HAL_LM3S_GPIO_CNF_OP ||
        cm == CYGHWR_HAL_LM3S_GPIO_CNF_OP_PULLUP ||
        cm == CYGHWR_HAL_LM3S_GPIO_CNF_OP_PULLDOWN) {
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_ODR, (reg | bit));
    } else
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_ODR, (reg & ~(bit)));

    HAL_READ_UINT32(port + CYGHWR_HAL_LM3S_GPIO_PUR, reg);
    if (cm == CYGHWR_HAL_LM3S_GPIO_CNF_PULLUP ||
        cm == CYGHWR_HAL_LM3S_GPIO_CNF_OP_PULLUP) {
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_PUR, (reg | bit));
    } else
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_PUR, (reg & ~(bit)));

    HAL_READ_UINT32(port + CYGHWR_HAL_LM3S_GPIO_PDR, reg);
    if (cm == CYGHWR_HAL_LM3S_GPIO_CNF_PULLDOWN ||
        cm == CYGHWR_HAL_LM3S_GPIO_CNF_OP_PULLDOWN) {
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_PDR, (reg | bit));
    } else
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_PDR, (reg & ~(bit)));

    /*
     *  Handle IO strength
     */
    HAL_READ_UINT32(port + CYGHWR_HAL_LM3S_GPIO_DR2R, dr2r);
    HAL_READ_UINT32(port + CYGHWR_HAL_LM3S_GPIO_DR4R, dr4r);
    HAL_READ_UINT32(port + CYGHWR_HAL_LM3S_GPIO_DR8R, dr8r);
    if (st == CYGHWR_HAL_LM3S_GPIO_STRENGTH_2_MA) {
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_DR4R, (dr4r & ~(bit)));
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_DR8R, (dr8r & ~(bit)));
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_DR2R, (dr2r | bit));
    } else if (st == CYGHWR_HAL_LM3S_GPIO_STRENGTH_4_MA) {
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_DR2R, (dr2r & ~(bit)));
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_DR8R, (dr8r & ~(bit)));
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_DR4R, (dr4r | bit));
    } else if (st == CYGHWR_HAL_LM3S_GPIO_STRENGTH_8_MA) {
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_DR2R, (dr2r & ~(bit)));
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_DR4R, (dr4r & ~(bit)));
        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_DR8R, (dr8r | bit));
    }

    /*
     *  Handle interrupt settings
     */
    HAL_READ_UINT32(port + CYGHWR_HAL_LM3S_GPIO_IM, im);
    HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_IM, (im & ~(bit)));
    if (irq != CYGHWR_HAL_LM3S_GPIO_IRQ_DISABLE) {
        HAL_READ_UINT32(port + CYGHWR_HAL_LM3S_GPIO_IS, reg);

        if (irq == CYGHWR_HAL_LM3S_GPIO_IRQ_LOW_LEVEL ||
            irq == CYGHWR_HAL_LM3S_GPIO_IRQ_HIGH_LEVEL)
            HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_IS, (reg | bit));
        else
            HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_IS, (reg & ~(bit)));

        HAL_READ_UINT32(port + CYGHWR_HAL_LM3S_GPIO_IBE, reg);
        if (irq == CYGHWR_HAL_LM3S_GPIO_IRQ_BOTH_EDGES) {
            HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_IBE, (reg | bit));
        } else {
            HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_IBE, (reg & ~(bit)));

            HAL_READ_UINT32(port + CYGHWR_HAL_LM3S_GPIO_IEV, reg);
            if (irq == CYGHWR_HAL_LM3S_GPIO_IRQ_LOW_LEVEL ||
                irq == CYGHWR_HAL_LM3S_GPIO_IRQ_FALLING_EDGE)
                HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_IEV,
                                 (reg & ~(bit)));
            else
                HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_IEV,
                                 (reg | bit));
        }

        HAL_WRITE_UINT32(port + CYGHWR_HAL_LM3S_GPIO_IM, (im | bit));
    }
}

__externC void
hal_lm3s_gpio_out(cyg_uint32 pin, int val)
{
    cyg_uint32      port = CYGHWR_HAL_LM3S_GPIO_PORT(pin);
    int             bit = (1 << CYGHWR_HAL_LM3S_GPIO_BIT(pin));

    port += (CYGHWR_HAL_LM3S_GPIO_DATA + (bit << 2));

    HAL_WRITE_UINT32(port, (val ? bit : 0));
}

__externC void
hal_lm3s_gpio_in(cyg_uint32 pin, int *val)
{
    cyg_uint32      port = CYGHWR_HAL_LM3S_GPIO_PORT(pin);
    int             bit = (1 << CYGHWR_HAL_LM3S_GPIO_BIT(pin));
    cyg_uint32      pd;

    port += (CYGHWR_HAL_LM3S_GPIO_DATA + (bit << 2));

    HAL_READ_UINT32(port, pd);
    *val = pd;
}


//==========================================================================
// Peripheral support
//

__externC void
hal_lm3s_periph_set(cyg_uint32 periph, cyg_uint32 on_off)
{
    cyg_uint32      reg;

    if (CYGHWR_HAL_LM3S_PERIPH_GC0 & periph) {
        HAL_READ_UINT32(CYGHWR_HAL_LM3S_SC + CYGHWR_HAL_LM3S_SC_RCGC0, reg);
        if (on_off)
            reg |= (periph & ~CYGHWR_HAL_LM3S_PERIPH_GC0);
        else
            reg &= ~(periph & ~CYGHWR_HAL_LM3S_PERIPH_GC0);
        HAL_WRITE_UINT32(CYGHWR_HAL_LM3S_SC + CYGHWR_HAL_LM3S_SC_RCGC0, reg);
    }

    if (CYGHWR_HAL_LM3S_PERIPH_GC1 & periph) {
        HAL_READ_UINT32(CYGHWR_HAL_LM3S_SC + CYGHWR_HAL_LM3S_SC_RCGC1, reg);
        if (on_off)
            reg |= (periph & ~CYGHWR_HAL_LM3S_PERIPH_GC1);
        else
            reg &= ~(periph & ~CYGHWR_HAL_LM3S_PERIPH_GC1);
        HAL_WRITE_UINT32(CYGHWR_HAL_LM3S_SC + CYGHWR_HAL_LM3S_SC_RCGC1, reg);
    }

    if (CYGHWR_HAL_LM3S_PERIPH_GC2 & periph) {
        HAL_READ_UINT32(CYGHWR_HAL_LM3S_SC + CYGHWR_HAL_LM3S_SC_RCGC2, reg);
        if (on_off)
            reg |= (periph & ~CYGHWR_HAL_LM3S_PERIPH_GC2);
        else
            reg &= ~(periph & ~CYGHWR_HAL_LM3S_PERIPH_GC2);
        HAL_WRITE_UINT32(CYGHWR_HAL_LM3S_SC + CYGHWR_HAL_LM3S_SC_RCGC2, reg);
    }

}


//==========================================================================
// UART baud rate
//
// Set the baud rate divider of a UART based on the requested rate and
// the current APB clock settings.
//

__externC void
hal_lm3s_uart_setbaud(cyg_uint32 base, cyg_uint32 baud)
{
    cyg_uint32      int_div,
                    frac_div;

    int_div = ((((hal_cortexm_systick_clock << 3) / baud) + 1) >> 1);

    frac_div = int_div % 64;
    int_div = int_div >> 6;

    HAL_WRITE_UINT32(base + CYGHWR_HAL_LM3S_UART_IBRD, int_div);
    HAL_WRITE_UINT32(base + CYGHWR_HAL_LM3S_UART_FBRD, frac_div);
}


//==========================================================================
// I2C clock rate
//
__externC cyg_uint32
hal_lm3s_i2c_clock(void)
{
    return hal_lm3s_sysclk;
}

//==========================================================================
// Timer clock rate
//
__externC cyg_uint32
hal_lm3s_timer_clock(void)
{
    return hal_lm3s_sysclk;
}

//==========================================================================
// EOF lm3s_misc_misc.c
