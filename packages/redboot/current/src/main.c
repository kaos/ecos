//==========================================================================
//
//      main.c
//
//      RedBoot main routine
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

#define  DEFINE_VARS
#include <redboot.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>
#include CYGHWR_MEMORY_LAYOUT_H

#include <cyg/hal/hal_tables.h>
#ifdef CYGBLD_HAL_PLATFORM_STUB_H
#include CYGBLD_HAL_PLATFORM_STUB_H
#else
#include <cyg/hal/plf_stub.h>
#endif

// Builtin Self Test (BIST)
externC void bist(void);

// GDB interfaces
extern void breakpoint(void);

// CLI command processing (defined in this file)
RedBoot_cmd("version", 
            "Display RedBoot version information",
            "",
            do_version
    );
RedBoot_cmd("help", 
            "Help about help?", 
            "<topic>",
            do_help 
    );
RedBoot_cmd("go", 
            "Execute code at a location", 
            "[-w <timeout>] [entry]",
            do_go 
    );
RedBoot_cmd("dump", 
            "Display (hex dump) a range of memory", 
            "-b <location> [-l <length>]",
            do_dump 
    );
RedBoot_cmd("cache", 
            "Manage machine caches", 
            "[ON | OFF]",
            do_caches 
    );
#ifdef HAL_STUB_PLATFORM_RESET
RedBoot_cmd("reset", 
            "Reset the system", 
            "",
            do_reset 
    );
#endif

// Define table boundaries
CYG_HAL_TABLE_BEGIN( __RedBoot_INIT_TAB__, RedBoot_inits );
CYG_HAL_TABLE_END( __RedBoot_INIT_TAB_END__, RedBoot_inits );
extern struct init_tab_entry __RedBoot_INIT_TAB__[], __RedBoot_INIT_TAB_END__;
CYG_HAL_TABLE_BEGIN( __RedBoot_CMD_TAB__, RedBoot_commands );
CYG_HAL_TABLE_END( __RedBoot_CMD_TAB_END__, RedBoot_commands );
extern struct cmd __RedBoot_CMD_TAB__[], __RedBoot_CMD_TAB_END__;

#ifdef HAL_ARCH_PROGRAM_NEW_STACK
extern void HAL_ARCH_PROGRAM_NEW_STACK(void *fun);
#endif

void
do_version(int argc, char *argv[])
{
    extern char RedBoot_version[];
    printf(RedBoot_version);
#ifdef HAL_PLATFORM_CPU
    printf("Platform: %s (%s) %s\n", HAL_PLATFORM_BOARD, HAL_PLATFORM_CPU, HAL_PLATFORM_EXTRA);
#endif
    printf("Copyright (C) 2000, 2001, Red Hat, Inc.\n\n");
    printf("RAM: %p-%p\n", (void*)ram_start, (void*)ram_end);
}

//
// This is the main entry point for RedBoot
//
void
cyg_start(void)
{
    int res = 0;
    bool prompt = true;
    static char line[CYGPKG_REDBOOT_MAX_CMD_LINE];
    struct cmd *cmd;
    int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
    struct init_tab_entry *init_entry;

    CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL);
#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
    console_selected = false;
#endif
    console_echo = true;
    CYGACC_CALL_IF_DELAY_US(2*100000);

    ram_start = (unsigned char *)CYGMEM_REGION_ram;
    ram_end = (unsigned char *)(CYGMEM_REGION_ram+CYGMEM_REGION_ram_SIZE);
#ifdef HAL_MEM_REAL_REGION_TOP
    {
        unsigned char *ram_end_tmp = ram_end;
        ram_end = HAL_MEM_REAL_REGION_TOP( ram_end_tmp );
    }
#endif

    bist();

    do_version(0,0);

    config_ok = false;

    for (init_entry = __RedBoot_INIT_TAB__; init_entry != &__RedBoot_INIT_TAB_END__;  init_entry++) {
        (*init_entry->fun)();
    }

#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
    // Give the guy a chance to abort any boot script
    if (script) {
        unsigned char *hold_script = script;
        int script_timeout_ms = script_timeout * CYGNUM_REDBOOT_FLASH_SCRIPT_TIMEOUT_RESOLUTION;
        printf("== Executing boot script in %d.%03d seconds - enter ^C to abort\n", 
               script_timeout_ms/1000, script_timeout_ms%1000);
        script = (unsigned char *)0;
        while (script_timeout_ms >= 10) {
            res = gets(line, sizeof(line), 10);
            if (res == _GETS_OK) {
                printf("== Executing boot script in %d.%03d seconds - enter ^C to abort\n", 
                       script_timeout_ms/1000, script_timeout_ms%1000);
                continue;  // Ignore anything but ^C
            }
            if (res != _GETS_TIMEOUT) break;
#ifdef CYGPKG_REDBOOT_NETWORKING
            if (have_net) {
                // Check for incoming TCP debug connection
                net_io_test();
            }
#endif
            script_timeout_ms -= 10;
        }
        if (res == _GETS_CTRLC) {
            script = (unsigned char *)0;  // Disable script
        } else {
            script = hold_script;  // Re-enable script
        }
    }
