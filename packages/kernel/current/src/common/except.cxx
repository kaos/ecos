//==========================================================================
//
//	common/except.cxx
//
//	Exception handling implementation
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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	nickg
// Contributors:	nickg
// Date:	1998-04-09
// Purpose:	Exception handling implementation
// Description:	This file contains the code that registers and delivers
//              exceptions.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros
#include <cyg/kernel/instrmnt.h>       // instrumentation

#include <cyg/kernel/except.hxx>       // our header

#include <cyg/hal/hal_arch.h>          // architecture definitions
#include <cyg/hal/hal_intr.h>          // vector definitions

#include <cyg/kernel/thread.hxx>       // thread interface

#include <cyg/kernel/thread.inl>       // thread inlines

#ifdef CYGPKG_KERNEL_EXCEPTIONS

// -------------------------------------------------------------------------
// Null exception handler. This is used to capture exceptions that are
// not caught by user supplied handlers.

void cyg_null_exception_handler(
    CYG_ADDRWORD        data,                   // user supplied data
    cyg_code            exception_number,       // exception being raised
    CYG_ADDRWORD        exception_info          // any exception specific info
    )
{
    CYG_TRACE1( 1, "Uncaught exception: %d", exception_number);
}

// -------------------------------------------------------------------------
// Exception Controller constructor.

Cyg_Exception_Control::Cyg_Exception_Control()
{
#ifdef CYGSEM_KERNEL_EXCEPTIONS_DECODE

    for( int i = 0; i < CYG_EXCEPTION_COUNT ; i++ )
        exception_handler[i] = cyg_null_exception_handler,
            exception_data[i] = 0;
#else

    exception_handler = cyg_null_exception_handler;    
    exception_data = 0;
    
#endif

}

// -------------------------------------------------------------------------
// Exception registation. Stores the handler function and data to be used
// for handling the given exception number. Where exceptions are not decoded
// only a single handler may be registered for all exceptions. This function
// also returns the old values of the exception handler and data to allow
// chaining to be implemented.

void Cyg_Exception_Control::register_exception(
    cyg_code                exception_number,       // exception number
    cyg_exception_handler   handler,                // handler function
    CYG_ADDRWORD            data,                   // data argument
    cyg_exception_handler   **old_handler,          // handler function
    CYG_ADDRWORD            *old_data               // data argument
    )
{
    CYG_REPORT_FUNCTION();

    CYG_ASSERT( exception_number <= CYG_EXCEPTION_MAX,
                "Out of range exception number");
    CYG_ASSERT( exception_number >= CYG_EXCEPTION_MIN,
                "Out of range exception number");


    // Should we complain if there is already a registered
    // handler, or should we just replace is silently?
    
#ifdef CYGSEM_KERNEL_EXCEPTIONS_DECODE

    if( old_handler != NULL )
        *old_handler = exception_handler[exception_number];
    if( old_data != NULL )
        *old_data = exception_data[exception_number];
    exception_handler[exception_number] = handler;
    exception_data[exception_number] = data;
    
#else
    
    if( old_handler != NULL )
        *old_handler = exception_handler;
    if( old_data != NULL )
        *old_data = exception_data;
    exception_handler = handler;
    exception_data = data;
    
#endif
    
}

// -------------------------------------------------------------------------
// Exception deregistation. Revert the handler for the exception number
// to the default.

void Cyg_Exception_Control::deregister_exception(
    cyg_code                exception_number        // exception number
    )
{
    CYG_REPORT_FUNCTION();

    CYG_ASSERT( exception_number <= CYG_EXCEPTION_MAX,
                "Out of range exception number");
    CYG_ASSERT( exception_number >= CYG_EXCEPTION_MIN,
                "Out of range exception number");

#ifdef CYGSEM_KERNEL_EXCEPTIONS_DECODE

    exception_handler[exception_number] = cyg_null_exception_handler;
    exception_data[exception_number] = 0;
    
#else
    
    exception_handler = cyg_null_exception_handler;
    exception_data = 0;
    
#endif
    
}

// -------------------------------------------------------------------------
// Exception delivery. Call the appropriate exception handler.

void Cyg_Exception_Control::deliver_exception(
    cyg_code            exception_number,       // exception being raised
    CYG_ADDRWORD        exception_info          // exception specific info
    )
{
    cyg_exception_handler *handler = NULL;
    CYG_ADDRWORD data = 0;

    CYG_ASSERT( exception_number <= CYG_EXCEPTION_MAX,
                "Out of range exception number");
    CYG_ASSERT( exception_number >= CYG_EXCEPTION_MIN,
                "Out of range exception number");
    
#ifdef CYGSEM_KERNEL_EXCEPTIONS_DECODE

    handler = exception_handler[exception_number];
    data = exception_data[exception_number];
    
#else
    
    handler = exception_handler;
    data = exception_data;
    
#endif

    // The handler will always be a callable function: either the user's
    // registered function or the null handler. So it is always safe to
    // just go ahead and call it.
    
    handler( data, exception_number, exception_info );
    
}

// -------------------------------------------------------------------------
// Exception delivery function called from the HAL as a result of a
// hardware exception being raised.

externC void deliver_exception( CYG_WORD code, CYG_ADDRWORD data )
{
    Cyg_Thread::self()->deliver_exception( code, data );
}

// -------------------------------------------------------------------------
// Where exceptions are global, there is a single static instance of the
// exception control object. Define it here.

#ifdef CYGSEM_KERNEL_EXCEPTIONS_GLOBAL

Cyg_Exception_Control Cyg_Thread::exception_control;

#endif

// -------------------------------------------------------------------------

#endif

// -------------------------------------------------------------------------
// EOF common/except.cxx
