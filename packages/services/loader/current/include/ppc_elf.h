#ifndef CYGONCE_LOADER_PPC_ELF_H
#define CYGONCE_LOADER_PPC_ELF_H

//==========================================================================
//
//      ppc_elf.h
//
//      PowerPC specific ELF file format support
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Contributors: nickg
// Date:         2000-11-20
// Purpose:      Define PowerPC ELF support
// Description:  This file contains definitions for configuring the dynamic
//               loader to deal with the PowerPC specific parts of the ELF
//               file format.
//              
// Usage:
//              #include <cyg/loader/ppc_elf.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#if defined(CYGPKG_HAL_POWERPC)

#ifndef CYG_LOADER_DYNAMIC_LD

#include <cyg/infra/cyg_type.h>

//--------------------------------------------------------------------------
// Basic definitions

#define CYG_ELF_MACHINE     EM_PPC

//--------------------------------------------------------------------------
// Relocation types
// Taken from binutils/include/elf/ppc.h - not currently sure which
// of these are actually used in executables.


#define R_PPC_NONE                      0
#define R_PPC_ADDR32                    1
#define R_PPC_ADDR24                    2
#define R_PPC_ADDR16                    3
#define R_PPC_ADDR16_LO                 4
#define R_PPC_ADDR16_HI                 5
#define R_PPC_ADDR16_HA                 6
#define R_PPC_ADDR14                    7
#define R_PPC_ADDR14_BRTAKEN            8
#define R_PPC_ADDR14_BRNTAKEN           9
#define R_PPC_REL24                     10
#define R_PPC_REL14                     11
#define R_PPC_REL14_BRTAKEN             12
#define R_PPC_REL14_BRNTAKEN            13
#define R_PPC_GOT16                     14
#define R_PPC_GOT16_LO                  15
#define R_PPC_GOT16_HI                  16
#define R_PPC_GOT16_HA                  17
#define R_PPC_PLTREL24                  18
#define R_PPC_COPY                      19
#define R_PPC_GLOB_DAT                  20
#define R_PPC_JMP_SLOT                  21
#define R_PPC_RELATIVE                  22
#define R_PPC_LOCAL24PC                 23
#define R_PPC_UADDR32                   24
#define R_PPC_UADDR16                   25
#define R_PPC_REL32                     26
#define R_PPC_PLT32                     27
#define R_PPC_PLTREL32                  28
#define R_PPC_PLT16_LO                  29
#define R_PPC_PLT16_HI                  30
#define R_PPC_PLT16_HA                  31
#define R_PPC_SDAREL16                  32
#define R_PPC_SECTOFF                   33
#define R_PPC_SECTOFF_LO                34
#define R_PPC_SECTOFF_HI                35
#define R_PPC_SECTOFF_HA                36

/* The remaining relocs are from the Embedded ELF ABI, and are not
   in the SVR4 ELF ABI.  */
#define R_PPC_EMB_NADDR32               101
#define R_PPC_EMB_NADDR16               102
#define R_PPC_EMB_NADDR16_LO            103
#define R_PPC_EMB_NADDR16_HI            104
#define R_PPC_EMB_NADDR16_HA            105
#define R_PPC_EMB_SDAI16                106
#define R_PPC_EMB_SDA2I16               107
#define R_PPC_EMB_SDA2REL               108
#define R_PPC_EMB_SDA21                 109
#define R_PPC_EMB_MRKREF                110
#define R_PPC_EMB_RELSEC16              111
#define R_PPC_EMB_RELST_LO              112
#define R_PPC_EMB_RELST_HI              113
#define R_PPC_EMB_RELST_HA              114
#define R_PPC_EMB_BIT_FLD               115
#define R_PPC_EMB_RELSDA                116

  /* These are GNU extensions to enable C++ vtable garbage collection.  */
#define R_PPC_GNU_VTINHERIT             253
#define R_PPC_GNU_VTENTRY               254

/* This is a phony reloc to handle any old fashioned TOC16 references
   that may still be in object files.  */
#define R_PPC_TOC16                     255


//--------------------------------------------------------------------------
// Processor specific customization class for Cyg_LoadObject class.

#ifdef __cplusplus

class Cyg_LoadObject_Proc :
      public Cyg_LoadObject_Base
{
 public:

    inline Cyg_LoadObject_Proc()
        : Cyg_LoadObject_Base()
        {
        };

    inline Cyg_LoadObject_Proc( Cyg_LoaderStream& stream,
                    cyg_uint32 mode,
                    Cyg_LoaderMemAlloc *mem )
        : Cyg_LoadObject_Base( stream, mode, mem )
        {
        };

    inline ~Cyg_LoadObject_Proc() {};

    cyg_code apply_rel( unsigned char type, Elf32_Word sym, Elf32_Addr offset );

    cyg_code apply_rela( unsigned char type, Elf32_Word sym,
                         Elf32_Addr offset, Elf32_Sword addend );
};

//--------------------------------------------------------------------------

inline cyg_code Cyg_LoadObject_Proc::apply_rel( unsigned char type,
                                                Elf32_Word sym,
                                                Elf32_Addr offset )
{
    return 0;
}

inline cyg_code Cyg_LoadObject_Proc::apply_rela( unsigned char type,
                                                 Elf32_Word sym,
                                                 Elf32_Addr offset,
                                                 Elf32_Sword addend )
{
    return 0; // CYG_LOADERR_INVALID_RELOC;
}

//--------------------------------------------------------------------------

externC void abort() CYGBLD_ATTRIB_WEAK;

externC void abort() { for(;;); }

externC int strcmp(const char *a, const char *b) CYGBLD_ATTRIB_WEAK;

externC int strcmp(const char *a, const char *b) { return a==b; }

externC unsigned int strlen(const char *a) CYGBLD_ATTRIB_WEAK;

externC unsigned int strlen(const char *a) { return 1; }

//--------------------------------------------------------------------------

#endif // __cplusplus

#else // CYG_LOADER_DYNAMIC_LD

//--------------------------------------------------------------------------

#define CYG_LOADER_DYNAMIC_PREFIX                                               \
        OUTPUT_FORMAT("elf32-powerpc", "elf32-powerpc", "elf32-powerpc")     \
        OUTPUT_ARCH("powerpc")

/* Adjust the address for the data segment.  We want to adjust up to
   the same address within the page on the next page up.  */
#define CYG_LOADER_DYNAMIC_DATA_ALIGN                   \
        . = ALIGN(0x1000) + (. & (0x1000 - 1));

//--------------------------------------------------------------------------

#endif // CYG_LOADER_DYNAMIC_LD

#endif // defined(CYGPKG_HAL_POWERPC) && __cplusplus

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_LOADER_PPC_ELF_H
// End of ppc_elf.h
