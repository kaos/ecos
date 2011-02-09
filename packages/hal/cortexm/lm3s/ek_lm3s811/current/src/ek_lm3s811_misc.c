//==========================================================================
//
//      ek_lm3s811_misc.c
//
//      Cortex-M3 Stellaris EK-LM3S811 HAL functions
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
// Author(s):   Original for STM3210E EVAL: nickg
//              Updated for EK-LM3S811: ccoutand
// Date:        2011-01-18
// Description:  
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>


// On-board status LED
#define CYGHWR_HAL_EK_LM3S811_STATUS_LED \
        CYGHWR_HAL_LM3S_GPIO( C, 5, OUT, 4_MA, PULLUP, DISABLE )

// OLED regulator control
#define CYGHWR_HAL_EK_LM3S811_OLED_PWD \
        CYGHWR_HAL_LM3S_GPIO( D, 7, OUT, NONE, NONE, DISABLE )


//==========================================================================
// Drive OLED power ON/OFF IO

__externC void
hal_ek_lm3s811_oled_pwd( bool pwd )
{
    cyg_uint32      oled_pwd_io = CYGHWR_HAL_EK_LM3S811_OLED_PWD;

    // Power ON regulator
    if ( pwd == true )
        CYGHWR_HAL_LM3S_GPIO_OUT( oled_pwd_io, 1 );
    else
        CYGHWR_HAL_LM3S_GPIO_OUT( oled_pwd_io, 0 );
}


//==========================================================================
// Setup platform

__externC void
hal_platform_init( void )
{
    cyg_uint32      led_io = CYGHWR_HAL_EK_LM3S811_STATUS_LED;
    cyg_uint32      oled_pwd_io = CYGHWR_HAL_EK_LM3S811_OLED_PWD;

    // OLED power switch IO is on port D and user LED on port C
    CYGHWR_HAL_LM3S_PERIPH_SET( CYGHWR_HAL_LM3S_P_GPIOC, 1 );
    CYGHWR_HAL_LM3S_PERIPH_SET( CYGHWR_HAL_LM3S_P_GPIOD, 1 );
    CYGHWR_HAL_LM3S_PERIPH_SET( CYGHWR_HAL_LM3S_P_I2C0, 0 );

    // Status LED -> ON
    CYGHWR_HAL_LM3S_GPIO_SET( led_io );
    CYGHWR_HAL_LM3S_GPIO_OUT( led_io, 1 );

    // Shutdown power regulator
    CYGHWR_HAL_LM3S_GPIO_SET( oled_pwd_io );

    hal_ek_lm3s811_oled_pwd( false );
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
#ifdef CYGMEM_REGION_sram              // On-chip SRAM
    {
    CYGMEM_REGION_sram, CYGMEM_REGION_sram + CYGMEM_REGION_sram_SIZE - 1},
#endif
#ifdef CYGMEM_REGION_flash             // On-chip flash
    {
    CYGMEM_REGION_flash,
            CYGMEM_REGION_flash + CYGMEM_REGION_flash_SIZE - 1},
#endif
    {
    0xE0000000, 0x00000000 - 1},       // Cortex-M peripherals
    {
    0x40000000, 0x50000000 - 1},       // Stellaris peripherals
};

__externC int
cyg_hal_stub_permit_data_access( CYG_ADDRESS addr, cyg_uint32 count )
{
    int             i;
    for( i = 0; i < sizeof( hal_data_access ) / sizeof( hal_data_access[0] );
         i++ ) {
        if( ( addr >= hal_data_access[i].start )
            && ( addr + count ) <= hal_data_access[i].end )
            return true;
    }
    return false;
}

#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS


//==========================================================================
// EOF ek_lm3s811_misc.c
