#ifndef CYGONCE_LIBC_CLIBINCL_SETJMPSUPP_HXX
#define CYGONCE_LIBC_CLIBINCL_SETJMPSUPP_HXX
//===========================================================================
//
//      setjmpsupp.hxx
//
//      Support header file for ANSI standard non-local jumps
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
// Description: Header file to support ANSI standard non-local jumps as
//              per ANSI para 7.6
// Usage:       #include "clibincl/setjmpsupp.hxx"
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
#include <setjmp.h>                // Normal setjmp header for jmp_buf

// FUNCTION PROTOTYPES

// longjmp() function, as described in ANSI para 7.6.2.1
externC void
_longjmp( jmp_buf, int ) CYGPRI_LIBC_NORETURN;


#endif // ifdef CYGPKG_LIBC     

#endif // CYGONCE_LIBC_CLIBINCL_SETJMPSUPP_HXX multiple inclusion protection

// EOF setjmpsupp.hxx
