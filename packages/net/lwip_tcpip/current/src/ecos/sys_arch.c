//==========================================================================
//
//      sys_arch.c
//
//      lwIP system architecture support.
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008, 2009 Free Software Foundation
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Simon Kallweit
// Contributors:
// Date:         2008-12-02
// Purpose:
// Description:  Provides the system architecture support for lwIP.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/kernel/kapi.h>

#include <lwip.h>

#include "lwip/opt.h"
#include "arch/sys_arch.h"
#include "lwip/sys.h"
#include "lwip/def.h"
#include "lwip/stats.h"
#include "lwip/debug.h"

// Milliseconds per system tick
#define MS_PER_TICK ((u32_t) (CYGNUM_HAL_RTC_NUMERATOR / \
                     (CYGNUM_HAL_RTC_DENOMINATOR * 1000000LL)))

// Macros to convert between ticks and milliseconds
#define TICKS_TO_MS(_ticks_)    ((u16_t) ((_ticks_) * MS_PER_TICK + 1))
#define MS_TO_TICKS(_ms_)       ((cyg_tick_count_t) (((_ms_) + \
                                 (MS_PER_TICK - 1)) / MS_PER_TICK))

#if !NO_SYS

// Thread structure
struct lwip_thread {
    struct lwip_thread *next;  // Next thread in linked list
    struct sys_timeouts to;    // List of timeouts
    cyg_handle_t handle;       // Thread handle
    cyg_thread thread;         // Thread store
};

// A var memory pool is used for allocating semaphores, mboxes and threads
static char var_data[CYGNUM_LWIP_VARMEMPOOL_SIZE];
static cyg_mempool_var var_mempool;
static cyg_handle_t var_handle;

// Internal lwip thread stacks are statically allocated
#define TOTAL_STACKSIZE     (TCPIP_THREAD_STACKSIZE +       \
                             SLIPIF_THREAD_STACKSIZE +      \
                             PPP_THREAD_STACKSIZE +         \
                             ETH_THREAD_STACKSIZE)

static cyg_mutex_t stack_mutex;
static char stack_data[TOTAL_STACKSIZE];
static char *stack_pos = stack_data;

// Timeout for threads which were not created by sys_thread_new()
static struct sys_timeouts to;

// List of threads
static struct lwip_thread *threads;

//
// Is called to initialize the sys_arch layer.
//
void
sys_init(void)
{
    cyg_mempool_var_create(
	    var_data,
	    sizeof(var_data),
	    &var_handle,
	    &var_mempool
	);
	
	threads = NULL;
	to.next = NULL;
	
	cyg_mutex_init(&stack_mutex);
}

//
// Creates and returns a new semaphore. The "count" argument specifies the
// initial state of the semaphore.
//
sys_sem_t
sys_sem_new(u8_t count)
{
    sys_sem_t sem;

    // Allocate semaphore
    sem = (cyg_sem_t *) cyg_mempool_var_try_alloc(var_handle, sizeof(cyg_sem_t));
    if (!sem)
        return SYS_SEM_NULL;
    cyg_semaphore_init(sem, count);

#if SYS_STATS
    lwip_stats.sys.sem.used++;
    if (lwip_stats.sys.sem.used > lwip_stats.sys.sem.max)
        lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
#endif

    return sem;
}

//
// Deallocates a semaphore.
//
void
sys_sem_free(sys_sem_t sem)
{
    if (!sem)
        return;

    cyg_semaphore_destroy(sem);
    cyg_mempool_var_free(var_handle, (void *) sem);

#if SYS_STATS
    lwip_stats.sys.sem.used--;
#endif
}

//
// Signals a semaphore.
//
void
sys_sem_signal(sys_sem_t sem)
{
    cyg_semaphore_post(sem);
}

//
// Blocks the thread while waiting for the semaphore to be signaled. If the
// "timeout" argument is non-zero, the thread should only be blocked for the
// specified time (measured in milliseconds). If the "timeout" argument is
// zero, the thread should be blocked until the semaphore is signalled.
//
// If the timeout argument is non-zero, the return value is the number of
// milliseconds spent waiting for the semaphore to be signaled. If the
// semaphore wasn't signaled within the specified time, the return value is
// SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
// (i.e., it was already signaled), the function may return zero.
//
// Notice that lwIP implements a function with a similar name, sys_sem_wait(),
// that uses the sys_arch_sem_wait() function.
//
u32_t
sys_arch_sem_wait(sys_sem_t sem, u32_t timeout)
{
    if (timeout) {
        cyg_tick_count_t start_time = cyg_current_time();

        // Wait for semaphore with timeout
        if (!cyg_semaphore_timed_wait(sem, start_time + MS_TO_TICKS(timeout)))
            return SYS_ARCH_TIMEOUT;
        // Return elapsed time
        return TICKS_TO_MS(cyg_current_time() - start_time);
    } else {
        // Wait for semaphore indefinitely
        cyg_semaphore_wait(sem);
        return 0;
    }
}

