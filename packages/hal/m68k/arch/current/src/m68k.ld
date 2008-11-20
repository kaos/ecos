/*==========================================================================
//
//      m68k.ld
//
//      Linker script
//
//==========================================================================
//###ECOSGPLCOPYRIGHTBEGIN####
//-------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003,2004,2005,2006,2008 Free Software Foundation, Inc.
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//###DESCRIPTIONBEGIN####
//
// Author(s):     bartv
// Date:          2003-06-04
//
//###DESCRIPTIONEND####
//========================================================================
*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#define __LDI__ 1
#include CYGHWR_MEMORY_LAYOUT_H

STARTUP(vectors.o)
ENTRY(hal_m68k_exception_reset)
#ifdef EXTRAS
INPUT(extras.o)
#endif
GROUP(libtarget.a libgcc.a libsupc++.a)

#define ALIGN_LMA 4
#define FOLLOWING(_section_) AT ((LOADADDR (_section_) + SIZEOF (_section_) + ALIGN_LMA - 1) & ~ (ALIGN_LMA - 1))
#define LMA_EQ_VMA
#define FORCE_OUTPUT . = .

#ifndef CYG_LABEL_DEFN
# define CYG_LABEL_DEFN(_label) _label
#endif

// Depending on the startup type and issues such as the memory map
// before and after any remapping, _stext should usually be defined
// either at the beginning of the .m68k_startup section or at the
// beginning of the .text section. Or conceivably somewhere else, in
// which case it is up to the platform-specific linker script or
// code to define _stext. The platform HAL can specify the desired
// behaviour via the LAYOUT_H file.
#if defined(_HAL_M68K_STEXT_AT_M68K_TEXT_)
# define _HAL_M68K_STARTUP_SECTION_STEXT_
# define _HAL_TEXT_SECTION_STEXT_           _stext = . ;
#elif defined(_HAL_M68K_STEXT_ELSEWHERE_)
# define _HAL_M68K_STARTUP_SECTION_STEXT_
# define _HAL_TEXT_SECTION_STEXT_
#else
# define _HAL_M68K_STARTUP_SECTION_STEXT_   _stext = . ;
# define _HAL_TEXT_SECTION_STEXT_
#endif
    
#define SECTIONS_BEGIN

// RAM vectors, i.e. the M68K exception table, the virtual vector table,
// and possibly some data that needs to be shared between eCos and
// RedBoot. Usually this will be located at 0 but may be at another
// address via e.g. %vbr. Some platforms in some configurations may
// have an empty ram vectors section with all the work done in
// .vectors instead. The section may or may not be loadable.

#define SECTION_ram_vectors(_region_, _vma_)                            \
    . = ALIGN( _vma_ , ALIGN_LMA ) ;                                    \
    _hal_ram_vectors_section_start = . ;                                \
    .ram_vectors . : { KEEP (*(.ram_vectors)) } > _region_              \
    . = ALIGN(ALIGN_LMA) ;                                              \
    _hal_ram_vectors_section_end = . ;                                  \
    _hal_ram_vectors_section_size    = _hal_ram_vectors_section_end -   \
                                       _hal_ram_vectors_section_start ;

// .m68k_startup usually contains only the M68K startup code. It may also
// contain the exception vectors if booting from flash and the exception
// vectors are held in flash as well. .vectors.0 is for backwards
// compatibility with older HALs.
#define SECTION_m68k_start(_region_, _vma_)                                                             \
    . = ALIGN( _vma_ , ALIGN_LMA ) ;                                                                    \
    _hal_m68k_start_section_start = . ;                                                                 \
    _HAL_M68K_STARTUP_SECTION_STEXT_                                                                    \
    .m68k_start . : { KEEP (*(.m68k_start)) KEEP (*(.m68k_start.*)) KEEP (*(.vectors.0)) } > _region_   \
    . = ALIGN(ALIGN_LMA) ;                                                                              \
    _hal_m68k_start_section_end = . ;                                                                   \
    _hal_m68k_start_section_size    = _hal_m68k_start_section_end - _hal_m68k_start_section_start ;

// Some coldfires booting from internal flash need a signature block at
// a magic location
#define SECTION_mcfxxxx_cfm_security_settings(_region_, _vma_)                                          \
    . = ALIGN( _vma_ , ALIGN_LMA ) ;                                                                    \
    _hal_mcfxxxx_cfm_security_settings_section_start = . ;                                              \
    .mcfxxxx_cfm_security_settings . :                                                                  \
    {                                                                                                   \
        FORCE_OUTPUT;                                                                                   \
        KEEP (*(.mcfxxxx_cfm_security_settings));                                                       \
    } > _region_                                                                                        \
    . = ALIGN(ALIGN_LMA) ;                                                                              \
    _hal_mcfxxxx_cfm_security_settings_section_end  = . ;                                               \
    _hal_mcfxxxx_cfm_security_settings_section_size = _hal_mcfxxxx_cfm_security_settings_section_end -  \
                                                      _hal_mcfxxxx_cfm_security_settings_section_start ;

// ----------------------------------------------------------------------------
// C++ support. It should always be possible to discard the global destructors
// since eCos applications are not expected to exit. The exception handling
// support is normally needed only if linking with libstdc++, otherwise
// there is no code in the system to use the exception frame info.
#if defined(CYGINT_HAL_LINKER_SCRIPT_KEEP_DTORS)
# define _DTORS1_                                                       \
        __DTOR_LIST__ = ABSOLUTE(.);                                    \
        KEEP(*(SORT(.dtors*)))                                          \
        __DTOR_END__ = ABSOLUTE(.);                                     \
# define _DTORS2_
#else
# define _DTORS1_
# define _DTORS2_           /DISCARD/ : { *(.dtors*) }
#endif

#if defined(CYGPKG_LIBSTDCXX) || defined(CYGINT_HAL_LINKER_SCRIPT_KEEP_EH_FRAME)
# define _EH_FRAME1_(_region_)                                          \
    .eh_frame . : {                                                     \
       FORCE_OUTPUT;  __EH_FRAME_BEGIN__ = ABSOLUTE(.);                 \
       KEEP(*(.eh_frame))                                               \
       . = ALIGN(ALIGN_LMA);                                            \
       __FRAME_END__ = ABSOLUTE(.);                                     \
       LONG(0)                                                          \
       LONG(0)                                                          \
    } > _region_ = 0
# define _EH_FRAME2_
#else
# define _EH_FRAME1_(_region_)
# define _EH_FRAME2_ /DISCARD/ : { *(.eh_frame) }
#endif

#if defined(CYGPKG_LIBSTDCXX) || defined(CYGINT_HAL_LINKER_SCRIPT_KEEP_GCC_EXCEPT_TABLE)
# define _GCC_EXCEPT_TABLE1_(_region_)                                  \
    .gcc_except_table . : {                                             \
        __EXCEPT_START__ = ABSOLUTE(.);                                 \
        KEEP(*(.gcc_except_table)) *(.gcc_except_table.*)               \
      __EXCEPT_END__ = ABSOLUTE(.);                                     \
    } > _region_
# define _GCC_EXCEPT_TABLE2_
#else
# define _GCC_EXCEPT_TABLE1_(_region_)
# define _GCC_EXCEPT_TABLE2_ /DISCARD/ : { *(.gcc_except_table) *(.gcc_except_table.*) }
#endif

#define SECTION_code(_region_, _vma_)                                                           \
    . = ALIGN( _vma_ , ALIGN_LMA ) ;                                                            \
    _hal_code_section_start = . ;                                                               \
    _hal_text_section_start = . ;                                                               \
    _HAL_TEXT_SECTION_STEXT_                                                                    \
    .text . : {                                                                                 \
        *(.text*) *(.gnu.warning) *(.gnu.linkonce.t*) *(.init)                                  \
        . = ALIGN (4);                                                                          \
        __CTOR_LIST__ = ABSOLUTE(.);                                                            \
        KEEP(*(SORT(.ctors*)));                                                                 \
        __CTOR_END__ = ABSOLUTE(.);                                                             \
        . = ALIGN (4);                                                                          \
        _DTORS1_                                                                                \
    } > _region_                                                                                \
    . = ALIGN(ALIGN_LMA) ;                                                                      \
    _etext = . ;  PROVIDE (etext = .) ;                                                         \
    _hal_text_section_end   = . ;                                                               \
    _hal_text_section_size  = _hal_text_section_end - _hal_text_section_start ;                 \
    .rodata . : { *(.rodata1) *(.rodata*) *(.gnu.linkonce.r.*) } > _region_                     \
    . = ALIGN(ALIGN_LMA) ;                                                                      \
    _EH_FRAME1_(_region_)                                                                       \
    . = ALIGN(ALIGN_LMA) ;                                                                      \
    _GCC_EXCEPT_TABLE1_(_region_)                                                               \
    . = ALIGN(ALIGN_LMA) ;                                                                      \
    .rel.text    : { *(.rel.text)    *(.rel.text.*)    *(.rel.gnu.linkonce.t*) } > _region_     \
    .rela.text   : { *(.rela.text)   *(.rela.text.*)   *(.rela.gnu.linkonce.t*) } > _region_    \
    .rel.data    : { *(.rel.data)    *(.rel.data.*)    *(.rel.gnu.linkonce.d*) } > _region_     \
    .rela.data   : { *(.rela.data)   *(.rela.data.*)   *(.rela.gnu.linkonce.d*) } > _region_    \
    .rel.rodata  : { *(.rel.rodata)  *(.rel.rodata.*)  *(.rel.gnu.linkonce.r*) } > _region_     \
    .rela.rodata : { *(.rela.rodata) *(.rela.rodata.*) *(.rela.gnu.linkonce.r*) } > _region_    \
    .rel.got       :   { *(.rel.got)    } > _region_                                            \
    .fixup . : {                                                                                \
        __FIXUP_START__ = ABSOLUTE(.);                                                          \
        *(.fixup)                                                                               \
        __FIXUP_END__ = ABSOLUTE(.);                                                            \
    } > _region_                                                                                \
    .got . : {                                                                                  \
        FORCE_OUTPUT; *(.got.plt) *(.got)                                                       \
        _GOT1_START_ = ABSOLUTE (.); *(.got1) _GOT1_END_ = ABSOLUTE (.);                        \
        _GOT2_START_ = ABSOLUTE (.); *(.got2) _GOT2_END_ = ABSOLUTE (.);                        \
    } > _region_                                                                                \
    . = ALIGN(ALIGN_LMA) ;                                                                      \
    _hal_code_section_end  = . ;                                                                \
    _hal_code_section_size = _hal_code_section_end - _hal_code_section_start ;                  \
    _DTORS2_                                                                                    \
    _EH_FRAME2_                                                                                 \
    _GCC_EXCEPT_TABLE2_

#define SECTION_data(_region_, _vma_, _lma_)                                                    \
    . = ALIGN( _vma_ , ALIGN_LMA ) ;                                                            \
    _hal_data_section_start_vma = . ;                                                           \
    .data . : _lma_ {                                                                           \
        *(.shdata*) *(.data*) *(.gnu.linkonce.d.*) *(.sdata) *(.gnu.linkonce.s.*)               \
        . = ALIGN (4);                                                                          \
        *(.2ram.*)                                                                              \
        . = ALIGN (4);                                                                          \
        KEEP(*( SORT (.ecos.table.*)));                                                         \
        . = ALIGN (4);                                                                          \
    } > _region_                                                                                \
    . = ALIGN(ALIGN_LMA);                                                                       \
    _hal_data_section_start_lma = LOADADDR(.data) ;                                             \
    _hal_data_section_end_vma = . ;                                                             \
    _hal_data_section_size = _hal_data_section_end_vma - _hal_data_section_start_vma ;          \
    _hal_data_section_end_lma = _hal_data_section_start_lma + _hal_data_section_size ;          \
    _hal_bss_start = . ;                                                                        \
    .sbss . : { *(.sbss) *(.gnu.linkonce.sb.*) *(.scommon) } > _region_                         \
    . = ALIGN(ALIGN_LMA) ;                                                                      \
    .bss  . : { *(.dynbss) *(.shbss) *(.bss*) *(.gnu.linkonce.b.*) *(COMMON) } > _region_       \
    . = ALIGN(ALIGN_LMA) ;                                                                      \
    _hal_bss_end = . ;                                                                          \
    _hal_bss_size = _hal_bss_end - _hal_bss_start ;

// Some ColdFire processors have on-chip RAM. Allocate  .iram_text, .iram_data
// .iram_bss in the expected fashion.
#define SECTION_iram(_region_, _vma_, _lma_)                                                            	\
    . = ALIGN( _vma_ , ALIGN_LMA ) ;                                                                    	\
    _hal_iram_section_start_vma      = . ;                                                              	\
    _hal_iram_text_section_start_vma = . ;                                                              	\
    .iram_text . : _lma_ { *(.iram_text) *(.iram_text.*) } > _region_   =0                              	\
    _hal_iram_text_section_start_lma = LOADADDR(.iram_text) ;                                           	\
    . = ALIGN(ALIGN_LMA) ;                                                                              	\
    _hal_iram_text_section_end_vma = . ;                                                                	\
    _hal_iram_text_section_size = _hal_iram_text_section_end_vma - _hal_iram_text_section_start_vma ;   	\
    _hal_iram_text_section_end_lma = _hal_iram_text_section_start_lma + _hal_iram_text_section_size ;   	\
    _hal_iram_data_section_start_vma = . ;                                                              	\
    .iram_data . : AT(_hal_iram_text_section_end_lma) { *(.iram_data) *(.iram_data.*) } > _region_ =0	    \
    . = ALIGN(ALIGN_LMA) ;                                                                              	\
    _hal_iram_data_section_start_lma = LOADADDR(.iram_data) ;                                           	\
    _hal_iram_data_section_end_vma = . ;                                                                	\
    _hal_iram_data_section_size = _hal_iram_data_section_end_vma - _hal_iram_data_section_start_vma ;   	\
    _hal_iram_data_section_end_lma = _hal_iram_data_section_start_lma + _hal_iram_data_section_size ;   	\
    _hal_iram_bss_section_start = . ;                                                                   	\
    _iram_bss . (NOLOAD) : { *(.iram_bss) *(.iram_bss.*) } > _region_ =0                                	\
    . = ALIGN(ALIGN_LMA) ;                                                                              	\
    _hal_iram_bss_section_end = . ;                                                                     	\
    _hal_iram_section_end_vma = . ;                                                                     	\
    _hal_iram_section_size      = _hal_iram_section_end_vma - _hal_iram_section_start_vma ;                	\
    _hal_iram_section_start_lma = LOADADDR(.iram_text) ;                                                	\
    _hal_iram_section_end_lma   = _hal_iram_section_start_lma + _hal_iram_text_section_size +           	\
                                  _hal_iram_data_section_size ;

#define SECTIONS_END                            \
    .stab    0 (NOLOAD) : { *(.stab) }          \
    .stabstr 0 (NOLOAD) : { *(.stabstr) }       \
    .comment 0 (NOLOAD) : { *(.comment) }       \
    . = ALIGN(4); _end = .; PROVIDE (end = .);

// These macros are for backwards compatibility. Some are redundant.
#define SECTION_fixed_vectors(_region_, _vma_, _lma_)   SECTION_ram_vectors(_region_, _vma_)
#define SECTION_vectors(_region_, _vma_, _lma_)         SECTION_m68k_start(_region_, _vma_)
#define SECTION_text(_region_, _vma_, _lma_)            SECTION_code(_region_, _vma_)
#define SECTION_rodata(_region_, _vma_, _lma_)          SECTION_constants(_region_, _vma_)
#define SECTION_rodata1(_region_, _vma_, _lma_)
#define SECTION_fini(_region_, _vma_, _lma_)
#define SECTION_fixup(_region_, _vma_, _lma_)
#define SECTION_RELOCS(_region_, _vma_, _lma_)
#define SECTION_eh_frame(_region_, _vma_, _lma_)
#define SECTION_gcc_except_table(_region_, _vma_, _lma)
#define SECTION_got(_region_, _vma_, _lma_)
#define SECTION_sbss(_region_, _vma_, _lma_)
#define SECTION_bss(_region_, _vma_, _lma_)

#include <pkgconf/system.h>
#include CYGHWR_MEMORY_LAYOUT_LDI
