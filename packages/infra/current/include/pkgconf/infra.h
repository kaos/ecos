#ifndef CYGONCE_PKGCONF_INFRA_H
#define CYGONCE_PKGCONF_INFRA_H
//=========================================================================
//                                                                         
//      pkgconf/infra.h                                                    
//                                                                         
//      Infrastructure configuration file.                                 
//                                                                         
//=========================================================================
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
//=========================================================================
//#####DESCRIPTIONBEGIN####                                                
//                                                                         
// Author(s):   bartv,hmt                                                  
// Contributors:        bartv,hmt                                                  
// Date:        1998-04-17                                                 
// Purpose:                                                                
// Description:                                                            
// Usage:                                                                  
//                                                                         
//####DESCRIPTIONEND####                                                   
//=========================================================================

/* The header pkgconf/system.h contains configuration options controlled by
 * the pkgconf tool. These options cannot be edited by hand, they can only
 * be changed by running pkgconf again.
 */
#include <pkgconf/system.h>

/* ==================================================================== 
 * Debug facilities. The eCos source code contains a significant amount
 * of internal debugging support, in the form of assertions and
 * tracing. Tracing takes the form of text messages that get output
 * via the diagnostics channel to the host whenever an event occurs,
 * or whenever a function is called or returns (the implementation
 * may change in future releases of the system). It is possible to
 * enable assertions and tracing independently. There are also
 * options controlling the exact behaviour of the assertion and
 * tracing facilities, thus giving users finer control over the
 * code and data size requirements.
 *
 * Thought: Can we get a real 'mu' out of this in uITRON throughout?
 * It's \265 in C strings, in iso_8859_1
 * It is mentioned in the startup enabling stuff.
 *
 */

/* {{CFG_DATA
   cdl_package CYGPKG_INFRA {
       display "Infrastructure"
       type    dummy
       description "Common types and useful macros.
                Tracing and assertion facilities.
                Package startup options." 
   }
   }}CFG_DATA */


/* ========================================================================
 * The common types part of the system is not yet configurable.
 */
/* {{not-CFG_DATA
   cdl_component CYGPKG_INFRA_TYPES {
       display          "Common Types"
       type             dummy
       parent           CYGPKG_INFRA
       description "These types are used throughout the eCos source code.
                Currently there are no configuration options associated
                with the Common Types component; they are automatically
                set up according to the selected HAL."
   }
   }}not-CFG_DATA */


/* =================================================================
 * This option decides whether the diag_printf() function uses
 * variadic argument support, or uses a K&R style declaration to
 * support it. It is possible that variadic arguments do not work,
 * or support for them is not available in a particular tool chain;
 * hence the option for using an alternative implementation.
 */

/* {{CFG_DATA
   cdl_option CYGDBG_INFRA_DIAG_PRINTF_USE_VARARG {
       display          "Use varargs in diag_printf()"
       type             boolean
       parent           CYGPKG_INFRA
       description "The diagnostic routine diag_printf() can be defined
                    using either variadic arguments or a K&R style definition.
                    The former will work in most circumstances, but if variable
                    argument handling is broken or missing then the K&R variant
                    may be used. This will also generate warnings about the
                    declaration of diag_printf() not being a prototype.
                    These may safely be ignored."
   }
   }}CFG_DATA */

#define CYGDBG_INFRA_DIAG_PRINTF_USE_VARARG

/* ========================================================================
 * There is a global option that affects whether the user is building
 * a release system or a debug system.  It enables all the stuff below.
 *
 * This used to be CYG_RELEASE_DEBUG.
 */
/* {{CFG_DATA
   cdl_component CYGPKG_INFRA_DEBUG {
       display 		"Asserts & Tracing"
       type    		boolean
       parent		CYGPKG_INFRA
       description "The eCos source code contains a significant amount of
		internal debugging support, in the form of assertions and
		tracing.
                Assertions check at runtime that various conditions are as
                expected; if not, execution is halted.
                Tracing takes the form of text messages that are output
                whenever certain events occur, or whenever functions are
                called or return.
                The most important property of these checks and messages is
                that they are not required for the program to run.
                It is prudent to develop software with assertions enabled,
                but disable them when making a product release, thus
                removing the overhead of that checking.
                It is possible to enable assertions and tracing
                independently.
                There are also options controlling the exact behaviour of
                the assertion and tracing facilities, thus giving users
                finer control over the code and data size requirements."
   }
   }}CFG_DATA */
#undef CYGPKG_INFRA_DEBUG


