#ifndef CYGONCE_KERNEL_KAPI_H
#define CYGONCE_KERNEL_KAPI_H

/*==========================================================================
//
//      kapi.h
//
//      Native API for Kernel
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg, dsm
// Contributors:        nickg
// Date:        1998-03-02
// Purpose:     Native API for Kernel
// Description: This file describes the native API for using the kernel.
//              It is essentially a set of C wrappers for the C++ class
//              member functions.
// Usage:       #include <cyg/kernel/kapi.h>
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/kernel.h>

#ifdef CYGFUN_KERNEL_API_C
#include <cyg/infra/cyg_type.h>

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/
/* The following are derived types, they may have different                  */
/* definitions from these depending on configuration.                        */

typedef CYG_ADDRWORD   cyg_addrword_t;      /* May hold pointer or word      */
typedef cyg_addrword_t cyg_handle_t;        /* Object handle                 */
typedef cyg_uint32     cyg_priority_t;      /* type for priorities           */
typedef cyg_int32      cyg_code_t;          /* type for various codes        */
typedef cyg_uint32     cyg_vector_t;        /* Interrupt vector id           */

typedef cyg_uint64 cyg_tick_count_t;

typedef int cyg_bool_t;

/* Exception handler function definition                                     */
typedef void cyg_exception_handler_t(
    cyg_addrword_t data,
    cyg_code_t   exception_number,
    cyg_addrword_t info
);

/*---------------------------------------------------------------------------*/
struct cyg_thread;
typedef struct cyg_thread cyg_thread;

struct cyg_interrupt;
typedef struct cyg_interrupt cyg_interrupt;

struct cyg_counter;
typedef struct cyg_counter cyg_counter;

struct cyg_clock;
typedef struct cyg_clock cyg_clock;

struct cyg_alarm;
typedef struct cyg_alarm cyg_alarm;

struct cyg_mbox;
typedef struct cyg_mbox cyg_mbox;

struct cyg_mempool_var;
typedef struct cyg_mempool_var cyg_mempool_var;

struct cyg_mempool_fix;
typedef struct cyg_mempool_fix cyg_mempool_fix;

struct cyg_sem_t;
typedef struct cyg_sem_t cyg_sem_t;

struct cyg_flag_t;
typedef struct cyg_flag_t cyg_flag_t;

struct cyg_mutex_t;
typedef struct cyg_mutex_t cyg_mutex_t;

struct cyg_cond_t;
typedef struct cyg_cond_t cyg_cond_t;

/*---------------------------------------------------------------------------*/
/* Scheduler operations */

/* Starts scheduler with created threads.  Never returns. */
void cyg_scheduler_start(void) __attribute__ ((noreturn));

/* Lock and unlock the scheduler. When the scheduler is   */
/* locked thread preemption is disabled.                  */
void cyg_scheduler_lock(void);

void cyg_scheduler_unlock(void);
    
/*---------------------------------------------------------------------------*/
/* Thread operations */

typedef void cyg_thread_entry_t(cyg_addrword_t);

void cyg_thread_create(
    cyg_addrword_t      sched_info,             /* scheduling info (eg pri)  */
    cyg_thread_entry_t  *entry,                 /* entry point function      */
    cyg_addrword_t      entry_data,             /* entry data                */
    char                *name,                  /* optional thread name      */
    void                *stack_base,            /* stack base, NULL = alloc  */
    cyg_ucount32        stack_size,             /* stack size, 0 = default   */
    cyg_handle_t        *handle,                /* returned thread handle    */
    cyg_thread          *thread                 /* put thread here           */
);
    
void cyg_thread_exit(void);

/* It may be necessary to arrange for the victim to run for it to disappear */
cyg_bool_t cyg_thread_delete(cyg_handle_t thread); /* false if NOT deleted */

void cyg_thread_suspend(cyg_handle_t thread);

void cyg_thread_resume(cyg_handle_t thread);

void cyg_thread_kill(cyg_handle_t thread);

void cyg_thread_release(cyg_handle_t thread);    
    
void cyg_thread_yield(void);

cyg_handle_t cyg_thread_self(void);


/* Priority manipulation */

