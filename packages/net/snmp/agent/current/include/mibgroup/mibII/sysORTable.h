//==========================================================================
//
//      ./agent/current/include/mibgroup/mibII/sysORTable.h
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
 *  Template MIB group interface - sysORTable.h
 *
 */
#ifndef _MIBGROUP_SYSORTABLE_H
#define _MIBGROUP_SYSORTABLE_H

config_require(util_funcs)

struct sysORTable {
   char *OR_descr;
   oid  *OR_oid;
   size_t  OR_oidlen;
   struct timeval OR_uptime;
   struct snmp_session *OR_sess;
   struct sysORTable *next;
};

struct register_sysOR_parameters {
   oid  *name;
   int   namelen;
   const char *descr;
};

extern void     init_sysORTable (void);
extern FindVarMethod var_sysORTable;
extern FindVarMethod var_sysORLastChange;
extern int      register_sysORTable (oid *, size_t, const char *);
extern int    unregister_sysORTable (oid *, size_t);
extern int      register_sysORTable_sess (oid *, size_t, const char *, struct snmp_session *);
extern int    unregister_sysORTable_sess (oid *, size_t, struct snmp_session *);
extern void   unregister_sysORTable_by_session (struct snmp_session *);

#define	SYSORTABLEINDEX		        1
#define	SYSORTABLEID		        2
#define	SYSORTABLEDESCR		        3
#define	SYSORTABLEUPTIME	        4

#define SYS_ORTABLE_REGISTERED_OK              0
#define SYS_ORTABLE_REGISTRATION_FAILED       -1
#define SYS_ORTABLE_UNREGISTERED_OK            0
#define SYS_ORTABLE_NO_SUCH_REGISTRATION      -1

#ifdef  USING_MIBII_SYSORTABLE_MODULE
#define REGISTER_SYSOR_ENTRY(theoid, descr)                      \
  (void)register_sysORTable(theoid, sizeof(theoid)/sizeof(oid), descr);
#define REGISTER_SYSOR_TABLE(theoid, len, descr)                      \
  (void)register_sysORTable(theoid, len, descr);

#else
#define REGISTER_SYSOR_ENTRY
#define REGISTER_SYSOR_TABLE
#endif /* USING_MIBII_SYSORTABLE_MODULE */
#endif /* _MIBGROUP_SYSORTABLE_H */
