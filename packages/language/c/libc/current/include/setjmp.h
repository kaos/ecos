#ifndef CYGONCE_LIBC_SETJMP_H
#define CYGONCE_LIBC_SETJMP_H
//===========================================================================
//
//      setjmp.h
//
//      ANSI standard non-local jumps
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour@cygnus.co.uk
// Contributors:  jlarmour@cygnus.co.uk
// Date:        1998-02-13
// Purpose:     
// Description: Header file to implement ANSI standard non-local jumps as
//              per ANSI para 7.6
// Usage:       #include <setjmp.h>
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
#include <cyg/hal/hal_arch.h>      // HAL architecture specific implementation


// TYPE DEFINITIONS

// jmp_buf as per ANSI 7.6. This is simply the underlying HAL buffer

typedef hal_jmp_buf jmp_buf;

// MACROS

// setjmp() function, as described in ANSI para 7.6.1.1
#define setjmp( __env__ )  hal_setjmp( __env__ )

// FUNCTION PROTOTYPES

// longjmp() function, as described in ANSI para 7.6.2.1
externC void
longjmp( jmp_buf, int ) CYGPRI_LIBC_NORETURN;


// INLINE FUNCTIONS

#ifdef CYGIMP_LIBC_SETJMP_INLINES
# include <setjmp.inl>
#endif


#endif // ifdef CYGPKG_LIBC     

#endif // CYGONCE_LIBC_SETJMP_H multiple inclusion protection

// EOF setjmp.h