void cyg_thread_set_priority(cyg_handle_t thread, cyg_priority_t priority );

cyg_priority_t cyg_thread_get_priority(cyg_handle_t thread);              

/* Deadline scheduling control (optional) */

void cyg_thread_deadline_wait( 
    cyg_tick_count_t    start_time,             /* abs earliest start time   */
    cyg_tick_count_t    run_time,               /* worst case execution time */
    cyg_tick_count_t    deadline                /* absolute deadline         */
); 

void cyg_thread_delay(cyg_tick_count_t delay);

/*---------------------------------------------------------------------------*/
/* Per-thread Data                                                           */

#ifdef CYGVAR_KERNEL_THREADS_DATA

cyg_ucount32 cyg_thread_new_data_index(void);

void cyg_thread_free_data_index(cyg_ucount32 index);

CYG_ADDRWORD cyg_thread_get_data(cyg_ucount32 index);

CYG_ADDRWORD *cyg_thread_get_data_ptr(cyg_ucount32 index);

void cyg_thread_set_data(cyg_ucount32 index, CYG_ADDRWORD data);

#endif
    
/*---------------------------------------------------------------------------*/
/* Exception handling.                                                       */

/* Replace current exception handler, this may apply to either the           */
/* current thread, or to a global exception handler. The exception           */
/* number may be ignored, or used to specify a particular handler.           */

void cyg_exception_set_handler(
    cyg_code_t                  exception_number,
    cyg_exception_handler_t     *new_handler,
    cyg_addrword_t                new_data,
    cyg_exception_handler_t     **old_handler,
    cyg_addrword_t                *old_data
);

/* Clear exception hander to default value                                   */
void cyg_exception_clear_handler(
    cyg_code_t                  exception_number
);
    
/* Invoke exception handler                                                  */
void cyg_exception_call_handler(
    cyg_handle_t                thread,
    cyg_code_t                  exception_number,
    cyg_addrword_t              exception_info
);


/*---------------------------------------------------------------------------*/
/* Interrupt handling                                                        */
typedef void            cyg_VSR_t(void);
typedef cyg_uint32      cyg_ISR_t(cyg_vector_t vector, cyg_addrword_t data);
typedef void            cyg_DSR_t(
    cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);


enum cyg_ISR_results
{
    CYG_ISR_HANDLED  = 1,               /* Interrupt was handled             */
    CYG_ISR_CALL_DSR = 2                /* Schedule DSR                      */
};

void cyg_interrupt_create(
    cyg_vector_t        vector,         /* Vector to attach to               */
    cyg_priority_t      priority,       /* Queue priority                    */
    cyg_addrword_t      data,           /* Data pointer                      */
    cyg_ISR_t           *isr,           /* Interrupt Service Routine         */
    cyg_DSR_t           *dsr,           /* Deferred Service Routine          */
    cyg_handle_t        *handle,        /* returned handle                   */
    cyg_interrupt       *intr           /* put interrupt here                */
);

void cyg_interrupt_delete( cyg_handle_t interrupt );

void cyg_interrupt_attach( cyg_handle_t interrupt );

void cyg_interrupt_detach( cyg_handle_t interrupt );
    
/* VSR manipulation */

void cyg_interrupt_get_vsr(
    cyg_vector_t        vector,         /* vector to get                     */
    cyg_VSR_t           **vsr           /* vsr got                           */
);

void cyg_interrupt_set_vsr(
    cyg_vector_t        vector,         /* vector to set                     */
    cyg_VSR_t           *vsr            /* vsr to set                        */
);

/* CPU level interrupt mask                                                  */
void cyg_interrupt_disable(void);

void cyg_interrupt_enable(void);

/* Interrupt controller access                                               */
void cyg_interrupt_mask(cyg_vector_t vector);

void cyg_interrupt_unmask(cyg_vector_t vector);

void cyg_interrupt_acknowledge(cyg_vector_t vector);

void cyg_interrupt_configure(
    cyg_vector_t        vector,         /* vector to configure               */
    cyg_bool_t          level,          /* level or edge triggered           */
    cyg_bool_t          up              /* rising/faling edge, high/low level*/
);

