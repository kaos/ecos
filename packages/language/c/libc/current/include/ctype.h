#ifndef CYGONCE_LIBC_CTYPE_H
#define CYGONCE_LIBC_CTYPE_H
//===========================================================================
//
//      ctype.h
//
//      ANSI standard ctype routines defined in section 7.3 of the standard
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
// Author(s):   jlarmour
// Contributors:  jlarmour@cygnus.co.uk
// Date:        1998-02-13
// Purpose:     
// Description: 
// Usage:       #include <ctype.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>       // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

// INCLUDES

#include <cyg/infra/cyg_type.h> // Common type definitions and support


// FUNCTION PROTOTYPES

//===========================================================================

// 7.3.1 Character testing functions

externC int
isalnum( int );

externC int
isalpha( int );

externC int
iscntrl( int );

externC int
isdigit( int );

externC int
isgraph( int );

externC int
islower( int );

externC int
isprint( int );

externC int
ispunct( int );

externC int
isspace( int );

externC int
isupper( int );

externC int
isxdigit( int );

//===========================================================================

// 7.3.2 Character case mapping functions

externC int
tolower( int );

externC int
toupper( int );


// INLINE FUNCTIONS

#ifdef CYGIMP_LIBC_CTYPE_INLINES
#include <ctype.inl>
#endif

#endif // ifdef CYGPKG_LIBC     

#endif // CYGONCE_LIBC_CTYPE_H multiple inclusion protection

// EOF ctype.h
