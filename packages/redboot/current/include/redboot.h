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

#ifndef _REDBOOT_H_
#define _REDBOOT_H_

#include <pkgconf/redboot.h>
#include <pkgconf/hal.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_tables.h>
#include <cyg/infra/diag.h>
#include <string.h>

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
#define MAX_ARGV 16
EXTERN char *argv[MAX_ARGV];
EXTERN unsigned char *ram_start, *ram_end;
EXTERN unsigned char *user_ram_start, *user_ram_end;
EXTERN unsigned char *workspace_start, *workspace_end;
EXTERN unsigned long workspace_size;
EXTERN unsigned long *entry_address;

#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
EXTERN bool console_selected;
#endif
EXTERN bool console_echo;
EXTERN bool gdb_active;

#ifdef CYGPKG_REDBOOT_NETWORKING
EXTERN bool have_net, use_bootp;
EXTERN bootp_header_t my_bootp_info;
EXTERN int gdb_port;
EXTERN bool net_debug;
#endif

#ifdef CYGFUN_REDBOOT_BOOT_SCRIPT
EXTERN unsigned char *script;
EXTERN int script_timeout;
#ifdef CYGSEM_REDBOOT_VARIABLE_BAUD_RATE
EXTERN int console_baud_rate;
#endif
#endif

// Prototypes
typedef int _printf_fun(const char *fmt, ...);
externC int  strcasecmp(const char *s1, const char *s2);
externC int  strncasecmp(const char *s1, const char *s2, size_t len);

externC void mon_write_char(char c);
externC bool verify_action(char *fmt, ...);
//void dump_buf(void *, CYG_ADDRWORD);
//void dump_buf_with_offset(void *, CYG_ADDRWORD, void *);
//void vdump_buf_with_offset(_printf_fun *pf, void *, CYG_ADDRWORD, void *);

// Read a single line of input from the console, possibly with timeout
externC int  _rb_gets(char *line, int len, int timeout);
// Result codes from 'gets()'
#define _GETS_TIMEOUT -1
#define _GETS_CTRLC   -2
#define _GETS_GDB      0
#define _GETS_OK       1

// "console" selection
externC int  start_console(void);
externC void end_console(int old_console);

// CRC support
unsigned short crc16(unsigned char *buf, int len);
unsigned long  crc32(unsigned char *buf, int len);

#ifdef CYGPKG_COMPRESS_ZLIB
// Decompression support
typedef struct _pipe {
    unsigned char* in_buf;              // only changed by producer
    int in_avail;                       // only changed by producer
    unsigned char* out_buf;             // only changed by consumer (init by producer)
    int out_size;                       // only changed by consumer (init by producer)
    const char* msg;                    // message from consumer
    void* priv;                         // handler's data
} _pipe_t;

typedef int _decompress_fun_init(_pipe_t*);
typedef int _decompress_fun_inflate(_pipe_t*);
typedef int _decompress_fun_close(_pipe_t*, int);

externC _decompress_fun_init* _dc_init;
externC _decompress_fun_inflate* _dc_inflate;
externC _decompress_fun_close* _dc_close;
#endif // CYGPKG_COMPRESS_ZLIB

// CLI support functions
externC bool parse_num(char *s, unsigned long *val, char **es, char *delim);
externC bool parse_bool(char *s, bool *val);

