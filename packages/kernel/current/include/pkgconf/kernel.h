#ifndef CYGONCE_PKGCONF_KERNEL_H
#define CYGONCE_PKGCONF_KERNEL_H
// ====================================================================
//
//	pkgconf/kernel.h
//
//	Kernel configuration file
//
// ====================================================================
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
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	nickg
// Contributors:	nickg
// Date:	1997-09-29	
// Purpose:	To allow the user to edit kernel configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

/* ---------------------------------------------------------------------
 * Some of the kernel configuration options depend on global options,
 * so it is necessary to include the global options first.
 */

#include <pkgconf/system.h>

/* Only define options if the kernel is to be present */
#ifdef CYGPKG_KERNEL

#include <pkgconf/hal.h>
#include <pkgconf/infra.h>

/* ---------------------------------------------------------------------
 * The overall package definition.

 {{CFG_DATA
 
 cdl_package CYGPKG_KERNEL {
     display  "eCos kernel"
     type     boolean
     requires CYGFUN_HAL_COMMON_KERNEL_SUPPORT
     description "
         This package contains the core functionality of the eCos
         kernel. It relies on functionality provided by various HAL
         packages and by the eCos infrastructure. In turn the eCos
         kernel provides support for other packages such as the device
         drivers and the uITRON compatibility layer."
     doc ref/ecos-ref/ecos-kernel-overview.html
 }

 }}CFG_DATA */

/* ---------------------------------------------------------------------
 * The first component within the kernel is related to interrupt
 * handling.
 {{CFG_DATA

 cdl_component CYGPKG_KERNEL_INTERRUPTS {
     display "Kernel interrupt handling"
     parent  CYGPKG_KERNEL
     type    dummy
     description "
         The majority of configuration options related to interrupt
         handling are in the HAL packages, since usually the code has
         to be platform-specific. There are a number of options
         provided within the kernel related to slightly higher-level
         concepts, for example Delayed Service Routines."
     doc ref/ecos-ref/interrupts.html
 }

 # In the absence of active-if support this has to be
 # a sub-component. It does not use the CYGPKG_ prefix to make
 # it easier to change back into an ordinary option later on.
 cdl_component CYGIMP_KERNEL_INTERRUPTS_DSRS {
     display     "Use delayed service routines (DSRs)"
     parent      CYGPKG_KERNEL_INTERRUPTS
     description "
         In eCos the recommended way to handle device interrupts is to
         do a minimum amount of work inside the low level interrupt
         handler itself, and instead do as much as possible in a
         Delayed Service Routine or DSR. If an application does not
         make use of DSRs directly or indirectly then it is possible
         to disable the DSR support completely, which reduces the
         overheads of context switches and interrupt handling. Note
         that the kernel real-time clock makes use of DSRs, as do many
         of the device drivers. "
     doc ref/ecos-ref/interrupt-and-exception-handlers.html
     }

 # NOTE: the choice of list vs table should not be two separate
 # options. There is a single option which must have one of
 # two legal values.
 cdl_option CYGIMP_KERNEL_INTERRUPTS_DSRS_LIST {
     display "Use linked lists for DSRs"
     parent  CYGIMP_KERNEL_INTERRUPTS_DSRS
     type    radio
     description "
         When DSR support is enabled the kernel must keep track of all
         the DSRs that are pending. This information can be kept in a
         fixed-size table or in a linked list. The list implementation
         requires that the kernel disable interrupts for a very short
         period of time outside interrupt handlers, but there is no
         possibility of a table overflow occurring."
     doc ref/ecos-ref/interrupts.html
 }

 cdl_option CYGIMP_KERNEL_INTERRUPTS_DSRS_TABLE {
     display "Use fixed-size table for DSRs"
     parent  CYGIMP_KERNEL_INTERRUPTS_DSRS
     type    radio
     description "
         When DSR support is enabled the kernel must keep track of all
         the DSRs that are pending. This information can be kept in a
         fixed-size table or in a linked list. The table
         implementation involves a very small risk of overflow at
         run-time if a given interrupt source is able to have more
         than one pending DSR. However it has the advantage that
         the kernel does not need to disable interrupts outside
         interrupt handlers."
     doc ref/ecos-ref/interrupts.html
 }

 cdl_option CYGNUM_KERNEL_INTERRUPTS_DSRS_TABLE_SIZE {
     display            "Number of entries in fixed-size DSR table"
     parent             CYGIMP_KERNEL_INTERRUPTS_DSRS
     type               count
     legal_values       2 to 1024
     #active_if CYGIMP_KERNEL_INTERRUPTS_DSRS_TABLE
     description "
         When DSR support is enabled the kernel must keep track of all
         the DSRs that are pending. One approach involves a fixed-size
         table, which involves a very small risk of overflow at
         run-time. By increasing the table size it is possible to reduce
         this risk."
     doc ref/ecos-ref/interrupts.html
     }

 cdl_option CYGIMP_KERNEL_INTERRUPTS_CHAIN {
     display            "Chain all interrupts together"
     parent             CYGPKG_KERNEL_INTERRUPTS
     requires           CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN
     description "
         Interrupts can be attached to vectors either singly, or be
         chained together. The latter is necessary if there is no way
         of discovering which device has interrupted without
         inspecting the device itself. It can also reduce the amount
         of RAM needed for interrupt decoding tables and code."
     doc ref/ecos-ref/interrupts.html
 }

 }}CFG_DATA */

#define CYGIMP_KERNEL_INTERRUPTS_DSRS
#define CYGIMP_KERNEL_INTERRUPTS_DSRS_LIST
#undef  CYGIMP_KERNEL_INTERRUPTS_DSRS_TABLE
#define CYGNUM_KERNEL_INTERRUPTS_DSRS_TABLE_SIZE 32
#undef  CYGIMP_KERNEL_INTERRUPTS_CHAIN

