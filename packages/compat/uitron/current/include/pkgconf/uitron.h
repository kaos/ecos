#ifndef CYGONCE_PKGCONF_UITRON_H
#define CYGONCE_PKGCONF_UITRON_H
//===========================================================================
//
//      pkgconf/uitron.h
//
//      uITRON configuration file
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   hmt
// Contributors:        hmt
// Date:        1998-03-16
// Purpose:     uITRON configuration file
// Description: 
//
//####DESCRIPTIONEND####
//
//===========================================================================

// The layout of the uITRON Compatibility layer source code is documented in
//      .../<REPOSITORY>/compat/uitron/<VERSION>/include/uit_func.h


// First get external global configuration in force:
#include <pkgconf/kernel.h>

// The basic question: is the uITRON package provided at all?

#ifdef CYGPKG_UITRON

/* ---------------------------------------------------------------------
 * Define an overall package for the UITRON Compatibility layer. Then
 * create additional packages for the various different UITRON's currently
 * available.
 * 
 * Thought: Can we get a real 'mu' out of this in uITRON throughout?
 * It's \265 in C strings, in iso_8859_1
 *
 *
 * {{CFG_DATA
   cdl_package CYGPKG_UITRON {
       display "eCos uITRON compatibility layer"
       type    boolean
       requires CYGPKG_KERNEL
       description "eCos supports a uITRON Compatibility Layer, providing
                full Level S (Standard) compliance with Version 3.02 of
                the uITRON Standard, plus many Level E (Extended) features.
                uITRON is the premier Japanese embedded RTOS standard."
       doc     ref/ecos-ref/micro-itron-api.html
   }

   }}CFG_DATA */

// ------------------------------------------------------------------------
//                       Conformance check
// ------------------------------------------------------------------------
/* {{CFG_DATA
   cdl_option CYGIMP_UITRON_STRICT_CONFORMANCE {
       display "Check strict uITRON standards conformance"
       parent           CYGPKG_UITRON
       type             radio
       description "Require the rest of the system configuration
                to match the needs of strict uITRON standards conformance.
                This option can only be set if the rest of the system is
                configured correctly for uITRON semantics, for example
                there must be a realtime clock, a suitable scheduler, and no
                timeslicing.
                Of course a system without this selected can be completely
                conformant; this is here to help you check."
       doc      ref/ecos-ref/micro-itron-api.html
       requires  CYGVAR_KERNEL_COUNTERS_CLOCK
       requires  CYGSEM_KERNEL_SCHED_MLQUEUE
       requires !CYGSEM_KERNEL_SCHED_TIMESLICE
       requires  CYGFUN_KERNEL_THREADS_TIMER
   }
   cdl_option CYGIMP_UITRON_LOOSE_CONFORMANCE {
       display "System configuration overrides uITRON"
       parent           CYGPKG_UITRON
       type             radio
       description "Do not require the rest of the system configuration
                to match the needs of strict uITRON standards conformance.
                For example a bitmap scheduler, or timeslicing, can be used
                with the uITRON functions, but such an environment is not
                strictly conformant with the uITRON specification.
                Of course a system with this selected can be completely
                conformant; but it is up to you to configure it correctly."
       doc      ref/ecos-ref/micro-itron-api.html
   }
   }}CFG_DATA */
#undef  CYGIMP_UITRON_STRICT_CONFORMANCE
#define CYGIMP_UITRON_LOOSE_CONFORMANCE               

// ------------------------------------------------------------------------
//                       uITRON FUNCTION CALLS
// ------------------------------------------------------------------------

// If compiling with a C++ compiler, uITRON functions can be inline:
// define this to make it so.

/* {{CFG_DATA
   cdl_option CYGIMP_UITRON_INLINE_FUNCS {
       display          "Inline functions"
       parent           CYGPKG_UITRON
       type             boolean
       description "If compiling your application with a C++ compiler,
                uITRON functions can be inline: set this to make it so.
                Inlining functions often increases execution speed,
                though possibly at the cost of a larger executable,
                depending on what functions are used.
                Do NOT set this if compiling your application
                in plain C."
       doc      ref/ecos-ref/micro-itron-api.html
   }
   }}CFG_DATA */
#undef CYGIMP_UITRON_INLINE_FUNCS

// If not using inline functions, then uITRON functions are out-of-line; by
// default they have plain 'C' linkage ie. their names are extern 'C' in
// the C++ world and unadorned in the C world.  To give them C++ names (NOT
// useable by plain 'C' programs), define this:

/* {{CFG_DATA
   cdl_option CYGIMP_UITRON_CPP_OUTLINE_FUNCS {
       display          "C++ function names"
       parent           CYGPKG_UITRON
       type             boolean
       description "If compiling your application with a C++ compiler,
                uITRON functions can be given C++ style mangled names:
                set this to make it so.
                This option may make debugging your program easier,
                depending on your development environment.
                Do NOT set this if compiling your application
                in plain C."
       doc      ref/ecos-ref/micro-itron-api.html
   }
   }}CFG_DATA */
#undef CYGIMP_UITRON_CPP_OUTLINE_FUNCS

// When an application is fully debugged it might be useful to remove the
// code which checks parameters in the uITRON function calls.  To use
// asserts [CYG_ASSERT()] instead - which are compiled to nothing in
// release flavored builds - define this:

