//==========================================================================
//
//        stress_threads.cxx
//
//        Basic thread stress test
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
// Author(s):     rosalia
// Contributors:  rosalia
// Date:          1999-04-13
// Description:   Very simple thread stress test, with some memory
//                allocation and alarm handling.
//####DESCRIPTIONEND####

#include <pkgconf/system.h>
#include <cyg/infra/testcase.h>

#include <cyg/hal/hal_arch.h>

#if defined(CYGPKG_KERNEL) && defined(CYGPKG_IO) && defined(CYGPKG_LIBC)

#include <pkgconf/kernel.h>
#include <pkgconf/libc.h>

#if defined(CYGFUN_KERNEL_API_C)

#include <cyg/kernel/kapi.h>

#ifdef CYGPKG_LIBC_STDIO

#include <stdio.h>
#include <stdlib.h>

#if defined(CYGPKG_LIBM)

#include <math.h>
#include <assert.h>

#if defined(CYGFUN_KERNEL_THREADS_TIMER)
#if defined(CYGPKG_LIBC_MALLOC)

/* if TIME_LIMIT is defined, it represents the number of seconds this
   test should last; if it is undefined the test will go forever */
#define DEATH_TIME_LIMIT 15
/*  #undef DEATH_TIME_LIMIT */

#define STACK_SIZE (CYGNUM_HAL_STACK_SIZE_TYPICAL)
#define STACK_SIZE2 (8*1024 + CYGNUM_HAL_STACK_SIZE_TYPICAL)

#define N_CLIENTS 4
#define N_LISTENERS 4
#define MAX_HANDLERS 19

#if (CYGNUM_KERNEL_SCHED_PRIORITIES < (N_CLIENTS+N_LISTENERS+MAX_HANDLERS))
# error "not enough priorities available"
#endif

/* if we use the bitmap scheduler we must make sure we don't use the
   same priority more than once, so we must store those already in use */
static char priority_in_use[N_CLIENTS+N_LISTENERS+MAX_HANDLERS];

/* now declare (and allocate space for) some kernel objects, like the
   threads we will use */
cyg_thread client_thread_s[N_CLIENTS];
cyg_thread listener_thread_s[N_LISTENERS];
cyg_thread handler_thread_s[MAX_HANDLERS];

/* space for stacks for all threads */
char client_stack[N_CLIENTS][STACK_SIZE];
char listener_stack[N_LISTENERS][STACK_SIZE];
char handler_stack[MAX_HANDLERS][STACK_SIZE2];

/* now the handles for the threads */
cyg_handle_t clientH[N_CLIENTS];
cyg_handle_t listenerH[N_LISTENERS];
cyg_handle_t handlerH[MAX_HANDLERS];

#ifdef DEATH_TIME_LIMIT
/* how many client threads have been killed by the death handler */
int n_clients_killed = 0;
#endif /* DEATH_TIME_LIMIT */

/* and now variables for the procedure which is the thread */
cyg_thread_entry_t client_program, listener_program, handler_program;

/* a few mutexes used in the code */
cyg_mutex_t client_request_lock, handler_slot_lock, statistics_print_lock;

/* a global variable with which the client and server coordinate */
int client_makes_request = 0;

/* indicates that it's time to print out a report */
int time_to_report = 0;

/*** now application-specific variables ***/
/* an array that stores whether the handler threads are in use */
int handler_thread_in_use[MAX_HANDLERS];

/***** statistics-gathering variables *****/
struct s_statistics {
  /* store the number of times each handler has been invoked */
  unsigned long handler_invocation_histogram[MAX_HANDLERS];

  /* store how many times malloc has been attempted and how many times
     it has failed */
  unsigned long malloc_tries, malloc_failures;

  /* how many threads have been created */
  unsigned long thread_creations, thread_exits;
};

struct s_statistics statistics;

/* some function prototypes; those with the sc_ prefix are
   "statistics-collecting" versions of the cyg_ primitives */