/* ---------------------------------------------------------------------
 * Exceptions. Currently there are only two options. The first
 * determines whether or not exceptions are enabled at all. The
 * second controls whether they apply globally or on a per-thread
 * basis. There should probably be more options, but the boundary
 * between the HAL and kernel becomes blurred.
 
   {{CFG_DATA

   cdl_component CYGPKG_KERNEL_EXCEPTIONS {
       display "Exception handling"
       parent  CYGPKG_KERNEL
       requires CYGPKG_HAL_EXCEPTIONS
       description "
           In the context of the eCos kernel exceptions are unexpected
           events detected by the hardware, for example an attempt to
           execute an illegal instruction. There is no relation with
           other forms of exception, for example the catch and throw
           facilities of languages like C++. It is possible to disable
           all support for exceptions and thus save some memory."
       doc ref/ecos-ref/exceptions.html
   }

   cdl_option CYGSEM_KERNEL_EXCEPTIONS_DECODE {
       display "Decode exception types in kernel"
       parent  CYGPKG_KERNEL_EXCEPTIONS
       description "
           On targets where several different types of exception are
           possible, for example executing an illegal instruction and
           division by zero, it is possible for the kernel to do some
           decoding of the exception type and deliver the different
           types of exception to different handlers in the application
           code. Alternatively the kernel can simply pass all
           exceptions directly to application code, leaving the
           decoding to be done by the application"
       doc ref/ecos-ref/exceptions.html
   }
   
   cdl_option CYGSEM_KERNEL_EXCEPTIONS_GLOBAL {
       display "Use global exception handlers"
       parent  CYGPKG_KERNEL_EXCEPTIONS
       description "
           In the context of the eCos kernel exceptions are unexpected
           events detected by the hardware, for example an attempt to
           execute an illegal instruction. If the kernel is configured
           to support exceptions then two implementations are
           possible. The default implementation involves a single set
           of exception handlers that are in use for the entire
           system. The alternative implementation allows different
           exception handlers to be specified for each thread."
       doc ref/ecos-ref/exceptions.html
   }
   
   }}CFG_DATA */

#define CYGPKG_KERNEL_EXCEPTIONS
#undef  CYGSEM_KERNEL_EXCEPTIONS_DECODE
#define CYGSEM_KERNEL_EXCEPTIONS_GLOBAL


/* ---------------------------------------------------------------------
 * {{CFG_DATA

 cdl_component CYGPKG_KERNEL_SCHED {
     display "Kernel schedulers"
     type    dummy
     parent  CYGPKG_KERNEL
     description "
         The eCos kernel provides a choice of schedulers. In addition
         there are a number of configuration options to control the
         detailed behaviour of these schedulers.
     "
     doc ref/ecos-ref/ecos-kernel-overview.html#THE-SCHEDULER
 }

 cdl_option CYGSEM_KERNEL_SCHED_MLQUEUE {
     display    "Multi-level queue scheduler"
     type       radio
     parent     CYGPKG_KERNEL_SCHED
     description "
         The multi-level queue scheduler supports multiple priority
         levels and multiple threads at each priority level.
         Preemption between priority levels is automatic. Timeslicing
         within a given priority level is controlled by a separate
         configuration option"
     doc ref/ecos-ref/ecos-kernel-overview.html#THE-SCHEDULER
 }
 
 cdl_option CYGSEM_KERNEL_SCHED_BITMAP {
     display    "Bitmap scheduler"
     type       radio
     parent     CYGPKG_KERNEL_SCHED
     description "
         The bitmap scheduler supports multiple priority levels but
         only one thread can exist at each priority level. This means
         that scheduling decisions are very simple and hence the
         scheduler is efficient. Preemption between priority levels is
         automatic. Timeslicing within a given priority level is
         irrelevant since there can be only one thread at each
         priority level."
     doc ref/ecos-ref/ecos-kernel-overview.html#THE-SCHEDULER
 }

 #cdl_option CYGSEM_KERNEL_SCHED_LOTTERY {
 #    display    "Lottery scheduler"
 #    type       radio
 #    parent     CYGPKG_KERNEL_SCHED
 #    description "
 #       This scheduler is not yet available."
 #}

 # NOTE: this option only makes sense if the current scheduler
 # supports multiple priority levels.
 cdl_option CYGNUM_KERNEL_SCHED_PRIORITIES {
     display            "Number of priority levels"
     type               count
     legal_values       1 to 32
     parent             CYGPKG_KERNEL_SCHED
     #active_if         CYGINT_KERNEL_SCHED_PRIORITY_SCHEDULER
     description "
         This option controls the number of priority levels that are
         available. For some types of scheduler including the bitmap
         scheduler this may impose an upper bound on the number of
         threads in the system. For other schedulers such as the
         mlqueue scheduler the number of threads is independent from
         the number of priority levels. Note that the lowest priority
         level is normally used only by the idle thread, although
         application threads can run at this priority if necessary."
     doc ref/ecos-ref/ecos-kernel-overview.html#THE-SCHEDULER
 }

  # NOTE: this option only makes sense for some of the schedulers.
  # Timeslicing is irrelevant for bitmap schedulers.
  cdl_option CYGSEM_KERNEL_SCHED_TIMESLICE {
      display           "Scheduler timeslicing"
      parent            CYGPKG_KERNEL_SCHED
      requires          !CYGSEM_KERNEL_SCHED_BITMAP
      requires          CYGVAR_KERNEL_COUNTERS_CLOCK
      description "
          Some schedulers including the mlqueue scheduler support
          timeslicing. This means that the kernel will check regularly
          whether or not there is another runnable thread with the
          same priority, and if there is such a thread there will be
          an automatic context switch. Not all applications require
          timeslicing, for example because every thread performs a
          blocking operation regularly. For these applications it is
          possible to disable timeslicing, which reduces the overheads
          associated with timer interrupts."
  }

  cdl_option CYGNUM_KERNEL_SCHED_TIMESLICE_TICKS {
      display           "Number of clock ticks between timeslices"
      parent            CYGPKG_KERNEL_SCHED
      type              count
      legal_values      1 to 65535
      #active_if CYGSEM_KERNEL_SCHED_TIMESLICE
      description "
          Assuming timeslicing is enabled, how frequently should it
          take place? The value of this option corresponds to the
          number of clock ticks that should occur before a timeslice
          takes place, so increasing the value reduces the frequency
          of timeslices."
  }
  
 }}CFG_DATA */

