//==========================================================================
//
//        tm_basic.cxx
//
//        Basic timing test / scaffolding
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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     gthomas
// Contributors:  gthomas
// Date:          1998-10-19
// Description:   Very simple timing test setup
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/sched.hxx>
#include <cyg/kernel/thread.hxx>
#include <cyg/kernel/thread.inl>
#include <cyg/kernel/mutex.hxx>
#include <cyg/kernel/sema.hxx>
#include <cyg/kernel/sched.inl>
#include <cyg/kernel/clock.hxx>
#include <cyg/kernel/clock.inl>
#include <cyg/kernel/kapi.h>

#include <cyg/infra/testcase.h>
#define NTHREADS 1
#include "testaux.hxx"

// Structure used to keep track of times
typedef struct fun_times {
    cyg_uint32 start;
    cyg_uint32 end;
} fun_times;

#define NSAMPLES         10
#define NTEST_THREADS    32
#define NTHREAD_SWITCHES 128
#define NMUTEXES         32
#define NMBOXES          32
#define NSEMAPHORES      32
#define NSCHEDS          128
#define NCOUNTERS        32
#define NALARMS          32

#define STACK_SIZE 2048     // Is this large enough?
static char stacks[NTEST_THREADS][STACK_SIZE];
static cyg_thread test_threads[NTEST_THREADS];
static cyg_handle_t threads[NTEST_THREADS];
static int overhead;
static cyg_sem_t synchro;
static fun_times thread_ft[NTEST_THREADS];

static fun_times test2_ft[NTHREAD_SWITCHES];

static cyg_mutex_t test_mutexes[NMUTEXES];
static fun_times mutex_ft[NMUTEXES];
static cyg_thread mutex_test_thread;
static cyg_handle_t mutex_test_thread_handle;

static cyg_mbox test_mboxes[NMBOXES];
static cyg_handle_t test_mbox_handles[NMBOXES];
static fun_times mbox_ft[NMBOXES];
static cyg_thread mbox_test_thread;
static cyg_handle_t mbox_test_thread_handle;

static cyg_sem_t test_semaphores[NSEMAPHORES];
static fun_times semaphore_ft[NSEMAPHORES];
static cyg_thread semaphore_test_thread;
static cyg_handle_t semaphore_test_thread_handle;

static fun_times sched_ft[NSCHEDS];

static cyg_counter test_counters[NCOUNTERS];
static cyg_handle_t counters[NCOUNTERS];
static fun_times counter_ft[NCOUNTERS];

static cyg_alarm test_alarms[NALARMS];
static cyg_handle_t alarms[NALARMS];
static fun_times alarm_ft[NALARMS];

externC void diag_printf(const char *, ...);
externC void sprintf(char *, const char *, ...);

void run_sched_tests(void);
void run_thread_tests(void);
void run_thread_switch_test(void);
void run_mutex_tests(void);
void run_mutex_circuit_test(void);
void run_mbox_tests(void);
void run_mbox_circuit_test(void);
void run_semaphore_tests(void);
void run_semaphore_circuit_test(void);
void run_counter_tests(void);
void run_alarm_tests(void);

#ifdef HAL_CLOCK_LATENCY
extern cyg_tick_count total_clock_latency, total_clock_interrupts;
extern cyg_int32 min_clock_latency, max_clock_latency;
#endif

cyg_uint32
ticks_to_us(cyg_uint32 ticks)
{
    int us;
    // Assumes that the clock is set up for 10ms ticks
    us = (10000 * ticks) / CYGNUM_KERNEL_COUNTERS_RTC_PERIOD;
    return (us);
}

// Wait until a clock tick [real time clock] has passed.  This should keep it
// from happening again during a measurement, thus minimizing any fluctuations
void
wait_for_tick(void)
{
    cyg_uint32 tv0, tv1;
    HAL_CLOCK_READ(&tv0);
    while (true) {
        HAL_CLOCK_READ(&tv1);
        if (tv1 < tv0) break;
        tv0 = tv1;
    }
}

// Compute a name for a thread
char *
thread_name(char *basename, int indx) {
    return "<<NULL>>";  // Not currently used
}

// test0 - null test, never executed
void
test0(cyg_uint32 indx)
{
    diag_printf("test0.%d executed?\n", indx);
    cyg_thread_exit();
}

// test1 - empty test, simply exit.  Last thread signals parent.
void
test1(cyg_uint32 indx)
{
    if (indx == (NTEST_THREADS-1)) {
        cyg_semaphore_post(&synchro);  // Signal that last thread is dying
    }
    cyg_thread_exit();
}

// test2 - measure thread switch times
void
test2(cyg_uint32 indx)
{
    int i;
    for (i = 0;  i < NTHREAD_SWITCHES;  i++) {
        if (indx == 0) {
            HAL_CLOCK_READ(&test2_ft[i].start);
        } else {
            HAL_CLOCK_READ(&test2_ft[i].end);
        }
        cyg_thread_yield();
    }
    if (indx == 1) {
        cyg_semaphore_post(&synchro);
    }
    cyg_thread_exit();
}

// Full-circuit mutex unlock/lock test
void
mutex_test(cyg_uint32 indx)
{
    int i;
    cyg_mutex_lock(&test_mutexes[0]);
    for (i = 0;  i < NMUTEXES;  i++) {
        cyg_semaphore_wait(&synchro);
        wait_for_tick(); // Wait until the next clock tick to minimize aberations
        HAL_CLOCK_READ(&mutex_ft[i].start);
        cyg_mutex_unlock(&test_mutexes[0]);
        cyg_mutex_lock(&test_mutexes[0]);
        cyg_semaphore_post(&synchro);
    }
    cyg_thread_exit();
}

// Full-circuit mbox put/get test
void
mbox_test(cyg_uint32 indx)
{
    void *item;
    do {
        item = cyg_mbox_get(test_mbox_handles[0]);
        HAL_CLOCK_READ(&mbox_ft[(int)item].end);
        cyg_semaphore_post(&synchro);
    } while ((int)item != (NMBOXES-1));
    cyg_thread_exit();
}

