//==========================================================================
//
//      common/kapi.cxx
//
//      C API Implementation
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
// Author(s):   nickg, dsm
// Contributors:        nickg
// Date:        1998-03-02
// Purpose:     C API Implementation
// Description: C++ implementation of the C API
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>

#ifdef CYGFUN_KERNEL_API_C

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros
#include <cyg/kernel/instrmnt.h>       // instrumentation
#include <cyg/kernel/diag.h>

#include <cyg/kernel/thread.hxx>
#include <cyg/kernel/thread.inl>       // thread inlines
#include <cyg/kernel/sched.hxx>
#include <cyg/kernel/intr.hxx>
#include <cyg/kernel/clock.hxx>

#include <cyg/kernel/sema.hxx>
#include <cyg/kernel/flag.hxx>
#include <cyg/kernel/mutex.hxx>
#include <cyg/kernel/mbox.hxx>

#include <cyg/kernel/sched.inl>        // scheduler inlines
#include <cyg/kernel/clock.inl>        // clock inlines

#include <cyg/kernel/kapi.h>           // C API

// -------------------------------------------------------------------------
// Magic new function

inline void *operator new(size_t size, void *ptr)
{
    CYG_CHECK_DATA_PTR( ptr, "Bad pointer" );
    return ptr;
}

// -------------------------------------------------------------------------

#ifdef CYGDBG_USE_ASSERTS

#define CYG_ASSERT_SIZES(cstruct, cxxstruct)                      \
CYG_MACRO_START                                                   \
    char *msg = "Size of C struct " #cstruct                      \
                       " != size of C++ struct " #cxxstruct ;     \
    CYG_ASSERT( sizeof(cstruct) == sizeof(cxxstruct) , msg );     \
CYG_MACRO_END

#else

#define CYG_ASSERT_SIZES(cstruct, cxxstruct)

#endif

/*---------------------------------------------------------------------------*/
/* Scheduler operations */

/* Starts scheduler with created threads.  Never returns. */
externC void cyg_scheduler_start(void)
{
    Cyg_Scheduler::start();
}

/* Lock the scheduler. */
externC void cyg_scheduler_lock(void)
{
    Cyg_Scheduler::lock();
    // get_sched_lock() is unsigned, see below "cyg_ucount32 lock"
    CYG_ASSERT( (0xff000000 & (Cyg_Scheduler::get_sched_lock())) == 0,
                "Scheduler overlocked" );
}

/* Lock the scheduler, but never more than level=1. */
externC void cyg_scheduler_safe_lock(void)
{
    Cyg_Scheduler::lock();
    cyg_ucount32 slock = Cyg_Scheduler::get_sched_lock();
    if (slock > 1)
        Cyg_Scheduler::unlock();
    // get_sched_lock() is unsigned, see below "cyg_ucount32 lock"
    CYG_ASSERT( (0xff000000 & (Cyg_Scheduler::get_sched_lock())) == 0,
                "Scheduler overlocked" );
}

/* Unlock the scheduler. */
externC void cyg_scheduler_unlock(void)
{
    cyg_ucount32 slock = Cyg_Scheduler::get_sched_lock();
    CYG_ASSERT( 0 < slock, "Scheduler not locked" );
    // And program defensively too:
    if ( 0 < slock )
        Cyg_Scheduler::unlock();
}

/* Read the scheduler lock value. */
externC cyg_ucount32 cyg_scheduler_read_lock(void)
{
    cyg_ucount32 slock = Cyg_Scheduler::get_sched_lock();
    return slock;
}

/*---------------------------------------------------------------------------*/
/* Thread operations */

externC void cyg_thread_create(
    cyg_addrword_t      sched_info,             /* scheduling info (eg pri)  */
    cyg_thread_entry_t  *entry,                 /* entry point function      */
    cyg_addrword_t      entry_data,             /* entry data                */
    char                *name,                  /* optional thread name      */
    void                *stack_base,            /* stack base, NULL = alloc  */
    cyg_ucount32        stack_size,             /* stack size, 0 = default   */
    cyg_handle_t        *handle,                /* returned thread handle    */
    cyg_thread          *thread                 /* put thread here           */
)
{
    CYG_ASSERT_SIZES( cyg_thread, Cyg_Thread );

    Cyg_Thread *t = new((void *)thread) Cyg_Thread (
        (CYG_ADDRWORD) sched_info,
        (cyg_thread_entry *)entry,
        (CYG_ADDRWORD) entry_data,
        name,
        (CYG_ADDRWORD) stack_base,
        stack_size
        );
    t=t;
    
    CYG_CHECK_DATA_PTR( handle, "Bad handle pointer" );
    *handle = (cyg_handle_t)thread;
}

