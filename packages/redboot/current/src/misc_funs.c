//==========================================================================
//
//      misc_funs.c
//
//      Miscellaneous [library] functions for RedBoot
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

#include <redboot.h>

int
strlen(const char *s)
{
    int len = 0;
    if (s)
        while (*s++) len++;
    return len;
}

int
strcmp(const char *s1, const char *s2)
{
    char c1, c2;

    while ((c1 = *s1++) == (c2 = *s2++))
        if (c1 == 0)
            return (0);
    return ((unsigned char)c1 - (unsigned char)c2);
}

char *
strncpy(char *s1, const char *s2, unsigned long n)
{
    char c;
    char *_s1 = s1;

    if (n) {
        n--;                            // leave space for terminator
        while (n != 0 && (c = *s2++) != '\0') {
            *s1++ = c;
            n--;
        }
        *s1 = '\0';
        while (n != 0) {                // fill remainder with zero
            *s1++ = 0;
            n--;
        }
    }
    return _s1;
}

char *
strcpy(char *s1, const char *s2)
{
    char c;
    char *_s1 = s1;

    while ((c = *s2++) != '\0')
        *s1++ = c;
    *s1 = '\0';
    return _s1;
}

int
strcmpci(const char *s1, const char *s2)
{
    char c1, c2;
    while ((c1 = tolower(*s1++)) == (c2 = tolower(*s2++)))
        if (c1 == 0)
            return (0);
    return ((unsigned char)c1 - (unsigned char)c2);
}

int
strncmp(const char *s1, const char *s2, int len)
{
    char c1, c2;

    if (len == 0)
        return 0;
    do {
        if ((c1 = *s1++) != (c2 = *s2++))
            return ((unsigned char)c1 - (unsigned char)c2);
        if (c1 == 0)
            break;
    } while (--len != 0);
    return 0;
}

int
strncmpci(const char *s1, const char *s2, int len)
{
    char c1, c2;

    if (len == 0)
        return 0;
    do {
        if ((c1 = tolower(*s1++)) != (c2 = tolower(*s2++)))
            return ((unsigned char)c1 - (unsigned char)c2);
        if (c1 == 0)
            break;
    } while (--len != 0);
    return 0;
}

int
memcmp(const void *_dst, const void *_src, size_t len)
{
    unsigned char *dst = (unsigned char *)_dst;
    unsigned char *src = (unsigned char *)_src;
    while (len-- > 0) {
        if (*dst++ != *src++) {
            return (*--dst - *--src);
        }
    }
    return 0;
}
