//==========================================================================
//
//      null.cxx
//
//      Null trace and assert functions
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
// Author(s):   nickg
// Contributors:        nickg
// Date:        1997-09-15
// Purpose:     Trace and assert functions
// Description: The functions in this file are null implementations of the
//              standard trace and assert functions. These can be used to
//              eliminate all trace and assert messages without recompiling.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/infra.h>

#ifndef CYGDBG_USE_ASSERTS
// then you get an empty function regardless, so that other code can link:
// (for example libc assert() calls this whether or not eCos code has
//  asserts enabled - of course if it does, assert() maps to the chosen
//  implementation; if not, it's the same inner function to breakpoint.)

#include <cyg/infra/cyg_type.h>        // base types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros

externC void
cyg_assert_fail( const char *psz_func, const char *psz_file,
                 cyg_uint32 linenum, const char *psz_msg )
{
    for(;;);
};
#endif // not CYGDBG_USE_ASSERTS

#ifdef CYGDBG_INFRA_DEBUG_TRACE_ASSERT_NULL

#include <cyg/infra/cyg_type.h>        // base types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros

// -------------------------------------------------------------------------
// Trace functions:

#ifdef CYGDBG_USE_TRACING

externC void
cyg_tracenomsg( char *psz_func, char *psz_file, cyg_uint32 linenum )
{}

// provide every other one of these as a space/caller bloat compromise.

externC void
cyg_tracemsg( cyg_uint32 what, 
              char *psz_func, char *psz_file, cyg_uint32 linenum,
              char *psz_msg )
{}

externC void
cyg_tracemsg2( cyg_uint32 what, 
               char *psz_func, char *psz_file, cyg_uint32 linenum,
               char *psz_msg,
               CYG_ADDRWORD arg0,  CYG_ADDRWORD arg1 )
{}
externC void
cyg_tracemsg4( cyg_uint32 what, 
               char *psz_func, char *psz_file, cyg_uint32 linenum,
               char *psz_msg,
               CYG_ADDRWORD arg0,  CYG_ADDRWORD arg1,
               CYG_ADDRWORD arg2,  CYG_ADDRWORD arg3 )
{}
externC void
cyg_tracemsg6( cyg_uint32 what, 
               char *psz_func, char *psz_file, cyg_uint32 linenum,
               char *psz_msg,
               CYG_ADDRWORD arg0,  CYG_ADDRWORD arg1,
               CYG_ADDRWORD arg2,  CYG_ADDRWORD arg3,
               CYG_ADDRWORD arg4,  CYG_ADDRWORD arg5 )
{}
externC void
cyg_tracemsg8( cyg_uint32 what, 
               char *psz_func, char *psz_file, cyg_uint32 linenum,
               char *psz_msg,
               CYG_ADDRWORD arg0,  CYG_ADDRWORD arg1,
               CYG_ADDRWORD arg2,  CYG_ADDRWORD arg3,
               CYG_ADDRWORD arg4,  CYG_ADDRWORD arg5,
               CYG_ADDRWORD arg6,  CYG_ADDRWORD arg7 )
{}

// -------------------------------------------------------------------------

externC void
cyg_trace_dump(void)
{}

#endif // CYGDBG_USE_TRACING

// -------------------------------------------------------------------------
// Assert functions:

#ifdef CYGDBG_USE_ASSERTS

externC void
cyg_assert_fail( const char *psz_func, const char *psz_file,
                 cyg_uint32 linenum, const char *psz_msg )
{
    for(;;);
};

extern "C"
{
extern unsigned long CYG_LABEL_NAME(stext);
extern unsigned long CYG_LABEL_NAME(etext);

unsigned long stext_addr = (unsigned long)&CYG_LABEL_NAME(stext);
unsigned long etext_addr = (unsigned long)&CYG_LABEL_NAME(etext);
};

externC cyg_bool cyg_check_data_ptr(void *ptr)
{
    unsigned long p = (unsigned long)ptr;
    
    if( p == 0 ) return false;

    if( p > stext_addr && p < etext_addr ) return false;

    return true;
}

externC cyg_bool cyg_check_func_ptr(void (*ptr)(void))
{
    unsigned long p = (unsigned long)ptr;
    
    if( p == 0 ) return false;

    if( p < stext_addr && p > etext_addr ) return false;

    return true;
}

#endif // CYGDBG_USE_ASSERTS

#endif // CYGDBG_INFRA_DEBUG_TRACE_ASSERT_NULL

// -------------------------------------------------------------------------
// EOF null.cxx
