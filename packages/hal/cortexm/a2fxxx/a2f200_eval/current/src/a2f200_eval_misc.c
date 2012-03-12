/*==========================================================================
//
//      a2f200_eval_misc.c
//
//      Cortex-M3 A2F200 EVAL HAL functions
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2011 Free Software Foundation, Inc.
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
// Author(s):   ccoutand
// Original:    nickg (STM32 HAL)
// Date:        2011-04-03
// Description:  
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm.h>
#include <pkgconf/hal_cortexm_a2fxxx.h>
#include <pkgconf/hal_cortexm_a2f200_eval.h>
#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_intr.h>           // HAL header

#define TEST_IO 0

//==========================================================================
// System init
//
// This is run to set up the basic system, including GPIO setting,
// clock feeds, power supply, and memory initialization. This code
// runs before the DATA is copied from ROM and the BSS cleared, hence
// it cannot make use of static variables or data tables. 


__externC void
hal_system_init( void )
{
}

cyg_uint32 led[7] = {
    CYGHWR_HAL_A2FXXX_GPIO( 24,  OUT, NONE, DISABLE ),
    CYGHWR_HAL_A2FXXX_GPIO( 25,  OUT, NONE, DISABLE ),
    CYGHWR_HAL_A2FXXX_GPIO( 26,  OUT, NONE, DISABLE ),
    CYGHWR_HAL_A2FXXX_GPIO( 27,  OUT, NONE, DISABLE ),
};


//==========================================================================

__externC void
hal_platform_init( void )
{
#if TEST_IO
   cyg_uint8 i;
#endif

#if TEST_IO
   for(i = 0; i < 4; i++)
   {
      CYGHWR_HAL_A2FXXX_GPIO_SET( led[i] );
      CYGHWR_HAL_A2FXXX_GPIO_OUT( led[i], 1);
   }
#endif

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
#ifdef CYGMEM_REGION_sram              // On-chip SRAM
    {
    CYGMEM_REGION_sram, CYGMEM_REGION_sram + CYGMEM_REGION_sram_SIZE - 1},
#endif
#ifdef CYGMEM_REGION_flash             // On-chip flash
    {
    CYGMEM_REGION_flash,
            CYGMEM_REGION_flash + CYGMEM_REGION_flash_SIZE - 1},
#endif
    { 0xE0000000, 0x00000000-1 },      // Cortex-M peripherals
    { 0x40000000, 0x60000000-1 },      // Smartfusion peripherals
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
// EOF a2f200_eval_misc.c
