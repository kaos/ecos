//===========================================================================
//
//      vfnprintf.c
//
//      I/O routines for vfnprintf() for use with ANSI C library
//
//===========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1998-02-13
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================
//
// This code is based on original code with the following copyright:
//
/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library? And do we want the stdio stuff?
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// INCLUDES

#include <cyg/infra/cyg_type.h>   // Common type definitions and support
#include <stdarg.h>               // Variable argument definitions
#include <stdio.h>                // Standard header for all stdio files
#include "clibincl/stringsupp.hxx"// _memchr() and _strlen() functions
#include "clibincl/stream.hxx"    // C library streams


// EXPORTED SYMBOLS

externC int 
vfnprintf ( FILE *, size_t, const char *, va_list ) 
    CYGPRI_LIBC_WEAK_ALIAS("_vfnprintf");


#ifdef CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT

# include <float.h>      // for DBL_DIG etc. below
# include <math.h>       // for modf()
# include <sys/ieeefp.h> // Cyg_libm_ieee_double_shape_type

# define MAXFRACT  DBL_DIG
# define MAXEXP    DBL_MAX_10_EXP

# define BUF             (MAXEXP+MAXFRACT+1)     /* + decimal point */
# define DEFPREC         6

static int
cvt( double, int, int, char *, int, char *, char * );

#else // ifdef CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT

# define BUF            40

#endif // ifdef CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT

/*
 * Actual printf innards.
 *
 * This code is large and complicated...
 */



/*
 * Macros for converting digits to letters and vice versa
 */
#define to_digit(c)     ((c) - '0')
#define is_digit(c)     ((unsigned)to_digit(c) <= 9)
#define to_char(n)      ((n) + '0')

/*
 * Flags used during conversion.
 */
#define ALT             0x001           /* alternate form */
#define HEXPREFIX       0x002           /* add 0x or 0X prefix */
#define LADJUST         0x004           /* left adjustment */
#define LONGDBL         0x008           /* long double; unimplemented */
#define LONGINT         0x010           /* long integer */
#define QUADINT         0x020           /* quad integer */
#define SHORTINT        0x040           /* short integer */
#define ZEROPAD         0x080           /* zero (as opposed to blank) pad */
#define FPT             0x100           /* Floating point number */

static int
__mbtowc(char *pwc, const char *s, size_t n)
{
        if (s == NULL)
                return 0;
        if (n == 0)
                return -1;
        if (pwc)
                *pwc = (char) *s;
        return (*s != '\0');
}


externC int 
_vfnprintf ( FILE *stream, size_t n, const char *format, va_list arg)
{
        char *fmt;     /* format string */
        int ch;        /* character from fmt */
        int x, y;      /* handy integers (short term usage) */
        char *cp;      /* handy char pointer (short term usage) */
        int flags;     /* flags as above */
        int ret;                /* return value accumulator */
        int width;              /* width from format (%8d), or 0 */
        int prec;               /* precision from format (%.3d), or -1 */
        char sign;              /* sign prefix (' ', '+', '-', or \0) */
        char wc;

#define quad_t    long long
#define u_quad_t  unsigned long long

        u_quad_t _uquad;        /* integer arguments %[diouxX] */
        enum { OCT, DEC, HEX } base;/* base for [diouxX] conversion */
        int dprec;              /* a copy of prec if [diouxX], 0 otherwise */
        int fieldsz;            /* field size expanded by sign, etc */
        int realsz;             /* field size expanded by dprec */
        int size;               /* size of converted field or string */
        char *xdigs;            /* digits for [xX] conversion */
#define NIOV 8
        char buf[BUF];          /* space for %c, %[diouxX], %[eEfgG] */
        char ox[2];             /* space for 0x hex-prefix */
#ifdef CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT
        char softsign;          /* temporary negative sign for floats */
        double _double;         /* double precision arguments %[eEfgG] */
        int fpprec;             /* `extra' floating precision in [eEfgG] */
#endif

        /*
         * Choose PADSIZE to trade efficiency vs. size.  If larger printf
         * fields occur frequently, increase PADSIZE and make the initialisers
         * below longer.
         */
#define PADSIZE 16              /* pad chunk size */
        static char blanks[PADSIZE] =
         {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
        static char zeroes[PADSIZE] =
         {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};

        /*
         * BEWARE, these `goto error' on error, and PAD uses `n'.
         */
#define PRINT(ptr, len) { cyg_ucount32 length = len; \
             if (((Cyg_StdioStream *)stream)->write( (const cyg_uint8 *)ptr, \
                                               (cyg_ucount32) len, &length )) \
                goto error; }


#define PAD(howmany, with) { \
        if ((x = (howmany)) > 0) { \
                while (x > PADSIZE) { \
                        PRINT(with, PADSIZE); \
                        x -= PADSIZE; \
                } \
                PRINT(with, x); \
        } \
}
#define FLUSH() { if (((Cyg_StdioStream *)stream)->flush_output()) \
                     goto error; }

        /*
         * To extend shorts properly, we need both signed and unsigned
         * argument extraction methods.
         */