// Full-circuit semaphore post/wait test
void
semaphore_test(cyg_uint32 indx)
{
    int i;
    for (i = 0;  i < NSEMAPHORES;  i++) {
        cyg_semaphore_wait(&test_semaphores[0]);
        HAL_CLOCK_READ(&semaphore_ft[i].end);
        cyg_semaphore_post(&synchro);
    }
    cyg_thread_exit();
}

void
show_times_hdr(void)
{
    diag_printf("\n");
    diag_printf("   Ave     Min     Max Variance   Function\n");
    diag_printf("======  ======  ====== ========   ========\n");
}

void
show_times(fun_times ft[], int nsamples, char *title)
{
    int i, delta, min, max;
    cyg_uint32 total, ave, var;
    total = 0;
    min = 0x7FFFFFFF;
    max = 0;
    for (i = 0;  i < nsamples;  i++) {
        if (ft[i].end < ft[i].start) {
            // Clock wrapped around (timer tick)
            delta = (ft[i].end+CYGNUM_KERNEL_COUNTERS_RTC_PERIOD) - ft[i].start;
        } else {
            delta = ft[i].end - ft[i].start;
        }
        delta -= 2*overhead;
        if (delta < 0) delta = 0;
        total += delta;
        if (delta < min) min = delta;
        if (delta > max) max = delta;
    }
    ave = total / nsamples;
    total = 0;
    for (i = 0;  i < nsamples;  i++) {
        if (ft[i].end < ft[i].start) {
            // Clock wrapped around (timer tick)
            delta = (ft[i].end+CYGNUM_KERNEL_COUNTERS_RTC_PERIOD) - ft[i].start;
        } else {
            delta = ft[i].end - ft[i].start;
        }
        delta -= 2*overhead;
        if (delta < 0) delta = 0;
        total = (delta - ave) * (delta - ave);
    }
    var = total / (nsamples - 1);
    diag_printf("%6d  %6d  %6d   %6d   %s\n",
                ticks_to_us(ave),  ticks_to_us(min),  ticks_to_us(max), ticks_to_us(var), title);
}

void
show_test_parameters(void)
{
    diag_printf("\nTesting parameters:\n");
    diag_printf("   Clock samples:         %3d\n", NSAMPLES);
    diag_printf("   Threads:               %3d\n", NTEST_THREADS);
    diag_printf("   Thread switches:       %3d\n", NTHREAD_SWITCHES);
    diag_printf("   Mutexes:               %3d\n", NMUTEXES);
    diag_printf("   Mailboxes:             %3d\n", NMBOXES);
    diag_printf("   Semaphores:            %3d\n", NSEMAPHORES);
    diag_printf("   Scheduler operations:  %3d\n", NSCHEDS);
    diag_printf("   Counters:              %3d\n", NCOUNTERS);
    diag_printf("   Alarms:                %3d\n", NALARMS);
    diag_printf("\n"); 
}

