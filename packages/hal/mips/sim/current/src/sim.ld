/*==========================================================================
//
//	sim.ld
//
//	Linker script for MIPS simulator
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
// Author(s): 	nickg
// Contributors:	nickg
// Date:	1998-02-17
// Purpose:	MIPS simulator Linker script
// Description:	This script is passed to the linker to define the memory
//              of the MIPS simulator.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

STARTUP(vectors.o)
ENTRY(reset_vector)

GROUP(libtarget.a libgcc.a)

#include <pkgconf/hal.h>

#if defined(CYG_HAL_STARTUP_RAM)
#define LOAD_BASE 0x80000000
#define DATA_BASE
#define AT(x)
#elif defined(CYG_HAL_STARTUP_ROM)
#define LOAD_BASE 0xBFC00000
#define DATA_BASE 0x80000200
#endif

/*
 * Allocate the stack to be at the top of memory, since the stack
 * grows down
 */

/*__interrupt_stack = DATA_BASE + 0x00100000; */

SECTIONS
{
  /*------------------------------------------------------------*/

  /* Start of ROM */
  . = LOAD_BASE;

  .rom_vectors	 :
  {
#if defined(CYG_HAL_STARTUP_RAM)
    KEEP(*(.utlb_vector))
    . = ALIGN(0x80);
    KEEP(*(.other_vector))
    /* debug and reset vector not used in RAM version */
    KEEP(*(.debug_vector))
    KEEP(*(.reset_vector))
#elif defined(CYG_HAL_STARTUP_ROM)
    KEEP(*(.reset_vector))
    . = ALIGN(0x100);
    KEEP(*(.utlb_vector))
    . = ALIGN(0x80);
    KEEP(*(.debug_vector))
    . = ALIGN(0x100);
    KEEP(*(.other_vector))
#endif
  }

  /*------------------------------------------------------------*/

  .text      :
  {
    _stext = .;
#ifndef CYG_KERNEL_USE_INIT_PRIORITY
    __sched_text_start = .;
    *sched.o(.text*)
    __sched_text_end = .;
    __clock_text_start = .;
    *clock.o(.text*)
    __clock_text_end = .;
    __thread_text_start = .;
    *thread.o(.text*)
    __thread_text_end = .;
#endif
    *(.text*)
    *(.gnu.warning)
    *(.gnu.linkonce*)
    *(.init)

  }
  _etext = .;
  PROVIDE (etext = .);
  .fini      : { *(.fini)    }
  .rodata    : { *(.rodata*) }
  .rodata1   : { *(.rodata1) }
  .fixup     : { *(.fixup)   }
  .gcc_except_table : { *(.gcc_except_table) }

  /*------------------------------------------------------------*/

/*  __rom_data_start = __rom_ram_vectors_start + SIZEOF( .ram_vectors ); */
  __rom_data_start = ALIGN(8);
  .data DATA_BASE : 
  {
    __ram_data_start = .;
    *(.data*)
    *(.data1)
    _GOT1_START_ = .;
    *(.got1) 
    _GOT1_END_ = .;
    /* Put .ctors and .dtors next to the .got2 section, so that the pointers
       get relocated with -mrelocatable. */
    _GOT2_START_ = .;
    *(.got2)
#ifdef CYG_KERNEL_USE_INIT_PRIORITY
    SORT(CONSTRUCTORS)
    *(.ctors*)
    *(.dtors*)
#else
    CONSTRUCTORS
    *(.ctors*)
    *(.dtors*)
#endif
    _GOT2_END_ = .;
    . = ALIGN(8);
    _gp = .;
    __global = _gp;
    _GLOBAL_OFFSET_TABLE_ = .;
    _GOT_START_ = .;
    _SDA_BASE_ = .;
    *(.got.plt)
    *(.got)
    _GOT_END_ = .;
    *(.dynamic)
    /* We want the small data sections together, so single-instruction offsets
       can access them all, and initialized data all before uninitialized, so
       we can shorten the on-disk segment size.  */
    *(.sdata*)
    *(.sbss*)
  }
  __ram_data_end = .;
  PROVIDE( __ram_data_end = .);
  _edata  =  .;
  PROVIDE (edata = .);

  /*------------------------------------------------------------*/

  . = ALIGN(4);
  __bss_start = .;
  .bss       :
  {
   *(.scommon)
   *(.dynbss)
   *(.bss)
   *(COMMON)
  }
  . = ALIGN(4);
  _end = . ;
  PROVIDE (end = .);

  /*------------------------------------------------------------*/
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to 
     the beginning of the section so we begin them at 0.  */

  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }

  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }

  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }

  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }

  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }

}

//hal_vsr_table	= 0x80000100;

// End of sim.ld
