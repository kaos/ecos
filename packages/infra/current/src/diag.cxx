/*========================================================================
//
//      diag.c
//
//      Infrastructure diagnostic output code
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Contributors: nickg
// Date:         1999-02-22
// Purpose:      Infrastructure diagnostic output
// Description:  Implementations of infrastructure diagnostic routines.
//
//####DESCRIPTIONEND####
//
//======================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/infra.h>

#include <cyg/infra/cyg_type.h>         // base types

#include <cyg/infra/diag.h>             // HAL polled output
#include <cyg/hal/hal_arch.h>           // architectural stuff for...
#include <cyg/hal/hal_intr.h>           // interrupt control
#include <cyg/hal/hal_diag.h>           // diagnostic output routines

#ifdef CYGDBG_INFRA_DIAG_PRINTF_USE_VARARG

#include <stdarg.h>

#endif

#ifdef CYGDBG_INFRA_DIAG_USE_DEVICE
#include <cyg/io/io_diag.h>
#endif

/*----------------------------------------------------------------------*/

externC void diag_write_num(
    cyg_uint32  n,              /* number to write              */
    cyg_ucount8 base,           /* radix to write to            */
    cyg_ucount8 sign,           /* sign, '-' if -ve, '+' if +ve */
    cyg_bool    pfzero,         /* prefix with zero ?           */
    cyg_ucount8 width           /* min width of number          */
    );

class Cyg_dummy_diag_init_class {
public:
    Cyg_dummy_diag_init_class() {
#ifdef CYGDBG_INFRA_DIAG_USE_DEVICE
        diag_device_init(); 
#else
        HAL_DIAG_INIT();
#endif
    }
};

#ifdef CYGDBG_INFRA_DIAG_USE_DEVICE

// Initialize after IO devices.
static Cyg_dummy_diag_init_class cyg_dummy_diag_init_obj 
                                      CYGBLD_ATTRIB_INIT_AFTER(CYG_INIT_IO);

/*----------------------------------------------------------------------*/
/* Write single char to output                                          */

externC void diag_write_char(char c)
{
    diag_device_write_char(c);
}

#else

// Initialize after HAL.
static Cyg_dummy_diag_init_class cyg_dummy_diag_init_obj 
                                      CYGBLD_ATTRIB_INIT_AFTER(CYG_INIT_HAL);

/*----------------------------------------------------------------------*/
/* Write single char to output                                          */

#define PREFIX()
//#define PREFIX() HAL_DIAG_WRITE_CHAR(0x0F)

externC void diag_write_char(char c)
{    
    /* Translate LF into CRLF */
    
    if( c == '\n' )
    {
        PREFIX();
        HAL_DIAG_WRITE_CHAR('\r');        
    }

    PREFIX();
    HAL_DIAG_WRITE_CHAR(c);
}
#endif

/*----------------------------------------------------------------------*/
/* Initialize. Call to pull in diag initializing constructor            */

externC void diag_init(void)
{
}

/*----------------------------------------------------------------------*/
/* Write zero terminated string                                         */
  
externC void diag_write_string(const char *psz)
{
    while( *psz ) diag_write_char( *psz++ );
}

/*----------------------------------------------------------------------*/
/* Write decimal value                                                  */

externC void diag_write_dec( cyg_int32 n)
{
    cyg_ucount8 sign;

    if( n < 0 ) n = -n, sign = '-';
    else sign = '+';
    
    diag_write_num( n, 10, sign, false, 0);
}

/*----------------------------------------------------------------------*/
/* Write hexadecimal value                                              */

externC void diag_write_hex( cyg_uint32 n)
{
    diag_write_num( n, 16, '+', false, 0);
}    

/*----------------------------------------------------------------------*/
/* Generic number writing function                                      */
/* The parameters determine what radix is used, the signed-ness of the  */
/* number, its minimum width and whether it is zero or space filled on  */
/* the left.                                                            */

externC void diag_write_long_num(
    cyg_uint64  n,              /* number to write              */
    cyg_ucount8 base,           /* radix to write to            */
    cyg_ucount8 sign,           /* sign, '-' if -ve, '+' if +ve */
    cyg_bool    pfzero,         /* prefix with zero ?           */
    cyg_ucount8 width           /* min width of number          */
    )
{
    char buf[32];
    cyg_count8 bpos;
    char bufinit = pfzero?'0':' ';
    char *digits = "0123456789ABCDEF";

    /* init buffer to padding char: space or zero */
    for( bpos = 0; bpos < (cyg_count8)sizeof(buf); bpos++ ) buf[bpos] = bufinit;

    /* Set pos to start */
    bpos = 0;

    /* construct digits into buffer in reverse order */
    if( n == 0 ) buf[bpos++] = '0';
    else while( n != 0 )
    {
        cyg_ucount8 d = n % base;
        buf[bpos++] = digits[d];
        n /= base;
    }

    /* set pos to width if less. */
    if( (cyg_count8)width > bpos ) bpos = width;

    /* set sign if negative. */
    if( sign == '-' )
    {
        if( buf[bpos-1] == bufinit ) bpos--;
        buf[bpos] = sign;
    }
    else bpos--;

    /* Now write it out in correct order. */
    while( bpos >= 0 )
        diag_write_char(buf[bpos--]);
}

