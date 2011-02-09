//==========================================================================
//
//      lm3s8xx_misc.c
//
//      Stellaris Cortex-M3 800 Series HAL functions
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2010 Free Software Foundation, Inc.
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
// Date:         2010-11-21
// Description:  
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

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
#include <cyg/hal/var_io.h>

#if defined(CYGHWR_HAL_CORTEXM_LM3S8XX_PLL)
#if CYGHWR_HAL_CORTEXM_LM3S8XX_PLL_INPUT == 3579545
#define CYGHWR_HAL_CORTEXM_LM3S8XX_XTAL CYGHWR_HAL_LM3S_SC_RCC_XTAL(4)
#elif CYGHWR_HAL_CORTEXM_LM3S8XX_PLL_INPUT == 3686400
#define CYGHWR_HAL_CORTEXM_LM3S8XX_XTAL  CYGHWR_HAL_LM3S_SC_RCC_XTAL(5)
#elif CYGHWR_HAL_CORTEXM_LM3S8XX_PLL_INPUT == 4000000
#define CYGHWR_HAL_CORTEXM_LM3S8XX_XTAL  CYGHWR_HAL_LM3S_SC_RCC_XTAL(6)
#elif CYGHWR_HAL_CORTEXM_LM3S8XX_PLL_INPUT == 4096000
#define CYGHWR_HAL_CORTEXM_LM3S8XX_XTAL  CYGHWR_HAL_LM3S_SC_RCC_XTAL(7)
#elif CYGHWR_HAL_CORTEXM_LM3S8XX_PLL_INPUT == 4915200
#define CYGHWR_HAL_CORTEXM_LM3S8XX_XTAL  CYGHWR_HAL_LM3S_SC_RCC_XTAL(8)
#elif CYGHWR_HAL_CORTEXM_LM3S8XX_PLL_INPUT == 5000000
#define CYGHWR_HAL_CORTEXM_LM3S8XX_XTAL  CYGHWR_HAL_LM3S_SC_RCC_XTAL(9)
#elif CYGHWR_HAL_CORTEXM_LM3S8XX_PLL_INPUT == 5120000
#define CYGHWR_HAL_CORTEXM_LM3S8XX_XTAL  CYGHWR_HAL_LM3S_SC_RCC_XTAL(10)
#elif CYGHWR_HAL_CORTEXM_LM3S8XX_PLL_INPUT == 6000000
#define CYGHWR_HAL_CORTEXM_LM3S8XX_XTAL  CYGHWR_HAL_LM3S_SC_RCC_XTAL(11)
#elif CYGHWR_HAL_CORTEXM_LM3S8XX_PLL_INPUT == 6144000
#define CYGHWR_HAL_CORTEXM_LM3S8XX_XTAL  CYGHWR_HAL_LM3S_SC_RCC_XTAL(12)
#elif CYGHWR_HAL_CORTEXM_LM3S8XX_PLL_INPUT == 7372800
#define CYGHWR_HAL_CORTEXM_LM3S8XX_XTAL  CYGHWR_HAL_LM3S_SC_RCC_XTAL(13)
#elif CYGHWR_HAL_CORTEXM_LM3S8XX_PLL_INPUT == 8000000
#define CYGHWR_HAL_CORTEXM_LM3S8XX_XTAL  CYGHWR_HAL_LM3S_SC_RCC_XTAL(14)
#elif CYGHWR_HAL_CORTEXM_LM3S8XX_PLL_INPUT == 8192000
#define CYGHWR_HAL_CORTEXM_LM3S8XX_XTAL  CYGHWR_HAL_LM3S_SC_RCC_XTAL(15)
#endif
#endif

#if defined(CYGHWR_HAL_CORTEXM_LM3S8XX_CLOCK_INT)
#if CYGNUM_HAL_CORTEXM_LM3S8XX_CLOCK_INT_FREQ == 12000000
#define CYGHWR_HAL_CORTEXM_LM3S8XX_OSCSRC_FIELD CYGHWR_HAL_LM3S_SC_RCC_OSCSRC_IOSC
#else
#define CYGHWR_HAL_CORTEXM_LM3S8XX_OSCSRC_FIELD CYGHWR_HAL_LM3S_SC_RCC_OSCSRC_IOSC_DIV4
#endif
#else
#define CYGHWR_HAL_CORTEXM_LM3S8XX_OSCSRC_FIELD CYGHWR_HAL_LM3S_SC_RCC_OSCSRC_MOSC
#endif

#if CYGHWR_HAL_CORTEXM_LM3S8XX_SYSCLK_DIV != 1
#define CYGHWR_HAL_CORTEXM_LM3S8XX_SC_RCC_SYSDIV_VAL \
        CYGHWR_HAL_LM3S_SC_RCC_SYSDIV( (CYGHWR_HAL_CORTEXM_LM3S8XX_SYSCLK_DIV-1) );
#endif

//==========================================================================

void hal_lm3s8xx_periph_set( cyg_uint32 periph, cyg_uint32 on_off );

__externC cyg_uint32 hal_cortexm_systick_clock;
__externC cyg_uint32 hal_lm3s_sysclk;

//==========================================================================
// System init
//
// This code runs before the DATA is copied from ROM and the BSS cleared,
// hence it cannot make use of static variables or data tables.

