#ifndef CYGONCE_POSIX_MUTTYPES_H
#define CYGONCE_POSIX_MUTTYPES_H
//=============================================================================
//
//      muttypes.h
//
//      POSIX mutex types header
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
// Contributors:  nickg,jlarmour
// Date:          2000-03-17
// Purpose:       POSIX types header
// Description:   This header contains POSIX type definitions for mutexes
//                and cond vars. These types are implementation defined.
//              
// Usage:         #include <sys/types.h>
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/posix.h>
#include <pkgconf/kernel.h>

#include <cyg/infra/cyg_type.h>

//-----------------------------------------------------------------------------
// Mutex object
// This structure must exactly correspond in size and layout to the underlying
// eCos C++ class that implements this object. Because we have to support
// PTHREAD_MUTEX_INITIALIZER we cannot abstract this object very easily.

typedef struct
{
    CYG_WORD32  locked;
    CYG_ADDRESS owner;
    CYG_ADDRESS queue;

#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_DYNAMIC
    CYG_WORD32  protocol;       // this mutex's protocol
#endif    
    
#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_CEILING
    CYG_WORD32  ceiling;        // mutex priority ceiling
#endif
    
} pthread_mutex_t;

#if defined(CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_DYNAMIC) &&\
    defined(CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_CEILING)
#define PTHREAD_MUTEX_INITIALIZER { 0, 0, 0, 0, 0 }
#elif defined(CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_DYNAMIC) ||\
    defined(CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_CEILING)
#define PTHREAD_MUTEX_INITIALIZER { 0, 0, 0, 0 }
#else
#define PTHREAD_MUTEX_INITIALIZER { 0, 0, 0 }
#endif

//-----------------------------------------------------------------------------
// Mutex attributes structure

typedef struct
{
    int         protocol;
#ifdef _POSIX_THREAD_PRIO_PROTECT    
    int         prioceiling;
#endif    
} pthread_mutexattr_t;

// Values for protocol
#define PTHREAD_PRIO_NONE       1
#if defined(_POSIX_THREAD_PRIO_INHERIT)
#define PTHREAD_PRIO_INHERIT    2
#endif
#if defined(_POSIX_THREAD_PRIO_PROTECT)
#define PTHREAD_PRIO_PROTECT    3
#endif

//-----------------------------------------------------------------------------
// Condition Variable structure.
// Like mutexes, this must match the underlying eCos implementation class.

typedef struct
{
    CYG_ADDRESS         mutex;
    CYG_ADDRESS         queue;    
} pthread_cond_t;

#define PTHREAD_COND_INITIALIZER { 0, 0 }

//-----------------------------------------------------------------------------
// Condition variable attributes structure

typedef struct
{
    int         dummy;
} pthread_condattr_t;

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_POSIX_MUTTYPES_H
// End of muttypes.h