externC void diag_write_num(
    cyg_uint32  n,              /* number to write              */
    cyg_ucount8 base,           /* radix to write to            */
    cyg_ucount8 sign,           /* sign, '-' if -ve, '+' if +ve */
    cyg_bool    pfzero,         /* prefix with zero ?           */
    cyg_ucount8 width           /* min width of number          */
    )
{
    diag_write_long_num((long long)n, base, sign, pfzero, width);
}

/*----------------------------------------------------------------------*/
/* perform some simple sanity checks on a string to ensure that it      */
/* consists of printable characters and is of reasonable length.        */

static cyg_bool diag_check_string( const char *str )
{
    cyg_bool result = true;
    const char *s;

    if( str == NULL ) return false;
    
    for( s = str ; result && *s ; s++ )
    {
        char c = *s;

        /* Check for a reasonable length string. */
        
        if( s-str > 256 ) result = false;

        /* We only really support CR and NL at present. If we want to
         * use tabs or other special chars, this test will have to be
         * expanded.
         */
        
        if( c == '\n' || c == '\r' )
            continue;

        /* Check for printable chars. This assumes ASCII */
        
        if( c < ' ' || c > '~' )
            result = false;

    }

    return result;
}

/*----------------------------------------------------------------------*/

externC void diag_vprintf( const char *fmt, CYG_ADDRWORD *args)
{

    cyg_bool pad = true;

    if( !diag_check_string(fmt) )
    {
        int i;
        diag_write_string("<Bad format string: ");
        diag_write_hex((cyg_uint32)fmt);
        diag_write_string(" :");
        for( i = 0; i < 8; i++ )
        {
            diag_write_char(' ');
            diag_write_hex(args[i]);
        }
        diag_write_string(">\n");
        return;
    }
    
    while( *fmt != '\0' )
    {
        char c = *fmt;

        if( c != '%' ) diag_write_char( c );
        else
        {
            cyg_bool pfzero = false;
            cyg_count8 width = 0;
            char sign = '+';
            cyg_bool long_op = false;
                        
            c = *++fmt;
                        
            if( c == '0' ) pfzero = true;

            while( '0' <= c && c <= '9' )
            {
                width = width*10 + c - '0';
                c = *++fmt;
            }

            if (c == 'l') {
                // %lx and %ld are equivalent to %x/%d
                c = *++fmt;
                // %llx or %lld used to indicate (long long) operand
                if (c == 'l') {
                    long_op = true;
                    c = *++fmt;
                }
            }

            if (long_op) {
                    if(pad) args++;
                    pad=false;
            } else {
                    pad=!pad;
            }
               
            switch( c )
            {
            case 'd':
            case 'D':
            {
                if (long_op) {
                    long long *lp = (long long *)args;
                    long long val = *lp++;
                    args = (CYG_ADDRWORD *)lp;
                    if( val < 0 ) val = -val, sign = '-';
                    diag_write_long_num(val, 10, sign, pfzero, width);
                } else {
                    long val = (long)(*args++);
                    if( val < 0 ) val = -val, sign = '-';
                    diag_write_num(val, 10, sign, pfzero, width);
                }
                break;
            }

            case 'x':
            case 'X':
            {
                if (long_op) {
                    long long *lp = (long long *)args;
                    long long val = *lp++;
                    args = (CYG_ADDRWORD *)lp;
                    diag_write_long_num(val, 16, sign, pfzero, width);
                } else {
                    unsigned long val = (long)(*args++);
                    diag_write_num(val, 16, sign, pfzero, width);
                }
                break;
            }

            case 'c':
            case 'C':
            {
                char ch = (char)(*args++);
                diag_write_char(ch);
                break;
            }

            case 's':
            case 'S':
            {
                char *s = (char *)(*args++);
                cyg_count32 len = 0;
                cyg_count32 pre = 0, post = 0;

                if( s == NULL ) s = "<null>";
                else if( !diag_check_string(s) )
                {
                    diag_write_string("<Not a string: 0x");
                    diag_write_hex((cyg_uint32)s);
                    s = ">";
                    if( width > 25 ) width -= 25;
                    pfzero = false;
                    /* Drop through to print the closing ">" */
                    /* and pad to the required length.       */
                }
                
                while( s[len] != 0 ) len++;
                
                if( width && len > width ) len = width;

                if( pfzero ) pre = width-len;
                else post = width-len;

                while( pre-- > 0 ) diag_write_char(' ');

                while( *s != '\0' && len-- != 0)
                    diag_write_char(*s++);

                while( post-- > 0 ) diag_write_char(' ');
                                
                break;
            }

            case 'b':
            case 'B':
            {
                unsigned long val = (unsigned long)(*args++);
                cyg_uint32 i;
                if( width == 0 ) width = 32;

                for( i = width-1; i >= 0; i-- )
                    diag_write_char( (val&(1<<i))?'1':'.' );
                                
                break;
            }

            case '%':
                diag_write_char('%');
                break;

            default:
                diag_write_char('%');
                diag_write_char(c);
                break;
            }
        }

        fmt++;
    }   
    
}