/* {{CFG_DATA
   cdl_option CYGSEM_UITRON_BAD_PARAMS_RETURN_ERRORS {
       display          "Return error codes for bad params"
       parent           CYGPKG_UITRON
       type             boolean
       description "When an application is fully debugged there is no need
                to check for bad parameters on every system call, for those
                parameters which are typically pointers or constants.
                Removing the checking code saves space
                and improves performance: set this to make it so.
                When this option is set, the correctness of parameters
                is asserted using CYG_ASSERT() which compiles to
                nothing in a non-debug configuration."
       doc      ref/ecos-ref/micro-itron-api.html
   }
   }}CFG_DATA */
#define CYGSEM_UITRON_BAD_PARAMS_RETURN_ERRORS

// By default both C's NULL and uITRON's NADR are treated as bad addresses;
// but some controllers might have memory at address zero, and want to use
// it.  In that case, prevent NULL being bounced by enabling this:

/* {{CFG_DATA
   cdl_option CYGSEM_UITRON_PARAMS_NULL_IS_GOOD_PTR {
       display          "NULL is a good pointer"
       parent           CYGPKG_UITRON
       type             boolean
       description "uITRON deliberately defines the constant NADR (-1) for
                use as an invalid memory address.
                The value -1 is chosen to allow working in microcontrollers
                which have real memory at address zero, the traditional 'C'
                NULL pointer.
                By default, uITRON functions check for both NULL and NADR as
                bad addresses: set this option to prevent checking for
                NULL and allow pointers to address zero to be used."
       doc      ref/ecos-ref/micro-itron-api.html
   }
   }}CFG_DATA */
#undef CYGSEM_UITRON_PARAMS_NULL_IS_GOOD_PTR


// ------------------------------------------------------------------------
//                      uITRON KERNEL OBJECTS
// ------------------------------------------------------------------------
// First, simple synchronisation objects without complex initialization:
// semaphores, message or mail boxes and flags.  Declare how many of each
// will exist in the system.  They will be statically created with default
// initialization, ready for use.  Their IDs in the uITRON sense are from 1
// to the value of the appropriate symbol below:

/* {{CFG_DATA
   cdl_component CYGPKG_UITRON_SEMAS {
       display "Semaphores"
       type    boolean
       parent  CYGPKG_UITRON
       description "uITRON Semaphore objects are used with functions
                named xxx_sem(); they support traditional semaphore
                semantics."
   }
   cdl_option CYGNUM_UITRON_SEMAS {
       display          "Number of semaphores"
       parent           CYGPKG_UITRON_SEMAS
       type             count
       legal_values     1 to 65535
       description "The number of uITRON semaphores
                present in the system.
                Valid semaphore object IDs will range
                from 1 to this value."
   }
   cdl_component CYGPKG_UITRON_SEMAS_CREATE_DELETE {
       display "Support create and delete"
       parent  CYGPKG_UITRON_SEMAS
       type    boolean
       description "Support
                semaphore
                create and delete operations
                (cre_sem, del_sem).
                Otherwise all semaphores are created,
                up to the number specified above."
   }
   cdl_option CYGNUM_UITRON_SEMAS_INITIALLY {
       display          "Number of semaphores created initially"
       parent           CYGPKG_UITRON_SEMAS_CREATE_DELETE
       type             count
       legal_values     0 to 65535
       description "The number of uITRON semaphores initially created.
                This number should not be more than the number
                of semaphores in the system, though setting it to a large
                value to mean 'all' is acceptable.
                Initially, only semaphores numbered 1 to this number exist;
                higher numbered ones must be created before use.
                It is only useful to initialize semaphores up to this number;
                higher numbered ones must be created in order to use them,
                and so they will be re-initialized."

   }
   cdl_component CYGPKG_UITRON_SEMAS_ARE_INITIALIZED {
       display "Initialize semaphore counts"
       parent  CYGPKG_UITRON_SEMAS
       type    boolean
       description "Initialize semaphores to specific count values.
                Otherwise semaphores are initialized with the count
                set to zero."
   }
   cdl_option CYGDAT_UITRON_SEMA_INITIALIZERS {
       display          "Static initializers"
       parent           CYGPKG_UITRON_SEMAS_ARE_INITIALIZED
       type             multiline
       description "A list of initializers separated by commas,
                one per line.
                An initializer is 'CYG_UIT_SEMA(INITIAL-COUNT)'
                or 'CYG_UIT_SEMA_NOEXS' for slots above the number
                initially to be created, when create and delete
                operations are supported.
                Note: this option is invoked in the context of a
                C++ array initializer, between curly brackets.
                Ensure that the number of initializers here exactly
                matches the total number of semaphores specified."
   }
   }}CFG_DATA */

// Counting Semaphores: xxx_sem() ops in uITRON.
#define CYGPKG_UITRON_SEMAS
#define CYGNUM_UITRON_SEMAS   3
#undef  CYGPKG_UITRON_SEMAS_ARE_INITIALIZED