//
// Creates an empty mailbox for maximum "size" elements. Elements stored in
// mailboxes are pointers. You have to define macros "_MBOX_SIZE" in your
// lwipopts.h, or ignore this parameter in your implementation and use a
// default size.
//
sys_mbox_t
sys_mbox_new(int size)
{
	cyg_mbox *mbox;
	cyg_handle_t handle;

    LWIP_UNUSED_ARG(size);

	mbox = (cyg_mbox *) cyg_mempool_var_try_alloc(var_handle, sizeof(cyg_mbox));
	if (!mbox)
	    return SYS_MBOX_NULL;
	cyg_mbox_create(&handle, mbox);

#if SYS_STATS
    lwip_stats.sys.mbox.used++;
    if (lwip_stats.sys.mbox.used > lwip_stats.sys.mbox.max)
        lwip_stats.sys.mbox.max = lwip_stats.sys.mbox.used;
#endif

	return handle;
}

//
// Deallocates a mailbox. If there are messages still present in the mailbox
// when the mailbox is deallocated, it is an indication of a programming error
// in lwIP and the developer should be notified.
//
void
sys_mbox_free(sys_mbox_t mbox)
{
    if (!mbox)
        return;

    if (cyg_mbox_peek(mbox))
        LWIP_DEBUGF(SYS_DEBUG | LWIP_DBG_LEVEL_WARNING,
                    ("sys_mbox_free: mbox not empty\n"));

	cyg_mbox_delete(mbox);
	cyg_mempool_var_free(var_handle, (void *) mbox);

#if SYS_STATS
    lwip_stats.sys.mbox.used--;
#endif
}

//
// cyg_mbox_put() should not be passed a NULL, otherwise the cyg_mbox_get()
// will not know if it's real data or an error condition. But lwIP does pass
// NULL on occasion, in cases when maybe using a semaphore would be better. So
// this null_msg replaces NULL data.
//
static int null_msg;

//
// Posts the "msg" to the mailbox. This function have to block until the "msg"
// is really posted.
//
void
sys_mbox_post(sys_mbox_t mbox, void *msg)
{
    // Map NULL messages
    if (!msg)
        msg = &null_msg;
    while (cyg_mbox_put(mbox, msg) == false);
}

//
// Try to post the "msg" to the mailbox. Returns ERR_MEM if this one is full,
// else, ERR_OK if the "msg" is posted.
//
err_t
sys_mbox_trypost(sys_mbox_t mbox, void *msg)
{
    // Map NULL messages
    if (!msg)
        msg = &null_msg;
    return cyg_mbox_tryput(mbox, msg) ? ERR_OK : ERR_MEM;
}

//
// Blocks the thread until a message arrives in the mailbox, but does not block
// the thread longer than "timeout" milliseconds (similar to the
// sys_arch_sem_wait() function). If "timeout" is 0, the thread should be
// blocked until a message arrives. The "msg" argument is a result parameter
// that is set by the function (i.e., by doing "*msg = ptr"). The "msg"
// parameter maybe NULL to indicate that the message should be dropped.
//
// The return values are the same as for the sys_arch_sem_wait() function:
// Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
// timeout.
//
// Note that a function with a similar name, sys_mbox_fetch(), is implemented
// by lwIP.
//
u32_t
sys_arch_mbox_fetch(sys_mbox_t mbox, void **msg, u32_t timeout)
{
    void *m;

    if (timeout) {
        cyg_tick_count_t start_time = cyg_current_time();

        // Wait for mailbox with timeout
        if (!(m = cyg_mbox_timed_get(mbox, start_time + MS_TO_TICKS(timeout))))
            return SYS_ARCH_TIMEOUT;
        // Map NULL messages
        if (m == &null_msg)
            m = NULL;
        *msg = m;
        // Return elapsed time
        return TICKS_TO_MS(cyg_current_time() - start_time);
    } else {
        // Wait for semaphore indefinitely
        m = cyg_mbox_get(mbox);
        // Map NULL messages
        if (m == &null_msg)
            m = NULL;
        *msg = m;
        return 0;
    }
}

