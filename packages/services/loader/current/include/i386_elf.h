#ifndef CYGONCE_LOADER_I386_ELF_H
#define CYGONCE_LOADER_I386_ELF_H

//==========================================================================
//
//      i386_elf.h
//
//      I386 specific ELF file format support
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
// Purpose:      Define I386 ELF support
// Description:  This file contains definitions for configuring the dynamic
//               loader to deal with the I386 specific parts of the ELF
//               file format.
//              
// Usage:
//              #include <cyg/loader/i386_elf.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#if defined(CYGPKG_HAL_I386)

#ifndef CYG_LOADER_DYNAMIC_LD

#include <cyg/infra/cyg_type.h>

//--------------------------------------------------------------------------
// Basic definitions

#define CYG_ELF_MACHINE     EM_386

//--------------------------------------------------------------------------
// Relocation types
// Taken from bfd/include/elf/i386.h - not currently sure which of these
// are actually used in executables.

#define R_386_NONE              0       /* No reloc */
#define R_386_32                1       /* Direct 32 bit  */
#define R_386_PC32              2       /* PC relative 32 bit */
#define R_386_GOT32             3       /* 32 bit GOT entry */
#define R_386_PLT32             4       /* 32 bit PLT address */
#define R_386_COPY              5       /* Copy symbol at runtime */
#define R_386_GLOB_DAT          6       /* Create GOT entry */
#define R_386_JUMP_SLOT         7       /* Create PLT entry */
#define R_386_RELATIVE          8       /* Adjust by program base */
#define R_386_GOTOFF            9       /* 32 bit offset to GOT */
#define R_386_GOTPC             10      /* 32 bit PC relative offset to GOT */
#define R_386_16                20
#define R_386_PC16              21
#define R_386_8                 22
#define R_386_PC8               23
#define R_386_max               24
     /* These are GNU extensions to enable C++ vtable garbage collection.  */
#define R_386_GNU_VTINHERIT     250
#define R_386_GNU_VTENTRY       251

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
    CYG_ADDRESS addr = (CYG_ADDRESS)(offset + base);
    CYG_ADDRESS saddr = CYG_LOADER_NULLSYMADDR;
    
    switch( type )
    {

    case R_386_32:                          /* Direct 32 bit  */
        saddr = get_sym_addr_from_ix(sym);
        if( saddr == CYG_LOADER_NULLSYMADDR )
            return CYG_LOADERR_NO_SYMBOL;
        *(CYG_WORD32 *)addr += saddr;
        break;
        
    case R_386_PC32:                        /* PC relative 32 bit */
        saddr = get_sym_addr_from_ix(sym);
        if( saddr == CYG_LOADER_NULLSYMADDR )
            return CYG_LOADERR_NO_SYMBOL;
        *(CYG_WORD32 *)addr += saddr;
        *(CYG_WORD32 *)addr -= (CYG_WORD32)addr;
        break;

    case R_386_GOT32:                       /* 32 bit GOT entry */
    case R_386_PLT32:                       /* 32 bit PLT address */
    case R_386_COPY:                        /* Copy symbol at runtime */
    case R_386_GLOB_DAT:                    /* Create GOT entry */
    case R_386_JUMP_SLOT:                   /* Create PLT entry */
        return CYG_LOADERR_INVALID_RELOC;
        
    case R_386_RELATIVE:                    /* Adjust by program base */
        *(CYG_WORD32 *)addr += base;
        break;

    case R_386_GOTOFF:                      /* 32 bit offset to GOT */
    case R_386_GOTPC:                       /* 32 bit PC relative offset to GOT */
    case R_386_16:
    case R_386_PC16:
    case R_386_8:
    case R_386_PC8:
    default:
        return CYG_LOADERR_INVALID_RELOC;
    }
    return 0;
}

inline cyg_code Cyg_LoadObject_Proc::apply_rela( unsigned char type,
                                                 Elf32_Word sym,
                                                 Elf32_Addr offset,
                                                 Elf32_Sword addend )
{
    // No RELA relocs in i386
    return CYG_LOADERR_INVALID_RELOC;
}

//--------------------------------------------------------------------------

#endif // __cplusplus

#else // CYG_LOADER_DYNAMIC_LD

//--------------------------------------------------------------------------

#define CYG_LOADER_DYNAMIC_PREFIX                                               \
        OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")     \
        OUTPUT_ARCH("i386")

/* Adjust the address for the data segment.  We want to adjust up to
   the same address within the page on the next page up.  */
#define CYG_LOADER_DYNAMIC_DATA_ALIGN                   \
        . = ALIGN(0x1000) + (. & (0x1000 - 1));

//--------------------------------------------------------------------------

#endif // CYG_LOADER_DYNAMIC_LD

#endif // defined(CYGPKG_HAL_I386) && __cplusplus

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_LOADER_I386_ELF_H
// End of i386_elf.h
