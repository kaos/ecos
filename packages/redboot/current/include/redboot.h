//==========================================================================
//
//      redboot.h
//
//      Standard interfaces for RedBoot
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

#ifndef _REDBOOT_H_
#define _REDBOOT_H_

#include <pkgconf/redboot.h>
#include <pkgconf/hal.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_tables.h>

#ifdef CYGPKG_REDBOOT_NETWORKING
#include <net/net.h>
#include <net/tftp_support.h>
#include <net/bootp.h>
// Determine an IP address for this node, using BOOTP
extern int __bootp_find_local_ip(bootp_header_t *info);
#endif

#ifdef DEFINE_VARS
#define EXTERN
#else
#define EXTERN extern
#endif

// Global variables
EXTERN int argc;
#define MAX_ARGV 32
EXTERN char *argv[MAX_ARGV];
EXTERN unsigned char *ram_start, *ram_end;
EXTERN unsigned long *entry_address;
EXTERN bool config_ok;

#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
EXTERN bool console_selected;
#endif

#ifdef CYGPKG_REDBOOT_NETWORKING
EXTERN bool have_net, use_bootp;
EXTERN bootp_header_t my_bootp_info;
EXTERN int gdb_port;
#endif

#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
EXTERN unsigned char *script;
EXTERN int script_timeout;
#endif

// Prototypes
int  printf(char *fmt, ...);
int  vprintf(char *fmt, va_list ap);
int  sprintf(char *buf, char *fmt, ...);
int  strlen(const char *str);
int  strcmp(const char *s1, const char *s2);
int  strncmp(const char *s1, const char *s2, int len);
int  strcmpci(const char *s1, const char *s2);
int  strncmpci(const char *s1, const char *s2, int len);

void mon_write_char(char c);
int  gets(char *line, int len, int timeout);
bool verify_action(char *fmt, ...);
void dump_buf(void *, CYG_ADDRWORD);

// "console" selection
int  start_console(void);
void end_console(int old_console);

// CLI support functions
bool parse_num(char *s, unsigned long *val, char **es, char *delim);
bool parse_bool(char *s, bool *val);

typedef void cmd_fun(int argc, char *argv[]);
struct cmd {
    char    *str;
    char    *help;
    char    *usage;
    cmd_fun *fun;
} CYG_HAL_TABLE_TYPE;
extern struct cmd *cmd_search(struct cmd *tab, struct cmd *tabend, char *arg);
extern void        cmd_usage(struct cmd *tab, struct cmd *tabend, char *prefix);
#define RedBoot_cmd(_s_,_h_,_u_,_f_) cmd_entry(_s_,_h_,_u_,_f_,RedBoot_commands)
#define _cmd_entry(_s_,_h_,_u_,_f_,_n_)                                   \
cmd_fun _f_;                                                      \
struct cmd _cmd_tab_##_f_ CYG_HAL_TABLE_QUALIFIED_ENTRY(_n_,_f_) = {_s_, _h_, _u_, _f_};
#define cmd_entry(_s_,_h_,_u_,_f_,_n_)                                   \
extern _cmd_entry(_s_,_h_,_u_,_f_,_n_)
#define local_cmd_entry(_s_,_h_,_u_,_f_,_n_)                             \
static _cmd_entry(_s_,_h_,_u_,_f_,_n_)

// Initialization functions
#define RedBoot_INIT_FIRST 0000
#define RedBoot_INIT_PRIO(_n_) 1000+_n_
#define RedBoot_INIT_LAST  9999
typedef void void_fun(void);
typedef void_fun *void_fun_ptr;
struct init_tab_entry {
    void_fun_ptr fun;
} CYG_HAL_TABLE_TYPE;
#define _RedBoot_init(_f_,_p_)                                          \
struct init_tab_entry _init_tab_##_p_##_f_ CYG_HAL_TABLE_QUALIFIED_ENTRY(RedBoot_inits,_f_) = { _f_ }; 
#define RedBoot_init(_f_,_p_) _RedBoot_init(_f_,_p_)


// Option processing support

struct option_info {
    char flag;
    bool takes_arg;
    int  arg_type;
    void **arg;
    bool *arg_set;
    char *name;
};

#define OPTION_ARG_TYPE_NUM 0    // Numeric data
#define OPTION_ARG_TYPE_STR 1    // Generic string
#define OPTION_ARG_TYPE_FLG 2    // Flag only

extern void init_opts(struct option_info *opts, char flag, bool takes_arg, 
                      int arg_type, void **arg, bool *arg_set, char *name);
extern bool scan_opts(int argc, char *argv[], int first, 
                      struct option_info *opts, int num_opts, 
                      void **def_arg, int def_arg_type, char *def_descr);

extern int redboot_getc(void);
extern void redboot_getc_init(int (*fun)(char *, int, int *), int verbose);
extern void redboot_getc_rewind(void);

#define BUF_SIZE 256
typedef struct {
    int (*fun)(char *, int len, int *err);
    unsigned char  buf[BUF_SIZE];
    unsigned char *bufp;
    int   avail, len, err;
    int   verbose, tick;
} getc_info_t;


//
// Validate a hex character
//
__inline__ static bool
_is_hex(char c)
{
    return (((c >= '0') && (c <= '9')) ||
            ((c >= 'A') && (c <= 'F')) ||            
            ((c >= 'a') && (c <= 'f')));
}

//
// Convert a single hex nibble
//
__inline__ static int
_from_hex(char c) 
{
    int ret = 0;

    if ((c >= '0') && (c <= '9')) {
        ret = (c - '0');
    } else if ((c >= 'a') && (c <= 'f')) {
        ret = (c - 'a' + 0x0a);
    } else if ((c >= 'A') && (c <= 'F')) {
        ret = (c - 'A' + 0x0A);
    }
    return ret;
}

//
// Convert a character to lower case
//
__inline__ static char
tolower(char c)
{
    if ((c >= 'A') && (c <= 'Z')) {
        c = (c - 'A') + 'a';
    }
    return c;
}

#endif // _REDBOOT_H_