externC void cyg_thread_exit()
{
    Cyg_Thread::exit();
}

externC cyg_bool_t cyg_thread_delete( cyg_handle_t thread )
{
    Cyg_Thread *th = (Cyg_Thread *)thread;
    if( th->get_state() != Cyg_Thread::EXITED )
        th->kill(); // encourage it to terminate
    if( th->get_state() != Cyg_Thread::EXITED )
        return false; // it didn't run yet, leave it up to the app to fix
    th->~Cyg_Thread();
    return true;
}

externC void cyg_thread_suspend(cyg_handle_t thread)
{
    ((Cyg_Thread *)thread)->suspend();
}

externC void cyg_thread_resume(cyg_handle_t thread)
{
    Cyg_Thread *th = (Cyg_Thread *)thread;

    // If we are resuming an exited thread then
    // reinitialize it.
    
    if( th->get_state() == Cyg_Thread::EXITED )
        th->reinitialize();

    th->resume();
}

externC void cyg_thread_kill( cyg_handle_t thread)
{
    ((Cyg_Thread *)thread)->kill();
}

externC void cyg_thread_release( cyg_handle_t thread)
{
    ((Cyg_Thread *)thread)->release();    
}

externC void cyg_thread_yield()
{
    Cyg_Thread::yield();
}

externC cyg_handle_t cyg_thread_self()
{
    return (cyg_handle_t)Cyg_Thread::self();
}

// idle thread is not really a plain CygThread; danger.
externC cyg_handle_t cyg_thread_idle_thread()
{
    extern Cyg_Thread idle_thread;
    return (cyg_handle_t)&idle_thread;
}

/* Priority manipulation */
externC void cyg_thread_set_priority(
    cyg_handle_t thread, cyg_priority_t priority )
{
#ifdef CYGIMP_THREAD_PRIORITY
    ((Cyg_Thread *)thread)->set_priority(priority);
#endif
}


/* Get the normal priority, ie without any applied mutex inheritance or
 * ceiling protocol. */
externC cyg_priority_t cyg_thread_get_priority(cyg_handle_t thread)
{
#ifdef CYGIMP_THREAD_PRIORITY
    return ((Cyg_Thread *)thread)->get_priority();
#else
    return 0;
#endif
}


/* Get the current priority, ie any applied mutex inheritance or
 * ceiling protocol. */
externC cyg_priority_t cyg_thread_get_current_priority(cyg_handle_t thread)
{
#ifdef CYGIMP_THREAD_PRIORITY
    return ((Cyg_Thread *)thread)->get_current_priority();
#else
    return 0;
#endif
}

/* Deadline scheduling control (optional) */

externC void cyg_thread_deadline_wait( 
    cyg_tick_count_t    start_time,             /* abs earliest start time   */
    cyg_tick_count_t    run_time,               /* worst case execution time */
    cyg_tick_count_t    deadline                /* absolute deadline         */
)
{
    CYG_ASSERT(0,"Not implemented");
} 

externC void cyg_thread_delay(cyg_tick_count_t delay)
{
    Cyg_Thread::self()->delay(delay);
}

/* Stack information */
externC cyg_addrword_t cyg_thread_get_stack_base(cyg_handle_t thread)
{
    return ((Cyg_Thread *)thread)->get_stack_base();
}

externC cyg_uint32 cyg_thread_get_stack_size(cyg_handle_t thread)
{
    return ((Cyg_Thread *)thread)->get_stack_size();
}

#ifdef CYGFUN_KERNEL_THREADS_STACK_MEASUREMENT
externC cyg_uint32 cyg_thread_measure_stack_usage(cyg_handle_t thread)
{
    return ((Cyg_Thread *)thread)->measure_stack_usage();
}
#endif

/*---------------------------------------------------------------------------*/
/* Per-thread data                                                           */

#ifdef CYGVAR_KERNEL_THREADS_DATA