void
run_thread_tests(void)
{
    int i;
    cyg_priority_t prio;

    // Set my priority higher than any I plan to create
    cyg_thread_set_priority(cyg_thread_self(), 2);

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NTEST_THREADS;  i++) {
        HAL_CLOCK_READ(&thread_ft[i].start);
        cyg_thread_create(10,              // Priority - just a number
                          test0,           // entry
                          i,               // index
                          thread_name("thread", i),     // Name
                          &stacks[i][0],   // Stack
                          STACK_SIZE,      // Size
                          &threads[i],     // Handle
                          &test_threads[i] // Thread data structure
            );
        HAL_CLOCK_READ(&thread_ft[i].end);
    }
    show_times(thread_ft, NTEST_THREADS, "Create thread");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NTEST_THREADS;  i++) {
        HAL_CLOCK_READ(&thread_ft[i].start);
        cyg_thread_yield();
        HAL_CLOCK_READ(&thread_ft[i].end);
    }
    show_times(thread_ft, NTEST_THREADS, "Yield thread [all suspended]");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NTEST_THREADS;  i++) {
        HAL_CLOCK_READ(&thread_ft[i].start);
        cyg_thread_suspend(threads[i]);
        HAL_CLOCK_READ(&thread_ft[i].end);
    }
    show_times(thread_ft, NTEST_THREADS, "Suspend [suspended] thread");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NTEST_THREADS;  i++) {
        HAL_CLOCK_READ(&thread_ft[i].start);
        cyg_thread_resume(threads[i]);
        HAL_CLOCK_READ(&thread_ft[i].end);
    }
    show_times(thread_ft, NTEST_THREADS, "Resume thread");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NTEST_THREADS;  i++) {
        HAL_CLOCK_READ(&thread_ft[i].start);
        cyg_thread_set_priority(threads[i], 11);
        HAL_CLOCK_READ(&thread_ft[i].end);
    }
    show_times(thread_ft, NTEST_THREADS, "Set priority");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NTEST_THREADS;  i++) {
        HAL_CLOCK_READ(&thread_ft[i].start);
        prio = cyg_thread_get_priority(threads[i]);
        HAL_CLOCK_READ(&thread_ft[i].end);
    }
    show_times(thread_ft, NTEST_THREADS, "Get priority");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NTEST_THREADS;  i++) {
        HAL_CLOCK_READ(&thread_ft[i].start);
        cyg_thread_kill(threads[i]);
        HAL_CLOCK_READ(&thread_ft[i].end);
    }
    show_times(thread_ft, NTEST_THREADS, "Kill [suspended] thread");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NTEST_THREADS;  i++) {
        HAL_CLOCK_READ(&thread_ft[i].start);
        cyg_thread_yield();
        HAL_CLOCK_READ(&thread_ft[i].end);
    }
    show_times(thread_ft, NTEST_THREADS, "Yield [no other] thread");

    // Set my priority higher than any I plan to create
    cyg_thread_set_priority(cyg_thread_self(), 2);

    // Recreate the test set
    for (i = 0;  i < NTEST_THREADS;  i++) {
        cyg_thread_create(10,              // Priority - just a number
                          test0,           // entry
                          i,               // index
                          thread_name("thread", i),     // Name
                          &stacks[i][0],   // Stack
                          STACK_SIZE,      // Size
                          &threads[i],     // Handle
                          &test_threads[i] // Thread data structure
            );
    }

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NTEST_THREADS;  i++) {
        HAL_CLOCK_READ(&thread_ft[i].start);
        cyg_thread_resume(threads[i]);
        HAL_CLOCK_READ(&thread_ft[i].end);
    }
    show_times(thread_ft, NTEST_THREADS, "Resume [suspended low priority] thread");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NTEST_THREADS;  i++) {
        HAL_CLOCK_READ(&thread_ft[i].start);
        cyg_thread_resume(threads[i]);
        HAL_CLOCK_READ(&thread_ft[i].end);
    }
    show_times(thread_ft, NTEST_THREADS, "Resume [runnable low priority] thread");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NTEST_THREADS;  i++) {
        HAL_CLOCK_READ(&thread_ft[i].start);
        cyg_thread_suspend(threads[i]);
        HAL_CLOCK_READ(&thread_ft[i].end);
    }
    show_times(thread_ft, NTEST_THREADS, "Suspend [runnable] thread");


    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NTEST_THREADS;  i++) {
        HAL_CLOCK_READ(&thread_ft[i].start);
        cyg_thread_yield();
        HAL_CLOCK_READ(&thread_ft[i].end);
    }
    show_times(thread_ft, NTEST_THREADS, "Yield [only low prio] thread");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NTEST_THREADS;  i++) {
        HAL_CLOCK_READ(&thread_ft[i].start);
        cyg_thread_suspend(threads[i]);
        HAL_CLOCK_READ(&thread_ft[i].end);
    }
    show_times(thread_ft, NTEST_THREADS, "Suspend [runnable->not runnable] thread");
    for (i = 0;  i < NTEST_THREADS;  i++) {
        cyg_thread_resume(threads[i]);
    }

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NTEST_THREADS;  i++) {
        HAL_CLOCK_READ(&thread_ft[i].start);
        cyg_thread_kill(threads[i]);
        HAL_CLOCK_READ(&thread_ft[i].end);
    }
    show_times(thread_ft, NTEST_THREADS, "Kill [runnable] thread");
    // Set my priority lower than any I plan to create
    cyg_thread_set_priority(cyg_thread_self(), 3);

    // Set up the end-of-threads synchronizer
    cyg_semaphore_init(&synchro, 0);

    // Recreate the test set
    for (i = 0;  i < NTEST_THREADS;  i++) {
        cyg_thread_create(2,               // Priority - just a number
                          test1,           // entry
                          i,               // index
                          thread_name("thread", i),     // Name
                          &stacks[i][0],   // Stack
                          STACK_SIZE,      // Size
                          &threads[i],     // Handle
                          &test_threads[i] // Thread data structure
            );
    }

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NTEST_THREADS;  i++) {
        HAL_CLOCK_READ(&thread_ft[i].start);
        cyg_thread_resume(threads[i]);
        HAL_CLOCK_READ(&thread_ft[i].end);
    }
    show_times(thread_ft, NTEST_THREADS, "Resume [high priority] thread");
    cyg_semaphore_wait(&synchro);  // Wait for all threads to finish
    // Make sure they are all dead
    for (i = 0;  i < NTEST_THREADS;  i++) {
        cyg_thread_kill(threads[i]);
    }

    run_thread_switch_test();
}

void
run_thread_switch_test(void)
{
    int i;

    // Set up for thread context switch 
    for (i = 0;  i < 2;  i++) {
        cyg_thread_create(10,              // Priority - just a number
                          test2,           // entry
                          i,               // index
                          thread_name("thread", i),     // Name
                          &stacks[i][0],   // Stack
                          STACK_SIZE,      // Size
                          &threads[i],     // Handle
                          &test_threads[i] // Thread data structure
            );
        cyg_thread_resume(threads[i]);
    }
    // Set up the end-of-threads synchronizer
    cyg_semaphore_init(&synchro, 0);
    cyg_semaphore_wait(&synchro);
    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    show_times(test2_ft, NTHREAD_SWITCHES, "Thread switch");
    // Clean up
    for (i = 0;  i < 2;  i++) {
        cyg_thread_kill(threads[i]);
    }
}

void
run_mutex_tests(void)
{
    int i;

    // Mutex primitives
    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMUTEXES;  i++) {
        HAL_CLOCK_READ(&mutex_ft[i].start);
        cyg_mutex_init(&test_mutexes[i]);
        HAL_CLOCK_READ(&mutex_ft[i].end);
    }
    show_times(mutex_ft, NMUTEXES, "Init mutex");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMUTEXES;  i++) {
        HAL_CLOCK_READ(&mutex_ft[i].start);
        cyg_mutex_lock(&test_mutexes[i]);
        HAL_CLOCK_READ(&mutex_ft[i].end);
    }
    show_times(mutex_ft, NMUTEXES, "Lock [unlocked] mutex");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMUTEXES;  i++) {
        HAL_CLOCK_READ(&mutex_ft[i].start);
        cyg_mutex_unlock(&test_mutexes[i]);
        HAL_CLOCK_READ(&mutex_ft[i].end);
    }
    show_times(mutex_ft, NMUTEXES, "Unlock [locked] mutex");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMUTEXES;  i++) {
        HAL_CLOCK_READ(&mutex_ft[i].start);
        cyg_mutex_trylock(&test_mutexes[i]);
        HAL_CLOCK_READ(&mutex_ft[i].end);
    }
    show_times(mutex_ft, NMUTEXES, "Trylock [unlocked] mutex");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMUTEXES;  i++) {
        HAL_CLOCK_READ(&mutex_ft[i].start);
        cyg_mutex_trylock(&test_mutexes[i]);
        HAL_CLOCK_READ(&mutex_ft[i].end);
    }
    show_times(mutex_ft, NMUTEXES, "Trylock [locked] mutex");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMUTEXES;  i++) {
        HAL_CLOCK_READ(&mutex_ft[i].start);
        cyg_mutex_destroy(&test_mutexes[i]);
        HAL_CLOCK_READ(&mutex_ft[i].end);
    }
    show_times(mutex_ft, NMUTEXES, "Destroy mutex");
    run_mutex_circuit_test();
}

