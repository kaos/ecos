#ifndef CYGONCE_LIBC_I18N_INTERNAL_H
#define CYGONCE_LIBC_I18N_INTERNAL_H

/*===========================================================================
//
//      internal.h
//
//      Internal header file to support the i18n locale functions.
//
//===========================================================================
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
// The Original Code is under the newlib licence.
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
// Author(s):    jjohnstn
// Contributors:
// Date:         2000-11-16
// Purpose:
// Description:
// Usage:        This file is used internally by i18n functions.
//
//####DESCRIPTIONEND####
//
//=========================================================================*/


/* escape character used for JIS encoding */
#define ESC_CHAR 0x1b

/* PROTOTYPES */

typedef int (*mbtowc_fn_type)(wchar_t *pwc, const char *s, size_t n, int *state);
typedef int (*wctomb_fn_type)(char *s, wchar_t wchar, int *state );

extern int __mbtowc_jp ( wchar_t *__pwc, const char *__s, size_t __n, int *__state );
extern int __mbtowc_c  ( wchar_t *__pwc, const char *__s, size_t __n, int *__state );
extern int __wctomb_jp ( char *__s, wchar_t __wchar, int *__state );

/* MACROS used to support SHIFT_JIS, EUC-JP, and JIS multibyte encodings */

#define _issjis1(c)    (((c) >= 0x81 && (c) <= 0x9f) || ((c) >= 0xe0 && (c) <= 0xef))
#define _issjis2(c)    (((c) >= 0x40 && (c) <= 0x7e) || ((c) >= 0x80 && (c) <= 0xfc))
#define _iseucjp(c)    ((c) >= 0xa1 && (c) <= 0xfe)
#define _isjis(c)      ((c) >= 0x21 && (c) <= 0x7e)

// TYPE DEFINITIONS

// define a type to encapsulate the locale. In time this will get much
// richer and complete

typedef struct {
  const char *name;
  struct lconv numdata;
  int mb_cur_max;
  /* if next two fields are NULL, it implies the default "C" single-byte handling. */
  mbtowc_fn_type mbtowc_fn;
  wctomb_fn_type wctomb_fn;
} Cyg_libc_locale_t;

// ctype locale pointer shared between setlocale and mb routines
extern const Cyg_libc_locale_t *__current_ctype_locale;

#endif /* CYGONCE_LIBC_I18N_INTERNAL_H */