externC cyg_ucount32 cyg_thread_new_data_index()
{
    Cyg_Thread::cyg_data_index index = Cyg_Thread::new_data_index();
    CYG_ASSERT(index >= 0, "failed to allocate data index" );
    return index;
}

externC void cyg_thread_free_data_index(cyg_ucount32 index)
{
    Cyg_Thread::free_data_index(index);
}

externC CYG_ADDRWORD cyg_thread_get_data(cyg_ucount32 index)
{
    return Cyg_Thread::get_data(index);
}

externC CYG_ADDRWORD *cyg_thread_get_data_ptr(cyg_ucount32 index)
{
    return Cyg_Thread::get_data_ptr(index);
}

externC void cyg_thread_set_data(cyg_ucount32 index, CYG_ADDRWORD 
data)
{
    Cyg_Thread::self()->set_data(index, data);
}
#endif

/*---------------------------------------------------------------------------*/
/* Exception handling.                                                       */

#ifdef CYGPKG_KERNEL_EXCEPTIONS    
externC void cyg_exception_set_handler(
    cyg_code_t                  exception_number,
    cyg_exception_handler_t     *new_handler,
    cyg_addrword_t              new_data,
    cyg_exception_handler_t     **old_handler,
    cyg_addrword_t              *old_data
)
{
    Cyg_Thread::register_exception(
        exception_number,
        (cyg_exception_handler *)new_handler,
        (CYG_ADDRWORD)new_data,
        (cyg_exception_handler **)old_handler,
        (CYG_ADDRWORD *)old_data
        );
}

/* Clear exception handler to default                                        */
externC void cyg_exception_clear_handler(
    cyg_code_t                  exception_number
)
{
    Cyg_Thread::deregister_exception( exception_number );
}

/* Invoke exception handler                                                  */
externC void cyg_exception_call_handler(
    cyg_handle_t                thread,
    cyg_code_t                  exception_number,
    cyg_addrword_t              error_code
)
{
    Cyg_Thread *t = (Cyg_Thread *)thread;

    t->deliver_exception( exception_number, error_code );
}
#endif    

/*---------------------------------------------------------------------------*/
/* Interrupt handling                                                        */

externC void cyg_interrupt_create(
    cyg_vector_t        vector,         /* Vector to attach to               */
    cyg_priority_t      priority,       /* Queue priority                    */
    cyg_addrword_t      data,           /* Data pointer                      */
    cyg_ISR_t           *isr,           /* Interrupt Service Routine         */
    cyg_DSR_t           *dsr,           /* Deferred Service Routine          */
    cyg_handle_t        *handle,        /* returned handle                   */
    cyg_interrupt       *intr           /* put interrupt here                */
)
{
    CYG_ASSERT_SIZES( cyg_interrupt, Cyg_Interrupt );

    Cyg_Interrupt *t = new((void *)intr) Cyg_Interrupt (
        (cyg_vector)vector,
        (cyg_priority)priority,
        (CYG_ADDRWORD)data,
        (cyg_ISR *)isr,
        (cyg_DSR *)dsr );
    t=t;

    CYG_CHECK_DATA_PTR( handle, "Bad handle pointer" );
    *handle = (cyg_handle_t)intr;
}

externC void cyg_interrupt_delete( cyg_handle_t interrupt)
{
    ((Cyg_Interrupt *)interrupt)->~Cyg_Interrupt();
}

void cyg_interrupt_attach( cyg_handle_t interrupt )
{
    ((Cyg_Interrupt *)interrupt)->attach();
}

void cyg_interrupt_detach( cyg_handle_t interrupt )
{
    ((Cyg_Interrupt *)interrupt)->detach();
}

/* VSR manipulation */

externC void cyg_interrupt_get_vsr(
    cyg_vector_t        vector,         /* vector to get                     */
    cyg_VSR_t           **vsr           /* vsr got                           */
)
{
    Cyg_Interrupt::get_vsr( (cyg_vector)vector, (cyg_VSR **)vsr);
}

externC void cyg_interrupt_set_vsr(
    cyg_vector_t        vector,         /* vector to set                     */
    cyg_VSR_t           *vsr            /* vsr to set                        */
)
{
    Cyg_Interrupt::set_vsr( (cyg_vector)vector, (cyg_VSR *)vsr);
}

