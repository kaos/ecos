//==========================================================================
//
//      printf.c
//
//      Stand-alone minimal printf support for RedBoot
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

#include <stdarg.h>
#include <redboot.h>

static int
_cvt(unsigned long val, char *buf, long radix, char *digits)
{
    char temp[80];
    char *cp = temp;
    int length = 0;
    if (val == 0) {
        /* Special case */
        *cp++ = '0';
    } else {
        while (val) {
            *cp++ = digits[val % radix];
            val /= radix;
        }
    }
    while (cp != temp) {
        *buf++ = *--cp;
        length++;
    }
    *buf = '\0';
    return (length);
}

#define is_digit(c) ((c >= '0') && (c <= '9'))

static int
_vprintf(void (*putc)(char c), char *fmt0, va_list ap)
{
    char buf[32];
    char c, sign, *cp=buf;
    int left_prec, right_prec, zero_fill, pad, pad_on_right;
    long val;
    int res = 0, length = 0;
    while ((c = *fmt0++) != '\0') {
        if (c == '%') {
            c = *fmt0++;
            left_prec = right_prec = pad_on_right = 0;
            if (c == '-') {
                c = *fmt0++;
                pad_on_right++;
            }
            if (c == '0') {
                zero_fill = true;
                c = *fmt0++;
            } else {
                zero_fill = false;
            }
            while (is_digit(c)) {
                left_prec = (left_prec * 10) + (c - '0');
                c = *fmt0++;
            }
            if (c == '.') {
                c = *fmt0++;
                zero_fill++;
                while (is_digit(c)) {
                    right_prec = (right_prec * 10) + (c - '0');
                    c = *fmt0++;
                }
            } else {
                right_prec = left_prec;
            }
            sign = '\0';
            if (c == 'l') {
                // 'long' qualifier
                c = *fmt0++;
            }
            switch (c) {
            case 'p':  // Pointer
                (*putc)('0');
                (*putc)('x');
                zero_fill = true;
                left_prec = sizeof(unsigned long)*2;
            case 'd':
            case 'u':
            case 'x':
            case 'X':
                val = va_arg(ap, long);
                switch (c) {
                case 'd':
                    if (val < 0) {
                        sign = '-';
                        val = -val;
                    }
                case 'u':
                    length = _cvt(val, buf, 10, "0123456789");
                    break;
                case 'p':
                case 'x':
                    length = _cvt(val, buf, 16, "0123456789abcdef");
                    break;
                case 'X':
                    length = _cvt(val, buf, 16, "0123456789ABCDEF");
                    break;
                }
                cp = buf;
                break;
            case 's':
                cp = va_arg(ap, char *);
                length = strlen(cp);
                break;
            case 'c':
                c = va_arg(ap, long /*char*/);
                (*putc)(c);
                res++;
                continue;
            default:
                (*putc)('?');
                res++;
            }
            pad = left_prec - length;
            if (sign != '\0') {
                pad--;
            }
            if (zero_fill) {
                c = '0';
                if (sign != '\0') {
                    (*putc)(sign);
                    res++;
                    sign = '\0';
                }
            } else {
                c = ' ';
            }
            if (!pad_on_right) {
                while (pad-- > 0) {
                    (*putc)(c);
                    res++;
                }
            }
            if (sign != '\0') {
                (*putc)(sign);
                res++;
            }
            while (length-- > 0) {
                c = *cp++;
                if (c == '\n') {
                    (*putc)('\r');
                    res++;
                }
                (*putc)(c);
                res++;
            }
            if (pad_on_right) {
                while (pad-- > 0) {
                    (*putc)(' ');
                    res++;
                }
            }
        } else {
            if (c == '\n') {
                (*putc)('\r');
                res++;
            }
            (*putc)(c);
            res++;
        }
    }
    return (res);
}

static char *_sprintf_ptr;

static void 
_sputc(char c)
{
   *_sprintf_ptr++ = c;
   *_sprintf_ptr = '\0';
}

int
sprintf(char *buf, char *fmt, ...)
{
    int ret;
    va_list ap;

    va_start(ap, fmt);
    _sprintf_ptr = buf;
    ret = _vprintf(_sputc, fmt, ap);
    va_end(ap);
    return (ret);
}

int 
vsprintf(char *buf, char *fmt, va_list ap)
{
    int ret;

    _sprintf_ptr = buf;
    ret = _vprintf(_sputc, fmt, ap);
    return (ret);
}

extern void mon_write_char(char);

int
printf(char *fmt, ...)
{
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = _vprintf(mon_write_char, fmt, ap);
    va_end(ap);
    return (ret);
}

int
vprintf(char *fmt, va_list ap)
{
    int ret;

    ret = _vprintf(mon_write_char, fmt, ap);
    return (ret);
}

