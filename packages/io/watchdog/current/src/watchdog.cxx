//==========================================================================
//
//      io/watchdog/watchdog.cxx
//
//      Watchdog common code
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Contributors: nickg
// Date:         1999-02-18
// Purpose:      Watchdog class implementation
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>             // system configuration file
#include <pkgconf/watchdog.h>           // configuration for this package

#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/drv_api.h>            // for locking

#include <cyg/io/watchdog.hxx>          // watchdog API

// -------------------------------------------------------------------------
// Statics

// A static pointer to the single system defined watchdog device.
Cyg_Watchdog Cyg_Watchdog::watchdog;

// -------------------------------------------------------------------------
// Constructor


Cyg_Watchdog::Cyg_Watchdog()
{
    CYG_REPORT_FUNCTION();

#ifndef CYGSEM_WATCHDOG_RESETS_ON_TIMEOUT    
    action_list         = 0;
#endif

    // HW driver initialization. This must set the watchdog resolution.
    init_hw();
        
    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Return reset resolution

cyg_uint64
Cyg_Watchdog::get_resolution()
{
    return resolution;
}

#ifndef CYGSEM_WATCHDOG_RESETS_ON_TIMEOUT
// -------------------------------------------------------------------------
// Trigger the watchdog as if the timer had expired. This should be called
// from the driver's ISR.

void
Cyg_Watchdog::trigger()
{
    CYG_REPORT_FUNCTION();
    
    cyg_drv_dsr_lock();
    
    Cyg_Watchdog_Action *act = action_list;

    while( 0 != act )
    {
        act->action( act->data );

        act = act->next;
    }

    cyg_drv_dsr_unlock();

    CYG_REPORT_RETURN();
}
    
// -------------------------------------------------------------------------
// Register an action routine that will be called when the timer
// triggers.

void
Cyg_Watchdog::install_action( Cyg_Watchdog_Action *action )
{
    CYG_REPORT_FUNCTION();
    
    cyg_drv_dsr_lock();
    
    action->next = action_list;
    action_list = action;

    cyg_drv_dsr_unlock();

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Deregister a previously registered action routine.

void
Cyg_Watchdog::uninstall_action( Cyg_Watchdog_Action *action )
{
    CYG_REPORT_FUNCTION();
    
    cyg_drv_dsr_lock();

    Cyg_Watchdog_Action **act_ptr = &action_list;    

    while( 0 != *act_ptr )
    {
        Cyg_Watchdog_Action *a = *act_ptr;

        if( a == action )
        {
            *act_ptr = a->next;
            break;
        }
        act_ptr = &a->next;
    }
    
    cyg_drv_dsr_unlock();

    CYG_REPORT_RETURN();
}

#endif // CYGSEM_WATCHDOG_RESETS_ON_TIMEOUT

// -------------------------------------------------------------------------
// EOF io/watchdog/watchdog.cxx
