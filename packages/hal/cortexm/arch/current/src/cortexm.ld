//=============================================================================
//
// MLT linker script for Cortex-M
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2011 Free Software Foundation, Inc.                        
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//=============================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

STARTUP(vectors.o)
ENTRY(reset_vector)
INPUT(extras.o)
GROUP(libtarget.a libgcc.a libsupc++.a)

#if defined(__ARMEB__)
OUTPUT_FORMAT(elf32-bigarm)
#endif

// ALIGN_LMA is 4, but the AAPCS now requires that double word types
// be aligned to 8, which means some input sections can be marked as needing
// 8-byte alignment, even text ones (consider literal data).

#define ALIGN_LMA 4
#define AAPCS_ALIGN 8
#define FOLLOWING_ALIGNED(_section_, _align_) AT ((LOADADDR (_section_) + SIZEOF (_section_) + (_align_) - 1) & ~ ((_align_) - 1))
#define FOLLOWING(_section_) FOLLOWING_ALIGNED(_section_, ALIGN_LMA)

#define LMA_EQ_VMA
#define FORCE_OUTPUT . = .

// Some versions of gcc define "arm" which causes problems with .note.arm.ident
#undef arm
#define SECTIONS_BEGIN                          \
  /* Debug information */                       \
  .debug_aranges  0 : { *(.debug_aranges) }     \
  .debug_pubnames 0 : { *(.debug_pubnames) }    \
  .debug_info     0 : { *(.debug_info) }        \
  .debug_abbrev   0 : { *(.debug_abbrev) }      \
  .debug_line     0 : { *(.debug_line) }        \
  .debug_frame    0 : { *(.debug_frame) }       \
  .debug_str      0 : { *(.debug_str) }         \
  .debug_loc      0 : { *(.debug_loc) }         \
  .debug_macinfo  0 : { *(.debug_macinfo) }     \
  .note.arm.ident 0 : { KEEP (*(.note.arm.ident)) } \
  /DISCARD/       0 : { *(.fini_array*) }


// Following not used unless CYGHWR_HAL_ARM_SEPARATE_VSR_TABLE
// defined in hal_platform_setup.h. Otherwise vsr table
// goes in .fixed_vectors.
#define SECTION_hal_vsr_table(_region_, _vma_, _lma_) \
    .hal_vsr_table _vma_ : _lma_ \
    { FORCE_OUTPUT; KEEP (*(.hal_vsr_table)) } \
    > _region_

#define SECTION_fixed_vectors(_region_, _vma_, _lma_) \
    .fixed_vectors _vma_ : _lma_ \
    { FORCE_OUTPUT; KEEP (*(.fixed_vectors)) } \
    > _region_

#define SECTION_rom_vectors(_region_, _vma_, _lma_) \
    .rom_vectors _vma_ : _lma_ \
    { __rom_vectors_vma = ABSOLUTE(.); \
      FORCE_OUTPUT; KEEP (*(.vectors)) } \
    > _region_ \
    __rom_vectors_lma = LOADADDR(.rom_vectors);

// We slot in the .ARM.extab and .ARM.exidx sections first. They
// need to be close to .text due to their relocations which may
// want small offsets - .rodata may be somewhere very different.
// This approach also allows forward compatibility with targets
// which haven't used the EABI before (no new SECTION_xxx definition
// to use).
// One glitch is that the AAPCS can require up to 8-byte alignment
// for double word types. For sections after the first (for which we
// "trust" the MLT files (probably a mistake)), we need to
// ensure the subsequent sections' VMA and LMA move in sync, which
// means keeping the same alignment, and thus since for example literal
// data in .text can require up to 8 byte alignment, the LMA must also
// be 8 byte aligned.
#define SECTION_text(_region_, _vma_, _lma_) \
    .ARM.extab _vma_ : _lma_ \
    { PROVIDE (__stext = ABSOLUTE(.));_stext = ABSOLUTE(.) ; \
    FORCE_OUTPUT; \
    *(.ARM.extab* .gnu.linkonce.armextab.*) } > _region_ \
    . = ALIGN(AAPCS_ALIGN); \
    __exidx_start = ABSOLUTE(.); \
    .ARM.exidx ALIGN(AAPCS_ALIGN) : FOLLOWING_ALIGNED(.ARM.extab, AAPCS_ALIGN) { \
    *(.ARM.exidx* .gnu.linkonce.armexidx.*) \
    FORCE_OUTPUT; \
    } > _region_ \
    __exidx_end = ABSOLUTE(.);\
    .text ALIGN(AAPCS_ALIGN) : FOLLOWING_ALIGNED(.ARM.exidx, AAPCS_ALIGN) \
    { \
    *(.text*) *(.gnu.warning) *(.gnu.linkonce.t.*) *(.init) \
    *(.glue_7) *(.glue_7t)  \
    __CTOR_LIST__ = ABSOLUTE (.); KEEP (*(SORT (.ctors*))) __CTOR_END__ = ABSOLUTE (.); \
    __DTOR_LIST__ = ABSOLUTE (.); KEEP (*(SORT (.dtors*))) __DTOR_END__ = ABSOLUTE (.); \
    } > _region_ \
    _etext = .; PROVIDE (__etext = .);