void sc_thread_create(
    cyg_addrword_t        sched_info,           /* scheduling info (eg pri)  */
    cyg_thread_entry_t  *entry,                 /* entry point function      */
    cyg_addrword_t        entry_data,           /* entry data                */
    char                *name,                  /* optional thread name      */
    void                *stack_base,            /* stack base, NULL = alloc  */
    cyg_ucount32        stack_size,             /* stack size, 0 = default   */
    cyg_handle_t        *handle,                /* returned thread handle    */
    cyg_thread          *thread                 /* put thread here           */
);
void sc_thread_exit(void);

int get_handler_slot(cyg_handle_t current_threadH);
void perform_stressful_tasks(void);
void permute_array(char a[], int size, int seed);
void setup_death_alarm(cyg_addrword_t data, cyg_handle_t *deathHp,
                       cyg_alarm *death_alarm_p, int *killed_p);
void handle_death(cyg_handle_t deathH, cyg_handle_t alarmH);
void print_statistics(void);

/* we need to declare the alarm handling function (which is defined
   below), so that we can pass it to cyg_alarm_initialize() */
cyg_alarm_t report_alarm_func, death_alarm_func;

/* handle and alarm for the report alarm */
cyg_handle_t report_alarmH, counterH, system_clockH;
cyg_alarm report_alarm;

/* we install our own startup routine which sets up threads */
void cyg_user_start(void)
{
  int i;

  CYG_TEST_INIT();
  CYG_TEST_INFO("# Entering stress's cyg_user_start() function");

  cyg_mutex_init(&client_request_lock);
  cyg_mutex_init(&statistics_print_lock);

  /* initialize statistics */
  memset(&statistics, 0, sizeof(statistics));

  /* initialize all handler threads to not be in use */
  for (i = 0; i < MAX_HANDLERS; ++i) {
    handler_thread_in_use[i] = 0;
  }
  for (i = 0; i < N_CLIENTS; ++i) {
    int prio;
    char thread_name[20];
    sprintf(thread_name, "client-%02d", i);
    prio = i;
    sc_thread_create(prio, client_program, (cyg_addrword_t) i,
                      thread_name, (void *) client_stack[i], STACK_SIZE,
                      &(clientH[i]), &client_thread_s[i]);
    priority_in_use[prio] = 1;
  }
  for (i = 0; i < N_LISTENERS; ++i) {
    int prio;
    char thread_name[20];
    sprintf(thread_name, "listener-%02d", i);
    prio = N_CLIENTS + i;
    sc_thread_create(prio, listener_program, (cyg_addrword_t) i,
                      thread_name, (void *) listener_stack[i], STACK_SIZE,
                      &listenerH[i], &listener_thread_s[i]);
    priority_in_use[prio] = 1;
  }

  for (i = 0; i < N_CLIENTS; ++i) {
    cyg_thread_resume(clientH[i]);
  }
  for (i = 0; i < N_LISTENERS; ++i) {
    cyg_thread_resume(listenerH[i]);
  }

  /* set up the alarm which gives periodic wakeups to say "time to
     print a report */
  system_clockH = cyg_real_time_clock();
  cyg_clock_to_counter(system_clockH, &counterH);

  cyg_alarm_create(counterH, report_alarm_func,
                   (cyg_addrword_t) 4000,
                   &report_alarmH, &report_alarm);
  if (cyg_test_is_simulator) {
    cyg_alarm_initialize(report_alarmH, cyg_current_time()+300, 400);
  } else {
    cyg_alarm_initialize(report_alarmH, cyg_current_time()+300, 4000);
  }

}