void
run_mutex_circuit_test(void)
{
    int i;
    // Set my priority lower than any I plan to create
    cyg_thread_set_priority(cyg_thread_self(), 4);
    // Set up for full mutex unlock/lock test
    cyg_mutex_init(&test_mutexes[0]);
    cyg_semaphore_init(&synchro, 0);
    cyg_thread_create(3,              // Priority - just a number
                      mutex_test,           // entry
                      0,               // index
                      thread_name("thread", 0),     // Name
                      &stacks[0][0],   // Stack
                      STACK_SIZE,      // Size
                      &mutex_test_thread_handle,   // Handle
                      &mutex_test_thread    // Thread data structure
        );
    cyg_thread_resume(mutex_test_thread_handle);
    // Need to raise priority so that this thread will block on the "lock"
    cyg_thread_set_priority(cyg_thread_self(), 2);
    for (i = 0;  i < NMUTEXES;  i++) {
        cyg_semaphore_post(&synchro);
        cyg_mutex_lock(&test_mutexes[0]);
        HAL_CLOCK_READ(&mutex_ft[i].end);
        cyg_mutex_unlock(&test_mutexes[0]);
        cyg_semaphore_wait(&synchro);
    }
    show_times(mutex_ft, NMUTEXES, "Unlock/Lock mutex");
}

void
run_mbox_tests(void)
{
    int i, cnt;
    void *item;
    // Mailbox primitives
    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMBOXES;  i++) {
        HAL_CLOCK_READ(&mbox_ft[i].start);
        cyg_mbox_create(&test_mbox_handles[i], &test_mboxes[i]);
        HAL_CLOCK_READ(&mbox_ft[i].end);
    }
    show_times(mbox_ft, NMBOXES, "Create mbox");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMBOXES;  i++) {
        HAL_CLOCK_READ(&mbox_ft[i].start);
        cnt = cyg_mbox_peek(test_mbox_handles[i]);
        HAL_CLOCK_READ(&mbox_ft[i].end);
    }
    show_times(mbox_ft, NMBOXES, "Peek [empty] mbox");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMBOXES;  i++) {
        HAL_CLOCK_READ(&mbox_ft[i].start);
        cyg_mbox_put(test_mbox_handles[i], (void *)i);
        HAL_CLOCK_READ(&mbox_ft[i].end);
    }
    show_times(mbox_ft, NMBOXES, "Put [first] mbox");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMBOXES;  i++) {
        HAL_CLOCK_READ(&mbox_ft[i].start);
        cnt = cyg_mbox_peek(test_mbox_handles[i]);
        HAL_CLOCK_READ(&mbox_ft[i].end);
    }
    show_times(mbox_ft, NMBOXES, "Peek [1 msg] mbox");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMBOXES;  i++) {
        HAL_CLOCK_READ(&mbox_ft[i].start);
        cyg_mbox_put(test_mbox_handles[i], (void *)i);
        HAL_CLOCK_READ(&mbox_ft[i].end);
    }
    show_times(mbox_ft, NMBOXES, "Put [second] mbox");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMBOXES;  i++) {
        HAL_CLOCK_READ(&mbox_ft[i].start);
        cnt = cyg_mbox_peek(test_mbox_handles[i]);
        HAL_CLOCK_READ(&mbox_ft[i].end);
    }
    show_times(mbox_ft, NMBOXES, "Peek [2 msgs] mbox");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMBOXES;  i++) {
        HAL_CLOCK_READ(&mbox_ft[i].start);
        item = cyg_mbox_get(test_mbox_handles[i]);
        HAL_CLOCK_READ(&mbox_ft[i].end);
    }
    show_times(mbox_ft, NMBOXES, "Get [first] mbox");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMBOXES;  i++) {
        HAL_CLOCK_READ(&mbox_ft[i].start);
        item = cyg_mbox_get(test_mbox_handles[i]);
        HAL_CLOCK_READ(&mbox_ft[i].end);
    }
    show_times(mbox_ft, NMBOXES, "Get [second] mbox");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMBOXES;  i++) {
        HAL_CLOCK_READ(&mbox_ft[i].start);
        cyg_mbox_tryput(test_mbox_handles[i], (void *)i);
        HAL_CLOCK_READ(&mbox_ft[i].end);
    }
    show_times(mbox_ft, NMBOXES, "Tryput [first] mbox");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMBOXES;  i++) {
        HAL_CLOCK_READ(&mbox_ft[i].start);
        item = cyg_mbox_peek_item(test_mbox_handles[i]);
        HAL_CLOCK_READ(&mbox_ft[i].end);
    }
    show_times(mbox_ft, NMBOXES, "Peek item [non-empty] mbox");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMBOXES;  i++) {
        HAL_CLOCK_READ(&mbox_ft[i].start);
        item = cyg_mbox_tryget(test_mbox_handles[i]);
        HAL_CLOCK_READ(&mbox_ft[i].end);
    }
    show_times(mbox_ft, NMBOXES, "Tryget [non-empty] mbox");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMBOXES;  i++) {
        HAL_CLOCK_READ(&mbox_ft[i].start);
        item = cyg_mbox_peek_item(test_mbox_handles[i]);
        HAL_CLOCK_READ(&mbox_ft[i].end);
    }
    show_times(mbox_ft, NMBOXES, "Peek item [empty] mbox");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMBOXES;  i++) {
        HAL_CLOCK_READ(&mbox_ft[i].start);
        item = cyg_mbox_tryget(test_mbox_handles[i]);
        HAL_CLOCK_READ(&mbox_ft[i].end);
    }
    show_times(mbox_ft, NMBOXES, "Tryget [empty] mbox");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMBOXES;  i++) {
        HAL_CLOCK_READ(&mbox_ft[i].start);
        cyg_mbox_waiting_to_get(test_mbox_handles[i]);
        HAL_CLOCK_READ(&mbox_ft[i].end);
    }
    show_times(mbox_ft, NMBOXES, "Waiting to get mbox");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMBOXES;  i++) {
        HAL_CLOCK_READ(&mbox_ft[i].start);
        cyg_mbox_waiting_to_put(test_mbox_handles[i]);
        HAL_CLOCK_READ(&mbox_ft[i].end);
    }
    show_times(mbox_ft, NMBOXES, "Waiting to put mbox");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NMBOXES;  i++) {
        HAL_CLOCK_READ(&mbox_ft[i].start);
        cyg_mbox_delete(test_mbox_handles[i]);
        HAL_CLOCK_READ(&mbox_ft[i].end);
    }
    show_times(mbox_ft, NMBOXES, "Delete mbox");

    run_mbox_circuit_test();
}

