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
externC unsigned long cyg_hal_sys_fdatasync(int);

// Write characters to a buffer which is flushed at newline/overflow to
// improve performance.
void hal_diag_write_char(char __c)
{
    static cyg_int32 __index = 0;
    static cyg_uint8 __buffer[128];

    __buffer[__index++] = __c;

    if ('\n' == __c || 128 == __index) {
        cyg_hal_sys_write(1, &__buffer[0], __index);
        cyg_hal_sys_fdatasync(1);
        __index = 0;
    }
}

// Hmm... we need the definition of EINTR, which normally lives in
// <asm/errno.h> on linux. But we can't get at that here, so we just
// hard-code it. The burden of binary compatibility means x86 linux will
// never change it.
#define x86_linux_EINTR 4

void hal_diag_read_char(char *c)
{
    int rc;

    // The read syscall will get woken up by the itimer alarm, but we don't
    // want to stop reading if that's the case
    do {
        rc = cyg_hal_sys_read(0, c, 1);
    } while (-x86_linux_EINTR == rc);
}

//-----------------------------------------------------------------------------
// End of hal_diag.c
