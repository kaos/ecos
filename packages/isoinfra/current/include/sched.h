#ifndef CYGONCE_ISO_SCHED_H
#define CYGONCE_ISO_SCHED_H
/*========================================================================
//
//      sched.h
//
//      POSIX scheduler functions
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-04-14
// Purpose:       This file provides the scheduler macros, types and functions
//                required by POSIX 1003.1.
// Description:   The real contents of this file get set from the
//                configuration (set by the implementation)
// Usage:         #include <sched.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

/* CONFIGURATION */

#include <pkgconf/isoinfra.h>          /* Configuration header */

/* INCLUDES */

#include <time.h>

#if CYGINT_ISO_SCHED_IMPL
# ifdef CYGBLD_ISO_SCHED_IMPL_HEADER
#  include CYGBLD_ISO_SCHED_IMPL_HEADER
# else

//-----------------------------------------------------------------------------
// Scheduling Policys

#define SCHED_OTHER	        1
#define SCHED_FIFO	        2
#define SCHED_RR	        3

//-----------------------------------------------------------------------------
//Process scheduling functions.

#ifdef __cplusplus
extern "C" {
#endif

// Set scheduling parameters for given process.
extern int sched_setparam (pid_t pid, const struct sched_param *param);

// Get scheduling parameters for given process.
extern int sched_getparam (pid_t pid, struct sched_param *param);

// Set scheduling policy and/or parameters for given process.
extern int sched_setscheduler (pid_t pid,
                               int policy,
                               const struct sched_param *param);

// Get scheduling policy for given process.
extern int sched_getscheduler (pid_t pid);

// Force current thread to relinquish the processor.
extern int sched_yield (void);

//-----------------------------------------------------------------------------
// Scheduler parameter limits.

// Get maximum priority value for a policy.
extern int sched_get_priority_max (int policy);

// Get minimum priority value for a policy.
extern int sched_get_priority_min (int policy);

// Get the SCHED_RR interval for the given process.
extern int sched_rr_get_interval (pid_t pid, struct timespec *t);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif 

#endif /* if CYGINT_ISO_SCHED_IMPL */

#endif /* CYGONCE_ISO_SCHED_H multiple inclusion protection */

/* EOF sched.h */
