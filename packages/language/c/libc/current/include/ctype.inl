#ifndef CYGONCE_LIBC_CTYPE_INL
#define CYGONCE_LIBC_CTYPE_INL
//===========================================================================
//
//      ctype.inl
//
//      Inline implementations of ANSI standard ctype routines defined in
//      section 7.3 of the standard
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
// Usage:       #include <ctype.h> - do not include this file directly
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

// INCLUDES

#include <ctype.h>              // Just to be sure, include the real header

// FUNCTIONS

//===========================================================================

// 7.3.1 Character testing functions


CYGPRI_LIBC_INLINE int
isupper( int c )
{
    return (('A' <= c) && (c <= 'Z'));
} // isupper()


CYGPRI_LIBC_INLINE int
islower( int c )
{
    return (('a' <= c) && (c <= 'z'));
} // islower()


CYGPRI_LIBC_INLINE int
isalpha( int c )
{
    return ( islower(c) || isupper(c) );
} // isalpha()


CYGPRI_LIBC_INLINE int
isdigit( int c )
{
    return ( ('0' <= c) && (c <= '9') );
} // isdigit()


CYGPRI_LIBC_INLINE int
isalnum( int c )
{
    return ( isalpha(c) || isdigit(c) );
} // isalnum()


CYGPRI_LIBC_INLINE int
iscntrl( int c )
{
    // Simple standard 7-bit ASCII map is assumed
    return ( ((0 <= c) && (c <= 0x1F)) ||
             (c == 0x7F) );
} // iscntrl()


CYGPRI_LIBC_INLINE int
isgraph( int c )
{
    // Simple standard 7-bit ASCII map is assumed
    return ( ('!' <= c) && (c <= '~') );
} // isgraph()


CYGPRI_LIBC_INLINE int
isprint( int c )
{
    // Simple standard 7-bit ASCII map is assumed
    return ( (' ' <= c) && (c <= '~') );
} // isprint()


CYGPRI_LIBC_INLINE int
ispunct( int c )
{
    // Simple standard 7-bit ASCII map is assumed
    return ( (('!' <= c) && (c <= '/')) ||   // ASCII 0x21 - 0x2F
             ((':' <= c) && (c <= '@')) ||   // ASCII 0x3A - 0x40
             (('[' <= c) && (c <= '`')) ||   // ASCII 0x5B - 0x60
             (('{' <= c) && (c <= '~')) );   // ASCII 0x7B - 0x7E
             
} // ispunct()


CYGPRI_LIBC_INLINE int
isspace( int c )
{
    return ( (c == ' ') || (c == '\f') || (c == '\n') || (c == '\r') ||
             (c == '\t') || (c == '\v') );
} // isspace()


CYGPRI_LIBC_INLINE int
isxdigit( int c )
{
    return ( isdigit(c) ||
             (('a' <= c) && (c <= 'f')) ||
             (('A' <= c) && (c <= 'F')) );
} // isxdigit()

//===========================================================================

// 7.3.2 Character case mapping functions


CYGPRI_LIBC_INLINE int
tolower( int c )
{
    return isupper(c) ? c - 'A' + 'a' : c;
} // tolower()


CYGPRI_LIBC_INLINE int
toupper( int c )
{
    return islower(c) ? c - 'a' + 'A' : c;
} // toupper()


#endif // ifdef CYGPKG_LIBC     

#endif // CYGONCE_LIBC_CTYPE_INL multiple inclusion protection

// EOF ctype.inl
