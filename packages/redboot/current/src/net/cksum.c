//==========================================================================
//
//      net/cksum.c
//
//      Stand-alone network checksum support for RedBoot
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

#include <net/net.h>

/*
 * Do a one's complement checksum.
 * The data being checksum'd is in network byte order.
 * The returned checksum is in network byte order.
 */
unsigned short
__sum(word *w, int len, int init_sum)
{
    int sum = init_sum;

    union {
	unsigned char c[2];
	unsigned short s;
    } su;

    union {
	unsigned short s[2];
	int i;
    } iu;

    while ((len -= 2) >= 0)
	sum += *w++;

    if (len == -1) {
	su.c[0] = *(char *)w;
	su.c[1] = 0;
	sum += su.s;
    }

    iu.i = sum;
    sum = iu.s[0] + iu.s[1];
    if (sum > 65535)
	sum -= 65535;

    su.s = ~sum;

    return (su.c[0] << 8) | su.c[1];
}


/*
 * Compute a partial checksum for the UDP/TCP pseudo header.
 */
int
__pseudo_sum(ip_header_t *ip)
{
    int    sum;
    word   *p;

    union {
	unsigned char c[2];
	unsigned short s;
    } su;
    
    p = (word *)ip->source;
    sum  = *p++;
    sum += *p++;
    sum += *p++;
    sum += *p++;
    
    su.c[0] = 0;
    su.c[1] = ip->protocol;
    sum += su.s;

    sum += ip->length;
    
    return sum;
}