// Optionally initialize in terms of CYG_UIT_SEMA( initial_count ):
#ifdef CYGPKG_UITRON_SEMAS
#ifdef CYGPKG_UITRON_SEMAS_ARE_INITIALIZED
#if 0 < CYGNUM_UITRON_SEMAS
#define CYGDAT_UITRON_SEMA_INITIALIZERS \
        CYG_UIT_SEMA(  0 ),     \
        CYG_UIT_SEMA(  0 ),     \
        CYG_UIT_SEMA(  0 )
#endif
#endif // CYGPKG_UITRON_SEMAS_ARE_INITIALIZED

#define CYGPKG_UITRON_SEMAS_CREATE_DELETE

#ifdef  CYGPKG_UITRON_SEMAS_CREATE_DELETE
#define CYGNUM_UITRON_SEMAS_INITIALLY 3
#endif // CYGPKG_UITRON_SEMAS_CREATE_DELETE

#endif // CYGPKG_UITRON_SEMAS

/* {{CFG_DATA
   cdl_component CYGPKG_UITRON_MBOXES {
       display "Mailboxes"
       type    boolean
       parent  CYGPKG_UITRON
       description "uITRON Mailbox objects are used with functions
                named xxx_msg() and xxx_mbx(); they support
                passing addresses (of 'messages') between tasks
                in a safe manner."
   }
   cdl_option CYGNUM_UITRON_MBOXES {
       display          "Number of mailboxes"
       parent           CYGPKG_UITRON_MBOXES
       type             count
       legal_values     1 to 65535
       description "The number of uITRON mailboxes
                present in the system.
                Valid mailbox object IDs will range
                from 1 to this value."
   }
   cdl_component CYGPKG_UITRON_MBOXES_CREATE_DELETE {
       display "Support create and delete"
       parent  CYGPKG_UITRON_MBOXES
       type    boolean
       description "Support
                mailbox
                create and delete operations
                (cre_mbx, del_mbx).
                Otherwise all mailboxes are created,
                up to the number specified above."
   }
   cdl_option CYGNUM_UITRON_MBOXES_INITIALLY {
       display          "Number of mailboxes created initially"
       parent           CYGPKG_UITRON_MBOXES_CREATE_DELETE
       type             count
       legal_values     0 to 65535
       description "The number of uITRON mailboxes initially created.
                This number should not be more than the number
                of mailboxes in the system, though setting it to a large
                value to mean 'all' is acceptable.
                Initially, only mailboxes numbered 1 to this number exist;
                higher numbered ones must be created before use."
   }
   }}CFG_DATA */

// Message Boxes: xxx_msg() ops in uITRON.
#define CYGPKG_UITRON_MBOXES
#define CYGNUM_UITRON_MBOXES  4

#ifdef CYGPKG_UITRON_MBOXES

#define CYGPKG_UITRON_MBOXES_CREATE_DELETE

#ifdef  CYGPKG_UITRON_MBOXES_CREATE_DELETE
#define CYGNUM_UITRON_MBOXES_INITIALLY 4
#endif // CYGPKG_UITRON_MBOXES_CREATE_DELETE
#endif // CYGPKG_UITRON_MBOXES

/* {{CFG_DATA
   cdl_component CYGPKG_UITRON_FLAGS {
       display "Eventflags"
       type    boolean
       parent  CYGPKG_UITRON
       description "uITRON Eventflag objects are used with functions
                named xxx_flg(); they support communication between
                tasks by means of setting and clearing bits in a word
                or flag value.
                Waiting for all or any of a set of bits is supported."
   }
   cdl_option CYGNUM_UITRON_FLAGS {
       display          "Number of eventflags"
       parent           CYGPKG_UITRON_FLAGS
       type             count
       legal_values     1 to 65535
       description "The number of uITRON eventflag
                objects present in the system.
                Valid eventflag object IDs will range
                from 1 to this value."
   }
   cdl_component CYGPKG_UITRON_FLAGS_CREATE_DELETE {
       display "Support create and delete"
       parent  CYGPKG_UITRON_FLAGS
       type    boolean
       description "Support
                eventflag
                create and delete operations
                (cre_flg, del_flg).
                Otherwise all eventflags are created,
                up to the number specified above."
   }
   cdl_option CYGNUM_UITRON_FLAGS_INITIALLY {
       display          "Number of eventflags created initially"
       parent           CYGPKG_UITRON_FLAGS_CREATE_DELETE
       type             count
       legal_values     0 to 65535
       description "The number of uITRON eventflags initially created.
                This number should not be more than the number
                of eventflags in the system, though setting it to a large
                value to mean 'all' is acceptable.
                Initially, only eventflags numbered 1 to this number exist;
                higher numbered ones must be created before use."
   }
   }}CFG_DATA */

// Flags: xxx_flg ops in uITRON.
#define CYGPKG_UITRON_FLAGS
#define CYGNUM_UITRON_FLAGS   5

#ifdef CYGPKG_UITRON_FLAGS

#define CYGPKG_UITRON_FLAGS_CREATE_DELETE

#ifdef  CYGPKG_UITRON_FLAGS_CREATE_DELETE
#define CYGNUM_UITRON_FLAGS_INITIALLY 5
#endif // CYGPKG_UITRON_FLAGS_CREATE_DELETE
#endif // CYGPKG_UITRON_FLAGS

