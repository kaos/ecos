//==========================================================================
//
//      devs/watchdog/sh/sh3/watchdog_sh3.cxx
//
//      Watchdog implementation for Hitachi SH CPUs
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
// Author(s):    jskov
// Contributors: jskov
// Date:         1999-09-01
// Purpose:      Watchdog class implementation
// Description:  Contains an implementation of the Watchdog class for use
//               with the Hitachi SH watchdog timer.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>             // system configuration file
#include <pkgconf/watchdog.h>           // configuration for this package
#include <pkgconf/kernel.h>             // kernel config

#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/kernel/instrmnt.h>        // instrumentation

#include <cyg/hal/hal_io.h>             // IO register access
#include <cyg/hal/sh_regs.h>            // watchdog register definitions

#include <cyg/io/watchdog.hxx>          // watchdog API

// -------------------------------------------------------------------------
// Constructor

void
Cyg_Watchdog::init_hw(void)
{
    CYG_REPORT_FUNCTION();
    
    // No hardware init needed.

    resolution          = CYGARC_REG_WTCSR_PERIOD;

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Start the watchdog running.

void
Cyg_Watchdog::start()
{
    CYG_REPORT_FUNCTION();

    // Init the watchdog timer (note: 8 bit reads, 16 bit writes)
    cyg_uint16 csr;
    // First disable without changing other bits.
    HAL_READ_UINT8(CYGARC_REG_WTCSR, csr);
    csr |= CYGARC_REG_WTCSR_WRITE;
    csr &= ~CYGARC_REG_WTCSR_TME;
    HAL_WRITE_UINT16(CYGARC_REG_WTCSR, csr);
    // Then set control bits and clear counter.
    csr = (CYGARC_REG_WTCSR_WRITE
           |CYGARC_REG_WTCSR_WT_IT
           |CYGARC_REG_WTCSR_CKSx_SETTING);
    HAL_WRITE_UINT16(CYGARC_REG_WTCSR, csr);
    HAL_WRITE_UINT16(CYGARC_REG_WTCNT, CYGARC_REG_WTCNT_WRITE);
    // Finally enable timer.
    csr |= CYGARC_REG_WTCSR_TME;
    HAL_WRITE_UINT16(CYGARC_REG_WTCSR, csr);
    
    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Reset watchdog timer. This needs to be called regularly to prevent
// the watchdog firing.

void
Cyg_Watchdog::reset()
{    
    CYG_REPORT_FUNCTION();

    HAL_WRITE_UINT16(CYGARC_REG_WTCNT, CYGARC_REG_WTCNT_WRITE);
    
    CYG_REPORT_RETURN();
}

#if 0
// -------------------------------------------------------------------------
// Trigger the watchdog as if the timer had expired.

void
Cyg_Watchdog::action_reset(void)
{
    CYG_REPORT_FUNCTION();
    
    start();

    HAL_WRITE_UINT16(CYGARC_REG_WTCNT, CYGARC_REG_WTCNT_WRITE|0xfe);

    CYG_REPORT_RETURN();
}
#endif

// -------------------------------------------------------------------------
// EOF watchdog_sh3.cxx
