#ifndef CYGONCE_HAL_HAL_DIAG_H
#define CYGONCE_HAL_HAL_DIAG_H

//=============================================================================
//
//      hal_diag.h
//
//      HAL Support for Kernel Diagnostic Routines
//
//=============================================================================
//####UNSUPPORTEDBEGIN####
//
// -------------------------------------------
// This source file has been contributed to eCos/Cygnus. It may have been
// changed slightly to provide an interface consistent with those of other 
// files.
//
// The functionality and contents of this file is supplied "AS IS"
// without any form of support and will not necessarily be kept up
// to date by Cygnus.
//
// All inquiries about this file, or the functionality provided by it,
// should be directed to the 'ecos-discuss' mailing list (see
// http://sourceware.cygnus.com/ecos/intouch.html for details).
//
// Maintained by:  <Unmaintained>
// -------------------------------------------
//
//####UNSUPPORTEDEND####
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg
// Contributors:nickg
// Date:        1998-03-02
// Purpose:     HAL Support for Kernel Diagnostic Routines
// Description: Diagnostic routines for use during kernel development.
// Usage:       #include <cyg/hal/hal_diag.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>

//-----------------------------------------------------------------------------
// functions implemented in hal_diag.c

externC void hal_diag_init(void);

externC void hal_diag_write_char(char c);

externC void hal_diag_read_char(char *c);

//-----------------------------------------------------------------------------

#define HAL_DIAG_INIT() hal_diag_init()

#define HAL_DIAG_WRITE_CHAR(_c_) hal_diag_write_char(_c_)

#define HAL_DIAG_READ_CHAR(_c_) hal_diag_read_char(&_c_)

//-----------------------------------------------------------------------------
// end of hal_diag.h
#endif // CYGONCE_HAL_HAL_DIAG_H
