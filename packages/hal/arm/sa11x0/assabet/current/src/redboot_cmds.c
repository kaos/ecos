//==========================================================================
//
//      redboot_cmds.c
//
//      Assabet [platform] specific RedBoot commands
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
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-07-14
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <redboot.h>

#include <cyg/hal/hal_sa11x0.h>   // Board definitions
#include <cyg/hal/assabet.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>


// Exported CLI function(s)
RedBoot_cmd("exec", 
            "Execute an image - with MMU off", 
            "[-w timeout] [<entry_point>]",
            do_exec
    );

typedef void code_fun(void);

static void 
do_exec(int argc, char *argv[])
{
    unsigned long entry;
    unsigned long oldints;
    code_fun *fun, *prg;
    bool wait_time_set;
    int  wait_time, res, i;
    struct option_info opts[1];
    char line[8];
    unsigned long _prg[32], *ip;

    entry = (unsigned long)entry_address;  // Default from last 'load' operation
    init_opts(&opts[0], 'w', true, OPTION_ARG_TYPE_NUM, 
              (void **)&wait_time, (bool *)&wait_time_set, "wait timeout");
    if (!scan_opts(argc, argv, 1, opts, 1, (void *)&entry, OPTION_ARG_TYPE_NUM, "starting address"))
    {
        return;
    }
    if (wait_time_set) {
        printf("About to start execution at %p - abort with ^C within %d seconds\n",
               (void *)entry, wait_time);
        res = gets(line, sizeof(line), wait_time*1000);
        if (res == _GETS_CTRLC) {
            return;
        }
    }
    ip = (unsigned long *)&&lab1;
    for (i = 0;  i < (unsigned long)&&end1-(unsigned long)&&lab1;  i++) {
        _prg[i] = *ip++;
    }
    HAL_DISABLE_INTERRUPTS(oldints);
    HAL_DCACHE_SYNC();
    HAL_ICACHE_DISABLE();
    HAL_DCACHE_DISABLE();
    HAL_DCACHE_SYNC();
    HAL_ICACHE_INVALIDATE_ALL();
    HAL_DCACHE_INVALIDATE_ALL();
    ip = (unsigned long *)_prg;
    // Not call this code
    fun = (code_fun *)((entry & 0x0FFFFFFF) + SA11X0_RAM_BANK0_BASE);  // Absolute address
    prg = (code_fun *)((unsigned long)ip + SA11X0_RAM_BANK0_BASE);  // Absolute address
    asm volatile ("mov r5,%0" : : "r"(fun) : "r5");
    asm volatile ("mov r1,%0; mov pc,r1" : : "r"(prg));
 lab1:
    // Tricky code.  We are currently running with the MMU on and the
    // memory map convoluted from 1-1.  This code must be copied to RAM
    // and then executed at the non-mapped address.  Then when the code
    // turns off the MMU, the control flow is unchanged and control can
    // be passed safely to the program to be executed.
    // This magic was created in order to be able to execute standard
    // Linux kernels with as little change/perturberance as possible.
    asm volatile ("mrs r0,cpsr;"
                  "msr spsr,r0;"
                  "bic r0,r0,#0x1F;"  // Put processor in IRQ mode
                  "orr r0,r0,#0x12;"
                  "msr cpsr,r0;"
                  "mov lr,r5;"
                  "mov sp,r5;"        // Give the kernel a stack just below the entry point
                  "mov r1,#0;"
                  "mcr p15,0,r1,c1,c0;"
                  "mov r0,#0;"        // Set up Assabet load magic
                  "mov r1,#0x19;" 
                  "movs pc,lr" : : );
 end1:
}
