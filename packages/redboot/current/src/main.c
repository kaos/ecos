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

#define  DEFINE_VARS
#include <redboot.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>
#include CYGHWR_MEMORY_LAYOUT_H

#include <cyg/hal/hal_tables.h>

// GDB interfaces
extern void breakpoint(void);

// Internal functions
static struct cmd *parse(char *line, int *argc, char **argv);

// CLI command processing (defined in this file)
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
            "<location> [<length>]",
            do_dump 
    );
RedBoot_cmd("cache", 
            "Manage machine caches", 
            "[ON | OFF]",
            do_caches 
    );

// Define table boundaries
CYG_HAL_TABLE_BEGIN( __RedBoot_CMD_TAB__, RedBoot_commands );
CYG_HAL_TABLE_END( __RedBoot_CMD_TAB_END__, RedBoot_commands );
extern struct cmd __RedBoot_CMD_TAB__[], __RedBoot_CMD_TAB_END__;

CYG_HAL_TABLE_BEGIN( __RedBoot_INIT_TAB__, RedBoot_inits );
CYG_HAL_TABLE_END( __RedBoot_INIT_TAB_END__, RedBoot_inits );
extern struct init_tab_entry __RedBoot_INIT_TAB__[], __RedBoot_INIT_TAB_END__;

//
// This is the main entry point for RedBoot
//
void
cyg_start(void)
{
    int res;
    bool prompt = true;
    char line[256];
    struct cmd *cmd;
    int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
    struct init_tab_entry *init_entry;

    CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL);
#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
    console_selected = false;
#endif
    CYGACC_CALL_IF_DELAY_US(2*100000);

    ram_start = (unsigned char *)CYGMEM_REGION_ram;
    ram_end = (unsigned char *)(CYGMEM_REGION_ram+CYGMEM_REGION_ram_SIZE);

    printf("\nRedBoot(tm) debug environment - built %s, %s\n", __TIME__, __DATE__);
#ifdef HAL_PLATFORM_CPU
    printf("Platform: %s (%s) %s\n", HAL_PLATFORM_BOARD, HAL_PLATFORM_CPU, HAL_PLATFORM_EXTRA);
#endif
    printf("Copyright (C) 2000, Red Hat, Inc.\n\n");
    printf("RAM: %p-%p\n", ram_start, ram_end);

    config_ok = false;

    for (init_entry = __RedBoot_INIT_TAB__; init_entry != &__RedBoot_INIT_TAB_END__;  init_entry++) {
        (*init_entry->fun)();
    }

#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
    // Give the guy a chance to abort any boot script
    if (script) {
        unsigned char *hold_script = script;
        printf("== Executing boot script in %d seconds - enter ^C to abort\n", script_timeout);
        script = (unsigned char *)0;
        res = gets(line, sizeof(line), script_timeout*1000);
        if (res == -2) {
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
        if (res < 0) {
            // No input arrived
#ifdef CYGPKG_REDBOOT_NETWORKING
            if (have_net) {
                // Check for incoming TCP debug connection
                net_io_test();
            }
#endif
        } else {
            if (res == 0) {
                // Special case of '$' - need to start GDB protocol
                CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);
                breakpoint();  // Get GDB stubs started, with a proper environment, etc.
            } else {
                if (strlen(line) > 0) {
                    if ((cmd = parse(line, &argc, &argv[0])) != (struct cmd *)0) {
                        (cmd->fun)(argc, argv);
                    } else {
                        printf("Illegal command: %s\n", line);
                    }
                }
                prompt = true;
            }
        }
    }
}

