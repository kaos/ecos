//==========================================================================
//
//      main.c
//
//      RedBoot main routine
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
// at http://sources.redhat.com/ecos/ecos-license/
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
            "-b <location> [-l <length>] [-s] [-1|2|4]",
            do_dump 
    );
RedBoot_cmd("x", 
            "Display (hex dump) a range of memory", 
            "-b <location> [-l <length>] [-s] [-1|2|4]",
            do_x
    );
#ifdef CYGBLD_BUILD_REDBOOT_WITH_CKSUM
RedBoot_cmd("cksum", 
            "Compute a 32bit checksum [POSIX algorithm] for a range of memory", 
            "-b <location> -l <length>",
            do_cksum
    );
#endif
#ifdef CYGBLD_BUILD_REDBOOT_WITH_MFILL
RedBoot_cmd("mfill", 
            "Fill a block of memory with a pattern",
            "-b <location> -l <length> -p <pattern> [-1|-2|-4]",
            do_mfill
    );
#endif
#ifdef CYGBLD_BUILD_REDBOOT_WITH_MCMP
RedBoot_cmd("mcmp", 
            "Compare two blocks of memory",
            "-s <location> -d <location> -l <length> [-1|-2|-4]",
            do_mcmp
    );
#endif
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
    diag_printf("Copyright (C) 2000, 2001, 2002, Red Hat, Inc.\n\n");
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
    if (c == '\n') {
        mon_write_char('\r');
    }
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
    char *command;
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

    user_ram_start = workspace_start;
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
#if CYGNUM_REDBOOT_CMD_LINE_EDITING != 0
        cmd_history = true;  // Enable history collection
#endif
        res = _rb_gets(line, sizeof(line), CYGNUM_REDBOOT_CLI_IDLE_TIMEOUT);
#if CYGNUM_REDBOOT_CMD_LINE_EDITING != 0
        cmd_history = false;  // Enable history collection
#endif
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
                expand_aliases(line, sizeof(line));
		command = (char *)&line;
                if ((*command == '#') || (*command == '=')) {
                    // Special cases
                    if (*command == '=') {
                        // Print line on console
                        diag_printf("%s\n", &line[2]);
                    }
                } else {
                    while (strlen(command) > 0) {                    
                        if ((cmd = parse(&command, &argc, &argv[0])) != (struct cmd *)0) {
                            (cmd->fun)(argc, argv);
                        } else {
                            diag_printf("** Error: Illegal command: \"%s\"\n", argv[0]);
                        }
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
show_help(struct cmd *cmd, struct cmd *cmd_end, char *which, char *pre)
{
    bool show;
    int len = 0;

    if (which) {
        len = strlen(which);
    }
    while (cmd != cmd_end) {
        show = true;
        if (which && (strncasecmp(which, cmd->str, len) != 0)) {
            show = false;
        }
        if (show) {
            diag_printf("%s\n  %s %s %s\n", cmd->help, pre, cmd->str, cmd->usage);
            if ((cmd->sub_cmds != (struct cmd *)0) && (which != (char *)0)) {
                show_help(cmd->sub_cmds, cmd->sub_cmds_end, 0, cmd->str);
            }
        }
        cmd++;
    }
}

void
do_help(int argc, char *argv[])
{
    struct cmd *cmd;
    char *which = (char *)0;

    if (!scan_opts(argc, argv, 1, 0, 0, (void **)&which, OPTION_ARG_TYPE_STR, "<topic>")) {
        diag_printf("Invalid argument\n");
        return;
    }
    cmd = __RedBoot_CMD_TAB__;
    show_help(cmd, &__RedBoot_CMD_TAB_END__, which, "");
    return;
}

void
do_dump(int argc, char *argv[])
{
    struct option_info opts[6];
    unsigned long base, len;
    bool base_set, len_set;
    static unsigned long _base, _len;
    static char _size = 1;
    bool srec_dump, set_32bit, set_16bit, set_8bit;
    int i, n, off, cksum;
    cyg_uint8 ch;

    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void **)&base, (bool *)&base_set, "base address");
    init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&len, (bool *)&len_set, "length");
    init_opts(&opts[2], 's', false, OPTION_ARG_TYPE_FLG, 
              (void **)&srec_dump, 0, "dump data using Morotola S-records");
    init_opts(&opts[3], '4', false, OPTION_ARG_TYPE_FLG,
              (void *)&set_32bit, (bool *)0, "dump 32 bit units");
    init_opts(&opts[4], '2', false, OPTION_ARG_TYPE_FLG,
              (void **)&set_16bit, (bool *)0, "dump 16 bit units");
    init_opts(&opts[5], '1', false, OPTION_ARG_TYPE_FLG,
              (void **)&set_8bit, (bool *)0, "dump 8 bit units");
    if (!scan_opts(argc, argv, 1, opts, 6, 0, 0, "")) {
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

    if (set_32bit) {
      _size = 4;
    } else if (set_16bit) {
      _size = 2;
    } else if (set_8bit) {
      _size = 1;
    }

    if (!len_set) {
        len = 32;
    }
    if (srec_dump) {
        off = 0;
        while (off < len) {
            n = (len > 16) ? 16 : len;
            cksum = n+5;
            diag_printf("S3%02X%08X", n+5, off+base);
            for (i = 0;  i < 4;  i++) {
                cksum += (((base+off)>>(i*8)) & 0xFF);
            }
            for (i = 0;  i < n;  i++) {
                ch = *(cyg_uint8 *)(base+off+i);
                diag_printf("%02X", ch);
                cksum += ch;
            }
            diag_printf("%02X\n", ~cksum & 0xFF);
            off += n;
        }
    } else {
        switch( _size ) {
        case 1:
            diag_dump_buf((void *)base, len);
            break;
        case 2:
            diag_dump_buf_16bit((void *)base, len);
            break;
        case 4:
            diag_dump_buf_32bit((void *)base, len);
            break;
        }
    }
    _base = base + len;
    _len = len;
}

