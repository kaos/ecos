#ifndef CYGONCE_KERNEL_EXCEPT_HXX
#define CYGONCE_KERNEL_EXCEPT_HXX

//==========================================================================
//
//      except.hxx
//
//      Exception handling declarations
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
// Contributors: nickg, jlarmour
// Date:         1999-02-16
// Purpose:      Define exception interfaces
// Description:  The classes defined here collectively implement the
//               internal API used to register, manage and deliver
//               exceptions.
// Usage:        #include <cyg/kernel/thread.hxx>
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>
#include <cyg/infra/cyg_ass.h>          // assertion macros
#include <cyg/hal/hal_intr.h>           // exception defines

// -------------------------------------------------------------------------
// Exception handler function prototype

typedef void cyg_exception_handler(
    CYG_ADDRWORD        data,                   // user supplied data
    cyg_code            exception_number,       // exception being raised
    CYG_ADDRWORD        exception_info          // any exception specific info
    );

// -------------------------------------------------------------------------
// Exception delivery interface. This function is exported to the HAL which
// invokes it for all exceptions that it is not able to handle itself.

externC void cyg_hal_deliver_exception( CYG_WORD code, CYG_ADDRWORD data );

// -------------------------------------------------------------------------
// Exception control class. Depending on the configuration there is either
// one of these per thread, or one for the entire system.

#ifdef CYGPKG_KERNEL_EXCEPTIONS

class Cyg_Exception_Control
{

#ifdef CYGSEM_KERNEL_EXCEPTIONS_DECODE
    cyg_exception_handler   *exception_handler[CYGNUM_HAL_EXCEPTION_COUNT];
    
    CYG_ADDRWORD            exception_data[CYGNUM_HAL_EXCEPTION_COUNT];
#else
    cyg_exception_handler   *exception_handler; // Handler function
    
    CYG_ADDRWORD            exception_data;     // Handler data
#endif

public:

    Cyg_Exception_Control();

    // Register an exception handler for either the specific exception
    // or for all exceptions.
    void register_exception(
        cyg_code                exception_number,       // exception number
        cyg_exception_handler   handler,                // handler function
        CYG_ADDRWORD            data,                   // data argument
        cyg_exception_handler   **old_handler,          // handler function
        CYG_ADDRWORD            *old_data               // data argument
        );

    // Remove an exception handler.
    void deregister_exception(
        cyg_code                exception_number        // exception number
        );

    // Deliver the given exception now by invoking the appropriate
    // exception handler.
    void deliver_exception(
        cyg_code            exception_number,       // exception being raised
        CYG_ADDRWORD        exception_info          // exception specific info
        );
};

#endif

// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_KERNEL_EXCEPT_HXX
// EOF except.hxx