//
// Scan through an input line and break it into "arguments".  These
// are space delimited strings.  Return a structure which points to
// the strings, similar to a Unix program.
// Note: original input is destroyed by replacing the delimiters with 
// null ('\0') characters for ease of use.
//
struct cmd *
parse(char *line, int *argc, char **argv)
{
    char *cp = line;
    int indx = 0;

    while (*cp) {
        // Skip leading spaces
        while (*cp && *cp == ' ') cp++;
        if (!*cp) {
            break;  // Line ended with a string of spaces
        }
        argv[indx++] = cp;
        while (*cp) {
            if (*cp == ' ') {
                *cp++ = '\0';
                break;
            } else if (*cp == '"') {
                // Swallow quote, scan till following one
                if (argv[indx-1] == cp) {
                    argv[indx-1] = ++cp;
                }
                while (*cp && *cp != '"') cp++;
                if (!*cp) {
                    printf("Unbalanced string!\n");
                } else {
                    *cp++ = '\0';
                    break;
                }
            } else {
                cp++;
            }
        }
    }
    *argc = indx;
    return cmd_search(__RedBoot_CMD_TAB__, &__RedBoot_CMD_TAB_END__, argv[0]);
}

//
// Search through a list of commands
//
struct cmd *
cmd_search(struct cmd *tab, struct cmd *tabend, char *arg)
{
    int cmd_len;
    struct cmd *cmd, *cmd2;
    // Search command table
    cmd_len = strlen(arg);
    cmd = tab;
    while (cmd != tabend) {
        if (strncmpci(arg, cmd->str, cmd_len) == 0) {
            if (strlen(cmd->str) > cmd_len) {
                // Check for ambiguous commands here
                // Note: If there are commands which are not length-unique
                // then this check will be invalid.  E.g. "du" and "dump"
                bool first = true;
                cmd2 = tab;
                while (cmd2 != tabend) {
                    if ((cmd != cmd2) && 
                        (strncmpci(arg, cmd2->str, cmd_len) == 0)) {
                        if (first) {
                            printf("Ambiguous command '%s', choices are: %s", arg, cmd->str);
                            first = false;
                        }
                        printf(" %s", cmd2->str);
                    }
                    cmd2++;
                }
                if (!first) {
                    // At least one ambiguity found - fail the lookup
                    printf("\n");
                    return (struct cmd *)0;
                }
            }
            return cmd;
        }
        cmd++;
    }
    return (struct cmd *)0;
}

void
cmd_usage(struct cmd *tab, struct cmd *tabend, char *prefix)
{
    struct cmd *cmd;
    printf("Usage:\n"); for (cmd = tab;  cmd != tabend;  cmd++) {
        printf("  %s%s %s\n", prefix, cmd->str, cmd->usage);
    }
}

// Option processing

// Initialize option table entry (required because these entries
// may have dynamic contents, thus cannot be statically initialized)
//
void
init_opts(struct option_info *opts, char flag, bool takes_arg, 
          int arg_type, void **arg, bool *arg_set, char *name)
{
    opts->flag = flag;
    opts->takes_arg = takes_arg;
    opts->arg_type = arg_type,
    opts->arg = arg;
    opts->arg_set = arg_set;
    opts->name = name;
}

