#ifndef CYGONCE_POSIX_TYPES_H
#define CYGONCE_POSIX_TYPES_H
//=============================================================================
//
//      types.h
//
//      POSIX types header
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.      
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
// Purpose:       POSIX types header
// Description:   This header contains various POSIX type definitions. These types
//                are implementation defined.
//              
// Usage:         #include <sys/types.h>
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <pkgconf/posix.h>

#include <cyg/infra/cyg_type.h>

//-----------------------------------------------------------------------------
// Basic types.

typedef cyg_uint32 pthread_t;
typedef int pthread_key_t;
typedef int pthread_once_t;

//-----------------------------------------------------------------------------
// Scheduling parameters. At present only the priority is defined.
// Strictly this should be in <sched.h>, but the requirement for pthread_attr_t
// to contain a sched_param object means that it must be here.

struct sched_param
{
    int                 sched_priority;
};

//-----------------------------------------------------------------------------
// Thread attribute structure.

typedef struct pthread_attr_t
{
    unsigned int        detachstate:2,
                        scope:2,
                        inheritsched:2,
                        schedpolicy:2,
                        stackaddr_valid:1,
                        stacksize_valid:1;
    struct sched_param  schedparam;
    void                *stackaddr;
    size_t              stacksize;
} pthread_attr_t;

// Values for detachstate
#define PTHREAD_CREATE_JOINABLE	        1
#define PTHREAD_CREATE_DETACHED	        2

// Values for scope
#define PTHREAD_SCOPE_SYSTEM            1
#define PTHREAD_SCOPE_PROCESS           2

// Values for inheritsched
#define PTHREAD_INHERIT_SCHED           1
#define PTHREAD_EXPLICIT_SCHED          2

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_POSIX_TYPES_H
// End of types.h
