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
// Copyright (C) 2008, 2009, 2011 Free Software Foundation, Inc.                        
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
// Contributors: jld
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

#ifdef CYGFUN_HAL_CORTEXM_STM32_PROFILE_TIMER
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED
#include <cyg/profile/profile.h>        // __profile_hit()
#endif

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
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
cyg_uint32 hal_stm32_qclk;
#endif

void hal_start_clocks( void );
cyg_uint32 hal_exti_isr( cyg_uint32 vector, CYG_ADDRWORD data );

//==========================================================================

void hal_variant_init( void )
{

#if 1 //!defined(CYG_HAL_STARTUP_RAM)
    hal_start_clocks();
#endif

    // Attach EXTI springboard to interrupt vectors
    HAL_INTERRUPT_ATTACH( CYGNUM_HAL_INTERRUPT_EXTI9_5,   hal_exti_isr, 0, 0 );
    HAL_INTERRUPT_ATTACH( CYGNUM_HAL_INTERRUPT_EXTI15_10, hal_exti_isr, 0, 0 );
    
#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    hal_if_init();
#endif
}

//==========================================================================
// Setup up system clocks
//
// Set up clocks from configuration. In the future this should be extended so
// that clock rates can be changed at runtime.

void hal_start_clocks( void )
{
    CYG_ADDRESS rcc = CYGHWR_HAL_STM32_RCC;
    cyg_uint32 cr, cfgr;
    
    // Reset RCC

    cr = CYGHWR_HAL_STM32_RCC_CR_HSION;
    HAL_WRITE_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );
    
    // Start up HSE clock
    
    cr |= CYGHWR_HAL_STM32_RCC_CR_HSEON;
    HAL_WRITE_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );
    
    // Wait for HSE clock to startup
    
    do
    {
        HAL_READ_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );
    } while( !(cr & CYGHWR_HAL_STM32_RCC_CR_HSERDY) );

    // Configure clocks
    
    // Temporarily divide by 4 until we've dealt with potential large
    // multiplications overflow.
    hal_stm32_sysclk = CYGARC_HAL_CORTEXM_STM32_INPUT_CLOCK >> 2;
    
    cfgr = 0;

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)

#if defined(CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_SOURCE_HSE)
    cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_PLLSRC_HSE;
#endif

    // Just a little sanity check.
#if defined(CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_SOURCE_HSI) && (CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_PREDIV != 2)
# error PLL PREDIV must be 2
#endif

    // Ordering could be important if divisions below cause truncation, so multiply first.
    hal_stm32_sysclk *= CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_MUL;

#ifdef CYGHWR_HAL_CORTEXM_STM32_CONNECTIVITY
    HAL_WRITE_UINT32( rcc + CYGHWR_HAL_STM32_RCC_CFGR2,
                      CYGHWR_HAL_STM32_RCC_CFGR2_PREDIV1(CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_PREDIV-1) );
    hal_stm32_sysclk /= CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_PREDIV;
#else
    // Non-connectivity parts can only use PLLXTPRE
    if ( CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_PREDIV == 2 )
    {
        cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_PLLXTPRE; // irrelevant if HSI used, so just set anyway.
    hal_stm32_sysclk /= 2;
    }
#endif
    hal_stm32_sysclk <<= 2; // return to correct range now we've dealt with risk of overflow.
    
    cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_PLLMUL(CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_MUL);
    cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_HPRE;
    cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_PPRE1;
    cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_PPRE2;

#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

#if defined(CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_SOURCE_HSE)
    cfgr |= CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLSRC_HSE;