void
run_mbox_circuit_test(void)
{
    int i;
    // Set my priority lower than any I plan to create
    cyg_thread_set_priority(cyg_thread_self(), 3);
    // Set up for full mbox put/get test
    cyg_mbox_create(&test_mbox_handles[0], &test_mboxes[0]);
    cyg_semaphore_init(&synchro, 0);
    cyg_thread_create(2,              // Priority - just a number
                      mbox_test,           // entry
                      0,               // index
                      thread_name("thread", 0),     // Name
                      &stacks[0][0],   // Stack
                      STACK_SIZE,      // Size
                      &mbox_test_thread_handle,   // Handle
                      &mbox_test_thread    // Thread data structure
        );
    cyg_thread_resume(mbox_test_thread_handle);
    for (i = 0;  i < NMBOXES;  i++) {
        wait_for_tick(); // Wait until the next clock tick to minimize aberations
        HAL_CLOCK_READ(&mbox_ft[i].start);
        cyg_mbox_put(test_mbox_handles[0], (void *)i);
        cyg_semaphore_wait(&synchro);
    }
    show_times(mbox_ft, NMBOXES, "Put/Get mbox");
}

void
run_semaphore_tests(void)
{
    int i;
    cyg_ucount32 sem_val;
    // Semaphore primitives
    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NSEMAPHORES;  i++) {
        HAL_CLOCK_READ(&semaphore_ft[i].start);
        cyg_semaphore_init(&test_semaphores[i], 0);
        HAL_CLOCK_READ(&semaphore_ft[i].end);
    }
    show_times(semaphore_ft, NSEMAPHORES, "Init semaphore");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NSEMAPHORES;  i++) {
        HAL_CLOCK_READ(&semaphore_ft[i].start);
        cyg_semaphore_post(&test_semaphores[i]);
        HAL_CLOCK_READ(&semaphore_ft[i].end);
    }
    show_times(semaphore_ft, NSEMAPHORES, "Post [0] semaphore");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NSEMAPHORES;  i++) {
        HAL_CLOCK_READ(&semaphore_ft[i].start);
        cyg_semaphore_wait(&test_semaphores[i]);
        HAL_CLOCK_READ(&semaphore_ft[i].end);
    }
    show_times(semaphore_ft, NSEMAPHORES, "Wait [1] semaphore");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NSEMAPHORES;  i++) {
        HAL_CLOCK_READ(&semaphore_ft[i].start);
        cyg_semaphore_trywait(&test_semaphores[i]);
        HAL_CLOCK_READ(&semaphore_ft[i].end);
    }
    show_times(semaphore_ft, NSEMAPHORES, "Trywait [0] semaphore");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NSEMAPHORES;  i++) {
        cyg_semaphore_post(&test_semaphores[i]);
        HAL_CLOCK_READ(&semaphore_ft[i].start);
        cyg_semaphore_trywait(&test_semaphores[i]);
        HAL_CLOCK_READ(&semaphore_ft[i].end);
    }
    show_times(semaphore_ft, NSEMAPHORES, "Trywait [1] semaphore");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NSEMAPHORES;  i++) {
        HAL_CLOCK_READ(&semaphore_ft[i].start);
        cyg_semaphore_peek(&test_semaphores[i], &sem_val);
        HAL_CLOCK_READ(&semaphore_ft[i].end);
    }
    show_times(semaphore_ft, NSEMAPHORES, "Peek semaphore");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NSEMAPHORES;  i++) {
        HAL_CLOCK_READ(&semaphore_ft[i].start);
        cyg_semaphore_destroy(&test_semaphores[i]);
        HAL_CLOCK_READ(&semaphore_ft[i].end);
    }
    show_times(semaphore_ft, NSEMAPHORES, "Destroy semaphore");

    run_semaphore_circuit_test();
}

void
run_semaphore_circuit_test(void)
{
    int i;
    // Set my priority lower than any I plan to create
    cyg_thread_set_priority(cyg_thread_self(), 3);
    // Set up for full semaphore post/wait test
    cyg_semaphore_init(&test_semaphores[0], 0);
    cyg_semaphore_init(&synchro, 0);
    cyg_thread_create(2,              // Priority - just a number
                      semaphore_test,           // entry
                      0,               // index
                      thread_name("thread", 0),     // Name
                      &stacks[0][0],   // Stack
                      STACK_SIZE,      // Size
                      &semaphore_test_thread_handle,   // Handle
                      &semaphore_test_thread    // Thread data structure
        );
    cyg_thread_resume(semaphore_test_thread_handle);
    for (i = 0;  i < NSEMAPHORES;  i++) {
        wait_for_tick(); // Wait until the next clock tick to minimize aberations
        HAL_CLOCK_READ(&semaphore_ft[i].start);
        cyg_semaphore_post(&test_semaphores[0]);
        cyg_semaphore_wait(&synchro);
    }
    show_times(semaphore_ft, NSEMAPHORES, "Post/Wait semaphore");
}

