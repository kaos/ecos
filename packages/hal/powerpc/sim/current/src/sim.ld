//==========================================================================
//
//      sim.ld
//
//      Linker script for PowerPC simulator
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://www.cygnus.com.
//
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
//
// The Original Code is eCos code, released September 30, 1998.
//
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg
// Contributors:nickg
// Date:        1998-02-17
// Purpose:     PowerPC simulator Linker script
// Description: This script is passed to the linker to define the memory
//              of the PowerPC simulator.
//
//####DESCRIPTIONEND####
//
//==========================================================================
STARTUP(vectors.o)
OUTPUT_FORMAT("elf32-powerpc")
OUTPUT_ARCH(powerpc)
ENTRY(reset_vector)
// Do we need any of these for elf?
// __DYNAMIC = 0;

GROUP(libtarget.a libgcc.a)

#include <pkgconf/hal.h>

MEMORY 
{

  // In a RAM system, fake the division between ROM and
  // RAM for testing.
  ram_vectors   : ORIGIN = 0x00000000, LENGTH = 0x2000
  rom_vectors   : ORIGIN = 0x00002000, LENGTH = 0x2000
  ram           : ORIGIN = 0x00004000, LENGTH = 496k
  rom           : ORIGIN = 0x00080000, LENGTH = 512k
}

// Allocate the stack to be at the top of memory, since the stack
// grows down

PROVIDE (__interrupt_stack = 0x00100000);

// Initalize some symbols to be zero so we can reference them in the
// crt0 without core dumping. These functions are all optional, but
// we do this so we can have our crt0 always use them if they exist. 
// This is so BSPs work better when using the crt0 installed gcc.
// We have to initalize them twice, so we cover a.out (which prepends
// an underscore) and coff object file formats.

//PROVIDE (hardware_init_hook = 0);
//PROVIDE (software_init_hook = 0);

SECTIONS
{
  // Read-only sections, merged into text segment:
  .interp        : { *(.interp)         } >rom
  .hash          : { *(.hash)           } >rom
  .dynsym        : { *(.dynsym)         } >rom
  .dynstr        : { *(.dynstr)         } >rom
  .rel.text      : { *(.rel.text)       } >rom
  .rela.text     : { *(.rela.text)      } >rom
  .rel.data      : { *(.rel.data)       } >rom
  .rela.data     : { *(.rela.data)      } >rom
  .rel.rodata    : { *(.rel.rodata)     } >rom
  .rela.rodata   : { *(.rela.rodata)    } >rom
  .rel.got       : { *(.rel.got)        } >rom
  .rela.got      : { *(.rela.got)       } >rom
  .rel.got1      : { *(.rel.got1)       } >rom
  .rela.got1     : { *(.rela.got1)      } >rom
  .rel.got2      : { *(.rel.got2)       } >rom
  .rela.got2     : { *(.rela.got2)      } >rom
  .rel.ctors     : { *(.rel.ctors)      } >rom
  .rela.ctors    : { *(.rela.ctors)     } >rom
  .rel.dtors     : { *(.rel.dtors)      } >rom
  .rela.dtors    : { *(.rela.dtors)     } >rom
  .rel.init      : { *(.rel.init)       } >rom
  .rela.init     : { *(.rela.init)      } >rom
  .rel.fini      : { *(.rel.fini)       } >rom
  .rela.fini     : { *(.rela.fini)      } >rom
  .rel.bss       : { *(.rel.bss)        } >rom
  .rela.bss      : { *(.rela.bss)       } >rom
  .rel.plt       : { *(.rel.plt)        } >rom
  .rela.plt      : { *(.rela.plt)       } >rom
  .init          : {
                        *(.init.head)
                        *(.init)
                        *(.init.tail)
                   } >rom =0
  .plt           : { *(.plt)            } >rom
  .text      :
  {
    _stext = .;
    *(.text*)
    *(.gnu.warning)
    *(.gnu.linkonce*)
  } >rom =0
  _etext = .;
  PROVIDE (etext = .);
  .fini      : {
                 *(.fini.head)
                 *(.fini)
                 *(.fini.tail)
               } >rom =0

  .rodata1   : { *(.rodata1) } >rom
  .rodata    : { *(.rodata*) } >rom
  __FIXUP_START__ = .;
  .fixup     : { *(.fixup)   } >rom
  __FIXUP_END__ = .;
  __EXCEPT_START__ =.;
  .gcc_except_table : { *(.gcc_except_table) } >rom
  __EXCEPT_END__ =.;
  // Read-write section, merged into data segment:
  __rom_data_start = ALIGN(4);
  .data : 
  AT( __rom_data_start )
  {
    __ram_data_start = .;
    PROVIDE( __ram_data_start = .);

    *(.data*)
    __GOT1_START__ = .;
    *(.got1) 
    __GOT1_END__ = .;
    // Put .ctors and .dtors next to the .got2 section, so that the pointers
    // get relocated with -mrelocatable.
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
    *(.dtors*)
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
  } >ram
  __ram_data_end = .;
  PROVIDE( __ram_data_end = .);
  _edata  =  .;
  PROVIDE (edata = .);
  __bss_start = .;
  .sbss      :
  {
        __SBSS_START__ = .;
        *(.sbss*)
        __SBSS_END__ = .;
        __SBSS2_START__ = .;
        *(.sbss2)
        __SBSS2_END__ = .;
        *(.scommon)
  } >ram

  .bss       :
  {
   *(.dynbss)
   *(.bss)
   *(COMMON)
  } >ram
  _end = . ;
  PROVIDE (end = .);


  __rom_ram_vectors_start = ALIGN(4);
#if 1
  .vectors :
  {
    *(.vectors)
  } > ram_vectors
#else
  .ram_vectors   :
  AT ( __rom_ram_vectors_start )
  {
    __ram_vectors_start = .;
    *(.ram_vectors)
    __ram_vectors_end = .;
  } >ram_vectors

  .rom_vectors   :
  {
    *(.rom_vectors)
  } >rom_vectors
#endif

}

