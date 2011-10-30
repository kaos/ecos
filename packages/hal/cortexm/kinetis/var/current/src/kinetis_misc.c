//==========================================================================
//
//      kinetis_misc.c
//
//      Cortex-M Kinetis HAL functions
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
// Author(s):    Ilija kocho <ilijak@siva.com.mk>
// Date:         2011-02-05
// Description:
//
//####DESCRIPTIONEND####
//
//========================================================================


#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm.h>
#include <pkgconf/hal_cortexm_kinetis.h>
#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif


#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/cortexm_endian.h>
#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_intr.h>           // HAL header
#include <cyg/hal/hal_if.h>             // HAL header



// Clock related functions are in kinetis_clocking.c
void hal_start_clocks( void );

#ifdef CYG_HAL_STARTUP_ROM

//===========================================================================
// KINETIS FLASH configuration field
//===========================================================================

// Note: KINETIS FLASH configuration field must be present in Kinetis flash
//       image and ocupy addresses 0x00000400 to 0x0000040f.

// For ".flash_conf" section definition see MLT files.

const cyghwr_hal_kinetis_flash_conf_t CYGHWR_HAL_KINETIS_FLASH_CONF_FIELD
__attribute__((section(".flash_conf"), used)) = {
    .backdoor_key = CYGHWR_HAL_KINETIS_FLASH_CONF_BACKDOOR_KEY,
    .fprot = CYGHWR_HAL_KINETIS_FLASH_CONF_FPROT,
    .fsec = CYGHWR_HAL_KINETIS_FLASH_CONF_FSEC,
    .fopt = CYGHWR_HAL_KINETIS_FLASH_CONF_FOPT,
    .feprot = CYGHWR_HAL_KINETIS_FLASH_CONF_FEPROT,
    .fdprot = CYGHWR_HAL_KINETIS_FLASH_CONF_FDPROT
};

const cyghwr_hal_kinetis_flash_conf_t *
CYGOPT_HAL_KINETIS_MISC_FLASH_SECTION_ATTR
hal_kinetis_flash_conf_p( void )
{
    return &CYGHWR_HAL_KINETIS_FLASH_CONF_FIELD;
}

#endif // CYG_HAL_STARTUP_ROM

//=== KINETIS FLASH security configuration END. ============================


//==========================================================================
// Setup variant specific hardware
//=========================================================================


void hal_variant_init( void )
{
    hal_start_clocks();

#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    hal_if_init();
#endif
}


//===========================================================================
// The  WDOG at Freescale Kinetis is enabled after reset. hal_wdog_disable
// provides functionality for disabling it at startup.
//===========================================================================

static inline void CYGOPT_HAL_KINETIS_MISC_FLASH_SECTION_ATTR
hal_kinetis_wdog_unlock_simple(volatile CygHwr_HAL_Kinetis_wdog_t *wdog_p)
{
    wdog_p->Unlock = 0xC520;
    wdog_p->Unlock = 0xD928;
}

void CYGOPT_HAL_KINETIS_MISC_FLASH_SECTION_ATTR
hal_kinetis_wdog_unlock(volatile CygHwr_HAL_Kinetis_wdog_t *wdog_p)
{
    CYGARC_CPSID( i );
    hal_kinetis_wdog_unlock_simple(wdog_p);
    CYGARC_CPSIE( i );
}

void CYGOPT_HAL_KINETIS_MISC_FLASH_SECTION_ATTR
hal_wdog_disable(void)
{
    volatile CygHwr_HAL_Kinetis_wdog_t *wdog_p = CYGHWR_HAL_KINETIS_WDOG_P;
    hal_kinetis_wdog_unlock_simple(wdog_p);
    wdog_p->StCtrlH = CYGHWR_HAL_KINETIS_WDOG_STCTRLH_ALLOWUPDATE_M;
}

//==========================================================================
// Pin configuration functions
//

static cyghwr_hal_kinetis_port_t * const Ports[] = {
    CYGHWR_HAL_KINETIS_PORTA_P, CYGHWR_HAL_KINETIS_PORTB_P,
    CYGHWR_HAL_KINETIS_PORTC_P, CYGHWR_HAL_KINETIS_PORTD_P,
    CYGHWR_HAL_KINETIS_PORTE_P
};

void CYGOPT_HAL_KINETIS_MISC_FLASH_SECTION_ATTR
hal_set_pin_function(cyg_uint32 pin)
{
    cyghwr_hal_kinetis_port_t *port_p;

    if(pin != CYGHWR_HAL_KINETIS_PIN_NONE) {
        port_p = Ports[CYGHWR_HAL_KINETIS_PIN_PORT(pin)];
        port_p->pcr[CYGHWR_HAL_KINETIS_PIN_BIT(pin)] =
              CYGHWR_HAL_KINETIS_PIN_FUNC(pin);
    }
}

void
hal_dump_pin_function(cyg_uint32 pin)
{
    cyghwr_hal_kinetis_port_t *port_p;

    if(pin != CYGHWR_HAL_KINETIS_PIN_NONE) {
        port_p = Ports[CYGHWR_HAL_KINETIS_PIN_PORT(pin)];
        diag_printf("Port        %d: %p[%d] fun=%x\n",
            CYGHWR_HAL_KINETIS_PIN_PORT(pin),
            port_p,
            CYGHWR_HAL_KINETIS_PIN_BIT(pin),
            port_p->pcr[CYGHWR_HAL_KINETIS_PIN_BIT(pin)]);
    }
}

void
hal_dump_pin_setting(cyg_uint32 pin)
{
    if(pin != CYGHWR_HAL_KINETIS_PIN_NONE) {
        diag_printf("Pin: 0x%08x Port=%d bit=%d fun=%x\n",
            pin,
            CYGHWR_HAL_KINETIS_PIN_PORT(pin),
            CYGHWR_HAL_KINETIS_PIN_BIT(pin),
            CYGHWR_HAL_KINETIS_PIN_FUNC(pin));
    }
}


//==========================================================================
// EOF kinetis_misc.c
