//==========================================================================
//
//      ./lib/current/include/snmpv3.h
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
 * snmpv3.h
 */

#ifndef SNMPV3_H
#define SNMPV3_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_ENGINEID_LENGTH 128

int     setup_engineID(u_char **eidp, const char *text);
void    engineID_conf(const char *word, char *cptr);
void    engineBoots_conf(const char *, char *);
void    snmpv3_authtype_conf(const char *word, char *cptr);
void    snmpv3_privtype_conf(const char *word, char *cptr);
void	usm_parse_create_usmUser(const char *token, char *line);
void    init_snmpv3(const char *);
int	init_snmpv3_post_config(int majorid, int minorid, void *serverarg,
                                void *clientarg);
void    shutdown_snmpv3(const char *type);
int     snmpv3_store(int majorID, int minorID, void *serverarg,
                     void *clientarg);
u_long  snmpv3_local_snmpEngineBoots(void);
int     snmpv3_clone_engineID(u_char **, size_t* , u_char*, size_t);
int     snmpv3_get_engineID(u_char *buf, size_t buflen);
u_char *snmpv3_generate_engineID(int *);
u_long  snmpv3_local_snmpEngineTime(void);
int     get_default_secLevel(void);
oid    *get_default_authtype(size_t *);
oid    *get_default_privtype(size_t *);
void    snmpv3_set_engineBootsAndTime(int boots, int ttime); 

#ifdef __cplusplus
}
#endif

#endif /* SNMPV3_H */
