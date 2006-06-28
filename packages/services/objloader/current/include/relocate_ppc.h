#ifndef __RELOCATE_PPC_H__
#define __RELOCATE_PPC_H__

/* =================================================================
 *
 *      relocate_ppc.h
 *
 *      Architecture dependent relocation routines for the PowerPC
 *
 * ================================================================= 
 * ####ECOSGPLCOPYRIGHTBEGIN####
 * -------------------------------------------
 * This file is part of eCos, the Embedded Configurable Operating
 * System.
 * Copyright (C) 2005 eCosCentric Ltd.
 * 
 * eCos is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 or (at your option)
 * any later version.
 * 
 * eCos is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with eCos; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or
 * use macros or inline functions from this file, or you compile this
 * file and link it with other works to produce a work based on this
 * file, this file does not by itself cause the resulting work to be
 * covered by the GNU General Public License. However the source code
 * for this file must still be made available in accordance with
 * section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work
 * based on this file might be covered by the GNU General Public
 * License.
 *
 * -------------------------------------------
 * ####ECOSGPLCOPYRIGHTEND####
 * =================================================================
 * #####DESCRIPTIONBEGIN####
 * 
 *  Author(s):    Anthony Tonizzo (atonizzo@gmail.com)
 *  Date:         2005-05-13
 *  Purpose:      
 *  Description:  
 *               
 * ####DESCRIPTIONEND####
 * 
 * =================================================================
 */

#define Elf_Rel                0
#define Elf_Rela               1
#define ELF_ARCH_MACHINE_TYPE  20    // PowerPC.
#define ELF_ARCH_ENDIANNESS    ELFDATA2MSB
#define ELF_ARCH_RELTYPE       Elf_Rela

#define R_PPC_NONE              0
#define R_PPC_ADDR32            1  // 32bit absolute address 
#define R_PPC_ADDR24            2  // 26bit address, 2 bits ignored.  
#define R_PPC_ADDR16            3  // 16bit absolute address 
#define R_PPC_ADDR16_LO         4  // lower 16bit of absolute address 
#define R_PPC_ADDR16_HI         5  // high 16bit of absolute address 
#define R_PPC_ADDR16_HA         6  // adjusted high 16bit 
#define R_PPC_ADDR14            7  // 16bit address, 2 bits ignored 
#define R_PPC_ADDR14_BRTAKEN    8
#define R_PPC_ADDR14_BRNTAKEN   9
#define R_PPC_REL24            10  // PC relative 26 bit 
#define R_PPC_REL14            11   // PC relative 16 bit 
#define R_PPC_REL14_BRTAKEN    12
#define R_PPC_REL14_BRNTAKEN   13
#define R_PPC_GOT16            14
#define R_PPC_GOT16_LO         15
#define R_PPC_GOT16_HI         16
#define R_PPC_GOT16_HA         17
#define R_PPC_PLTREL24         18
#define R_PPC_COPY             19
#define R_PPC_GLOB_DAT         20
#define R_PPC_JMP_SLOT         21
#define R_PPC_RELATIVE         22
#define R_PPC_LOCAL24PC        23
#define R_PPC_UADDR32          24
#define R_PPC_UADDR16          25
#define R_PPC_REL32            26
#define R_PPC_PLT32            27
#define R_PPC_PLTREL32         28
#define R_PPC_PLT16_LO         29
#define R_PPC_PLT16_HI         30
#define R_PPC_PLT16_HA         31
#define R_PPC_SDAREL16         32
#define R_PPC_SECTOFF          33
#define R_PPC_SECTOFF_LO       34
#define R_PPC_SECTOFF_HI       35
#define R_PPC_SECTOFF_HA       36

// Keep this the last entry.  
#define R_PPC_NUM              37

// The remaining relocs are from the Embedded ELF ABI, and are not
//  in the SVR4 ELF ABI.  
#define R_PPC_EMB_NADDR32     101
#define R_PPC_EMB_NADDR16     102
#define R_PPC_EMB_NADDR16_LO  103
#define R_PPC_EMB_NADDR16_HI  104
#define R_PPC_EMB_NADDR16_HA  105
#define R_PPC_EMB_SDAI16      106
#define R_PPC_EMB_SDA2I16     107
#define R_PPC_EMB_SDA2REL     108
#define R_PPC_EMB_SDA21       109  // 16 bit offset in SDA 
#define R_PPC_EMB_MRKREF      110
#define R_PPC_EMB_RELSEC16    111
#define R_PPC_EMB_RELST_LO    112
#define R_PPC_EMB_RELST_HI    113
#define R_PPC_EMB_RELST_HA    114
#define R_PPC_EMB_BIT_FLD     115
#define R_PPC_EMB_RELSDA      116  // 16 bit relative offset in SDA 

// From the Elf PPC spec.
#define _lo_(x)    (x & 0xFFFF) 
#define _hi_(x)    ((x >> 16) & 0xFFFF)
#define _ha_(x)    (((x >> 16)+((x & 0x8000) ? 1 : 0)) & 0xFFFF)

void      cyg_ldr_flush_cache( void );
cyg_int32 cyg_ldr_relocate( cyg_int32, cyg_uint32, cyg_int32 );

#endif //__RELOCATE_PPC_H__