/* CPU level interrupt mask                                                  */
externC void cyg_interrupt_disable()
{
    Cyg_Interrupt::disable_interrupts();
}

externC void cyg_interrupt_enable()
{
    Cyg_Interrupt::enable_interrupts();
}

/* Interrupt controller access                                               */
externC void cyg_interrupt_mask(cyg_vector_t vector)
{
    Cyg_Interrupt::mask_interrupt( (cyg_vector)vector);
}

externC void cyg_interrupt_unmask(cyg_vector_t vector)
{
    Cyg_Interrupt::unmask_interrupt( (cyg_vector)vector);
}


externC void cyg_interrupt_acknowledge(cyg_vector_t vector)
{
    Cyg_Interrupt::acknowledge_interrupt( (cyg_vector)vector);
}


externC void cyg_interrupt_configure(
    cyg_vector_t        vector,         /* vector to configure               */
    cyg_bool_t          level,          /* level or edge triggered           */
    cyg_bool_t          up              /* rising/faling edge, high/low level*/
)
{
    Cyg_Interrupt::configure_interrupt( (cyg_vector)vector, level, up );
}

externC void cyg_interrupt_set_cpu(
    cyg_vector_t        vector,         /* vector to control                 */
    cyg_cpu_t           cpu             /* CPU to set                        */
)
{
#ifdef CYGPKG_KERNEL_SMP_SUPPORT    
    Cyg_Interrupt::set_cpu( vector, cpu );
#endif    
}

externC cyg_cpu_t cyg_interrupt_get_cpu(
    cyg_vector_t        vector          /* vector to control                 */
)
{
#ifdef CYGPKG_KERNEL_SMP_SUPPORT        
    return Cyg_Interrupt::get_cpu( vector );
#else
    return CYG_KERNEL_CPU_THIS();
#endif    
    
}

/*---------------------------------------------------------------------------*/
/* Counters, Clocks and Alarms                                               */

externC void cyg_counter_create(
    cyg_handle_t        *handle,        /* returned counter handle           */
    cyg_counter         *counter        /* put counter here                  */
)
{
    CYG_ASSERT_SIZES( cyg_counter, Cyg_Counter );

    Cyg_Counter *t = new((void *)counter) Cyg_Counter ();
    t=t;

    CYG_CHECK_DATA_PTR( handle, "Bad handle pointer" );
    *handle = (cyg_handle_t)counter;
}

externC void cyg_counter_delete(cyg_handle_t counter)
{
    ((Cyg_Counter *)counter)->~Cyg_Counter();
}

/* Return current value of counter                                           */
externC cyg_tick_count_t cyg_counter_current_value(cyg_handle_t counter)
{
    return ((Cyg_Counter *)counter)->current_value();
}

/* Set new current value                                                     */
externC void cyg_counter_set_value(
    cyg_handle_t        counter,
    cyg_tick_count_t new_value
)
{
    ((Cyg_Counter *)counter)->set_value( new_value );
}

/* Advance counter by one tick                                               */
externC void cyg_counter_tick(cyg_handle_t counter)
{
    ((Cyg_Counter *)counter)->tick(); 
}

/* Create a clock object                */
externC void cyg_clock_create(
    cyg_resolution_t    resolution,     /* Initial resolution                */
    cyg_handle_t        *handle,        /* Returned clock handle             */
    cyg_clock           *clock          /* put clock here                    */
)
{
    CYG_ASSERT_SIZES( cyg_clock, Cyg_Clock );

    Cyg_Clock::cyg_resolution res;

    res.dividend = resolution.dividend;
    res.divisor  = resolution.divisor;

    Cyg_Clock *t = new((void *)clock) Cyg_Clock ( res );
    t=t;

    CYG_CHECK_DATA_PTR( handle, "Bad handle pointer" );
    *handle = (cyg_handle_t)clock;
}

externC void cyg_clock_delete(cyg_handle_t clock)
{
    ((Cyg_Clock *)clock)->~Cyg_Clock();
}

/* convert a clock handle to a counter handle so we can use the              */
/* counter API on it.                                                        */
externC void cyg_clock_to_counter(
    cyg_handle_t        clock,
    cyg_handle_t        *counter
)
{
    CYG_CHECK_DATA_PTR( counter, "Bad counter handle pointer" );
    *counter = (cyg_handle_t)(Cyg_Counter *)clock;
}

