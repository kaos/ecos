#ifndef CYGONCE_LIBC_CLIBINCL_TIMESUPP_HXX
#define CYGONCE_LIBC_CLIBINCL_TIMESUPP_HXX
//===========================================================================
//
//      timesupp.hxx
//
//      Support for the ANSI standard time functions
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
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

// INCLUDES

#include <cyg/infra/cyg_type.h>       // Common type definitions

// FUNCTION PROTOTYPES

// These are function prototypes for the aliased functions that actually
// implement the time functions

//===========================================================================

// 7.12.2 Time manipulation functions

externC clock_t
_clock( void );


#endif // ifdef CYGPKG_LIBC     

#endif // CYGONCE_LIBC_CLIBINCL_TIMESUPP_HXX multiple inclusion protection

// EOF timesupp.hxx
