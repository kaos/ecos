//==========================================================================
//
//      select.cxx
//
//      Fileio select() support
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
// Date:                2000-05-25
// Purpose:             Fileio select() support
// Description:         Support for select().
//                      
//              
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <pkgconf/io_fileio.h>

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros

#include <stdarg.h>                    // for fcntl()

#include "fio.h"                       // Private header

#include <sys/select.h>                // select header

#include <cyg/kernel/sched.hxx>        // scheduler definitions
#include <cyg/kernel/thread.hxx>       // thread definitions
#include <cyg/kernel/mutex.hxx>        // mutex definitions
#include <cyg/kernel/clock.hxx>        // clock definitions

#include <cyg/kernel/sched.inl>
#include <cyg/kernel/thread.inl>
#include <cyg/kernel/clock.inl>

//==========================================================================
// File object locking

#define LOCK_FILE( fp ) cyg_file_lock( fp )

#define UNLOCK_FILE( fp ) cyg_file_unlock( fp )

//==========================================================================
// Local variables

// Mutex for serializing select processing. This essntially controls
// access to the contents of the selinfo structures embedded in the
// client system data structures.
static Cyg_Mutex select_mutex CYGBLD_ATTRIB_INIT_PRI(CYG_INIT_IO);

// Condition variable where any thread that is waiting for a select to
// fire is suspended. Note that select is not intended to be a real time
// operation. Whenever any selectable event occurs, all selecting threads
// will be resumed. They must then rescan their selectees and resuspend if
// necessary.
static Cyg_Condition_Variable selwait( select_mutex ) CYGBLD_ATTRIB_INIT_PRI(CYG_INIT_IO);

static volatile cyg_uint32 selwake_count = 0;

//==========================================================================
// Timeval to ticks conversion support

// Converters from sec and us to ticks
static struct Cyg_Clock::converter us_converter, sec_converter;

static cyg_bool converters_initialized = false;

externC cyg_tick_count cyg_timeval_to_ticks( const struct timeval *tv )
{
    if( !converters_initialized )
    {
        // Create the converters we need.
        Cyg_Clock::real_time_clock->get_other_to_clock_converter( 1000, &us_converter );
        Cyg_Clock::real_time_clock->get_other_to_clock_converter( 1000000000, &sec_converter );

        converters_initialized = true;
    }
    
    // Short circuit zero timeval
    if( tv->tv_sec == 0 && tv->tv_usec == 0 )
    {
        return 0;
    }
        
    // Convert the seconds field to ticks.
    cyg_tick_count ticks = Cyg_Clock::convert( tv->tv_sec, &sec_converter );

    // Convert the nanoseconds. This will round down to nearest whole tick.
    ticks += Cyg_Clock::convert( (cyg_tick_count)tv->tv_usec, &us_converter );

    return ticks;
}

//==========================================================================
// Select API function

__externC int
select(int nfd, fd_set *in, fd_set *out, fd_set *ex, struct timeval *tv)
{
    FILEIO_ENTRY();

    int error = ENOERR;
    int fd, mode, num;
    cyg_file *fp;
    fd_set in_res, out_res, ex_res;  // Result sets
    fd_set *selection[3], *result[3];
    cyg_tick_count ticks;
    int mode_type[] = {CYG_FREAD, CYG_FWRITE, 0};
    cyg_uint32 wake_count;
    
    FD_ZERO(&in_res);
    FD_ZERO(&out_res);
    FD_ZERO(&ex_res);

    // Set up sets
    selection[0] = in;   result[0] = &in_res;
    selection[1] = out;  result[1] = &out_res;
    selection[2] = ex;   result[2] = &ex_res;

    // Compute end time
    if (tv)
        ticks = Cyg_Clock::real_time_clock->current_value() +
            cyg_timeval_to_ticks( tv );
    else ticks = 0;

    // Lock the mutex
    select_mutex.lock();

    // Scan sets for possible I/O until something found, timeout or error.
    while (!error)
    {
        wake_count = selwake_count;
        
        num = 0;  // Total file descriptors "ready"
        for (mode = 0;  !error && mode < 3;  mode++)
        {
            if (selection[mode]) {
                for (fd = 0;  !error && fd < nfd;  fd++)
                {
                    if (FD_ISSET(fd, selection[mode]))
                    {
                        fp = cyg_fp_get( fd );
                        if( fp == NULL )
                        {
                            error = EBADF;
                            break;
                        }

                        if ((*fp->f_ops->fo_select)(fp, mode_type[mode], 0))
                        {
                            FD_SET(fd, result[mode]);
                            num++;
                        }

                        cyg_fp_free( fp );
                    }
                }
            }
        }
        
        if (num)
        {
            // Found something, update user's sets
            if (in)  FD_COPY( &in_res, in );
            if (out) FD_COPY( &out_res, out );
            if (ex)  FD_COPY( &ex_res, ex );
            select_mutex.unlock();
            FILEIO_RETURN_VALUE(num);
        }

        Cyg_Scheduler::lock();

        if( wake_count == selwake_count )
        {
            // Nothing found, see if we want to wait
            if (tv)
            {
                if (ticks == 0)
                {
                    // Special case of "poll"
                    select_mutex.unlock();
                    Cyg_Scheduler::unlock();
                    FILEIO_RETURN_VALUE(0);
                }

                if( !selwait.wait( ticks ) )
                {
                    // A non-standard wakeup, if the current time is equal to
                    // or past the timeout, return zero. Otherwise return
                    // EINTR, since we have been released.
                    if( Cyg_Clock::real_time_clock->current_value() >= ticks )
                    {
                        select_mutex.unlock();
                        Cyg_Scheduler::unlock();
                        FILEIO_RETURN_VALUE(0);
                    }
                    else error = EINTR;
                }
            }
            else
            {
                // Wait forever (until something happens)
            
                if( !selwait.wait() )
                    error = EINTR;
            }
        }

        Cyg_Scheduler::unlock();
        
    } // while(!error)

    select_mutex.unlock();
 
    FILEIO_RETURN(error);
}

//==========================================================================
// Select support functions.

// -------------------------------------------------------------------------
// cyg_selinit() is used to initialize a selinfo structure

void cyg_selinit( struct CYG_SELINFO_TAG *sip )
{
    sip->si_info = 0;
    sip->si_thread = 0;
}

// -------------------------------------------------------------------------
// cyg_selrecord() is called when a client device needs to register
// the current thread for selection.

void cyg_selrecord( CYG_ADDRWORD info, struct CYG_SELINFO_TAG *sip )
{
    sip->si_info = info;
    sip->si_thread = (CYG_ADDRESS)Cyg_Thread::self();
}

// -------------------------------------------------------------------------
// cyg_selwakeup() is called when the client device matches the select
// criterion, and needs to wake up a selector.

void cyg_selwakeup( struct CYG_SELINFO_TAG *sip )
{
    // We don't actually use the si_info field of selinfo at present.
    // A potential use would be to select one of several selwait condition
    // variables to signal. However, that would only be necessary if we
    // end up having lots of threads in select.

    Cyg_Scheduler::lock();
 
    if( sip->si_thread != 0 )
    {
        // If the thread pointer is still present, this selection has
        // not been fired before. We just wake up all threads waiting,
        // regardless of whether they are waiting for this event or
        // not.  This avoids any race conditions, and is consistent
        // with the behaviour of the BSD kernel.
        
        sip->si_thread = 0;
        selwait.broadcast();
        selwake_count++;

    }

    Cyg_Scheduler::unlock();    
}

// -------------------------------------------------------------------------
// EOF select.cxx
