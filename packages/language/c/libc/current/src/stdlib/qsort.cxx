//===========================================================================
//
//      qsort.cxx
//
//      ANSI standard sorting function defined in section 7.10.5.2
//      of the standard
//
//===========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
 * Copyright (c) 1992, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
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

// Include the C library?
#ifdef CYGPKG_LIBC     

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_trac.h>    // Tracing support
#include <cyg/infra/cyg_ass.h>     // Assertion support
#include <stdlib.h>                // Header for all stdlib functions
                                   // (like this one)
#include "clibincl/stdlibsupp.hxx" // Support for stdlib functions

// TRACING

# if defined(CYGDBG_USE_TRACING) && defined(CYGNUM_LIBC_QSORT_TRACE_LEVEL)
static int qsort_trace = CYGNUM_LIBC_QSORT_TRACE_LEVEL;
#  define TL1 (0 < qsort_trace)
# else
#  define TL1 (0)
# endif


// EXPORTED SYMBOLS

externC void
qsort( void *base, size_t nmemb, size_t size, Cyg_comparison_fn_t compar ) \
    CYGPRI_LIBC_WEAK_ALIAS("_qsort");


// FUNCTION PROTOTYPES

static __inline__ char *med3(char *, char *, char *, Cyg_comparison_fn_t);
static __inline__ void swapfunc(char *, char *, int, int);


// MACRO FUNCTIONS

#define min(a, b)       ((a) < (b) ? (a) : (b))

//
// Qsort routine from Bentley & McIlroy's "Engineering a Sort Function".
//

#define swapcode(TYPE, parmi, parmj, n) do {            \
        long i = (n) / sizeof (TYPE);                   \
        TYPE *pi = (TYPE *) (parmi);                    \
        TYPE *pj = (TYPE *) (parmj);                    \
        do {                                            \
                TYPE   t = *pi;                         \
                *pi++ = *pj;                            \
                *pj++ = t;                              \
        } while (--i > 0);                              \
} while (0)

#define SWAPINIT(a, es) swaptype = ((char *)a - (char *)0) % sizeof(long) || \
        es % sizeof(long) ? 2 : es == sizeof(long)? 0 : 1;


#define swap(a, b)                                      \
        if (swaptype == 0) {                            \
                long t = *(long *)(a);                  \
                *(long *)(a) = *(long *)(b);            \
                *(long *)(b) = t;                       \
        } else                                          \
                swapfunc(a, b, size, swaptype)

#define vecswap(a, b, n)        if ((n) > 0) swapfunc(a, b, n, swaptype)


// FUNCTIONS

// Debug wrapper for comparison function
static __inline__ int
cmp_wrapper( Cyg_comparison_fn_t cmp, void *x, void *y )
{
    int retval;
    CYG_TRACE3( TL1, "Calling comparison function address %08x with args "
                "( %08x, %08x )", cmp, x, y );
    retval = cmp(x, y);

    CYG_TRACE1( TL1, "Comparison function returned %d", retval );

    return retval;
} // cmp_wrapper()

static __inline__ void
swapfunc( char *a, char *b, int n, int swaptype)
{
    if(swaptype <= 1) 
        swapcode(long, a, b, n);
    else
        swapcode(char, a, b, n);
} // swapfunc()


static __inline__ char *
med3( char *a, char *b, char *c, Cyg_comparison_fn_t cmp )
{
    return cmp_wrapper(cmp, a, b) < 0 ?
        (cmp_wrapper(cmp, b, c) < 0 ? b : (cmp_wrapper(cmp, a, c) < 0 ? c : a ))
        :(cmp_wrapper(cmp, b, c) > 0 ? b : (cmp_wrapper(cmp, a, c) < 0 ? a : c ));
} // med3()


void
_qsort( void *base, size_t nmemb, size_t size, Cyg_comparison_fn_t compar )
{
    char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
    int d, r, swaptype, swap_cnt;

    CYG_REPORT_FUNCNAME( "_qsort" );
    CYG_REPORT_FUNCARG4( "base=%08x, nmemb=%d, size=%d, compar=%08x",
                         base, nmemb, size, compar );

    CYG_CHECK_DATA_PTR( base, "base is not a valid pointer!" );
    CYG_CHECK_FUNC_PTR( compar, "compar is not a valid function pointer!" );
    
loop:   
    SWAPINIT(base, size);
    swap_cnt = 0;
    if (nmemb < 7) {
        for (pm = (char *) base + size;
             pm < (char *) base + nmemb * size;
             pm += size)
            for (pl = pm; pl > (char *) base && cmp_wrapper( compar, pl - size, pl) > 0;
                 pl -= size)
                swap(pl, pl - size);
        {
            CYG_REPORT_RETURN();
            return;
        } // for
    } // if
    pm = (char *) base + (nmemb / 2) * size;
    if (nmemb > 7) {
        pl = (char *) base;
        pn = (char *) base + (nmemb - 1) * size;
        if (nmemb > 40) {
            d = (nmemb / 8) * size;
            pl = med3(pl, pl + d, pl + 2 * d, compar);
            pm = med3(pm - d, pm, pm + d, compar);
            pn = med3(pn - 2 * d, pn - d, pn, compar);
        } // if
        pm = med3(pl, pm, pn, compar);
    } // if
    swap( (char *)base, pm );
    pa = pb = (char *) base + size;
    
    pc = pd = (char *) base + (nmemb - 1) * size;
    for (;;) {
        while (pb <= pc && (r = cmp_wrapper( compar, pb, base)) <= 0) {
            if (r == 0) {
                swap_cnt = 1;
                swap(pa, pb);
                pa += size;
            } // if
            pb += size;
        } // while
        while (pb <= pc && (r = cmp_wrapper( compar, pc, base)) >= 0) {
            if (r == 0) {
                swap_cnt = 1;
                swap(pc, pd);
                pd -= size;
            } // if
            pc -= size;
        } // while
        if (pb > pc)
            break;
        swap(pb, pc);
        swap_cnt = 1;
        pb += size;
        pc -= size;
    } // for
    if (swap_cnt == 0) {  // Switch to insertion sort
        for (pm = (char *) base + size;
             pm < (char *) base + nmemb * size;
             pm += size)
            for (pl = pm; pl > (char *) base && cmp_wrapper( compar, pl - size, pl) > 0; 
                 pl -= size)
                swap(pl, pl - size);
        {
            CYG_REPORT_RETURN();
            return;
        } // for
    } //if
    
    pn = (char *) base + nmemb * size;
    r = min(pa - (char *)base, pb - pa);
    vecswap((char *)base, pb - r, r);
    r = min( (unsigned)(pd - pc), pn - pd - size );
    vecswap(pb, pn - r, r);
    if ((unsigned)(r = pb - pa) > size)
        _qsort(base, r / size, size, compar);
    if ((unsigned)(r = pd - pc) > size) { 
        // Iterate rather than recurse to save stack space
        base = pn - r;
        nmemb = r / size;
        goto loop;
    } // if
/*              qsort(pn - r, r / size, size, compar);*/

    CYG_REPORT_RETURN();
} // _qsort()

#endif // ifdef CYGPKG_LIBC     

// EOF qsort.cxx
