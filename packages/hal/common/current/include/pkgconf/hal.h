#ifndef CYGONCE_PKGCONF_HAL_H
#define CYGONCE_PKGCONF_HAL_H
// ====================================================================
//
//      pkgconf/hal.h
//
//      HAL configuration file
//
// ====================================================================
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
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg, jskov
// Contributors: nickg, jskov,
//               jlarmour
// Date:        1999-01-21
// Purpose:     To allow the user to edit HAL configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

/* ---------------------------------------------------------------------
 * Some of the HAL configuration options depend on global options,
 * so it is necessary to include the global options first.
 */

#include <pkgconf/system.h>

/* ---------------------------------------------------------------------
   Define an overall package for the HAL. All other HAL packages
   live below this.

   {{CFG_DATA

   cdl_package CYGPKG_HAL {
       display "eCos HAL"
       type    dummy
       description "
           The eCos HAL package provide a porting layer for
           higher-level parts of the system such as the kernel and the
           C library. Each installation should have HAL packages for
           one or more architectures, and for each architecture there
           may be one or more supported platforms. It is necessary to
           select one target architecture and one platform for that
           architecture. There are also a number of configuration
           options that are common to all HAL packages."
           doc ref/ecos-ref/the-ecos-hardware-abstraction-layer-hal.html
   }

   }}CFG_DATA */

/* ---------------------------------------------------------------------
 * The platform-independent HAL configuration options go here,
 * inside a component CYGPKG_HAL_COMMON. In practice some of the
 * options are not completely platform-independent, but they are
 * likely to be available on the vast majority of platforms and
 * it is more convenient to provide them by default and let the
 * appropriate platforms disable them (probably via a suitable
 * interface).
 *
 * The common component is subdivided into components for
 * interrupt handling, thread context, and rom monitor/gdb support.

 {{CFG_DATA

 cdl_component CYGPKG_HAL_COMMON {
     display "Platform-independent HAL options"
     parent  CYGPKG_HAL
     type    dummy
     description "
         A number of configuration options are common to most or all
         HAL packages, for example options controlling how much state
         should be saved during a context switch. The implementations
         of these options will vary from architecture to architecture."
 }

 cdl_option CYGFUN_HAL_COMMON_KERNEL_SUPPORT {
     display  "Provide eCos kernel support"
     parent   CYGPKG_HAL_COMMON
     requires CYGPKG_KERNEL
     description "
         The HAL can be configured to either support the full eCos
         kernel, or to support only very simple applications which do
         not require a full kernel. If kernel support is not required
         then some of the startup, exception, and interrupt handling
         code can be eliminated."
 }

 cdl_option CYGDBG_HAL_DIAG_DISABLE_GDB_PROTOCOL {
     display    "Disable GDB protocol for diagnostic output"
     parent     CYGPKG_HAL_COMMON
     description "
         This option forces diagnostic output to not use the GDB
         protocol.  Most programs will use the GDB protocol for
         diagnostic I/O because of the environment.  This option
         allows for an explicit override in these cases."
 }

 }}CFG_DATA */
#define CYGFUN_HAL_COMMON_KERNEL_SUPPORT
#undef  CYGDBG_HAL_DIAG_DISABLE_GDB_PROTOCOL

#ifdef CYGDBG_HAL_DIAG_DISABLE_GDB_PROTOCOL
#if !defined(CYGPKG_HAL_ARM)
#err GDB protocol cannot be explicitly disabled for this platform.
#endif
#endif

