//==========================================================================
//
//        tcdiag.cxx
//
//        Kernel diag test harness.
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     dsm
// Contributors:    dsm
// Date:          1998-03-17
// Description:   Test harness implementation that uses the kernel's
//                diag channel.  This is intended for manual testing
//                of the kernel.
//####DESCRIPTIONEND####

#include <testcase.h>
#include <diag.h>

void cyg_test_init()
{
    diag_init();
}

void cyg_test_output(int status, char *msg, int line, char *file)
{
    char *st;

    switch (status) {
    case 0:
        st = "FAIL:";
        break;
    case 1:
        st = "PASS:";
        break;
    case 2:
        st = "EXIT:";
        break;
    case 3:
        st = "INFO:";
        break;
    }

    diag_write_string(st);
    diag_write_char('<');
    diag_write_string(msg);
    diag_write_string("> Line: ");
    diag_write_dec(line);
    diag_write_string(", File: ");
    diag_write_string(file);
    diag_write_char('\n');

}

// This is an appropriate function to set a breakpoint on
void cyg_test_exit()
{
    for(;;)
        ;
}
// EOF tcdiag.cxx
