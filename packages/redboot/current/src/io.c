//==========================================================================
//
//      io.c
//
//      RedBoot I/O support
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

#include "redboot.h"

// GDB interface functions
extern void ungetDebugChar(char c);

void 
mon_write_char(char c)
{
    hal_virtual_comm_table_t *__chan;

#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
    if (!console_selected) {
        int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
        int i;
        // Send output to all channels
        for (i = 0;  i < CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS;  i++) {
            CYGACC_CALL_IF_SET_CONSOLE_COMM(i);
            __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
            CYGACC_COMM_IF_PUTC(*__chan, c);
        }
        CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);
    } else 
#endif
    {
        __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
        if (__chan)
            CYGACC_COMM_IF_PUTC(*__chan, c);
        else {
            __chan = CYGACC_CALL_IF_DEBUG_PROCS();
            CYGACC_COMM_IF_PUTC(*__chan, c);
        }
    }
}

static void 
mon_read_char(char *c)
{
    hal_virtual_comm_table_t* __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
    
    if (__chan)
        *c = CYGACC_COMM_IF_GETC(*__chan);
    else {
        __chan = CYGACC_CALL_IF_DEBUG_PROCS();
        *c = CYGACC_COMM_IF_GETC(*__chan);
    }
}

static bool
mon_read_char_with_timeout(char *c)
{
    bool res;
    hal_virtual_comm_table_t *__chan;

#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
    if (!console_selected) {
        int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
        int i;
        // Try input from all channels
        for (i = 0;  i < CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS;  i++) {
            CYGACC_CALL_IF_SET_CONSOLE_COMM(i);
            __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
            res = CYGACC_COMM_IF_GETC_TIMEOUT(*__chan, c);
            if (res) {
                // Input available on this channel, make it be the console
                console_selected = true;
                CYGACC_CALL_IF_SET_DEBUG_COMM(i);
                return res;
            }
        }
        CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);        
    } else 
#endif
    {
        __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
        if (__chan)
            res = CYGACC_COMM_IF_GETC_TIMEOUT(*__chan, c);
        else {
            __chan = CYGACC_CALL_IF_DEBUG_PROCS();
            res = CYGACC_COMM_IF_GETC_TIMEOUT(*__chan, c);
        }
    }
    return res;
}

static void
mon_set_read_char_timeout(int ms)
{
    hal_virtual_comm_table_t *__chan;

#ifdef CYGPKG_REDBOOT_ANY_CONSOLE
    if (!console_selected) {
        int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
        int i;
        // Set timeout on each channel so total amounts to desired value
        ms = ms / CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS;
        for (i = 0;  i < CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS;  i++) {
            CYGACC_CALL_IF_SET_CONSOLE_COMM(i);
            if ((__chan = CYGACC_CALL_IF_CONSOLE_PROCS()) != 0) {
                CYGACC_COMM_IF_CONTROL(*__chan, __COMMCTL_SET_TIMEOUT, ms);
            }
        }
        CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);        
    } else 
#endif
    if ((__chan = CYGACC_CALL_IF_CONSOLE_PROCS()) != 0) {
        CYGACC_COMM_IF_CONTROL(*__chan, __COMMCTL_SET_TIMEOUT, ms);
    }
    if ((__chan = CYGACC_CALL_IF_DEBUG_PROCS()) != 0) {
        CYGACC_COMM_IF_CONTROL(*__chan, __COMMCTL_SET_TIMEOUT, ms);
    }
}

//
// Read a line of input from the user
// Return:
//    n: 'n' valid characters received
//    0: '$' (GDB lead-in)
//   -1: No input before timeout
//   -2: ^C typed
//
int
gets(char *buf, int buflen, int timeout)
{
    char *ptr = buf;
    char c;
    bool res = false;
    static char last_ch = '\0';

    while (true) {
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
        if (script && *script) {
            c = *script++;
        } else
#endif
        if ((timeout > 0) && (ptr == buf)) {
            mon_set_read_char_timeout(50);
            while (timeout > 0) {
                res = mon_read_char_with_timeout(&c);
                if (res) {
                    // Got a character
                    break;
                }
                timeout -= 50;
            }
            if (res == false) {
                return -1;  // Input timed out
            }
        } else {
            mon_read_char(&c);
        }
        *ptr = '\0';
        switch (c) {
        case 0x03: // ^C
            if (ptr == buf) {
                printf("^C\n");
                return -2;
            }
            *ptr++ = c;
            break;
        case '\n':
        case '\r':
            // If previous character was the "other" end-of-line, ignore this one
            if (((c == '\n') && (last_ch == '\r')) ||
                ((c == '\r') && (last_ch == '\n'))) {
                break;
            }
            // End of line
            mon_write_char('\r');
            mon_write_char('\n');
            last_ch = c;
            return 1;
        case '\b':
        case 0x7F:  // DEL
            if (ptr != buf) {
                mon_write_char('\b');
                mon_write_char(' ');
                mon_write_char('\b');
                ptr--;
            }
            break;
        case '+':
        case '$':
            if (ptr == buf) {
                // Give up and try GDB protocol
                ungetDebugChar(c);  // Push back character so stubs will see it
                return 0;
            }
            // Fall through - accept '$' at other than start of line
        default:
            mon_write_char(c);
            *ptr++ = c;
        }
        last_ch = c;
    }
}

void
dump_buf_with_offset(cyg_uint8     *p, 
                     CYG_ADDRWORD   s, 
                     cyg_uint8     *base)
{
    int i, c;
    if ((CYG_ADDRWORD)s > (CYG_ADDRWORD)p) {
        s = (CYG_ADDRWORD)s - (CYG_ADDRWORD)p;
    }
    while ((int)s > 0) {
        if (base) {
            printf("0x%08X: ", (CYG_ADDRWORD)p - (CYG_ADDRWORD)base);
        } else {
            printf("0x%08X: ", (CYG_ADDRWORD)p);
        }
        for (i = 0;  i < 16;  i++) {
            if (i < (int)s) {
                printf("%02X", p[i] & 0xFF);
            } else {
                printf("  ");
            }
            if ((i % 2) == 1) printf(" ");
            if ((i % 8) == 7) printf(" ");
        }
        printf(" |");
        for (i = 0;  i < 16;  i++) {
            if (i < (int)s) {
                c = p[i] & 0xFF;
                if ((c < 0x20) || (c >= 0x7F)) c = '.';
            } else {
                c = ' ';
            }
            printf("%c", c);
        }
        printf("|\n");
        s -= 16;
        p += 16;
    }
}

void
dump_buf(void *p, CYG_ADDRWORD s)
{
   dump_buf_with_offset((cyg_uint8 *)p, s, 0);
}

bool
verify_action(char *fmt, ...)
{
    va_list ap;
    char ans[8];
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
    unsigned char *hold_script = script;
#endif

    va_start(ap, fmt);
    vprintf(fmt, ap);
    printf(" - are you sure (y/n)? ");
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
    script = (unsigned char *)0;
#endif
    gets(ans, sizeof(ans), 0);
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
    script = hold_script;
#endif
    return ((ans[0] == 'y') || (ans[0] == 'Y'));
}