#define SARG() \
        (flags&QUADINT ? va_arg(arg, cyg_int64) : \
            flags&LONGINT ? va_arg(arg, cyg_int32) : \
            flags&SHORTINT ? (long)va_arg(arg, cyg_int32) : \
            (long)va_arg(arg, cyg_int32))
#define UARG() \
        (flags&QUADINT ? va_arg(arg, cyg_uint64) : \
            flags&LONGINT ? va_arg(arg, cyg_uint32) : \
            flags&SHORTINT ? (unsigned long)va_arg(arg, cyg_uint32) : \
            (unsigned long)va_arg(arg, cyg_uint32))


        xdigs = NULL;  // stop compiler whinging
        fmt = (char *)format;
        ret = 0;

        /*
         * Scan the format for conversions (`%' character).
         */
        for (;;) {
                cp = (char *)fmt;
                while ((x = __mbtowc(&wc, fmt, 1)) > 0) {
                        fmt += x;
                        if (wc == '%') {
                                fmt--;
                                break;
                        }
                }
                if ((y = fmt - cp) != 0) {
                        PRINT(cp, y);
                        ret += y;
                }
                if ((x <= 0) || (ret >= (int)n))  // @@@ this check with n isn't good enough
                        goto done;
                fmt++;          /* skip over '%' */

                flags = 0;
                dprec = 0;
#ifdef CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT
                fpprec = 0;
#endif
                width = 0;
                prec = -1;
                sign = '\0';

rflag:          ch = *fmt++;
reswitch:       switch (ch) {
                case ' ':
                        /*
                         * ``If the space and + flags both appear, the space
                         * flag will be ignored.''
                         *      -- ANSI X3J11
                         */
                        if (!sign)
                                sign = ' ';
                        goto rflag;
                case '#':
                        flags |= ALT;
                        goto rflag;
                case '*':
                        /*
                         * ``A negative field width argument is taken as a
                         * - flag followed by a positive field width.''
                         *      -- ANSI X3J11
                         * They don't exclude field widths read from args.
                         */
                        if ((width = va_arg(arg, int)) >= 0)
                                goto rflag;
                        width = -width;
                        /* FALLTHROUGH */
                case '-':
                        flags |= LADJUST;
                        goto rflag;
                case '+':
                        sign = '+';
                        goto rflag;
                case '.':
                        if ((ch = *fmt++) == '*') {
                                x = va_arg(arg, int);
                                prec = x < 0 ? -1 : x;
                                goto rflag;
                        }
                        x = 0;
                        while (is_digit(ch)) {
                                x = 10 * x + to_digit(ch);
                                ch = *fmt++;
                        }
                        prec = x < 0 ? -1 : x;
                        goto reswitch;
                case '0':
                        /*
                         * ``Note that 0 is taken as a flag, not as the
                         * beginning of a field width.''
                         *      -- ANSI X3J11
                         */
                        flags |= ZEROPAD;
                        goto rflag;
                case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                        x = 0;
                        do {
                                x = 10 * x + to_digit(ch);
                                ch = *fmt++;
                        } while (is_digit(ch));
                        width = x;
                        goto reswitch;
#ifdef CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT
                case 'L':
                        flags |= LONGDBL;
                        goto rflag;
#endif
                case 'h':
                        flags |= SHORTINT;
                        goto rflag;
                case 'l':
                        if (*fmt == 'l') {
                                fmt++;
                                flags |= QUADINT;
                        } else {
                                flags |= LONGINT;
                        }
                        goto rflag;
                case 'q':
                        flags |= QUADINT;
                        goto rflag;
                case 'c':
                        *(cp = buf) = va_arg(arg, int);
                        size = 1;
                        sign = '\0';
                        break;
                case 'D':
                        flags |= LONGINT;
                        /*FALLTHROUGH*/
                case 'd':
                case 'i':
                        _uquad = SARG();
#ifndef _NO_LONGLONG
                        if ((quad_t)_uquad < 0)
#else
                        if ((long) _uquad < 0)
#endif
                        {

                                _uquad = -_uquad;
                                sign = '-';
                        }
                        base = DEC;
                        goto number;

#ifdef CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT
                case 'e':
                case 'E':
                case 'f':
                case 'g':
                case 'G':
                        _double = va_arg(arg, double);
                        /*
                         * don't do unrealistic precision; just pad it with
                         * zeroes later, so buffer size stays rational.
                         */
                        if (prec > MAXFRACT) {
                                if ((ch != 'g' && ch != 'G') || (flags&ALT))
                                        fpprec = prec - MAXFRACT;
                                prec = MAXFRACT;
                        } else if (prec == -1)
                                prec = DEFPREC;
                        /*
                         * cvt may have to round up before the "start" of
                         * its buffer, i.e. ``intf("%.2f", (double)9.999);'';
                         * if the first character is still NUL, it did.
                         * softsign avoids negative 0 if _double < 0 but
                         * no significant digits will be shown.
                         */
                        cp = buf;
                        *cp = '\0';
                        size = cvt(_double, prec, flags, &softsign, ch,
                            cp, buf + sizeof(buf));
                        if (softsign)
                                sign = '-';
                        if (*cp == '\0')
                                cp++;
                        break;
#else
                case 'e':
                case 'E':
                case 'f':
                case 'g':
                case 'G':
                        // Output nothing at all
                        (void) va_arg(arg, double); // take off arg anyway
                        cp = "";
                        size = 0;
                        sign = '\0';
                        break;
                        
                         
#endif // ifdef CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT

                case 'n':
#ifndef _NO_LONGLONG
                        if (flags & QUADINT)
                                *va_arg(arg, quad_t *) = ret;
                        else 
#endif
                        if (flags & LONGINT)
                                *va_arg(arg, long *) = ret;
                        else if (flags & SHORTINT)
                                *va_arg(arg, short *) = ret;
                        else
                                *va_arg(arg, int *) = ret;
                        continue;       /* no output */
                case 'O':
                        flags |= LONGINT;
                        /*FALLTHROUGH*/
                case 'o':
                        _uquad = UARG();
                        base = OCT;
                        goto nosign;
                case 'p':
                        /*
                         * ``The argument shall be a pointer to void.  The
                         * value of the pointer is converted to a sequence
                         * of printable characters, in an implementation-
                         * defined manner.''
                         *      -- ANSI X3J11
                         */
                        /* NOSTRICT */
                        _uquad = (unsigned long)va_arg(arg, void *);
                        base = HEX;
                        xdigs = "0123456789abcdef";
                        flags |= HEXPREFIX;
                        ch = 'x';
                        goto nosign;
                case 's':
                        if ((cp = va_arg(arg, char *)) == NULL)
                                cp = "(null)";
                        if (prec >= 0) {
                                /*
                                 * can't use strlen; can only look for the
                                 * NUL in the first `prec' characters, and
                                 * strlen() will go further.
                                 */
                                char *p = (char *)_memchr(cp, 0, prec);

                                if (p != NULL) {
                                        size = p - cp;
                                        if (size > prec)
                                                size = prec;
                                } else
                                        size = prec;
                        } else
                                size = _strlen(cp);
                        sign = '\0';
                        break;
                case 'U':
                        flags |= LONGINT;
                        /*FALLTHROUGH*/
                case 'u':
                        _uquad = UARG();
                        base = DEC;
                        goto nosign;
                case 'X':
                        xdigs = "0123456789ABCDEF";
                        goto hex;
                case 'x':
                        xdigs = "0123456789abcdef";
hex:                    _uquad = UARG();
                        base = HEX;
                        /* leading 0x/X only if non-zero */
                        if (flags & ALT && _uquad != 0)
                                flags |= HEXPREFIX;

                        /* unsigned conversions */
nosign:                 sign = '\0';
                        /*
                         * ``... diouXx conversions ... if a precision is
                         * specified, the 0 flag will be ignored.''
                         *      -- ANSI X3J11
                         */
number:                 if ((dprec = prec) >= 0)
                                flags &= ~ZEROPAD;

                        /*
                         * ``The result of converting a zero value with an
                         * explicit precision of zero is no characters.''
                         *      -- ANSI X3J11
                         */
                        cp = buf + BUF;
                        if (_uquad != 0 || prec != 0) {
                                /*
                                 * Unsigned mod is hard, and unsigned mod
                                 * by a constant is easier than that by
                                 * a variable; hence this switch.
                                 */
                                switch (base) {
                                case OCT:
                                        do {
                                                *--cp = to_char(_uquad & 7);
                                                _uquad >>= 3;
                                        } while (_uquad);
                                        /* handle octal leading 0 */
                                        if (flags & ALT && *cp != '0')
                                                *--cp = '0';
                                        break;

                                case DEC:
                                        /* many numbers are 1 digit */
                                        while (_uquad >= 10) {
                                                *--cp = to_char(_uquad % 10);
                                                _uquad /= 10;
                                        }
                                        *--cp = to_char(_uquad);
                                        break;

                                case HEX:
                                        do {
                                                *--cp = xdigs[_uquad & 15];
                                                _uquad >>= 4;
                                        } while (_uquad);
                                        break;

                                default:
                                        cp = "bug in vfprintf: bad base";
                                        size = _strlen(cp);
                                        goto skipsize;
                                }
                        }
                        size = buf + BUF - cp;
                skipsize:
                        break;
                default:        /* "%?" prints ?, unless ? is NUL */
                        if (ch == '\0')
                                goto done;
                        /* pretend it was %c with argument ch */
                        cp = buf;
                        *cp = ch;
                        size = 1;
                        sign = '\0';
                        break;
                }

                /*
                 * All reasonable formats wind up here.  At this point, `cp'
                 * points to a string which (if not flags&LADJUST) should be
                 * padded out to `width' places.  If flags&ZEROPAD, it should
                 * first be prefixed by any sign or other prefix; otherwise,
                 * it should be blank padded before the prefix is emitted.
                 * After any left-hand padding and prefixing, emit zeroes
                 * required by a decimal [diouxX] precision, then print the
                 * string proper, then emit zeroes required by any leftover
                 * floating precision; finally, if LADJUST, pad with blanks.
                 *
                 * Compute actual size, so we know how much to pad.
                 * fieldsz excludes decimal prec; realsz includes it.
                 */
#ifdef CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT
                fieldsz = size + fpprec;
#else
                fieldsz = size;
#endif
                if (sign)
                        fieldsz++;
                else if (flags & HEXPREFIX)
                        fieldsz+= 2;
                realsz = dprec > fieldsz ? dprec : fieldsz;

                /* right-adjusting blank padding */
                if ((flags & (LADJUST|ZEROPAD)) == 0)
                        PAD(width - realsz, blanks);

                /* prefix */
                if (sign) {
                        PRINT(&sign, 1);
                } else if (flags & HEXPREFIX) {
                        ox[0] = '0';
                        ox[1] = ch;
                        PRINT(ox, 2);
                }

                /* right-adjusting zero padding */
                if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD)
                        PAD(width - realsz, zeroes);

                /* leading zeroes from decimal precision */
                PAD(dprec - fieldsz, zeroes);

                /* the string or number proper */
                PRINT(cp, size);

