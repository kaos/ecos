//==========================================================================
//
//      ./agent/current/include/mibgroup/mibII/snmp_mib.h
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
 *  SNMP MIB group interface - snmp.h
 *
 */
#ifndef _MIBGROUP_SNMP_H
#define _MIBGROUP_SNMP_H

struct variable;

extern FindVarMethod var_snmp;
extern WriteMethod write_snmp;

void init_snmp_mib(void);

extern int snmp_inpkts;			/*  1 - current */
extern int snmp_outpkts;		/*  2 - obsolete */
extern int snmp_inbadversions;		/*  3 - current */
extern int snmp_inbadcommunitynames;	/*  4 - current */
extern int snmp_inbadcommunityuses;	/*  5 - current */
extern int snmp_inasnparseerrors;	/*  6 - current */
extern int snmp_intoobigs;		/*  8 - obsolete */
extern int snmp_innosuchnames;		/*  9 - obsolete */
extern int snmp_inbadvalues;		/* 10 - obsolete */
extern int snmp_inreadonlys;		/* 11 - obsolete */
extern int snmp_ingenerrs;		/* 12 - obsolete */
extern int snmp_intotalreqvars;		/* 13 - obsolete */
extern int snmp_intotalsetvars;		/* 14 - obsolete */
extern int snmp_ingetrequests;		/* 15 - obsolete */
extern int snmp_ingetnexts;		/* 16 - obsolete */
extern int snmp_insetrequests;		/* 17 - obsolete */
extern int snmp_ingetresponses;		/* 18 - obsolete */
extern int snmp_intraps;		/* 19 - obsolete */
extern int snmp_outtoobigs;		/* 20 - obsolete */
extern int snmp_outnosuchnames;		/* 21 - obsolete */
extern int snmp_outbadvalues;		/* 22 - obsolete */
extern int snmp_outgenerrs;		/* 24 - obsolete */
extern int snmp_outgetrequests;		/* 25 - obsolete */
extern int snmp_outgetnexts;		/* 26 - obsolete */
extern int snmp_outsetrequests;		/* 27 - obsolete */
extern int snmp_outgetresponses;	/* 28 - obsolete */
extern int snmp_outtraps;		/* 29 - obsolete */
extern int snmp_enableauthentraps;	/* 30 - current */
extern int snmp_silentdrops;		/* 31 - current */
extern int snmp_proxydrops;		/* 32 - current */

extern char *snmp_trapsink;
extern char *snmp_trapcommunity;


#define SNMPINPKTS		1
#define SNMPOUTPKTS		2
#define SNMPINBADVERSIONS	3
#define SNMPINBADCOMMUNITYNAMES	4
#define SNMPINBADCOMMUNITYUSES	5
#define SNMPINASNPARSEERRORS	6
#define SNMPINBADTYPES		7
#define SNMPINTOOBIGS		8
#define SNMPINNOSUCHNAMES	9
#define SNMPINBADVALUES		10
#define SNMPINREADONLYS		11
#define SNMPINGENERRS		12
#define SNMPINTOTALREQVARS	13
#define SNMPINTOTALSETVARS	14
#define SNMPINGETREQUESTS	15
#define SNMPINGETNEXTS		16
#define SNMPINSETREQUESTS	17
#define SNMPINGETRESPONSES	18
#define SNMPINTRAPS		19
#define SNMPOUTTOOBIGS		20
#define SNMPOUTNOSUCHNAMES	21
#define SNMPOUTBADVALUES	22
#define SNMPOUTREADONLYS	23
#define SNMPOUTGENERRS		24
#define SNMPOUTGETREQUESTS	25
#define SNMPOUTGETNEXTS		26
#define SNMPOUTSETREQUESTS	27
#define SNMPOUTGETRESPONSES	28
#define SNMPOUTTRAPS		29
#define SNMPENABLEAUTHENTRAPS	30
#define SNMPSILENTDROPS		31
#define SNMPPROXYDROPS		32

#endif /* _MIBGROUP_SNMP_H */
