//==========================================================================
//
//      ./agent/current/src/mibgroup/snmpv3/snmpEngine.c
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
/* snmpEngine.c: implements the SNMP-FRAMEWORK-MIB. */

#include <config.h>

#if HAVE_WINSOCK_H
#include <winsock.h>
#endif

#include "mibincl.h"
#include "snmpv3.h"
//#include "util_funcs.h"
#include "lcd_time.h"
#include "mibgroup/mibII/sysORTable.h"
#include "snmpEngine.h"

struct variable2 snmpEngine_variables[] = {
  { SNMPENGINEID        , ASN_OCTET_STR  , RONLY , var_snmpEngine, 1, { 1 } },
#ifdef SNMP_TESTING_CODE 
  { SNMPENGINEBOOTS     , ASN_INTEGER    , RWRITE, var_snmpEngine, 1, { 2 } },
  { SNMPENGINETIME      , ASN_INTEGER    , RWRITE, var_snmpEngine, 1, { 3 } },
#else /* !SNMP_TESTING_CODE */ 
  { SNMPENGINEBOOTS     , ASN_INTEGER    , RONLY , var_snmpEngine, 1, { 2 } },
  { SNMPENGINETIME      , ASN_INTEGER    , RONLY , var_snmpEngine, 1, { 3 } },
#endif /* SNMP_TESTING_CODE */
  { SNMPENGINEMAXMESSAGESIZE, ASN_INTEGER, RONLY , var_snmpEngine, 1, { 4 } },
};

/* now load this mib into the agents mib table */
oid snmpEngine_variables_oid[] = {1,3,6,1,6,3,10,2,1};

void init_snmpEngine (void) {
#ifdef USING_MIBII_SYSORTABLE_MODULE
  static oid reg[] = {1,3,6,1,6,3,10,3,1,1};
  register_sysORTable(reg,10,"The SNMP Management Architecture MIB.");
#endif

  REGISTER_MIB("snmpv3/snmpEngine", snmpEngine_variables, variable2,
		 snmpEngine_variables_oid);

/* place any initialization routines needed here */
}

extern struct timeval starttime;

#ifdef SNMP_TESTING_CODE
int write_engineBoots(int, u_char *,u_char, size_t, u_char *,oid*, size_t);
int write_engineTime(int, u_char *,u_char, size_t, u_char *,oid*, size_t);
#endif /* SNMP_TESTING_CODE */

u_char *
var_snmpEngine(
    struct variable *vp,
    oid     *name,
    size_t  *length,
    int     exact,
    size_t  *var_len,
    WriteMethod **write_method)
{

  /* variables we may use later */
  static long long_ret;
  static unsigned char engineID[SNMP_MAXBUF];

  *write_method = 0;           /* assume it isnt writable for the time being */
  *var_len = sizeof(long_ret); /* assume an integer and change later if not */

  if (header_generic(vp,name,length,exact,var_len,write_method))
      return 0;

  /* this is where we do the value assignments for the mib results. */
  switch(vp->magic) {

    case SNMPENGINEID:
      *var_len = snmpv3_get_engineID(engineID, SNMP_MAXBUF);
      /* XXX  Set ERROR_MSG() upon error? */
      return (unsigned char *) engineID;

    case SNMPENGINEBOOTS:
#ifdef SNMP_TESTING_CODE
      *write_method = write_engineBoots;
#endif /* SNMP_TESTING_CODE */
      long_ret = snmpv3_local_snmpEngineBoots();
      return (unsigned char *) &long_ret;

    case SNMPENGINETIME:
#ifdef SNMP_TESTING_CODE
      *write_method = write_engineTime;
#endif /* SNMP_TESTING_CODE */
      long_ret = snmpv3_local_snmpEngineTime();
      return (unsigned char *) &long_ret;

    case SNMPENGINEMAXMESSAGESIZE:
      long_ret = 1500;
      return (unsigned char *) &long_ret;

    default:
      DEBUGMSGTL(("snmpd", "unknown sub-id %d in var_snmpEngine\n", vp->magic));
  }
  return 0;
}

#ifdef SNMP_TESTING_CODE
/* write_engineBoots():

   This is technically not writable a writable mib object, but we
   allow it so we can run some time synchronization tests.
*/
int
write_engineBoots(
   int      action,
   u_char   *var_val,
   u_char   var_val_type,
   size_t   var_val_len,
   u_char   *statP,
   oid      *name,
   size_t   name_len)
{
  /* variables we may use later */
  static long long_ret;
  size_t size;
  int bigsize=SNMP_MAXBUF_MEDIUM;
  u_char engineIDBuf[SNMP_MAXBUF_MEDIUM];
  int engineIDBufLen = 0;

  if (var_val_type != ASN_INTEGER){
      DEBUGMSGTL(("snmpEngine","write to engineBoots not ASN_INTEGER\n"));
      return SNMP_ERR_WRONGTYPE;
  }
  if (var_val_len > sizeof(long_ret)){
      DEBUGMSGTL(("snmpEngine","write to engineBoots: bad length\n"));
      return SNMP_ERR_WRONGLENGTH;
  }
  long_ret = *((long *) var_val);
  if (action == COMMIT) {
    engineIDBufLen = snmpv3_get_engineID(engineIDBuf, SNMP_MAXBUF_MEDIUM);
    /* set our local engineTime in the LCD timing cache */
    snmpv3_set_engineBootsAndTime(long_ret, snmpv3_local_snmpEngineTime());
    set_enginetime(engineIDBuf, engineIDBufLen, 
                   snmpv3_local_snmpEngineBoots(), 
                   snmpv3_local_snmpEngineTime(),
                   TRUE);
  }
  return SNMP_ERR_NOERROR;
}

/* write_engineTime():

   This is technically not writable a writable mib object, but we
   allow it so we can run some time synchronization tests.
*/
int
write_engineTime(
   int      action,
   u_char   *var_val,
   u_char   var_val_type,
   size_t   var_val_len,
   u_char   *statP,
   oid      *name,
   size_t   name_len)
{
  /* variables we may use later */
  static long long_ret;
  size_t size;
  int bigsize=SNMP_MAXBUF_MEDIUM;
  u_char engineIDBuf[SNMP_MAXBUF_MEDIUM];
  int engineIDBufLen = 0;

  if (var_val_type != ASN_INTEGER){
      DEBUGMSGTL(("snmpEngine","write to engineTime not ASN_INTEGER\n"));
      return SNMP_ERR_WRONGTYPE;
  }
  if (var_val_len > sizeof(long_ret)){
      DEBUGMSGTL(("snmpEngine","write to engineTime: bad length\n"));
      return SNMP_ERR_WRONGLENGTH;
  }
  long_ret = *((long *) var_val);
  if (action == COMMIT) {
    engineIDBufLen = snmpv3_get_engineID(engineIDBuf, SNMP_MAXBUF_MEDIUM);
    /* set our local engineTime in the LCD timing cache */
    snmpv3_set_engineBootsAndTime(snmpv3_local_snmpEngineBoots(), long_ret);
    set_enginetime(engineIDBuf, engineIDBufLen, 
                   snmpv3_local_snmpEngineBoots(), 
                   snmpv3_local_snmpEngineTime(),
                   TRUE);
  }
  return SNMP_ERR_NOERROR;
}

#endif /* SNMP_TESTING_CODE */
