#ifndef CYGONCE_LOADER_ARM_ELF_H
#define CYGONCE_LOADER_ARM_ELF_H

//==========================================================================
//
//      arm_elf.h
//
//      ARM specific ELF file format support
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
// Purpose:      Define ARM ELF support
// Description:  This file contains definitions for configuring the dynamic
//               loader to deal with the ARM specific parts of the ELF
//               file format.
//              
// Usage:
//              #include <cyg/loader/arm_elf.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>


#if defined(CYGPKG_HAL_ARM)

#ifndef CYG_LOADER_DYNAMIC_LD

#include <cyg/infra/cyg_type.h>

//--------------------------------------------------------------------------
// Basic definitions

#define CYG_ELF_MACHINE     EM_ARM

//--------------------------------------------------------------------------
// Relocation types
// Taken from bfd/include/elf/arm.h - not currently sure which of these
// are actually used in executables.

#define R_ARM_NONE         0
#define R_ARM_PC24         1
#define R_ARM_ABS32        2
#define R_ARM_REL32        3
#define R_ARM_PC13         4
#define R_ARM_ABS16        5
#define R_ARM_ABS12        6
#define R_ARM_THM_ABS5     7
#define R_ARM_ABS8         8
#define R_ARM_SBREL32      9
#define R_ARM_THM_PC22    10
#define R_ARM_THM_PC8     11
#define R_ARM_AMP_VCALL9  12
#define R_ARM_SWI24       13
#define R_ARM_THM_SWI8    14
#define R_ARM_XPC25       15
#define R_ARM_THM_XPC22   16
#define R_ARM_COPY        20       /* copy symbol at runtime */
#define R_ARM_GLOB_DAT    21       /* create GOT entry */
#define R_ARM_JUMP_SLOT   22       /* create PLT entry */
#define R_ARM_RELATIVE    23       /* adjust by program base */
#define R_ARM_GOTOFF      24       /* 32 bit offset to GOT */
#define R_ARM_GOTPC       25       /* 32 bit PC relative offset to GOT */
#define R_ARM_GOT32       26       /* 32 bit GOT entry */
#define R_ARM_PLT32       27       /* 32 bit PLT address */
#define R_ARM_GNU_VTENTRY 100
#define R_ARM_GNU_VTINHERIT 101
#define R_ARM_THM_PC11    102       /* Cygnus extension to abi: Thumb unconditional branch */
#define R_ARM_THM_PC9     103       /* Cygnus extension to abi: Thumb conditional branch */
#define R_ARM_RXPC25     249
#define R_ARM_RSBREL32   250
#define R_ARM_THM_RPC22  251
#define R_ARM_RREL32     252
#define R_ARM_RABS32     253
#define R_ARM_RPC24      254
#define R_ARM_RBASE      255


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
        OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")     \
        OUTPUT_ARCH("arm")

//--------------------------------------------------------------------------

#endif // CYG_LOADER_DYNAMIC_LD

#endif // defined(CYGPKG_HAL_ARM) 

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_LOADER_ARM_ELF_H
// End of arm_elf.h
