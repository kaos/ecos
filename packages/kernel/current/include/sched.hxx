#ifndef CYGONCE_KERNEL_SCHED_HXX
#define CYGONCE_KERNEL_SCHED_HXX

//==========================================================================
//
//      sched.hxx
//
//      Scheduler class declaration(s)
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
// Author(s):   nickg
// Contributors:        nickg
// Date:        1997-09-09
// Purpose:     Define Scheduler class interfaces
// Description: These class definitions supply the internal API
//              used to scheduler threads. 
// Usage:       #include <cyg/kernel/sched.hxx>
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>
#include <cyg/infra/cyg_ass.h>         // assertion macros


// -------------------------------------------------------------------------
// Miscellaneous types

#ifdef CYGSEM_KERNEL_SCHED_ASR_SUPPORT

typedef void Cyg_ASR( CYG_ADDRWORD data );      // ASR type signature

#endif

// -------------------------------------------------------------------------
// Scheduler base class. This defines stuff that is needed by the
// specific scheduler implementation. Each scheduler comprises three
// classes: Cyg_Scheduler_Base, Cyg_Scheduler_Implementation which
// inherits from it and Cyg_Scheduler which inherits from _it_ in turn.

class Cyg_Scheduler_Base
{
    friend class Cyg_HardwareThread;
    friend class Cyg_SchedThread;
    
protected:
    // The following variables are implicit in the API, but are
    // not publically visible.

    static volatile cyg_ucount32 sched_lock         // lock counter
                    CYGBLD_ATTRIB_ASM_ALIAS( cyg_scheduler_sched_lock );
    
    static Cyg_Thread   *current_thread;        // current running thread

    static cyg_bool     need_reschedule;        // set when schedule needed

    static cyg_ucount32 thread_switches;        // count of number of thread switches
};

// -------------------------------------------------------------------------
// Include the scheduler implementation header

#include CYGPRI_KERNEL_SCHED_IMPL_HXX

// Do some checking that we have a consistent universe.

#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL
# ifndef CYGIMP_THREAD_PRIORITY
#  error Priority inversion protocols will not work without priorities!!!
# endif
# if CYG_SCHED_UNIQUE_PRIORITIES
#  error Priority inversion protocols need non-unique priorities!!!
# endif
#endif

// -------------------------------------------------------------------------
// Scheduler class. This is the public scheduler interface seen by the
// rest of the kernel.

class Cyg_Scheduler
    : public Cyg_Scheduler_Implementation
{
    friend class Cyg_Thread;
    
    // This function is the actual implementation of the unlock
    // function.  The unlock() later is an inline shell that deals
    // with the common case.
    
    static void             unlock_inner(cyg_uint32 new_lock = 0);
    
public:

    CYGDBG_DEFINE_CHECK_THIS

    // The following API functions are common to all scheduler
    // implementations.

    // claim the preemption lock
    static void             lock();         

    // release the preemption lock and possibly reschedule
    static void             unlock();

    // release and reclaim the lock atomically, keeping the old
    // value on restart
    static void             reschedule();

    // decrement the lock but also look for a reschedule opportunity
    static void             unlock_reschedule();

    // release the preemption lock without rescheduling
    static void             unlock_simple();
    
    // return a pointer to the current thread
    static Cyg_Thread       *get_current_thread();

    // Return current value of lock
    static cyg_ucount32 get_sched_lock();

    // Return current number of thread switches
    static cyg_ucount32 get_thread_switches();
    
    // Start execution of the scheduler
    static void start() __attribute__ ((noreturn));
    
    // The only  scheduler instance should be this one...
    static Cyg_Scheduler scheduler;

};

// -------------------------------------------------------------------------
// This class encapsulates the scheduling abstractions in a thread.
// Cyg_SchedThread is included as a base class of Cyg_Thread. The actual
// implementation of the abstractions is in Cyg_SchedThread_Implementation
// so this class has little to do.

