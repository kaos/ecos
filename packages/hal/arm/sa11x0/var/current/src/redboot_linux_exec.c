//==========================================================================
//
//      redboot_linux_boot.c
//
//      RedBoot command to boot Linux on StrongARM/SA11x0
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
//####OTHERCOPYRIGHTBEGIN####
//
//  The structure definitions below are taken from include/asm-arm/setup.h in
//  the Linux kernel, Copyright (C) 1997-1999 Russell King. Their presence
//  here is for the express purpose of communication with the Linux kernel
//  being booted and is considered 'fair use' by the original author and
//  are included with his permission.
//
//####OTHERCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
//               Russell King <rmk@arm.linux.org.uk>
// Date:         2001-02-20
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
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>

#ifdef HAL_PLATFORM_MACHINE_TYPE

// Exported CLI function(s)
static void do_exec(int argc, char *argv[]);
RedBoot_cmd("exec", 
            "Execute an image - with MMU off", 
            "[-w timeout] [-b <load addr> [-l <length>]]\n"
            "        [-r <ramdisk addr> [-s <ramdisk length>]]\n"
            "        [-c \"kernel command line\"] [<entry_point>]",
            do_exec
    );

typedef void code_fun(void);
#define UNMAPPED_ADDR(x) (((unsigned long)x & 0x0FFFFFFF) + SA11X0_RAM_BANK0_BASE)

//
// Parameter info for Linux kernel
//   ** C A U T I O N **  This setup must match "asm-arm/setup.h"
//
// Info is passed at a fixed location, using a sequence of tagged
// data entries.
//

typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;

//=========================================================================
//  From Linux <asm-arm/setup.h>

#define ATAG_NONE	0x00000000
struct tag_header {
    u32 size;    // Size of tag (hdr+data) in *longwords*
    u32 tag;
};

#define ATAG_CORE	0x54410001
struct tag_core {
    u32 flags;		/* bit 0 = read-only */
    u32 pagesize;
    u32 rootdev;
};

#define ATAG_MEM		0x54410002
struct tag_mem32 {
    u32	size;
    u32	start;
};

#define ATAG_VIDEOTEXT	0x54410003
struct tag_videotext {
    u8	 x;
    u8	 y;
    u16	 video_page;
    u8	 video_mode;
    u8	 video_cols;
    u16	 video_ega_bx;
    u8	 video_lines;
    u8	 video_isvga;
    u16	 video_points;
};

#define ATAG_RAMDISK	0x54410004
struct tag_ramdisk {
    u32 flags;		/* b0 = load, b1 = prompt */
    u32 size;
    u32 start;
};

#define ATAG_INITRD	0x54410005
struct tag_initrd {
    u32 start;
    u32 size;
};

#define ATAG_SERIAL	0x54410006
struct tag_serialnr {
    u32 low;
    u32 high;
};

#define ATAG_REVISION	0x54410007
struct tag_revision {
    u32 rev;
};

#define ATAG_VIDEOLFB	0x54410008
struct tag_videolfb {
    u16	lfb_width;
    u16	lfb_height;
    u16	lfb_depth;
    u16	lfb_linelength;
    u32	lfb_base;
    u32	lfb_size;
    u8	red_size;
    u8	red_pos;
    u8	green_size;
    u8	green_pos;
    u8	blue_size;
    u8	blue_pos;
    u8	rsvd_size;
    u8	rsvd_pos;
};

#define ATAG_CMDLINE	0x54410009
struct tag_cmdline {
    char cmdline[1];
};

#define ATAG_ACORN	0x41000101
struct tag_acorn {
    u32 memc_control_reg;
    u32 vram_pages;
    u8 sounddefault;
    u8 adfsdrives;
};

#define ATAG_MEMCLK	0x41000402
struct tag_memclk {
    u32 fmemclk;
};

struct tag {
    struct tag_header hdr;
    union {
        struct tag_core		core;
        struct tag_mem32	mem;
        struct tag_videotext	videotext;
        struct tag_ramdisk	ramdisk;
        struct tag_initrd	initrd;
        struct tag_serialnr	serialnr;
        struct tag_revision	revision;
        struct tag_videolfb	videolfb;
        struct tag_cmdline	cmdline;

        /*
         * Acorn specific
         */
        struct tag_acorn	acorn;

        /*
         * DC21285 specific
         */
        struct tag_memclk	memclk;
    } u;
};