/*
 * If global debugging is enabled then by default all assertions
 * are enabled. Tracing is not enabled by default because it
 * involves excessive overheads, for example on some targets
 * it requires sending a string down a serial line for every
 * function call. Both assertions and tracing are controlled
 * by the following lines.
 */
/* {{CFG_DATA
   cdl_component CYGDBG_USE_ASSERTS {
       display 		"Use asserts"
       type    		boolean
       parent		CYGPKG_INFRA_DEBUG
       description "If this option is defined, asserts in the code
       		are tested.
                Assert functions (CYG_ASSERT()) are defined in
                'include/cyg/infra/cyg_ass.h' within the 'install' tree.
                If it is not defined, these result in no additional
                object code and no checking of the asserted conditions."
   }
   cdl_component CYGDBG_USE_TRACING {
       display 		"Use tracing"
       type    		boolean
       parent		CYGPKG_INFRA_DEBUG
       description "If this option is defined, tracing operations
		result in output or logging, depending on other options.
                This may have adverse effects on performance, if the time
                taken to output message overwhelms the available CPU
                power or output bandwidth.
		Trace functions (CYG_TRACE())are defined in
                'include/cyg/infra/cyg_trac.h' within the 'install' tree.
                If it is not defined, these result in no additional
                object code and no trace information."
   }
   }}CFG_DATA */
#ifdef CYGPKG_INFRA_DEBUG
# define CYGDBG_USE_ASSERTS
# undef  CYGDBG_USE_TRACING
#endif

/*
 * The eCos system uses a number of more specialised assertions in
 * addition to a conventional ASSERT() macro. By default these are all
 * enabled when general assertions are enabled, but it is possible to
 * suppress some of them and thus reduce the size of the generated code.
 *
 * Preconditions check that a condition holds true at the beginning of
 * a piece of code, typically at the start of a function. For example a
 * kernel function might have a precondition that it is only invoked
 * when the scheduler is locked.
 *
 * Postconditions check that a condition holds at the end of a piece of
 * code, typically at the point where a function returns. For example
 * at the end of scheduler initialisation there could be a
 * postcondition that there is at least one runnable thread, the idle
 * thread.
 *
 * Loop invariants check that a condition holds on every iteration of
 * a loop. For example the deferred service support code in the kernel
 * could have a loop invariant that interrupts are enabled whenever
 * there are still DSR's pending.
 */
/* {{CFG_DATA
   cdl_option CYGDBG_INFRA_DEBUG_PRECONDITIONS {
       display 		"Preconditions"
       type    		boolean
       parent		CYGDBG_USE_ASSERTS
       description "This option allows individual control of preconditions.
       		A precondition is one type of assert, which it is
                useful to control separately from more general asserts.
                The function is CYG_PRECONDITION(condition,msg)."
   }
   cdl_option CYGDBG_INFRA_DEBUG_POSTCONDITIONS {
       display 		"Postconditions"
       type    		boolean
       parent		CYGDBG_USE_ASSERTS
       description "This option allows individual control of postconditions.
       		A postcondition is one type of assert, which it is
                useful to control separately from more general asserts.
                The function is CYG_POSTCONDITION(condition,msg)."
   }
   cdl_option CYGDBG_INFRA_DEBUG_LOOP_INVARIANTS {
       display 		"Loop invariants"
       type    		boolean
       parent		CYGDBG_USE_ASSERTS
       description "This option allows individual control of loop invariants.
       		A loop invariant is one type of assert, which it is
                useful to control separately from more general asserts,
                particularly since a loop invariant is typically evaluated
                a great many times when used correctly.
                The function is CYG_LOOP_INVARIANT(condition,msg)."
   }
   }}CFG_DATA */
#ifdef CYGDBG_USE_ASSERTS
# define CYGDBG_INFRA_DEBUG_PRECONDITIONS
# define CYGDBG_INFRA_DEBUG_POSTCONDITIONS
# define CYGDBG_INFRA_DEBUG_LOOP_INVARIANTS
#endif

/*
 * The eCos system uses two types of tracing mechanisms. The most common
 * type traces events, for example an event could be logged whenever
 * an interrupt occurs or whenever a context switch takes place. The
 * second type of tracing mechanism records every function entry and
 * exit. It is possible to disable this second type of tracing while
 * leaving the main tracing facility enabled.
 */
/* {{CFG_DATA
   cdl_option CYGDBG_INFRA_DEBUG_FUNCTION_REPORTS {
       display 		"Trace function reports"
       type    		boolean
       parent		CYGDBG_USE_TRACING
       description "This option allows individual control of
       		function entry/exit tracing, independent of
                more general tracing output.
                This may be useful to remove clutter from a
                trace log."
   }
   }}CFG_DATA */
