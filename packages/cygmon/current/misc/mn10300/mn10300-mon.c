//==========================================================================
//
//      mn10300-mon.c
//
//      Support code to extend the generic monitor code to support
//      MN10300 processors.
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    dmoseley
// Contributors: dmoseley
// Date:         2000-08-11
// Purpose:      Support code to extend the generic monitor code to support
//               MN10300 processors.
// Description:  Further board specific support is in other files.
//               This file contains:
//                 register names lookup table
//
//               Empty Stubs:
//                 Interval timer - This should really belong to the application
//                 operating system.
//
//               Should not contain:
//                 low level uart getchar and putchar functions
//                 delay function to support uart
//
//####DESCRIPTIONEND####
//
//=========================================================================

#include "monitor.h"

struct regstruct regtab[] =
{
    {"d0",       D0,          1},
    {"d1",       D1,          1},
    {"d2",       D2,          1},
    {"d3",       D3,          1},
    {"a0",       A0,          1},
    {"a1",       A1,          1},
    {"a2",       A2,          1},
    {"a3",       A3,          1},
    {"sp",       SP,          1},
    {"pc",       PC,          1},
    {"mdr",      MDR,         1},
    {"psw",      PSW,         1},
    {"lir",      LIR,         1},
    {"lar",      LAR,         1},
#ifdef CYGPKG_HAL_MN10300_AM33
    {"r0",       R0,          1},
    {"r1",       R1,          1},
    {"r2",       R2,          1},
    {"r3",       R3,          1},
    {"r4",       R4,          1},
    {"r5",       R5,          1},
    {"r6",       R6,          1},
    {"r7",       R7,          1},
    {"ssp",      SSP,         1},
    {"msp",      MSP,         1},
    {"usp",      USP,         1},
    {"mcrh",     MCRH,        1},
    {"mcrl",     MCRL,        1},
    {"mcvf",     MCVF,        1},
    {"mdrq",     MDRQ,        1},
#endif  
    { 0,         0,           1}, /* Terminating element must be last */
} ;

void
initialize_mon(void)
{
} /* initialize_mon */


#ifdef CYGPKG_HAL_MN10300_AM33
extern int msp_read;
#endif

void
initialize_mon_each_time(void)
{
  int i;

#ifdef CYGPKG_HAL_MN10300_AM33
  // Make sure the regtab[] indicates the valid status of the MSP
  for (i = 0; regtab[i].registername != NULL; i++)
    {
      if (regtab[i].registernumber == MSP)
          regtab[i].registervalid = msp_read;
    }
#endif
} /* initialize_mon_each_time */


#include <cyg/hal/hal_arch.h>
#include <bsp/common/bsp_if.h>
int
machine_syscall(HAL_SavedRegisters *regs)
{
    int res, err;
    target_register_t d0, d1, d2, d3;

    d0 = get_register(D0);
    d1 = get_register(D1);
    d2 = get_register(D2);
    d3 = get_register(D3);

    err = _bsp_do_syscall(d0, // Function
                          d1, d2, d3, 0, // arguments,
                          &res);
    if (err)
    {
        // This was a syscall.  It has now been handled, so update the registers appropriately
        put_register(D0, res);
        bsp_skip_instruction(regs);
    }

    return err;
}


// Utility function for printing breakpoints
void bp_print(target_register_t bp_val)
{
    bsp_printf("0x%08lx\n", (unsigned long)bp_val);
}
