#ifndef CYGONCE_LIBC_LOCALE_H
#define CYGONCE_LIBC_LOCALE_H
//========================================================================
//
//      locale.h
//
//      Standard ISO C internationalisation (i18n) and localisation (i10n)
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1998-08-31
// Purpose:     
// Description: This implementation is designed to implement the contents
//              of ISO C section 7.4
// Usage:       #include <locale.h>
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <stddef.h>                // locale.h must provide NULL

// TYPE DEFINITIONS

// struct lconv contains information about numeric and monetary numbers
// and is described in the ISO C standard section 7.4

struct lconv {

    // the following affect formatted NONMONETARY QUANTITIES only
    char *decimal_point;    // decimal point
    char *thousands_sep;    // separates groups of digits before decimal
                            // point
    char *grouping;         // string whose elements indicate the size
                            // of each group of digits

    // the following affect formatted MONETARY QUANTITIES only
    char *int_curr_symbol;   // international curreny symbol
    char *currency_symbol;   // local currency symbol
    char *mon_decimal_point; // decimal point
    char *mon_thousands_sep; // separator for groups of digits
                             // before the decimal point
    char *mon_grouping;      // string whose elements indicate the size
                             // of each group of digits
    char *positive_sign;     // string to indicate zero or positive value
    char *negative_sign;     // string to indicate negative value
    char int_frac_digits;    // number of digits after decimal point for
                             // internationally formatted monetary nums
    char frac_digits;        // number of digits after decimal point for
                             // formatted monetary nums
    char p_cs_precedes;      // 1 if currency_symbol precedes non-negative
                             // monetary quantity. 0 if succeeds
    char p_sep_by_space;     // 1 if currency_symbol separated from value
                             // of non-negative monetary quantity by space.
                             // Otherwise 0.
    char n_cs_precedes;      // 1 if currency_symbol precedes negative
                             // monetary quantity. 0 if succeeds
    char n_sep_by_space;     // 1 if currency_symbol separated from value
                             // of negative monetary quantity by space.
                             // Otherwise 0.
    char p_sign_posn;        // set according to position of positive_sign
    char n_sign_posn;        // set according to position of negative_sign
};

// CONSTANTS

#define LC_COLLATE  (1<<0)
#define LC_CTYPE    (1<<1)
#define LC_MONETARY (1<<2)
#define LC_NUMERIC  (1<<3)
#define LC_TIME     (1<<4)
#define LC_ALL      (LC_COLLATE|LC_CTYPE|LC_MONETARY|LC_NUMERIC|LC_TIME)

// FUNCTION PROTOTYPES

externC char *
setlocale( int /* category */, const char * /* locale */ );

externC struct lconv *
localeconv( void );


#endif // ifdef CYGPKG_LIBC

#endif // CYGONCE_LIBC_LOCALE_H multiple inclusion protection

// EOF locale.h
