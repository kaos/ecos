//==========================================================================
//
//      lib/getserv.c
//
//      getservbyname(), getservbynumber()
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
//####BSDCOPYRIGHTBEGIN####
//
// -------------------------------------------
//
// Portions of this software may have been derived from OpenBSD or other sources,
// and are covered by the appropriate copyright disclaimers included herein.
//
// -------------------------------------------
//
//####BSDCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-01-10
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================


#include <sys/param.h>
#include <netdb.h>

static struct servent services[] = {
    { "ftp",      0, 21, "tcp" },
    { "ftp-data", 0, 20, "tcp" },
    { "tftp",     0, 69, "udp" },
    { NULL,       0,  0, NULL  }
};

struct servent *
getservbyname(const char *name, const char *proto)
{
    struct servent *p = services;
    while (p->s_name) {
        if ((strcmp(name, p->s_name) == 0) &&
            (strcmp(proto, p->s_proto) == 0)) {
            return p;
        }
        p++;
    }
    errno = ENOENT;
    return (struct servent *)0;
}

struct servent *
getservbynumber(const int num, const char *proto)
{
    struct servent *p = services;
    while (p->s_name) {
        if ((p->s_port == num) &&
            (strcmp(proto, p->s_proto) == 0)) {
            return p;
        }
        p++;
    }
    errno = ENOENT;
    return (struct servent *)0;
}