/*---------------------------------------------------------------------------*/
/* Counters, Clocks and Alarms                                               */

void cyg_counter_create(
    cyg_handle_t        *handle,        /* returned counter handle           */
    cyg_counter         *counter        /* put counter here                  */
);

void cyg_counter_delete(cyg_handle_t counter);

/* Return current value of counter                                           */
cyg_tick_count_t cyg_counter_current_value(cyg_handle_t counter);

/* Set new current value                                                     */
void cyg_counter_set_value(
    cyg_handle_t        counter,
    cyg_tick_count_t new_value
);

/* Advance counter by one tick                                               */
void cyg_counter_tick(cyg_handle_t counter);


typedef struct 
{
    cyg_uint32  dividend;
    cyg_uint32  divisor;
} cyg_resolution_t;

/* Create a clock object                */
void cyg_clock_create(
    cyg_resolution_t    resolution,     /* Initial resolution                */
    cyg_handle_t        *handle,        /* Returned clock handle             */
    cyg_clock           *clock          /* put clock here                    */    
);

void cyg_clock_delete(cyg_handle_t clock);

/* convert a clock handle to a counter handle so we can use the              */
/* counter API on it.                                                        */
void cyg_clock_to_counter(
    cyg_handle_t        clock,
    cyg_handle_t        *counter
);

void cyg_clock_set_resolution(
    cyg_handle_t        clock,
    cyg_resolution_t    resolution      /* New resolution                    */
);

cyg_resolution_t cyg_clock_get_resolution(cyg_handle_t clock);

/* handle of real time clock                                                 */
cyg_handle_t cyg_real_time_clock(void);

/* returns value of real time clock's counter.
   This is the same as:
   (cyg_clock_to_counter(cyg_real_time_clock(), &h),
    cyg_counter_current_value(h))                                            */
cyg_tick_count_t cyg_current_time(void);

/* Alarm handler function                                                    */
typedef void cyg_alarm_t(cyg_handle_t alarm, cyg_addrword_t data);

void cyg_alarm_create(
    cyg_handle_t        counter,        /* Attached to this counter          */
    cyg_alarm_t         *alarmfn,       /* Call-back function                */
    cyg_addrword_t      data,           /* Call-back data                    */
    cyg_handle_t        *handle,        /* Returned alarm object             */
    cyg_alarm           *alarm          /* put alarm here                    */    
);

/* Disable alarm, detach from counter and invalidate handles                 */
void cyg_alarm_delete( cyg_handle_t alarm);

void cyg_alarm_initialize(
    cyg_handle_t        alarm,
    cyg_tick_count_t    trigger,        /* Absolute trigger time             */
    cyg_tick_count_t    interval        /* Relative retrigger interval       */
);

void cyg_alarm_enable( cyg_handle_t alarm );

void cyg_alarm_disable( cyg_handle_t alarm );

/*---------------------------------------------------------------------------*/
/* Mail boxes                                                                */
void cyg_mbox_create(
    cyg_handle_t        *handle,
    cyg_mbox            *mbox
);

void cyg_mbox_delete(cyg_handle_t mbox);

void *cyg_mbox_get(cyg_handle_t mbox);

#ifdef CYGFUN_KERNEL_THREADS_TIMER
void *cyg_mbox_timed_get(
    cyg_handle_t mbox,
    cyg_tick_count_t abstime
    );
#endif

void *cyg_mbox_tryget(cyg_handle_t mbox);

void *cyg_mbox_peek_item(cyg_handle_t mbox);

#ifdef CYGMFN_KERNEL_SYNCH_MBOXT_PUT_CAN_WAIT
cyg_bool_t cyg_mbox_put(cyg_handle_t mbox, void *item);
#ifdef CYGFUN_KERNEL_THREADS_TIMER
cyg_bool_t cyg_mbox_timed_put(
    cyg_handle_t mbox,
    void *item,
    cyg_tick_count_t abstime
    );
#endif
#endif

cyg_bool_t cyg_mbox_tryput(cyg_handle_t mbox, void *item);

cyg_count32 cyg_mbox_peek(cyg_handle_t mbox);

cyg_bool_t cyg_mbox_waiting_to_get(cyg_handle_t mbox);