typedef void cmd_fun(int argc, char *argv[]);
struct cmd {
    char    *str;
    char    *help;
    char    *usage;
    cmd_fun *fun;
} CYG_HAL_TABLE_TYPE;
externC struct cmd *cmd_search(struct cmd *tab, struct cmd *tabend, char *arg);
externC void        cmd_usage(struct cmd *tab, struct cmd *tabend, char *prefix);
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
// Specify a 3 digit numeric value for proper prioritizing
#define RedBoot_INIT_PRIO(_n_) 1##_n_
#define RedBoot_INIT_LAST  9999
typedef void void_fun(void);
typedef void_fun *void_fun_ptr;
struct init_tab_entry {
    void_fun_ptr fun;
} CYG_HAL_TABLE_TYPE;
#define _RedBoot_init(_f_,_p_)                                          \
struct init_tab_entry _init_tab_##_p_##_f_                              \
  CYG_HAL_TABLE_QUALIFIED_ENTRY(RedBoot_inits,_p_##_f_) = { _f_ }; 
#define RedBoot_init(_f_,_p_) _RedBoot_init(_f_,_p_)

// Main loop [idle] call-back functions
#define RedBoot_IDLE_FIRST          0000
#define RedBoot_IDLE_BEFORE_NETIO   3000
#define RedBoot_IDLE_NETIO          5000
#define RedBoot_IDLE_AFTER_NETIO    7000
#define RedBoot_IDLE_LAST           9999
typedef void idle_fun(bool);
typedef idle_fun *idle_fun_ptr;
struct idle_tab_entry {
    idle_fun_ptr fun;
} CYG_HAL_TABLE_TYPE;
#define _RedBoot_idle(_f_,_p_)                                          \
struct idle_tab_entry _idle_tab_##_p_##_f_                              \
   CYG_HAL_TABLE_QUALIFIED_ENTRY(RedBoot_idle,_p_##_f_) = { _f_ }; 
#define RedBoot_idle(_f_,_p_) _RedBoot_idle(_f_,_p_)

// This function called when changing idle/not - mostly used by I/O
// to support idle when timeout, etc.
void do_idle(bool state);
 
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

// Command line parsing
externC struct cmd *parse(char **line, int *argc, char **argv);

externC void init_opts(struct option_info *opts, char flag, bool takes_arg, 
                       int arg_type, void **arg, bool *arg_set, char *name);
externC bool scan_opts(int argc, char *argv[], int first, 
                       struct option_info *opts, int num_opts, 
                       void **def_arg, int def_arg_type, char *def_descr);

externC int redboot_getc(void);
externC void redboot_getc_init(int (*fun)(char *, int, int *), int verbose);
externC void redboot_getc_rewind(void);

#define BUF_SIZE 256
typedef struct {
    int (*fun)(char *, int len, int *err);
    unsigned char  buf[BUF_SIZE];
    unsigned char *bufp;
    int   avail, len, err;
    int   verbose, tick;
} getc_info_t;


#ifdef CYGNUM_HAL_VIRTUAL_VECTOR_AUX_CHANNELS
#define CYGNUM_HAL_VIRTUAL_VECTOR_NUM_CHANNELS \
  (CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS+CYGNUM_HAL_VIRTUAL_VECTOR_AUX_CHANNELS)
#else
#define CYGNUM_HAL_VIRTUAL_VECTOR_NUM_CHANNELS \
  CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS
#endif

#ifdef CYGPKG_REDBOOT_NETWORKING
//-----------------------------------------------------------------------------
// DNS wrapper
#ifdef CYGPKG_REDBOOT_NETWORKING_DNS

// I would really like if we could just pull in cyg/ns/dns/dns.h, but
// that would require adding dummy <network.h> and <netinet/in.h> files.

// Host name / IP mapping
struct hostent {
    char    *h_name;        /* official name of host */
    char    **h_aliases;    /* alias list */
    int     h_addrtype;     /* host address type */
    int     h_length;       /* length of address */
    char    **h_addr_list;  /* list of addresses */
};
#define h_addr  h_addr_list[0]  /* for backward compatibility */

externC int redboot_dns_res_init(void);
externC struct hostent *gethostbyname(const char *host);

// Error reporting
externC int h_errno;

#define DNS_SUCCESS  0
#define HOST_NOT_FOUND 1
#define TRY_AGAIN      2
#define NO_RECOVERY    3
#define NO_DATA        4

static inline bool
_gethostbyname(const char* name, in_addr_t* host)
{ 
    struct hostent* hent = gethostbyname(name);
    if (hent) {
        memcpy(host, hent->h_addr_list[0], sizeof(in_addr_t));
        return true;
    }
    // Fall back to inet_aton - gethostbyname may already have tried
    // it, but we can't know for sure (the DNS IP may not have been
    // valid, preventing the inet_aton).
    return inet_aton(name, host);
}
#else
static inline bool
_gethostbyname(const char* name, in_addr_t* host)
{ 
    return inet_aton(name, host);
}
#endif // CYGPKG_REDBOOT_NETWORKING_DNS
#endif // CYGPKG_REDBOOT_NETWORKING

//-----------------------------------------------------------------------------
// String functions. Some of these are duplicates of the same functions in
// the I18N package.

// Validate a hex character
__inline__ static bool
_is_hex(char c)
{
    return (((c >= '0') && (c <= '9')) ||
            ((c >= 'A') && (c <= 'F')) ||            
            ((c >= 'a') && (c <= 'f')));
}

// Convert a single hex nibble
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

// Convert a character to lower case
__inline__ static char
_tolower(char c)
{
    if ((c >= 'A') && (c <= 'Z')) {
        c = (c - 'A') + 'a';
    }
    return c;
}

// Validate alpha
__inline__ static bool
isalpha(int c)
{
    return (((c >= 'a') && (c <= 'z')) || 
            ((c >= 'A') && (c <= 'Z')));
}

// Validate digit
__inline__ static bool
isdigit(int c)
{
    return ((c >= '0') && (c <= '9'));
}

// Validate alphanum
__inline__ static bool
isalnum(int c)
{
    return (isalpha(c) || isdigit(c));
}

#endif // _REDBOOT_H_
