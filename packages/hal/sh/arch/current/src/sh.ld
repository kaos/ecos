/*==========================================================================
//
//	sh.ld
//
//	Linker script for SH
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	jskov
// Contributors:jskov
// Date:	1999-04-24
// Purpose:	SH Linker script
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/system.h>
#include <cyg/hal/basetype.h>

STARTUP(vectors.o)
ENTRY(CYG_LABEL_DEFN(_reset))
INPUT(extras.o)
#if (__GNUC__ >= 3)
GROUP(libtarget.a libgcc.a libsupc++.a)
#else
GROUP(libtarget.a libgcc.a)
#endif

#define ALIGN_LMA 16
#define FOLLOWING(_section_) AT ((LOADADDR (_section_) + SIZEOF (_section_) + ALIGN_LMA - 1) & ~ (ALIGN_LMA - 1))
#define LMA_EQ_VMA
#define FORCE_OUTPUT . = .


# define SECTIONS_BEGIN

# define SECTION_vectors(_region_, _vma_, _lma_)		\
  .vectors _vma_ : _lma_					\
  { FORCE_OUTPUT; KEEP(*(.vectors))				\
  }								\
  > _region_							\
  CYG_LABEL_DEFN(_vector_code_lma) = LOADADDR(.vectors);

# define SECTION_text(_region_, _vma_, _lma_)			\
  .text _vma_ : _lma_						\
  { CYG_LABEL_DEFN(_stext) = .;					\
    *(.text*) *(.gnu.warning) *(.gnu.linkonce*) *(.init) }	\
  > _region_							\
  CYG_LABEL_DEFN(_etext) = .;  PROVIDE (__etext = .);

# define SECTION_data(_region_, _vma_, _lma_)				 \
    .data _vma_ : _lma_							 \
    { CYG_LABEL_DEFN(_ram_data_start) = ABSOLUTE(.); *(.data*)		 \
      *( .2ram.*) ;                                                      \
     . = ALIGN(8); CYG_LABEL_DEFN(__CTOR_LIST__) = ABSOLUTE(.);		 \
      KEEP(*(SORT(.ctors*))) CYG_LABEL_DEFN(__CTOR_END__) = ABSOLUTE(.); \
    CYG_LABEL_DEFN(__DTOR_LIST__) = ABSOLUTE(.);			 \
      KEEP(*(SORT(.dtors*))) CYG_LABEL_DEFN(__DTOR_END__) = ABSOLUTE(.); \
    . = ALIGN(8);							 \
      KEEP(*( SORT (.ecos.table.*))) ;					 \
    . = ALIGN(4);                                                        \
    } > _region_							 \
    CYG_LABEL_DEFN(_rom_data_start) = LOADADDR(.data);			 \
    CYG_LABEL_DEFN(_ram_data_end) = .; PROVIDE(_ram_data_end = .);	 \
    CYG_LABEL_DEFN(edata) = .; PROVIDE (edata = .);


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
    } > _region_

#define SECTION_fini(_region_, _vma_, _lma_)	\
    .fini _vma_ : _lma_				\
    { FORCE_OUTPUT; *(.fini) }			\
    > _region_

#define SECTION_rodata1(_region_, _vma_, _lma_)	\
    .rodata1 _vma_ : _lma_			\
    { FORCE_OUTPUT; *(.rodata1) }		\
    > _region_

#define SECTION_rodata(_region_, _vma_, _lma_)	\
    .rodata _vma_ : _lma_			\
    { FORCE_OUTPUT; *(.rodata*) }		\
    > _region_

#define SECTION_fixup(_region_, _vma_, _lma_)				     \
    .fixup _vma_ : _lma_						     \
    { __FIXUP_START__ = ABSOLUTE(.); *(.fixup) __FIXUP_END__ = ABSOLUTE(.);} \
    > _region_

#define SECTION_gcc_except_table(_region_, _vma_, _lma_)	\
    .gcc_except_table _vma_ : _lma_				\
    { __EXCEPT_START__ = ABSOLUTE(.); *(.gcc_except_table)	\
      __EXCEPT_END__ = ABSOLUTE(.);}				\
    > _region_

#define SECTION_bss(_region_, _vma_, _lma_)		\
    .bss _vma_ : _lma_					\
    { CYG_LABEL_DEFN(_bss_start) = ABSOLUTE (.);	\
      FORCE_OUTPUT; *(.dynbss) *(.bss) *(COMMON)	\
      . = ALIGN(4);                                     \
      CYG_LABEL_DEFN(_bss_end) = ABSOLUTE (.); }	\
    > _region_

#define SECTIONS_END . = ALIGN(4); CYG_LABEL_DEFN(end) = .; PROVIDE (end = .);

#include <pkgconf/system.h>
#include CYGHWR_MEMORY_LAYOUT_LDI
#include CYGBLD_HAL_PLATFORM_H

// Define VSR and vector tables to reside at fixed address.
CYG_LABEL_DEFN(hal_vsr_table) = CYGHWR_HAL_VSR_TABLE;
CYG_LABEL_DEFN(hal_virtual_vector_table) = CYGHWR_HAL_VECTOR_TABLE;
