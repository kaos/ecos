//==========================================================================
//
//      sched.cxx
//
//      POSIX scheduler API implementation
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           nickg
// Contributors:        nickg
// Date:                2000-03-27
// Purpose:             POSIX scheduler API implementation
// Description:         This file contains the implementation of the POSIX scheduler
//                      functions.
//              
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <pkgconf/posix.h>

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros

#include "pprivate.h"                   // POSIX private header

#include <cyg/kernel/sched.hxx>        // scheduler definitions
#include <cyg/kernel/thread.hxx>       // thread definitions

#include <cyg/kernel/sched.inl>        // scheduler inlines
#include <cyg/kernel/thread.inl>       // thread inlines

//==========================================================================
// Process scheduling functions.

//--------------------------------------------------------------------------
// Set scheduling parameters for given process.

int sched_setparam (pid_t pid, const struct sched_param *param)
{
    CYG_REPORT_FUNCTYPE( "returning %d" );

    if( pid != 0 )
    {
        errno = ESRCH;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }

    errno = ENOSYS;
    CYG_REPORT_RETVAL( -1 );
    return -1;
}

//--------------------------------------------------------------------------
// Get scheduling parameters for given process.

int sched_getparam (pid_t pid, struct sched_param *param)
{
    CYG_REPORT_FUNCTYPE( "returning %d" );

    if( pid != 0 )
    {
        errno = ESRCH;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }
    
    errno = ENOSYS;
    CYG_REPORT_RETVAL( -1 );
    return -1;
}

//--------------------------------------------------------------------------
// Set scheduling policy and/or parameters for given process.
int sched_setscheduler (pid_t pid,
                        int policy,
                        const struct sched_param *param)
{
    CYG_REPORT_FUNCTYPE( "returning %d" );

    if( pid != 0 )
    {
        errno = ESRCH;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }

    errno = ENOSYS;
    CYG_REPORT_RETVAL( -1 );
    return -1;
}
    

//--------------------------------------------------------------------------
// Get scheduling policy for given process.

int sched_getscheduler (pid_t pid)
{
    CYG_REPORT_FUNCTYPE( "returning %d" );

    if( pid != 0 )
    {
        errno = ESRCH;
        CYG_REPORT_RETVAL( 0 );
        return -1;
    }
    
    errno = ENOSYS;
    CYG_REPORT_RETVAL( -1 );
    return -1;
}    

//--------------------------------------------------------------------------
// Force current thread to relinquish the processor.

int sched_yield (void)
{
    CYG_REPORT_FUNCTYPE( "returning %d" );

    Cyg_Thread::yield();
    
    CYG_REPORT_RETVAL( 0 );
    return 0;
}
   

//==========================================================================
// Scheduler parameter limits.

//--------------------------------------------------------------------------
// Get maximum priority value for a policy.

int sched_get_priority_max (int policy)
{
    CYG_REPORT_FUNCTYPE( "returning %d" );

    if( policy != SCHED_FIFO &&
        policy != SCHED_RR &&
        policy != SCHED_OTHER )
    {
        errno = EINVAL;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }
    
    int pri = PTHREAD_POSIX_PRIORITY( CYG_THREAD_MAX_PRIORITY );
    
    CYG_REPORT_RETVAL( pri );
    return pri;
}    

//--------------------------------------------------------------------------
// Get minimum priority value for a policy.

int sched_get_priority_min (int policy)
{
    CYG_REPORT_FUNCTYPE( "returning %d" );

    if( policy != SCHED_FIFO &&
        policy != SCHED_RR &&
        policy != SCHED_OTHER )
    {
        errno = EINVAL;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }

    int pri = PTHREAD_POSIX_PRIORITY( CYG_THREAD_MIN_PRIORITY );
    
    CYG_REPORT_RETVAL( pri );
    return pri;
}    

//--------------------------------------------------------------------------
// Get the SCHED_RR interval for the given process.

int sched_rr_get_interval (pid_t pid, struct timespec *t)
{
    CYG_REPORT_FUNCTYPE( "returning %d" );

    if( pid != 0 )
    {
        errno = ESRCH;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }
    
    cyg_ticks_to_timespec( CYGNUM_KERNEL_SCHED_TIMESLICE_TICKS, t );
    
    CYG_REPORT_RETVAL( 0 );
    return 0;
} 

// -------------------------------------------------------------------------
// EOF sched.cxx
