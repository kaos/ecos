//========================================================================
//
//      locale.cxx
//
//      Implementation of ISO C internationalisation (i18n) locales
//
//========================================================================
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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour@cygnus.com
// Contributors:  jlarmour@cygnus.co.uk
// Date:        1998-08-31
// Purpose:     
// Description: 
// Usage:       
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
#include <cyg/infra/cyg_trac.h>    // Common tracing support
#include <cyg/infra/cyg_ass.h>     // Common assertion support
#include <locale.h>                // struct lconv
#include <string.h>                // several string functions
#include <limits.h>                // CHAR_MAX
#include "clibincl/i18nsupp.hxx"   // Support for i18n functions

// SYMBOLS

externC char *
setlocale( int, const char * ) CYGPRI_LIBC_WEAK_ALIAS("_setlocale");

externC struct lconv *
localeconv( void ) CYGPRI_LIBC_WEAK_ALIAS("_localeconv");

// TYPE DEFINITIONS

// define a type to encapsulate the locale. In time this will get much
// richer and complete

typedef struct {
    
    const char *name;
    struct lconv numdata;

} Cyg_libc_locale_t;


// CONSTANTS

// define the "C" locale
static const Cyg_libc_locale_t
C_locale = { "C",
             { ".", "", "", "", "", "", "", "", "", "",
               CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX,
               CHAR_MAX, CHAR_MAX
             }
           };

// only one locale now, but leave room for expansion
static const Cyg_libc_locale_t *all_locales[] = { &C_locale };

// GLOBALS

// the current locales. Our default is the C locale
static const Cyg_libc_locale_t *current_collate_locale  = &C_locale;
static const Cyg_libc_locale_t *current_ctype_locale    = &C_locale;
static const Cyg_libc_locale_t *current_monetary_locale = &C_locale;
static const Cyg_libc_locale_t *current_numeric_locale  = &C_locale;
static const Cyg_libc_locale_t *current_time_locale     = &C_locale;

// FUNCTIONS

static const Cyg_libc_locale_t *
find_locale_data( const char *locale_str, cyg_ucount32 checklen )
{
    CYG_REPORT_FUNCNAMETYPE( "find_locale_data", "returning %08x" );
    CYG_REPORT_FUNCARG1( "locale_str=%s", locale_str );

    const Cyg_libc_locale_t *temp_locale, *curr_locale=NULL;
    cyg_ucount32 i;

    // is it "" i.e. use the default?
    if (*locale_str=='\0') {
        curr_locale = &C_locale;
        CYG_REPORT_RETVAL( curr_locale );
        return curr_locale;
    } // if

    for (i=0; i<sizeof(all_locales)/sizeof(Cyg_libc_locale_t *); i++ ) {

        temp_locale = all_locales[i];

        if ( !strncmp(temp_locale->name, locale_str, checklen) )
            curr_locale = temp_locale;
    } // for

    CYG_REPORT_RETVAL( curr_locale );
    return curr_locale;
} // find_locale_data()


