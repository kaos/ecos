/*==========================================================================
//
//	cogent.ld
//
//	Linker script for Cogent/MPC860 board
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
// Contributors:nickg
// Date:	1998-02-17
// Purpose:	Cogent Linker script
// Description:	This script is passed to the linker to define the memory
//              of the Cogent/MPC860 board.
//
//####DESCRIPTIONEND####
//
//========================================================================*/
STARTUP(vectors.o)
ENTRY(reset_vector)

GROUP(libtarget.a libgcc.a)

#include <pkgconf/hal.h>

#ifdef CYG_HAL_STARTUP_ROM
#define LOAD_BASE 0xFFF00000
#define DATA_BASE 0x00780000      // Data at top of memory so GDB
                                  // doesn't trash ROM state while
                                  // downloading programs.
#endif
#ifdef CYG_HAL_STARTUP_RAM
#define LOAD_BASE 0x00000000
#define DATA_BASE
#endif

// Alignment of sections must be equal to or bigger than the alignment
// requested by gcc (use 'objdump -h' to see these).

SECTIONS
{

  /* Start of ROM */
  . = LOAD_BASE;

  .rom_vectors   :
  {
    KEEP(*(.vectors))
  }

  /*------------------------------------------------------------*/

  .text      :
  {
    _stext = .;
    *(.text*)
    *(.gnu.warning)
    *(.gnu.linkonce*)
    *(.init)
  }

  _etext = .;
  PROVIDE (etext = .);
  .fini      : { *(.fini)    }
  .rodata1   : { *(.rodata1) }
  .rodata    : { *(.rodata*)  }
  .fixup     : { *(.fixup)   }
  .gcc_except_table : { *(.gcc_except_table) }

//  __rom_data_start = __rom_ram_vectors_start + SIZEOF( .ram_vectors );
  __rom_data_start = ALIGN(8);
  .data DATA_BASE : 
  AT( __rom_data_start )
  {
    __ram_data_start = .;
    *(.data*)
    __GOT1_START__ = .;
    *(.got1) 
    __GOT1_END__ = .;
    /* Put .ctors and .dtors next to the .got2 section, so that the pointers
       get relocated with -mrelocatable. */
#ifdef CYG_KERNEL_USE_INIT_PRIORITY
    . = ALIGN(8);
    __CTOR_LIST__ = .;
    KEEP(*(SORT(.ctors*)))
    __CTOR_END__ = .;
    __DTOR_LIST__ = .;
    KEEP(*(SORT(.dtors*)))
    __DTOR_END__ = .;
#else
    __CTOR_LIST__ = .;
    *sched.o(.ctors*)
    *clock.o(.ctors*)
    *thread.o(.ctors*)
    *(.ctors*)
    __CTOR_END__ = .;
    __DTOR_LIST__ = .;
    *(.dtors)
    __DTOR_END__ = .;
#endif
    __GOT2_START__ = .;
    *(.got2)
    __GOT2_END__ = .;
    __GOT_START__ = .;
    _GLOBAL_OFFSET_TABLE_ = . + 32768;
    _SDA_BASE_ = .;
    *(.got.plt)
    *(.got)
    __GOT_END__ = .;
    *(.dynamic)
    // We want the small data sections together, so single-instruction offsets
    // can access them all, and initialized data all before uninitialized, so
    // we can shorten the on-disk segment size.
    __SDATA_START__ = .;
    *(.sdata)
    __SDATA2_START__ = .;
    *(.sdata2)
  }
  __ram_data_end = .;
  PROVIDE( __ram_data_end = .);
  _edata  =  .;
  PROVIDE (edata = .);
  . = ALIGN(4);
  __bss_start = .;
  .sbss      :
  {
        __SBSS_START__ = .;
        *(.sbss)
        __SBSS_END__ = .;
        __SBSS2_START__ = .;
        *(.sbss2)
        __SBSS2_END__ = .;
        *(.scommon)
  }
  . = ALIGN(8);
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

}
