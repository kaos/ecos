#ifndef CYGONCE_ISO_LOCALE_H
#define CYGONCE_ISO_LOCALE_H
/*========================================================================
//
//      locale.h
//
//      ISO locale functions
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
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-04-14
// Purpose:       This file provides the locale functions required by 
//                ISO C and POSIX 1003.1.
// Description:   The real contents of this file get set from the
//                configuration (set by the implementation)
// Usage:         #include <locale.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

/* CONFIGURATION */

#include <pkgconf/isoinfra.h>          /* Configuration header */

/* INCLUDES */

/* This is the "standard" way to get NULL from stddef.h,
 * which is the canonical location of the definitions.
 */
#define __need_NULL
#include <stddef.h>

#if CYGINT_ISO_LOCALE
# ifdef CYGBLD_ISO_LOCALE_HEADER
#  include CYGBLD_ISO_LOCALE_HEADER
# else

/* TYPE DEFINITIONS */

/* struct lconv contains information about numeric and monetary numbers
 * and is described in the ISO C standard section 7.4 */

struct lconv {

    /* the following affect formatted NONMONETARY QUANTITIES only */
    char *decimal_point;    /* decimal point                                */
    char *thousands_sep;    /* separates groups of digits before decimal
                               point                                        */
    char *grouping;         /* string whose elements indicate the size      */
                            /* of each group of digits                      */

    /* the following affect formatted MONETARY QUANTITIES only              */
    char *int_curr_symbol;   /* international curreny symbol                */
    char *currency_symbol;   /* local currency symbol                       */
    char *mon_decimal_point; /* decimal point                               */
    char *mon_thousands_sep; /* separator for groups of digits
                                before the decimal point                    */
    char *mon_grouping;      /* string whose elements indicate the size
                                of each group of digits                     */
    char *positive_sign;     /* string to indicate zero or positive value   */
    char *negative_sign;     /* string to indicate negative value           */
    char int_frac_digits;    /* number of digits after decimal point for
                                internationally formatted monetary nums     */
    char frac_digits;        /* number of digits after decimal point for
                                formatted monetary nums                     */
    char p_cs_precedes;      /* 1 if currency_symbol precedes non-negative
                                monetary quantity. 0 if succeeds            */
    char p_sep_by_space;     /* 1 if currency_symbol separated from value 
                                of non-negative monetary quantity by space.
                                Otherwise 0.                                */
    char n_cs_precedes;      /* 1 if currency_symbol precedes negative
                                monetary quantity. 0 if succeeds            */
    char n_sep_by_space;     /* 1 if currency_symbol separated from value
                                of negative monetary quantity by space.
                                Otherwise 0.                                */
    char p_sign_posn;        /* set according to position of positive_sign  */
    char n_sign_posn;        /* set according to position of negative_sign  */
};

/* CONSTANTS */

#define LC_COLLATE  (1<<0)
#define LC_CTYPE    (1<<1)
#define LC_MONETARY (1<<2)
#define LC_NUMERIC  (1<<3)
#define LC_TIME     (1<<4)
#define LC_ALL      (LC_COLLATE|LC_CTYPE|LC_MONETARY|LC_NUMERIC|LC_TIME)

/* FUNCTION PROTOTYPES */

#ifdef __cplusplus
extern "C" {
#endif

extern char *
setlocale( int /* category */, const char * /* locale */ );

extern struct lconv *
localeconv( void );

#ifdef __cplusplus
} /* extern "C" */
#endif 

# endif /* #elif !defined(CYGBLD_ISO_LOCALE_HEADER) */
#endif /* CYGINT_ISO_LOCALE */

#endif /* CYGONCE_ISO_LOCALE_H multiple inclusion protection */

/* EOF locale.h */
