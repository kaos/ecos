//==========================================================================
//
//	i386.ld
//
//	Linker script for i386
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
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	jskov
// Contributors:jskov
// Date:	1999-01-18
// Purpose:	i386 Linker script
//
//####DESCRIPTIONEND####
//
//==========================================================================
STARTUP(vectors.o)
ENTRY(_start)
#ifdef EXTRAS
INPUT(extras.o)
#endif
GROUP(libtarget.a libgcc.a)

#define ALIGN_LMA 8
#define FOLLOWING(_section_) AT ((LOADADDR (_section_) + SIZEOF (_section_) + ALIGN_LMA - 1) & ~ (ALIGN_LMA - 1))
#define LMA_EQ_VMA
#define FORCE_OUTPUT . = .


#define SECTIONS_BEGIN

#define SECTION_vectors(_region_, _vma_, _lma_)        \
  .vectors _vma_ : _lma_                                              \
  { FORCE_OUTPUT; KEEP(*(.vectors)) }         \
  > _region_

#define SECTION_text(_region_, _vma_, _lma_)           \
  .text _vma_ : _lma_                                                 \
  { _stext = .;                 \
    *(.text*) *(.gnu.warning) *(.gnu.linkonce*) *(.init) }      \
  > _region_                                                    \
  _etext = .;  PROVIDE (etext = .);

#define SECTION_fini(_region_, _vma_, _lma_)   \
    .fini _vma_ : _lma_                                       \
    { FORCE_OUTPUT; *(.fini) }        \
    > _region_

#define SECTION_rodata1(_region_, _vma_, _lma_)        \
    .rodata1 _vma_ : _lma_                                            \
    { FORCE_OUTPUT; *(.rodata1) }             \
    > _region_

#define SECTION_rodata(_region_, _vma_, _lma_)                 \
    .rodata _vma_ : _lma_                                                     \
    { FORCE_OUTPUT; *(.rodata*) } \
    > _region_

#define SECTION_fixup(_region_, _vma_, _lma_)                   \
    .fixup _vma_ : _lma_                                                       \
    { _FIXUP_START_ = ABSOLUTE(.); *(.fixup) _FIXUP_END_ = ABSOLUTE(.);} \
    > _region_

#define SECTION_gcc_except_table(_region_, _vma_, _lma_)       \
    .gcc_except_table _vma_ : _lma_                                           \
    { _EXCEPT_START_ = ABSOLUTE(.); *(.gcc_except_table)                \
      _EXCEPT_END_ = ABSOLUTE(.);}                                      \
    > _region_

// Note: The __ in the name is an encoding of the .
#define SECTION_rel__got(_region_, _vma_, _lma_)        \
    .rel.got _vma_ : _lma_                                            \
    { *(.rel.got)      }                                        \
    > _region_

#define SECTION_data(_region_, _vma_, _lma_)                       \
    .data _vma_ : _lma_                                                           \
    { __ram_data_start = ABSOLUTE(.); *(.data*)                               \
    _GOT1_START_ = ABSOLUTE(.); *(.got1) _GOT1_END_ = ABSOLUTE(.);          \
      /* Put .ctors and .dtors next to the .got2 section, so that */        \
      /* the pointers get relocated with -mrelocatable.           */        \
     . = ALIGN(8); __CTOR_LIST__ = ABSOLUTE(.);                             \
      KEEP(*(SORT(.ctors*))) __CTOR_END__ = ABSOLUTE(.);                    \
    __DTOR_LIST__ = ABSOLUTE(.);                                            \
      KEEP(*(SORT(.dtors*))) __DTOR_END__ = ABSOLUTE(.);                    \
    . = ALIGN(8);                                                           \
    __DEVTAB__ = ABSOLUTE (.); KEEP (*(SORT (.devtab*))) __DEVTAB_END__ = ABSOLUTE (.); \
    _GOT2_START_ = ABSOLUTE(.); *(.got2) _GOT2_END_ = ABSOLUTE(.);          \
    _GOT_START_ = ABSOLUTE(.); _GLOBAL_OFFSET_TABLE_ = ABSOLUTE(. + 32768); \
    _SDA_BASE_ = ABSOLUTE(.); *(.got.plt) *(.got)                           \
    _GOT_END_ = ABSOLUTE(.);  *(.dynamic) *(.eh_frame)                      \
      /* We want the small data sections together, so single-instruction */ \
      /* offsets can access them all, and initialized data all before    */ \
      /* uninitialized, so we can shorten the on-disk segment size.      */ \
    _SDATA_START_ = ABSOLUTE(.); *(.sdata) }                                \
    > _region_                                                              \
    __rom_data_start = LOADADDR(.data);                                     \
    __ram_data_end = .; PROVIDE(__ram_data_end = .);                        \
    _edata = .; PROVIDE (edata = .);

#define SECTION_sbss(_region_, _vma_, _lma_)                   \
    .sbss _vma_ : _lma_                                                       \
    { __bss_start = ABSOLUTE (.);       \
    _SBSS_START_ = ABSOLUTE(.); *(.sbss) _SBSS_END_ = ABSOLUTE(.);      \
    *(.scommon) }                                                       \
    > _region_

#define SECTION_bss(_region_, _vma_, _lma_)                      \
    .bss _vma_ : _lma_                                                          \
    { FORCE_OUTPUT; *(.dynbss) *(.bss) *(COMMON) }      \
    > _region_

#define SECTIONS_END . = ALIGN(4); _end = .; PROVIDE (end = .);

#include <pkgconf/system.h>
#include CYGHWR_MEMORY_LAYOUT_LDI
