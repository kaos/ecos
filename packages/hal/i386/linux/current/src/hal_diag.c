//=============================================================================
//
//      hal_diag.c
//
//      HAL diagnostic output code
//
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
// October 31, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   proven
// Contributors:proven
// Date:        1998-10-05
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types

#include <cyg/hal/hal_diag.h>

//-----------------------------------------------------------------------------

void hal_diag_init( void )
{
}

externC unsigned long cyg_hal_sys_write(int, const void*, long);
externC unsigned long cyg_hal_sys_read(int, void*, long);

// Write characters to a buffer which is flushed at newline/overflow to
// improve performance.
void hal_diag_write_char(char __c)
{
    static cyg_int32 __index = 0;
    static cyg_uint8 __buffer[128];

    __buffer[__index++] = __c;

    if ('\n' == __c || 128 == __index) {
        cyg_hal_sys_write(1, &__buffer[0], __index);
        __index = 0;
    }
}

void hal_diag_read_char(char *c)
{
    cyg_hal_sys_read(0, c, 1);
}

//-----------------------------------------------------------------------------
// End of hal_diag.c