// ------------------------------------------------------------------------
// Next, uITRON tasks.  These must be initialized, and the number of them
// must match the number of entries in the initializer definition.
//
// CYGNUM_UITRON_TASKS defines the number of tasks; they are numbered 1
// through CYGNUM_UITRON_TASKS.
//
// CYGDAT_UITRON_TASK_EXTERNS is used to declare external symbols which are
// used in the initializations, typically the addresses of the functions who
// implement each of the tasks.  Other objects might also be declared, for
// example stack data or scratch data which is used by the tasks.
//
// CYGDAT_UITRON_TASK_INITIALIZERS defines a list of CYG_UIT_TASK()
// initializers, separated by commas.  The number of these defines how many
// uITRON tasks exist in the system.
// 
// Both of these symbols must be defined.
//
// Task IDs in the uITRON sense are from 1 to the number of uITRON tasks
// which exist in the system, defined by CYGDAT_UITRON_TASK_INITIALIZERS.
//
// Tasks are dormant by default from the start; they must be resumed in
// order to run when the scheduler starts.  Code to do this is provided in
// the uITRON package; it is present if this symbol is defined, and can be
// called from plain C code:
//   CYGNUM_UITRON_START_TASKS > 0: start tasks 1..<value>
//   CYGNUM_UITRON_START_TASKS = 0: start all uITRON tasks
//                                ie. 1..CYGNUM_UITRON_TASKS
//
// Alternatively, an expert programmer could start the uITRON tasks
// individually using the C++ system calls on the static array of tasks
// declared in uit_objs.hxx

/* {{CFG_DATA
   cdl_component CYGPKG_UITRON_TASKS {
       display "Tasks"
       type    dummy
       parent  CYGPKG_UITRON
       description "uITRON Tasks are the basic blocks of multi-tasking
                in the uITRON world; they are threads or lightweight
                processes, sharing the address space and the CPU.
                They communicate using the primitives outlined above.
                Each has a stack, an entry point (a C or C++ function),
                and (where appropriate) a scheduling priority."
   }
   cdl_option CYGNUM_UITRON_TASKS {
       display          "Number of tasks"
       parent           CYGPKG_UITRON_TASKS
       type             count
       legal_values     1 to 65535
       description "The number of uITRON tasks
                present in the system.
                Valid task object IDs will range
                from 1 to this value."
   }
   cdl_option CYGNUM_UITRON_START_TASKS {
       display          "Start tasks"
       parent           CYGPKG_UITRON_TASKS
       type             count
       legal_values     0 to 65535
       description "The number of uITRON tasks
                to start automatically.
                Tasks from 1 to this value will be started
                at the beginning of application execution.
                A value of zero here means to start them all.
                Tasks started in this way have a start code of
                zero, as if they were started by sta_tsk(i,0).
                If create and delete operations are supported,
                this number should be no greater than the number
                of tasks created initially."
   }
   cdl_component CYGPKG_UITRON_TASKS_CREATE_DELETE {
       display "Support create and delete"
       parent  CYGPKG_UITRON_TASKS
       type    boolean
       description "Support
                task
                create and delete operations
                (cre_tsk, del_tsk).
                Otherwise all tasks are created,
                up to the number specified above."
   }
   cdl_option CYGNUM_UITRON_TASKS_INITIALLY {
       display          "Number of tasks created initially"
       parent           CYGPKG_UITRON_TASKS_CREATE_DELETE
       type             count
       legal_values     1 to 65535
       description "The number of uITRON tasks initially created.
                This number should not be more than the number
                of tasks in the system, though setting it to a large
                value to mean 'all' is acceptable.
                Initially, only tasks numbered 1 to this number exist;
                higher numbered ones must be created before use."
   }
   cdl_option CYGNUM_UITRON_STACK_SIZE {
       display          "Default stack size"
       parent           CYGPKG_UITRON_TASKS
       type             count
       legal_values     128 to 0x7FFFFFFF
       description "Define a default stack size for uITRON tasks,
                for use in the initialization options below.
                This will be overridden where it is used if the
                architectural HAL requires a minimum stack size
                to handle interrupts correctly."
   }
   cdl_option CYGDAT_UITRON_TASK_EXTERNS {
       display          "Externs for initialization"
       parent           CYGPKG_UITRON_TASKS
       type             multiline
       description "Task initializers may refer to external objects
                such as memory for stack or functions to call.
                Use this option to define or declare any external
                objects needed by the task static initializer below.
                Example: create some memory for a stack using
                 'static char stack1[CYGNUM_UITRON_STACK_SIZE];'
                to set up a chunk of memory of the default stack size.
                Note: this option is invoked in the 'outermost' context
                of C++ source, where global/static objects are created;
                it should contain valid, self-contained, C++ source."
   }
   cdl_option CYGDAT_UITRON_TASK_INITIALIZERS {
       display          "Static initializers"
       parent           CYGPKG_UITRON_TASKS
       type             multiline
       description "Tasks must be statically
                initialized: enter a list of initializers
                separated by commas, one per line.
                An initializer is
                'CYG_UIT_TASK(NAME,PRIO,FUNC,STACK,SIZE)'
                where name is a quoted string to name the task,
                prio is the initial priority of the task,
                func is the name of the entry point,
                stack is the address of the task's stack,
                and size is the size of the task's stack.
                When create and delete operations are supported,
                'CYG_UIT_TASK_NOEXS(NAME,STACK,SIZE)' should be
                used for tasks which are not initially created,
                in order to tell the system what memory to use
                for stacks when these tasks are created later on.
                Using 'CYGNUM_UITRON_STACK_SIZE' for size
                is recommended, to use the option defined above,
                so long as that truly is the size of your stack(s).
                Note: this option is invoked in the context of a
                C++ array initializer, between curly brackets.
                Ensure that the number of initializers here exactly
                matches the number of tasks specified."
   }
   }}CFG_DATA */