externC void cyg_clock_set_resolution(
    cyg_handle_t        clock,
    cyg_resolution_t    resolution      /* New resolution                    */
)
{
    Cyg_Clock::cyg_resolution res;

    res.dividend = resolution.dividend;
    res.divisor  = resolution.divisor;

    ((Cyg_Clock *)clock)->set_resolution( res );
}

externC cyg_resolution_t cyg_clock_get_resolution(cyg_handle_t clock)
{
    Cyg_Clock::cyg_resolution res =
        ((Cyg_Clock *)clock)->get_resolution();    

    cyg_resolution_t resolution;

    resolution.dividend = res.dividend;
    resolution.divisor  = res.divisor;

    return resolution;
}

#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK
externC cyg_handle_t cyg_real_time_clock(void)
{
    return (cyg_handle_t)Cyg_Clock::real_time_clock;
}

externC cyg_tick_count_t cyg_current_time(void)
{
    return Cyg_Clock::real_time_clock->current_value();
}
#endif

externC void cyg_alarm_create(
    cyg_handle_t        counter,        /* Attached to this counter          */
    cyg_alarm_t         *alarmfn,       /* Call-back function                */
    cyg_addrword_t      data,           /* Call-back data                    */
    cyg_handle_t        *handle,        /* Returned alarm object             */
    cyg_alarm           *alarm          /* put alarm here                    */
)
{
    CYG_ASSERT_SIZES( cyg_alarm, Cyg_Alarm );

    Cyg_Alarm *t = new((void *)alarm) Cyg_Alarm (
        (Cyg_Counter *)counter,
        (cyg_alarm_fn *)alarmfn,
        (CYG_ADDRWORD)data
    );
    t=t;

    CYG_CHECK_DATA_PTR( handle, "Bad handle pointer" );
    *handle = (cyg_handle_t)alarm;
}

/* Disable alarm, detach from counter and invalidate handles                 */
externC void cyg_alarm_delete( cyg_handle_t alarm)
{
    ((Cyg_Alarm *)alarm)->~Cyg_Alarm();
}

externC void cyg_alarm_initialize(
    cyg_handle_t        alarm,
    cyg_tick_count_t    trigger,        /* Absolute trigger time             */
    cyg_tick_count_t    interval        /* Relative retrigger interval       */
)
{
    ((Cyg_Alarm *)alarm)->initialize(
        (cyg_tick_count)trigger,
        (cyg_tick_count)interval);
}

externC void cyg_alarm_enable( cyg_handle_t alarm )
{
    ((Cyg_Alarm *)alarm)->enable();
}

externC void cyg_alarm_disable( cyg_handle_t alarm )
{
    ((Cyg_Alarm *)alarm)->disable();
}

/*---------------------------------------------------------------------------*/
/* Mail boxes                                                                */

externC void cyg_mbox_create(
    cyg_handle_t        *handle,
    cyg_mbox            *mbox
)
{
    CYG_ASSERT_SIZES( cyg_mbox, Cyg_Mbox );
    
    Cyg_Mbox *t = new((void *)mbox) Cyg_Mbox();
    t=t;

    CYG_CHECK_DATA_PTR( handle, "Bad handle pointer" );
    *handle = (cyg_handle_t)mbox;
}

externC void cyg_mbox_delete(cyg_handle_t mbox)
{
    ((Cyg_Mbox *)mbox)->~Cyg_Mbox();
}

externC void *cyg_mbox_get(cyg_handle_t mbox)
{
    return ((Cyg_Mbox *)mbox)->get();
}

#ifdef CYGFUN_KERNEL_THREADS_TIMER
void *cyg_mbox_timed_get(
    cyg_handle_t mbox,
    cyg_tick_count_t abstime
    )
{
    return ((Cyg_Mbox *)mbox)->get(abstime);
}
#endif

externC void *cyg_mbox_tryget(cyg_handle_t mbox)
{
    return ((Cyg_Mbox *)mbox)->tryget();
}

externC void *cyg_mbox_peek_item(cyg_handle_t mbox)
{
    return ((Cyg_Mbox *)mbox)->peek_item();
}

#ifdef CYGMFN_KERNEL_SYNCH_MBOXT_PUT_CAN_WAIT
externC cyg_bool_t cyg_mbox_put(cyg_handle_t mbox, void *item)
{
    return ((Cyg_Mbox *)mbox)->put(item);
}