#endif

    cfgr |= CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLM(CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_PREDIV);
    cfgr |= CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLN(CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_MUL);
    // Ordering could be important if divisions below cause truncation, so multiply first.
    hal_stm32_sysclk *= CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_MUL;
    hal_stm32_sysclk /= CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_PREDIV;

    hal_stm32_sysclk <<= 2; // return to correct range now we've dealt with risk of overflow.

    cfgr |= CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLP(CYGHWR_HAL_CORTEXM_STM32_CLOCK_SYSCLK_DIV);
    cfgr |= CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLQ(CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLLQ_DIV);

    // qclk divides down VCO output, so calc it first before updating sysclk for PLLP
    hal_stm32_qclk = hal_stm32_sysclk / CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLLQ_DIV;

    hal_stm32_sysclk /= CYGHWR_HAL_CORTEXM_STM32_CLOCK_SYSCLK_DIV;

    HAL_WRITE_UINT32( rcc + CYGHWR_HAL_STM32_RCC_PLLCFGR, cfgr );

    cfgr  = CYGHWR_HAL_STM32_RCC_CFGR_HPRE;
    cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_PPRE1;
    cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_PPRE2;

    // RTCPRE divides down HSE, which is the input clock. Must be 1MHz.
    cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_RTCPRE( CYGARC_HAL_CORTEXM_STM32_INPUT_CLOCK/1000000 );

#endif // elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

    HAL_WRITE_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CFGR, cfgr );

    // Enable the PLL and wait for it to lock
    
    cr |= CYGHWR_HAL_STM32_RCC_CR_PLLON;

    HAL_WRITE_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );
    do
    {
        HAL_READ_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );
    } while( !(cr & CYGHWR_HAL_STM32_RCC_CR_PLLRDY) );

    // Now switch to use PLL as SYSCLK
    // TODO: make this configurable between HSI, HSE and PLL
    
    cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_SW_PLL;

    HAL_WRITE_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CFGR, cfgr );
    do
    {
        HAL_READ_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CFGR, cfgr );
    } while( (cfgr & CYGHWR_HAL_STM32_RCC_CFGR_SWS_XXX) != 
             CYGHWR_HAL_STM32_RCC_CFGR_SWS_PLL );

    // Calculate clocks from configuration

    hal_stm32_hclk = hal_stm32_sysclk / CYGHWR_HAL_CORTEXM_STM32_CLOCK_HCLK_DIV;
    hal_stm32_pclk1 = hal_stm32_hclk / CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK1_DIV;
    hal_stm32_pclk2 = hal_stm32_hclk / CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK2_DIV;
#ifdef CYGHWR_HAL_CORTEXM_SYSTICK_CLK_SOURCE_INTERNAL
    hal_cortexm_systick_clock = hal_stm32_hclk;
#else
    hal_cortexm_systick_clock = hal_stm32_hclk / 8;
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
  // FIXME: Power on GPIO ports selectively here, rather than
  // platform having to power them all on for boot.
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
    cyg_uint32 port = CYGHWR_HAL_STM32_GPIO_PORT(pin);
    int bit = CYGHWR_HAL_STM32_GPIO_BIT(pin);
    cyg_uint32 cm = CYGHWR_HAL_STM32_GPIO_CFG(pin);
    cyg_uint32 cr;

    if( pin == CYGHWR_HAL_STM32_GPIO_NONE )
        return;
    
    if( bit > 7 ) port += 4, bit -= 8;
    HAL_READ_UINT32( port, cr );
    CYGHWR_HAL_STM32_GPIO_CNFMODE_SET(bit,cm,cr);
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
#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
    CYG_ADDRESS port = CYGHWR_HAL_STM32_GPIO_PORT(pin);
    int bit = CYGHWR_HAL_STM32_GPIO_BIT(pin);
    cyg_uint32 mode = CYGHWR_HAL_STM32_GPIO_MODE(pin);
    cyg_uint32 af = CYGHWR_HAL_STM32_GPIO_AF(pin);
    cyg_uint32 od = CYGHWR_HAL_STM32_GPIO_OPENDRAIN(pin);
    cyg_uint32 pupd = CYGHWR_HAL_STM32_GPIO_PULLUPDOWN(pin);
    cyg_uint32 speed = CYGHWR_HAL_STM32_GPIO_SPEED(pin);
    cyg_uint32 reg;

    if( pin == CYGHWR_HAL_STM32_GPIO_NONE )
        return;

    
    HAL_READ_UINT32( port+CYGHWR_HAL_STM32_GPIO_OTYPER, reg );
    CYGHWR_HAL_STM32_GPIO_OTYPE_SET( bit, od, reg );
    HAL_WRITE_UINT32( port+CYGHWR_HAL_STM32_GPIO_OTYPER, reg );

    HAL_READ_UINT32( port+CYGHWR_HAL_STM32_GPIO_OSPEEDR, reg );
    CYGHWR_HAL_STM32_GPIO_OSPEED_SET( bit, speed, reg );
    HAL_WRITE_UINT32( port+CYGHWR_HAL_STM32_GPIO_OSPEEDR, reg );

    HAL_READ_UINT32( port+CYGHWR_HAL_STM32_GPIO_PUPDR, reg );
    CYGHWR_HAL_STM32_GPIO_PUPD_SET( bit, pupd, reg );
    HAL_WRITE_UINT32( port+CYGHWR_HAL_STM32_GPIO_PUPDR, reg );

    if ( CYGHWR_HAL_STM32_GPIO_MODE_ALTFN == mode )
    {
        CYGHWR_HAL_STM32_GPIO_AFR_SET( port, bit, af );
    }

    HAL_READ_UINT32( port+CYGHWR_HAL_STM32_GPIO_MODER, reg );
    CYGHWR_HAL_STM32_GPIO_MODE_SET( bit, mode, reg );
    HAL_WRITE_UINT32( port+CYGHWR_HAL_STM32_GPIO_MODER, reg );
