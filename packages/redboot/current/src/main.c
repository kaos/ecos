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
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_cache.h>
#include CYGHWR_MEMORY_LAYOUT_H

#include <cyg/hal/hal_tables.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
#ifdef CYGBLD_HAL_PLATFORM_STUB_H
#include CYGBLD_HAL_PLATFORM_STUB_H
#else
#include <cyg/hal/plf_stub.h>
#endif
#endif

// Builtin Self Test (BIST)
externC void bist(void);

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
// GDB interfaces
extern void breakpoint(void);
#endif

// CLI command processing (defined in this file)
RedBoot_cmd("version", 
            "Display RedBoot version information",
            "",
            do_version
    );
RedBoot_cmd("help", 
            "Help about help?", 
            "[<topic>]",
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
RedBoot_cmd("cksum", 
            "Compute a 32bit checksum [POSIX algorithm] for a range of memory", 
            "-b <location> -l <length>",
            do_cksum
    );
RedBoot_cmd("cache", 
            "Manage machine caches", 
            "[ON | OFF]",
            do_caches 
    );
#ifdef HAL_PLATFORM_RESET
RedBoot_cmd("reset", 
            "Reset the system", 
            "",
            do_reset 
    );
#endif
#ifdef CYGSEM_REDBOOT_VARIABLE_BAUD_RATE
RedBoot_cmd("baudrate", 
            "Set/Query the system console baud rate", 
            "[-b <rate>]",
            do_baud_rate
    );
#endif

// Define table boundaries
CYG_HAL_TABLE_BEGIN( __RedBoot_INIT_TAB__, RedBoot_inits );
CYG_HAL_TABLE_END( __RedBoot_INIT_TAB_END__, RedBoot_inits );
extern struct init_tab_entry __RedBoot_INIT_TAB__[], __RedBoot_INIT_TAB_END__;

CYG_HAL_TABLE_BEGIN( __RedBoot_CMD_TAB__, RedBoot_commands );
CYG_HAL_TABLE_END( __RedBoot_CMD_TAB_END__, RedBoot_commands );
extern struct cmd __RedBoot_CMD_TAB__[], __RedBoot_CMD_TAB_END__;

CYG_HAL_TABLE_BEGIN( __RedBoot_IDLE_TAB__, RedBoot_idle );
CYG_HAL_TABLE_END( __RedBoot_IDLE_TAB_END__, RedBoot_idle );
extern struct idle_tab_entry __RedBoot_IDLE_TAB__[], __RedBoot_IDLE_TAB_END__;

#ifdef HAL_ARCH_PROGRAM_NEW_STACK
extern void HAL_ARCH_PROGRAM_NEW_STACK(void *fun);
#endif

#ifdef CYGSEM_REDBOOT_FLASH_ALIASES
externC void expand_aliases(char *line, int len);
#endif

void
do_version(int argc, char *argv[])
{
    extern char RedBoot_version[];
#ifdef CYGPKG_IO_FLASH
    externC void _flash_info(void);
#endif

    diag_printf(RedBoot_version);
#ifdef HAL_PLATFORM_CPU
    diag_printf("Platform: %s (%s) %s\n", HAL_PLATFORM_BOARD, HAL_PLATFORM_CPU, HAL_PLATFORM_EXTRA);
#endif
    diag_printf("Copyright (C) 2000, 2001, Red Hat, Inc.\n\n");
    diag_printf("RAM: %p-%p, %p-%p available\n", 
                (void*)ram_start, (void*)ram_end,
                (void*)user_ram_start, (void *)user_ram_end);
#ifdef CYGPKG_IO_FLASH
    _flash_info();
#endif
}

void
do_idle(bool is_idle)
{
    struct idle_tab_entry *idle_entry;

    for (idle_entry = __RedBoot_IDLE_TAB__; 
         idle_entry != &__RedBoot_IDLE_TAB_END__;  idle_entry++) {
        (*idle_entry->fun)(is_idle);
    }
}

// Wrapper used by diag_printf()
static void
_mon_write_char(char c, void **param)
{
    mon_write_char(c);
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
    int cur;
    struct init_tab_entry *init_entry;

    // Make sure the channels are properly initialized.
    diag_init_putc(_mon_write_char);
    hal_if_diag_init();

    // Force console to output raw text - but remember the old setting
    // so it can be restored if interaction with a debugger is
    // required.
    cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
    CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL);
#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
    console_selected = false;
#endif
    console_echo = true;
    CYGACC_CALL_IF_DELAY_US((cyg_int32)2*100000);

    ram_start = (unsigned char *)CYGMEM_REGION_ram;
    ram_end = (unsigned char *)(CYGMEM_REGION_ram+CYGMEM_REGION_ram_SIZE);
#ifdef HAL_MEM_REAL_REGION_TOP
    {
        unsigned char *ram_end_tmp = ram_end;
        ram_end = HAL_MEM_REAL_REGION_TOP( ram_end_tmp );
    }
#endif
#ifdef CYGMEM_SECTION_heap1
    workspace_start = (unsigned char *)CYGMEM_SECTION_heap1;
    workspace_end = (unsigned char *)(CYGMEM_SECTION_heap1+CYGMEM_SECTION_heap1_SIZE);
    workspace_size = CYGMEM_SECTION_heap1_SIZE;
#else
    workspace_start = (unsigned char *)CYGMEM_REGION_ram;
    workspace_end = (unsigned char *)(CYGMEM_REGION_ram+CYGMEM_REGION_ram_SIZE);
    workspace_size = CYGMEM_REGION_ram_SIZE;
#endif

    if ( ram_end < workspace_end ) {
        // when *less* SDRAM is installed than the possible maximum,
        // but the heap1 region remains greater...
        workspace_end = ram_end;
        workspace_size = workspace_end - workspace_start;
    }

    bist();

    for (init_entry = __RedBoot_INIT_TAB__; init_entry != &__RedBoot_INIT_TAB_END__;  init_entry++) {
        (*init_entry->fun)();
    }

#ifdef CYGPKG_COMPRESS_ZLIB
#define ZLIB_COMPRESSION_OVERHEAD 0xB800
#else
#define ZLIB_COMPRESSION_OVERHEAD 0
#endif
    user_ram_start = workspace_start + ZLIB_COMPRESSION_OVERHEAD;
    user_ram_end = workspace_end;
    do_version(0,0);

#ifdef CYGFUN_REDBOOT_BOOT_SCRIPT
# ifdef CYGDAT_REDBOOT_DEFAULT_BOOT_SCRIPT
    if (!script) {
      script = CYGDAT_REDBOOT_DEFAULT_BOOT_SCRIPT;
#  ifndef CYGSEM_REDBOOT_FLASH_CONFIG
      script_timeout = CYGNUM_REDBOOT_BOOT_SCRIPT_DEFAULT_TIMEOUT;
#  endif
    }
# endif
    if (script) {
        // Give the guy a chance to abort any boot script
        unsigned char *hold_script = script;
        int script_timeout_ms = script_timeout * CYGNUM_REDBOOT_BOOT_SCRIPT_TIMEOUT_RESOLUTION;
        diag_printf("== Executing boot script in %d.%03d seconds - enter ^C to abort\n", 
                    script_timeout_ms/1000, script_timeout_ms%1000);
        script = (unsigned char *)0;
        res = _GETS_CTRLC;  // Treat 0 timeout as ^C
        while (script_timeout_ms >= CYGNUM_REDBOOT_CLI_IDLE_TIMEOUT) {
            res = _rb_gets(line, sizeof(line), CYGNUM_REDBOOT_CLI_IDLE_TIMEOUT);
            if (res >= _GETS_OK) {
                diag_printf("== Executing boot script in %d.%03d seconds - enter ^C to abort\n", 
                            script_timeout_ms/1000, script_timeout_ms%1000);
                continue;  // Ignore anything but ^C
            }
            if (res != _GETS_TIMEOUT) break;
            script_timeout_ms -= CYGNUM_REDBOOT_CLI_IDLE_TIMEOUT;
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
            diag_printf("RedBoot> ");
            prompt = false;
        }
        res = _rb_gets(line, sizeof(line), CYGNUM_REDBOOT_CLI_IDLE_TIMEOUT);
        if (res == _GETS_TIMEOUT) {
            // No input arrived
        } else {
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
            if (res == _GETS_GDB) {
		int dbgchan;
                hal_virtual_comm_table_t *__chan;
                int i;
                // Special case of '$' - need to start GDB protocol
                gdb_active = true;
                // Mask interrupts on all channels
                for (i = 0;  i < CYGNUM_HAL_VIRTUAL_VECTOR_NUM_CHANNELS;  i++) {
                    CYGACC_CALL_IF_SET_CONSOLE_COMM(i);
                    __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
                    CYGACC_COMM_IF_CONTROL( *__chan, __COMMCTL_IRQ_DISABLE );
                }
    
                CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);
#ifdef HAL_ARCH_PROGRAM_NEW_STACK
                HAL_ARCH_PROGRAM_NEW_STACK(breakpoint);
#else
                breakpoint();  // Get GDB stubs started, with a proper environment, etc.
#endif
		dbgchan = CYGACC_CALL_IF_SET_DEBUG_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
		CYGACC_CALL_IF_SET_CONSOLE_COMM(dbgchan);
            } else 
#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
            {
#ifdef CYGSEM_REDBOOT_FLASH_ALIASES
                expand_aliases(line, sizeof(line));
#endif
                if (strlen(line) > 0) {                    
                    if ((cmd = parse(line, &argc, &argv[0])) != (struct cmd *)0) {
                        (cmd->fun)(argc, argv);
                    } else {
                        diag_printf("** Error: Illegal command: \"%s\"\n", argv[0]);
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
    int dcache_on=0, icache_on=0;

    if (argc == 2) {
        if (strcasecmp(argv[1], "on") == 0) {
            HAL_DISABLE_INTERRUPTS(oldints);
            HAL_ICACHE_ENABLE();
            HAL_DCACHE_ENABLE();
            HAL_RESTORE_INTERRUPTS(oldints);
        } else if (strcasecmp(argv[1], "off") == 0) {
            HAL_DISABLE_INTERRUPTS(oldints);
            HAL_DCACHE_SYNC();
            HAL_ICACHE_DISABLE();
            HAL_DCACHE_DISABLE();
            HAL_DCACHE_SYNC();
            HAL_ICACHE_INVALIDATE_ALL();
            HAL_DCACHE_INVALIDATE_ALL();
            HAL_RESTORE_INTERRUPTS(oldints);
        } else {
            diag_printf("Invalid cache mode: %s\n", argv[1]);
        }
    } else {
#ifdef HAL_DCACHE_IS_ENABLED
        HAL_DCACHE_IS_ENABLED(dcache_on);
#endif
#ifdef HAL_ICACHE_IS_ENABLED
        HAL_ICACHE_IS_ENABLED(icache_on);
#endif
        diag_printf("Data cache: %s, Instruction cache: %s\n", 
                    dcache_on?"On":"Off", icache_on?"On":"Off");
    }
}


void
do_help(int argc, char *argv[])
{
    struct cmd *cmd;
    char *which = (char *)0;
    bool show;
    int len = 0;

    if (!scan_opts(argc, argv, 1, 0, 0, (void **)&which, OPTION_ARG_TYPE_STR, "<topic>")) {
        diag_printf("Invalid argument\n");
        return;
    }
    if (which) {
        len = strlen(which);
    }
    cmd = __RedBoot_CMD_TAB__;
    while (cmd != &__RedBoot_CMD_TAB_END__) {
        show = true;
        if (which && (strncasecmp(which, cmd->str, len) != 0)) {
            show = false;
        }
        if (show) {
            diag_printf("%s\n   %s %s\n", cmd->help, cmd->str, cmd->usage);
        }
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
    static unsigned long _base, _len;

    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void **)&base, (bool *)&base_set, "base address");
    init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&len, (bool *)&len_set, "length");
    if (!scan_opts(argc, argv, 1, opts, 2, 0, 0, "")) {
        return;
    }
    if (!base_set) {
        if (_base == 0) {
            diag_printf("Dump what [location]?\n");
            return;
        }
        base = _base;
        if (!len_set) {
            len = _len;
            len_set = true;
        }
    }
    if (!len_set) {
        len = 32;
    }
    diag_dump_buf((void *)base, len);
    _base = base + len;
    _len = len;
}

void
do_cksum(int argc, char *argv[])
{
    // Compute a CRC, using the POSIX 1003 definition
    extern unsigned long 
        posix_crc32(unsigned char *s, int len);

    struct option_info opts[2];
    unsigned long base, len, crc;
    bool base_set, len_set;

    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void **)&base, (bool *)&base_set, "base address");
    init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&len, (bool *)&len_set, "length");
    if (!scan_opts(argc, argv, 1, opts, 2, 0, 0, "")) {
        return;
    }
    if (!base_set || !len_set) {
        diag_printf("usage: cksum -b <addr> -l <length>\n");
        return;
    }
    crc = posix_crc32((unsigned char *)base, len);
    diag_printf("POSIX cksum = 0x%08lx (%lu)\n", crc, crc);
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
        int script_timeout_ms = wait_time * 1000;
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
        unsigned char *hold_script = script;
        script = (unsigned char *)0;
