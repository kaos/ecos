//==========================================================================
//
//      watchdog/aeb1.cxx
//
//      Watchdog implementation for ARM AEB1 board (SHARP LH77790 CPU)
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jskov
// Contributors: jskov
// Date:         1999-09-01
// Purpose:      Watchdog class implementation
// Description:  Contains an implementation of the Watchdog class for use
//               with the SHARP LH77790 watchdog timer.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>             // system configuration file
#include <pkgconf/watchdog.h>           // configuration for this package

#if defined(CYGPKG_HAL_ARM_AEB) && !defined(CYGIMP_WATCHDOG_EMULATE)

#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros
#include <cyg/kernel/instrmnt.h>        // instrumentation

#include <cyg/hal/hal_io.h>             // IO register access

#include <cyg/devs/watchdog.hxx>        // my header

// -------------------------------------------------------------------------
// Register definitions
#define CYGARC_REG_WATCHDOG_BASE        0xFFFFAC00
#define CYGARC_REG_WATCHDOG_WDCTLR      (CYGARC_REG_WATCHDOG_BASE+0x30)
#define CYGARC_REG_WATCHDOG_WDCNTR      (CYGARC_REG_WATCHDOG_BASE+0x34)

// Control register bits
#define CYGARC_REG_WATCHDOG_WDCTLR_EN        0x01 // enable
#define CYGARC_REG_WATCHDOG_WDCTLR_RSP_NMF   0x00 // non-maskable fiq
#define CYGARC_REG_WATCHDOG_WDCTLR_RSP_ER    0x04 // external reset
#define CYGARC_REG_WATCHDOG_WDCTLR_RSP_SR    0x06 // system reset
#define CYGARC_REG_WATCHDOG_WDCTLR_FRZ       0x08 // lock enable bit
#define CYGARC_REG_WATCHDOG_WDCTLR_TOP_MASK  0x70 // time out period

#define CYGARC_REG_WATCHDOG_WDCTLR_TOP_17    0x00 // 2^17
#define CYGARC_REG_WATCHDOG_WDCTLR_TOP_17_P  5242880 // = 5.2ms

#define CYGARC_REG_WATCHDOG_WDCTLR_TOP_25    0x40 // 2^25
#define CYGARC_REG_WATCHDOG_WDCTLR_TOP_25_P  1342177300 // = 1.3421773s


// -------------------------------------------------------------------------
// Statics
// A static pointer to the single system defined watchdog device.
Cyg_Watchdog Cyg_Watchdog::watchdog;

// -------------------------------------------------------------------------
// Constructor


Cyg_Watchdog::Cyg_Watchdog()
{
    CYG_REPORT_FUNCTION();
    
    action_list         = 0;

    resolution          = CYGARC_REG_WATCHDOG_WDCTLR_TOP_25_P;

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Return reset resolution

cyg_uint64 Cyg_Watchdog::get_resolution()
{
    return resolution;
}


// -------------------------------------------------------------------------
// Start the watchdog running.

void
Cyg_Watchdog::start()
{
    CYG_REPORT_FUNCTION();

    // Clear the watchdog counter.
    HAL_WRITE_UINT32(CYGARC_REG_WATCHDOG_WDCNTR, 0);

    // Enable the watchdog (and lock/FRZ it).
    HAL_WRITE_UINT8(CYGARC_REG_WATCHDOG_WDCTLR, 
                    (CYGARC_REG_WATCHDOG_WDCTLR_TOP_25
                     | CYGARC_REG_WATCHDOG_WDCTLR_FRZ
                     | CYGARC_REG_WATCHDOG_WDCTLR_RSP_SR
                     | CYGARC_REG_WATCHDOG_WDCTLR_EN));
    
    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Reset watchdog timer. This needs to be called regularly to prevent
// the watchdog firing.

void
Cyg_Watchdog::reset()
{    
    CYG_REPORT_FUNCTION();

    HAL_WRITE_UINT32(CYGARC_REG_WATCHDOG_WDCNTR, 0);
    
    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Trigger the watchdog as if the timer had expired.

void
Cyg_Watchdog::trigger()
{
    CYG_REPORT_FUNCTION();
    
    // Clear the watchdog counter.
    HAL_WRITE_UINT32(CYGARC_REG_WATCHDOG_WDCNTR, 0);

    // Enable the watchdog with the smallest timeout.
    HAL_WRITE_UINT8(CYGARC_REG_WATCHDOG_WDCTLR, 
                    (CYGARC_REG_WATCHDOG_WDCTLR_TOP_17
                     | CYGARC_REG_WATCHDOG_WDCTLR_FRZ
                     | CYGARC_REG_WATCHDOG_WDCTLR_RSP_SR
                     | CYGARC_REG_WATCHDOG_WDCTLR_EN));

    CYG_REPORT_RETURN();
}
    
// -------------------------------------------------------------------------
// Register an action routine that will be called when the timer
// triggers.

void
Cyg_Watchdog::install_action( Cyg_Watchdog_Action *action )
{
    CYG_REPORT_FUNCTION();
    
    // Not implemented. Possible to select a FIQ interrupt instead of reset.
    CYG_FAIL("Not implemented.");

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Deregister a previously registered action routine.

void
Cyg_Watchdog::uninstall_action( Cyg_Watchdog_Action *action )
{
    CYG_REPORT_FUNCTION();
    
    // Not implemented. Possible to select a FIQ interrupt instead of reset.
    CYG_FAIL("Not implemented.");

    CYG_REPORT_RETURN();
}

#endif // defined(CYGPKG_HAL_ARM_AEB) &&
       // !defined(CYGIMP_WATCHDOG_EMULATE)
// -------------------------------------------------------------------------
// EOF watchdog/aeb1.cxx