/* {{CFG_DATA
   
 cdl_component CYGPKG_HAL_COMMON_INTERRUPTS {
     display "HAL interrupt handling"
     parent  CYGPKG_HAL_COMMON
     type    dummy
     description "
         A number of configuration options related to interrupt
         handling are common to most or all HAL packages, even though
         the implementations will vary from architecture to
         architecture."
 }

 cdl_option CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK {
     display      "Use separate stack for interrupts"
     parent       CYGPKG_HAL_COMMON_INTERRUPTS
     description  "
         When an interrupt occurs this interrupt can be handled either
         on the current stack or on a separate stack maintained by the
         HAL. Using a separate stack requires a small number of extra
         instructions in the interrupt handling code, but it has the
         advantage that it is no longer necessary to allow extra space
         in every thread stack for the interrupt handlers. The amount
         of extra space required depends on the interrupt handlers
         that are being used."
 }

 # NOTE: various parts of the system such as device drivers should
 # impose lower bounds on this. The actual lower bound depends on a
 # platform-specific value for startup overheads, and the minimum
 # sizes specified by the various device drivers. If interrupts are
 # not handled on a separate stack then only the startup overheads
 # are significant. If nested interrupts are disabled then the
 # lower bound is the maximum of the individual sizes, otherwise
 # it is the sum of these sizes. It is not currently possible to
 # express a relationship like this.
 cdl_option CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE {
     display        "Interrupt stack size"
     parent         CYGPKG_HAL_COMMON_INTERRUPTS
     type           count
     legal_values   1024 to 1048576
     description "
         This configuration option specifies the stack size in bytes
         for the interrupt stack. Typically this should be a multiple
         of 16, but the exact requirements will vary from architecture
         to architecture. The interrupt stack serves two separate
         purposes. It is used as the stack during system
         initialization. In addition, if the interrupt system is
         configured to use a separate stack then all interrupts will
         be processed on this stack. The exact memory requirements
         will vary from application to application, and will depend
         heavily on whether or not other interrupt-related options,
         for example nested interrupts, are enabled."
 }

 cdl_option CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING {
     display "Allow nested interrupts"
     parent  CYGPKG_HAL_COMMON_INTERRUPTS
     description "
         When an interrupt occurs the HAL interrupt handling code can
         either leave interrupts disabled for the duration of the
         interrupt handling code, or by doing some extra work it can
         reenable interrupts before invoking the interrupt handler and
         thus allow nested interrupts to happen. If all the interrupt
         handlers being used are small and do not involve any loops
         then it is usually better to disallow nested interrupts.
         However if any of the interrupt handlers are more complicated
         than nested interrupts will usually be required."
 }

 cdl_option CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT {
     display "Save minimum context on interrupt"
     parent  CYGPKG_HAL_COMMON_INTERRUPTS
     description "
         The HAL interrupt handling code can exploit the calling conventions
         defined for a given architecture to reduce the amount of state
         that has to be saved. Generally this improves performance and
         reduces code size. However it can make source-level debugging
         more difficult.
     "
 }
 
 cdl_option CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN {
     display            "Chain all interrupts together"
     parent             CYGPKG_HAL_COMMON_INTERRUPTS
     description "
         Interrupts can be attached to vectors either singly, or be
         chained together. The latter is necessary if there is no way
         of discovering which device has interrupted without
         inspecting the device itself. It can also reduce the amount
         of RAM needed for interrupt decoding tables and code."
 }

 cdl_option CYGIMP_HAL_COMMON_INTERRUPTS_IGNORE_SPURIOUS {
     display      "Ignore spurious [fleeting] interrupts"
     parent       CYGPKG_HAL_COMMON_INTERRUPTS
     description  "
         On some hardware, interrupt sources may not be de-bounced or
         de-glitched.  Rather than try to handle these interrupts (no
         handling may be possible), this option allows the HAL to simply
         ignore them.  In most cases, if the interrupt is real it will
         reoccur in a detectable form."
 }


 }}CFG_DATA

*/

#define CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
#define CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE 4096
#undef  CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING
#define CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT
#undef  CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN
#undef  CYGIMP_HAL_COMMON_INTERRUPTS_IGNORE_SPURIOUS


/* {{CFG_DATA
   
 cdl_component CYGPKG_HAL_COMMON_CONTEXT {
     display "HAL context switch support"
     parent  CYGPKG_HAL_COMMON
     type    dummy
     description "
         A number of configuration options related to thread contexts
         are common to most or all HAL packages, even though the
         implementations will vary from architecture to architecture."
 }

 cdl_option CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM {
     display "Use minimum thread context"
     parent  CYGPKG_HAL_COMMON_CONTEXT
     description "
         The thread context switch code can exploit the calling conventions
         defined for a given architecture to reduce the amount of state
         that has to be saved during a context switch. Generally this
         improves performance and reduces code size. However it can make
         source-level debugging more difficult."
 }
 
 }}CFG_DATA */
#define CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM

/* {{CFG_DATA

  # NOTE: The requirement for kernel exception support is bogus in that
  # the user can supply a deliver_exception function herself. In that
  # case, however, it is easy to force the kernel option off while leaving
  # this one on.  Having the requirement prevents accidental invalid
  # configurations of the kernel.
  cdl_option CYGPKG_HAL_EXCEPTIONS {
      display  "HAL exception support"
      parent   CYGPKG_HAL_COMMON
      requires CYGPKG_KERNEL_EXCEPTIONS
      description "
          When a processor exception occurs, for example an attempt to
          execute an illegal instruction or to perform a divide by
          zero, this exception may be handled in a number of different
          ways. If the target system has gdb support then typically
          the exception will be handled by gdb code. Otherwise if the
          HAL exception support is enabled then the HAL will invoke a
          routine deliver_exception(). Typically this routine will be
          provided by the eCos kernel, but it is possible for
          application code to provide its own implementation. If the
          HAL exception support is not enabled and a processor
          exception occurs then the behaviour of the system is
          undefined.
      "  
   }

   }}CFG_DATA */
#define CYGPKG_HAL_EXCEPTIONS

/* {{CFG_DATA

   cdl_option CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG {
       display     "Stop calling constructors early"
       description "This option supports environments where some constructors
                    must be run in the context of a thread rather than at
                    simple system startup time. A boolean flag named
                    cyg_hal_stop_constructors is set to 1 when constructors
                    should no longer be invoked. It is up to some other
                    package to deal with the rest of the constructors.
                    In the current version this is only possible with the
                    C library."
       type        boolean
       requires    CYGSEM_LIBC_INVOKE_DEFAULT_STATIC_CONSTRUCTORS
       parent      CYGPKG_HAL_COMMON
   }

   }}CFG_DATA */

#undef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG

/* {{CFG_DATA

   cdl_option CYGSEM_HAL_INSTALL_MMU_TABLES {
       display     "Install MMU tables."
       description "This option controls whether this application installs
       it's own Memory Management Unit (MMU) tables, or relies on the
       existing environment to run."
       type        boolean
       parent      CYGPKG_HAL_COMMON
   }

   cdl_option CYGSEM_HAL_STATIC_MMU_TABLES {
       display     "Use static MMU tables."
       description "This option defines an environment where any Memory
       Management Unit (MMU) tables are constant.  Normally used by ROM
       based environments, this provides a way to save RAM usage which
       would otherwise be required for these tables."
       type        boolean
       requires    CYGSEM_HAL_INSTALL_MMU_TABLES
       parent      CYGPKG_HAL_COMMON
   }

   }}CFG_DATA */

#define CYGSEM_HAL_INSTALL_MMU_TABLES
#undef  CYGSEM_HAL_STATIC_MMU_TABLES

/* ---------------------------------------------------------------------
 * Options related to source-level debugging and diagnostics.

 {{CFG_DATA

 cdl_component CYGPKG_HAL_DEBUG {
     display    "Source-level debugging support"
     type       dummy
     parent     CYGPKG_HAL
     description "
         If the source level debugger gdb is to be used for debugging
         application code then it may be necessary to configure in support
         for this in the HAL."
 }

 cdl_option CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS {
     display    "Include GDB stubs in HAL"
     parent     CYGPKG_HAL_DEBUG
     requires   !CYGPKG_HAL_POWERPC_SIM
     requires   !CYGPKG_HAL_MN10300_AM31_SIM
     requires   !CYGPKG_HAL_MIPS_SIM
     requires   !CYGPKG_HAL_I386_LINUX
     requires   !CYGPKG_HAL_SPARCLITE
     description "
         This option causes a set of GDB stubs to be included into the
         system. On some target systems the GDB support will be
         provided by other means, for example by a ROM monitor. On
         other targets, especially when building a ROM-booting system,
         the necessary support has to go into the target library
         itself."
 }

 cdl_option CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT {
     display    "Include GDB external break support for stubs"
     parent     CYGPKG_HAL_DEBUG
     requires   CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
     requires   !CYGPKG_HAL_MIPS_TX39_JMR3904
     requires   !CYGPKG_HAL_POWERPC_FADS
     requires   !CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT
     description "
        This option causes the GDB stub to add a serial interrupt handler
        which will listen for GDB break packets. This lets you stop the
        target asynchronously when using GDB, usually by hitting Control+C
        or pressing the STOP button. This option differs from
        CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT in that it is used when
        GDB stubs are present."
 }

  cdl_option CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT {
     display    "Include GDB external break support when no stubs"
     parent     CYGPKG_HAL_DEBUG
     requires   !CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
     description "
        This option adds an interrupt handler for the GDB serial line
        which will listen for GDB break packets. This lets you stop the
        target asynchronously when using GDB, usually by hitting Control+C
        or pressing the STOP button. This option differs from
        CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT in that it is used when the GDB
        stubs are NOT present."
 }

 cdl_option CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT {
     display    "Include GDB multi-threading debug support"
     parent     CYGPKG_HAL_DEBUG
     requires   CYGDBG_KERNEL_DEBUG_GDB_THREAD_SUPPORT
     description "
         This option enables some extra HAL code which is needed
         to support multi-threaded source level debugging."
 }

   }}CFG_DATA */