__externC void
hal_system_init( void )
{
}


//==========================================================================
// Setup up system clocks
//
void
hal_start_clocks( void )
{
    CYG_ADDRESS     sc = CYGHWR_HAL_LM3S_SC;
    cyg_uint32      rcc;
#if defined(CYGHWR_HAL_CORTEXM_LM3S8XX_PLL)
    cyg_uint32      plllmis = CYGHWR_HAL_LM3S_SC_MISC_PLLLMIS;
    cyg_uint32      plllris = CYGHWR_HAL_LM3S_SC_RIS_PLLLRIS;
    volatile cyg_uint16 wait;
#endif

    // At power up, the LM3S8xx is setup to use external oscillator.
    // The PLL is powered down and bypass. Same goes for the system
    // clock divider.

    // For JTAG cold restart, first we make sure the PLL and system
    // clock divider are bypassed, enable all clock source and shutdown
    // the PLL.

    HAL_READ_UINT32( sc + CYGHWR_HAL_LM3S_SC_RCC, rcc );

    rcc &= ~( CYGHWR_HAL_LM3S_SC_RCC_USESYSDIV |
              CYGHWR_HAL_LM3S_SC_RCC_MOSCDIS |
              CYGHWR_HAL_LM3S_SC_RCC_IOSCDIS );

    rcc |= CYGHWR_HAL_LM3S_SC_RCC_BYPASS;

    HAL_WRITE_UINT32( sc + CYGHWR_HAL_LM3S_SC_RCC, rcc );

    rcc |= ( CYGHWR_HAL_LM3S_SC_RCC_PWRDN | CYGHWR_HAL_LM3S_SC_RCC_OEN );

    HAL_WRITE_UINT32( sc + CYGHWR_HAL_LM3S_SC_RCC, rcc );

    // PLL is setup if in use
    //
    // The XTAL frequency is configured. The PLL is powered and
    // its output is enable

    // Setup Clock Source
    rcc |= CYGHWR_HAL_CORTEXM_LM3S8XX_OSCSRC_FIELD;

#if defined(CYGHWR_HAL_CORTEXM_LM3S8XX_PLL)

    // Clear PLL lock bit
    HAL_WRITE_UINT32( sc + CYGHWR_HAL_LM3S_SC_MISC, plllmis );

    rcc &= ~( CYGHWR_HAL_LM3S_SC_RCC_XTAL_MASK |
              ( CYGHWR_HAL_LM3S_SC_RCC_PWRDN ) |
              ( CYGHWR_HAL_LM3S_SC_RCC_OEN ) );

    rcc |= CYGHWR_HAL_CORTEXM_LM3S8XX_XTAL;

#endif

    HAL_WRITE_UINT32( sc + CYGHWR_HAL_LM3S_SC_RCC, rcc );

    //
    // Setup System Clock divider
    //

#if CYGHWR_HAL_CORTEXM_LM3S8XX_SYSCLK_DIV != 1

    // Use system clock divider
    rcc |= CYGHWR_HAL_LM3S_SC_RCC_USESYSDIV;

    // Clear system clock divider bits
    rcc &= ~CYGHWR_HAL_LM3S_SC_RCC_SYSDIV_MASK;

    // Configure divider
    rcc |= CYGHWR_HAL_CORTEXM_LM3S8XX_SC_RCC_SYSDIV_VAL;

    HAL_WRITE_UINT32( sc + CYGHWR_HAL_LM3S_SC_RCC, rcc );

#endif

    // Wait for PLL lock before feeding the clock to the
    // device

#if defined(CYGHWR_HAL_CORTEXM_LM3S8XX_PLL)

    // Wait for PLL lock, potentially a dead lock
    plllris = 0;
    while ( 0 == ( plllris & CYGHWR_HAL_LM3S_SC_RIS_PLLLRIS ) ) {
        // Wait
        for ( wait = 0; wait < ( ( 2 ^ 16 ) - 1 ); wait++ )
            HAL_READ_UINT32( sc + CYGHWR_HAL_LM3S_SC_RIS, plllris );
    }

    // Clear bypass bit
    rcc &= ~CYGHWR_HAL_LM3S_SC_RCC_BYPASS;

    HAL_WRITE_UINT32( sc + CYGHWR_HAL_LM3S_SC_RCC, rcc );

#endif

    //
    // Disable clock source not in use
    //
#if defined(CYGHWR_HAL_CORTEXM_LM3S8XX_CLOCK_EXT)

    rcc |= CYGHWR_HAL_LM3S_SC_RCC_IOSCDIS;

#elif defined(CYGHWR_HAL_CORTEXM_LM3S8XX_CLOCK_INT)

    rcc |= CYGHWR_HAL_LM3S_SC_RCC_MOSCDIS;

#endif

    HAL_WRITE_UINT32( sc + CYGHWR_HAL_LM3S_SC_RCC, rcc );

    hal_cortexm_systick_clock = CYGHWR_HAL_CORTEXM_LM3S8XX_SYSCLK;
    hal_lm3s_sysclk = CYGHWR_HAL_CORTEXM_LM3S8XX_SYSCLK;
}

//==========================================================================
// EOF lm3s8xx_misc.c
