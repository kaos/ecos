#ifndef CYGONCE_PKGCONF_HAL_TX39_H
#define CYGONCE_PKGCONF_HAL_TX39_H
// ====================================================================
//
//      pkgconf/hal_tx39.h
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
// Author(s):           nickg
// Contributors:        nickg
// Date:                1999-04-06      
// Purpose:             To allow the user to edit HAL configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

#include <pkgconf/hal_mips.h>
#include CYGBLD_HAL_PLATFORM_H

/* -------------------------------------------------------------------
 * Decide which variant of the MIPS family we have.

 {{CFG_DATA

   cdl_package CYGPKG_HAL_MIPS_TX39 {
       display "TX39 variant"
       type    radio
       parent  CYGFUN_HAL_MIPS_VARIANTS
       target  mips_tx39
       description "
           The TX39 architecture HAL package provides generic support
           for this processor architecture. It is also necessary to
           select a specific target platform HAL package."
   }
 
   cdl_option CYGPKG_HAL_MIPS_TX3904 {
       display "TX3904 microprocessor"
       parent  CYGPKG_HAL_MIPS_TX39
       type    radio
       description "
           The TMPR3904F microprocessor. This is an embedded part that in
           addition to the TX39 processor core has built in peripherals
           such as memory controllers, DMA controllers, serial ports and
           timers/counters."               
   }

 }}CFG_DATA */

#define CYGPKG_HAL_MIPS_TX3904

/* -------------------------------------------------------------------
 * Characterize the CPU so that the generic architecture part can
 * configure itself.
 */

#undef  CYGHWR_HAL_MIPS_FPU             /* We have no FPU                  */
#define CYGPKG_HAL_MIPS_MSBFIRST        /* Big-endian CPU                  */

/* -------------------------------------------------------------------*/
/* Workaround for TX3904 Timer TRR register problem                   */
/*                                                                    */
/* We do it conditionally so that it is possible to override it on    */
/* the command line, for example, for a simulator build               */

#ifndef CYGARC_TX39_PR19846
//# define CYGHWR_HAL_MIPS_TX3904_TRR_REQUIRES_SYNC
#endif

/* -------------------------------------------------------------------*/
/* Translate the user defined CPU frequency into a the real number of */
/* cycles.                                                            */

#ifdef CYGPKG_HAL_MIPS_TX3904

#if (CYGHWR_HAL_MIPS_CPU_FREQ == 50)
#define CYGHWR_HAL_MIPS_CPU_FREQ_ACTUAL 49152000
#elif (CYGHWR_HAL_MIPS_CPU_FREQ == 66)
#define CYGHWR_HAL_MIPS_CPU_FREQ_ACTUAL 66355200
#else
#error Unsupported TX3904 CPU frequency
#endif

#endif

/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_TX39_H */
/* EOF hal_tx39.h */
