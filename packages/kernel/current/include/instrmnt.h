#ifndef CYGONCE_KERNEL_INSTRMNT_HXX
#define CYGONCE_KERNEL_INSTRMNT_HXX

//==========================================================================
//
//      instrmnt.hxx
//
//      Kernel Instrumentation mechanism
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Author(s):   nickg
// Contributors:        nickg
// Date:        1997-10-24
// Purpose:     Define kernel instrumentation
// Description: A set of definitions and macros used to implement an
//              instrumentation interface for the kernel.
// Usage:       #include <cyg/kernel/instrmnt.hxx>
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>
#include <cyg/infra/cyg_ass.h>            // assertion macros

// -------------------------------------------------------------------------
// Underlying instrumentation function

externC void cyg_instrument( cyg_uint32 type, CYG_ADDRWORD arg1, CYG_ADDRWORD arg2 );

// -------------------------------------------------------------------------
// The following functions are used to enable and disable specific
// instrumentation classes and events. The class must be one of the
// class defines below. The event may be one of the event defines below
// or zero, in which case all of the events in the class are enabled or
// disabled.

#ifdef CYGDBG_KERNEL_INSTRUMENT_FLAGS

externC void cyg_instrument_enable( cyg_uint32 cl, cyg_uint32 event );

externC void cyg_instrument_disable( cyg_uint32 cl, cyg_uint32 event );

#endif

// -------------------------------------------------------------------------
// Instrumentation macros

#ifdef CYGPKG_KERNEL_INSTRUMENT

#define CYG_INSTRUMENT(_type_,_arg1_,_arg2_) cyg_instrument(_type_, (CYG_ADDRWORD)(_arg1_), (CYG_ADDRWORD)(_arg2_))

#else   // ifdef CYGPKG_KERNEL_INSTRUMENT

#define CYG_INSTRUMENT(_type_,_arg1_,_arg2_)                    \
    CYG_MACRO_START                                             \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg1_));     \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg2_));     \
    CYG_MACRO_END

#endif  // ifdef CYGPKG_KERNEL_INSTRUMENT

// -------------------------------------------------------------------------
// Type codes.
// Each code is 16 bit with an event class in the top 8 bits
// and an event code in the lower 8 bits.
           
// Event classes
#define CYG_INSTRUMENT_CLASS_SCHED              0x0100
#define CYG_INSTRUMENT_CLASS_THREAD             0x0200
#define CYG_INSTRUMENT_CLASS_INTR               0x0300
#define CYG_INSTRUMENT_CLASS_MUTEX              0x0400
#define CYG_INSTRUMENT_CLASS_CONDVAR            0x0500
#define CYG_INSTRUMENT_CLASS_BINSEM             0x0600
#define CYG_INSTRUMENT_CLASS_CNTSEM             0x0700
#define CYG_INSTRUMENT_CLASS_CLOCK              0x0800
#define CYG_INSTRUMENT_CLASS_ALARM              0x0900
#define CYG_INSTRUMENT_CLASS_MBOXT              0x0a00

#define CYG_INSTRUMENT_CLASS_USER               0x0f00

#define CYG_INSTRUMENT_CLASS_MAX                CYG_INSTRUMENT_CLASS_USER

// Scheduler events
#define CYG_INSTRUMENT_EVENT_SCHED_LOCK         1
#define CYG_INSTRUMENT_EVENT_SCHED_UNLOCK       2
#define CYG_INSTRUMENT_EVENT_SCHED_RESCHEDULE   3
#define CYG_INSTRUMENT_EVENT_SCHED_TIMESLICE    4

// Thread events
#define CYG_INSTRUMENT_EVENT_THREAD_SWITCH      1
#define CYG_INSTRUMENT_EVENT_THREAD_SLEEP       2
#define CYG_INSTRUMENT_EVENT_THREAD_WAKE        3
#define CYG_INSTRUMENT_EVENT_THREAD_SUSPEND     4
#define CYG_INSTRUMENT_EVENT_THREAD_RESUME      5
#define CYG_INSTRUMENT_EVENT_THREAD_PRIORITY    6
#define CYG_INSTRUMENT_EVENT_THREAD_DELAY       7
#define CYG_INSTRUMENT_EVENT_THREAD_ALARM       8
#define CYG_INSTRUMENT_EVENT_THREAD_ENTER       9