#ifdef CYGDBG_USE_TRACING
# define CYGDBG_INFRA_DEBUG_FUNCTION_REPORTS
#endif

/*
 * There are currently three tracing and assert destinations. The NULL
 * destination simply throws all tracing messages away. The SIMPLE
 * destination uses the kernel diagnostic output routines to print test
 * messages on the diagnostic device.  The FANCY destination does a more
 * elaborate job of it.  Define exactly one of these.
 */
/* {{CFG_DATA
   cdl_option CYGDBG_INFRA_DEBUG_TRACE_ASSERT_NULL {
       display 		"Null output"
       type    		radio
       parent		CYGPKG_INFRA_DEBUG
       description "A null output module which is useful when
       		debugging interactively; the output routines
                can be breakpointed rather than have them actually
		'print' something."
   }
   cdl_option CYGDBG_INFRA_DEBUG_TRACE_ASSERT_SIMPLE {
       display 		"Simple output"
       type    		radio
       parent		CYGPKG_INFRA_DEBUG
       description "An output module which produces simple output
       		from tracing and assertion events."
   }
   cdl_option CYGDBG_INFRA_DEBUG_TRACE_ASSERT_FANCY {
       display 		"Fancy output"
       type    		radio
       parent		CYGPKG_INFRA_DEBUG
       description "An output module which produces fancy output
       		from tracing and assertion events."
   }
   cdl_component CYGDBG_INFRA_DEBUG_TRACE_ASSERT_BUFFER {
       display 		"Buffered tracing"
       type    		radio
       parent		CYGPKG_INFRA_DEBUG
       description "An output module which buffers output
       		from tracing and assertion events. The stored
                messages are output when an assert fires, or
                cyg_trace_output() is called.
                Of course, there will only be stored messages
                if tracing per se (CYGDBG_USE_TRACING)
                is enabled above."
   }
   cdl_option CYGDBG_INFRA_DEBUG_TRACE_BUFFER_SIZE {
       display 		"Trace buffer size"
       type    		count
       legal_values     5 to 65535
       parent		CYGDBG_INFRA_DEBUG_TRACE_ASSERT_BUFFER
       description "The size of the trace buffer. This counts the number
                    of trace records stored. When the buffer fills it
                    either wraps, stops recording, or generates output."
   }
   cdl_option CYGDBG_INFRA_DEBUG_TRACE_BUFFER_WRAP {
       display 		"Wrap trace buffer when full"
       type    		radio
       parent		CYGDBG_INFRA_DEBUG_TRACE_ASSERT_BUFFER
       description "When the trace buffer has filled with records it
                    starts again at the beginning. Hence only the last
                    CYGDBG_INFRA_DEBUG_TRACE_BUFFER_SIZE messages will
                    be recorded."
   }
   cdl_option CYGDBG_INFRA_DEBUG_TRACE_BUFFER_HALT {
       display 		"Halt trace buffer when full"
       type    		radio
       parent		CYGDBG_INFRA_DEBUG_TRACE_ASSERT_BUFFER
       description "When the trace buffer has filled with records it
                    stops recording. Hence only the first
                    CYGDBG_INFRA_DEBUG_TRACE_BUFFER_SIZE messages will
                    be recorded."
   }
   cdl_option CYGDBG_INFRA_DEBUG_TRACE_BUFFER_PRINT {
       display 		"Print trace buffer when full"
       type    		radio
       parent		CYGDBG_INFRA_DEBUG_TRACE_ASSERT_BUFFER
       description "When the trace buffer has filled with records it
                    prints the contents of the buffer. The buffer is then
                    emptied and the system continues."
   }
   cdl_option CYGDBG_INFRA_DEBUG_TRACE_BUFFER_PRINT_ON_ASSERT {
       display 		"Print trace buffer on assert fail"
       type    		bool
       parent		CYGDBG_INFRA_DEBUG_TRACE_ASSERT_BUFFER
       description "When an assertion fails the trace buffer will be printed to
                    the default diagnostic device."
   }
   }}CFG_DATA */
# if defined( CYGDBG_USE_TRACING ) || defined( CYGDBG_USE_ASSERTS )
#  undef  CYGDBG_INFRA_DEBUG_TRACE_ASSERT_NULL
#  undef  CYGDBG_INFRA_DEBUG_TRACE_ASSERT_SIMPLE
#  undef  CYGDBG_INFRA_DEBUG_TRACE_ASSERT_FANCY
#  define CYGDBG_INFRA_DEBUG_TRACE_ASSERT_BUFFER
# endif