#ifdef CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT
                /* trailing f.p. zeroes */
                PAD(fpprec, zeroes);
#endif

                /* left-adjusting padding (always blank) */
                if (flags & LADJUST)
                        PAD(width - realsz, blanks);

                /* finally, adjust ret */
                ret += width > realsz ? width : realsz;

                FLUSH();        /* copy out the I/O vectors */
        }
done:
        FLUSH();
error:
        return (((Cyg_StdioStream *) stream)->get_error() ? EOF : ret);
        /* NOTREACHED */
}


#ifdef CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT

static char *
round(double fract, int *exp, char *start, char *end, char ch, char *signp)
{
        double tmp;

        if (fract)
        (void)modf(fract * 10, &tmp);
        else
                tmp = to_digit(ch);
        if (tmp > 4)
                for (;; --end) {
                        if (*end == '.')
                                --end;
                        if (++*end <= '9')
                                break;
                        *end = '0';
                        if (end == start) {
                                if (exp) {      /* e/E; increment exponent */
                                        *end = '1';
                                        ++*exp;
                                }
                                else {          /* f; add extra digit */
                                *--end = '1';
                                --start;
                                }
                                break;
                        }
                }
        /* ``"%.3f", (double)-0.0004'' gives you a negative 0. */
        else if (*signp == '-')
                for (;; --end) {
                        if (*end == '.')
                                --end;
                        if (*end != '0')
                                break;
                        if (end == start)
                                *signp = 0;
                }
        return (start);
} // round()


