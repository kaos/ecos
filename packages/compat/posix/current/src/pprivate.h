#ifndef CYGONCE_PPRIVATE_H
#define CYGONCE_PPRIVATE_H
//=============================================================================
//
//      pprivate.h
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
// Purpose:       POSIX private header
// Description:   This header contains various POSIX type definitions that are
//                shared between the various parts of the POSIX package.
//              
// Usage:         #include <pprivate.h>
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <pkgconf/posix.h>

#include <stddef.h>                     // NULL, size_t

#include <sys/types.h>
#include <sched.h>
#include <pthread.h>
#include <errno.h>                      // error codes
#include <signal.h>                     // sigset_t
#include <limits.h>                     // PTHREAD_KEYS_MAX

#include <cyg/posix/export.h>           // POSIX exports header

#include <cyg/kernel/thread.hxx>        // thread definitions
#include <cyg/kernel/mutex.hxx>         // mutex definitions

//=============================================================================
// Constructor prioritization

// Prioritization for POSIX library support objects
#define CYGBLD_POSIX_INIT CYGBLD_ATTRIB_INIT_PRI(CYG_INIT_COMPAT)

// Prioritization for POSIX library startup initialization. This must
// come after CYGBLD_POSIX_INIT constructors.
#define CYGBLD_POSIX_START CYGBLD_ATTRIB_INIT_PRI(CYG_INIT_COMPAT+5)

//=============================================================================
// Thread control data structure

// Per-thread information needed by POSIX
// This is pointed to by the CYGNUM_KERNEL_THREADS_DATA_POSIX entry of the
// per-thread data.

typedef struct
{
    unsigned int        state:4,                // Thread state
                        cancelstate:2,          // Cancel state of thread
                        canceltype:2,           // Cancel type of thread
                        cancelpending:1,        // pending cancel flag
                        freestack:1;            // stack malloced, must be freed

    pthread_t           id;                     // My thread ID
    Cyg_Thread          *thread;                // pointer to eCos thread object
    pthread_attr_t      attr;                   // Current thread attributes
    void                *retval;                // return value
    void                *(*start_routine)(void *); // start routine
    void                *start_arg;             // argument to start routine
    char                name[20];               // name string for debugging
    Cyg_Condition_Variable *joiner;             // joining threads wait here
    CYG_ADDRWORD        stackmem;               // base of stack memory area
                                                // only valid if freestack == true

    struct pthread_cleanup_buffer *cancelbuffer; // stack of cleanup buffers

    sigset_t            sigpending;     // Set of pending signals
    sigset_t            sigmask;        // Thread's signal mask
    
    // The following is space for the eCos thread object that underlies
    // this POSIX thread. It is allocated like this to avoid constructing
    // it on startup.
    cyg_uint8           thread_obj[sizeof(Cyg_Thread)];

    // And the same for the joiner condition variable.
    cyg_uint8           joiner_obj[sizeof(Cyg_Condition_Variable)];

    // Per-thread data table pointer
    void                **thread_data;
    
} pthread_info;


// Values for the state field. These are solely concerned with the
// states visible to POSIX. The thread's run state is stored in the
// eCos thread object.
// Note: numerical order here is important, do not rearrange.

#define PTHREAD_STATE_FREE      0       // This structure is free for reuse
#define PTHREAD_STATE_DETACHED  1       // The thread is running but detached
#define PTHREAD_STATE_RUNNING   2       // The thread is running and will wait
                                        // to join when it exits
#define PTHREAD_STATE_JOIN      3       // The thread has exited and is waiting
                                        // to be joined
#define PTHREAD_STATE_EXITED    4       // The thread has exited and is ready to
                                        // be reaped
//-----------------------------------------------------------------------------
// Priority translation.
// eCos priorities run from 0 as the highest to 31 as the lowest. POSIX priorities
// run in the opposite direction. The following macros translate between the two
// priority ranges.

#define PTHREAD_ECOS_PRIORITY(pri) (CYG_THREAD_MIN_PRIORITY-(pri))

#define PTHREAD_POSIX_PRIORITY(pri) (CYG_THREAD_MIN_PRIORITY-(pri))

//-----------------------------------------------------------------------------
// Global data structures

// Mutex for locking access to pthread_info structures
extern Cyg_Mutex pthread_mutex;

//-----------------------------------------------------------------------------
// Functions exported by pthread.cxx to the other parts of the POSIX subsystem.

externC void cyg_posix_pthread_start( void );

externC pthread_info *pthread_self_info(void);

externC pthread_info *pthread_info_id( pthread_t id );

externC void cyg_posix_pthread_release_thread( sigset_t *mask );

//-----------------------------------------------------------------------------
// Functions exported by signal.cxx to the other parts of the POSIX subsystem.

externC void cyg_posix_signal_start();

externC void cyg_posix_signal_asr(pthread_info *self);

externC cyg_bool cyg_sigqueue( const struct sigevent *sev, int code,
                               pthread_info *thread = NULL );

externC cyg_bool cyg_deliver_signals();
    
externC void cyg_posix_thread_siginit( pthread_info *thread,
                                       pthread_info *parentthread );

externC void cyg_posix_thread_sigdestroy( pthread_info *thread );

//-----------------------------------------------------------------------------
// Functions exported by time.cxx to other parts of the POSIX subsystem.

externC void cyg_posix_clock_start();

externC cyg_tick_count cyg_timespec_to_ticks( const struct timespec *tp,
                                         cyg_bool roundup = false);

externC void cyg_ticks_to_timespec( cyg_tick_count ticks, struct timespec *tp );

externC void cyg_posix_timer_asr( pthread_info *self );

//-----------------------------------------------------------------------------
// Functions exported by except.cxx

externC void cyg_posix_exception_start();

externC void cyg_pthread_exception_init(pthread_info *thread);

externC void cyg_pthread_exception_destroy(pthread_info *thread);

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_PPRIVATE_H
// End of pprivate.h
