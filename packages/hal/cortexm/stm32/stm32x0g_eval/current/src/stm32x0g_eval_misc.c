/*==========================================================================
//
//      stm32x0g_eval_misc.c
//
//      Cortex-M3/-M4 STM32X0G EVAL HAL functions
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2011, 2012 Free Software Foundation, Inc.
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
// Author(s):    jlarmour based on stm3210e by nickg
// Date:         2008-07-30
// Description:  
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm.h>
#include <pkgconf/hal_cortexm_stm32.h>
#include <pkgconf/hal_cortexm_stm32_stm32x0g_eval.h>
#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif

#include CYGHWR_MEMORY_LAYOUT_H         // Memory regions

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_intr.h>           // HAL header
#include <cyg/hal/hal_if.h>             // HAL header

//==========================================================================

#if defined(CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT)
#if (CYGNUM_CALL_IF_TABLE_SIZE > 64)
// We force a compilation error for this fatal condition since run-time asserts
// may not be enabled for the build.
#error "The CALL_IF_TABLE_SIZE pre-allocation in the linker scripts for this platform need to be updated"
#endif
#endif // CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT

//==========================================================================
// System init
//
// This is run to set up the basic system, including GPIO setting,
// clock feeds, power supply, and memory initialization. This code
// runs before the DATA is copied from ROM and the BSS cleared, hence
// it cannot make use of static variables or data tables. 

__externC void hal_system_init( void )
{
    CYG_ADDRESS base;
    
    // Enable peripheral clocks in RCC

    base = CYGHWR_HAL_STM32_RCC;

    // All GPIO ports
    // FIXME: this should be done in variant HAL at point of gpio_set
    HAL_WRITE_UINT32(base+CYGHWR_HAL_STM32_RCC_AHB1ENR,
#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_F4) // enable CCM clock
                     BIT_(CYGHWR_HAL_STM32_RCC_AHB1ENR_CCMDATARAMEN) |
#endif // CYGHWR_HAL_CORTEXM_STM32_FAMILY_F4
                     BIT_(CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOA) |
                     BIT_(CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOB) |
                     BIT_(CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOC) |
                     BIT_(CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOD) |
                     BIT_(CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOE) |
                     BIT_(CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOF) |
                     BIT_(CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOG) |
                     BIT_(CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOH) |
                     BIT_(CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOI) );

    // Enable FSMC
    HAL_WRITE_UINT32(base+CYGHWR_HAL_STM32_RCC_AHB3ENR,
                     BIT_(CYGHWR_HAL_STM32_RCC_AHB3ENR_FSMC) );

#if defined(CYG_HAL_STARTUP_ROM) | defined(CYG_HAL_STARTUP_ROMINT) | defined(CYG_HAL_STARTUP_SRAM)
    
    // Reset FSMC in case it was already enabled. This should set
    // all regs back to default documented values, so we don't need
    // to do any precautionary resets.
    HAL_WRITE_UINT32(base+CYGHWR_HAL_STM32_RCC_AHB3RSTR,
                     BIT_(CYGHWR_HAL_STM32_RCC_AHB3ENR_FSMC) );
    // Bring out of reset:
    HAL_WRITE_UINT32(base+CYGHWR_HAL_STM32_RCC_AHB3RSTR, 0 );
#endif

    // Set all unused GPIO lines to input with pull down to prevent
    // them floating and annoying any external hardware.

    // GPIO Ports C..I reset GPIOx_MODER to 0x00000000
    // GPIO Ports A..I reset GPIOx_OTYPER to 0x00000000
    // CPIO Ports A,C..I reset GPIOx_OSPEEDR to 0x00000000
    // GPIO Ports C..I reset GPIOx_PUPDR to 0x00000000

    // GPIO Port A resets GPIOA_MODER to 0xA8000000
    // GPIO Port A resets GPIOA_PUPDR to 0x64000000
    // GPIO Port A keeps the default JTAG pins on PA13,14,15
    base = CYGHWR_HAL_STM32_GPIOA;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_PUPDR, 0x02AAAAAA );

    // GPIO Port B resets GPIOB_MODER to 0x00000280
    // GPIO Port B resets GPIOB_OSPEEDR to 0x000000C0
    // GPIO Port B resets GPIOB_PUPDR to 0x00000100
    // GPIO Port B keeps the default JTAG pins on PB3,4
    base = CYGHWR_HAL_STM32_GPIOB;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_PUPDR, 0xAAAAA82A );

    // GPIO Port C - setup PC7 for LED4 as GPIO out, RS232 (USART4) on PC10,11.
    // Rest stay default, with pulldowns on all except PC14,15 (OSC32)
    // just in case that is important.

    base = CYGHWR_HAL_STM32_GPIOC;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_AFRH, 0x00008800 );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_PUPDR, 0x0A0A2AAA );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_MODER, 0x00A04000 );

    // GPIO Port D - setup FSMC for SRAM (PD0-1,3-15) and MicroSDcard (PD2) alternate functions
    base = CYGHWR_HAL_STM32_GPIOD;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_AFRL, 0xCCCCCCCC );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_AFRH, 0xCCCCCCCC );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_MODER, 0xAAAAAAAA );
    // TODO:CONSIDER: OSPEEDR for SRAM pins to 100MHz

    // GPIO Port E - setup FSMC alternate function. PE0-1,3-4,7-15.
    // But not PE5 (A21), PE6(A22), PE2(A23) which are not connected to SRAM.
    base = CYGHWR_HAL_STM32_GPIOE;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_AFRL, 0xC00CC0CC );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_AFRH, 0xCCCCCCCC );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_PUPDR, 0x00002820 );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_MODER, 0xAAAA828A );
    // TODO:CONSIDER: OSPEEDR for SRAM pins to 100MHz

    // GPIO Port F - setup FSMC alternate function. PF0-5,12-15.
    // But not PF6-11 which aren't connected to SRAM.
    base = CYGHWR_HAL_STM32_GPIOF;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_AFRL, 0x00CCCCCC );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_AFRH, 0xCCCC0000 );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_PUPDR, 0x00AAA000 );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_MODER, 0xAA000AAA );
    // TODO:CONSIDER: OSPEEDR for SRAM pins to 100MHz

    // GPIO Port G - setup FSMC alternate function. PG0-5,9,10.
    // Other FSMC pins not connected to SRAM.
    // LED1 is PG6, LED2 is PG8, so set as GPIO out.
    base = CYGHWR_HAL_STM32_GPIOG;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_AFRL, 0x00CCCCCC );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_AFRH, 0x00000CC0 );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_PUPDR, 0xAA808000 );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_MODER, 0x00291AAA );
    // TODO:CONSIDER: OSPEEDR for SRAM pins to 100MHz

    // GPIO Port H stays default, with pulldowns on all except PH0,1 (OSC) just in case that is important.
    base = CYGHWR_HAL_STM32_GPIOH;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_PUPDR, 0xAAAAAAA0 );

    // GPIO Port I - setup PI9 for LED3 as GPIO out, rest stay default, with pulldowns
    base = CYGHWR_HAL_STM32_GPIOI;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_PUPDR, 0xAAA2AAAA );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_MODER, 0x00040000 );


    // Set up FSMC NOR/SRAM bank 2 for SRAM

    base = CYGHWR_HAL_STM32_FSMC;


#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_F4)
    // NOTEs:
    // - The "STM32 20-21-45-46 G-EVAL" boards we have are populated with the
    //   IS61WV102416BLL-10MLI part and not the Cypress CY7C1071DV33 part.
    // - The F4[01][57]xx devices can operate upto 168MHz (or 144MHz) so maximum HCLK
    //   timing of 6ns (or 6.94444ns).
    //
    // NOTE: The code does NOT set BWTR2 for SRAM write-cycle timing (so will be
    // the default reset value of 0x0FFFFFFF) since BCRx:EXTMOD bit is NOT set.

    // TODO:IMPROVE: Derive values based on CLK settings. The following "fixed"
    // values are based on a 168MHz SYSCLK:

    // BCR2 = MBKEN | MWID=0b01 (16bits) | WREN
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FSMC_BCR2, 0x00001015 );

    // BTR2:
    //   ADDSET=3 (3 HCLK cycles)
    //   ADDHLD=0 (SRAM:do not care)
    //   DATAST=6 (6 HCLK cycles)
    //   BUSTURN=1 (1 HCLK cycle)
    //   CLKDIV=0 (SRAM:do not care)
    //   DATLAT=0 (SRAM:do not care)
    //   ACCMOD=0 (access mode A)
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FSMC_BTR2, 0x00010603 );
#else // CYGHWR_HAL_CORTEXM_STM32_FAMILY_F2
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FSMC_BCR2, 0x00001011 );
    // SRAM timings for the fitted CY7C1071DV33-12BAXI async SRAM
    // We could try and make this depend on hclk as it should, but that's
    // probably overkill for now. With an hclk of 120MHz, each hclk period
    // is 8.33ns, so we just use that. This might mean being slightly
    // suboptimal at lower configured hclk speeds.
    // It's tricky to get the waveforms in the STM32 FSMC docs and the SRAM
    // datasheet, to match up, so there's a small amount of guess work involved
    // here. From the SRAM datasheet, ADDSET should be at least 7ns (tHZWE), and
    // DATAST should be at least 9ns (tPWE) plus one HCLK (from Fig 397 in FSMC
    // docs showing Mode 1 write accesses). This gives ADDSET=1 and
    // DATAST=3.
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FSMC_BTR2, 0x00000301 );
#endif // CYGHWR_HAL_CORTEXM_STM32_FAMILY_F2

    // Enable flash prefetch buffer, cacheability and set latency to 2 wait states.
    // Latency has to be set before clock is switched to a higher speed.
    {
        cyg_uint32 acr;
        
        base = CYGHWR_HAL_STM32_FLASH;

        HAL_READ_UINT32( base+CYGHWR_HAL_STM32_FLASH_ACR, acr );
        acr |= CYGHWR_HAL_STM32_FLASH_ACR_PRFTEN;
        acr |= CYGHWR_HAL_STM32_FLASH_ACR_DCEN|CYGHWR_HAL_STM32_FLASH_ACR_ICEN;
        acr |= CYGHWR_HAL_STM32_FLASH_ACR_LATENCY(CYGNUM_HAL_CORTEXM_STM32_FLASH_WAIT_STATES);
        HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_ACR, acr );
    }
}

//==========================================================================

// Set LEDs 1-4 to lowest 4 bits of supplied char.
__externC void hal_stm32x0_led(char c)
{
    CYGHWR_HAL_STM32_GPIO_OUT( CYGHWR_HAL_STM32X0G_LED1, 0 != (c&1) );
    CYGHWR_HAL_STM32_GPIO_OUT( CYGHWR_HAL_STM32X0G_LED2, 0 != (c&2) );
    CYGHWR_HAL_STM32_GPIO_OUT( CYGHWR_HAL_STM32X0G_LED3, 0 != (c&4) );
    CYGHWR_HAL_STM32_GPIO_OUT( CYGHWR_HAL_STM32X0G_LED4, 0 != (c&8) );
}

//==========================================================================

__externC void hal_platform_init( void )
{
#ifdef CYGDBG_USE_ASSERTS
    __externC char __sram_data_start[];
#endif

#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    // Check the number of VSRs matches the linker script. We can do this
    // because we intend the VV table to follow the VSR table with no gaps.
    CYG_ASSERT( (char*)&hal_virtual_vector_table[0] - (char*)&hal_vsr_table >= CYGNUM_HAL_VSR_COUNT*4,
                "VSR table size does not match" );
    // Now check the declared start of SRAM data follows the VV table end
    CYG_ASSERT( (__sram_data_start - (char*)&hal_virtual_vector_table[0]) >= CYGNUM_CALL_IF_TABLE_SIZE*4,
                "VV table size does not match sram space" );
#else
    // Check the VSR table fits below declared start of SRAM data
    CYG_ASSERT( (__sram_data_start - (char*)&hal_vsr_table[0]) >= CYGNUM_HAL_VSR_COUNT*4,
                "VSR table size does not match" );
#endif
    hal_stm32x0_led(1);
}

//==========================================================================

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

//--------------------------------------------------------------------------
// Accesses to areas not backed by real devices or memory can cause
// the CPU to hang.
//
// The following table defines the memory areas that GDB is allowed to
// touch. All others are disallowed.
// This table needs to be kept up to date with the set of memory areas
// that are available on the board.

static struct
{
    CYG_ADDRESS         start;          // Region start address
    CYG_ADDRESS         end;            // End address (last byte)
} hal_data_access[] =
{
    { CYGMEM_REGION_ram,        CYGMEM_REGION_ram+CYGMEM_REGION_ram_SIZE-1      },      // External SRAM
#ifdef CYGMEM_REGION_sram    
    { CYGMEM_REGION_sram,       CYGMEM_REGION_sram+CYGMEM_REGION_sram_SIZE-1    },      // On-chip SRAM
#endif
#ifdef CYGMEM_REGION_flash    
    { CYGMEM_REGION_flash,      CYGMEM_REGION_flash+CYGMEM_REGION_flash_SIZE-1  },      // On-chip flash
#endif
    { 0xE0000000,               0x00000000-1                                    },      // Cortex-M peripherals
    { 0x40000000,               0x60000000-1                                    },      // STM32 peripherals
    { 0xA0000000,               0xA0001000-1                                    },      // FSMC control
#ifdef CYGMEM_REGION_ccm
    { CYGMEM_REGION_ccm,        CYGMEM_REGION_ccm+CYGMEM_REGION_ccm_SIZE-1      },      // On-chip (close-coupled) SRAM
#endif
};

__externC int cyg_hal_stub_permit_data_access( CYG_ADDRESS addr, cyg_uint32 count )
{
    int i;
    for( i = 0; i < sizeof(hal_data_access)/sizeof(hal_data_access[0]); i++ )
    {
        if( (addr >= hal_data_access[i].start) &&
            (addr+count) <= hal_data_access[i].end)
            return true;
    }
    return false;
}

#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

//==========================================================================

#ifdef CYGPKG_REDBOOT
#include <redboot.h>
#include CYGHWR_MEMORY_LAYOUT_H

//--------------------------------------------------------------------------
// Memory layout
//
// We report the on-chip SRAM and external SRAM.


void
cyg_plf_memory_segment(int seg, unsigned char **start, unsigned char **end)
{
    switch (seg) {
    case 0:
        *start = (unsigned char *)CYGMEM_REGION_ram;
        *end = (unsigned char *)(CYGMEM_REGION_ram + CYGMEM_REGION_ram_SIZE);
        break;
#ifdef CYGMEM_REGION_sram
    case 1:
        *start = (unsigned char *)CYGMEM_REGION_sram;
        *end = (unsigned char *)(CYGMEM_REGION_sram + CYGMEM_REGION_sram_SIZE);
        break;
#elif defined(CYGMEM_REGION_xram)
    case 1:
        *start = (unsigned char *)CYGMEM_REGION_xram;
        *end = (unsigned char *)(CYGMEM_REGION_xram + CYGMEM_REGION_xram_SIZE);
        break;
#endif
#ifdef CYGMEM_REGION_ccm
    case 2:
        *start = (unsigned char *)CYGMEM_REGION_ccm;
        *end = (unsigned char *)(CYGMEM_REGION_ccm + CYGMEM_REGION_ccm_SIZE);
        break;
#endif
    default:
        *start = *end = NO_MEMORY;
        break;
    }
} // cyg_plf_memory_segment()

#endif // CYGPKG_REDBOOT


//==========================================================================
// EOF stm32x0g_eval_misc.c