// This default configuration has 4 tasks called task1, task2, task3 and
// task4, and only task1 is started up by cyg_uitron_start().

// first specify how many tasks we have
// no PKG define, tasks are not optional
#define CYGNUM_UITRON_TASKS 4

#define CYGNUM_UITRON_START_TASKS 1 // start task1 only

#define CYGNUM_UITRON_STACK_SIZE 2048

#define CYGDAT_UITRON_TASK_EXTERNS \
extern "C" void task1( unsigned int ); \
extern "C" void task2( unsigned int ); \
extern "C" void task3( unsigned int ); \
extern "C" void task4( unsigned int ); \
static char stack1[ CYGNUM_UITRON_STACK_SIZE ], \
            stack2[ CYGNUM_UITRON_STACK_SIZE ], \
            stack3[ CYGNUM_UITRON_STACK_SIZE ], \
            stack4[ CYGNUM_UITRON_STACK_SIZE ];

// then initialize in terms of
//          CYG_UIT_TASK( "name", prio, func, stackaddress, stacksize )
// [or CYG_UIT_TASK_NOEXS( "name",            stackaddress, stacksize ) ]
// [for create/delete enabled, tasks which are not initially created.   ]
#define CYGDAT_UITRON_TASK_INITIALIZERS \
   CYG_UIT_TASK( "t1", 1, task1, &stack1, CYGNUM_UITRON_STACK_SIZE ), \
   CYG_UIT_TASK( "t2", 2, task2, &stack2, CYGNUM_UITRON_STACK_SIZE ), \
   CYG_UIT_TASK( "t3", 3, task3, &stack3, CYGNUM_UITRON_STACK_SIZE ), \
   CYG_UIT_TASK( "t4", 4, task4, &stack4, CYGNUM_UITRON_STACK_SIZE ), \

// Do we support create and delete?
#define CYGPKG_UITRON_TASKS_CREATE_DELETE

#ifdef  CYGPKG_UITRON_TASKS_CREATE_DELETE
#define CYGNUM_UITRON_TASKS_INITIALLY 4
#endif // CYGPKG_UITRON_TASKS_CREATE_DELETE

// ------------------------------------------------------------------------
// Memory Pools, both fixed and variable block:
//
// Any such object must be initialized, so the number of them defined must
// match the number of entries in the initializer much as for tasks.

// Fixed block allocation memory pools:

/* {{CFG_DATA
   cdl_component CYGPKG_UITRON_MEMPOOLFIXED {
       display "Fixed-size memorypools"
       type    boolean
       parent  CYGPKG_UITRON
       description "uITRON supports memory pools for dynamic, task-safe
                memory allocation.
                Two kinds are supported, fixed-size and variable-size.
                There may be multiple of each
                type of pool, each with differing characteristics.
                This option controls whether there are any fixed-size
                memorypools in the system.
                A fixed-size memorypool allocates blocks of memory of
                its preset fixed size and none other."
   }
   cdl_option CYGNUM_UITRON_MEMPOOLFIXED {
       display          "Number of fixed-size memorypools"
       parent           CYGPKG_UITRON_MEMPOOLFIXED
       type             count
       legal_values     1 to 65535
       description "The number of uITRON Fixed-Size
                Memorypools present in the system.
                Valid Fixed-Size Memorypool IDs will range
                from 1 to this value."
   }
   cdl_component CYGPKG_UITRON_MEMPOOLFIXED_CREATE_DELETE {
       display "Support create and delete"
       parent  CYGPKG_UITRON_MEMPOOLFIXED
       type    boolean
       description "Support
                fixed-size memory pool
                create and delete operations
                (cre_mpf, del_mpf).
                Otherwise all fixed mempools are created,
                up to the number specified above."
   }
   cdl_option CYGNUM_UITRON_MEMPOOLFIXED_INITIALLY {
       display          "Number of fixed mempools created initially"
       parent           CYGPKG_UITRON_MEMPOOLFIXED_CREATE_DELETE
       type             count
       legal_values     0 to 65535
       description "The number of fixed mempools initially created.
                This number should not be more than the number
                of fixed mempools in the system, though setting
                it to a large value to mean 'all' is acceptable.
                Initially, only fixed mempools numbered from
                1 to this number exist;
                higher numbered ones must be created before use.
                Whilst all mempools must be initialized to tell
                the system what memory to use for each pool,
                it is only useful to initialize the blocksize of
                fixed mempools up to this number;
                the blocksize for higher numbered ones
                will be defined when they are created."
   }
   cdl_option CYGDAT_UITRON_MEMPOOLFIXED_EXTERNS {
       display          "Externs for initialization"
       parent           CYGPKG_UITRON_MEMPOOLFIXED
       type             multiline
       description "Fixed mempool initializers may refer to external
                objects such as memory for the pool to manage.
                Use this option to define or declare any external
                objects needed by the pool's static initializer below.
                Example: create some memory for a mempool using
                 'static char fpool1[2000];'
                to set up a chunk of memory of 2000 bytes.
                Note: this option is invoked in the 'outermost' context
                of C++ source, where global/static objects are created;
                it should contain valid, self-contained, C++ source."
   }
   cdl_option CYGDAT_UITRON_MEMPOOLFIXED_INITIALIZERS {
       display          "Static initializers"
       parent           CYGPKG_UITRON_MEMPOOLFIXED
       type             multiline
       description "Fixed block memory pools should be statically
                initialized: enter a list of initializers
                separated by commas, one per line.
                An initializer is
                'CYG_UIT_MEMPOOLFIXED(ADDR,SIZE,BLOCK)'
                where addr is the address of memory to manage,
                size is the total size of that memory, and
                block is the block size for allocation by the pool.
                If create and delete operations are supported,
                initializers of the form
                'CYG_UIT_MEMPOOLFIXED_NOEXS(ADDR,SIZE)' should be
                used for pools which are not initially created, to tell
                the system what memory to use for each pool.
                Note: this option is invoked in the context of a
                C++ array initializer, between curly brackets.
                Ensure that the number of initializers here exactly
                matches the total number of fixed pools specified."
   }
   }}CFG_DATA */

