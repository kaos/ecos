//==========================================================================
//
//      ./agent/current/include/m2m.h
//
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
//####UCDSNMPCOPYRIGHTBEGIN####
//
// -------------------------------------------
//
// Portions of this software may have been derived from the UCD-SNMP
// project,  <http://ucd-snmp.ucdavis.edu/>  from the University of
// California at Davis, which was originally based on the Carnegie Mellon
// University SNMP implementation.  Portions of this software are therefore
// covered by the appropriate copyright disclaimers included herein.
//
// The release used was version 4.1.2 of May 2000.  "ucd-snmp-4.1.2"
// -------------------------------------------
//
//####UCDSNMPCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    hmt
// Contributors: hmt
// Date:         2000-05-30
// Purpose:      Port of UCD-SNMP distribution to eCos.
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================
/********************************************************************
       Copyright 1989, 1991, 1992 by Carnegie Mellon University

			  Derivative Work -
Copyright 1996, 1998, 1999, 2000 The Regents of the University of California

			 All Rights Reserved

Permission to use, copy, modify and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appears in all copies and
that both that copyright notice and this permission notice appear in
supporting documentation, and that the name of CMU and The Regents of
the University of California not be used in advertising or publicity
pertaining to distribution of the software without specific written
permission.

CMU AND THE REGENTS OF THE UNIVERSITY OF CALIFORNIA DISCLAIM ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL CMU OR
THE REGENTS OF THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR ANY SPECIAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
FROM THE LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*********************************************************************/
/*
 * m2m.h
 */

struct get_req_state {
    int type;
    void *info;
};

/* values for type field in get_req_state */
#define ALARM_GET_REQ	1
#define EVENT_GET_REQ	2

/* the following define is used to document a routine or variable which
** is not static to a module.
*/
#define Export

/* values for EntryStatus */
#define ENTRY_ACTIVE		1
#define ENTRY_NOTINSERVICE	2
#define ENTRY_NOTREADY		3
#define ENTRY_CREATEANDGO	4
#define ENTRY_CREATEANDWAIT	5
#define ENTRY_DESTROY		6

/* maximum length for an OwnerString variable */
#define MAX_OWNER_STR_LEN 128

/* maximum length for a description field */
#define MAX_DESCRIPTION_LEN 128

/* defines for noting whether the incoming packet is unicast, broadcast,
** or multicast
*/
#define PKT_UNICAST 0
#define PKT_BROADCAST 1
#define PKT_MULTICAST 2

/* macro to compare two ethernet addresses.  addr1 is a pointer to a
** struct ether_addr; addr2 is just a struct ether_addr.
*/
#define sameEtherAddr(addr1, addr2) \
	((*((short *)((addr1)->ether_addr_octet)) == \
					*((short *)((addr2).ether_addr_octet))) &&\
	 (*((short *)(((addr1)->ether_addr_octet) + 2)) == \
					*((short *)((((addr2).ether_addr_octet) + 2)))) &&\
	 (*((short *)(((addr1)->ether_addr_octet) + 4)) == \
					*((short *)((((addr2).ether_addr_octet) + 4)))))