// Simple alias for the dump command
void
do_x(int argc, char *argv[])
{
    do_dump(argc, argv);
}

#ifdef CYGBLD_BUILD_REDBOOT_WITH_CKSUM
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
	if (load_address >= (CYG_ADDRESS)ram_start &&
	    load_address_end < (CYG_ADDRESS)ram_end &&
	    load_address < load_address_end) {
	    base = load_address;
	    len = load_address_end - load_address;
            diag_printf("Computing cksum for area %p-%p\n",
                        base, load_address_end);
	} else {
	    diag_printf("usage: cksum -b <addr> -l <length>\n");
	    return;
	}
    }
    crc = posix_crc32((unsigned char *)base, len);
    diag_printf("POSIX cksum = %lu %lu (0x%08lx 0x%08lx)\n", crc, len, crc, len);
}
#endif

#ifdef CYGBLD_BUILD_REDBOOT_WITH_MFILL
void
do_mfill(int argc, char *argv[])
{
    // Fill a region of memory with a pattern
    struct option_info opts[6];
    unsigned long base, pat;
    long len;
    bool base_set, len_set, pat_set;
    bool set_32bit, set_16bit, set_8bit;

    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void **)&base, (bool *)&base_set, "base address");
    init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&len, (bool *)&len_set, "length");
    init_opts(&opts[2], 'p', true, OPTION_ARG_TYPE_NUM, 
              (void **)&pat, (bool *)&pat_set, "pattern");
    init_opts(&opts[3], '4', false, OPTION_ARG_TYPE_FLG,
              (void *)&set_32bit, (bool *)0, "fill 32 bit units");
    init_opts(&opts[4], '2', false, OPTION_ARG_TYPE_FLG,
              (void **)&set_16bit, (bool *)0, "fill 16 bit units");
    init_opts(&opts[5], '1', false, OPTION_ARG_TYPE_FLG,
              (void **)&set_8bit, (bool *)0, "fill 8 bit units");
    if (!scan_opts(argc, argv, 1, opts, 6, 0, 0, "")) {
        return;
    }
    if (!base_set || !len_set) {
        diag_printf("usage: mfill -b <addr> -l <length> [-p <pattern>] [-1|-2|-4]\n");
        return;
    }
    if (!pat_set) {
        pat = 0;
    }
    // No checks here    
    if (set_8bit) {
        // Fill 8 bits at a time
        while ((len -= sizeof(cyg_uint8)) >= 0) {
            *((cyg_uint8 *)base)++ = (cyg_uint8)pat;
        }
    } else if (set_16bit) {
        // Fill 16 bits at a time
        while ((len -= sizeof(cyg_uint16)) >= 0) {
            *((cyg_uint16 *)base)++ = (cyg_uint16)pat;
        }
    } else {
        // Default - 32 bits
        while ((len -= sizeof(cyg_uint32)) >= 0) {
            *((cyg_uint32 *)base)++ = (cyg_uint32)pat;
        }
    }
}
#endif