void
run_counter_tests(void)
{
    int i;
    cyg_tick_count_t val;

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NCOUNTERS;  i++) {
        HAL_CLOCK_READ(&counter_ft[i].start);
        cyg_counter_create(&counters[i], &test_counters[i]);
        HAL_CLOCK_READ(&counter_ft[i].end);
    }
    show_times(counter_ft, NCOUNTERS, "Create counter");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NCOUNTERS;  i++) {
        HAL_CLOCK_READ(&counter_ft[i].start);
        val = cyg_counter_current_value(counters[i]);
        HAL_CLOCK_READ(&counter_ft[i].end);
    }
    show_times(counter_ft, NCOUNTERS, "Get counter value");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NCOUNTERS;  i++) {
        HAL_CLOCK_READ(&counter_ft[i].start);
        cyg_counter_set_value(counters[i], val);
        HAL_CLOCK_READ(&counter_ft[i].end);
    }
    show_times(counter_ft, NCOUNTERS, "Set counter value");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NCOUNTERS;  i++) {
        HAL_CLOCK_READ(&counter_ft[i].start);
        cyg_counter_tick(counters[i]);
        HAL_CLOCK_READ(&counter_ft[i].end);
    }
    show_times(counter_ft, NCOUNTERS, "Tick counter");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NCOUNTERS;  i++) {
        HAL_CLOCK_READ(&counter_ft[i].start);
        cyg_counter_delete(counters[i]);
        HAL_CLOCK_READ(&counter_ft[i].end);
    }
    show_times(counter_ft, NCOUNTERS, "Delete counter");
}

// Alarm callback function
void
alarm_cb(cyg_handle_t alarm, cyg_addrword_t val)
{
    // empty call back
}

// Callback used to test determinancy
static volatile int alarm_cnt;
void
alarm_cb2(cyg_handle_t alarm, cyg_addrword_t indx)
{
    if (alarm_cnt == NSCHEDS) return;
    sched_ft[alarm_cnt].start = 0;
    HAL_CLOCK_READ(&sched_ft[alarm_cnt++].end);
    if (alarm_cnt == NSCHEDS) {
        cyg_semaphore_post(&synchro);
    }
}

// Null thread, used to keep scheduler busy
void
alarm_test(cyg_uint32 id)
{
    while (true) {
        cyg_thread_yield();
    }
}