#undef   CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
#undef   CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
#define  CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT
#define  CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT

/*
 * NOTE:
 *
 * Right now there is no easy way to express the ROM monitor options
 * in CDL.  The problem is that the default values depend on the
 * target platform and on the desired start-up. It is possible to
 * produce something that will work correctly in a clean world, but
 * not something more general.
 *
 * One possibility is to duplicate the relevant options in the various
 * platform HAL packages, renaming them to avoid collisions. This appears
 * to be feasible with the current sources because the rom monitor options
 * are only referenced in the HAL packages. It is not clear whether or not
 * this will continue to be the case.  */

/*
 * Some development boards come with a ROM monitor that provides
 * download and debug facilities. Such ROMs may also supply interrupt
 * redirection and IO facilities. The following option causes the HAL
 * to integrate with the ROM monitor present on the configured
 * platform. If this option is not defined, the monitor will not be
 * used.  Where there is more than one ROM monitor for a specific
 * platform, a subsidiary define should identify the appropriate monitor
 * type.
 */

//#define CYG_HAL_USE_ROM_MONITOR

#if defined(CYG_HAL_MIPS_TX39_JMR3904) && defined(CYG_HAL_STARTUP_RAM)
#define CYGSEM_HAL_USE_ROM_MONITOR
#define CYGSEM_HAL_USE_ROM_MONITOR_CygMon
#endif
#if defined(CYG_HAL_MN10300_AM31_STDEVAL1) && defined(CYG_HAL_STARTUP_RAM)
#define CYGSEM_HAL_USE_ROM_MONITOR
#define CYGSEM_HAL_USE_ROM_MONITOR_CygMon
#endif

#ifdef CYG_HAL_USE_ROM_MONITOR

//#define CYG_HAL_USE_ROM_MONITOR_SLOAD
//#define CYG_HAL_USE_ROM_MONITOR_GDB_STUBS
//#define CYG_HAL_USE_ROM_MONITOR_CYGMON
//#define CYG_HAL_USE_ROM_MONITOR_PMON

#endif

/* ---------------------------------------------------------------------
 * This option decides whether we are going to BE a ROM monitor. On
 * some targets and platforms we provide a simple ROM monitor (the GDB
 * STUBS monitor) by means of a simple configuration of the HAL and
 * GDB stubs.
 * This option has no CDL since it is only set by a special configuration
 * permutation used for building a monitor.
 */

#undef CYG_HAL_ROM_MONITOR

/* ---------------------------------------------------------------------
 *
 * The section below deals with some inferences about package/feature
 * presence that are not yet supported by the full external configuration
 * tool; they are NOT user configuration, and you should NOT edit them.
 */

/* 
 * CYG_HAL_<TARGET> and CYG_HAL_<TARGET>_<PLATFORM> are generated by
 * pkgconf, using the information from the targets file.
 *
 * NOTE: Currently the code also needs these #defines.
 */
#ifdef CYGPKG_HAL_POWERPC_COGENT
# define CYGPKG_HAL_POWERPC_MPC8xx
# define CYGPKG_HAL_POWERPC_MPC860
# undef CYGPKG_HAL_POWERPC_MPC850
# undef CYGPKG_HAL_POWERPC_MPC823
#endif
#ifdef CYGPKG_HAL_POWERPC_FADS
# define CYGPKG_HAL_POWERPC_MPC8xx
# define CYGPKG_HAL_POWERPC_MPC860
#endif
#ifdef CYGPKG_HAL_POWERPC_MBX
# define CYGPKG_HAL_POWERPC_MPC8xx
# define CYGPKG_HAL_POWERPC_MPC860
# define CYGSEM_HAL_POWERPC_COPY_VECTORS
#endif
#ifdef CYGPKG_HAL_POWERPC_SIM
# define CYGPKG_HAL_POWERPC_MPC603
#endif



/* Enable this when configuring as a ROM monitor (i.e., stub) */
#undef  CYGSEM_HAL_ROM_MONITOR

#include CYGBLD_HAL_TARGET_H
#include CYGBLD_HAL_PLATFORM_H 

/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_H */
/* EOF hal.h */
