//========================================================================
//
//      freopen.cxx
//
//      Implementation of C library freopen() function
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-01-21
// Purpose:       Provides ISO C freopen() function
// Description:   Implementation of C library freopen() function as per
//                ISO C standard chap. 7.9.5.4
// Usage:       
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Do we want the stdio stuff? And we only want freopen() if we have fopen()
#if defined(CYGPKG_LIBC_STDIO) && defined(CYGPKG_LIBC_STDIO_OPEN)


// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common project-wide type definitions
#include <stddef.h>                 // NULL and size_t from compiler
#include <stdio.h>                  // header for freopen()
#include "clibincl/stdiosupp.hxx"   // support for stdio


// EXPORTED SYMBOLS

externC FILE *
freopen( const char *, const char *, FILE * ) 
    CYGBLD_ATTRIB_WEAK_ALIAS(_freopen);


// FUNCTIONS


externC FILE *
_freopen( const char *, const char *, FILE * )
{
    return NULL;  // Returning NULL is valid! FIXME
} // _freopen()
        
#endif // if defined(CYGPKG_LIBC_STDIO) && defined(CYGPKG_LIBC_STDIO_OPEN)

// EOF freopen.cxx