# if defined(CYGDBG_USE_TRACING)
#  if defined(CYGDBG_INFRA_DEBUG_TRACE_ASSERT_BUFFER)
#   define CYGDBG_INFRA_DEBUG_TRACE_BUFFER_SIZE  32
#   define CYGDBG_INFRA_DEBUG_TRACE_BUFFER_WRAP
#   undef  CYGDBG_INFRA_DEBUG_TRACE_BUFFER_HALT
#   undef  CYGDBG_INFRA_DEBUG_TRACE_BUFFER_PRINT
#   define CYGDBG_INFRA_DEBUG_TRACE_BUFFER_PRINT_ON_ASSERT
#  endif
# endif


/*
 * All assertions within eCos contain a text message which should give
 * some information about the condition being tested. These text messages
 * will end up being embedded in the final application image and hence
 * there is a significant penalty in terms of code size. It is possible
 * to suppress the use of these messages by disabling
 * CYGDBG_INFRA_DEBUG_ASSERT_MESSAGE.
 * This results in smaller code size, but there is less information if
 * an assertion actually gets triggered.
 *
 * Similarly eCos trace macros also make use of a text message, and
 * this message can be suppressed by disabling
 * CYGDBG_INFRA_DEBUG_TRACE_MESSAGE.
 */

/* {{CFG_DATA
   cdl_option CYGDBG_INFRA_DEBUG_ASSERT_MESSAGE {
       display 		"Use assert text"
       type    		boolean
       parent		CYGDBG_USE_ASSERTS
       description "All assertions within eCos contain a text message
		which should give some information about the condition
		being tested.
                These text messages will end up being embedded in the
                application image and hence there is a significant penalty
                in terms of image size.
                It is possible to suppress the use of these messages by
		disabling this option.
                This results in smaller code size, but there is less
		human-readable information if an assertion actually gets
		triggered."
   }
   cdl_option CYGDBG_INFRA_DEBUG_TRACE_MESSAGE {
       display 		"Use trace text"
       type    		boolean
       parent		CYGDBG_USE_TRACING
       description "All trace calls within eCos contain a text message
		which should give some information about the circumstances.
                These text messages will end up being embedded in the
                application image and hence there is a significant penalty
                in terms of image size.
                It is possible to suppress the use of these messages by
		disabling this option.
                This results in smaller code size, but there is less
		human-readable information available in the trace output,
                possibly only filenames and line numbers."
   }
   }}CFG_DATA */
#define CYGDBG_INFRA_DEBUG_ASSERT_MESSAGE
#define CYGDBG_INFRA_DEBUG_TRACE_MESSAGE

/* ========================================================================
 * By default all assertion and trace macros will make use of the name
 * of the current function, via gcc's __PRETTY_FUNCTION__ extension.
 * This provides useful extra information, but again there is some
 * code size overhead. It is possible to suppress the use of function
 * names, and have the assertions and trace facilities only use the
 * filename and linenumber information provided by __FILE__ and __LINE__.
 */
/* {{CFG_DATA
   cdl_option CYGDBG_INFRA_DEBUG_FUNCTION_PSEUDOMACRO {
       display 		"Use function names"
       type    		boolean
       parent		CYGPKG_INFRA_DEBUG
       description "All trace and assert calls within eCos contain a
       		reference to the builtin macro '__PRETTY_FUNCTION__',
                which evaluates to a string containing
                the name of the current function.
                This is useful when reading a trace log.
                It is possible to suppress the use of the function name
		by disabling this option.
                This results in smaller code size, but there is less
		human-readable information available in the trace output,
                possibly only filenames and line numbers."
   }
   }}CFG_DATA */
#define CYGDBG_INFRA_DEBUG_FUNCTION_PSEUDOMACRO

/* =================================================================
 * The following options allow particular compatibility modes to be
 * enabled, when they require specialised support from the startup
 * process. These can affect the environment in which the program
 * runs.
 *
 * CYGSEM_START_ISO_C_COMPATIBILITY enables compatibility with ISO C,
 * in particular in eCos it creates a thread that invokes the function
 * main() which the user must supply. It does this by invoking the function
 * cyg_iso_c_start(). For more information, refer to the C library
 * documentation.
 *
 * CYGSEM_START_UITRON_COMPATIBILITY enables compatibility with uItron.
 * You must configure uItron with the correct tasks, and then enabling this
 * option starts the uItron subsystem. It does this by invoking the function
 * cyg_uitron_start().
 *
 * Both these can also be done by the user overriding cyg_user_start(),
 * cyg_package_start(), or cyg_prestart(). Refer to the documentation on
 * how and when to do this.
 */