// Interrupt events
#define CYG_INSTRUMENT_EVENT_INTR_RAISE         1
#define CYG_INSTRUMENT_EVENT_INTR_END           2
#define CYG_INSTRUMENT_EVENT_INTR_RESTORE       3
#define CYG_INSTRUMENT_EVENT_INTR_POST_DSR      4
#define CYG_INSTRUMENT_EVENT_INTR_CALL_DSR      5
#define CYG_INSTRUMENT_EVENT_INTR_ATTACH        6
#define CYG_INSTRUMENT_EVENT_INTR_DETACH        7
#define CYG_INSTRUMENT_EVENT_INTR_SET_VSR       8
#define CYG_INSTRUMENT_EVENT_INTR_DISABLE       9
#define CYG_INSTRUMENT_EVENT_INTR_ENABLE        10
#define CYG_INSTRUMENT_EVENT_INTR_MASK          11
#define CYG_INSTRUMENT_EVENT_INTR_UNMASK        12
#define CYG_INSTRUMENT_EVENT_INTR_CONFIGURE     13
#define CYG_INSTRUMENT_EVENT_INTR_ACK           14
#define CYG_INSTRUMENT_EVENT_INTR_CHAIN_ISR     15

// Mutex events
#define CYG_INSTRUMENT_EVENT_MUTEX_LOCK         1
#define CYG_INSTRUMENT_EVENT_MUTEX_WAIT         2
#define CYG_INSTRUMENT_EVENT_MUTEX_LOCKED       3
#define CYG_INSTRUMENT_EVENT_MUTEX_TRY          4
#define CYG_INSTRUMENT_EVENT_MUTEX_UNLOCK       5
#define CYG_INSTRUMENT_EVENT_MUTEX_WAKE         6
#define CYG_INSTRUMENT_EVENT_MUTEX_RELEASE      7
#define CYG_INSTRUMENT_EVENT_MUTEX_RELEASED     8

// Condition variable events
#define CYG_INSTRUMENT_EVENT_CONDVAR_WAIT       1
#define CYG_INSTRUMENT_EVENT_CONDVAR_WOKE       2
#define CYG_INSTRUMENT_EVENT_CONDVAR_SIGNAL     3
#define CYG_INSTRUMENT_EVENT_CONDVAR_WAKE       4
#define CYG_INSTRUMENT_EVENT_CONDVAR_BROADCAST  5
#define CYG_INSTRUMENT_EVENT_CONDVAR_TIMED_WAIT 6

// Binary semaphore events
#define CYG_INSTRUMENT_EVENT_BINSEM_CLAIM       1
#define CYG_INSTRUMENT_EVENT_BINSEM_WAIT        2
#define CYG_INSTRUMENT_EVENT_BINSEM_WOKE        3
#define CYG_INSTRUMENT_EVENT_BINSEM_TRY         4
#define CYG_INSTRUMENT_EVENT_BINSEM_POST        5
#define CYG_INSTRUMENT_EVENT_BINSEM_WAKE        6

// Counting semaphore events
#define CYG_INSTRUMENT_EVENT_CNTSEM_CLAIM       1
#define CYG_INSTRUMENT_EVENT_CNTSEM_WAIT        2
#define CYG_INSTRUMENT_EVENT_CNTSEM_WOKE        3
#define CYG_INSTRUMENT_EVENT_CNTSEM_TRY         4
#define CYG_INSTRUMENT_EVENT_CNTSEM_POST        5
#define CYG_INSTRUMENT_EVENT_CNTSEM_WAKE        6
#define CYG_INSTRUMENT_EVENT_CNTSEM_TIMEOUT     7

// Clock events
#define CYG_INSTRUMENT_EVENT_CLOCK_TICK_START   1
#define CYG_INSTRUMENT_EVENT_CLOCK_TICK_END     2
#define CYG_INSTRUMENT_EVENT_CLOCK_ISR          3

// Alarm events
#define CYG_INSTRUMENT_EVENT_ALARM_ADD          1
#define CYG_INSTRUMENT_EVENT_ALARM_REM          2
#define CYG_INSTRUMENT_EVENT_ALARM_CALL         3
#define CYG_INSTRUMENT_EVENT_ALARM_INIT         4
#define CYG_INSTRUMENT_EVENT_ALARM_TRIGGER      5
#define CYG_INSTRUMENT_EVENT_ALARM_INTERVAL     6