#ifdef CYGFUN_KERNEL_THREADS_TIMER
externC cyg_bool_t cyg_mbox_timed_put(
    cyg_handle_t mbox,
    void *item,
    cyg_tick_count_t abstime
    )
{
    return ((Cyg_Mbox *)mbox)->put(item, abstime);
}
#endif
#endif

externC cyg_bool_t cyg_mbox_tryput(cyg_handle_t mbox, void *item)
{
    return ((Cyg_Mbox *)mbox)->tryput(item);
}

externC cyg_count32 cyg_mbox_peek(cyg_handle_t mbox)
{
    return ((Cyg_Mbox *)mbox)->peek();
}

externC cyg_bool_t cyg_mbox_waiting_to_get(cyg_handle_t mbox)
{
    return ((Cyg_Mbox *)mbox)->waiting_to_get();
}

externC cyg_bool_t cyg_mbox_waiting_to_put(cyg_handle_t mbox)
{
    return ((Cyg_Mbox *)mbox)->waiting_to_put();
}


/*---------------------------------------------------------------------------*/
/* Semaphores                                                                */

externC void      cyg_semaphore_init(
    cyg_sem_t           *sem,            /* Semaphore to init                */
    cyg_count32         val              /* Initial semaphore value          */
)
{
    CYG_ASSERT_SIZES( cyg_sem_t, Cyg_Counting_Semaphore );
    
    Cyg_Counting_Semaphore *t = new((void *)sem) Cyg_Counting_Semaphore(val);
    t=t;
}

externC void cyg_semaphore_destroy( cyg_sem_t *sem )
{
    ((Cyg_Counting_Semaphore *)sem)->~Cyg_Counting_Semaphore();
}

externC cyg_bool_t cyg_semaphore_wait( cyg_sem_t *sem )
{
    return ((Cyg_Counting_Semaphore *)sem)->wait();
}

#ifdef CYGFUN_KERNEL_THREADS_TIMER
externC cyg_bool_t cyg_semaphore_timed_wait(
    cyg_sem_t          *sem,
    cyg_tick_count_t   abstime
    )
{
    return ((Cyg_Counting_Semaphore *)sem)->wait(abstime);
}
#endif


externC int cyg_semaphore_trywait( cyg_sem_t *sem )
{
    return ((Cyg_Counting_Semaphore *)sem)->trywait();
}

externC void cyg_semaphore_post( cyg_sem_t *sem )
{
    ((Cyg_Counting_Semaphore *)sem)->post();
}

externC void cyg_semaphore_peek( cyg_sem_t *sem, cyg_count32 *val )
{
    CYG_CHECK_DATA_PTR( val, "Bad val parameter" );

    *val = ((Cyg_Counting_Semaphore *)sem)->peek();
}


/*---------------------------------------------------------------------------*/
/* Flags                                                                     */

void cyg_flag_init(
    cyg_flag_t        *flag             /* Flag to init                      */
)
{
    CYG_ASSERT_SIZES( cyg_flag_t, Cyg_Flag );
    CYG_ASSERT(
        ( Cyg_Flag::AND == CYG_FLAG_WAITMODE_AND ) &&
        ( Cyg_Flag::OR  == CYG_FLAG_WAITMODE_OR  ) &&
        ( Cyg_Flag::CLR == CYG_FLAG_WAITMODE_CLR ),
        "CYG_FLAG_WAITMODE_xxx definition != C++ Cyg_Flag::xxx" );
    
    Cyg_Flag *t = new((void *)flag) Cyg_Flag();
    t=t;
}

void cyg_flag_destroy( cyg_flag_t *flag )
{
    ((Cyg_Flag *)flag)->~Cyg_Flag();
}

void cyg_flag_setbits( cyg_flag_t *flag, cyg_flag_value_t value)
{
    ((Cyg_Flag *)flag)->setbits( value ); 
}

void cyg_flag_maskbits( cyg_flag_t *flag, cyg_flag_value_t value)
{
    ((Cyg_Flag *)flag)->maskbits( value ); 
}

cyg_flag_value_t cyg_flag_wait( cyg_flag_t        *flag,
                                cyg_flag_value_t   pattern, 
                                cyg_flag_mode_t    mode )
{
    if ( 0 == pattern || 0 != (mode & ~3) )
        return 0;
    return ((Cyg_Flag *)flag)->wait( pattern, mode );

}