#endif // if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
}
    
__externC void hal_stm32_gpio_out( cyg_uint32 pin, int val )
{
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
    cyg_uint32 port = CYGHWR_HAL_STM32_GPIO_PORT(pin);
    int bit = CYGHWR_HAL_STM32_GPIO_BIT(pin);
    
    port += CYGHWR_HAL_STM32_GPIO_BSRR;
    if( (val&1) == 0 ) port += 4;
    HAL_WRITE_UINT32( port, 1<<bit );
#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
    CYG_ADDRESS port = CYGHWR_HAL_STM32_GPIO_PORT(pin);
    int bit = CYGHWR_HAL_STM32_GPIO_BIT(pin);

    CYGHWR_HAL_STM32_GPIO_BSRR_SET( port, bit, val );
#endif // if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
}
    
__externC void hal_stm32_gpio_in ( cyg_uint32 pin, int *val )
{
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
    cyg_uint32 port = CYGHWR_HAL_STM32_GPIO_PORT(pin);
    int bit = CYGHWR_HAL_STM32_GPIO_BIT(pin);
    cyg_uint32 pd;
    
    HAL_READ_UINT32( port+CYGHWR_HAL_STM32_GPIO_IDR, pd );
    *val = (pd>>bit)&1;
#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
    CYG_ADDRESS port = CYGHWR_HAL_STM32_GPIO_PORT(pin);
    int bit = CYGHWR_HAL_STM32_GPIO_BIT(pin);

    CYGHWR_HAL_STM32_GPIO_IDR_GET( port, bit, *val );
#endif // if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
}

//==========================================================================
// Clock support.
//
// These functions provide support for enabling and disabling clock
// control bits.

__externC void hal_stm32_clock_enable( cyg_uint32 desc )
{
    cyg_uint32 r;
    cyg_uint32 reg = CYGHWR_HAL_STM32_RCC+CYGHWR_HAL_STM32_CLOCK_REG(desc);
    HAL_READ_UINT32( reg, r );
    r |= BIT_(CYGHWR_HAL_STM32_CLOCK_PIN(desc));
    HAL_WRITE_UINT32( reg, r );
}

