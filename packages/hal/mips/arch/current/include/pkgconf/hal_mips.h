#ifndef CYGONCE_PKGCONF_HAL_MIPS_H
#define CYGONCE_PKGCONF_HAL_MIPS_H
// ====================================================================
//
//      pkgconf/hal_mips.h
//
//      HAL configuration file
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
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           bartv
// Contributors:        bartv
// Date:                1998-09-02      
// Purpose:             To allow the user to edit HAL configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================


/* ---------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_HAL_MIPS {
       display "MIPS architecture"
       type    radio
       parent  CYGPKG_HAL
       description "
           The MIPS architecture HAL package provides generic support
           for this processor architecture. It is also necessary to
           select a CPU variant and a specific target platform HAL
           package."
   }

   cdl_component CYGFUN_HAL_MIPS_VARIANTS {
       display "MIPS variants"
       type dummy
       parent CYGPKG_HAL_MIPS
       description "
           A number MIPS variants are supported."
   }

   cdl_option CYGHWR_HAL_MIPS_CPU_FREQ {
       display "CPU frequency"
       type     count
       legal_values 0 to 1000000
       parent  CYGPKG_HAL_MIPS
       description "
           This option contains the frequency of the CPU in MegaHertz.
           Choose the frequency to match the processor you have. This
           may affect thing like serial device, interval clock and
           memory access speed settings."
   }
   
   cdl_option CYGSEM_HAL_MIPS_EMULATE_UNIMPLEMENTED_FPU_OPS {
       display  "Emulate unimplemented FPU opcodes"
       type     boolean
       parent   CYGPKG_HAL_MIPS
       description "
           Enabling this option will include a hook in the exception
           processing so that Unimplemented Operation FPU exceptions
           may be handled. This option has no effect if there is no
           hardware floating-point unit. Note that not all situations
           in which an exception is raised may be handled. If not, the
           exception will be passed on as normal through the standard
           exception delivery mechanism."
   }
   
   }}CFG_DATA */

#define CYGHWR_HAL_MIPS_CPU_FREQ        50
#define CYGSEM_HAL_MIPS_EMULATE_UNIMPLEMENTED_FPU_OPS

/* -------------------------------------------------------------------*/
/* If either the CTRLC or BREAK support options in hal.h are set      */
/* then set our own option to turn on shared generic support for      */
/* control C handling.                                                */


#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) || \
    defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)

#define CYGDBG_HAL_MIPS_DEBUG_GDB_CTRLC_SUPPORT

#endif


/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_MIPS_H */
/* EOF hal_mips.h */