#ifdef CYGFUN_KERNEL_THREADS_TIMER
cyg_flag_value_t cyg_flag_timed_wait( cyg_flag_t        *flag,
                                      cyg_flag_value_t   pattern, 
                                      cyg_flag_mode_t    mode,
                                      cyg_tick_count_t   abstime )
{
    if ( 0 == pattern || 0 != (mode & ~3) )
        return 0;
    return ((Cyg_Flag *)flag)->wait( pattern, mode, abstime );

}
#endif

cyg_flag_value_t cyg_flag_poll( cyg_flag_t         *flag,
                                cyg_flag_value_t    pattern, 
                                cyg_flag_mode_t     mode )
{
    if ( 0 == pattern || 0 != (mode & ~3) )
        return 0;
    return ((Cyg_Flag *)flag)->poll( pattern, mode );

}

cyg_flag_value_t cyg_flag_peek( cyg_flag_t *flag )
{
    return ((Cyg_Flag *)flag)->peek();
}

cyg_bool_t cyg_flag_waiting( cyg_flag_t *flag )
{
    return ((Cyg_Flag *)flag)->waiting();
}

/*---------------------------------------------------------------------------*/
/* Mutex                                                                     */

externC void cyg_mutex_init(
    cyg_mutex_t        *mutex          /* Mutex to init                      */
)
{
    CYG_ASSERT_SIZES( cyg_mutex_t, Cyg_Mutex );
    
    Cyg_Mutex *m = new((void *)mutex) Cyg_Mutex;

    m=m;
}

externC void cyg_mutex_destroy( cyg_mutex_t *mutex )
{
    ((Cyg_Mutex *)mutex)->~Cyg_Mutex();
}

externC cyg_bool_t cyg_mutex_lock( cyg_mutex_t *mutex )
{
    return ((Cyg_Mutex *)mutex)->lock();
}

externC cyg_bool_t cyg_mutex_trylock( cyg_mutex_t *mutex )
{
    return ((Cyg_Mutex *)mutex)->trylock();
}

externC void cyg_mutex_unlock( cyg_mutex_t *mutex )
{
    ((Cyg_Mutex *)mutex)->unlock();
}

externC void cyg_mutex_release( cyg_mutex_t *mutex )
{
    ((Cyg_Mutex *)mutex)->release();
}

#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_CEILING
externC void cyg_mutex_set_ceiling( 
    cyg_mutex_t *mutex, 
    cyg_priority_t priority )
{
    ((Cyg_Mutex *)mutex)->set_ceiling(priority);
}
#endif

#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_DYNAMIC
externC void cyg_mutex_set_protocol( 
    cyg_mutex_t *mutex,
    enum cyg_mutex_protocol protocol )
{
    ((Cyg_Mutex *)mutex)->set_protocol((Cyg_Mutex::cyg_protcol)protocol);
}
#endif

/*---------------------------------------------------------------------------*/
/* Condition Variables                                                       */

externC void cyg_cond_init(
    cyg_cond_t          *cond,          /* condition variable to init        */
    cyg_mutex_t         *mutex          /* associated mutex                  */
)
{
    CYG_ASSERT_SIZES( cyg_cond_t, Cyg_Condition_Variable );
    
    Cyg_Condition_Variable *t = new((void *)cond) Cyg_Condition_Variable(
        *(Cyg_Mutex *)mutex);
    t=t;
}

externC void cyg_cond_destroy( cyg_cond_t *cond )
{
    ((Cyg_Condition_Variable *)cond)->~Cyg_Condition_Variable();
}

externC cyg_bool_t cyg_cond_wait( cyg_cond_t *cond )
{
    return ((Cyg_Condition_Variable *)cond)->wait();
}

externC void cyg_cond_signal( cyg_cond_t *cond )
{
    ((Cyg_Condition_Variable *)cond)->signal();
}

externC void cyg_cond_broadcast( cyg_cond_t *cond )
{
    ((Cyg_Condition_Variable *)cond)->broadcast();
}

#ifdef CYGMFN_KERNEL_SYNCH_CONDVAR_TIMED_WAIT
externC cyg_bool_t cyg_cond_timed_wait(
    cyg_cond_t        *cond,
    cyg_tick_count_t  abstime
    )
{
    return ((Cyg_Condition_Variable *)cond)->wait(abstime);
}