#define CYGPKG_UITRON_MEMPOOLFIXED
#define CYGNUM_UITRON_MEMPOOLFIXED 3

#ifdef CYGPKG_UITRON_MEMPOOLFIXED
#if 0 < CYGNUM_UITRON_MEMPOOLFIXED

#define CYGPKG_UITRON_MEMPOOLFIXED_CREATE_DELETE

#ifdef  CYGPKG_UITRON_MEMPOOLFIXED_CREATE_DELETE
#define CYGNUM_UITRON_MEMPOOLFIXED_INITIALLY 3
#endif // CYGPKG_UITRON_MEMPOOLFIXED_CREATE_DELETE

#define CYGDAT_UITRON_MEMPOOLFIXED_EXTERNS \
static char fpool1[ 2000 ], \
            fpool2[ 2000 ], \
            fpool3[ 2000 ];

// in terms of CYG_UIT_MEMPOOLFIXED( addr, size, blocksize );
// [or CYG_UIT_MEMPOOLFIXED_NOEXS( addr, size ) if cre/del supported]
#define CYGDAT_UITRON_MEMPOOLFIXED_INITIALIZERS \
   CYG_UIT_MEMPOOLFIXED( fpool1, 2000,  20 ), \
   CYG_UIT_MEMPOOLFIXED( fpool2, 2000, 100 ), \
   CYG_UIT_MEMPOOLFIXED( fpool3, 2000, 500 ), \

#endif
#endif // do we have fixed memory pools?

// Variable block memory pools:

/* {{CFG_DATA
   cdl_component CYGPKG_UITRON_MEMPOOLVAR {
       display "Variable-size memorypools"
       type    boolean
       parent  CYGPKG_UITRON
       description "uITRON supports memory pools for dynamic, task-safe
                memory allocation.
                Two kinds are supported, fixed-size and variable-size.
                There may be multiple of each
                type of pool, each with differing characteristics.
                This option controls whether there are any variable-size
                memorypools in the system.
                A variable-size memorypool allocates blocks of memory of
                any size requested, resources permitting."
   }
   cdl_option CYGNUM_UITRON_MEMPOOLVAR {
       display          "Number of variable-size memory pools"
       parent           CYGPKG_UITRON_MEMPOOLVAR
       type             count
       legal_values     1 to 65535
       description "The number of uITRON Variable-Size
                Memorypools present in the system.
                Valid Variable-Size Memorypool IDs will range
                from 1 to this value."
   }
   cdl_component CYGPKG_UITRON_MEMPOOLVAR_CREATE_DELETE {
       display "Support create and delete"
       parent  CYGPKG_UITRON_MEMPOOLVAR
       type    boolean
       description "Support
                variable-size memory pool
                create and delete operations
                (cre_mpl, del_mpl).
                Otherwise all variable-size mempools are created,
                up to the number specified above."
   }
   cdl_option CYGNUM_UITRON_MEMPOOLVAR_INITIALLY {
       display          "Number of variable-size mempools created initially"
       parent           CYGPKG_UITRON_MEMPOOLVAR_CREATE_DELETE
       type             count
       legal_values     0 to 65535
       description "The number of variable-size mempools initially created.
                This number should not be more than the number
                of variable mempools in the system, though setting
                it to a large value to mean 'all' is acceptable.
                Initially, only variable mempools numbered from
                1 to this number exist;
                higher numbered ones must be created before use.
                All mempools must be initialized to tell
                the system what memory to use for each pool."
   }
   cdl_option CYGDAT_UITRON_MEMPOOLVAR_EXTERNS {
       display          "Externs for initialization"
       parent           CYGPKG_UITRON_MEMPOOLVAR
       type             multiline
       description "Variable mempool initializers may refer to external
                objects such as memory for the pool to manage.
                Use this option to define or declare any external
                objects needed by the pool's static initializer below.
                Example: create some memory for a mempool using
                 'static char vpool1[2000];'
                to set up a chunk of memory of 2000 bytes.
                Note: this option is invoked in the 'outermost' context
                of C++ source, where global/static objects are created;
                it should contain valid, self-contained, C++ source."
   }
   cdl_option CYGDAT_UITRON_MEMPOOLVAR_INITIALIZERS {
       display          "Static initializers"
       parent           CYGPKG_UITRON_MEMPOOLVAR
       type             multiline
       description "Variable block memory pools should be statically
                initialized: enter a list of initializers
                separated by commas, one per line.
                An initializer is
                'CYG_UIT_MEMPOOLVAR(ADDR,SIZE)'
                where addr is the address of memory to manage, and
                size is the total size of that memory.
                If create and delete operations are supported,
                initializers of the form
                'CYG_UIT_MEMPOOLVAR_NOEXS(ADDR,SIZE)' should be
                used for pools which are not initially created, to tell
                the system what memory to use for each pool.
                Note: this option is invoked in the context of a
                C++ array initializer, between curly brackets.
                Ensure that the number of initializers here exactly
                matches the total number of variable pools specified."
   }
   }}CFG_DATA */

