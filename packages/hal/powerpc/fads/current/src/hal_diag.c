//=============================================================================
//
//      hal_diag.c
//
//      HAL diagnostic output code
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
// Contributed by: Kevin Hester <khester@opticworks.com>
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
// Author(s):   nickg, jskov
// Contributors:nickg
// Date:        1998-03-02
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // Interrupt macros

#if defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
#include <cyg/hal/hal_stub.h>           // hal_output_gdb_string
#endif

#include <cyg/hal/ppc_regs.h>
#include <cyg/hal/quicc_smc2.h>

//-----------------------------------------------------------------------------
// Select default diag channel to use

//#define CYG_KERNEL_DIAG_ROMART
//#define CYG_KERNEL_DIAG_SERIAL

#if !defined(CYG_KERNEL_DIAG_SERIAL)
#define CYG_KERNEL_DIAG_SERIAL
#endif

//-----------------------------------------------------------------------------
// Fads board specific serial

#if defined(CYG_KERNEL_DIAG_SERIAL)

void hal_diag_init(void)
{
#if !defined(CYGDBG_KERNEL_DEBUG_GDB_INCLUDE_STUBS)
    // init the actual serial port
    cyg_smc2_init(9600);
#endif
}

void hal_diag_write_char(char c)
{
#if !defined(CYGDBG_KERNEL_DEBUG_GDB_INCLUDE_STUBS)
    unsigned long __state;
    HAL_DISABLE_INTERRUPTS(__state)
    cyg_smc2_putchar(c);
    HAL_RESTORE_INTERRUPTS(__state);
#else
    hal_output_gdb_string (&c, 1);
#endif
}

void hal_diag_read_char(char *c)
{
    *c = cyg_smc2_getchar();
}
#endif