#endif

/*---------------------------------------------------------------------------*/
/* Spinlocks                                                                 */

externC void cyg_spinlock_init(
    cyg_spinlock_t      *lock,          /* spinlock to initialize            */
    cyg_bool_t          locked          /* init locked or unlocked           */
)
{
    CYG_ASSERT_SIZES( cyg_spinlock_t, Cyg_SpinLock );

    // Create the spinlock in cleared state
    Cyg_SpinLock *t = new((void *)lock) Cyg_SpinLock();

    // If the lock is to start locked, then lock it now.
    if( locked )
        t->spin();
}

externC void cyg_spinlock_destroy( cyg_spinlock_t *lock )
{
    ((Cyg_SpinLock *)lock)->~Cyg_SpinLock();
}

externC void cyg_spinlock_spin( cyg_spinlock_t *lock )
{
    ((Cyg_SpinLock *)lock)->spin();
}

externC void cyg_spinlock_clear( cyg_spinlock_t *lock )
{
    ((Cyg_SpinLock *)lock)->clear();
}

externC cyg_bool_t cyg_spinlock_try( cyg_spinlock_t *lock )
{
    return ((Cyg_SpinLock *)lock)->trylock();
}

externC cyg_bool_t cyg_spinlock_test( cyg_spinlock_t *lock )
{
    return ((Cyg_SpinLock *)lock)->test();
}

externC void cyg_spinlock_spin_intsave( cyg_spinlock_t *lock,
                                cyg_addrword_t *istate )
{
    ((Cyg_SpinLock *)lock)->spin_intsave((CYG_INTERRUPT_STATE *)istate);
}
    
externC void cyg_spinlock_clear_intsave( cyg_spinlock_t *lock,
                                 cyg_addrword_t istate )
{
    ((Cyg_SpinLock *)lock)->clear_intsave((CYG_INTERRUPT_STATE)istate);
}
    

// -------------------------------------------------------------------------
// Check structure sizes.
// This class and constructor get run automatically in debug versions
// of the kernel and check that the structures configured in the C
// code are the same size as the C++ classes they should match.

#ifdef CYGPKG_INFRA_DEBUG

class Cyg_Check_Structure_Sizes
{
    int dummy;
public:    
    Cyg_Check_Structure_Sizes( int x );

};

#define CYG_CHECK_SIZES(cstruct, cxxstruct)                               \
if( sizeof(cstruct) != sizeof(cxxstruct) )                                \
{                                                                         \
    char *fmt = "Size of C struct " #cstruct                              \
                " != size of C++ struct " #cxxstruct ;                    \
    CYG_TRACE2(1, fmt, sizeof(cstruct) , sizeof(cxxstruct) );             \
    fail = true;                                                          \
    fmt = fmt;                                                            \
}

Cyg_Check_Structure_Sizes::Cyg_Check_Structure_Sizes(int x)
{
    cyg_bool fail = false;

    dummy = x+1;
    
    CYG_CHECK_SIZES( cyg_thread, Cyg_Thread );
    CYG_CHECK_SIZES( cyg_interrupt, Cyg_Interrupt );
    CYG_CHECK_SIZES( cyg_counter, Cyg_Counter );
    CYG_CHECK_SIZES( cyg_clock, Cyg_Clock );
    CYG_CHECK_SIZES( cyg_alarm, Cyg_Alarm );
    CYG_CHECK_SIZES( cyg_mbox, Cyg_Mbox );
    CYG_CHECK_SIZES( cyg_sem_t, Cyg_Counting_Semaphore );
    CYG_CHECK_SIZES( cyg_flag_t, Cyg_Flag );
    CYG_CHECK_SIZES( cyg_mutex_t, Cyg_Mutex );
    CYG_CHECK_SIZES( cyg_cond_t, Cyg_Condition_Variable );
    CYG_CHECK_SIZES( cyg_spinlock_t, Cyg_SpinLock );
    
    CYG_ASSERT( !fail, "Size checks failed");
}

static Cyg_Check_Structure_Sizes cyg_kapi_check_structure_sizes(1);

#endif


// -------------------------------------------------------------------------

#endif
// EOF common/kapi.cxx