#define CYGPKG_UITRON_MEMPOOLVAR
#define CYGNUM_UITRON_MEMPOOLVAR 3

#ifdef CYGPKG_UITRON_MEMPOOLVAR   
#if 0 < CYGNUM_UITRON_MEMPOOLVAR

#define CYGPKG_UITRON_MEMPOOLVAR_CREATE_DELETE

#ifdef  CYGPKG_UITRON_MEMPOOLVAR_CREATE_DELETE
#define CYGNUM_UITRON_MEMPOOLVAR_INITIALLY 3
#endif // CYGPKG_UITRON_MEMPOOLVAR_CREATE_DELETE

#define CYGDAT_UITRON_MEMPOOLVAR_EXTERNS \
static char vpool1[ 2000 ], \
            vpool2[ 2000 ], \
            vpool3[ 2000 ];

// in terms of CYG_UIT_MEMPOOLVAR( addr, size );
// [or CYG_UIT_MEMPOOLVAR_NOEXS( addr, size ) if cre/del supported]
#define CYGDAT_UITRON_MEMPOOLVAR_INITIALIZERS \
   CYG_UIT_MEMPOOLVAR( vpool1, 2000 ), \
   CYG_UIT_MEMPOOLVAR( vpool2, 2000 ), \
   CYG_UIT_MEMPOOLVAR( vpool3, 2000 ), \

#endif   
#endif // do we have variable memory pools?

// ------------------------------------------------------------------------
// One-shot Alarm and Cyclic Alarm handlers:
//
// These are declared without uninitialization; they must be defined in
// order to use them.


/* {{CFG_DATA
   cdl_component CYGPKG_UITRON_ALARMS {
       display "Alarm handlers"
       type    boolean
       parent  CYGPKG_UITRON
       requires CYGVAR_KERNEL_COUNTERS_CLOCK
       description "uITRON Alarm Handlers are used with functions
                named def_alm() and ref_alm(); they support
                simple timing, with a function callback
                at the end of the timed period."
   }
   cdl_option CYGNUM_UITRON_ALARMS {
       display          "Number of alarm handlers"
       parent           CYGPKG_UITRON_ALARMS
       type             count
       legal_values     1 to 65535
       description "The number of uITRON alarm
                handlers present in the system.
                Valid alarm handler numbers will range
                from 1 to this value."
   }
   }}CFG_DATA */
#define CYGPKG_UITRON_ALARMS
#define CYGNUM_UITRON_ALARMS   3
#if 0 // initializers not supported
#if 0 < CYGNUM_UITRON_ALARMS
//#define CYGDAT_UITRON_ALARM_EXTERNS
// in terms of ALARM( ... )
//#define CYGDAT_UITRON_ALARM_INITIALIZERS
#endif
#endif // NOT SUPPORTED 


/* {{CFG_DATA
   cdl_component CYGPKG_UITRON_CYCLICS {
       display "Cyclic handlers"
       type    boolean
       parent  CYGPKG_UITRON
       requires CYGVAR_KERNEL_COUNTERS_CLOCK
       description "uITRON Cyclic Handlers are used with functions
                named xxx_cyc(); they support timing
                with a periodic function callback that
                can be dynamically turned on or off, and
                resynchronized with external events."
   }
   cdl_option CYGNUM_UITRON_CYCLICS {
       display          "Number cyclic handlers"
       parent           CYGPKG_UITRON_CYCLICS
       type             count
       legal_values     1 to 65535
       description "The number of uITRON cyclics
                handlers present in the system.
                Valid cyclic handler numbers will range
                from 1 to this value."
   }
   }}CFG_DATA */
#define CYGPKG_UITRON_CYCLICS
#define CYGNUM_UITRON_CYCLICS  3
#if 0 // initializers not supported
#if 0 < CYGNUM_UITRON_CYCLICS
//#define CYGDAT_UITRON_CYCLIC_EXTERNS
// in terms of CYCLIC( ... )
//#define CYGDAT_UITRON_CYCLIC_INITIALIZERS
#endif
#endif // NOT SUPPORTED 