#endif

    while (true) {
        if (prompt) {
            printf("RedBoot> ");
            prompt = false;
        }
        res = gets(line, sizeof(line), 250);
        if (res == _GETS_TIMEOUT) {
            // No input arrived
#ifdef CYGPKG_REDBOOT_NETWORKING
            if (have_net) {
                // Check for incoming TCP debug connection
                net_io_test();
            }
#endif
        } else {
            if (res == _GETS_GDB) {
                // Special case of '$' - need to start GDB protocol
                CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);
#ifdef HAL_ARCH_PROGRAM_NEW_STACK
                HAL_ARCH_PROGRAM_NEW_STACK(breakpoint);
#else
                breakpoint();  // Get GDB stubs started, with a proper environment, etc.
#endif
            } else {
                if (strlen(line) > 0) {
                    if ((cmd = parse(line, &argc, &argv[0])) != (struct cmd *)0) {
                        (cmd->fun)(argc, argv);
                    } else {
                        printf("** Error: Illegal command: \"%s\"\n", argv[0]);
                    }
                }
                prompt = true;
            }
        }
    }
}

void
do_caches(int argc, char *argv[])
{
    unsigned long oldints;
    if (argc == 2) {
        if (strcmpci(argv[1], "on") == 0) {
            HAL_DISABLE_INTERRUPTS(oldints);
            HAL_ICACHE_ENABLE();
            HAL_DCACHE_ENABLE();
            HAL_RESTORE_INTERRUPTS(oldints);
        } else if (strcmpci(argv[1], "off") == 0) {
            HAL_DISABLE_INTERRUPTS(oldints);
            HAL_DCACHE_SYNC();
            HAL_ICACHE_DISABLE();
            HAL_DCACHE_DISABLE();
            HAL_DCACHE_SYNC();
            HAL_ICACHE_INVALIDATE_ALL();
            HAL_DCACHE_INVALIDATE_ALL();
            HAL_RESTORE_INTERRUPTS(oldints);
        } else {
            printf("Invalid cache mode: %s\n", argv[1]);
        }
    } else {
        printf("caches ?\n");
    }
}


void
do_help(int argc, char *argv[])
{
    struct cmd *cmd;

    cmd = __RedBoot_CMD_TAB__;
    while (cmd != &__RedBoot_CMD_TAB_END__) {
        printf("%s\n   %s %s\n", cmd->help, cmd->str, cmd->usage);
        cmd++;
    }
    return;
}

void
do_dump(int argc, char *argv[])
{
    struct option_info opts[2];
    unsigned long base, len;
    bool base_set, len_set;

    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void **)&base, (bool *)&base_set, "base address");
    init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&len, (bool *)&len_set, "length");
    if (!scan_opts(argc, argv, 1, opts, 2, 0, 0, ""))
    {
        return;
    }
    if (!base_set) {
        printf("Dump what [location]?\n");
        return;
    }
    if (!len_set) {
        len = 32;
    }
    dump_buf((void *)base, len);
}

void
do_go(int argc, char *argv[])
{
    typedef void code_fun(void);
    unsigned long entry;
    unsigned long oldints;
    code_fun *fun;
    bool wait_time_set;
    int  wait_time, res;
    struct option_info opts[1];
    char line[8];

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
    fun = (code_fun *)entry;
    HAL_DISABLE_INTERRUPTS(oldints);
    HAL_DCACHE_SYNC();
    HAL_ICACHE_DISABLE();
    HAL_DCACHE_DISABLE();
    HAL_DCACHE_SYNC();
    HAL_ICACHE_INVALIDATE_ALL();
    HAL_DCACHE_INVALIDATE_ALL();
#ifdef HAL_ARCH_PROGRAM_NEW_STACK
    HAL_ARCH_PROGRAM_NEW_STACK(fun);
#else
    (*fun)();
#endif
}

#ifdef HAL_STUB_PLATFORM_RESET
void
do_reset(int argc, char *argv[])
{
    printf("... Resetting.");
    CYGACC_CALL_IF_DELAY_US(2*100000);
    printf("\n");
    CYGACC_CALL_IF_DELAY_US(50000);
    HAL_STUB_PLATFORM_RESET();
    printf("!! oops, RESET not working on this platform\n");
}
#endif

// 
// [Null] Builtin [Power On] Self Test
//
void bist(void) CYGBLD_ATTRIB_WEAK;

void
bist(void) 
{
}