// Mboxt events
#define CYG_INSTRUMENT_EVENT_MBOXT_WAIT         1
#define CYG_INSTRUMENT_EVENT_MBOXT_GET          2
#define CYG_INSTRUMENT_EVENT_MBOXT_GOT          3
#define CYG_INSTRUMENT_EVENT_MBOXT_TIMEOUT      4
#define CYG_INSTRUMENT_EVENT_MBOXT_WAKE         5
#define CYG_INSTRUMENT_EVENT_MBOXT_TRY          6
#define CYG_INSTRUMENT_EVENT_MBOXT_PUT          7



// User events

#define CYG_INSTRUMENT_EVENT_USER_1             1
#define CYG_INSTRUMENT_EVENT_USER_2             2
#define CYG_INSTRUMENT_EVENT_USER_3             3
#define CYG_INSTRUMENT_EVENT_USER_4             4
#define CYG_INSTRUMENT_EVENT_USER_5             5
#define CYG_INSTRUMENT_EVENT_USER_6             6
#define CYG_INSTRUMENT_EVENT_USER_7             7
#define CYG_INSTRUMENT_EVENT_USER_8             8
#define CYG_INSTRUMENT_EVENT_USER_9             9
#define CYG_INSTRUMENT_EVENT_USER_10            10
#define CYG_INSTRUMENT_EVENT_USER_11            11
#define CYG_INSTRUMENT_EVENT_USER_12            12
#define CYG_INSTRUMENT_EVENT_USER_13            13
#define CYG_INSTRUMENT_EVENT_USER_14            14
#define CYG_INSTRUMENT_EVENT_USER_15            15
#define CYG_INSTRUMENT_EVENT_USER_16            16
#define CYG_INSTRUMENT_EVENT_USER_17            17
#define CYG_INSTRUMENT_EVENT_USER_18            18
#define CYG_INSTRUMENT_EVENT_USER_19            19

// -------------------------------------------------------------------------
// Handy-dandy macro to make event codes.
// Note that this relies on the definitions above
// having a well defined form.

#define CYGINST_EVENT(_class_,_event_) \
((CYG_INSTRUMENT_CLASS_##_class_)|(CYG_INSTRUMENT_EVENT_##_class_##_##_event_))

// -------------------------------------------------------------------------
// Scheduler instrumentation macros

#ifdef CYGDBG_KERNEL_INSTRUMENT_SCHED
            
#define CYG_INSTRUMENT_SCHED(_event_,_arg1_,_arg2_) \
    CYG_INSTRUMENT(CYGINST_EVENT(SCHED,_event_),_arg1_,_arg2_)

#else

#define CYG_INSTRUMENT_SCHED(_event_,_arg1_,_arg2_)             \
    CYG_MACRO_START                                             \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg1_));     \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg2_));     \
    CYG_MACRO_END

#endif

// -------------------------------------------------------------------------
// Thread instrumentation macros

#ifdef CYGDBG_KERNEL_INSTRUMENT_THREAD
            
#define CYG_INSTRUMENT_THREAD(_event_,_arg1_,_arg2_) \
    CYG_INSTRUMENT(CYGINST_EVENT(THREAD,_event_),_arg1_,_arg2_)

#else

#define CYG_INSTRUMENT_THREAD(_event_,_arg1_,_arg2_)            \
    CYG_MACRO_START                                             \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg1_));     \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg2_));     \
    CYG_MACRO_END

#endif

// -------------------------------------------------------------------------
// Interrupt instrumentation macros

#ifdef CYGDBG_KERNEL_INSTRUMENT_INTR
            
#define CYG_INSTRUMENT_INTR(_event_,_arg1_,_arg2_) \
    CYG_INSTRUMENT(CYGINST_EVENT(INTR,_event_),_arg1_,_arg2_)

#else

#define CYG_INSTRUMENT_INTR(_event_,_arg1_,_arg2_)              \
    CYG_MACRO_START                                             \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg1_));     \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg2_));     \
    CYG_MACRO_END

#endif

// -------------------------------------------------------------------------
// Mutex instrumentation macros

#ifdef CYGDBG_KERNEL_INSTRUMENT_MUTEX
            
#define CYG_INSTRUMENT_MUTEX(_event_,_arg1_,_arg2_) \
    CYG_INSTRUMENT(CYGINST_EVENT(MUTEX,_event_),_arg1_,_arg2_)

#else

#define CYG_INSTRUMENT_MUTEX(_event_,_arg1_,_arg2_)             \
    CYG_MACRO_START                                             \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg1_));     \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg2_));     \
    CYG_MACRO_END

