//=================================================================
//
//        basic.c
//
//        HAL basic functions test
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
// Author(s):     gthomas
// Contributors:  gthomas
// Date:          2001-10-24
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/infra/testcase.h>         // test case macros
#include <cyg/infra/diag.h>             // diag_printf
#include <cyg/infra/cyg_ass.h>          // assertions

#include <cyg/hal/hal_arch.h>           // context macros

// -------------------------------------------------------------------------

#define BITS(t) (sizeof(t)*8)

void
entry(void)
{
    int res;
    int i, mask, ndx;
    hal_jmp_buf jmpbuf;

    res = 1;

    // Check HAL_MSBIT_NDEX() functions
    mask = 1;  // One bits set
    for (i = 0;  i < BITS(int);  i++) {
        HAL_MSBIT_INDEX(ndx, mask);
        res &= (ndx == i);
        HAL_LSBIT_INDEX(ndx, mask);
        res &= (ndx == i);
        mask <<= 1;
    }

    mask = 3;  // Two bits set
    for (i = 0;  i < BITS(int)-1;  i++) {
        HAL_MSBIT_INDEX(ndx, mask);
        res &= (ndx == (i+1));
        HAL_LSBIT_INDEX(ndx, mask);
        res &= (ndx == i);
        mask <<= 1;
    }
    CYG_TEST_PASS_FAIL(res, "HAL_xSBIT_INDEX() basic functions");

    res = 0;
    if (hal_setjmp(jmpbuf)) {
        res = 1;
    } else {
        hal_longjmp(jmpbuf, 1);
    }
    CYG_TEST_PASS_FAIL(res, "hal_setjmp()/hal_longjmp() basic functions");

    CYG_TEST_FINISH("HAL basic functions test");
}

// -------------------------------------------------------------------------

externC void
cyg_start( void )
{
    CYG_TEST_INIT();

    entry();
}

// -------------------------------------------------------------------------
// EOF basic.c
