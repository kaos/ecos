//==========================================================================
//
//	powerpc.ld
//
//	Linker script for PowerPC
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2004, 2005, 2008 eCosCentric Limited
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	jskov
// Contributors:jskov
// Date:	1999-01-18
// Purpose:	PowerPC Linker script
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

STARTUP(vectors.o)
ENTRY(__exception_reset)
#ifdef EXTRAS
INPUT(extras.o)
#endif
GROUP( CYGBLD_HAL_LINKER_GROUPED_LIBS )

#define ALIGN_LMA 8
#define FOLLOWING(_section_) AT ((LOADADDR (_section_) + SIZEOF (_section_) + ALIGN_LMA - 1) & ~ (ALIGN_LMA - 1))
#define LMA_EQ_VMA
#define FORCE_OUTPUT . = .


#define SECTIONS_BEGIN

#define SECTION_RELOCS(_region_, _vma_, _lma_)                          \
  .rel.dyn        :                                                     \
    {                                                                   \
      *(.rel.init)                                                      \
      *(.rel.text .rel.text.* .rel.gnu.linkonce.t.*)                    \
      *(.rel.fini)                                                      \
      *(.rel.rodata .rel.rodata.* .rel.gnu.linkonce.r.*)                \
      *(.rel.data.rel.ro*)                                              \
      *(.rel.data .rel.data.* .rel.gnu.linkonce.d.*)                    \
      *(.rel.tdata .rel.tdata.* .rel.gnu.linkonce.td.*)                 \
      *(.rel.tbss .rel.tbss.* .rel.gnu.linkonce.tb.*)                   \
      *(.rel.ctors)                                                     \
      *(.rel.dtors)                                                     \
      *(.rel.got)                                                       \
      *(.rel.sdata .rel.sdata.* .rel.gnu.linkonce.s.*)                  \
      *(.rel.sbss .rel.sbss.* .rel.gnu.linkonce.sb.*)                   \
      *(.rel.sdata2 .rel.sdata2.* .rel.gnu.linkonce.s2.*)               \
      *(.rel.sbss2 .rel.sbss2.* .rel.gnu.linkonce.sb2.*)                \
      *(.rel.bss .rel.bss.* .rel.gnu.linkonce.b.*)                      \
    } > _region_                                                        \
  .rela.dyn       :                                                     \
    {                                                                   \
      *(.rela.init)                                                     \
      *(.rela.text .rela.text.* .rela.gnu.linkonce.t.*)                 \
      *(.rela.fini)                                                     \
      *(.rela.rodata .rela.rodata.* .rela.gnu.linkonce.r.*)             \
      *(.rela.data .rela.data.* .rela.gnu.linkonce.d.*)                 \
      *(.rela.tdata .rela.tdata.* .rela.gnu.linkonce.td.*)              \
      *(.rela.tbss .rela.tbss.* .rela.gnu.linkonce.tb.*)                \
      *(.rela.ctors)                                                    \
      *(.rela.dtors)                                                    \
      *(.rela.got)                                                      \
      *(.rela.got1)                                                     \
      *(.rela.got2)                                                     \
      *(.rela.sdata .rela.sdata.* .rela.gnu.linkonce.s.*)               \
      *(.rela.sbss .rela.sbss.* .rela.gnu.linkonce.sb.*)                \
      *(.rela.sdata2 .rela.sdata2.* .rela.gnu.linkonce.s2.*)            \
      *(.rela.sbss2 .rela.sbss2.* .rela.gnu.linkonce.sb2.*)             \
      *(.rela.bss .rela.bss.* .rela.gnu.linkonce.b.*)                   \
    } > _region_                                                        \
  .rel.plt        : { *(.rel.plt) } > _region_                          \
  .rela.plt       : { *(.rela.plt) } > _region_

#define SECTION_vectors(_region_, _vma_, _lma_)        \
  .vectors _vma_ : _lma_                                              \
  { FORCE_OUTPUT; KEEP(*(.vectors)) }         \
  > _region_

#define SECTION_text(_region_, _vma_, _lma_)           \
  .text _vma_ : _lma_                                                 \
  { _stext = .;       \
    *(.text*) *(.gnu.warning) *(.gnu.linkonce.t.*) *(.init) }      \
  > _region_                                                    \
  _etext = .;  PROVIDE (etext = .);

#define SECTION_fini(_region_, _vma_, _lma_)   \
    .fini _vma_ : _lma_                                       \
    { FORCE_OUTPUT; *(.fini) }        \
    > _region_

#define SECTION_rodata1(_region_, _vma_, _lma_)        \
    .rodata1 _vma_ : _lma_                                            \
    { FORCE_OUTPUT; *(.rodata1*) }             \
    > _region_

#define SECTION_rodata(_region_, _vma_, _lma_)          \
    .rodata _vma_ : _lma_                               \
    { FORCE_OUTPUT; *(.rodata*) *(.gnu.linkonce.r.*)    \
    } > _region_

