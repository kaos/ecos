/*==========================================================================
//
//      stm32_misc.c
//
//      Cortex-M STM32 HAL functions
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008 Free Software Foundation, Inc.                        
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
// Author(s):    nickg
// Date:         2008-07-30
// Description:  
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm.h>
#include <pkgconf/hal_cortexm_stm32.h>
#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_intr.h>           // HAL header
#include <cyg/hal/hal_if.h>             // HAL header

//==========================================================================
// Clock Initialization values

#if CYGHWR_HAL_CORTEXM_STM32_CLOCK_HCLK_DIV == 1
# define CYGHWR_HAL_STM32_RCC_CFGR_HPRE CYGHWR_HAL_STM32_RCC_CFGR_HPRE_1
#elif CYGHWR_HAL_CORTEXM_STM32_CLOCK_HCLK_DIV == 2
# define CYGHWR_HAL_STM32_RCC_CFGR_HPRE CYGHWR_HAL_STM32_RCC_CFGR_HPRE_2
#elif CYGHWR_HAL_CORTEXM_STM32_CLOCK_HCLK_DIV == 4
# define CYGHWR_HAL_STM32_RCC_CFGR_HPRE CYGHWR_HAL_STM32_RCC_CFGR_HPRE_4
#elif CYGHWR_HAL_CORTEXM_STM32_CLOCK_HCLK_DIV == 8
# define CYGHWR_HAL_STM32_RCC_CFGR_HPRE CYGHWR_HAL_STM32_RCC_CFGR_HPRE_8
#elif CYGHWR_HAL_CORTEXM_STM32_CLOCK_HCLK_DIV == 16
# define CYGHWR_HAL_STM32_RCC_CFGR_HPRE CYGHWR_HAL_STM32_RCC_CFGR_HPRE_16
#elif CYGHWR_HAL_CORTEXM_STM32_CLOCK_HCLK_DIV == 64
# define CYGHWR_HAL_STM32_RCC_CFGR_HPRE CYGHWR_HAL_STM32_RCC_CFGR_HPRE_64
#elif CYGHWR_HAL_CORTEXM_STM32_CLOCK_HCLK_DIV == 128
# define CYGHWR_HAL_STM32_RCC_CFGR_HPRE CYGHWR_HAL_STM32_RCC_CFGR_HPRE_128
#elif CYGHWR_HAL_CORTEXM_STM32_CLOCK_HCLK_DIV == 256
# define CYGHWR_HAL_STM32_RCC_CFGR_HPRE CYGHWR_HAL_STM32_RCC_CFGR_HPRE_256
#elif CYGHWR_HAL_CORTEXM_STM32_CLOCK_HCLK_DIV == 512
# define CYGHWR_HAL_STM32_RCC_CFGR_HPRE CYGHWR_HAL_STM32_RCC_CFGR_HPRE_512
#endif

#if CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK1_DIV == 1
# define CYGHWR_HAL_STM32_RCC_CFGR_PPRE1 CYGHWR_HAL_STM32_RCC_CFGR_PPRE1_1
#elif CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK1_DIV == 2
# define CYGHWR_HAL_STM32_RCC_CFGR_PPRE1 CYGHWR_HAL_STM32_RCC_CFGR_PPRE1_2
#elif CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK1_DIV == 4
# define CYGHWR_HAL_STM32_RCC_CFGR_PPRE1 CYGHWR_HAL_STM32_RCC_CFGR_PPRE1_4
#elif CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK1_DIV == 8
# define CYGHWR_HAL_STM32_RCC_CFGR_PPRE1 CYGHWR_HAL_STM32_RCC_CFGR_PPRE1_8
#elif CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK1_DIV == 16
# define CYGHWR_HAL_STM32_RCC_CFGR_PPRE1 CYGHWR_HAL_STM32_RCC_CFGR_PPRE1_16
#endif

#if CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK2_DIV == 1
# define CYGHWR_HAL_STM32_RCC_CFGR_PPRE2 CYGHWR_HAL_STM32_RCC_CFGR_PPRE2_1
#elif CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK2_DIV == 2
# define CYGHWR_HAL_STM32_RCC_CFGR_PPRE2 CYGHWR_HAL_STM32_RCC_CFGR_PPRE2_2
#elif CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK2_DIV == 4
# define CYGHWR_HAL_STM32_RCC_CFGR_PPRE2 CYGHWR_HAL_STM32_RCC_CFGR_PPRE2_4
#elif CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK2_DIV == 8
# define CYGHWR_HAL_STM32_RCC_CFGR_PPRE2 CYGHWR_HAL_STM32_RCC_CFGR_PPRE2_8
#elif CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK2_DIV == 16
# define CYGHWR_HAL_STM32_RCC_CFGR_PPRE2 CYGHWR_HAL_STM32_RCC_CFGR_PPRE2_16
#endif


//==========================================================================
// Clock frequencies
//
// These are set to the frequencies of the various system clocks.

cyg_uint32 hal_stm32_sysclk;
cyg_uint32 hal_stm32_hclk;
cyg_uint32 hal_stm32_pclk1;
cyg_uint32 hal_stm32_pclk2;
cyg_uint32 hal_cortexm_systick_clock;

cyg_uint32 hal_exti_isr( cyg_uint32 vector, CYG_ADDRWORD data );

//==========================================================================

void hal_variant_init( void )
{
    CYG_ADDRESS rcc = CYGHWR_HAL_STM32_RCC;

    // Enable all devices in RCC
    {
        HAL_WRITE_UINT32( CYGHWR_HAL_STM32_RCC+CYGHWR_HAL_STM32_RCC_APB2ENR, 0xFFFFFFFF );
        HAL_WRITE_UINT32( CYGHWR_HAL_STM32_RCC+CYGHWR_HAL_STM32_RCC_APB1ENR, 0xFFFFFFFF );
    }


#if 1 //!defined(CYG_HAL_STARTUP_RAM)
    // Set up clocks from configuration. In the future this should be moved to a
    // function so that clock rates can be changed at runtime.
    {
        cyg_uint32 cr, cfgr;

        // Reset RCC

        HAL_WRITE_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, 0x00000001 );
        
        // Start up HSE clock

        HAL_READ_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );
        cr &= CYGHWR_HAL_STM32_RCC_CR_HSEON|CYGHWR_HAL_STM32_RCC_CR_HSEBYP;
        HAL_WRITE_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );

        HAL_READ_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );
        cr |= CYGHWR_HAL_STM32_RCC_CR_HSEON;
        HAL_WRITE_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );
        
        // Wait for HSE clock to startup
        do
        {
            HAL_READ_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );
        } while( !(cr & CYGHWR_HAL_STM32_RCC_CR_HSERDY) );

//        HAL_READ_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CFGR, cfgr );

        // Configure clocks

        hal_stm32_sysclk = CYGARC_HAL_CORTEXM_STM32_INPUT_CLOCK;
        
        cfgr = 0;

#if defined(CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_SOURCE_HSE)
        cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_PLLSRC_HSE;
#elif defined(CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_SOURCE_HSE_HALF)
        cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_PLLSRC_HSE |
                CYGHWR_HAL_STM32_RCC_CFGR_PLLXTPRE;
        hal_stm32_sysclk /= 2;
#elif defined(CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_SOURCE_HSI_HALF)
        hal_stm32_sysclk /= 2;
#endif
        
        cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_PLLMUL(CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_MUL);
        cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_HPRE;
        cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_PPRE1;
        cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_PPRE2;

        HAL_WRITE_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CFGR, cfgr );        

        // Enable the PLL and wait for it to lock

        cr |= CYGHWR_HAL_STM32_RCC_CR_PLLON;

        HAL_WRITE_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );
        do
        {
            HAL_READ_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );
        } while( !(cr & CYGHWR_HAL_STM32_RCC_CR_PLLRDY) );

        // Now switch to use PLL as SYSCLK

        cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_SW_PLL;

        HAL_WRITE_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CFGR, cfgr );        

        // Calculate clocks from configuration

        hal_stm32_sysclk *= CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_MUL;
        hal_stm32_hclk = hal_stm32_sysclk / CYGHWR_HAL_CORTEXM_STM32_CLOCK_HCLK_DIV;
        hal_stm32_pclk1 = hal_stm32_hclk / CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK1_DIV;
        hal_stm32_pclk2 = hal_stm32_hclk / CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK2_DIV;
        hal_cortexm_systick_clock = hal_stm32_hclk / 8;
    }
#endif

    // Attach EXTI springboard to interrupt vectors
    HAL_INTERRUPT_ATTACH( CYGNUM_HAL_INTERRUPT_EXTI9_5,   hal_exti_isr, 0, 0 );
    HAL_INTERRUPT_ATTACH( CYGNUM_HAL_INTERRUPT_EXTI15_10, hal_exti_isr, 0, 0 );

    
#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    hal_if_init();
#endif
}

//==========================================================================
// ISR springboard
//
// This is attached to the ISR table entries for EXTI9_5 and EXTI15_10
// to decode the contents of the EXTI registers and deliver the
// interrupt to the correct ISR.

cyg_uint32 hal_exti_isr( cyg_uint32 vector, CYG_ADDRWORD data )
{
    CYG_ADDRESS base = CYGHWR_HAL_STM32_EXTI;
    cyg_uint32 imr, pr;

    // Get EXTI pending and interrupt mask registers
    HAL_READ_UINT32( base+CYGHWR_HAL_STM32_EXTI_IMR, imr );
    HAL_READ_UINT32( base+CYGHWR_HAL_STM32_EXTI_PR, pr );

    // Mask PR by IMR and lose ls 5 bits
    pr &= imr;
    pr &= 0xFFFFFFE0;

    // Isolate LS pending bit and translate into interrupt vector
    // number.
    HAL_LSBIT_INDEX( vector, pr );
    vector += CYGNUM_HAL_INTERRUPT_EXTI5 - 5;

    // Deliver it
    hal_deliver_interrupt( vector );

    return 0;
}

//==========================================================================
// GPIO support
//
// These functions provide configuration and IO for GPIO pins.

__externC void hal_stm32_gpio_set( cyg_uint32 pin )
{
    cyg_uint32 port = CYGHWR_HAL_STM32_GPIO_PORT(pin);
    int bit = CYGHWR_HAL_STM32_GPIO_BIT(pin);
    cyg_uint32 cm = CYGHWR_HAL_STM32_GPIO_CFG(pin);
    cyg_uint32 cr;

    if( pin == CYGHWR_HAL_STM32_GPIO_NONE )
        return;
    
    if( bit > 7 ) port += 4, bit -= 8;
    HAL_READ_UINT32( port, cr );
    cr &= ~(0xF<<(bit*4));
    cr |= cm<<(bit*4);
    HAL_WRITE_UINT32( port, cr );

    // If this is a pullup/down input, set the ODR bit to switch on
    // the appropriate pullup/down resistor.
    if( cm == (CYGHWR_HAL_STM32_GPIO_MODE_IN|CYGHWR_HAL_STM32_GPIO_CNF_PULL) )
    {
        cyg_uint32 odr;
        port = CYGHWR_HAL_STM32_GPIO_PORT( pin );
        bit = CYGHWR_HAL_STM32_GPIO_BIT(pin);
        HAL_READ_UINT32( port+CYGHWR_HAL_STM32_GPIO_ODR, odr );
        if( pin & CYGHWR_HAL_STM32_GPIO_PULLUP )
            odr |= (1<<bit);
        else
            odr &= ~(1<<bit);
        HAL_WRITE_UINT32( port+CYGHWR_HAL_STM32_GPIO_ODR, odr );
    }
}
    
__externC void hal_stm32_gpio_out( cyg_uint32 pin, int val )
{
    cyg_uint32 port = CYGHWR_HAL_STM32_GPIO_PORT(pin);
    int bit = CYGHWR_HAL_STM32_GPIO_BIT(pin);
    
    port += CYGHWR_HAL_STM32_GPIO_BSRR;
    if( (val&1) == 0 ) port += 4;
    HAL_WRITE_UINT32( port, 1<<bit );
}
    
__externC void hal_stm32_gpio_in ( cyg_uint32 pin, int *val )
{
    cyg_uint32 port = CYGHWR_HAL_STM32_GPIO_PORT(pin);
    int bit = CYGHWR_HAL_STM32_GPIO_BIT(pin);
    cyg_uint32 pd;
    
    HAL_READ_UINT32( port+CYGHWR_HAL_STM32_GPIO_IDR, pd );
    *val = (pd>>bit)&1;
}

//==========================================================================
// Backup domain

void hal_stm32_bd_protect( int protect )
{
    CYG_ADDRESS pwr = CYGHWR_HAL_STM32_PWR;
    cyg_uint32 cr;
    
    HAL_READ_UINT32( pwr+CYGHWR_HAL_STM32_PWR_CR, cr );
    if( protect )
        cr &= ~CYGHWR_HAL_STM32_PWR_CR_DBP;
    else
        cr |= CYGHWR_HAL_STM32_PWR_CR_DBP;        

    HAL_WRITE_UINT32( pwr+CYGHWR_HAL_STM32_PWR_CR, cr );
}

//==========================================================================
// UART baud rate
//
// Set the baud rate divider of a UART based on the requested rate and
// the current APB clock settings.

void hal_stm32_uart_setbaud( cyg_uint32 base, cyg_uint32 baud )
{
    cyg_uint32 apbclk = hal_stm32_pclk1;
    cyg_uint32 int_div, frac_div;
    cyg_uint32 brr;
    
    if( base == CYGHWR_HAL_STM32_UART1 )
        apbclk = hal_stm32_pclk2;

    int_div = (25 * apbclk ) / (4 * baud );
    brr = ( int_div / 100 ) << 4;
    frac_div = int_div - (( brr >> 4 ) * 100 );

    brr |= (((frac_div * 16 ) + 50 ) / 100) & 0xF;

    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_UART_BRR, brr );
}

//==========================================================================
// EOF stm32_misc.c