static char *
exponent(char *p, int exp, int fmtch)
{
        char *t;
        char expbuf[MAXEXP];

        *p++ = fmtch;
        if (exp < 0) {
                exp = -exp;
                *p++ = '-';
        }
        else
                *p++ = '+';
        t = expbuf + MAXEXP;
        if (exp > 9) {
                do {
                        *--t = to_char(exp % 10);
                } while ((exp /= 10) > 9);
                *--t = to_char(exp);
                for (; t < expbuf + MAXEXP; *p++ = *t++);
        }
        else {
                *p++ = '0';
                *p++ = to_char(exp);
        }
        return (p);
} // exponent()


static int
cvt(double number, int prec, int flags, char *signp, int fmtch, char *startp,
    char *endp)
{
        char *p, *t;
        double fract;
        int dotrim, expcnt, gformat;
        double integer, tmp;
        Cyg_libm_ieee_double_shape_type ieeefp;

        dotrim = expcnt = gformat = 0;
        ieeefp.value = number;
        if ( ieeefp.number.sign ){  // this checks for <0.0 and -0.0
                number = -number;
                *signp = '-';
        } else
                *signp = 0;

        fract = modf(number, &integer);

        /* get an extra slot for rounding. */
        t = ++startp;

        /*
         * get integer portion of number; put into the end of the buffer; the
         * .01 is added for modf(356.0 / 10, &integer) returning .59999999...
         */
        for (p = endp - 1; integer; ++expcnt) {
                tmp = modf(integer / 10, &integer);
                *p-- = to_char((int)((tmp + .01) * 10));
        }
        switch (fmtch) {
        case 'f':
                /* reverse integer into beginning of buffer */
                if (expcnt)
                        for (; ++p < endp; *t++ = *p);
                else
                        *t++ = '0';
                /*
                 * if precision required or alternate flag set, add in a
                 * decimal point.
                 */
                if (prec || flags&ALT)
                        *t++ = '.';
                /* if requires more precision and some fraction left */
                if (fract) {
                        if (prec)
                                do {
                                        fract = modf(fract * 10, &tmp);
                                        *t++ = to_char((int)tmp);
                                } while (--prec && fract);
                        if (fract)
                                startp = round(fract, (int *)NULL, startp,
                                    t - 1, (char)0, signp);
                }
                for (; prec--; *t++ = '0');
                break;
        case 'e':
        case 'E':
eformat:        if (expcnt) {
                        *t++ = *++p;
                        if (prec || flags&ALT)
                                *t++ = '.';
                        /* if requires more precision and some integer left */
                        for (; prec && ++p < endp; --prec)
                                *t++ = *p;
                        /*
                         * if done precision and more of the integer component,
                         * round using it; adjust fract so we don't re-round
                         * later.
                         */
                        if (!prec && ++p < endp) {
                                fract = 0;
                                startp = round((double)0, &expcnt, startp,
                                    t - 1, *p, signp);
                        }
                        /* adjust expcnt for digit in front of decimal */
                        --expcnt;
                }
                /* until first fractional digit, decrement exponent */
                else if (fract) {
                        /* adjust expcnt for digit in front of decimal */
                        for (expcnt = -1;; --expcnt) {
                                fract = modf(fract * 10, &tmp);
                                if (tmp)
                                        break;
                        }
                        *t++ = to_char((int)tmp);
                        if (prec || flags&ALT)
                                *t++ = '.';
                }
                else {
                        *t++ = '0';
                        if (prec || flags&ALT)
                                *t++ = '.';
                }
                /* if requires more precision and some fraction left */
                if (fract) {
                        if (prec)
                                do {
                                        fract = modf(fract * 10, &tmp);
                                        *t++ = to_char((int)tmp);
                                } while (--prec && fract);
                        if (fract)
                                startp = round(fract, &expcnt, startp,
                                    t - 1, (char)0, signp);
                }
                /* if requires more precision */
                for (; prec--; *t++ = '0');

                /* unless alternate flag, trim any g/G format trailing 0's */
                if (gformat && !(flags&ALT)) {
                        while (t > startp && *--t == '0');
                        if (*t == '.')
                                --t;
                        ++t;
                }
                t = exponent(t, expcnt, fmtch);
                break;
        case 'g':
        case 'G':
                /* a precision of 0 is treated as a precision of 1. */
                if (!prec)
                        ++prec;
                /*
                 * ``The style used depends on the value converted; style e
                 * will be used only if the exponent resulting from the
                 * conversion is less than -4 or greater than the precision.''
                 *      -- ANSI X3J11
                 */
                if (expcnt > prec || (!expcnt && fract && fract < .0001)) {
                        /*
                         * g/G format counts "significant digits, not digits of
                         * precision; for the e/E format, this just causes an
                         * off-by-one problem, i.e. g/G considers the digit
                         * before the decimal point significant and e/E doesn't
                         * count it as precision.
                         */
                        --prec;
                        fmtch -= 2;             /* G->E, g->e */
                        gformat = 1;
                        goto eformat;
                }
                /*
                 * reverse integer into beginning of buffer,
                 * note, decrement precision
                 */
                if (expcnt)
                        for (; ++p < endp; *t++ = *p, --prec);
                else
                        *t++ = '0';
                /*
                 * if precision required or alternate flag set, add in a
                 * decimal point.  If no digits yet, add in leading 0.
                 */
                if (prec || flags&ALT) {
                        dotrim = 1;
                        *t++ = '.';
                }
                else
                        dotrim = 0;
                /* if requires more precision and some fraction left */
                if (fract) {
                        if (prec) {
                                do {
                                        fract = modf(fract * 10, &tmp);
                                        *t++ = to_char((int)tmp);
                                } while(!tmp);
                                while (--prec && fract) {
                                        fract = modf(fract * 10, &tmp);
                                        *t++ = to_char((int)tmp);
                                }
                        }
                        if (fract)
                                startp = round(fract, (int *)NULL, startp,
                                    t - 1, (char)0, signp);
                }
                /* alternate format, adds 0's for precision, else trim 0's */
                if (flags&ALT)
                        for (; prec--; *t++ = '0');
                else if (dotrim) {
                        while (t > startp && *--t == '0');
                        if (*t != '.')
                                ++t;
                }
        }
        return (t - startp);
} // cvt()

#endif // ifdef CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT

#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// EOF vfnprintf.cxx