/*-----------------------------------------------------------------------*/
/* Formatted diagnostic output.                                          */

#ifdef CYGDBG_INFRA_DIAG_PRINTF_USE_VARARG

externC void diag_printf(const char *fmt, ... )
{
    
    CYG_ADDRWORD args[8];

    va_list a;

    va_start(a, fmt);

    /* Move all of the arguments into simple scalars. This avoids
     * having to use varargs to define diag_vprintf().
     */
    
    args[0] = va_arg(a,CYG_ADDRWORD);
    args[1] = va_arg(a,CYG_ADDRWORD);
    args[2] = va_arg(a,CYG_ADDRWORD);
    args[3] = va_arg(a,CYG_ADDRWORD);
    args[4] = va_arg(a,CYG_ADDRWORD);
    args[5] = va_arg(a,CYG_ADDRWORD);
    args[6] = va_arg(a,CYG_ADDRWORD);
    args[7] = va_arg(a,CYG_ADDRWORD);

    va_end(a);
    
    diag_vprintf( fmt, args);
}

#else

/* The prototype for diag_printf in diag.h is defined using K&R syntax   */
/* to allow us to use a variable number of arguments in the call without */
/* using ellipses, which would require use of varargs stuff. If we ever  */
/* need to support arguments that are not simple words, we may need to   */
/* use varargs.                                                          */
/* For the actual implementation, a normal ANSI C prototype is           */
/* acceptable.                                                            */

externC void diag_printf(const char *fmt, CYG_ADDRWORD a1, CYG_ADDRWORD a2,
                         CYG_ADDRWORD a3, CYG_ADDRWORD a4,
                         CYG_ADDRWORD a5, CYG_ADDRWORD a6,
                         CYG_ADDRWORD a7, CYG_ADDRWORD a8)
{
    
    CYG_ADDRWORD args[8];

    args[0] = a1;
    args[1] = a2;
    args[2] = a3;
    args[3] = a4;
    args[4] = a5;
    args[5] = a6;
    args[6] = a7;
    args[7] = a8;
    
    diag_vprintf( fmt, args);
}

#endif

static void
diag_dump_buf_with_offset(cyg_uint8     *p, 
                          CYG_ADDRWORD   s, 
                          cyg_uint8     *base)
{
    int i, c;
    if ((CYG_ADDRWORD)s > (CYG_ADDRWORD)p) {
        s = (CYG_ADDRWORD)s - (CYG_ADDRWORD)p;
    }
    while ((int)s > 0) {
        if (base) {
            diag_printf("%08X: ", (CYG_ADDRWORD)p - (CYG_ADDRWORD)base);
        } else {
            diag_printf("%08X: ", p);
        }
        for (i = 0;  i < 16;  i++) {
            if (i < (int)s) {
                diag_printf("%02X", p[i] & 0xFF);
            } else {
                diag_printf("  ");
            }
            if ((i % 2) == 1) diag_printf(" ");
            if ((i % 8) == 7) diag_printf(" ");
        }
        diag_printf(" |");
        for (i = 0;  i < 16;  i++) {
            if (i < (int)s) {
                c = p[i] & 0xFF;
                if ((c < 0x20) || (c >= 0x7F)) c = '.';
            } else {
                c = ' ';
            }
            diag_printf("%c", c);
        }
        diag_printf("|\n");
        s -= 16;
        p += 16;
    }
}

externC void
diag_dump_buf(void *p, CYG_ADDRWORD s)
{
   diag_dump_buf_with_offset((cyg_uint8 *)p, s, 0);
}

/*-----------------------------------------------------------------------*/
/* EOF infra/diag.c */