cyg_bool_t cyg_mbox_waiting_to_put(cyg_handle_t mbox);


/*-----------------------------------------------------------------------*/
/* Memory pools                                                          */

/* There are two sorts of memory pools.  A variable size memory pool
   is for allocating blocks of any size.  A fixed size memory pool, has
   the block size specified when the pool is created, and only provides
   blocks of that size.  */

/* Create a variable size memory pool */
void cyg_mempool_var_create(
    void            *base,              /* base of memory to use for pool */
    cyg_int32       size,               /* size of memory in bytes        */
    cyg_handle_t    *handle,            /* returned handle of memory pool */
    cyg_mempool_var *var                /* space to put pool structure in */
    );

/* Delete variable size memory pool */
void cyg_mempool_var_delete(cyg_handle_t varpool);

/* Allocates a block of length size.  This waits if the memory is not
   currently available.  */
void *cyg_mempool_var_alloc(cyg_handle_t varpool, cyg_int32 size);

/* Allocates a block of length size.  This waits until abstime,
   if the memory is not already available.  NULL is returned if
   no memory is available. */
void *cyg_mempool_var_timed_alloc(
    cyg_handle_t     varpool,
    cyg_int32        size,
    cyg_tick_count_t abstime);

/* Allocates a block of length size.  NULL is returned if no memory is
   available. */
void *cyg_mempool_var_try_alloc(
    cyg_handle_t varpool,
    cyg_int32    size);

/* Frees memory back into variable size pool. */
void cyg_mempool_var_free(cyg_handle_t varpool, void *p);

/* Returns true if there are any threads waiting for memory in the
   given memory pool. */
cyg_bool_t cyg_mempool_var_waiting(cyg_handle_t varpool);

typedef struct {
    cyg_int32 totalmem;
    cyg_int32 freemem;
    void      *base;
    cyg_int32 size;
    cyg_int32 blocksize;
    cyg_int32 maxfree;                  // The largest free block
} cyg_mempool_info;

/* Puts information about a variable memory pool into the structure
   provided. */
void cyg_mempool_var_get_info(cyg_handle_t varpool, cyg_mempool_info *info);

/* Create a fixed size memory pool */
void cyg_mempool_fix_create(
    void            *base,              // base of memory to use for pool
    cyg_int32       size,               // size of memory in byte
    cyg_int32       blocksize,          // size of allocation in bytes
    cyg_handle_t    *handle,            // handle of memory pool
    cyg_mempool_fix *fix                // space to put pool structure in
    );

/* Delete fixed size memory pool */
void cyg_mempool_fix_delete(cyg_handle_t fixpool);

/* Allocates a block.  This waits if the memory is not
   currently available.  */
void *cyg_mempool_fix_alloc(cyg_handle_t fixpool);

/* Allocates a block.  This waits until abstime, if the memory
   is not already available.  NULL is returned if no memory is
   available. */
void *cyg_mempool_fix_timed_alloc(
    cyg_handle_t     fixpool,
    cyg_tick_count_t abstime);

/* Allocates a block.  NULL is returned if no memory is available. */
void *cyg_mempool_fix_try_alloc(cyg_handle_t fixpool);

/* Frees memory back into fixed size pool. */
void cyg_mempool_fix_free(cyg_handle_t fixpool, void *p);

/* Returns true if there are any threads waiting for memory in the
   given memory pool. */
cyg_bool_t cyg_mempool_fix_waiting(cyg_handle_t fixpool);

/* Puts information about a variable memory pool into the structure
   provided. */
void cyg_mempool_fix_get_info(cyg_handle_t fixpool, cyg_mempool_info *info);

/*---------------------------------------------------------------------------*/
/* Semaphores                                                                */

void      cyg_semaphore_init(
    cyg_sem_t           *sem,            /* Semaphore to init                */
    cyg_count32         val              /* Initial semaphore value          */
);

void cyg_semaphore_destroy( cyg_sem_t *sem );

void cyg_semaphore_wait( cyg_sem_t *sem );

#ifdef CYGFUN_KERNEL_THREADS_TIMER
cyg_bool_t cyg_semaphore_timed_wait(
    cyg_sem_t          *sem,
    cyg_tick_count_t   abstime
    );