void
run_alarm_tests(void)
{
    int i;
    cyg_tick_count_t init_val, step_val;
    cyg_handle_t rtc_handle;

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NCOUNTERS;  i++) {
        cyg_counter_create(&counters[i], &test_counters[i]);
    }
    for (i = 0;  i < NALARMS;  i++) {
        HAL_CLOCK_READ(&alarm_ft[i].start);
        cyg_alarm_create(counters[0], alarm_cb, 0, &alarms[i], &test_alarms[i]);
        HAL_CLOCK_READ(&alarm_ft[i].end);
    }
    show_times(alarm_ft, NALARMS, "Create alarm");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    init_val = 0;  step_val = 0;
    for (i = 0;  i < NALARMS;  i++) {
        HAL_CLOCK_READ(&alarm_ft[i].start);
        cyg_alarm_initialize(alarms[i], init_val, step_val);
        HAL_CLOCK_READ(&alarm_ft[i].end);
    }
    show_times(alarm_ft, NALARMS, "Initialize alarm");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    init_val = 0;  step_val = 0;
    for (i = 0;  i < NALARMS;  i++) {
        HAL_CLOCK_READ(&alarm_ft[i].start);
        cyg_alarm_disable(alarms[i]);
        HAL_CLOCK_READ(&alarm_ft[i].end);
    }
    show_times(alarm_ft, NALARMS, "Disable alarm");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    init_val = 0;  step_val = 0;
    for (i = 0;  i < NALARMS;  i++) {
        HAL_CLOCK_READ(&alarm_ft[i].start);
        cyg_alarm_enable(alarms[i]);
        HAL_CLOCK_READ(&alarm_ft[i].end);
    }
    show_times(alarm_ft, NALARMS, "Enable alarm");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NALARMS;  i++) {
        HAL_CLOCK_READ(&alarm_ft[i].start);
        cyg_alarm_delete(alarms[i]);
        HAL_CLOCK_READ(&alarm_ft[i].end);
    }
    show_times(alarm_ft, NALARMS, "Delete alarm");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    cyg_counter_create(&counters[0], &test_counters[0]);
    cyg_alarm_create(counters[0], alarm_cb, 0, &alarms[0], &test_alarms[0]);
    init_val = 9999;  step_val = 9999;
    cyg_alarm_initialize(alarms[0], init_val, step_val);
    cyg_alarm_enable(alarms[0]);
    for (i = 0;  i < NCOUNTERS;  i++) {
        HAL_CLOCK_READ(&counter_ft[i].start);
        cyg_counter_tick(counters[0]);
        HAL_CLOCK_READ(&counter_ft[i].end);
    }
    show_times(counter_ft, NCOUNTERS, "Tick counter [1 alarm]");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    cyg_counter_create(&counters[0], &test_counters[0]);
    for (i = 0;  i < NALARMS;  i++) {
        cyg_alarm_create(counters[0], alarm_cb, 0, &alarms[i], &test_alarms[i]);
        init_val = 9999;  step_val = 9999;
        cyg_alarm_initialize(alarms[i], init_val, step_val);
        cyg_alarm_enable(alarms[i]);
    }
    for (i = 0;  i < NCOUNTERS;  i++) {
        HAL_CLOCK_READ(&counter_ft[i].start);
        cyg_counter_tick(counters[0]);
        HAL_CLOCK_READ(&counter_ft[i].end);
    }
    show_times(counter_ft, NCOUNTERS, "Tick counter [many alarms]");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    cyg_counter_create(&counters[0], &test_counters[0]);
    cyg_alarm_create(counters[0], alarm_cb, 0, &alarms[0], &test_alarms[0]);
    init_val = 1;  step_val = 1;
    cyg_alarm_initialize(alarms[0], init_val, step_val);
    cyg_alarm_enable(alarms[0]);
    for (i = 0;  i < NCOUNTERS;  i++) {
        HAL_CLOCK_READ(&counter_ft[i].start);
        cyg_counter_tick(counters[0]);
        HAL_CLOCK_READ(&counter_ft[i].end);
    }
    show_times(counter_ft, NCOUNTERS, "Tick & fire counter [1 alarm]");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    cyg_counter_create(&counters[0], &test_counters[0]);
    for (i = 0;  i < NALARMS;  i++) {
        cyg_alarm_create(counters[0], alarm_cb, i, &alarms[i], &test_alarms[i]);
        init_val = 1;  step_val = 1;
        cyg_alarm_initialize(alarms[i], init_val, step_val);
        cyg_alarm_enable(alarms[i]);
    }
    for (i = 0;  i < NCOUNTERS;  i++) {
        HAL_CLOCK_READ(&counter_ft[i].start);
        cyg_counter_tick(counters[0]);
        HAL_CLOCK_READ(&counter_ft[i].end);
    }
    for (i = 0;  i < NALARMS;  i++) {
        cyg_alarm_delete(alarms[i]);
    }
    show_times(counter_ft, NCOUNTERS, "Tick & fire counter [many alarms]");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    cyg_clock_to_counter(cyg_real_time_clock(), &rtc_handle);
    cyg_alarm_create(rtc_handle, alarm_cb2, 0, &alarms[0], &test_alarms[0]);
    init_val = 5;  step_val = 5;  alarm_cnt = 0;
    cyg_alarm_initialize(alarms[0], init_val, step_val);
    cyg_semaphore_init(&synchro, 0);
    cyg_alarm_enable(alarms[0]);
    cyg_semaphore_wait(&synchro);
    cyg_alarm_disable(alarms[0]);
    cyg_alarm_delete(alarms[0]);
    show_times(sched_ft, NSCHEDS, "Alarm latency [0 threads]");

    // Set my priority higher than any I plan to create
    cyg_thread_set_priority(cyg_thread_self(), 2);
    for (i = 0;  i < 2;  i++) {
        cyg_thread_create(10,              // Priority - just a number
                          alarm_test,      // entry
                          i,               // index
                          thread_name("thread", i),     // Name
                          &stacks[i][0],   // Stack
                          STACK_SIZE,      // Size
                          &threads[i],     // Handle
                          &test_threads[i] // Thread data structure
            );
        cyg_thread_resume(threads[i]);
    }
    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    cyg_clock_to_counter(cyg_real_time_clock(), &rtc_handle);
    cyg_alarm_create(rtc_handle, alarm_cb2, 0, &alarms[0], &test_alarms[0]);
    init_val = 5;  step_val = 5;  alarm_cnt = 0;
    cyg_alarm_initialize(alarms[0], init_val, step_val);
    cyg_semaphore_init(&synchro, 0);
    cyg_alarm_enable(alarms[0]);
    cyg_semaphore_wait(&synchro);
    cyg_alarm_disable(alarms[0]);
    cyg_alarm_delete(alarms[0]);
    show_times(sched_ft, NSCHEDS, "Alarm latency [2 threads]");
    for (i = 0;  i < 2;  i++) {
        cyg_thread_suspend(threads[i]);
        cyg_thread_kill(threads[i]);
    }

    // Set my priority higher than any I plan to create
    cyg_thread_set_priority(cyg_thread_self(), 2);
    for (i = 0;  i < NTEST_THREADS;  i++) {
        cyg_thread_create(10,              // Priority - just a number
                          alarm_test,      // entry
                          i,               // index
                          thread_name("thread", i),     // Name
                          &stacks[i][0],   // Stack
                          STACK_SIZE,      // Size
                          &threads[i],     // Handle
                          &test_threads[i] // Thread data structure
            );
        cyg_thread_resume(threads[i]);
    }
    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    cyg_clock_to_counter(cyg_real_time_clock(), &rtc_handle);
    cyg_alarm_create(rtc_handle, alarm_cb2, 0, &alarms[0], &test_alarms[0]);
    init_val = 5;  step_val = 5;  alarm_cnt = 0;
    cyg_alarm_initialize(alarms[0], init_val, step_val);
    cyg_semaphore_init(&synchro, 0);
    cyg_alarm_enable(alarms[0]);
    cyg_semaphore_wait(&synchro);
    cyg_alarm_disable(alarms[0]);
    cyg_alarm_delete(alarms[0]);
    show_times(sched_ft, NSCHEDS, "Alarm latency [many threads]");
    for (i = 0;  i < NTEST_THREADS;  i++) {
        cyg_thread_suspend(threads[i]);
        cyg_thread_kill(threads[i]);
    }
}