/* client_program() -- an obnoxious client which makes a lot of requests */
void client_program(cyg_addrword_t data)
{
  int delay;

  cyg_handle_t counterH, deathH, system_clockH;
  cyg_alarm death_alarm;
  int is_dead = 0;

  setup_death_alarm(data, &deathH, &death_alarm, &is_dead);

  printf("# Starting client-%d\n", (int) data);

  system_clockH = cyg_real_time_clock();
  cyg_clock_to_counter(system_clockH, &counterH);

  for (;;) {
    delay = (rand() % 3);

    /* now send a request to the server */
    cyg_mutex_lock(&client_request_lock); {
      ++client_makes_request;
/*        printf("client_makes_request        %d\n", client_makes_request); */
    } cyg_mutex_unlock(&client_request_lock);

    cyg_thread_delay(10+delay);
/*      cyg_thread_delay(0); */
#ifdef DEATH_TIME_LIMIT
    if (is_dead) {
      handle_death(deathH, report_alarmH);
    }
#endif /* DEATH_TIME_LIMIT */
  }
}

/* listener_program() -- listens for a request and spawns a handler to
   take care of the request */
void listener_program(cyg_addrword_t data)
{
/*   int message = (int) data; */
  int handler_slot;

  printf("# Beginning execution; thread data is %d\n", (int) data);

  for (;;) {
#ifdef DEATH_TIME_LIMIT
    /* as an extra task, the listener sees if all clients have been
       killed off, so it can report that the test is over */
    if (n_clients_killed == N_CLIENTS) {
      n_clients_killed = -1;    /* so we don't call this again */
      CYG_TEST_PASS_FINISH("Kernel thread stress test OK");
    }
#endif /* DEATH_TIME_LIMIT */
    if (client_makes_request > 0) {
      int prio;
      /*          printf("just got a request from a client (count = %d)\n", */
      /*                 client_makes_request); */
      cyg_mutex_lock(&client_request_lock); {
        --client_makes_request;
      } cyg_mutex_unlock(&client_request_lock);

      handler_slot = get_handler_slot(listenerH[(int) data]);
      prio = N_CLIENTS+N_LISTENERS+handler_slot;
      priority_in_use[prio] = 1;
      sc_thread_create(prio, handler_program,
                       (cyg_addrword_t) handler_slot,
                       "handler", (void *) handler_stack[handler_slot],
                       STACK_SIZE2, &handlerH[handler_slot],
                       &handler_thread_s[handler_slot]);
      cyg_thread_resume(handlerH[handler_slot]);
      ++statistics.handler_invocation_histogram[handler_slot];
    }
    cyg_thread_delay(1);
  }
}

/* handler_program() -- is spawned to handle each incoming request */
void handler_program(cyg_addrword_t data)
{
  /* here is where we perform specific stressful tasks */
  perform_stressful_tasks();

  if (time_to_report) {
    time_to_report = 0;
    print_statistics();
  }

  cyg_thread_delay(4 + (int) (0.5*log(1.0 + fabs((rand() % 1000000)))));
/*    cyg_thread_delay(0); */

  /* lock the scheduler before we declare this thread slot available
     and quit; note that cyg_thread_exit() will unlock the scheduler
     as many times as necessary */
  cyg_mutex_lock(&handler_slot_lock); {
    handler_thread_in_use[data] = 0;
    priority_in_use[N_CLIENTS + N_LISTENERS + (int) data] = 0;
  } cyg_mutex_unlock(&handler_slot_lock);
  /* FIXME: could there be a race condition right here? I unlock the
     scheduler, so I could get pre-empted out, but meanwhile I have
     declared this thread available again.  must fix it. */
  sc_thread_exit();
}

/* look for an available handler thread */
int get_handler_slot(cyg_handle_t current_threadH)
{
  int i;
  int found = 0;

  while (!found) {
    for (i = 0; i < MAX_HANDLERS; ++i) {
      cyg_mutex_lock(&handler_slot_lock); {
        if (!handler_thread_in_use[i]) {
          found = 1;
          handler_thread_in_use[i] = 1;
        }
      } cyg_mutex_unlock(&handler_slot_lock);
      if (found) {
        break;
      }
#ifdef DEATH_TIME_LIMIT
      /* must do a check here to see if all clients have been killed,
         since otherwise we might end up in an infinite loop */
      if (n_clients_killed == N_CLIENTS) {
	n_clients_killed = -1;    /* so we don't call this again */
	CYG_TEST_PASS_FINISH("Kernel thread stress test OK");
      }
#endif
    }
    cyg_thread_delay(1);
  }
  return i;
}

