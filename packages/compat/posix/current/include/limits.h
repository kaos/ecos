#ifndef CYGONCE_LIMITS_H
#define CYGONCE_LIMITS_H
//=============================================================================
//
//      limits.h
//
//      POSIX limits header
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     nickg
// Contributors:  nickg
// Date:          2000-03-17
// Purpose:       POSIX limits header
// Description:   This file contains the compile time definitions that describe
//                the minima and current values of various values and defined
//                in POSIX.1 section 2.8.
//              
// Usage:
//              #include <limits.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <pkgconf/posix.h>

//-----------------------------------------------------------------------------
// Runtime invariants
// These are all equal to or greater than the minimum values defined above.

// From table 2-4

#define NGROUPS_MAX             _POSIX_NGROUPS_MAX

// From table 2-5

#define AIO_LISTIO_MAX          _POSIX_AIO_LISTIO_MAX

#define AIO_MAX                 _POSIX_AIO_MAX

#define AIO_PRIO_DELTA_MAX      0

#define ARG_MAX                 _POSIX_ARG_MAX

#define CHILD_MAX               _POSIX_CHILD_MAX

#define DELAYTIMER_MAX          _POSIX_DELAYTIMER_MAX

#define LOGIN_NAME_MAX          _POSIX_LOGIN_NAME_MAX

#define PAGESIZE                1

#define PTHREAD_DESTRUCTOR_ITERATIONS CYGNUM_POSIX_PTHREAD_DESTRUCTOR_ITERATIONS

#define PTHREAD_KEYS_MAX        CYGNUM_POSIX_PTHREAD_KEYS_MAX

// Minimum size needed on a pthread stack to contain its per-thread control
// structures and per-thread data. Since we cannot include all the headers
// necessary to calculate this value here, the following is a generous estimate.
#define PTHREAD_STACK_OVERHEAD  (0x180+(PTHREAD_KEYS_MAX*sizeof(void *)))

#define PTHREAD_STACK_MIN       (CYGNUM_HAL_STACK_SIZE_MINIMUM+PTHREAD_STACK_OVERHEAD)

#define PTHREAD_THREADS_MAX     CYGNUM_POSIX_PTHREAD_THREADS_MAX

#define RTSIG_MAX               _POSIX_RTSIG_MAX

#define SEM_NSEMS_MAX           _POSIX_SEM_NSEMS_MAX

#define SEM_VALUE_MAX           _POSIX_SEM_VALUE_MAX

#define SIGQUEUE_MAX            _POSIX_SIGQUEUE_MAX

#define STREAM_MAX              _POSIX_STREAM_MAX

#define TIMER_MAX               _POSIX_TIMER_MAX

#define TTY_NAME_MAX            _POSIX_TTY_NAME_MAX

#define TZNAME_MAX              _POSIX_TZNAME_MAX


// From table 2-6.

#define MAX_CANON               _POSIX_MAX_CANON

#define MAX_INPUT               _POSIX_MAX_INPUT

#define PIPE_BUF                _POSIX_PIPE_BUF


//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_LIMITS_H
// End of limits.h