#define CYGSEM_KERNEL_SCHED_MLQUEUE
#undef  CYGSEM_KERNEL_SCHED_BITMAP
#undef  CYGSEM_KERNEL_SCHED_LOTTERY
#define CYGNUM_KERNEL_SCHED_PRIORITIES          32
#define CYGSEM_KERNEL_SCHED_TIMESLICE
#define CYGNUM_KERNEL_SCHED_TIMESLICE_TICKS      5

/* ---------------------------------------------------------------------
 * Counters and clocks.

  {{CFG_DATA
   
  cdl_component CYGPKG_KERNEL_COUNTERS {
      display "Counters and clocks"
      parent  CYGPKG_KERNEL
      type    dummy
      description "
          The counter objects provided by the kernel provide an
          abstraction of the clock facility that is generally provided.
          Application code can associate alarms with counters, where an
          alarm is identified by the number of ticks until it triggers,
          the action to be taken on triggering, and whether or not the
          alarm should be repeated."
     doc ref/ecos-ref/counters-clocks-and-alarms.html
  }

  cdl_option CYGVAR_KERNEL_COUNTERS_CLOCK {
      display "Provide real-time clock"
      parent  CYGPKG_KERNEL_COUNTERS
      requires CYGIMP_KERNEL_INTERRUPTS_DSRS
      description "
          On all current target systems the kernel can provide a
          real-time clock. This clock serves two purposes. First it is
          necessary to support clock and alarm related functions.
          Second it is needed to implement timeslicing in some of the
          schedulers including the mlqueue scheduler. If the
          application does not require any of these facilities then it
          is possible to disable the real time clock support
          completely."
     doc ref/ecos-ref/counters-clocks-and-alarms.html
  }

  cdl_component CYGPKG_KERNEL_COUNTERS_CLOCK_OVERRIDE {
      display   "Override default clock settings"
      parent    CYGPKG_KERNEL_COUNTERS
      requires  CYGVAR_KERNEL_COUNTERS_CLOCK
      type      bool
      description "
          The kernel has default settings for the clock interrupt
          frequency. These settings will vary from platform to
          platform, but typically there will be a 100 clock interrupts
          every second. It is possible to change this frequency, but
          it requires some knowledge of the target hardware.
      "
  }

  cdl_option CYGNUM_KERNEL_COUNTERS_CLOCK_OVERRIDE_PERIOD {
      display       "Clock hardware initialization value"
      parent        CYGPKG_KERNEL_COUNTERS_CLOCK_OVERRIDE
      type          count
      legal_values  1 to 0x7fffffff
      description   "
          During system initialization this value is used to initialize
          the clock hardware. The exact meaning of the value and the
          range of legal values therefore depends on the target hardware,
          and the hardware documentation should be consulted for further
          details. In addition the clock resolution numerator and
          denominator values should be updated. Typical values for
          this option would be 150000 on the MN10300 stdeval1 board,
          15625 on the tx39 jmr3904 board, and 20625 on the powerpc
          cogent board."
  }

  cdl_option CYGNUM_KERNEL_COUNTERS_CLOCK_OVERRIDE_NUMERATOR {
      display       "Clock resolution numerator"
      parent        CYGPKG_KERNEL_COUNTERS_CLOCK_OVERRIDE
      type          count
      legal_values  1 to 0x7fffffff
      description "
          If a non-default clock interrupt frequency is used then it
          is necessary to specify the clock resolution explicitly.
          This resolution involves two separate values, the numerator
          and the denominator. The result of dividing the numerator by
          the denominator should correspond to the number of
          nanoseconds between clock interrupts. For example a
          numerator of 1000000000 and a denominator of 100 means that
          there are 10000000 nanoseconds (or 10 milliseconds) between
          clock interrupts. Expressing the resolution as a fraction
          should minimize clock drift even for frequencies that cannot
          be expressed as a simple integer. For example a frequency of
          60Hz corresponds to a clock resolution of 16666666.66...
          nanoseconds. This can be expressed accurately as 1000000000
          over 60."
  }

  cdl_option CYGNUM_KERNEL_COUNTERS_CLOCK_OVERRIDE_DENOMINATOR {
      display       "Clock resolution denominator"
      parent        CYGPKG_KERNEL_COUNTERS_CLOCK_OVERRIDE
      type          count
      legal_values  1 to 0x7fffffff
      description "
          If a non-default clock interrupt frequency is used then it
          is necessary to specify the clock resolution explicitly.
          This resolution involves two separate values, the numerator
          and the denominator. The result of dividing the numerator by
          the denominator should correspond to the number of
          nanoseconds between clock interrupts. For example a
          numerator of 1000000000 and a denominator of 100 means that
          there are 10000000 nanoseconds (or 10 milliseconds) between
          clock interrupts. Expressing the resolution as a fraction
          should minimize clock drift even for frequencies that cannot
          be expressed as a simple integer. For example a frequency of
          60Hz corresponds to a clock resolution of 16666666.66...
          nanoseconds. This can be expressed accurately as 1000000000
          over 60."
  }
  
  
  # NOTE: these option should really be a single enum.
  cdl_option CYGIMP_KERNEL_COUNTERS_SINGLE_LIST {
      display "Implement counters using a single list"
      parent  CYGPKG_KERNEL_COUNTERS
      type    radio
      description "
          There are two different implementations of the counter
          objects. The first implementation stores all alarms in a
          single linked list. The alternative implementation uses a
          table of linked lists. A single list is more efficient in
          terms of memory usage and is generally adequate when the
          application only makes use of a small number of alarms."
     doc ref/ecos-ref/counters-clocks-and-alarms.html
 }

 cdl_option CYGIMP_KERNEL_COUNTERS_MULTI_LIST {
     display "Implement counters using a table of lists"
     parent  CYGPKG_KERNEL_COUNTERS
     type    radio
     description "
          There are two different implementations of the counter
          objects. The first implementation stores all alarms in a
          single linked list. The alternative implementation uses a
          table of linked lists, with the size of the table being a
          separate configurable option. For more complicated
          operations it is better to have a table of lists since this
          reduces the amount of computation whenever the timer goes
          off. Assuming a table size of 8 (the default value) on
          average the timer code will only need to check 1/8 of the
          pending alarms instead of all of them."
     doc ref/ecos-ref/counters-clocks-and-alarms.html
 }

 cdl_option CYGNUM_KERNEL_COUNTERS_MULTI_LIST_SIZE {
     display            "Size of counter list table"
     parent             CYGPKG_KERNEL_COUNTERS
     type               count
     legal_values       1 to 1024
     #active_if CYGIMP_KERNEL_COUNTERS_MULTI_LIST
     description "
         If counters are implemented using an array of linked lists
         then this option controls the size of the array. A larger
         size reduces the amount of computation that needs to take
         place whenever the timer goes off, but requires extra
         memory."
     doc ref/ecos-ref/counters-clocks-and-alarms.html
 }

 }}CFG_DATA */