/* {{CFG_DATA
   cdl_component CYGPKG_INFRA_STARTUP {
       display 		"Startup options"
       type    		dummy
       parent		CYGPKG_INFRA
       description "Some packages require a startup routine to be called.
       		This can be carried out by application code, by supplying
                a routine called cyg_package_start() which calls the
                appropriate package startup routine(s).
                Alternatively, this routine can be constructed automatically
                and configured to call the startup routines of your choice."
	   doc ref/ecos-ref/c-library-startup.html
   }
   cdl_option CYGSEM_START_ISO_C_COMPATIBILITY {
       display 		"Start ISO C library"
       type    		boolean
       parent		CYGPKG_INFRA_STARTUP
       requires		CYGPKG_LIBC
       description "Generate a call to initialize the ISO C library
		(libc) within the system version of cyg_package_start().
		This enables compatibility with ISO C, in particular in
		eCos it creates a thread that invokes the function main()
		which the user must supply.
                It does this by invoking the function cyg_iso_c_start().
                If this is disabled, and you want to use the C library, you
                call cyg_iso_c_start() from your own cyg_package_start()
                or cyg_userstart()."
   }
   cdl_option CYGSEM_START_UITRON_COMPATIBILITY {
       display 		"Start uITRON subsystem"
       type    		boolean
       parent		CYGPKG_INFRA_STARTUP
       requires		CYGPKG_UITRON
       description "Generate a call to initialize the
       		uITRON compatibility subsystem
                within the system version of cyg_package_start().
		This enables compatibility with uITRON.
		You must configure uITRON with the correct tasks before
		starting the uItron subsystem.
                If this is disabled, and you want to use uITRON,
                you must call cyg_uitron_start() from your own
                cyg_package_start() or cyg_userstart()."
   }
   }}CFG_DATA */
#ifdef CYGPKG_LIBC
#define CYGSEM_START_ISO_C_COMPATIBILITY
#endif

#ifdef CYGPKG_UITRON
#undef CYGSEM_START_UITRON_COMPATIBILITY
#endif

/* ========================================================================
 * memcpy()/memset() configuration
 *
 * The infra package contains implementations of memcpy() and memset()
 * because gcc generates code that uses these when initialising
 * structures, if it has been configured with its own builtin versions
 * disabled (-fno-builtin).
 *
 */

/*
  {{CFG_DATA

  cdl_option CYGIMP_INFRA_PREFER_SMALL_TO_FAST_MEMCPY {
      display         "Smaller slower memcpy()"
      description     "Enabling this option causes the implementation of
                       the standard memcpy() routine to reduce code
                       size at the expense of execution speed. This
                       option is automatically enabled with the use of
                       the -Os option to the compiler. Also note that
                       the compiler will try to use its own builtin
                       version of memcpy() if possible, ignoring the
                       implementation in this package, unless given
                       the -fno-builtin compiler option."
      type            boolean
      parent          CYGPKG_INFRA
  }

  cdl_option CYGIMP_INFRA_PREFER_SMALL_TO_FAST_MEMSET {
      display         "Smaller slower memset()"
      description     "Enabling this option causes the implementation of
                       the standard memset() routine to reduce code
                       size at the expense of execution speed. This
                       option is automatically enabled with the use of
                       the -Os option to the compiler. Also note that
                       the compiler will try to use its own builtin
                       version of memset() if possible, ignoring the
                       implementation in this package, unless given
                       the -fno-builtin compiler option."
      type            boolean
      parent          CYGPKG_INFRA
  }

  }}CFG_DATA
*/

#undef CYGIMP_INFRA_PREFER_SMALL_TO_FAST_MEMCPY
#undef CYGIMP_INFRA_PREFER_SMALL_TO_FAST_MEMSET

/* ========================================================================
 * Debugging-related miscellania.
 *
 * NOTE: this is not yet fully supported, by CDL nor by having a proper
 * symbol name.
 *
 * Define how diag data is directed off of the board. If using an actual
 * device then a device must be configured to support diag.
 */

#undef CYGDBG_INFRA_DIAG_USE_DEVICE

/* ========================================================================
 * That's all.
 */

#endif /* CYGONCE_PKGCONF_INFRA_H */
/* EOF infra.h */
