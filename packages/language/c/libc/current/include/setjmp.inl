#ifndef CYGONCE_LIBC_SETJMP_INL
#define CYGONCE_LIBC_SETJMP_INL
//===========================================================================
//
//      setjmp.inl
//
//      Inline functions for ANSI standard non-local jumps
//
//===========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1998-02-13
// Purpose:     
// Description: Inline functions to implement ANSI standard non-local jumps as
//              per ANSI para 7.6
// Usage:       Do not include this file, #include <setjmp.h> instead
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_trac.h>    // Common tracing code
#include <cyg/infra/cyg_ass.h>     // Common assertion code
#include <setjmp.h>                // Header for this file, just in case
#include <cyg/hal/hal_arch.h>      // HAL architecture specific implementation

// INLINE FUNCTIONS

CYGPRI_LIBC_INLINE void
longjmp( jmp_buf cyg_buf, int cyg_val)
{
    CYG_REPORT_FUNCNAME( "longjmp" );
    CYG_REPORT_FUNCARG2( "&cyg_buf=%08x, cyg_val=%d", &cyg_buf, cyg_val );

    // ANSI says that if we are passed cyg_val==0, then we change it to 1
    if (cyg_val == 0)
        ++cyg_val;

    // we let the HAL do the work

    HAL_REORDER_BARRIER(); // prevent any chance of optimisation re-ordering
    hal_longjmp( cyg_buf, cyg_val );
    HAL_REORDER_BARRIER(); // prevent any chance of optimisation re-ordering

#ifdef CYGDBG_USE_ASSERTS
    CYG_ASSERT( 0, "longjmp should not have reached this point!" );
#else
    for (;;)
        CYG_EMPTY_STATEMENT;
#endif
} // longjmp()


#endif // ifdef CYGPKG_LIBC     

#endif // CYGONCE_LIBC_SETJMP_INL multiple inclusion protection

// EOF setjmp.inl
