//==========================================================================
//
//      ./agent/current/include/usmUser.h
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
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
// Author(s):    Andrew.Lunn@ascom.ch, Manu.Sharma@ascom.ch
// Contributors: hmt
// Date:         2001-05-29
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
 * usmUser.h
 *
 */

#ifndef _MIBGROUP_USMUSER_H
#define _MIBGROUP_USMUSER_H

#include "callback.h"
#include "snmpusm.h"

/* <...prefix>.<engineID_length>.<engineID>.<user_name_length>.<user_name>
   = 1 + 32 + 1 + 32 */
#define USM_LENGTH_OID_MAX	66

/* we use header_generic and checkmib from the util_funcs module */

config_require(util_funcs)
config_add_mib(SNMP-USER-BASED-SM-MIB)

/* Magic number definitions: */

#define   USMUSERSPINLOCK       1
#define   USMUSERSECURITYNAME   2
#define   USMUSERCLONEFROM      3
#define   USMUSERAUTHPROTOCOL   4
#define   USMUSERAUTHKEYCHANGE  5
#define   USMUSEROWNAUTHKEYCHANGE  6
#define   USMUSERPRIVPROTOCOL   7
#define   USMUSERPRIVKEYCHANGE  8
#define   USMUSEROWNPRIVKEYCHANGE  9
#define   USMUSERPUBLIC         10
#define   USMUSERSTORAGETYPE    11
#define   USMUSERSTATUS         12

/* function definitions */

extern void   init_usmUser(void);
extern FindVarMethod var_usmUser;

void shutdown_usmUser(void);
int store_usmUser(int majorID, int minorID, void *serverarg, void *clientarg);
oid *usm_generate_OID(oid *prefix, size_t prefixLen, struct usmUser *uptr,
                  size_t *length);
int usm_parse_oid(oid *oidIndex, size_t oidLen,
              unsigned char **engineID, size_t *engineIDLen,
              unsigned char **name, size_t *nameLen);

WriteMethod write_usmUserSpinLock;
WriteMethod write_usmUserCloneFrom;
WriteMethod write_usmUserAuthProtocol;
WriteMethod write_usmUserAuthKeyChange;
WriteMethod write_usmUserPrivProtocol;
WriteMethod write_usmUserPrivKeyChange;
WriteMethod write_usmUserPublic;
WriteMethod write_usmUserStorageType;
WriteMethod write_usmUserStatus;

#endif /* _MIBGROUP_USMUSER_H */