#define CYGVAR_KERNEL_COUNTERS_CLOCK
#undef  CYGPKG_KERNEL_COUNTERS_CLOCK_OVERRIDE
#define CYGNUM_KERNEL_COUNTERS_CLOCK_OVERRIDE_PERIOD      9999
#define CYGNUM_KERNEL_COUNTERS_CLOCK_OVERRIDE_NUMERATOR   1000000000
#define CYGNUM_KERNEL_COUNTERS_CLOCK_OVERRIDE_DENOMINATOR 100
#define CYGIMP_KERNEL_COUNTERS_SINGLE_LIST
#undef  CYGIMP_KERNEL_COUNTERS_MULTI_LIST
#define CYGNUM_KERNEL_COUNTERS_MULTI_LIST_SIZE 8

/* ---------------------------------------------------------------------
 * Thread-related options
 {{CFG_DATA

 cdl_component CYGPKG_KERNEL_THREADS {
     display            "Thread-related options"
     type               dummy
     parent             CYGPKG_KERNEL
     description "
         There are a number of configuration options related to the
         implementation of threads, for example whether or not the
         eCos kernel supports per-thread data."
     doc ref/ecos-ref/thread-operations.html
 }

 cdl_option CYGFUN_KERNEL_THREADS_TIMER {
     display            "Allow per-thread timers"
     parent             CYGPKG_KERNEL_THREADS
     requires           CYGVAR_KERNEL_COUNTERS_CLOCK
     description "
         This option controls whether or not the kernel should support
         per-thread clock and alarm related functions. Also some of
         the synchronization primitives such as semaphore and
         condition variable timed wait operations require per-thread
         timer support. If none of these facilities are required then
         the option can be disabled."
     doc ref/ecos-ref/thread-operations.html
 }

 cdl_option CYGVAR_KERNEL_THREADS_NAME {
     display            "Support optional name for each thread"
     parent             CYGPKG_KERNEL_THREADS
     description "
         Threads may optionally be supplied with a name string that is
         used to identify them during debugging. This name is only
         present if `this option is defined. Disabling it reduces both
         code and data size."
     doc ref/ecos-ref/thread-operations.html
 }

 cdl_option CYGVAR_KERNEL_THREADS_LIST {
     display            "Keep track of all threads using a linked list"
     parent             CYGPKG_KERNEL_THREADS
     description "
         Threads may optionally be placed on a housekeeping list so
         that all threads may be located easily. This is useful mainly
         in conjunction with source-level debugging."
     doc ref/ecos-ref/thread-operations.html
 }

 cdl_option CYGFUN_KERNEL_THREADS_STACK_LIMIT {
     display            "Keep track of the base of each thread's stack"
     parent             CYGPKG_KERNEL_THREADS
     description "
         This option makes the kernel keep track of the lower limit on
         each thread's stack. It allows the kernel to adjust the lower
         limit, thus making space for per-thread data. Note that it
         does not imply any form of run-time stack overflow checking."
     doc ref/ecos-ref/thread-operations.html
 }
 
 cdl_option CYGVAR_KERNEL_THREADS_DATA {
     display            "Support for per-thread data"
     parent             CYGPKG_KERNEL_THREADS
     requires           CYGFUN_KERNEL_THREADS_STACK_LIMIT
     description "
         It is possible for the kernel to support per-thread data, in
         other words an area of memory specific to each thread which
         can be used to store data for that thread. This per-thread
         data can be used by applications or by other packages such as
         the ISO C library."
     doc ref/ecos-ref/thread-operations.html
 }

 cdl_option CYGNUM_KERNEL_THREADS_DATA_MAX {
     display            "Number of words of per-thread data"
     parent             CYGPKG_KERNEL_THREADS
     #active_if         CYGVAR_KERNEL_THREADS_DATA
     type               count
     legal_values       1 to 65535
     description "
         It is possible for the kernel to support per-thread data, in
         other words an area of memory specific to each thread which
         can be used to store data for that thread. This per-thread
         data can be used by applications or by other packages such as
         the ISO C library. This configuration option controls the
         number of words of per-thread data that the kernel will
         allow."
     doc ref/ecos-ref/thread-operations.html
 }

 cdl_option CYGNUM_KERNEL_THREADS_IDLE_STACK_SIZE {
     display            "Stack size for the idle thread"
     parent             CYGPKG_KERNEL_THREADS
     type               count
     legal_values       512 to 65536
     description "
         This configuration option specifies the stack size in bytes
         for the idle thread. Unless the HAL is configured to use a
         separate interrupt stack this size must be sufficient to meet
         the requirements of all interrupt handlers - these
         requirements are cumulative if nested interrupted are
         enabled. Depending on the target architecture, the stack size
         typically has to be a multiple of eight or sixteen bytes."
         
     doc ref/ecos-ref/thread-operations.html
 }

 }}CFG_DATA */