class Cyg_SchedThread
    : public Cyg_SchedThread_Implementation
{
    friend class Cyg_ThreadQueue_Implementation;
    friend class Cyg_Scheduler_Implementation;
    friend class Cyg_Scheduler;
    
    Cyg_ThreadQueue     *queue;


public:

    Cyg_SchedThread(Cyg_Thread *thread, CYG_ADDRWORD sched_info);

    // Return current queue pointer

    Cyg_ThreadQueue     *get_current_queue();
    
    // Remove this thread from current queue
    void remove();

#ifdef CYGSEM_KERNEL_SCHED_ASR_SUPPORT

    // ASR support.
    // An ASR is an Asynchronous Service Routine. When set pending it
    // is called when the thread exits the scheduler. ASRs are mainly
    // used by compatibility subsystems, such as POSIX, to implement
    // such things as thread cancellation and signal delivery.

private:

    volatile cyg_bool   asr_inhibit;    // If true, blocks calls to ASRs

    volatile cyg_bool   asr_pending;    // If true, this thread's ASR should be called.

#ifdef CYGSEM_KERNEL_SCHED_ASR_GLOBAL
    static
#endif    
    Cyg_ASR             *asr;            // ASR function
#ifdef CYGSEM_KERNEL_SCHED_ASR_DATA_GLOBAL
    static
#endif    
    CYG_ADDRWORD        asr_data;       // ASR data pointer

    // Default ASR function
    static void         asr_default(CYG_ADDRWORD data);

public:

    // Public interface to ASR mechanism

    // Set, clear and get inhibit flag.
    inline void set_asr_inhibit() { asr_inhibit = true; }
    inline void clear_asr_inhibit() { asr_inhibit = false; }
    inline cyg_bool get_asr_inhibit() { return asr_inhibit; }

    // Set and get pending flag. The flag is only cleared when the
    // ASR is called.
    inline void set_asr_pending() { asr_pending = true; }
    inline cyg_bool get_asr_pending() { return asr_pending; }

    // Set a new ASR, returning the old one. 
    void set_asr( Cyg_ASR  *new_asr, CYG_ADDRWORD  new_data,
                  Cyg_ASR **old_asr, CYG_ADDRWORD *old_data);

    // Clear the ASR function back to the default.
    void clear_asr();

#else

public:
    
    // Even when we do not have ASRs enabled, we keep these functions
    // available. This avoids excessive ifdefs in the rest of the
    // kernel code.
    inline void set_asr_inhibit() { }
    inline void clear_asr_inhibit() { }
    
#endif    
    
#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL

private:

    // For all priority inversion protocols we need to keep track of how
    // many mutexes we have locked, including one which we are waiting to
    // lock, because we can inherit priority while sleeping just prior to
    // wakeup.
    
    cyg_count32         mutex_count;

protected:
    // These are implementation functions that are common to all protocols.
        
    // Inherit the given priority. If thread is non-NULL the priority is
    // being inherited from it, otherwise it has come from the mutex.
    void set_inherited_priority( cyg_priority pri, Cyg_Thread *thread = 0 );

    // Relay the priority of the ex-owner thread or from the queue if it
    // has a higher priority than ours.
    void relay_inherited_priority( Cyg_Thread *ex_owner, Cyg_ThreadQueue *pqueue);

    // Lose priority inheritance
    void clear_inherited_priority();
    
public:    
    // Count and uncount the number of mutexes held by
    // this thread.
    void count_mutex() { mutex_count++; };
    void uncount_mutex() { mutex_count--; };

#if defined(CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_SIMPLE)
    
protected:    

    // The simple priority inversion protocols simply needs
    // somewhere to store the base priority of the current thread.
    
    cyg_priority        original_priority;      // our original priority

    cyg_bool            priority_inherited;     // have we inherited?

#endif

#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_INHERIT
    
public:

    // Inherit the priority of the provided thread if it
    // has higher priority than this.
    void inherit_priority( Cyg_Thread *thread);

    // Relay the priority of the ex-owner thread or from the queue if it
    // has a higher priority than ours.
    void relay_priority( Cyg_Thread *ex_owner, Cyg_ThreadQueue *pqueue);

    // Lose priority inheritance
    void disinherit_priority();
    
#endif

#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_CEILING

public:

    // Set the priority of this thread to the given ceiling.
    void set_priority_ceiling( cyg_priority pri );

    // Clear the ceiling, if necessary.
    void clear_priority_ceiling();
    
#endif    

#endif
    
};

// -------------------------------------------------------------------------
// Simple inline accessor functions

inline Cyg_Thread *Cyg_Scheduler::get_current_thread()
{
    return current_thread;
}

// Return current value of lock
inline cyg_ucount32 Cyg_Scheduler::get_sched_lock()
{
    return sched_lock;
}

// Return current number of thread switches
inline cyg_ucount32 Cyg_Scheduler::get_thread_switches()
{
    return thread_switches;
}

// Return current queue pointer
inline Cyg_ThreadQueue *Cyg_SchedThread::get_current_queue()
{
    return queue;
}

// -------------------------------------------------------------------------
#endif // ifndef __SCHED_HXX__
// EOF sched.hxx
