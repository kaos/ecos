//==========================================================================
//
//      lpc1766stk_misc.c
//
//      Cortex-M3 LPC1766STK HAL functions
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
// Author(s):      nickg
// Contributor(s): ilijak
// Date:           2010-12-22
// Description:  
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm.h>
#include <pkgconf/hal_cortexm_lpc17xx.h>
#include <pkgconf/hal_cortexm_lpc17xx_lpc1766stk.h>
#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
#endif

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros

#include <cyg/hal/hal_arch.h>          // HAL header
#include <cyg/hal/hal_intr.h>          // HAL header
static inline void hal_gpio_init(void);


//==========================================================================
// System init
//
// This is run to set up the basic system, including GPIO setting,
// clock feeds, power supply, and memory initialization. This code
// runs before the DATA is copied from ROM and the BSS cleared, hence
// it cannot make use of static variables or data tables.

__externC void
hal_system_init(void)
{
#if defined(CYG_HAL_STARTUP_ROM) | defined(CYG_HAL_STARTUP_SRAM)
    hal_gpio_init();
#endif

#if defined(CYG_HAL_STARTUP_ROM)
    {
        // Set flash accelerator according to CPU clock speed.
        cyg_uint32      regval;
        HAL_READ_UINT32(CYGHWR_HAL_LPC17XX_REG_SCB_BASE +
                        CYGHWR_HAL_LPC17XX_REG_FLASHCFG, regval);
        regval &= ~CYGHWR_HAL_LPC17XX_REG_FLTIM_MASK;
        regval |= CYGHWR_HAL_LPC17XX_REG_FLASHTIM;
        HAL_WRITE_UINT32(CYGHWR_HAL_LPC17XX_REG_SCB_BASE +
                         CYGHWR_HAL_LPC17XX_REG_FLASHCFG, regval);
    }
#endif
}


//===========================================================================
// hal_gpio_init
//===========================================================================
static inline void
hal_gpio_init(void)
{
    // Enable UART0 and UART1 (has wired flow control and line status lines)
    CYGHWR_HAL_LPC17XX_PIN_SET(CYGHWR_HAL_LPC17XX_REG_PINSEL0,
                               (1 /* TXD0 */  << 4) |
                               (1 /* RXD0 */  << 6) |
                               (1 /* TXD1 */  << 30)
        );
    CYGHWR_HAL_LPC17XX_PIN_SET(CYGHWR_HAL_LPC17XX_REG_PINSEL1,
                               (1 /* RXD1 */  << 0) |
                               (1 /* CTS1 */  << 2) |
                               (1 /* DCD1 */  << 4) |
                               (1 /* DSR1 */  << 6) |
                               (1 /* DTR1 */  << 8) |
                               (1 /* RTS1 */  << 12)
        );
    CYGHWR_HAL_LPC17XX_PIN_SET(CYGHWR_HAL_LPC17XX_REG_PINSEL2, 0);
    CYGHWR_HAL_LPC17XX_PIN_SET(CYGHWR_HAL_LPC17XX_REG_PINSEL3, 0);
    CYGHWR_HAL_LPC17XX_PIN_SET(CYGHWR_HAL_LPC17XX_REG_PINSEL4, 0);
#if 0 // not used
    CYGHWR_HAL_LPC17XX_PIN_SET(CYGHWR_HAL_LPC17XX_REG_PINSEL5, 0);
    CYGHWR_HAL_LPC17XX_PIN_SET(CYGHWR_HAL_LPC17XX_REG_PINSEL6, 0);
#endif
    CYGHWR_HAL_LPC17XX_PIN_SET(CYGHWR_HAL_LPC17XX_REG_PINSEL7, 0);
#if 0 // not used
    CYGHWR_HAL_LPC17XX_PIN_SET(CYGHWR_HAL_LPC17XX_REG_PINSEL8, 0);
#endif
    CYGHWR_HAL_LPC17XX_PIN_SET(CYGHWR_HAL_LPC17XX_REG_PINSEL9, 0);
    CYGHWR_HAL_LPC17XX_PIN_SET(CYGHWR_HAL_LPC17XX_REG_PINSEL10, 0);
}


//==========================================================================

__externC void
hal_platform_init(void)
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

static struct {
    CYG_ADDRESS     start;             // Region start address
    CYG_ADDRESS     end;               // End address (last byte)
} hal_data_access[] = {
    {
    // Main RAM (On-chip SRAM in code area)
    CYGMEM_REGION_ram, CYGMEM_REGION_ram + CYGMEM_REGION_ram_SIZE - 1},
#ifdef CYGMEM_REGION_ahb_sram_bank0
    {
    // On-chip AHB SRAM bank 0
    CYGMEM_REGION_ahb_sram_bank0, CYGMEM_REGION_ahb_sram_bank0 + CYGMEM_REGION_ahb_sram_bank0_SIZE - 1},
#endif
#ifdef CYGMEM_REGION_ahb_sram_bank1
    {
    // On-chip AHB SRAM bank 1
    CYGMEM_REGION_ahb_sram_bank1, CYGMEM_REGION_ahb_sram_bank1 + CYGMEM_REGION_ahb_sram_bank1_SIZE - 1},
#endif
#ifdef CYGMEM_REGION_flash
    {
    // On-chip flash
    CYGMEM_REGION_flash, CYGMEM_REGION_flash + CYGMEM_REGION_flash_SIZE - 1},
#endif
#ifdef CYGMEM_REGION_rom
    {
    // External flash
    CYGMEM_REGION_rom, CYGMEM_REGION_rom + CYGMEM_REGION_rom_SIZE - 1},
#endif
    {
    0xE0000000, 0x00000000 - 1},       // Cortex-M peripherals
    {
    0x40000000, 0x60000000 - 1},       // Chip specific peripherals
};

__externC int
cyg_hal_stub_permit_data_access(CYG_ADDRESS addr, cyg_uint32 count)
{
    int             i;

    for (i = 0; i < sizeof(hal_data_access) / sizeof(hal_data_access[0]); i++) {
        if ((addr >= hal_data_access[i].start) &&
            (addr + count) <= hal_data_access[i].end)
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
// We report the main (S)RAM and peripheral (AHB) SRAM banks.


void
cyg_plf_memory_segment(int seg, unsigned char **start, unsigned char **end)
{
    switch (seg) {
    case 0:
        *start = (unsigned char *)CYGMEM_REGION_ram;
        *end = (unsigned char *)(CYGMEM_REGION_ram + CYGMEM_REGION_ram_SIZE);
        break;
#ifdef CYGMEM_REGION_ahb_sram_bank0
    case 1:
        *start = (unsigned char *)CYGMEM_REGION_ahb_sram_bank0;
        *end =
            (unsigned char *)(CYGMEM_REGION_ahb_sram_bank0 +
                              CYGMEM_REGION_ahb_sram_bank0_SIZE);
        break;
#endif
#ifdef CYGMEM_REGION_ahb_sram_bank1
# ifndef CYGMEM_REGION_ahb_sram_bank0
    case 1:
# else
    case 2:
# endif
        *start = (unsigned char *)CYGMEM_REGION_ahb_sram_bank1;
        *end =
            (unsigned char *)(CYGMEM_REGION_ahb_sram_bank1 +
                              CYGMEM_REGION_ahb_sram_bank1_SIZE);
        break;
#endif
    default:
        *start = *end = NO_MEMORY;
        break;
    }
}

#endif // CYGPKG_REDBOOT


//==========================================================================
// EOF lpc1766stk_misc.c
