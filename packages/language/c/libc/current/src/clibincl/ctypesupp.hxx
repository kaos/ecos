#ifndef CYGONCE_LIBC_CLIBINCL_CTYPESUPP_HXX
#define CYGONCE_LIBC_CLIBINCL_CTYPESUPP_HXX
//========================================================================
//
//      ctypesupp.hxx
//
//      Support for ANSI ctype functions
//
//========================================================================
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
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1998-02-13
// Purpose:     
// Description: 
// Usage:       #include "clibincl/ctypesupp.hxx"
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

// INCLUDES

#include <cyg/infra/cyg_type.h> // Common type definitions and support


// FUNCTION PROTOTYPES

// These are function prototypes for the aliased functions that actually
// implement the ctype functions

//========================================================================

// 7.3.1 Character testing functions

externC int
_isalnum( int );

externC int
_isalpha( int );

externC int
_iscntrl( int );

externC int
_isdigit( int );

externC int
_isgraph( int );

externC int
_islower( int );

externC int
_isprint( int );

externC int
_ispunct( int );

externC int
_isspace( int );

externC int
_isupper( int );

externC int
_isxdigit( int );

//========================================================================

// 7.3.2 Character case mapping functions

externC int
_tolower( int );

externC int
_toupper( int );


#endif // ifdef CYGPKG_LIBC     

#endif // CYGONCE_LIBC_CLIBINCL_CTYPESUPP_HXX multiple inclusion protection

// EOF ctypesupp.hxx