__externC void hal_stm32_clock_disable( cyg_uint32 desc )
{
    cyg_uint32 r;
    cyg_uint32 reg = CYGHWR_HAL_STM32_RCC+CYGHWR_HAL_STM32_CLOCK_REG(desc);
    HAL_READ_UINT32( reg, r );
    r &= ~BIT_(CYGHWR_HAL_STM32_CLOCK_PIN(desc));
    HAL_WRITE_UINT32( reg, r );
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
    
    if( base == CYGHWR_HAL_STM32_UART1 ||
#ifdef CYGHWR_HAL_STM32_UART6
        base == CYGHWR_HAL_STM32_UART6 ||
#endif
        0)
    {
        apbclk = hal_stm32_pclk2;
    }

    int_div = (25 * apbclk ) / (4 * baud );
    brr = ( int_div / 100 ) << 4;
    frac_div = int_div - (( brr >> 4 ) * 100 );

    brr |= (((frac_div * 16 ) + 50 ) / 100) & 0xF;

    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_UART_BRR, brr );
}

//==========================================================================
// Timer clock rate
//
// Returns the current timer clock rate of a timer.

cyg_uint32 hal_stm32_timer_clock( CYG_ADDRESS base )
{
    if( base == CYGHWR_HAL_STM32_TIM1 ||
        base == CYGHWR_HAL_STM32_TIM8 )
    {
#if CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK2_DIV == 1
        return hal_stm32_pclk2;
#else
        return hal_stm32_pclk2 << 1;
#endif
    } else {
#if CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK1_DIV == 1
        return hal_stm32_pclk1;
#else
        return hal_stm32_pclk1 << 1;
#endif
    }
}

//==========================================================================
// Profiling timer
//
// Implementation of profiling support using general-purpose timer TIM2.

#ifdef CYGFUN_HAL_CORTEXM_STM32_PROFILE_TIMER
// Use TIM2 for profiling
#define STM32_TIMER_PROFILE CYGHWR_HAL_STM32_TIM2
#define HAL_INTERRUPT_PROFILE CYGNUM_HAL_INTERRUPT_TIM2

// Profiling timer ISR
static cyg_uint32 profile_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    extern HAL_SavedRegisters *hal_saved_interrupt_state;

    HAL_WRITE_UINT32(STM32_TIMER_PROFILE+CYGHWR_HAL_STM32_TIM_SR, 0); // clear interrupt pending flag
    HAL_INTERRUPT_ACKNOWLEDGE(HAL_INTERRUPT_PROFILE);
    __profile_hit(hal_saved_interrupt_state->u.interrupt.pc);
    return CYG_ISR_HANDLED;
}

// Profiling timer setup
int hal_enable_profile_timer(int resolution)
{
    CYG_ASSERT(resolution < 0x10000, "Invalid profile timer resolution"); // 16 bits only

    // Attach ISR
    HAL_INTERRUPT_ATTACH(HAL_INTERRUPT_PROFILE, &profile_isr, 0x1111, 0);
    HAL_INTERRUPT_UNMASK(HAL_INTERRUPT_PROFILE);

    // Setup timer
    HAL_WRITE_UINT32(STM32_TIMER_PROFILE+CYGHWR_HAL_STM32_TIM_PSC,
        (hal_stm32_timer_clock(STM32_TIMER_PROFILE) / 1000000) - 1); // prescale to microseconds
    HAL_WRITE_UINT32(STM32_TIMER_PROFILE+CYGHWR_HAL_STM32_TIM_CR2, 0);
    HAL_WRITE_UINT32(STM32_TIMER_PROFILE+CYGHWR_HAL_STM32_TIM_DIER, CYGHWR_HAL_STM32_TIM_DIER_UIE);
    HAL_WRITE_UINT32(STM32_TIMER_PROFILE+CYGHWR_HAL_STM32_TIM_ARR, resolution);
    HAL_WRITE_UINT32(STM32_TIMER_PROFILE+CYGHWR_HAL_STM32_TIM_CR1, CYGHWR_HAL_STM32_TIM_CR1_CEN);

    return resolution;
}

#endif // CYGFUN_HAL_CORTEXM_STM32_PROFILE_TIMER

//==========================================================================
// EOF stm32_misc.c
