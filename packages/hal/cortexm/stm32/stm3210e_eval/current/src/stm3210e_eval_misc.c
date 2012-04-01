/*==========================================================================
//
//      stm3210e_eval_misc.c
//
//      Cortex-M3 STM3210E EVAL HAL functions
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
#include <pkgconf/hal_cortexm_stm32_stm3210e_eval.h>
#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_intr.h>           // HAL header


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
    
#if defined(CYG_HAL_STARTUP_ROM) | defined(CYG_HAL_STARTUP_ROMINT) | defined(CYG_HAL_STARTUP_SRAM)
    
    // Enable peripheral clocks in RCC

    base = CYGHWR_HAL_STM32_RCC;
    
    HAL_WRITE_UINT32(base+CYGHWR_HAL_STM32_RCC_AHBENR,
                     BIT_(CYGHWR_HAL_STM32_RCC_AHBENR_FSMC)  |
                     BIT_(CYGHWR_HAL_STM32_RCC_AHBENR_FLITF) |
                     BIT_(CYGHWR_HAL_STM32_RCC_AHBENR_SRAM)  );

    HAL_WRITE_UINT32(base+CYGHWR_HAL_STM32_RCC_APB2ENR,
                     BIT_(CYGHWR_HAL_STM32_RCC_APB2ENR_IOPA) |
                     BIT_(CYGHWR_HAL_STM32_RCC_APB2ENR_IOPB) |
                     BIT_(CYGHWR_HAL_STM32_RCC_APB2ENR_IOPC) |
                     BIT_(CYGHWR_HAL_STM32_RCC_APB2ENR_IOPD) |
                     BIT_(CYGHWR_HAL_STM32_RCC_APB2ENR_IOPE) |
                     BIT_(CYGHWR_HAL_STM32_RCC_APB2ENR_IOPF) |
                     BIT_(CYGHWR_HAL_STM32_RCC_APB2ENR_IOPG) );

    // Set all unused GPIO lines to input with pull down to prevent
    // them floating and annoying any external hardware.

    base = CYGHWR_HAL_STM32_GPIOA;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_CRL, 0x88888888 );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_CRH, 0x88888888 );
    
    base = CYGHWR_HAL_STM32_GPIOB;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_CRL, 0x88888888 );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_CRH, 0x88888888 );
    
    base = CYGHWR_HAL_STM32_GPIOC;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_CRL, 0x88888888 );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_CRH, 0x88888888 );
    
    // Set up GPIO lines for external bus

    base = CYGHWR_HAL_STM32_GPIOD;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_CRL, 0x44bb44bb );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_CRH, 0xbbbbbbbb );

    base = CYGHWR_HAL_STM32_GPIOE;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_CRL, 0xbbbbb4bb );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_CRH, 0xbbbbbbbb );

    base = CYGHWR_HAL_STM32_GPIOF;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_CRL, 0x44bbbbbb );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_CRH, 0xbbbb4444 );

    base = CYGHWR_HAL_STM32_GPIOG;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_CRL, 0x44bbbbbb );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_GPIO_CRH, 0x44444bb4 );

    
    // Set up FSMC NOR/SRAM bank 2 for NOR Flash

    base = CYGHWR_HAL_STM32_FSMC;

    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FSMC_BCR2, 0x00001059 );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FSMC_BTR2, 0x10000705 );

    // Set up FSMC NOR/SRAM bank 3 for SRAM

    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FSMC_BCR3, 0x00001011 );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FSMC_BTR3, 0x00000200 );

#endif

    // Enable flash prefetch buffer and set latency to 2 wait states.
    {
        cyg_uint32 acr;
        
        base = CYGHWR_HAL_STM32_FLASH;

        HAL_READ_UINT32( base+CYGHWR_HAL_STM32_FLASH_ACR, acr );
        acr |= CYGHWR_HAL_STM32_FLASH_ACR_PRFTBE;
        acr |= CYGHWR_HAL_STM32_FLASH_ACR_LATENCY(2);
        HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_ACR, acr );
    }
}

//==========================================================================

__externC void hal_platform_init( void )
{
}

//==========================================================================

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include CYGHWR_MEMORY_LAYOUT_H

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
#ifdef CYGMEM_REGION_rom    
    { CYGMEM_REGION_rom,        CYGMEM_REGION_rom+CYGMEM_REGION_rom_SIZE-1      },      // External flash
#endif
    { 0xE0000000,               0x00000000-1                                    },      // Cortex-M peripherals
    { 0x40000000,               0x60000000-1                                    },      // STM32 peripherals
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
    default:
        *start = *end = NO_MEMORY;
        break;
    }
} // cyg_plf_memory_segment()

#endif // CYGPKG_REDBOOT


//==========================================================================
// EOF stm3210e_eval_misc.c