#define CYGFUN_KERNEL_THREADS_TIMER
#define CYGVAR_KERNEL_THREADS_NAME
#define CYGVAR_KERNEL_THREADS_LIST
#define CYGFUN_KERNEL_THREADS_STACK_LIMIT
#define CYGVAR_KERNEL_THREADS_DATA
#define CYGNUM_KERNEL_THREADS_DATA_MAX          6
#define CYGNUM_KERNEL_THREADS_IDLE_STACK_SIZE   2048


/* ---------------------------------------------------------------------
 * Synchronization primitives.

 {{CFG_DATA

 cdl_component CYGPKG_KERNEL_SYNCH {
     display    "Synchronization primitives"
     type       dummy
     parent     CYGPKG_KERNEL
     description "
         The eCos kernel supports a number of different
         synchronization primitives such as mutexes, semaphores,
         condition variables, and message boxes. There are
         configuration options to control the exact behaviour of some
         of these synchronization primitives.
     "
     doc ref/ecos-ref/thread-synchronization.html
 }

 # NOTE: the requires statement is only valid for the current kernel.
 # Priority inheritance is possible in other schedulers as well
 # but is not yet implemented.
 cdl_option CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE_SIMPLE {
     display    "Simple mutex priority inheritance"
     requires   CYGSEM_KERNEL_SCHED_MLQUEUE
     parent     CYGPKG_KERNEL_SYNCH
     description "
         This option enables a relatively simple implementation of
         mutex priority inheritance. The implementation will only work
         in the mlqueue scheduler, and it does not handle the rare
         case of nested mutexes completely correctly. However it is
         both fast and deterministic. Mutex priority inheritance can
         be disabled if the application does not require it, which
         will reduce both code size and data space."
     doc ref/ecos-ref/synchronization.html#MUTEXES
 }

 cdl_option CYGMFN_KERNEL_SYNCH_MBOXT_PUT_CAN_WAIT {
     display    "Message box blocking put support"
     parent     CYGPKG_KERNEL_SYNCH
     description "
         Message boxes can support three different versions of the
         put-message operation. The first is tryput(), which will fail
         if the message box is already full. The other two are the
         ordinary put() function which will block if the message box
         is full, and a timed put() operation which will block for
         upto a certain length of time if the message box is currently
         full. The blocking versions require extra memory in the
         message box data structure and extra code in the other
         message box functions, so they can be disabled if the
         application does not require them. If this option is enabled
         then the system will always provide the blocking put()
         function, and it will also provide the timed put() function
         if thread timers are enabled."
     doc ref/ecos-ref/message-boxes.html
 }

 cdl_option CYGNUM_KERNEL_SYNCH_MBOX_QUEUE_SIZE {
     display            "Message box queue size"
     parent             CYGPKG_KERNEL_SYNCH
     type               count
     legal_values       1 to 65535
     description "
         This configuration option controls the number of messages
         that can be queued in a message box before a non-blocking
         put() operation will fail or a blocking put() operation will
         block. The cost in memory is one pointer per message box for
         each possible message."
 }

 cdl_option CYGMFN_KERNEL_SYNCH_CONDVAR_TIMED_WAIT {
     display            "Condition variable timed-wait support"
     parent             CYGPKG_KERNEL_SYNCH
     requires           CYGVAR_KERNEL_COUNTERS_CLOCK
     description "
         This option enables the condition variable timed wait
         facility."
     doc ref/ecos-ref/synchronization.html#CONDITION-VARIABLES
 }

 }}CFG_DATA
 
 */

#define CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE_SIMPLE
#define CYGMFN_KERNEL_SYNCH_MBOXT_PUT_CAN_WAIT
#define CYGNUM_KERNEL_SYNCH_MBOX_QUEUE_SIZE     10
#define CYGMFN_KERNEL_SYNCH_CONDVAR_TIMED_WAIT

