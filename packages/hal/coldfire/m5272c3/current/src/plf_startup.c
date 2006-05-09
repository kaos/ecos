//==========================================================================
//
//      plf_startup.c
//
//      M5272C3 platform HAL startup code
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Ltd.
//
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Enrico Piria
// Contributors:
// Date:          2005-25-06
// Purpose:       Platform startup code.
// Description:   This module contains code that sets up the platform specific
//                hardware and data. All the code must be contained in the
//                section called ".boot", in order for the ROMRAM startup
//                to work properly.
//
//####DESCRIPTIONEND####
//========================================================================

#include <cyg/infra/cyg_type.h>
#include <pkgconf/hal.h>
#include <cyg/hal/hal_startup.h>
#include CYGHWR_MEMORY_LAYOUT_H
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/coldfire_regs.h>

static void plf_init_sim(void) __attribute__ ((section (".boot")));
static void plf_init_intc(void) __attribute__ ((section (".boot")));
static void plf_init_cs(void) __attribute__ ((section (".boot")));
static void plf_init_sdramc(void) __attribute__ ((section (".boot")));
static void plf_init_cache_acr(void) __attribute__ ((section (".boot")));


// Platform-specific reset vector initialization routine
void plf_reset(void)
{
    plf_init_sim();
    plf_init_intc();
    plf_init_cs();
    plf_init_sdramc();

    // Call a routine to set  up  the  cache  and  ACRs  for  this  specific
    // platform.
    plf_init_cache_acr();
}


// Initialize the cache and access control registers.
// The reset procedure already invalidated the cache and ACRs.
// This routine only needs to enable the ACRs that it will use.
static void plf_init_cache_acr(void)
{
    // Enable the instruction cache with the following options:
    // Enable CPUSHL invalidation.
    // No freeze.
    // Invalidate all cache lines (flush).
    // No external arbiter control.
    // Disable non-cacheable instruction bursting.
    // Default memory is cacheable.
    // Enable buffered writes.
    // Read and write access permitted by default.
    // Instruction fetch size is cache line.

#ifdef CYGSEM_HAL_ENABLE_ICACHE_ON_STARTUP
    CYGARC_MOVEC((CYG_WORD32) 0x81000102, CYGARC_REG_CACR);
#endif

    // Leave the access control registers disabled.
}


// Initialize SIM module and system configuration registers
void plf_init_sim(void)
{
    // Set up the mapping of our internal registers.  The LSB indicates that
    // the register contents are valid.
    CYGARC_MOVEC((CYG_WORD32)(CYGMEM_REGION_devs | 1), CYGARC_REG_MBAR);
    
    // Initialize System Config Register
    // Setup Watch Dog Timeout
    HAL_WRITE_UINT16(&MCF5272_DEVS->cfg.scr, MCF5272_SIM_SCR_HWWD_1024);
        
    // Initialize System Protection Register
    // Enable all bus error exceptions
    HAL_WRITE_UINT16(&MCF5272_DEVS->cfg.spr,
        (0 | MCF5272_SIM_SPR_ADC | MCF5272_SIM_SPR_ADCEN
        | MCF5272_SIM_SPR_WPV | MCF5272_SIM_SPR_WPVEN
        | MCF5272_SIM_SPR_SMV | MCF5272_SIM_SPR_SMVEN
        | MCF5272_SIM_SPR_SBE | MCF5272_SIM_SPR_SBEEN
        | MCF5272_SIM_SPR_HWT | MCF5272_SIM_SPR_HWTEN
        | MCF5272_SIM_SPR_RPV | MCF5272_SIM_SPR_RPVEN
        | MCF5272_SIM_SPR_EXT | MCF5272_SIM_SPR_EXTEN
        | MCF5272_SIM_SPR_SUV | MCF5272_SIM_SPR_SUVEN
        )) ;
}


// Initialize interrupt controller
void plf_init_intc(void)
{
    int i;

    // Initialize the vector base register in the interrupt controller.
    HAL_WRITE_UINT8(&MCF5272_DEVS->intc.ipvr, HAL_PROG_INT_VEC_BASE);

    // Initialize the  interrupt  control  register.
    // Disable all interrupts by setting all priorities to zero.
    for (i = 0; i < 4; i++)
    {
        HAL_WRITE_UINT32(&MCF5272_DEVS->intc.icr[i], 0x88888888);
    }

    // Initialize the processor's vector base register (align to 1M boundary).
    CYGARC_MOVEC((CYG_WORD32) __ramvec_start & 0xFFF00000, CYGARC_REG_VBR);
}


