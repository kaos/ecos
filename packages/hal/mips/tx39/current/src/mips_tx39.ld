//===========================================================================
//
// MLT linker script for MIPS TX39
//
//===========================================================================
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
//===========================================================================

#include <pkgconf/system.h>

STARTUP(vectors.o)
ENTRY(reset_vector)
#ifdef EXTRAS
INPUT(extras.o)
#endif
GROUP(libtarget.a libgcc.a)

#define ALIGN_LMA 8
#define FOLLOWING(_section_) AT ((LOADADDR (_section_) + SIZEOF (_section_) + ALIGN_LMA - 1) & ~ (ALIGN_LMA - 1))
#define LMA_EQ_VMA
#define FORCE_OUTPUT . = .

#define SECTIONS_BEGIN

#if defined(CYG_HAL_STARTUP_RAM)

/* this version for RAM startup */
#define SECTION_rom_vectors(_region_, _vma_, _lma_) \
    .rom_vectors _vma_ : _lma_ \
    { KEEP (*(.utlb_vector)) \
    . = ALIGN(0x80); KEEP(*(.other_vector)) \
	/* debug and reset vector not used in RAM version */ \
        KEEP(*(.debug_vector)) \
	KEEP (*(.reset_vector)) } \
    > _region_

#elif defined(CYG_HAL_STARTUP_ROM)

/* this version for ROM startup */
#define SECTION_rom_vectors(_region_, _vma_, _lma_) \
    .rom_vectors _vma_ : _lma_ \
    { KEEP (*(.reset_vector)) \
    . = ALIGN(0x100); KEEP (*(.utlb_vector)) \
    . = ALIGN(0x80); KEEP(*(.other_vector)) \
    . = ALIGN(0x100); KEEP(*(.debug_vector)) } \
    > _region_

#endif /* ROM startup version of ROM vectors */

#define SECTION_text(_region_, _vma_, _lma_) \
    .text _vma_ : _lma_ \
    { _stext = ABSOLUTE(.); \
    *(.text*) *(.gnu.warning) *(.gnu.linkonce*) *(.init) } \
    > _region_ \
    _etext = .; PROVIDE (etext = .);

#define SECTION_fini(_region_, _vma_, _lma_) \
    .fini _vma_ : _lma_ \
    { FORCE_OUTPUT; *(.fini) } \
    > _region_

#define SECTION_ctors(_region_, _vma_, _lma_)	\
   .ctors _vma_ : _lma_				\
   {						\
     __CTOR_LIST__ = ABSOLUTE (.);		\
     KEEP(*(.ctors))				\
     KEEP(*(SORT(.ctors.*)))			\
     __CTOR_END__ = ABSOLUTE (.);		\
   } > _region_

#define SECTION_dtors(_region_, _vma_, _lma_)	\
   .dtors _vma_ : _lma_				\
   {						\
     __DTOR_LIST__ = ABSOLUTE (.);		\
     KEEP(*(SORT(.dtors.*)))			\
     KEEP(*(.dtors))				\
     __DTOR_END__ = ABSOLUTE (.);		\
   } > _region_

#define SECTION_rodata(_region_, _vma_, _lma_) \
    .rodata _vma_ : _lma_ \
    { FORCE_OUTPUT; *(.rodata*) } \
    > _region_

#define SECTION_vsr_table(_region_, _vma_, _lma_) \
    .vsr_table _vma_ : _lma_ \
    { FORCE_OUTPUT; *(.vsr_table) } \
    > _region_

#define SECTION_rodata1(_region_, _vma_, _lma_) \
    .rodata1 _vma_ : _lma_ \
    { FORCE_OUTPUT; *(.rodata1) } \
    > _region_

#define SECTION_fixup(_region_, _vma_, _lma_) \
    .fixup _vma_ : _lma_ \
    { FORCE_OUTPUT; *(.fixup) } \
    > _region_

#define SECTION_rel__dyn(_region_, _vma_, _lma_) \
    .rel.dyn _vma_ : _lma_ \
    { FORCE_OUTPUT; *(.rel.dyn) } \
    > _region_

#define SECTION_gcc_except_table(_region_, _vma_, _lma_) \
    .gcc_except_table _vma_ : _lma_ \
    { FORCE_OUTPUT; *(.gcc_except_table) } \
    > _region_

/* FIXME: The SORT(CONSTRUCTORS) line below is for backwards compatibility
 * with older (pre-99r1) toolchains
 */
#define SECTION_data(_region_, _vma_, _lma_) \
    .data _vma_ : _lma_ \
    { __ram_data_start = ABSOLUTE (.); \
    *(.data*) *(.data1) \
    _GOT1_START_ = ABSOLUTE (.); *(.got1) _GOT1_END_ = ABSOLUTE (.); \
    _GOT2_START_ = ABSOLUTE (.); *(.got2) _GOT2_END_ = ABSOLUTE (.); \
    . = ALIGN (8); \
    SORT (CONSTRUCTORS) \
    . = ALIGN (8); \
    __DEVTAB__ = ABSOLUTE (.); KEEP (*(SORT (.devtab*))) __DEVTAB_END__ = ABSOLUTE (.); \
    . = ALIGN (8); \
    _GOT_START = ABSOLUTE (.); _gp = ABSOLUTE (.); __global = _gp; _GLOBAL_OFFSET_TABLE_ = ABSOLUTE (.); _SDA_BASE_ = ABSOLUTE (.); \
    *(.got.plt) *(.got) _GOT_END_ = ABSOLUTE (.); \
    *(.dynamic) *(.sdata*) *(.sbss*) *(.eh_frame) } \
    > _region_ \
    __rom_data_start = LOADADDR (.data); \
    __ram_data_end = .; PROVIDE (__ram_data_end = .); _edata = .; PROVIDE (edata = .);
   
#define SECTION_bss(_region_, _vma_, _lma_) \
    .bss _vma_ : _lma_ \
    { __bss_start = ABSOLUTE (.); \
    *(.scommon) *(.dynbss) *(.bss) *(COMMON) \
    __bss_end = ABSOLUTE (.); } \
    > _region_

#define SECTIONS_END . = ALIGN(4); _end = .; PROVIDE (end = .); \
    .debug          0 : { *(.debug) } \
    .line           0 : { *(.line) } \
    .debug_srcinfo  0 : { *(.debug_srcinfo) } \
    .debug_sfnames  0 : { *(.debug_sfnames) } \
    .debug_aranges  0 : { *(.debug_aranges) } \
    .debug_pubnames 0 : { *(.debug_pubnames) } \
    .debug_info     0 : { *(.debug_info) } \
    .debug_abbrev   0 : { *(.debug_abbrev) } \
    .debug_line     0 : { *(.debug_line) } \
    .debug_frame    0 : { *(.debug_frame) } \
    .debug_str      0 : { *(.debug_str) } \
    .debug_loc      0 : { *(.debug_loc) } \
    .debug_macinfo  0 : { *(.debug_macinfo) } \
    .debug_weaknames 0 : { *(.debug_weaknames) } \
    .debug_funcnames 0 : { *(.debug_funcnames) } \
    .debug_typenames 0 : { *(.debug_typenames) } \
    .debug_varnames  0 : { *(.debug_varnames) }

#include CYGHWR_MEMORY_LAYOUT_LDI

#ifndef CYGPKG_HAL_MIPS_SIM
hal_vsr_table = 0x80000100;
#endif