externC char *
_setlocale( int category, const char *locale )
{
    CYG_REPORT_FUNCNAMETYPE("_setlocale", "returning %08x");
    CYG_REPORT_FUNCARG2( "category=%d, locale=%s", category, locale );

    if (locale != NULL)
        CYG_CHECK_DATA_PTR( locale, "locale pointer is invalid!" );

    const char *str;

    // special case if locale==NULL, return current locale name
    if (locale==NULL) {

        CYG_TRACE0( true, "Getting current locale value" );

        switch (category) {

        case LC_COLLATE:
            str = current_collate_locale->name;
            break;
        case LC_CTYPE:
            str = current_ctype_locale->name;
            break;
        case LC_MONETARY:
            str = current_monetary_locale->name;
            break;
        case LC_NUMERIC:
            str = current_numeric_locale->name;
            break;
        case LC_TIME:
            str = current_time_locale->name;
            break;
        case LC_ALL:

            // create static string to give a constructed string back
            // to the user. The size is the number of categories other
            // than LC_ALL times the maximum name size, and add a constant
            // for the delimiting octothorpes
            static char my_str[ CYGNUM_LIBC_MAX_LOCALE_NAME_SIZE*5+10 ];

            strcpy( my_str, "#" );
            strcat( my_str, current_collate_locale->name );
            strcat( my_str, "#" );
            strcat( my_str, current_ctype_locale->name );
            strcat( my_str, "#" );
            strcat( my_str, current_monetary_locale->name );
            strcat( my_str, "#" );
            strcat( my_str, current_numeric_locale->name );
            strcat( my_str, "#" );
            strcat( my_str, current_time_locale->name );
            strcat( my_str, "#" );

            str = &my_str[0];
            break;
        default:
            str=NULL;
            CYG_FAIL("setlocale() passed bad category!" );
            break;

        } // switch

        CYG_REPORT_RETVAL( (char *)str);
        return (char *)str;
    } // if
        
    // we only get here if locale is non-NULL, i.e. we want to set it

    const Cyg_libc_locale_t *loc;
    cyg_bool default_locale = (*locale=='\0');

    CYG_TRACE0( true, "Setting current locale value" );

    switch( category ) {
    case LC_COLLATE:
        loc = find_locale_data( locale, CYGNUM_LIBC_MAX_LOCALE_NAME_SIZE );
        if (loc != NULL)      // found it
            current_collate_locale=loc;
        break;
        
    case LC_CTYPE:
        loc = find_locale_data( locale, CYGNUM_LIBC_MAX_LOCALE_NAME_SIZE );
        if (loc != NULL)      // found it
            current_ctype_locale=loc;
        break;
        
    case LC_MONETARY:
        loc = find_locale_data( locale, CYGNUM_LIBC_MAX_LOCALE_NAME_SIZE );
        if (loc != NULL)      // found it
            current_monetary_locale=loc;
        break;
        
    case LC_NUMERIC:
        loc = find_locale_data( locale, CYGNUM_LIBC_MAX_LOCALE_NAME_SIZE );
        if (loc != NULL)      // found it
            current_numeric_locale=loc;
        break;
        
    case LC_TIME:
        loc = find_locale_data( locale, CYGNUM_LIBC_MAX_LOCALE_NAME_SIZE );
        if (loc != NULL)      // found it
            current_time_locale=loc;
        break;
        
    case LC_ALL:
        // first try and match it exactly
        loc = find_locale_data( locale, CYGNUM_LIBC_MAX_LOCALE_NAME_SIZE );
        if (loc != NULL) {     // found it

            CYG_TRACE0(true, "Matched locale string exactly");
            current_collate_locale = current_ctype_locale = loc;
            current_monetary_locale = current_numeric_locale = loc;
            current_time_locale = loc;
        } // if
        else {
            CYG_TRACE0( true, "Attempting to parse string previously "
                        "returned from setlocale()" );
            // now try and see if it is a compound string returned
            // earlier by setlocale( LC_ALL, NULL );

            // Note we don't do much checking here. This could be
            // much more rigorous (but at the expense of speed/size?)

            const Cyg_libc_locale_t *temp_collate_locale,
                *temp_ctype_locale, *temp_monetary_locale,
                *temp_numeric_locale, *temp_time_locale;

            cyg_ucount32 token_len;

            str = &locale[0];
            if ( *str=='#' ) {
                ++str;
                token_len = strcspn( str, "#" );
                loc = find_locale_data( str, token_len );
                if (loc!=NULL) {
                    temp_collate_locale=loc;
                    str += token_len+1;
                    token_len = strcspn( str, "#" );
                    loc = find_locale_data( str, token_len );
                } // if
                if (loc!=NULL) {
                    temp_ctype_locale=loc;
                    str += token_len+1;
                    token_len = strcspn( str, "#" );
                    loc = find_locale_data( str, token_len );
                } // if
                if (loc!=NULL) {
                    temp_monetary_locale=loc;
                    str += token_len+1;
                    token_len = strcspn( str, "#" );
                    loc = find_locale_data( str, token_len );
                } // if
                if (loc!=NULL) {
                    temp_numeric_locale=loc;
                    str += token_len+1;
                    token_len = strcspn( str, "#" );
                    loc = find_locale_data( str, token_len );
                } // if
                if (loc!=NULL) {
                    temp_time_locale=loc;
                    str += token_len+1;
                    token_len = strcspn( str, "#" );
                    loc = find_locale_data( str, token_len );
                } // if
                if (loc!=NULL) {
                    // if we've got this far and loc still isn't NULL,
                    // then everything's fine, and we've matched everything

                    current_collate_locale = temp_collate_locale;
                    current_ctype_locale = temp_ctype_locale;
                    current_monetary_locale = temp_monetary_locale;
                    current_numeric_locale = temp_numeric_locale;
                    current_time_locale = temp_time_locale;

                } // if
            } // if
            
        } // else
        break; // case LC_ALL
        
    default:
            CYG_FAIL("setlocale() passed bad category!" );
            loc=NULL;
            break;
    } // switch

    if (loc==NULL) {
        CYG_REPORT_RETVAL( NULL );
        return NULL;
    } // if
    else if (default_locale==true) {
        CYG_REPORT_RETVAL(C_locale.name);
        return (char *)C_locale.name;
    } // else if
    else {
        CYG_REPORT_RETVAL(locale);
        return (char *)locale;
    } // else
}

externC struct lconv *
_localeconv( void )
{
    CYG_REPORT_FUNCNAMETYPE( "_localeconv", "returning %08x" );
    CYG_REPORT_FUNCARGVOID();

    static struct lconv static_lconv;

    static_lconv.decimal_point = 
        current_numeric_locale->numdata.decimal_point;

    static_lconv.thousands_sep = 
        current_numeric_locale->numdata.thousands_sep;

    static_lconv.grouping = 
        current_numeric_locale->numdata.grouping;

    // we cheat a bit, but it should be worth it - a lot of these are
    // constants which optimise nicely
    cyg_ucount32 size_used;
    size_used = (char *)&static_lconv.int_curr_symbol -
                (char *)&static_lconv;

    memcpy( &(static_lconv.int_curr_symbol),
            &(current_monetary_locale->numdata.int_curr_symbol),
            sizeof(struct lconv) - size_used );
            

    CYG_REPORT_RETVAL( &static_lconv );
    return &static_lconv;
}


#endif // ifdef CYGPKG_LIBC     

// EOF locale.cxx