#ifdef CYGBLD_BUILD_REDBOOT_WITH_MCMP
void
do_mcmp(int argc, char *argv[])
{
    // Fill a region of memory with a pattern
    struct option_info opts[6];
    unsigned long src_base, dst_base;
    long len;
    bool src_base_set, dst_base_set, len_set;
    bool set_32bit, set_16bit, set_8bit;

    init_opts(&opts[0], 's', true, OPTION_ARG_TYPE_NUM, 
              (void **)&src_base, (bool *)&src_base_set, "base address");
    init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&len, (bool *)&len_set, "length");
    init_opts(&opts[2], 'd', true, OPTION_ARG_TYPE_NUM, 
              (void **)&dst_base, (bool *)&dst_base_set, "base address");
    init_opts(&opts[3], '4', false, OPTION_ARG_TYPE_FLG,
              (void *)&set_32bit, (bool *)0, "fill 32 bit units");
    init_opts(&opts[4], '2', false, OPTION_ARG_TYPE_FLG,
              (void **)&set_16bit, (bool *)0, "fill 16 bit units");
    init_opts(&opts[5], '1', false, OPTION_ARG_TYPE_FLG,
              (void **)&set_8bit, (bool *)0, "fill 8 bit units");
    if (!scan_opts(argc, argv, 1, opts, 6, 0, 0, "")) {
        return;
    }
    if (!src_base_set || !dst_base_set || !len_set) {
        diag_printf("usage: mcmp -s <addr> -d <addr> -l <length> [-1|-2|-4]\n");
        return;
    }
    // No checks here    
    if (set_8bit) {
        // Compare 8 bits at a time
        while ((len -= sizeof(cyg_uint8)) >= 0) {
            if (*((cyg_uint8 *)src_base)++ != *((cyg_uint8 *)dst_base)++) {
                ((cyg_uint8 *)src_base)--;
                ((cyg_uint8 *)dst_base)--;
                diag_printf("Buffers don't match - %p=0x%02x, %p=0x%02x\n",
                            src_base, *((cyg_uint8 *)src_base),
                            dst_base, *((cyg_uint8 *)dst_base));
                return;
            }
        }
    } else if (set_16bit) {
        // Compare 16 bits at a time
        while ((len -= sizeof(cyg_uint16)) >= 0) {
            if (*((cyg_uint16 *)src_base)++ != *((cyg_uint16 *)dst_base)++) {
                ((cyg_uint16 *)src_base)--;
                ((cyg_uint16 *)dst_base)--;
                diag_printf("Buffers don't match - %p=0x%04x, %p=0x%04x\n",
                            src_base, *((cyg_uint16 *)src_base),
                            dst_base, *((cyg_uint16 *)dst_base));
                return;
            }
        }
    } else {
        // Default - 32 bits
        while ((len -= sizeof(cyg_uint32)) >= 0) {
            if (*((cyg_uint32 *)src_base)++ != *((cyg_uint32 *)dst_base)++) {
                ((cyg_uint32 *)src_base)--;
                ((cyg_uint32 *)dst_base)--;
                diag_printf("Buffers don't match - %p=0x%08x, %p=0x%08x\n",
                            src_base, *((cyg_uint32 *)src_base),
                            dst_base, *((cyg_uint32 *)dst_base));
                return;
            }
        }
    }
}
#endif

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

    entry = entry_address;  // Default from last 'load' operation
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

int
set_console_baud_rate(int rate)
{
    hal_virtual_comm_table_t *__chan;
    static int current_rate = CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD;
    int ret = current_rate;

    if (rate != current_rate) {
        __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
        ret = CYGACC_COMM_IF_CONTROL(*__chan, __COMMCTL_SETBAUD, rate);
        if (ret < 0) {
            diag_printf("Setting console baud rate to %d failed\n", rate);
            return ret;
        }
        ret = current_rate;
        current_rate = rate;
    }
    return ret;
}

static void
_sleep(int ms)
{
    int i;
    for (i = 0;  i < ms;  i++) {
        CYGACC_CALL_IF_DELAY_US((cyg_int32)1000);
    }
}

void
do_baud_rate(int argc, char *argv[])
{
    int new_rate, ret, old_rate;
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
        diag_printf("Baud rate will be changed to %d - update your settings\n", new_rate);
        _sleep(500);  // Give serial time to flush
        old_rate = CYGACC_COMM_IF_CONTROL(*__chan, __COMMCTL_GETBAUD);
        ret = set_console_baud_rate(new_rate);
        if (ret < 0) {
            if (old_rate > 0) {
                // Try to restore
                set_console_baud_rate(old_rate);
                _sleep(500);  // Give serial time to flush
                diag_printf("\nret = %d\n", ret);
            }
            return;  // Couldn't set the desired rate
        }
        old_rate = ret;
        // Make sure this new rate works or back off to previous value
        // Sleep for a few seconds, then prompt to see if it works
        _sleep(3000);  // Give serial time to flush
        if (!verify_action_with_timeout(5000, "Baud rate changed to %d", new_rate)) {
            _sleep(500);  // Give serial time to flush
            set_console_baud_rate(old_rate);
            _sleep(500);  // Give serial time to flush
            return;
        }
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
        opt.type = CONFIG_INT;
        opt.enable = (char *)0;
        opt.enable_sense = 1;
        opt.key = "console_baud_rate";
        opt.dflt = new_rate;
        flash_add_config(&opt, true);
#endif
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
