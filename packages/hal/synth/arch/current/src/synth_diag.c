//=============================================================================
//
//      synth_diag.c
//
//      Synthetic target diagnostic output code
//
//=============================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   proven
// Contributors:proven, bartv
// Date:        1998-10-05
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//              There are two possible ways of performing I/O. The first
//              involves simply writing to stdout. This is robust, but
//              has some disadvantages such as the output getting mixed up
//              with gdb output. The second involves sending the data on
//              to the auxiliary, less robust but much more flexible.
//
//              Similarly, input can be handled by reading from stdin or
//              by a suitable device in the auxiliary.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_io.h>
#include <cyg/infra/cyg_ass.h>

//-----------------------------------------------------------------------------

// When the auxiliary exists, hal_diag_init() will need to contact it and
// get hold of a suitable console device.
//
// When interacting with stdin/stdout, arguably there should be some
// manipulation of tty settings e.g. to support single character
// input. However it is not clear which settings would be preferable
// to the default.

void hal_diag_init( void )
{
}

// Output a single character.
//
// The calling code will output one character at a time. Output
// involves at least one system call, and this is expensive for
// a single character (especially when used in conjunction with
// I/O intensive facilities like unbuffered tracing). Therefore
// this code will buffer lines up to 128 characters before
// doing the I/O.

void hal_diag_write_char(char c)
{
    static int  diag_index = 0;
    static char diag_buffer[128];

    CYG_ASSERT(diag_index < 128, "Diagnostic buffer overflow");
    
    diag_buffer[diag_index++] = c;
    if (('\n' == c) || (128 == diag_index)) {
        int     written;
        char*   next    = diag_buffer;

        while (diag_index > 0) {
            written = cyg_hal_sys_write(1, next, diag_index);
            if (written > 0) {
                diag_index -= written;
                next       += written;
            } else if ((-CYG_HAL_SYS_EINTR != written) && (-CYG_HAL_SYS_EAGAIN != written)) {
                CYG_FAIL("Unexpected error writing to stdout.");
                diag_index = 0;
                break;
            }
        }
        CYG_ASSERT(0 == diag_index, "All data should have been written out");
        diag_index = 0;
        cyg_hal_sys_fdatasync(1);
    }
}

// Diagnostic input. It is not clear that this is actually useful. The
// read syscall will get woken up by the itimer alarm, but we don't
// want to stop reading if that's the case

void hal_diag_read_char(char *c)
{
    int rc;
    do {
        rc = cyg_hal_sys_read(0, c, 1);
    } while ((-CYG_HAL_SYS_EINTR == rc) || (-CYG_HAL_SYS_EAGAIN == rc));
}

//-----------------------------------------------------------------------------
// End of hal_diag.c