/* ---------------------------------------------------------------------
 {{CFG_DATA

 cdl_component CYGPKG_KERNEL_INSTRUMENT {
     display "Kernel instrumentation"
     parent  CYGPKG_KERNEL
     description "
         The current release of the kernel contains an initial version
         of instrumentation support. The various parts of the kernel
         will invoke instrumentation routines whenever appropriate
         events occur, and these will be stored in a circular buffer
         for later reference."
 }


 cdl_option CYGNUM_KERNEL_INSTRUMENT_BUFFER_SIZE {
     display            "Size of instrumentation buffer size"
     parent             CYGPKG_KERNEL_INSTRUMENT
     type               count
     legal_values       16 to 0x100000
     description "
         If kernel instrumentation is enabled then the instrumentation
         data goes into a circular buffer. A larger buffer allows
         more data to be stored, but at a significant cost in memory.
         The value of this option corresponds to the number of entries
         in the table, and typically each entry will require 16 bytes
         of memory."
 }

 cdl_option CYGDBG_KERNEL_INSTRUMENT_FLAGS {
     display            "Perform selective instrumentation"
     parent             CYGPKG_KERNEL_INSTRUMENT
     description "
         The kernel can either collect all instrumentation events, or
         it can filter out events at runtime based on a set of flags.
         For example it would be possible to decide at runtime that
         only scheduler and interrupt instrumentation flags are of
         interest and that all other flags should be ignored. This
         flag mechanism involves extra code and processor cycle
         overhead in the instrumentation code, so it can be disabled
         if the application developer is interested in all
         instrumentation events."
 }

 # NOTE: many of these options should only be active if the appropriate
 # component is active.
 cdl_option CYGDBG_KERNEL_INSTRUMENT_SCHED {
     display    "Instrument the scheduler"
     parent     CYGPKG_KERNEL_INSTRUMENT
     description "
         It is possible to perform selective instrumentation at
         run-time. It is also possible to disable instrumentation
         in various kernel components at compile-time, thus
         reducing the code size overheads. This option controls
         whether or not instrumentation support is compiled into
         the scheduling code."
 }

 cdl_option CYGDBG_KERNEL_INSTRUMENT_THREAD {
     display    "Instrument thread operations"
     parent     CYGPKG_KERNEL_INSTRUMENT
     description "
         It is possible to perform selective instrumentation at
         run-time. It is also possible to disable instrumentation
         in various kernel components at compile-time, thus
         reducing the code size overheads. This option controls
         whether or not instrumentation support is compiled into
         the code that manipulates threads."
 }

 cdl_option CYGDBG_KERNEL_INSTRUMENT_INTR {
     display    "Instrument interrupts"
     parent     CYGPKG_KERNEL_INSTRUMENT
     description "
         It is possible to perform selective instrumentation at
         run-time. It is also possible to disable instrumentation
         in various kernel components at compile-time, thus
         reducing the code size overheads. This option controls
         whether or not instrumentation support is compiled into
         the interrupt handling code."
 }

 cdl_option CYGDBG_KERNEL_INSTRUMENT_MUTEX {
     display    "Instrument mutex operations"
     parent     CYGPKG_KERNEL_INSTRUMENT
     description "
         It is possible to perform selective instrumentation at
         run-time. It is also possible to disable instrumentation
         in various kernel components at compile-time, thus
         reducing the code size overheads. This option controls
         whether or not instrumentation support is compiled into
         the mutex code."
 }

 cdl_option CYGDBG_KERNEL_INSTRUMENT_CONDVAR {
     display    "Instrument condition variable operations"
     parent     CYGPKG_KERNEL_INSTRUMENT
     description "
         It is possible to perform selective instrumentation at
         run-time. It is also possible to disable instrumentation
         in various kernel components at compile-time, thus
         reducing the code size overheads. This option controls
         whether or not instrumentation support is compiled into
         the condition variable code."
 }

 cdl_option CYGDBG_KERNEL_INSTRUMENT_BINSEM {
     display    "Instrument binary semaphore operations"
     parent     CYGPKG_KERNEL_INSTRUMENT
     description "
         It is possible to perform selective instrumentation at
         run-time. It is also possible to disable instrumentation
         in various kernel components at compile-time, thus
         reducing the code size overheads. This option controls
         whether or not instrumentation support is compiled into
         the binary semaphore code."
 }

 cdl_option CYGDBG_KERNEL_INSTRUMENT_CNTSEM {
     display    "Instrument counting semaphore operations"
     parent     CYGPKG_KERNEL_INSTRUMENT
     description "
         It is possible to perform selective instrumentation at
         run-time. It is also possible to disable instrumentation
         in various kernel components at compile-time, thus
         reducing the code size overheads. This option controls
         whether or not instrumentation support is compiled into
         the counting semaphore code."
 }

 cdl_option CYGDBG_KERNEL_INSTRUMENT_MBOXT {
     display    "Instrument message box operations"
     parent     CYGPKG_KERNEL_INSTRUMENT
     description "
         It is possible to perform selective instrumentation at
         run-time. It is also possible to disable instrumentation
         in various kernel components at compile-time, thus
         reducing the code size overheads. This option controls
         whether or not instrumentation support is compiled into
         the message box code."
 }

 cdl_option CYGDBG_KERNEL_INSTRUMENT_CLOCK {
     display    "Instrument clock operations"
     parent     CYGPKG_KERNEL_INSTRUMENT
     description "
         It is possible to perform selective instrumentation at
         run-time. It is also possible to disable instrumentation
         in various kernel components at compile-time, thus
         reducing the code size overheads. This option controls
         whether or not instrumentation support is compiled into
         the real-time clock code."
 }

 cdl_option CYGDBG_KERNEL_INSTRUMENT_ALARM {
     display    "Instrument alarm-related operations"
     parent     CYGPKG_KERNEL_INSTRUMENT
     description "
         It is possible to perform selective instrumentation at
         run-time. It is also possible to disable instrumentation
         in various kernel components at compile-time, thus
         reducing the code size overheads. This option controls
         whether or not instrumentation support is compiled into
         the code related to alarm operations."
 }

 cdl_option CYGDBG_KERNEL_INSTRUMENT_USER {
     display    "Support application-level instrumentation"
     parent     CYGPKG_KERNEL_INSTRUMENT
     description "
         It is possible to perform selective instrumentation at
         run-time. It is also possible to disable instrumentation
         in various kernel components at compile-time, thus
         reducing the code size overheads. This option controls
         whether or not application-level instrumentation gets
         compiled in."
 }
 
 }}CFG_DATA
*/

#undef  CYGPKG_KERNEL_INSTRUMENT
#undef  CYGVAR_KERNEL_INSTRUMENT_EXTERNAL_BUFFER
#define CYGNUM_KERNEL_INSTRUMENT_BUFFER_SIZE           256
#define CYGDBG_KERNEL_INSTRUMENT_FLAGS
#define CYGDBG_KERNEL_INSTRUMENT_SCHED
#define CYGDBG_KERNEL_INSTRUMENT_THREAD
#define CYGDBG_KERNEL_INSTRUMENT_INTR
#define CYGDBG_KERNEL_INSTRUMENT_MUTEX
#define CYGDBG_KERNEL_INSTRUMENT_CONDVAR
#define CYGDBG_KERNEL_INSTRUMENT_BINSEM
#define CYGDBG_KERNEL_INSTRUMENT_CNTSEM
#define CYGDBG_KERNEL_INSTRUMENT_CLOCK
#define CYGDBG_KERNEL_INSTRUMENT_ALARM
#define CYGDBG_KERNEL_INSTRUMENT_MBOXT
#define CYGDBG_KERNEL_INSTRUMENT_USER

