//==========================================================================
//
//      net/inet_addr.c
//
//      Stand-alone IP address parsing for RedBoot
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

#include <redboot.h>
#include <net/net.h>

bool
inet_aton(char *s, in_addr_t *addr)
{
    int i, val, radix, digit;
    unsigned long res = 0;
    bool first;
    char c;
    
    for (i = 0;  i < 4;  i++) {
        // Parse next digit string
        first = true;
        val = 0;
        radix = 10;
        while ((c = *s++) != '\0') {
            if (first && (c == '0') && (_tolower(*s) == 'x')) {
                radix = 16;
                s++;  // Skip over 0x
                c = *s++;
            }
            first = false;
            if (_is_hex(c) && ((digit = _from_hex(c)) < radix)) {
                // Valid digit
                val = (val * radix) + digit;
            } else if (c == '.') {
                break;
            } else {
                return false;
            }
        }
        // merge result
#ifdef __LITTLE_ENDIAN__
        res |= val << ((3-i)*8);  // 24, 16, 8, 0
#else
        res = (res << 8) | val;
#endif
    }
    addr->s_addr = htonl(res);
    return true;
}

// Assumes address is in network byte order
char *
inet_ntoa(in_addr_t *addr)
{
    static char str[32];
    unsigned char *ap;

    ap = (unsigned char *)addr;
    diag_sprintf(str, "%d.%d.%d.%d", ap[0], ap[1], ap[2], ap[3]);
    return str;
}