// End of inclusion from <asm-arm/setup.h>
//=========================================================================

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
    struct tag *params = (struct tag *)0x0100;

    entry = (unsigned long)UNMAPPED_ADDR(0xC0008000);  // Default Linux execute address
    base_addr = 0x8000;
    ramdisk_size = 4096*1024;
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
    if (!scan_opts(argc, argv, 1, opts, 6, (void *)&entry, OPTION_ARG_TYPE_NUM, "[physical] starting address"))
    {
        return;
    }

    // Set up parameters to pass to kernel
    if (cmd_line_set || ramdisk_addr_set) {
        // CORE tag must be present & first
        params->hdr.size = (sizeof(struct tag_core) + sizeof(struct tag_header))/sizeof(long);
        params->hdr.tag = ATAG_CORE;
        params->u.core.flags = 0;
        params->u.core.pagesize = 0;
        params->u.core.rootdev = 0;
        params = (struct tag *)((long *)params + params->hdr.size);
    }
    if (ramdisk_addr_set) {
        params->hdr.size = (sizeof(struct tag_initrd) + sizeof(struct tag_header))/sizeof(long);
        params->hdr.tag = ATAG_INITRD;
        params->u.initrd.start = UNMAPPED_ADDR(ramdisk_addr);
        params->u.initrd.size = ramdisk_size;
        params = (struct tag *)((long *)params + params->hdr.size);
    }
    if (cmd_line_set) {
// Round up a quantity to a longword (32 bit) length
#define ROUNDUP(n) (((n)+3)&~3)
        params->hdr.size = (ROUNDUP(strlen(cmd_line)) + sizeof(struct tag_header))/sizeof(long);
        params->hdr.tag = ATAG_CMDLINE;
        strcpy(params->u.cmdline.cmdline, cmd_line);
        params = (struct tag *)((long *)params + params->hdr.size);
    }
    // Mark end of parameter list
    params->hdr.size = 0;
    params->hdr.tag = ATAG_NONE;

    if (wait_time_set) {
        int script_timeout_ms = wait_time * 1000;
#ifdef CYGFUN_REDBOOT_BOOT_SCRIPT
        unsigned char *hold_script = script;
        script = (unsigned char *)0;
#endif
        diag_printf("About to start execution at %p - abort with ^C within %d seconds\n",
                    (void *)entry, wait_time);
        while (script_timeout_ms >= CYGNUM_REDBOOT_CLI_IDLE_TIMEOUT) {
            res = _rb_gets(line, sizeof(line), CYGNUM_REDBOOT_CLI_IDLE_TIMEOUT);
            if (res == _GETS_CTRLC) {
#ifdef CYGFUN_REDBOOT_BOOT_SCRIPT
                script = hold_script;  // Re-enable script
#endif
                return;
            }
            script_timeout_ms -= CYGNUM_REDBOOT_CLI_IDLE_TIMEOUT;
        }
    }
    if (base_addr_set && !length_set) {
      diag_printf("Length required for non-standard base address\n");
      return;
    }
    ip = (unsigned long *)&&lab1;
    _prg = (unsigned long *)0x1F00;  // Should be a very safe location to execute from
    fun = (code_fun *)entry;
    prg = (code_fun *)UNMAPPED_ADDR(_prg);
    base_addr = UNMAPPED_ADDR(base_addr);
    for (i = 0;  i < (unsigned long)&&end1-(unsigned long)&&lab1;  i++) {
        *_prg++ = *ip++;
    }
    HAL_DISABLE_INTERRUPTS(oldints);
    HAL_DCACHE_SYNC();
    HAL_ICACHE_DISABLE();
    HAL_DCACHE_DISABLE();
    HAL_DCACHE_SYNC();
    HAL_ICACHE_INVALIDATE_ALL();
    HAL_DCACHE_INVALIDATE_ALL();
    // Now call this code
    asm volatile ("mov r5,%0;"
		  "mov r2,%1;"
		  "mov r3,%2;"
		  "mov r1,%3;"
                  "mov r7,%4;"
		  "ldr r6,=0xC0008000;"   // Unmapped address Linux wants to load at
		  "mov pc,r1" : : 
		  "r"(fun),"r"(base_addr),"r"(length),"r"(prg),"r"(HAL_PLATFORM_MACHINE_TYPE) : 
                  "r7","r6","r5","r2","r3","r1");
 lab1:
    // Tricky code.  We are currently running with the MMU on and the
    // memory map convoluted from 1-1.  This code must be copied to RAM
    // and then executed at the non-mapped address.  Then when the code
    // turns off the MMU, the control flow is unchanged and control can
    // be passed safely to the program to be executed.
    // This magic was created in order to be able to execute standard
    // Linux kernels with as little change/perturberance as possible.
    asm volatile ("mrs r1,cpsr;"
                  "bic r0,r1,#0x1F;"  // Put processor in IRQ mode
                  "orr r0,r0,#0x12;"
                  "msr cpsr,r0;"
                  "msr spsr,r1;"
                  "mov lr,r5;"
                  "mov sp,r5;"        // Give the kernel a stack just below the entry point
                  "mov r1,#0;"
                  "mcr p15,0,r1,c1,c0;" // Turns off MMU, caches
                  "nop;nop;nop;"
                  "cmp r2,r6;"        // Default kernel load address
                  "beq 10f;"
		  "05:;"
		  "ldr r4,[r2],#4;"
		  "str r4,[r6],#4;"
		  "sub r3,r3,#4;"
		  "cmp r3,#0;"
		  "bne 05b;"
		  "10:;"
                  "mov r0,#0;"        // Set up SA11x0[Assabet] load magic
                  "mov r1,r7;" 
                  "movs pc,lr" : : );
 end1:
}

#endif // HAL_PLATFORM_MACHINE_TYPE - otherwise we do not support this stuff...

// EOF redboot_linux_exec.c