#define SECTION_fini(_region_, _vma_, _lma_) \
    .fini _vma_ : _lma_ \
    { FORCE_OUTPUT; *(.fini) } \
    > _region_

#define SECTION_rodata(_region_, _vma_, _lma_) \
    .rodata _vma_ : _lma_ \
    { FORCE_OUTPUT; *(.rodata*) *(.gnu.linkonce.r.*) } \
    > _region_

#define SECTION_rodata1(_region_, _vma_, _lma_) \
    .rodata1 _vma_ : _lma_ \
    { FORCE_OUTPUT; *(.rodata1) } \
    > _region_

#define SECTION_fixup(_region_, _vma_, _lma_) \
    .fixup _vma_ : _lma_ \
    { FORCE_OUTPUT; *(.fixup) } \
    > _region_

#define SECTION_gcc_except_table(_region_, _vma_, _lma_)      \
    .gcc_except_table _vma_ : _lma_                           \
    { FORCE_OUTPUT;                                           \
      KEEP(*(.gcc_except_table))                              \
      *(.gcc_except_table.*)                                  \
    } > _region_

#define SECTION_eh_frame(_region_, _vma_, _lma_)      \
  .eh_frame _vma_ : _lma_                             \
    {                                                 \
       FORCE_OUTPUT;  __EH_FRAME_BEGIN__ = .;         \
       KEEP(*(.eh_frame))                             \
       __FRAME_END__ = .;                             \
       . = . + 8;                                     \
    } > _region_ = 0

#define SECTION_RELOCS(_region_, _vma_, _lma_)                              \
  .rel.text      :                                                          \
    {                                                                       \
      *(.rel.text)                                                          \
      *(.rel.text.*)                                                        \
      *(.rel.gnu.linkonce.t*)                                               \
    } > _region_                                                            \
  .rela.text     :                                                          \
    {                                                                       \
      *(.rela.text)                                                         \
      *(.rela.text.*)                                                       \
      *(.rela.gnu.linkonce.t*)                                              \
    } > _region_                                                            \
  .rel.data      :                                                          \
    {                                                                       \
      *(.rel.data)                                                          \
      *(.rel.data.*)                                                        \
      *(.rel.gnu.linkonce.d*)                                               \
    } > _region_                                                            \
  .rela.data     :                                                          \
    {                                                                       \
      *(.rela.data)                                                         \
      *(.rela.data.*)                                                       \
      *(.rela.gnu.linkonce.d*)                                              \
    } > _region_                                                            \
  .rel.rodata    :                                                          \
    {                                                                       \
      *(.rel.rodata)                                                        \
      *(.rel.rodata.*)                                                      \
      *(.rel.gnu.linkonce.r*)                                               \
    } > _region_                                                            \
  .rela.rodata   :                                                          \
    {                                                                       \
      *(.rela.rodata)                                                       \
      *(.rela.rodata.*)                                                     \
      *(.rela.gnu.linkonce.r*)                                              \
    } > _region_                                                            \
  .rel.got       :   { *(.rel.got)    } > _region_                          \
  .rela.got      :   { *(.rela.got)   } > _region_                          \
  .rel.ctors     :   { *(.rel.ctors)  } > _region_                          \
  .rela.ctors    :   { *(.rela.ctors) } > _region_                          \
  .rel.dtors     :   { *(.rel.dtors)  } > _region_                          \
  .rela.dtors    :   { *(.rela.dtors) } > _region_                          \
  .rel.init      :   { *(.rel.init)   } > _region_                          \
  .rela.init     :   { *(.rela.init)  } > _region_                          \
  .rel.fini      :   { *(.rel.fini)   } > _region_                          \
  .rela.fini     :   { *(.rela.fini)  } > _region_                          \
  .rel.bss       :   { *(.rel.bss)    } > _region_                          \
  .rela.bss      :   { *(.rela.bss)   } > _region_                          \
  .rel.plt       :   { *(.rel.plt)    } > _region_                          \
  .rela.plt      :   { *(.rela.plt)   } > _region_                          \
  .rel.dyn       :   { *(.rel.dyn)    } > _region_

