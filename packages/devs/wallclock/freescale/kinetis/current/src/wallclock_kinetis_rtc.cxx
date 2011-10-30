//==========================================================================
//
//      wallclock_kinetis_rtc.cxx
//
//      Wallclock implementation for Kinetis RTC module
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2011 Free Software Foundation, Inc.                        
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
// Author(s):     Ilija Kocho <ilijak@siva.com.mk>
// Contributors:
// Date:          2011-04-23
// Purpose:       Wallclock driver for Kinetis RTC module
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/wallclock.h>
#include <pkgconf/devices_wallclock_kinetis_rtc.h>

#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/infra/cyg_type.h>

#include <cyg/io/wallclock.hxx>

void
Cyg_WallClock::init_hw_seconds(void)
{
    volatile cyghwr_hal_kinetis_rtc_t *rtc_p = CYGHWR_HAL_KINETIS_RTC_P;
#ifdef CYGSEM_WALLCLOCK_SET_GET_MODE
    // if RTC is disabled (e.g. due to power loss) initialize it.
    if(!(rtc_p->sr &CYGHWR_HAL_KINETIS_RTC_SR_TCE))
#endif
    {
        // halt clock, reset
        rtc_p->sr &= ~CYGHWR_HAL_KINETIS_RTC_SR_TCE;
        // set time
        rtc_p->tpr = 0;
        rtc_p->tsr = 0;
        // restart clock
        rtc_p->sr |= CYGHWR_HAL_KINETIS_RTC_SR_TCE;
    }
}

cyg_uint32
Cyg_WallClock::get_hw_seconds(void)
{
    volatile cyghwr_hal_kinetis_rtc_t *rtc_p = CYGHWR_HAL_KINETIS_RTC_P;

    return rtc_p->tsr;
}

#ifdef CYGSEM_WALLCLOCK_SET_GET_MODE
void
Cyg_WallClock::set_hw_seconds(cyg_uint32 secs)
{
    volatile cyghwr_hal_kinetis_rtc_t *rtc_p = CYGHWR_HAL_KINETIS_RTC_P;

    // halt clock, reset
    rtc_p->sr &= ~CYGHWR_HAL_KINETIS_RTC_SR_TCE;
    // set time
    rtc_p->tpr = 0;
    rtc_p->tsr = secs;
    // restart clock
    rtc_p->sr |= CYGHWR_HAL_KINETIS_RTC_SR_TCE;
}
#endif // CYGSEM_WALLCLOCK_SET_GET_MODE

//==========================================================================
// EOF wallclock_kinetis_rtc.cxx