/* do things which will stress the system */
void perform_stressful_tasks()
{
#define MAX_MALLOCED_SPACES 100  /* do this many mallocs at most */
#define MALLOCED_BASE_SIZE 1    /* basic size in bytes */
  char *spaces[MAX_MALLOCED_SPACES];
  unsigned int i;

  cyg_mutex_t tmp_lock;

  cyg_uint8 pool_space[10][100];
  cyg_handle_t mempool_handles[10];
  cyg_mempool_fix mempool_objects[10];

  cyg_mutex_init(&tmp_lock);

  /* here I use malloc, which uses the kernel's variable memory pools.
     note that malloc/free is a bit simple-minded here: it does not
     try to really fragment things, and it does not try to make the
     allocation/deallocation concurrent with other thread execution
     (although I'm about to throw in a yield()) */
  for (i = 0; i < MAX_MALLOCED_SPACES; ++i) {
    ++statistics.malloc_tries;
/*      spaces[i] = (char *) malloc(((int)(sqrt(i*2.0))+1)*MALLOCED_BASE_SIZE); */
    spaces[i] = (char *) malloc(((int)i*2.0+1)*MALLOCED_BASE_SIZE);
    if (i % 100 == 0) {
      cyg_thread_yield();
    }
  }

  /* now free it all up */
  for (i = 0; i < MAX_MALLOCED_SPACES; ++i) {
    if (spaces[i] != NULL) {
      unsigned int j;
      for (j = 0; j < (i*2+1)*MALLOCED_BASE_SIZE; ++j) {
	spaces[i][j] = 0xAA;	/* write a bit pattern */
      }
      free(spaces[i]);
    } else {
      ++statistics.malloc_failures;
    }
  }
  /* now allocate and then free some fixed-size memory pools; for
     now this is simple-minded because it does not have many threads
     sharing the memory pools and racing for memory. */
  for (i = 0; i < 10; ++i) {
    cyg_mempool_fix_create(pool_space[i], 100, (i+1)*3,
                           &mempool_handles[i], &mempool_objects[i]);
  }

  for (i = 0; i < 10; ++i) {
    spaces[i] = cyg_mempool_fix_try_alloc(mempool_handles[i]);
  }

  for (i = 0; i < 10; ++i) {
    if (spaces[i]) {
      cyg_mempool_fix_delete(mempool_handles[i]);
    }
  }

  cyg_mutex_destroy(&tmp_lock);
}

/* report_alarm_func() is invoked as an alarm handler, so it should be
   quick and simple.  in this case it sets a global flag which is
   checked by threads. */
void report_alarm_func(cyg_handle_t alarmH, cyg_addrword_t data)
{
    time_to_report = 1;
}

/* this sets up death alarms. it gets the handle and alarm from the
   caller, since they must persist for the life of the alarm */
void setup_death_alarm(cyg_addrword_t data, cyg_handle_t *deathHp,
                       cyg_alarm *death_alarm_p, int *killed_p)
{
#ifdef DEATH_TIME_LIMIT
  cyg_handle_t system_clockH, counterH;
  cyg_resolution_t rtc_res;

  system_clockH = cyg_real_time_clock();
  cyg_clock_to_counter(system_clockH, &counterH);

  cyg_alarm_create(counterH, death_alarm_func,
                   (cyg_addrword_t) killed_p,
                   deathHp, death_alarm_p);
  rtc_res = cyg_clock_get_resolution(system_clockH);
  {
    cyg_tick_count_t tick_delay;
    tick_delay = (long long)
      ((1000000000.0*rtc_res.divisor)
       *((double)DEATH_TIME_LIMIT)/((double)rtc_res.dividend));
    if ( cyg_test_is_simulator )
        tick_delay /= 10;
    cyg_alarm_initialize(*deathHp, cyg_current_time() + tick_delay, 0);
  }
#endif /* DEATH_TIME_LIMIT */
}

