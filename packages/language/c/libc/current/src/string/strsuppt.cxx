//===========================================================================
//
//      strsuppt.cxx
//
//      Support for the ANSI standard string functions
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
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
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

#include <cyg/infra/cyg_type.h>      // Common type definitions
#include "clibincl/stringsupp.hxx"   // Header for this file

// CONSTANTS

// Masks for CYG_LIBC_DETECTNULL macro so they are only defined once in
// the library
const cyg_uint64 Cyg_libc_str_null_mask_1 = 0x0101010101010101LL;
const cyg_uint64 Cyg_libc_str_null_mask_2 = 0x8080808080808080LL;

#endif // ifdef CYGPKG_LIBC     

// EOF strsuppt.cxx
