//=================================================================
//
//        vaargs.c
//
//        HAL variable argument calls test
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jskov
// Contributors:  jskov
// Date:          2001-08-03
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/infra/testcase.h>         // test case macros
#include <cyg/infra/diag.h>             // diag_printf
#include <cyg/infra/cyg_ass.h>          // assertions

#include <cyg/hal/hal_arch.h>           // context macros

#include <stdarg.h>                     // vaargs magic

// -------------------------------------------------------------------------

int
function(int n, ...)
{
    va_list args;
    int c = 11 * n;
    int i = 1;
    int res = 1;

    CYG_ASSERT(n >= 0 && n < 8, "Invalid count argument");

    va_start(args, n);

    for (i = 1; i <= n; c++, i++) {
        int v = va_arg(args, int);
        if (v != c) {
            diag_printf("FAIL:<Bad argument: index %d expected %d got %d>\n", i, c, v);
            res = 0;
        }
    }       

    va_end(args);

    return res;
}

int
function_proxy(int n, va_list args)
{
    int c = 11 * n;
    int i = 1;
    int res = 1;

    CYG_ASSERT(n >= 0 && n < 8, "Invalid count argument");

    for (i = 1; i <= n; c++, i++) {
        int v = va_arg(args, int);
        if (v != c) {
            diag_printf("FAIL:<Bad argument: index %d expected %d got %d>\n", i, c, v);
            res = 0;
        }
    }       

    return res;
}

int
proxy(int n, ...)
{
    int res;
    va_list args;

    va_start(args, n);
    res = function_proxy(n, args);
    va_end(args);

    return res;
}


void
entry(void)
{
    int res;

    res =  function(0);
    res &= function(1, 11);
    res &= function(2, 22, 23);
    res &= function(3, 33, 34, 35);
    res &= function(4, 44, 45, 46, 47);
    res &= function(5, 55, 56, 57, 58, 59);
    res &= function(6, 66, 67, 68, 69, 70, 71);
    res &= function(7, 77, 78, 79, 80, 81, 82, 83);
    CYG_TEST_PASS_FAIL(res, "Direct vaargs calls");

    res =  proxy(0);
    res &= proxy(1, 11);
    res &= proxy(2, 22, 23);
    res &= proxy(3, 33, 34, 35);
    res &= proxy(4, 44, 45, 46, 47);
    res &= proxy(5, 55, 56, 57, 58, 59);
    res &= proxy(6, 66, 67, 68, 69, 70, 71);
    res &= proxy(7, 77, 78, 79, 80, 81, 82, 83);
    CYG_TEST_PASS_FAIL(res, "Proxy vaargs calls");

    CYG_TEST_FINISH("HAL vaargs test");
}

// -------------------------------------------------------------------------

externC void
cyg_start( void )
{
    entry();
}

// -------------------------------------------------------------------------
// EOF vaargs.c
