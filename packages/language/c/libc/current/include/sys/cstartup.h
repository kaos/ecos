#ifndef CYGONCE_LIBC_CSTARTUP_H
#define CYGONCE_LIBC_CSTARTUP_H
//===========================================================================
//
//      cstartup.h
//
//      Starting C library support
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
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-03-23
// Purpose:     
// Description: 
// Usage:         #include <sys/cstartup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================

// FUNCTION PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif

extern void
cyg_iso_c_start( void );

#ifdef __cplusplus
}
#endif

#endif CYGONCE_LIBC_CSTARTUP_H multiple inclusion protection

// EOF cstartup.h
