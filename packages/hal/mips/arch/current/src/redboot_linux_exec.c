//==========================================================================
//
//      redboot_linux_exec.c
//
//      RedBoot exec command for Linux booting
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
// Author(s):    t@keshi.org
// Contributors: t@keshi.org, jskov, dwmw2
// Date:         2001-07-09
// Purpose:      RedBoot exec command for Linux booting
//
//####DESCRIPTIONEND####
//
//===========================================================================

#include <redboot.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>

#define xstr(s) str(s)
#define str(s...) #s

typedef struct
{
	char *name;
	char *val;
} t_env_var;

struct parmblock {
	t_env_var memsize;
	t_env_var modetty0;
	t_env_var ethaddr;
	t_env_var env_end;
	char *argv[2];
	char text[0];
};

static void do_exec(int argc, char *argv[]);
RedBoot_cmd("exec", 
            "Execute an image", 
            "[-b <argv addr>] [-c \"kernel command line\"] [-w <timeout>]\n"
	    "        [<entry point>]",
            do_exec
    );

static void 
do_exec(int argc, char *argv[])
{
    cyg_uint32 entry = (cyg_uint32)entry_address?:CYGDAT_REDBOOT_MIPS_LINUX_BOOT_ENTRY;
    cyg_uint32 base_addr = CYGDAT_REDBOOT_MIPS_LINUX_BOOT_ARGV_ADDR;
    char *cmd_line = xstr(CYGDAT_REDBOOT_MIPS_LINUX_BOOT_COMMAND_LINE);
    bool base_addr_set, cmd_line_set, wait_time_set;
    int wait_time, res;
    char line[8];
    
    struct option_info opts[3];
    char *pcmd;
    struct parmblock *pb;
    void (*linux)(int, char **, void *);
    int oldints;

    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void **)&base_addr, &base_addr_set, "base address");
    init_opts(&opts[1], 'w', true, OPTION_ARG_TYPE_NUM, 
              (void **)&wait_time, (bool *)&wait_time_set, "wait timeout");
    init_opts(&opts[2], 'c', true, OPTION_ARG_TYPE_STR, 
              (void **)&cmd_line, &cmd_line_set, "kernel command line");
    
    if (!scan_opts(argc, argv, 1, opts, 3, (void *)&entry, 
                   OPTION_ARG_TYPE_NUM, "entry address"))
        return;

    linux = (void *)entry;

    printf("Now booting linux kernel:\n");
    printf(" Base address 0x%08x Entry 0x%08x\n", base_addr, entry);
    printf(" Cmdline : %s\n", cmd_line);

    if (wait_time_set) {
        printf("About to start execution at %p - abort with ^C within %d seconds\n",
               (void *)entry, wait_time);
        res = gets(line, sizeof(line), wait_time*1000);
        if (res == _GETS_CTRLC) {
            return;
        }
    }
    
    HAL_DISABLE_INTERRUPTS(oldints);

    pb = (struct parmblock *)base_addr;
    pcmd = pb->text;

    pb->memsize.name = pcmd;
    pcmd += sprintf(pcmd, "memsize");
    pb->memsize.val = ++pcmd;
    pcmd += sprintf(pcmd, "0x%08x", (ram_end - ram_start + 0xFFFFF) & ~0xFFFFF);

    pb->modetty0.name = ++pcmd;
    pcmd += sprintf(pcmd, "modetty0");
    pb->modetty0.val = ++pcmd;
    pcmd += sprintf(pcmd, "%d,n,8,1,hw", CYGNUM_HAL_VIRTUAL_VECTOR_CHANNELS_DEFAULT_BAUD);

#ifdef CYGPKG_REDBOOT_NETWORKING
    pb->ethaddr.name = ++pcmd;
    pcmd += sprintf(pcmd, "ethaddr");
    pb->ethaddr.val = ++pcmd;
    pcmd += sprintf(pcmd, "%02x.%02x.%02x.%02x.%02x.%02x",
            __local_enet_addr[0], __local_enet_addr[1],
            __local_enet_addr[2], __local_enet_addr[3],
            __local_enet_addr[4], __local_enet_addr[5]);
    pb->env_end.name = NULL;
    pb->env_end.val = NULL;
#else
    pb->ethaddr.name = NULL;
    pb->ethaddr.val = NULL;
#endif

    /* Point argv[0] at a handy `\0` */
    pb->argv[0] = pcmd;
    pb->argv[1] = ++pcmd;
    
    strcpy(pcmd, cmd_line);
    
    HAL_DCACHE_SYNC();
    HAL_ICACHE_DISABLE();
    HAL_DCACHE_DISABLE();
    HAL_DCACHE_SYNC();
    HAL_ICACHE_INVALIDATE_ALL();
    HAL_DCACHE_INVALIDATE_ALL();

    linux(2, pb->argv, pb);
}