//
// This is similar to sys_arch_mbox_fetch, however if a message is not present
// in the mailbox, it immediately returns with the code SYS_MBOX_EMPTY. On
// success 0 is returned.
//
// To allow for efficient implementations, this can be defined as a
// function-like macro in sys_arch.h instead of a normal function. For example,
// a naive implementation could be:
//
// #define sys_arch_mbox_tryfetch(mbox,msg) sys_arch_mbox_fetch(mbox,msg,1)
//
// although this would introduce unnecessary delays.
//
u32_t
sys_arch_mbox_tryfetch(sys_mbox_t mbox, void **msg)
{
    void *m;

    m = cyg_mbox_tryget(mbox);
    if (!m)
        return SYS_MBOX_EMPTY;

    if (m == &null_msg)
        m = NULL;
    *msg = m;

    return 0;
}

//
// Returns a pointer to the per-thread sys_timeouts structure. In lwIP, each
// thread has a list of timeouts which is repressented as a linked list of
// sys_timeout structures. The sys_timeouts structure holds a pointer to a
// linked list of timeouts. This function is called by the lwIP timeout
// scheduler and must not return a NULL value.
//
// In a single thread sys_arch implementation, this function will simply return
// a pointer to a global sys_timeouts variable stored in the sys_arch module.
//
struct sys_timeouts *
sys_arch_timeouts(void)
{
    cyg_handle_t handle;
    struct lwip_thread *t;

    handle = cyg_thread_self();
    for (t = threads; t; t = t->next)
        if (t->handle == handle)
            return &(t->to);

    return &to;
}

//
// Starts a new thread named "name" with priority "prio" that will begin its
// execution in the function "thread()". The "arg" argument will be passed as
// an argument to the thread() function. The stack size to used for this thread
// is the "stacksize" parameter. The id of the new thread is returned. Both the
// id and the priority are system dependent.
//
sys_thread_t
sys_thread_new(char *name, void (* thread)(void *arg), void *arg,
               int stacksize, int prio)
{
    void *stack;

    cyg_mutex_lock(&stack_mutex);
    stack = stack_pos;
    stack_pos += stacksize;
    cyg_mutex_unlock(&stack_mutex);
    
    if (stack_pos > stack_data + TOTAL_STACKSIZE)
        CYG_FAIL("Not enough memory to allocate the thread's stack. You may "
                 "want to use cyg_lwip_thread_new() instead of "
                 "sys_thread_new() so you can provide external stack memory.");

    return cyg_lwip_thread_new(name, thread, arg, stack, stacksize, prio);
}

//
// Basically implements the sys_thread_new() call, but adds a "stack" parameter,
// allowing clients to provide their own stack buffers.
//
sys_thread_t
cyg_lwip_thread_new(char *name, void (* thread)(void *arg), void *arg,
                    void *stack, int stacksize, int prio)
{
    struct lwip_thread *t;

    t = (struct lwip_thread *) cyg_mempool_var_alloc(
            var_handle, sizeof(struct lwip_thread));
    
    t->next = threads;
    t->to.next = NULL;

    threads = t;

    cyg_thread_create(
        prio,
        (cyg_thread_entry_t *) thread,
        (cyg_addrword_t) arg,
        name,
        stack,
        stacksize,
        &t->handle,
        &t->thread
    );
    cyg_thread_resume(t->handle);

    return t->handle;
}

#endif // !NO_SYS

//
// Returns the current time in milliseconds.
//
u32_t
sys_now(void)
{
    return cyg_current_time() * MS_PER_TICK;
}

//
// This optional function does a "fast" critical region protection and returns
// the previous protection level. This function is only called during very short
// critical regions. An embedded system which supports ISR-based drivers might
// want to implement this function by disabling interrupts. Task-based systems
// might want to implement this by using a mutex or disabling tasking. This
// function should support recursive calls from the same task or interrupt. In
// other words, sys_arch_protect() could be called while already protected. In
// that case the return value indicates that it is already protected.
//
// sys_arch_protect() is only required if your port is supporting an operating
// system.
//
sys_prot_t
sys_arch_protect(void)
{
    cyg_scheduler_lock();
    
    return 0;
}

//
// This optional function does a "fast" set of critical region protection to the
// value specified by pval. See the documentation for sys_arch_protect() for
// more information. This function is only required if your port is supporting
// an operating system.
//
void
sys_arch_unprotect(sys_prot_t pval)
{
    LWIP_UNUSED_ARG(pval);
    
    cyg_scheduler_unlock();
}
