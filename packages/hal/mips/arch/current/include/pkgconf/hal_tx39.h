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

   cdl_package CYGPKG_HAL_TX39 {
       display "TX39 architecture"
       type    radio
       parent  CYGPKG_HAL
       target  tx39
       description "
           The TX39 architecture HAL package provides generic support
           for this processor architecture. It is also necessary to
           select a specific target platform HAL package."
   }

   cdl_option CYGHWR_HAL_MIPS_CPU_FREQ {
       display "CPU frequency"
       type     count
       legal_values 0 to 1000000
       parent  CYGPKG_HAL_TX39
       description "
           This option contains the frequency of the CPU in MegaHertz.
           Choose the frequency to match the processor you have. This
           may affect thing like serial device, interval clock and
           memory access speed settings."
   }
   
   cdl_option CYGDBG_HAL_MIPS_INSTALL_CTRL_C_ISR {
       display "Install an Interrupt Service Routine to catch HALT requests"
       type     bool
       parent   CYGPKG_HAL_TX39
       description "
           This option enables startup code to install a default ISR on the
           serial debug connection which reads characters looking for the
           HALT character control-C (\003). This allows a running program
           to be interrupted from GDB. This option only has effect if eCos
           is configured to run with the Cygmon ROM monitor on the JMR3904
           development board. Interrupts must be enabled for the code to
           work. The serial device drivers, if enabled, do take over the
           ISR and function correctly."
   }
   
   }}CFG_DATA */


#define CYGHWR_HAL_MIPS_CPU_FREQ        50

#define CYGDBG_HAL_MIPS_INSTALL_CTRL_C_ISR

/* -------------------------------------------------------------------*/
/* Workaround for TX3904 Timer TRR register problem                   */
/*                                                                    */
/* We do it conditionally so that it is possible to override it on    */
/* the command line, for example, for a simulator build               */
#ifndef CYGARC_TX39_PR19846
# define CYGHWR_HAL_MIPS_TX3904_TRR_REQUIRES_SYNC
#endif


/* -------------------------------------------------------------------*/
/* translate the user defined CPU frequency into a the real number of */
/* cycles.                                                            */

#if (CYGHWR_HAL_MIPS_CPU_FREQ == 50)
#define CYGHWR_HAL_MIPS_CPU_FREQ_ACTUAL 49152000
#elif (CYGHWR_HAL_MIPS_CPU_FREQ == 66)
#define CYGHWR_HAL_MIPS_CPU_FREQ_ACTUAL 66355200
#else
#error Unsupported TX39 CPU frequency
#endif

/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_TX39_H */
/* EOF hal_tx39.h */
