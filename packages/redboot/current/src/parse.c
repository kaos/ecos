//==========================================================================
//
//      parse.c
//
//      RedBoot command line parsing routine
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
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>
#include CYGHWR_MEMORY_LAYOUT_H
#include <cyg/hal/hal_tables.h>

// Define table boundaries
extern struct cmd __RedBoot_CMD_TAB__[], __RedBoot_CMD_TAB_END__;

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
    char *pp;
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
                pp = cp;
                while (*cp && *cp != '"') {
                    if (*cp == '\\') {
                        // Skip over escape - allows for escaped '"'
                        cp++;
                    }
                    // Move string to swallow escapes
                    *pp++ = *cp++;
                }
                if (!*cp) {
                    printf("Unbalanced string!\n");
                } else {
                    if (pp != cp) *pp = '\0';
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
                        printf("** Error: %s already specified\n", opt->name);
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
                                printf("** Error: invalid number '%s' for %s\n", s, opt->name);
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
                printf("** Error: invalid flag '%c'\n", c);
                ret = false;
            }
        } else {
            if (def_arg) {
                if (def_arg_set) {
                    printf("** Error: %s already specified\n", def_descr);
                    ret = false;
                }
                switch (def_arg_type) {
                case OPTION_ARG_TYPE_NUM:
                    if (!parse_num(argv[i], (unsigned long *)def_arg, 0, 0)) {
                        printf("** Error: invalid number '%s' for %s\n", argv[i], def_descr);
                        ret = false;
                    }
                    break;
                case OPTION_ARG_TYPE_STR:
                    *def_arg = argv[i];
                    break;
                }
                def_arg_set = true;
            } else {
                printf("** Error: no default/non-flag arguments supported\n");
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
#ifdef CYGPKG_HAL_MIPS
            // FIXME: tx49 compiler generates 0x2539018 for MUL which
            // isn't any good.
            if (16 == radix)
                result = result << 4;
            else
                result = 10 * result;
            result += digit;
#else
            result = (result * radix) + digit;
#endif
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