//
// Scan command line arguments (argc/argv), processing options, etc.
//
bool
scan_opts(int argc, char *argv[], int first, 
          struct option_info *opts, int num_opts, 
          void **def_arg, int def_arg_type, char *def_descr)
{
    bool ret = true;
    bool flag_ok;
    bool def_arg_set = false;
    int i, j;
    char c, *s;
    struct option_info *opt;

    if (def_arg && (def_arg_type == OPTION_ARG_TYPE_STR)) {
        *def_arg = (char *)0;
    }
    opt = opts;
    for (j = 0;  j < num_opts;  j++, opt++) {
        if (opt->arg_set) {
            *opt->arg_set = false;
        }
        if (!opt->takes_arg) {
            switch (opt->arg_type) {
            case OPTION_ARG_TYPE_NUM:
                *(int *)opt->arg = 0;
                break;
            case OPTION_ARG_TYPE_FLG:
                *(bool *)opt->arg = false;
                break;
            }
        }
    }
    for (i = first;  i < argc;  i++) {
        if (argv[i][0] == '-') {
            c = argv[i][1];
            flag_ok = false;
            opt = opts;
            for (j = 0;  j < num_opts;  j++, opt++) {
                if (c == opt->flag) {
                    if (opt->arg_set && *opt->arg_set) {
                        printf("%s alread set\n", opt->name);
                        ret = false;
                    }
                    if (opt->takes_arg) {
                        if (argv[i][2] == '=') {
                            s = &argv[i][3];
                        } else {
                            s = argv[i+1];
                            i++;
                        }
                        switch (opt->arg_type) {
                        case OPTION_ARG_TYPE_NUM:
                            if (!parse_num(s, (unsigned long *)opt->arg, 0, 0)) {
                                printf("invalid number '%s' for %s\n", s, opt->name);
                                ret = false;
                            }
                            break;
                        case OPTION_ARG_TYPE_STR:
                            *opt->arg = s;
                            break;
                        }
                        *opt->arg_set = true;
                    } else {
                        switch (opt->arg_type) {
                        case OPTION_ARG_TYPE_NUM:
                            *(int *)opt->arg = *(int *)opt->arg + 1;
                            break;
                        case OPTION_ARG_TYPE_FLG:
                            *(bool *)opt->arg = true;
                            break;
                        }
                    }
                    flag_ok = true;
                    break;
                }
            }
            if (!flag_ok) {
                printf("invalid flag '%c'\n", c);
                ret = false;
            }
        } else {
            if (def_arg) {
                if (def_arg_set) {
                    printf("%s already set\n", def_descr);
                    ret = false;
                }
                switch (def_arg_type) {
                case OPTION_ARG_TYPE_NUM:
                    if (!parse_num(argv[i], (unsigned long *)def_arg, 0, 0)) {
                        printf("invalid number '%s' for %s\n", argv[i], def_descr);
                        ret = false;
                    }
                    break;
                case OPTION_ARG_TYPE_STR:
                    *def_arg = argv[i];
                    break;
                }
                def_arg_set = true;
            } else {
                printf("no default/non-flag arguments supported\n");
                ret = false;
            }
        }
    }
    return ret;
}

//
// Parse (scan) a number
//
bool
parse_num(char *s, unsigned long *val, char **es, char *delim)
{
    bool first = true;
    int radix = 10;
    char c;
    unsigned long result = 0;
    int digit;

    while (*s == ' ') s++;
    while (*s) {
        if (first && (s[0] == '0') && (tolower(s[1]) == 'x')) {
            radix = 16;
            s += 2;
        }
        first = false;
        c = *s++;
        if (_is_hex(c) && ((digit = _from_hex(c)) < radix)) {
            // Valid digit
            result = (result * radix) + digit;
        } else {
            if (delim != (char *)0) {
                // See if this character is one of the delimiters
                char *dp = delim;
                while (*dp && (c != *dp)) dp++;
                if (*dp) break;  // Found a good delimiter
            }
            return false;  // Malformatted number
        }
    }
    *val = result;
    if (es != (char **)0) {
        *es = s;
    }
    return true;
}

bool
parse_bool(char *s, bool *val)
{
    while (*s == ' ') s++;
    if ((*s == 't') || (*s == 'T')) {
        *val = true;
    } else 
    if ((*s == 'f') || (*s == 'F')) {
        *val = false;
    } else {
        return false;
    }
    return true;
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
    unsigned long base, len;

    if (argc < 2) {
        printf("Dump what [location]?\n");
        return;
    }
    if (!parse_num(argv[1], &base, 0, 0)) {
        printf("Invalid start address: %s\n", argv[1]);
        return;
    }
    if (argc > 2) {
        if (!parse_num(argv[2], &len, 0, 0)) {
            printf("Invalid length/end address: %s\n", argv[2]);
            return;
        }
    } else {
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
        if (res == -2) {
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
    (*fun)();
}