/* death_alarm_func() is the alarm handler that kills the current
   thread after a specified timeout. It does so by setting a flag the
   thread is constantly checking. */
void death_alarm_func(cyg_handle_t alarmH, cyg_addrword_t data)
{
  int *killed_p;
  killed_p = (int *) data;
  *killed_p = 1;
}

#ifdef DEATH_TIME_LIMIT
/* handle_death is called by a client thread when it dies; it kills
   off the alarm */
void handle_death(cyg_handle_t deathH, cyg_handle_t alarmH)
{
  ++n_clients_killed;
  cyg_alarm_delete(deathH);
  cyg_alarm_delete(alarmH);
  cyg_thread_exit();
}
#endif /* DEATH_TIME_LIMIT */

/* now I write the sc_ versions of the cyg_functions */
void sc_thread_create(
    cyg_addrword_t        sched_info,          /* scheduling info (eg pri)  */
    cyg_thread_entry_t  *entry,                /* entry point function      */
    cyg_addrword_t        entry_data,          /* entry data                */
    char                *name,                 /* optional thread name      */
    void                *stack_base,           /* stack base, NULL = alloc  */
    cyg_ucount32        stack_size,            /* stack size, 0 = default   */
    cyg_handle_t        *handle,               /* returned thread handle    */
    cyg_thread          *thread                /* put thread here           */
)
{
/*printf("Creating a thread -- priority is %lu\n", (unsigned long) sched_info);*/
/*    fflush(stdout); */
  ++statistics.thread_creations;
  cyg_thread_create(sched_info, entry, entry_data, name,
                    stack_base, stack_size, handle, thread);
}

void sc_thread_exit()
{
/*    printf("exiting\n"); */
/*    fflush(stdout); */
  ++statistics.thread_exits;
  cyg_thread_exit();
}

void print_statistics(void)
{
  int i;

  cyg_mutex_lock(&statistics_print_lock); {
    printf("Handler-invocations: ");
    for (i = 0; i < MAX_HANDLERS; ++i) {
      printf("%4lu ", statistics.handler_invocation_histogram[i]);
    }
    printf("\n");
    printf("malloc()-tries/failures: -- %7lu %7lu\n",
           statistics.malloc_tries, statistics.malloc_failures);
    printf("client_makes_request:       %d\n", client_makes_request);
  } cyg_mutex_unlock(&statistics_print_lock);
}

#else /* CYGSEM_LIBC_MALLOC */
# define N_A_MSG "this test needs malloc"
#endif /* CYGSEM_LIBC_MALLOC */

#else /* CYGFUN_KERNEL_THREADS_TIMER */
# define N_A_MSG "this test needs kernel threads timer"
#endif /* CYGFUN_KERNEL_THREADS_TIMER */

#else /* CYGPKG_LIBM */
# define N_A_MSG "this test needs libm"
#endif /* CYGPKG_LIBM */

#else /* CYGSEM_LIBC_STDIO */
# define N_A_MSG "this test needs stdio"
#endif /* CYGSEM_LIBC_STDIO */

#else // def CYGFUN_KERNEL_API_C
# define N_A_MSG "this test needs Kernel C API"
#endif

#else // def CYGPKG_KERNEL && CYGPKG_IO && CYGPKG_LIBC
# define N_A_MSG "this tests needs Kernel, libc and IO"
#endif

#ifdef N_A_MSG
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA( N_A_MSG);
}
#endif // N_A_MSG