void
run_sched_tests(void)
{
    int i;

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NSCHEDS;  i++) {
        HAL_CLOCK_READ(&sched_ft[i].start);
        cyg_scheduler_lock();
        HAL_CLOCK_READ(&sched_ft[i].end);
        cyg_scheduler_unlock();
    }
    show_times(sched_ft, NSCHEDS, "Scheduler lock");

    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NSCHEDS;  i++) {
        cyg_scheduler_lock();
        HAL_CLOCK_READ(&sched_ft[i].start);
        cyg_scheduler_unlock();
        HAL_CLOCK_READ(&sched_ft[i].end);
    }
    show_times(sched_ft, NSCHEDS, "Scheduler unlock [0 threads]");

    // Set my priority higher than any I plan to create
    cyg_thread_set_priority(cyg_thread_self(), 2);
    for (i = 0;  i < 1;  i++) {
        cyg_thread_create(10,              // Priority - just a number
                          test0,           // entry
                          i,               // index
                          thread_name("thread", i),     // Name
                          &stacks[i][0],   // Stack
                          STACK_SIZE,      // Size
                          &threads[i],     // Handle
                          &test_threads[i] // Thread data structure
            );
    }
    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NSCHEDS;  i++) {
        cyg_scheduler_lock();
        HAL_CLOCK_READ(&sched_ft[i].start);
        cyg_scheduler_unlock();
        HAL_CLOCK_READ(&sched_ft[i].end);
    }
    show_times(sched_ft, NSCHEDS, "Scheduler unlock [1 suspended thread]");
    for (i = 0;  i < 1;  i++) {
        cyg_thread_kill(threads[i]);
    }

    // Set my priority higher than any I plan to create
    cyg_thread_set_priority(cyg_thread_self(), 2);
    for (i = 0;  i < NTEST_THREADS;  i++) {
        cyg_thread_create(10,              // Priority - just a number
                          test0,           // entry
                          i,               // index
                          thread_name("thread", i),     // Name
                          &stacks[i][0],   // Stack
                          STACK_SIZE,      // Size
                          &threads[i],     // Handle
                          &test_threads[i] // Thread data structure
            );
    }
    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NSCHEDS;  i++) {
        cyg_scheduler_lock();
        HAL_CLOCK_READ(&sched_ft[i].start);
        cyg_scheduler_unlock();
        HAL_CLOCK_READ(&sched_ft[i].end);
    }
    show_times(sched_ft, NSCHEDS, "Scheduler unlock [many suspended threads]");
    for (i = 0;  i < NTEST_THREADS;  i++) {
        cyg_thread_kill(threads[i]);
    }

    // Set my priority higher than any I plan to create
    cyg_thread_set_priority(cyg_thread_self(), 2);
    for (i = 0;  i < NTEST_THREADS;  i++) {
        cyg_thread_create(10,              // Priority - just a number
                          test0,           // entry
                          i,               // index
                          thread_name("thread", i),     // Name
                          &stacks[i][0],   // Stack
                          STACK_SIZE,      // Size
                          &threads[i],     // Handle
                          &test_threads[i] // Thread data structure
            );
        cyg_thread_resume(threads[i]);
    }
    wait_for_tick(); // Wait until the next clock tick to minimize aberations
    for (i = 0;  i < NSCHEDS;  i++) {
        cyg_scheduler_lock();
        HAL_CLOCK_READ(&sched_ft[i].start);
        cyg_scheduler_unlock();
        HAL_CLOCK_READ(&sched_ft[i].end);
    }
    show_times(sched_ft, NSCHEDS, "Scheduler unlock [many low prio threads]");
    for (i = 0;  i < NTEST_THREADS;  i++) {
        cyg_thread_kill(threads[i]);
    }
}

void 
run_all_tests(CYG_ADDRESS id)
{
    int i;
    cyg_uint32 tv[NSAMPLES], tv0, tv1, us;
    cyg_tick_count_t ticks;
#ifdef CYG_SCHEDULER_LOCK_TIMINGS
    cyg_uint32 lock_ave, lock_max;
#endif
#ifdef HAL_CLOCK_LATENCY
    cyg_int32 clock_ave;
#endif

    for (i = 0;  i < NSAMPLES;  i++) {
        HAL_CLOCK_READ(&tv[i]);
    }
    diag_printf("Clock: ");
    tv0 = 0;
    for (i = 0;  i < NSAMPLES;  i++) {
        diag_printf("%x ", tv[i]);
        if (i > 0) {
            tv0 += tv[i] - tv[i-1];
        }
    }
    diag_printf("\n");
    
    overhead = tv0 / (NSAMPLES-1);
    diag_printf("Average %d clock ticks overhead\n", overhead);

    // Try and measure how long the clock interrupt handling takes
    HAL_CLOCK_READ(&tv0);
    while (true) {
        HAL_CLOCK_READ(&tv1);
        if (tv1 < tv0) break;
        tv0 = tv1;
    }
    tv1 -= overhead;  // Adjust out the cost of getting the timer value
    us = ticks_to_us(tv1);
    diag_printf("Clock interrupt took %d microseconds (%d clock ticks)\n", us, tv1);

    ticks = cyg_current_time();
    diag_printf("Ticks: %x\n", (int)ticks);

    show_test_parameters();
    show_times_hdr();

    run_thread_tests();
    run_sched_tests();
    run_mutex_tests();
    run_mbox_tests();
    run_semaphore_tests();
    run_counter_tests();
    run_alarm_tests();

#ifdef CYG_SCHEDULER_LOCK_TIMINGS
    Cyg_Scheduler::get_lock_times(&lock_ave, &lock_max);
    diag_printf("\nMax lock: %d, Ave lock: %d\n", ticks_to_us(lock_max), ticks_to_us(lock_ave));
#endif

#ifdef HAL_CLOCK_LATENCY
    clock_ave = total_clock_latency / total_clock_interrupts;
    diag_printf("\nClock/interrupt latency - ave: %d, min: %d, max: %d\n",
                ticks_to_us(clock_ave), ticks_to_us(min_clock_latency), ticks_to_us(max_clock_latency));
#endif

    ticks = cyg_current_time();
    diag_printf("\nTiming complete - %d ms total\n", (int)ticks*10);

    CYG_TEST_PASS_FINISH("Basic timing OK");
}

void tm_basic_main( void )
{
    CYG_TEST_INIT();

    new_thread(run_all_tests, 0);
    
    Cyg_Scheduler::scheduler.start();
}

externC void
cyg_start( void )
{
    tm_basic_main();
}   
// EOF tm_basic.cxx