#define SECTION_fixup(_region_, _vma_, _lma_)                      \
    .fixup _vma_ : _lma_                                                          \
    { __FIXUP_START__ = ABSOLUTE(.); *(.fixup) __FIXUP_END__ = ABSOLUTE(.);}\
    > _region_

/* We want to discard C++ exception frame data if we don't have the
 * C++ support to use it. But we can't. With current binutils (2008-05-13)
 * the linker goes very wrong if either of the contents of
 * .gcc_except_table or .eh_frame are discarded. Yes I tried.
 */
#define SECTION_gcc_except_table(_region_, _vma_, _lma_)       \
    .gcc_except_table _vma_ : _lma_                            \
    { __EXCEPT_START__ = ABSOLUTE(.);                          \
      *(.gcc_except_table)                                     \
      *(.gcc_except_table.*)                                   \
      __EXCEPT_END__ = ABSOLUTE(.);                            \
    } > _region_

#define SECTION_eh_frame(_region_, _vma_, _lma_)      \
  .eh_frame _vma_ : _lma_                             \
    {                                                 \
       *(.eh_frame_hdr)                               \
       FORCE_OUTPUT;  __EH_FRAME_BEGIN__ = .;         \
       KEEP(*(.eh_frame))                             \
       __FRAME_END__ = .;                             \
       . = . + 8;                                     \
    } > _region_ = 0

#define SECTION_data(_region_, _vma_, _lma_)                         \
    .data _vma_ : _lma_                                                             \
    { __ram_data_start = ABSOLUTE(.); *(.data*)                                 \
      *(.gnu.linkonce.d.*) \
    __GOT1_START__ = ABSOLUTE(.); *(.got1) __GOT1_END__ = ABSOLUTE(.);        \
      /* Put .ctors and .dtors next to the .got2 section, so that */          \
      /* the pointers get relocated with -mrelocatable.           */          \
     . = ALIGN(8); __CTOR_LIST__ = ABSOLUTE(.);                               \
      KEEP(*(SORT(.ctors*))) __CTOR_END__ = ABSOLUTE(.);                      \
    __DTOR_LIST__ = ABSOLUTE(.);                                              \
      KEEP(*(SORT(.dtors*))) __DTOR_END__ = ABSOLUTE(.);                      \
      . = ALIGN(8);                                                           \
      KEEP(*( SORT (.ecos.table.*))) ;                                        \
      . = ALIGN(4);                                                           \
      *( .2ram.*) ;                                                           \
    __GOT2_START__ = ABSOLUTE(.); *(.got2) __GOT2_END__ = ABSOLUTE(.);        \
    __GOT_START = ABSOLUTE(.);                                                \
    _SDA_BASE_ = ABSOLUTE(.); *(.got.plt) *(.got)                             \
    __GOT_END__ = ABSOLUTE(.);  *(.dynamic)                                   \
      /* We want the small data sections together, so single-instruction */   \
      /* offsets can access them all, and initialized data all before    */   \
      /* uninitialized, so we can shorten the on-disk segment size.      */   \
    __SDATA_START__ = ABSOLUTE(.); *(.sdata) *(.sdata.*) *(.gnu.linkonce.s.*) \
    __SDATA2_START__ = ABSOLUTE(.); *(.sdata2*) }                             \
    > _region_                                                                \
    __rom_data_start = LOADADDR(.data);                                       \
    __ram_data_end = .; PROVIDE(__ram_data_end = .);                          \
    _edata = .; PROVIDE (edata = .);

#define SECTION_sbss(_region_, _vma_, _lma_)                     \
    .sbss _vma_ : _lma_                                                         \
    { __sbss_start = ABSOLUTE (.);                                        \
    __SBSS_START__ = ABSOLUTE(.); *(.sbss.*) __SBSS_END__ = ABSOLUTE(.);   \
    __SBSSx_START__ = ABSOLUTE(.); *(.sbss*) __SBSSx_END__ = ABSOLUTE(.);\
     *(.gnu.linkonce.sb.*) *(.scommon*)                                       \
    __sbss_end = ABSOLUTE (.); }                                          \
    > _region_

#define SECTION_bss(_region_, _vma_, _lma_)                      \
    .bss _vma_ : _lma_                                                          \
    { __bss_start = ABSOLUTE (.);                       \
      FORCE_OUTPUT; *(.dynbss*) *(.bss*) *(.gnu.linkonce.b.*) *(COMMON)      \
      __bss_end = ABSOLUTE (.); }                       \
    > _region_

#define SECTIONS_END . = ALIGN(4); _end = .; PROVIDE (end = .);

#include <pkgconf/system.h>
#include CYGHWR_MEMORY_LAYOUT_LDI

// Define VSR and virtual tables to reside at fixed addresses.
#include CYGBLD_HAL_TARGET_H
hal_vsr_table = CYGHWR_HAL_VSR_TABLE;
hal_virtual_vector_table = CYGHWR_HAL_VIRTUAL_VECTOR_TABLE;