/* ---------------------------------------------------------------------
 * There appears to be somewhat of a lack of configuration options here.
 {{CFG_DATA

 cdl_component CYGPKG_KERNEL_MEMORY {
     display    "Memory allocators"
     type       dummy
     parent     CYGPKG_KERNEL
     description "
         Configuration options related to the kernel memory allocation
         support."
 }

 cdl_option CYGSEM_KERNEL_MEMORY_COALESCE {
     display    "Coalesce memory in the variable-block allocator"
     parent     CYGPKG_KERNEL_MEMORY
     description "
         The variable-block memory allocator can perform coalescing
         of memory whenever the application code releases memory back
         to the pool. This coalescing reduces the possibility of
         memory fragmentation problems, but involves extra code and
         processor cycles."
 }

 }}CFG_DATA */
 
#define CYGSEM_KERNEL_MEMORY_COALESCE

/* ---------------------------------------------------------------------
 * Options related to source-level debugging and diagnostics.

 {{CFG_DATA

 cdl_component CYGPKG_KERNEL_DEBUG {
     display    "Source-level debugging support"
     type       dummy
     parent     CYGPKG_KERNEL
     description "
         If the source level debugger gdb is to be used for debugging
         application code then it may be necessary to configure in support
         for this in the kernel."
 }

 # NOTE: does this require any other support ?
 cdl_option CYGDBG_KERNEL_DEBUG_GDB_THREAD_SUPPORT {
     display    "Include GDB multi-threading debug support"
     parent     CYGPKG_KERNEL_DEBUG
     requires   CYGVAR_KERNEL_THREADS_LIST
     requires   CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
     description "
         This option enables some extra kernel code which is needed
         to support multi-threaded source level debugging."
 }

 }}CFG_DATA */

#define CYGDBG_KERNEL_DEBUG_GDB_THREAD_SUPPORT

/* ---------------------------------------------------------------------
 * Kernel API's. The C++ one is the default. A C API is optional.
 * Support for other languages is possible.

 {{CFG_DATA

 cdl_component CYGPKG_KERNEL_API {
     display    "Kernel APIs"
     type       dummy
     parent     CYGPKG_KERNEL
     description "
         The eCos kernel is implemented in C++, so a C++ interface
         to the kernel is always available. There is also an optional
         C API. Additional API's may be provided in future versions."
	 doc ref/ecos-ref/kernel-apis.html
 }

 cdl_option     CYGFUN_KERNEL_API_C {
     display    "Provide C API"
     parent     CYGPKG_KERNEL_API
     description "
         The eCos kernel is implemented in C++, but there is an
         optional C API for use by application code. This C API can be
         disabled if the application code does not invoke the kernel
         directly, but instead uses higher level code such as the
         uITRON compatibility layer."
 }

 }}CFG_DATA */

#define CYGFUN_KERNEL_API_C
 
/* ---------------------------------------------------------------------
 * The rest of this header file contains various fix-ups and
 * options which cannot yet be controlled via the GUI tool.
 *
 * First some miscellaneous scheduler support, all calculated.
 */

#if defined(CYGSEM_KERNEL_SCHED_TIMESLICE) && defined(CYGSEM_KERNEL_SCHED_BITMAP)
# error Timeslicing cannot be enabled if the bitmap scheduler is used.
#endif

/*
 * These #define's are for the system's internal use only and should
 * not be edited by users. */
#ifdef CYGSEM_KERNEL_SCHED_BITMAP
# define CYGPRI_KERNEL_SCHED_IMPL_HXX           <cyg/kernel/bitmap.hxx>
#elif defined(CYGSEM_KERNEL_SCHED_MLQUEUE)
# define CYGPRI_KERNEL_SCHED_IMPL_HXX           <cyg/kernel/mlqueue.hxx>
#elif defined(CYGSEM_KERNEL_SCHED_LOTTERY)
# define CYGPRI_KERNEL_SCHED_IMPL_HXX           <cyg/kernel/lottery.hxx>
#else
#error No Scheduler defined
#endif

#define CYGNUM_KERNEL_SCHED_BITMAP_SIZE         CYGNUM_KERNEL_SCHED_PRIORITIES

/* ---------------------------------------------------------------------
 * Counter and clock related miscellania.
 *
 * If a real-time clock is enabled then there are additional
 * configuration options controlling the resolution of the clock,
 * CYGNUM_KERNEL_COUNTERS_RTC_RESOLUTION and
 * CYGNUM_KERNEL_COUNTERS_RTC_PERIOD. Different targets require
 * different default values, and the current configuration tool cannot
 * deal with this yet. Also the exact details of the resolution and
 * period values are still subject to change.
 *
 * To allow the clock settings to be controlled in some fashion,
 * there is a configuration option to override the default settings.
 * If this option is enabled then the resolution and period
 * #define's will come from configuration options (which do not
 * necessarily have sensible defaults, and which are not validated).
 * Otherwise the platform-specific numbers below are used. On
 * hardware this result in 100 clock interrupts every second.
 * For simulators slightly different values are used, so that the
 * various test cases run faster.
 */

#ifdef CYGPKG_KERNEL_COUNTERS_CLOCK_OVERRIDE

# define CYGNUM_KERNEL_COUNTERS_RTC_RESOLUTION \
        { CYGNUM_KERNEL_COUNTERS_CLOCK_OVERRIDE_NUMERATOR, \
          CYGNUM_KERNEL_COUNTERS_CLOCK_OVERRIDE_DENOMINATOR }

