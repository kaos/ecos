#ifndef CYGONCE_PKGCONF_HAL_SPARCLITE_SIM_H
#define CYGONCE_PKGCONF_HAL_SPARCLITE_SIM_H
// ====================================================================
//
//      pkgconf/hal_sparclite_sim.h
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
// Contributors:        bartv, hmt
// Date:                1998-09-02      
// Purpose:             To allow the user to edit HAL configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================


/* ---------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_HAL_SPARCLITE_SLEB {
       display  "SPARClite MB8683x evaluation board"
       type     radio
       parent   CYGPKG_HAL_SPARCLITE
       platform sleb
       description "
           The SPARClite evaluation board HAL package is provided for use
           with the Fujitsu MB8683x boards."
       }
   
   cdl_option CYGHWR_HAL_SPARCLITE_SLEB_STARTUP {
       display          "Startup type"
       parent           CYGPKG_HAL_SPARCLITE_SLEB
       #type             count
       type             enum
       legal_values     ram rom
       startup
       description      "
           When targetting the SPARClite Evaluation Board it is possible to
           build the system for either RAM bootstrap or ROM bootstrap. The
           former generally requires that the board is equipped with ROMs
           containing the Cygmon ROM monitor or equivalent software that
           allows gdb to download the eCos application on to the board. The
           latter typically requires that the eCos application be blown
           into EPROMs or equivalent technology."
   }
 
   cdl_option CYGHWR_HAL_SPARCLITE_MULTIPLE_VECTOR_TRAPPING {
       display          "Multiple Vector Trapping (MVT)"
       parent           CYGPKG_HAL_SPARCLITE_SLEB
       type             boolean
       description      "
           Use Multiple Vector Trapping (MVT) rather than Single Vector
           Trapping (SVT); SVT is a feature of SPARClite CPUs which saves
           code RAM at a cost of perhaps slower interrupt and trap dispatch
           time, depending on cache behavior.  This includes speed of
           handling register window underflow and overflow, a feature of
           deep function call stacks on SPARC.  MVT requires 4kB of code
           space for trap vectors; in contrast SVT uses fewer than 20
           instructions for trap decoding."
   }

   cdl_option CYGIMP_HAL_SPARCLITE_COPY_VECTORS_TO_RAM {
       display          "Copy vectors to RAM"
       parent           CYGPKG_HAL_SPARCLITE_SLEB
       type             boolean
       description      "
           Copy the vectors and trap code out of the executable image
           to separate RAM.  With ROM startup, performance might be gained
           by copying the vectors into RAM; this includes the code for
           handling register window under/overflow.  Enable this with
           RAM startup to simulate the code and data sizes of an eventual
           ROM image.  Note: if MVT is not selected with ROM start, the
           trap code (including register window handling) is copied
           to RAM regardless; that code is small."
   }
                
   }}CFG_DATA */

#define CYGHWR_HAL_SPARCLITE_SLEB_STARTUP         ram

// Use SVT by default on SLEBs...
#undef CYGHWR_HAL_SPARCLITE_MULTIPLE_VECTOR_TRAPPING
// ...so minimal trampoline code is copied anyway; if MVT do not copy:
#undef CYGIMP_HAL_SPARCLITE_COPY_VECTORS_TO_RAM

// This is true for SPARClite:
#define CYGHWR_HAL_SPARCLITE_HAS_ASR17

/* -------------------------------------------------------------------*/
// The following are NOT config options for now: support for the native
// Fujitsu ROMs is not a requirement, just a handy thing to have:

#ifdef CYG_HAL_STARTUP_RAM
// then there is a ROM Monitor of some sort:

// CYG_HAL_USE_ROM_MONITOR_CYGMON:
// This is defined by default to allow interworking with CygMon and thus
// GDB so that Breakpoints and ^C interrupts and the like work.
//
// Undefine it if building to run with the native Fujitsu boot proms (NOT
// CYGMON) ie. a "load-and-go" type startup by means of 
//      (gdb) target sparclite udp sleb0
// or
//      (gdb) target sparclite serial /dev/ttyS0
// as opposed to the CygMon way:
//      (gdb) set remotebaud 19200
//      (gdb) target remote /dev/ttyS0
//
// Such builds will load-and-go when using CygMon, but load-and-go is all
// the interaction you get.

#define CYG_HAL_USE_ROM_MONITOR_CYGMON

// If using CygMon it's generally helpful to wrap output characters in the
// GDB protocol as $O packets; CYG_KERNEL_DIAG_GDB enables this by means of
// calling into CygMon through the vectors provided; this therefore also
// works with eg. ethernet debugging.
// 
// Disable CYG_KERNEL_DIAG_GDB and output goes direct, in clear, to
// serial port 0 (CON1).

#ifdef CYG_HAL_USE_ROM_MONITOR_CYGMON
#define CYG_KERNEL_DIAG_GDB
#endif

// However, you might want to force GDB-encoded output to the serial port
// NOT using CygMon to perform the formatting; this is really only here as
// a debugging option, in case GDB is behaving oddly.  Define
// CYG_KERNEL_DIAG_GDB_SERIAL_DIRECT + CYG_KERNEL_DIAG_GDB to make GDB $O
// packets come out the serial port (CON1)

#ifdef CYG_KERNEL_DIAG_GDB
#undef CYG_KERNEL_DIAG_GDB_SERIAL_DIRECT
#endif


#endif // CYG_HAL_STARTUP_RAM

/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_SPARCLITE_SIM_H */
/* EOF hal_sparclite_sim.h */