#endif

// -------------------------------------------------------------------------
// Condition variable instrumentation macros

#ifdef CYGDBG_KERNEL_INSTRUMENT_CONDVAR
            
#define CYG_INSTRUMENT_CONDVAR(_event_,_arg1_,_arg2_) \
    CYG_INSTRUMENT(CYGINST_EVENT(CONDVAR,_event_),_arg1_,_arg2_)

#else

#define CYG_INSTRUMENT_CONDVAR(_event_,_arg1_,_arg2_)           \
    CYG_MACRO_START                                             \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg1_));     \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg2_));     \
    CYG_MACRO_END

#endif

// -------------------------------------------------------------------------
// Binary semaphore instrumentation macros

#ifdef CYGDBG_KERNEL_INSTRUMENT_BINSEM
            
#define CYG_INSTRUMENT_BINSEM(_event_,_arg1_,_arg2_) \
    CYG_INSTRUMENT(CYGINST_EVENT(BINSEM,_event_),_arg1_,_arg2_)

#else

#define CYG_INSTRUMENT_BINSEM(_event_,_arg1_,_arg2_)            \
    CYG_MACRO_START                                             \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg1_));     \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg2_));     \
    CYG_MACRO_END

#endif

// -------------------------------------------------------------------------
// Counting semaphore instrumentation macros

#ifdef CYGDBG_KERNEL_INSTRUMENT_CNTSEM
            
#define CYG_INSTRUMENT_CNTSEM(_event_,_arg1_,_arg2_) \
    CYG_INSTRUMENT(CYGINST_EVENT(CNTSEM,_event_),_arg1_,_arg2_)

#else

#define CYG_INSTRUMENT_CNTSEM(_event_,_arg1_,_arg2_)            \
    CYG_MACRO_START                                             \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg1_));     \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg2_));     \
    CYG_MACRO_END

#endif

// -------------------------------------------------------------------------
// Clock instrumentation macros

#ifdef CYGDBG_KERNEL_INSTRUMENT_CLOCK
            
#define CYG_INSTRUMENT_CLOCK(_event_,_arg1_,_arg2_) \
    CYG_INSTRUMENT(CYGINST_EVENT(CLOCK,_event_),_arg1_,_arg2_)

#else

#define CYG_INSTRUMENT_CLOCK(_event_,_arg1_,_arg2_)             \
    CYG_MACRO_START                                             \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg1_));     \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg2_));     \
    CYG_MACRO_END

#endif

// -------------------------------------------------------------------------
// Alarm instrumentation macros

#ifdef CYGDBG_KERNEL_INSTRUMENT_ALARM
            
#define CYG_INSTRUMENT_ALARM(_event_,_arg1_,_arg2_) \
    CYG_INSTRUMENT(CYGINST_EVENT(ALARM,_event_),_arg1_,_arg2_)

#else

#define CYG_INSTRUMENT_ALARM(_event_,_arg1_,_arg2_)             \
    CYG_MACRO_START                                             \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg1_));     \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg2_));     \
    CYG_MACRO_END

#endif

// -------------------------------------------------------------------------
// Mboxt instrumentation macros

#ifdef CYGDBG_KERNEL_INSTRUMENT_MBOXT
            
#define CYG_INSTRUMENT_MBOXT(_event_,_arg1_,_arg2_) \
    CYG_INSTRUMENT(CYGINST_EVENT(MBOXT,_event_),_arg1_,_arg2_)

#else

#define CYG_INSTRUMENT_MBOXT(_event_,_arg1_,_arg2_)             \
    CYG_MACRO_START                                             \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg1_));     \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg2_));     \
    CYG_MACRO_END

#endif

// -------------------------------------------------------------------------
// User instrumentation

#ifdef CYGDBG_KERNEL_INSTRUMENT_USER
            
#define CYG_INSTRUMENT_USER(_event_,_arg1_,_arg2_) \
    CYG_INSTRUMENT(CYGINST_EVENT(USER,_event_),_arg1_,_arg2_)

#else

#define CYG_INSTRUMENT_USER(_event_,_arg1_,_arg2_)              \
    CYG_MACRO_START                                             \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg1_));     \
    CYG_UNUSED_PARAM(CYG_ADDRWORD, (CYG_ADDRWORD)(_arg2_));     \
    CYG_MACRO_END

#endif


// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_KERNEL_INSTRMNT_HXX
// EOF instrmnt.hxx