// Initialize chip-select modules
void plf_init_cs(void)
{
    // ChipSelect 0 - 2MB FLASH
    // At startup, CS0 is configured so that addresses starting at 0xXXX00000
    // are aliased to 0x00000000, so, in ROM startup configuration, code can
    // be placed starting at VMA address 0xFFE00000. When we are here,
    // the PC points to addresses in 0xFFE00000 space, and we can safely
    // reconfigure CS0 to respond uniquely to those addresses.
    HAL_WRITE_UINT32(&MCF5272_DEVS->cs[0].csbr, (0
        | MCF5272_CS_BR_BASE(CYGMEM_REGION_flash)
        | MCF5272_CS_BR_SRAM
        | MCF5272_CS_BR_PS_16
        | MCF5272_CS_BR_EN));    

    HAL_WRITE_UINT32(&MCF5272_DEVS->cs[0].csor,
        (0 | MCF5272_CS_OR_MASK_2M
        | MCF5272_CS_OR_WS(5)));

#ifdef CYGHWR_EXT_SRAM_INSTALLED
    // Chip Select 2 - 512KB SRAM
    HAL_WRITE_UINT32(&MCF5272_DEVS->cs[2].csbr, (0
        | MCF5272_CS_BR_BASE(CYGMEM_REGION_ext_sram)
        | MCF5272_CS_BR_SRAM
        | MCF5272_CS_BR_PS_32
        | MCF5272_CS_BR_EN));

    HAL_WRITE_UINT32(&MCF5272_DEVS->cs[2].csor, (0
        | MCF5272_CS_OR_MASK_512K
        | MCF5272_CS_OR_WS(0)));
#endif // CYGHWR_EXT_SRAM_INSTALLED

    // ChipSelect 7 - 16MB SDRAM
    HAL_WRITE_UINT32(&MCF5272_DEVS->cs[7].csbr, (0
        | MCF5272_CS_BR_BASE(CYGMEM_REGION_sdram)
        | MCF5272_CS_BR_SDRAM 
        | MCF5272_CS_BR_PS_LINE 
        | MCF5272_CS_BR_EN));

    HAL_WRITE_UINT32(&MCF5272_DEVS->cs[7].csor, (0
#if (CYGHWR_INSTALLED_SDRAM_SIZE == 4)
        | MCF5272_CS_OR_MASK_4M
#else
        | MCF5272_CS_OR_MASK_16M
#endif
        | MCF5272_CS_OR_WS(0x1F)));
}


// Initialize SDRAM controller
void plf_init_sdramc(void)
{
    cyg_uint16 sdcr;

    
    HAL_READ_UINT16(&MCF5272_DEVS->sdramc.sdcr, sdcr);
    
    // Do not initialize SDRAM if it is already active
    if (!(sdcr & MCF5272_SDRAMC_SDCCR_ACT))
    {
#if (CYGHWR_HAL_SYSTEM_CLOCK_MHZ == 66)
        HAL_WRITE_UINT16(&MCF5272_DEVS->sdramc.sdtr, (0
            | MCF5272_SDRAMC_SDCTR_RTP_66MHz
            | MCF5272_SDRAMC_SDCTR_RC(0)
            | MCF5272_SDRAMC_SDCTR_RP(1)
            | MCF5272_SDRAMC_SDCTR_RCD(1)
            | MCF5272_SDRAMC_SDCTR_CLT_2));
#else
        // Clock frequency must be 48 Mhz
        HAL_WRITE_UINT16(&MCF5272_DEVS->sdramc.sdtr, (0
            | MCF5272_SDRAMC_SDCTR_RTP_48MHz
            | MCF5272_SDRAMC_SDCTR_RC(0)
            | MCF5272_SDRAMC_SDCTR_RP(1)
            | MCF5272_SDRAMC_SDCTR_RCD(0)
            | MCF5272_SDRAMC_SDCTR_CLT_2));
#endif

        HAL_WRITE_UINT16(&MCF5272_DEVS->sdramc.sdcr, (0
            | MCF5272_SDRAMC_SDCCR_MCAS_A9
#if (CYGHWR_INSTALLED_SDRAM_SIZE == 4)
            | MCF5272_SDRAMC_SDCCR_BALOC_A21
#else
            | MCF5272_SDRAMC_SDCCR_BALOC_A22
#endif
            | MCF5272_SDRAMC_SDCCR_REG
            | MCF5272_SDRAMC_SDCCR_INIT));

        // Start SDRAM controller with a memory write
        *((volatile char *) CYGMEM_REGION_sdram) = 0;

        // Wait until controller is ready
        do
        {
            HAL_READ_UINT16(&MCF5272_DEVS->sdramc.sdcr, sdcr);
        } while(!(sdcr & MCF5272_SDRAMC_SDCCR_ACT));
    }
}


// Platform specific data initialization routine
void plf_init_data(void)
{
    // Nothing to do
}
