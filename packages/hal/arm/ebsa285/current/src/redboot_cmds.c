//==========================================================================
//
//      redboot_cmds.c
//
//      EBSA-285 [platform] specific RedBoot commands
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
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

#include <cyg/hal/hal_ebsa285.h>   // Board definitions
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>

// Exported CLI function(s)
static void do_exec(int argc, char *argv[]);
RedBoot_cmd("exec", 
            "Execute an image - with MMU off", 
            "[-w timeout] [-b <load addr> [-l <length>]] 
             [-r <ramdisk addr> [-s <ramdisk length>]] 
             [-c \"kernel command line\"] [<entry_point>]",
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
    bool base_addr_set, length_set, cmd_line_set;
    bool ramdisk_addr_set, ramdisk_size_set;
    unsigned long base_addr, length;
    unsigned long ramdisk_addr, ramdisk_size;
    struct option_info opts[6];
    char line[8];
    unsigned long *_prg, *ip;
    char *cmd_line;
    struct param_struct {
        unsigned long key;
        unsigned long rd_start;
        unsigned long rd_size;
        unsigned char cmdline[256];
    } *params = (struct param_struct *)0x100;

    entry = (unsigned long)entry_address;  // Default from last 'load' operation
    base_addr = 0x8000;
    init_opts(&opts[0], 'w', true, OPTION_ARG_TYPE_NUM, 
              (void **)&wait_time, (bool *)&wait_time_set, "wait timeout");
    init_opts(&opts[1], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void **)&base_addr, (bool *)&base_addr_set, "base address");
    init_opts(&opts[2], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&length, (bool *)&length_set, "length");
    init_opts(&opts[3], 'c', true, OPTION_ARG_TYPE_STR, 
              (void **)&cmd_line, (bool *)&cmd_line_set, "kernel command line");
    init_opts(&opts[4], 'r', true, OPTION_ARG_TYPE_NUM, 
              (void **)&ramdisk_addr, (bool *)&ramdisk_addr_set, "ramdisk_addr");
    init_opts(&opts[5], 's', true, OPTION_ARG_TYPE_NUM, 
              (void **)&ramdisk_size, (bool *)&ramdisk_size_set, "ramdisk_size");
    if (!scan_opts(argc, argv, 1, opts, 6, (void *)&entry, OPTION_ARG_TYPE_NUM, "starting address"))
    {
        return;
    }
    if (cmd_line_set || ramdisk_addr_set) {
        params->key = 0xDEADF00D;;
        params->rd_size = 0;
    }
    if (cmd_line_set) {
        strcpy(params->cmdline, cmd_line);
    }
    if (ramdisk_addr_set) {
        params->rd_start = ramdisk_addr;
        if (ramdisk_size_set) {
            params->rd_size = ramdisk_size;
        } else {
            params->rd_size = 4096*1024;
        }
    }
    if (wait_time_set) {
        diag_printf("About to start execution at %p - abort with ^C within %d seconds\n",
                    (void *)entry, wait_time);
        res = _rb_gets(line, sizeof(line), wait_time*1000);
        if (res == _GETS_CTRLC) {
            return;
        }
    }
    if (base_addr_set && !length_set) {
      diag_printf("Length required for non-standard base address\n");
      return;
    }
    ip = (unsigned long *)&&lab1;
    _prg = (unsigned long *)0x1F00;  // Should be a very safe location to execute from
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
    // Now call this code
    fun = (code_fun *)((entry & 0x0FFFFFFF) + EBSA285_RAM_BANK0_BASE);  // Absolute address
    prg = (code_fun *)((unsigned long)ip + EBSA285_RAM_BANK0_BASE);  // Absolute address
    asm volatile ("mov r5,%0;"
		  "mov r2,%1;"
		  "mov r3,%2;"
		  "mov r1,%3;"
		  "mov pc,r1" : : 
		  "r"(fun),"r"(base_addr),"r"(length),"r"(prg) : "r5","r2","r3","r1");
    //    asm volatile ("mov r2,%0" : : "r"(base_addr) : "r2");
    //    asm volatile ("mov r3,%0" : : "r"(length) : "r3");
    //    asm volatile ("mov r1,%0; mov pc,r1" : : "r"(prg) : "r1");
 lab1:
    // Tricky code.  We are currently running with the MMU on and the
    // memory map convoluted from 1-1.  This code must be copied to RAM
    // and then executed at the non-mapped address.  Then when the code
    // turns off the MMU, the control flow is unchanged and control can
    // be passed safely to the program to be executed.
    // This magic was created in order to be able to execute standard
    // Linux kernels with as little change/perturberance as possible.
    //		  ".long 0xE7FFDEFE;"  // Illegal
    asm volatile ("mrs r1,cpsr;"
                  "bic r0,r1,#0x1F;"  // Put processor in IRQ mode
                  "orr r0,r0,#0x12;"
                  "msr cpsr,r0;"
                  "msr spsr,r1;"
                  "mov lr,r5;"
                  "mov sp,r5;"        // Give the kernel a stack just below the entry point
                  "mov r1,#0;"
                  "mcr p15,0,r1,c1,c0;" // Turns off MMU, caches
		  "mov r1,#0x8000;"   // Default kernel load address
                  "cmp r2,r1;"        // Is it there?
                  "beq 10f;"
		  "ldr r4,=0x40012000;mov r5,#0;str r5,[r4];"
		  "05:;"
		  "ldr r4,[r2],#4;"
		  "str r4,[r1],#4;"
		  "sub r3,r3,#4;"
		  "cmp r3,#0;"
		  "bne 05b;"
		  "10:;"
                  "mov r0,#0;"        // Set up SA110 load magic
                  "mov r1,#0x4;" 
		  "ldr r4,=0x40012000;mov r5,#1;str r5,[r4];"
                  "movs pc,lr" : : );
 end1:
}