# define CYGNUM_KERNEL_COUNTERS_RTC_PERIOD CYGNUM_KERNEL_COUNTERS_CLOCK_OVERRIDE_PERIOD

#else /* CYGPKG_KERNEL_COUNTERS_CLOCK_OVERRIDE */

#if defined(CYG_HAL_MN10300_STDEVAL1)

#define CYGNUM_KERNEL_COUNTERS_RTC_RESOLUTION   {1000000000, 100}
#define CYGNUM_KERNEL_COUNTERS_RTC_PERIOD       150000

#endif
#if defined(CYG_HAL_MN10300_SIM)

#define CYGNUM_KERNEL_COUNTERS_RTC_RESOLUTION   {1000000000, 100}
#define CYGNUM_KERNEL_COUNTERS_RTC_PERIOD       9999

#endif

#if defined(CYG_HAL_MIPS_JMR3904)

#define CYGNUM_KERNEL_COUNTERS_RTC_RESOLUTION   {1000000000, 100}
#define CYGNUM_KERNEL_COUNTERS_RTC_PERIOD       15360

#endif

#if defined(CYG_HAL_MIPS_SIM)

#define CYGNUM_KERNEL_COUNTERS_RTC_RESOLUTION   {1000000000, 100}
#define CYGNUM_KERNEL_COUNTERS_RTC_PERIOD       999

#endif

#if defined(CYG_HAL_POWERPC_MP860)

#define CYGNUM_KERNEL_COUNTERS_RTC_RESOLUTION   {1000000000, 100}
#define CYGNUM_KERNEL_COUNTERS_RTC_PERIOD       20625

#endif


#ifndef CYGNUM_KERNEL_COUNTERS_RTC_RESOLUTION

#define CYGNUM_KERNEL_COUNTERS_RTC_RESOLUTION   {1000000000, 100}
#define CYGNUM_KERNEL_COUNTERS_RTC_PERIOD       9999

#endif

#endif /* CYGPKG_KERNEL_COUNTERS_CLOCK_OVERRIDE */

/* ---------------------------------------------------------------------
 * Thread-related miscellania.
 * 
 * These options provide for an assertion that the count value for counted
 * thread wakeups or for thread suspends do not exceed set limits.  This is
 * to help with debugging, to allow a runaaway loop, for example, to be
 * detected more easily.
 * 
 * If the option is not defined, no assert is included.  Whether asserts
 * are themselves included depends on infrastructure configury in infra.h
 *
 * It is hoped that these defaults are helpful rather than interfering.
 */
#define CYGNUM_KERNEL_MAX_SUSPEND_COUNT_ASSERT 		(500)
#define CYGNUM_KERNEL_MAX_COUNTED_WAKE_COUNT_ASSERT 	(500)

/*
 * If the scheduler configuration only has a single priority level,
 * then the idle thread must yield each time around its loop.
 */

#if CYGNUM_KERNEL_SCHED_PRIORITIES == 1
#  define CYGIMP_IDLE_THREAD_YIELD
#endif

/*
 * Per thread data options. Per thread data suuport is based loosely
 * on that define by POSIX. Each thread has an array of slots, up to
 * CYGNUM_KERNEL_THREADS_DATA_MAX, that may contain data. Some of the
 * slots have been preallocated to specific packages. Others may be
 * allocated dynamically.
 */

#define CYGNUM_KERNEL_THREADS_DATA_KERNEL       0
#define CYGNUM_KERNEL_THREADS_DATA_ITRON        1
#define CYGNUM_KERNEL_THREADS_DATA_LIBC         2
#define CYGNUM_KERNEL_THREADS_DATA_POSIX        3

#define CYGNUM_KERNEL_THREADS_DATA_ALL          0xF

/*
 * Some consistency checks.
 */
#if defined(CYGFUN_KERNEL_THREADS_TIMER) && !defined(CYGVAR_KERNEL_COUNTERS_CLOCK)
# error "Cannot have Thread Timers without Real Time Clock"
#endif

#if defined(CYGDBG_KERNEL_DEBUG_GDB_THREAD_SUPPORT) && !defined(CYGVAR_KERNEL_THREADS_LIST)
# error "GDB thread support requires a list of all known threads."
#endif

/* ---------------------------------------------------------------------
 * Synchronization primitives miscellania.
 *
 * These lines should not be edited by users.
 */
#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE_SIMPLE
# define CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE
#endif
#if defined(CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE) && !defined(CYGSEM_KERNEL_SCHED_MLQUEUE)
# error "Priority inheritance can only be enabled if the mlqueue scheduler is used."
#endif

/* ---------------------------------------------------------------------
 * This code allows the kernel build to be configured as a GDB
 * stub ROM monitor. It is only intended for internal use.
 */

/*
 * BLV: This code must be kept commented out for now, as it can
 * confuse the configuration tool. The correct solution is to have a
 * standard configuration for the system as represented by e.g. a
 * .ptest file which does the right thing. A ROM monitor is an
 * application, and application-specific configuration stuff should
 * not live in the master header file.
 */

/* 
#undef CYG_HAL_ROM_MONITOR

#ifdef CYG_HAL_ROM_MONITOR

#undef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
#undef CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT
#undef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
#undef CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING
#undef CYGIMP_KERNEL_INTERRUPTS_CHAIN
#undef CYGVAR_KERNEL_COUNTERS_CLOCK
#undef CYGFUN_KERNEL_THREADS_TIMER
#undef CYGPKG_KERNEL_EXCEPTIONS
#undef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE_SIMPLE
#undef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE
#undef CYGPKG_KERNEL_INSTRUMENT
#undef CYGDBG_INFRA_DIAG_USE_DEVICE     // now lives in infra

#define CYGDBG_KERNEL_DEBUG_GDB_INCLUDE_STUBS

#endif
*/

/* -------------------------------------------------------------------*/

#endif  /* ifdef CYGPKG_KERNEL */

/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_KERNEL_H */
/* EOF kernel.h */
