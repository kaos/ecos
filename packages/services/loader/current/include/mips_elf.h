#ifndef CYGONCE_LOADER_MIPS_ELF_H
#define CYGONCE_LOADER_MIPS_ELF_H

//==========================================================================
//
//      mips_elf.h
//
//      MIPS specific ELF file format support
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
// Purpose:      Define MIPS ELF support
// Description:  This file contains definitions for configuring the dynamic
//               loader to deal with the MIPS specific parts of the ELF
//               file format.
//              
// Usage:
//              #include <cyg/loader/mips_elf.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>


#if defined(CYGPKG_HAL_MIPS)

#ifndef CYG_LOADER_DYNAMIC_LD

#include <cyg/infra/cyg_type.h>

//--------------------------------------------------------------------------
// Basic definitions

#define CYG_ELF_MACHINE     EM_MIPS

//--------------------------------------------------------------------------
// Relocation types
// Taken from bfd/include/elf/mips.h - not currently sure which of these
// are actually used in executables.

#define R_MIPS_NONE                     0
#define R_MIPS_16                       1
#define R_MIPS_32                       2
#define R_MIPS_REL32                    3
#define R_MIPS_26                       4
#define R_MIPS_HI16                     5
#define R_MIPS_LO16                     6
#define R_MIPS_GPREL16                  7
#define R_MIPS_LITERAL                  8
#define R_MIPS_GOT16                    9
#define R_MIPS_PC16                     10
#define R_MIPS_CALL16                   11
#define R_MIPS_GPREL32                  12
/* Irix and MIPS16 relocs currently omitted. */
/* These are GNU extensions to handle embedded-pic.  */
#define R_MIPS_PC32                     248
#define R_MIPS_PC64                     249
#define R_MIPS_GNU_REL16_S2             250
#define R_MIPS_GNU_REL_LO16             251
#define R_MIPS_GNU_REL_HI16             252
/* These are GNU extensions to enable C++ vtable garbage collection.  */
#define R_MIPS_GNU_VTINHERIT            253
#define R_MIPS_GNU_VTENTRY              254

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
    return 0;
}


//--------------------------------------------------------------------------

#endif // __cplusplus

#else // CYG_LOADER_DYNAMIC_LD

//--------------------------------------------------------------------------

#define CYG_LOADER_DYNAMIC_PREFIX                                               \
        OUTPUT_FORMAT("elf32-bigmips", "elf32-bigmips", "elf32-littlemips")     \
        OUTPUT_ARCH("mips")


//--------------------------------------------------------------------------

#endif // CYG_LOADER_DYNAMIC_LD

#endif // defined(CYGPKG_HAL_MIPS) 

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_LOADER_MIPS_ELF_H
// End of mips_elf.h
