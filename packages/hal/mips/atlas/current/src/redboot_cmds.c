//==========================================================================
//
//      redboot_cmds.c
//
//      Atlas [platform] specific RedBoot commands
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
// Contributors: gthomas, jskov
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

#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>

// Exported CLI function(s)

// The exec command works like the go command in YAMON - that is,
// providing callee with argc, argv, and env. In addition to the
// information provided by the user, 

static void do_exec(int argc, char *argv[]);

static char exec_usage[] =
#if 0
            "[-r <ramdisk addr> [-s <ramdisk length>]] \n"
#endif
            "[-w timeout] [-c \"kernel command line\"] [<entry_point>]";

RedBoot_cmd("exec", 
            "Execute an image - with MMU off", 
            exec_usage,
            do_exec
    );

typedef void code_fun(void);

#define MAX_ARGS_LEN 128
#define _ARGC 2

char _cmd_str[] = "exec";
char _arg_str[MAX_ARGS_LEN];
char* _exec_argv[_ARGC] = { _cmd_str, _arg_str };

/* Environment variable */
typedef struct
{
	char *name;
	char *val;
} t_env_var;

char _env_memsize[11];
char _env_ttymode[32];
char _env_ethaddr[18];
t_env_var _exec_envp[] = {
    { "memsize", _env_memsize },
    { "modetty0", _env_ttymode },
    { "ethaddr", _env_ethaddr },
    { NULL, NULL }
};

static void 
do_exec(int argc, char *argv[])
{
    unsigned long entry;
    unsigned long oldints;
    bool wait_time_set;
    int  wait_time, res;
    bool cmd_line_set;
#if 0
    bool ramdisk_addr_set, ramdisk_size_set;
    unsigned long ramdisk_addr, ramdisk_size;
#endif
    struct option_info opts[4];
    char line[8];
    char *cmd_line;
    int num_options;

    entry = (unsigned long)entry_address;  // Default from last 'load' operation
    init_opts(&opts[0], 'w', true, OPTION_ARG_TYPE_NUM, 
              (void **)&wait_time, (bool *)&wait_time_set, "wait timeout");
    init_opts(&opts[1], 'c', true, OPTION_ARG_TYPE_STR, 
              (void **)&cmd_line, (bool *)&cmd_line_set, "kernel command line");
    num_options = 2;
#if 0
    init_opts(&opts[2], 'r', true, OPTION_ARG_TYPE_NUM, 
              (void **)&ramdisk_addr, (bool *)&ramdisk_addr_set, "ramdisk_addr");
    init_opts(&opts[3], 's', true, OPTION_ARG_TYPE_NUM, 
              (void **)&ramdisk_size, (bool *)&ramdisk_size_set, "ramdisk_size");
    num_options += 2;
#endif

    if (!scan_opts(argc, argv, 1, opts, num_options, (void *)&entry, 
                   OPTION_ARG_TYPE_NUM, "starting address"))
    {
        return;
    }
    if (cmd_line_set) {
        strncpy(_arg_str, cmd_line, MAX_ARGS_LEN);
    }
#if 0
    if (ramdisk_addr_set) {
        params->rd_start = ramdisk_addr;
        if (ramdisk_size_set) {
            params->rd_size = ramdisk_size;
        } else {
            params->rd_size = 4096*1024;
        }
    }
#endif

    // Compute memory size
    sprintf(_env_memsize, "0x%08x", (ram_end - ram_start + (1024*1024-1)) & ~(1024*1024-1));
    // Set enet address
    sprintf(_env_ethaddr, "%02x.%02x.%02x.%02x.%02x.%02x",
            __local_enet_addr[0], __local_enet_addr[1],
            __local_enet_addr[2], __local_enet_addr[3],
            __local_enet_addr[4], __local_enet_addr[5]);
    // Set console parameters (only baud rate part configurable for now)
    sprintf(_env_ttymode, "%d,n,8,1,hw", CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL_BAUD);

    if (wait_time_set) {
        printf("About to start execution at %p - abort with ^C within %d seconds\n",
               (void *)entry, wait_time);
        res = gets(line, sizeof(line), wait_time*1000);
        if (res == _GETS_CTRLC) {
            return;
        }
    }

    HAL_DISABLE_INTERRUPTS(oldints);
    HAL_DCACHE_SYNC();
    HAL_ICACHE_DISABLE();
    HAL_DCACHE_DISABLE();
    HAL_DCACHE_SYNC();
    HAL_ICACHE_INVALIDATE_ALL();
    HAL_DCACHE_INVALIDATE_ALL();
    asm volatile ("move $4,%1;"         // a0
		  "move $5,%2;"         // a1
		  "move $6,%3;"         // a2
		  "j    %0;"
		  : : "r"(entry),"r"(_ARGC),"r"(_exec_argv),"r"(_exec_envp) : "$4","$5","$6");
}

// EOF redboot_cmds.c