#define SECTION_got(_region_, _vma_, _lma_)                                 \
  .got _vma_ : _lma_                                                        \
    {                                                                       \
      FORCE_OUTPUT; *(.got.plt) *(.got)                                     \
      _GOT1_START_ = ABSOLUTE (.); *(.got1) _GOT1_END_ = ABSOLUTE (.);      \
      _GOT2_START_ = ABSOLUTE (.); *(.got2) _GOT2_END_ = ABSOLUTE (.);      \
    } > _region_

#define SECTION_mmu_tables(_region_, _vma_, _lma_) \
    .mmu_tables _vma_ : _lma_ \
    { FORCE_OUTPUT; *(.mmu_tables) } \
    > _region_

#define SECTION_sram(_region_, _vma_, _lma_) \
    .sram _vma_ : _lma_ \
    { __sram_data_start = ABSOLUTE (.); \
    *(.sram*) . = ALIGN (4); } \
    > _region_ \
    __srom_data_start = LOADADDR (.sram); \
    __sram_data_end = .; PROVIDE (__sram_data_end = .); \
    PROVIDE (__srom_data_end = LOADADDR (.sram) + SIZEOF(.sram));

#define SECTION_data(_region_,  _vma_, _lma_) \
    .data _vma_ : _lma_ \
    { __ram_data_start = ABSOLUTE (.); \
    *(.data*) *(.data1) *(.gnu.linkonce.d.*) \
    . = ALIGN (4); \
    KEEP(*( SORT (.ecos.table.*))) ;            \
    . = ALIGN (4); \
    __init_array_start__ = ABSOLUTE (.); KEEP (*(SORT (.init_array.*))) \
    KEEP (*(SORT (.init_array))) __init_array_end__ = ABSOLUTE (.); \
    *(.dynamic) *(.sdata*) *(.gnu.linkonce.s.*) \
    . = ALIGN (4); *(.2ram.*) } \
    > _region_ \
    __rom_data_start = LOADADDR (.data); \
    __ram_data_end = .; PROVIDE (__ram_data_end = .); _edata = .; PROVIDE (edata = .); \
    PROVIDE (__rom_data_end = LOADADDR (.data) + SIZEOF(.data));

#define SECTION_bss(_region_,  _vma_, _lma_) \
    .bss _vma_ : _lma_ \
    { __bss_start = ABSOLUTE (.); \
    *(.scommon) *(.dynsbss) *(.sbss*) *(.gnu.linkonce.sb.*) \
    *(.dynbss) *(.bss*) *(.gnu.linkonce.b.*) *(COMMON) \
    __bss_end = ABSOLUTE (.); } \
    > _region_

#define USER_SECTION(_name_, _region_,  _vma_, _lma_) \
    ._name_ _vma_ : _lma_ \
    { __ ## _name_ ## _start = ABSOLUTE (.); \
     *(._name_*) \
    __ ## _name_ ## _end = ABSOLUTE (.); } \
    > _region_

#define SECTIONS_END . = ALIGN(4); _end = .; PROVIDE (end = .);

#include <pkgconf/hal_cortexm.h>
#include CYGHWR_MEMORY_LAYOUT_LDI