#endif
        diag_printf("About to start execution at %p - abort with ^C within %d seconds\n",
                    (void *)entry, wait_time);
        while (script_timeout_ms >= CYGNUM_REDBOOT_CLI_IDLE_TIMEOUT) {
            res = _rb_gets(line, sizeof(line), CYGNUM_REDBOOT_CLI_IDLE_TIMEOUT);
            if (res == _GETS_CTRLC) {
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
                script = hold_script;  // Re-enable script
#endif
                return;
            }
            script_timeout_ms -= CYGNUM_REDBOOT_CLI_IDLE_TIMEOUT;
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

#ifdef HAL_PLATFORM_RESET
void
do_reset(int argc, char *argv[])
{
    diag_printf("... Resetting.");
    CYGACC_CALL_IF_DELAY_US(2*100000);
    diag_printf("\n");
    CYGACC_CALL_IF_RESET();
    diag_printf("!! oops, RESET not working on this platform\n");
}
#endif

#ifdef CYGSEM_REDBOOT_VARIABLE_BAUD_RATE
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
#include <flash_config.h>
#endif

void
set_console_baud_rate(int rate)
{
    hal_virtual_comm_table_t *__chan;
    int ret;
    static int current_rate = CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD;

    if (rate != current_rate) {
        __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
        ret = CYGACC_COMM_IF_CONTROL(*__chan, __COMMCTL_SETBAUD, rate);
        if (ret <= 0) {
            diag_printf("Failed\n");
        }
        current_rate = rate;
    }
}

void
do_baud_rate(int argc, char *argv[])
{
    int new_rate, ret;
    bool new_rate_set;
    hal_virtual_comm_table_t *__chan;
    struct option_info opts[1];
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
    struct config_option opt;
#endif

    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void **)&new_rate, (bool *)&new_rate_set, "new baud rate");
    if (!scan_opts(argc, argv, 1, opts, 1, 0, 0, "")) {
        return;
    }
    __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
    if (new_rate_set) {
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
        opt.type = CONFIG_INT;
        opt.enable = (char *)0;
        opt.enable_sense = 1;
        opt.key = "console_baud_rate";
        opt.dflt = new_rate;
        flash_add_config(&opt, true);
#endif
        set_console_baud_rate(new_rate);
    } else {
        ret = CYGACC_COMM_IF_CONTROL(*__chan, __COMMCTL_GETBAUD);
        diag_printf("Baud rate = ");
        if (ret <= 0) {
            diag_printf("unknown\n");
        } else {
            diag_printf("%d\n", ret);
        }
    }
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
