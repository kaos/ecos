#ifndef CYGONCE_LIBC_ASSERT_H
#define CYGONCE_LIBC_ASSERT_H
//===========================================================================
//
//      assert.h
//
//      ANSI standard assertion support as per ANSI chapter 7.2
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
// Description: ANSI standard assertion support as per ANSI chapter 7.2
// Usage:       #include <assert.h>
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
#include <cyg/infra/cyg_ass.h>     // Common assertion support

// MACROS

// The user should define NDEBUG to turn off these user assertions. Also the
// common infrastructure assertions should be turned off too.

#ifdef NDEBUG

# define assert( _bool_ ) ((void)0)

#else // if NDEBUG is NOT defined

# define assert( _bool_ ) \
        CYG_MACRO_START \
        CYG_ASSERT( _bool_, "User assertion failed: \""  # _bool_ "\"" ); \
        CYG_MACRO_END

#endif // if NDEBUG is NOT defined



#endif // ifdef CYGPKG_LIBC     

#endif // CYGONCE_LIBC_ASSERT_H multiple inclusion protection

// EOF assert.h
