#ifndef CYGONCE_POSIX_TIME_H
#define CYGONCE_POSIX_TIME_H
//=============================================================================
//
//      time.h
//
//      POSIX time header
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
// Contributors:  nickg, jlarmour
// Date:          2000-03-17
// Purpose:       POSIX time header
// Description:   This header contains all the definitions needed to support
//                the POSIX timer and timer API under eCos.
//              
// Usage:         Do not include this file directly - instead include <time.h>
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>

//-----------------------------------------------------------------------------
// Types for timers and clocks

typedef int clockid_t;

typedef int timer_t;

// forward declaration - if the app uses it it will have to include
// signal.h anyway
struct sigevent;

//-----------------------------------------------------------------------------
// Structures

struct timespec
{
    time_t      tv_sec;
    long        tv_nsec;
};

struct itimerspec
{
    struct timespec     it_interval;
    struct timespec     it_value;
};

//-----------------------------------------------------------------------------
// Manifest constants

#define CLOCK_REALTIME          0

#define TIMER_ABSTIME           1

//-----------------------------------------------------------------------------
// Clock functions

// Set the clocks current time
externC int clock_settime( clockid_t clock_id, const struct timespec *tp);

// Get the clocks current time
externC int clock_gettime( clockid_t clock_id, struct timespec *tp);

// Get the clocks resolution
externC int clock_getres( clockid_t clock_id, struct timespec *tp);


//-----------------------------------------------------------------------------
// Timer functions

// Create a timer based on the given clock.
externC int timer_create( clockid_t clock_id,
                          struct sigevent *evp,
                          timer_t *timer_id);

// Delete the timer
externC int timer_delete( timer_t timer_id );

// Set the expiration time of the timer.
externC int timer_settime( timer_t timerid, int flags,
                           const struct itimerspec *value,
                           struct itimerspec *ovalue );

// Get current timer values
externC int timer_gettime( timer_t timerid, struct itimerspec *value );

// Get number of missed triggers
externC int timer_getoverrun( timer_t timerid );

//-----------------------------------------------------------------------------
// Nanosleep

// Sleep for the given time.
externC int nanosleep( const struct timespec *rqtp,
                       struct timespec *rmtp);


//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_POSIX_TIME_H
// End of time.h
