//==========================================================================
//
//      ./lib/current/include/mt_support.h
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

/* mt_support.h - multi-thread resource locking support declarations */
/*
 * Author: Markku Laukkanen
 * Created: 6-Sep-1999
 * History:
 *  8-Sep-1999 M. Slifcak method names changed;
 *                        use array of resource locking structures.
 */

#ifndef MT_SUPPORT_H
#define MT_SUPPORT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Lock group identifiers */

#define MT_LIBRARY_ID      0
#define MT_APPLICATION_ID  1
#define MT_TOKEN_ID        2

#define MT_MAX_IDS         3  /* one greater than last from above */
#define MT_MAX_SUBIDS      10


/* Lock resource identifiers for library resources */

#define MT_LIB_NONE        0
#define MT_LIB_SESSION     1
#define MT_LIB_REQUESTID   2
#define MT_LIB_MESSAGEID   3
#define MT_LIB_SESSIONID   4
#define MT_LIB_TRANSID     5

#define MT_LIB_MAXIMUM     6  /* must be one greater than the last one */


#ifdef _REENTRANT

#if HAVE_PTHREAD_H

#include <pthread.h>
typedef pthread_mutex_t mutex_type;
#ifdef pthread_mutexattr_default
    #define MT_MUTEX_INIT_DEFAULT pthread_mutexattr_default
#else
    #define MT_MUTEX_INIT_DEFAULT 0
#endif

#elif defined(WIN32) || defined(cygwin)

#include <windows.h>
typedef CRITICAL_SECTION  mutex_type;

#else
  error "There is no re-entrant support as defined."
#endif

int snmp_res_init(void);
int snmp_res_lock(int groupID, int resourceID);
int snmp_res_unlock(int groupID, int resourceID);
int snmp_res_destroy_mutex(int groupID, int resourceID);

#else  /* !_REENTRANT */

#define snmp_res_init() do {} while (0)
#define snmp_res_lock(x,y) do {} while (0)
#define snmp_res_unlock(x,y) do {} while (0)
#define snmp_res_destroy_mutex(x,y) do {} while (0)

#endif /* !_REENTRANT */

#ifdef __cplusplus
}
#endif

#endif /* MT_SUPPORT_H */