#endif

cyg_bool_t cyg_semaphore_trywait( cyg_sem_t *sem );

void cyg_semaphore_post( cyg_sem_t *sem );

void cyg_semaphore_peek( cyg_sem_t *sem, cyg_count32 *val );

/*---------------------------------------------------------------------------*/
/* Flags                                                                     */

typedef cyg_uint32 cyg_flag_value_t;
typedef cyg_uint8  cyg_flag_mode_t;
#define CYG_FLAG_WAITMODE_AND ((cyg_flag_mode_t)0) /* all bits must be set */
#define CYG_FLAG_WAITMODE_OR  ((cyg_flag_mode_t)2) /* any bit must be set  */
#define CYG_FLAG_WAITMODE_CLR ((cyg_flag_mode_t)1) /* clear when satisfied */

void cyg_flag_init(
    cyg_flag_t        *flag             /* Flag to init                      */
);

void cyg_flag_destroy( cyg_flag_t *flag );

/* bitwise-or in the bits in value; awaken any waiting tasks whose
   condition is now satisfied */
void cyg_flag_setbits( cyg_flag_t *flag, cyg_flag_value_t value);

/* bitwise-and with the the bits in value; this clears the bits which
   are not set in value.  No waiting task can be awoken. */
void cyg_flag_maskbits( cyg_flag_t *flag, cyg_flag_value_t value);

/* wait for the flag value to match the pattern, according to the mode.
   If mode includes CLR, set the flag value to zero when
   our pattern is matched.  The return value is that which matched
   the request, or zero for an error/timeout return.
   Value must not itself be zero. */
cyg_flag_value_t cyg_flag_wait( cyg_flag_t        *flag,
                                cyg_flag_value_t   pattern, 
                                cyg_flag_mode_t    mode );

#ifdef CYGFUN_KERNEL_THREADS_TIMER
cyg_flag_value_t cyg_flag_timed_wait( cyg_flag_t        *flag,
                                      cyg_flag_value_t   pattern, 
                                      cyg_flag_mode_t    mode,
                                      cyg_tick_count_t   abstime );

#endif

cyg_flag_value_t cyg_flag_poll( cyg_flag_t         *flag,
                                cyg_flag_value_t    pattern, 
                                cyg_flag_mode_t     mode );

cyg_flag_value_t cyg_flag_peek( cyg_flag_t *flag );

cyg_bool_t cyg_flag_waiting( cyg_flag_t *flag );

/*---------------------------------------------------------------------------*/
/* Mutex                                                                     */

void cyg_mutex_init(
    cyg_mutex_t        *mutex          /* Mutex to init                      */
);

void cyg_mutex_destroy( cyg_mutex_t *mutex );

cyg_bool_t cyg_mutex_lock( cyg_mutex_t *mutex );

cyg_bool_t cyg_mutex_trylock( cyg_mutex_t *mutex );

void cyg_mutex_unlock( cyg_mutex_t *mutex );

void cyg_mutex_release( cyg_mutex_t *mutex );

/*---------------------------------------------------------------------------*/
/* Condition Variables                                                       */

void cyg_cond_init(
    cyg_cond_t          *cond,          /* condition variable to init        */
    cyg_mutex_t         *mutex          /* associated mutex                  */
);

void cyg_cond_destroy( cyg_cond_t *cond );

void cyg_cond_wait( cyg_cond_t *cond );

void cyg_cond_signal( cyg_cond_t *cond );

void cyg_cond_broadcast( cyg_cond_t *cond );

#ifdef CYGMFN_KERNEL_SYNCH_CONDVAR_TIMED_WAIT
cyg_bool_t cyg_cond_timed_wait(
    cyg_cond_t        *cond,
    cyg_tick_count_t  abstime
    );
#endif


#ifdef __cplusplus
}
#endif

#include <cyg/kernel/kapidata.h>

/*---------------------------------------------------------------------------*/
/* EOF kapi.h                                                                */
#endif /* CYGFUN_KERNEL_API_C   */
#endif /* CYGONCE_KERNEL_KAPI_H */