// ========================================================================
// ------------------------------------------------------------------------
//                      uITRON VERSION INFORMATION
// ------------------------------------------------------------------------

/* {{CFG_DATA
   cdl_component CYGPKG_UITRON_VERSION {
       display "Version information"
       type    dummy
       parent  CYGPKG_UITRON
       description "The get_ver() uITRON system call returns
                several version related values describing
                the vendor, product and CPU in question
                as well as the version of the uITRON
                standard supported.
                These values may be specified here."
   }
   cdl_option CYGNUM_UITRON_VER_MAKER {
       display          "OS maker"
       parent           CYGPKG_UITRON_VERSION
       type             count
       legal_values     0 to 0xFFFF
       description "This value is returned in the 'maker'
                field of the T_VER structure in
                response to a get_ver() system call."
   }
   cdl_option CYGNUM_UITRON_VER_ID     {
       display          "OS identification"
       parent           CYGPKG_UITRON_VERSION
       type             count
       legal_values     0 to 0xFFFF
       description "This value is returned in the 'id'
                field of the T_VER structure in
                response to a get_ver() system call."
   }
   cdl_option CYGNUM_UITRON_VER_SPVER  {
       display          "ITRON specification"
       parent           CYGPKG_UITRON_VERSION
       type             count
       legal_values     0 to 0xFFFF
       description "This value is returned in the 'spver'
                field of the T_VER structure in
                response to a get_ver() system call.
                Do NOT change this value."
   }
   cdl_option CYGNUM_UITRON_VER_PRVER  {
       display          "OS product version"
       parent           CYGPKG_UITRON_VERSION
       type             count
       legal_values     0 to 0xFFFF
       description "This value is returned in the 'prver'
                field of the T_VER structure in
                response to a get_ver() system call."
   }
   # PRNO fields in own folder
   cdl_component CYGPKG_UITRON_VERSION_PRNO {
       display "Product info"
       type    dummy
       parent  CYGPKG_UITRON_VERSION
       description "The get_ver() uITRON system call returns
                several version related values describing
                the vendor, product and CPU in question
                as well as the version of the uITRON
                standard supported.
                These values may be specified here."
   }
   cdl_option CYGNUM_UITRON_VER_PRNO_0 {
       display          "Field 0"
       parent           CYGPKG_UITRON_VERSION_PRNO
       type             count
       legal_values     0 to 0xFFFF
       description "This value is returned in the 'prno[0]'
                field of the T_VER structure in
                response to a get_ver() system call."
   }
   cdl_option CYGNUM_UITRON_VER_PRNO_1 {
       display          "Field 1"
       parent           CYGPKG_UITRON_VERSION_PRNO
       type             count
       legal_values     0 to 0xFFFF
       description "This value is returned in the 'prno[1]'
                field of the T_VER structure in
                response to a get_ver() system call."
   }
   cdl_option CYGNUM_UITRON_VER_PRNO_2 {
       display          "Field 2"
       parent           CYGPKG_UITRON_VERSION_PRNO
       type             count
       legal_values     0 to 0xFFFF
       description "This value is returned in the 'prno[2]'
                field of the T_VER structure in
                response to a get_ver() system call."
   }
   cdl_option CYGNUM_UITRON_VER_PRNO_3 {
       display          "Field 3"
       parent           CYGPKG_UITRON_VERSION_PRNO
       type             count
       legal_values     0 to 0xFFFF
       description "This value is returned in the 'prno[3]'
                field of the T_VER structure in
                response to a get_ver() system call."
   }
   # up a level
   cdl_option CYGNUM_UITRON_VER_CPU    {
       display          "CPU information"
       parent           CYGPKG_UITRON_VERSION
       type             count
       legal_values     0 to 0xFFFF
       description "This value is returned in the 'cpu'
                field of the T_VER structure in
                response to a get_ver() system call."
   }
   cdl_option CYGNUM_UITRON_VER_VAR    {
       display          "System variant"
       parent           CYGPKG_UITRON_VERSION
       type             count
       legal_values     0 to 0xFFFF
       description "This value is returned in the 'var'
                field of the T_VER structure in
                response to a get_ver() system call.
                Do NOT change this value."
   }
   }}CFG_DATA
 */

// these constants are returned by the get_ver API call.

#define CYGNUM_UITRON_VER_MAKER          0 /* vendor */
#define CYGNUM_UITRON_VER_ID             0 /* format number */
#define CYGNUM_UITRON_VER_SPVER   0x5302   /* specification version */
#define CYGNUM_UITRON_VER_PRVER   0x0100   /* product version */
#define CYGNUM_UITRON_VER_PRNO_0         0 /* product control information */
#define CYGNUM_UITRON_VER_PRNO_1         0
#define CYGNUM_UITRON_VER_PRNO_2         0
#define CYGNUM_UITRON_VER_PRNO_3         0
#define CYGNUM_UITRON_VER_CPU            0 /* CPU information */
#define CYGNUM_UITRON_VER_VAR     0x8000   /* variation descriptor */


#endif // CYGPKG_UITRON

#endif // CYGONCE_PKGCONF_UITRON_H
// EOF pgkconf/uitron.h
