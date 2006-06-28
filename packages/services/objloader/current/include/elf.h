#ifndef __ELF_H__
#define __ELF_H__

/* =================================================================
 *
 *      elf.h
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

typedef cyg_uint32     Elf32_Addr;
typedef cyg_uint16     Elf32_Half;
typedef cyg_uint32     Elf32_Off;
typedef cyg_int32      Elf32_Sword;
typedef cyg_uint32     Elf32_Word;

#define EI_NIDENT      16
typedef struct
{
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off  e_phoff;
    Elf32_Off  e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

// The ELFCLASS* macros are the defined values of e_ident[EI_CLASS].
#define ELFCLASSNONE      0   // Invalid class.
#define ELFCLASS32        1   // 32-bit objects.
#define ELFCLASS64        2   // 64-bit objects.

// The ELFDATA* macros are the allowed values of e_ident[EI_DATA].
#define ELFDATA2LSB     1 // Little Endian
#define ELFDATA2MSB     2 // Big Endian.

// The ET_* macros define the values of the e_type field of the ElfXX_Ehdr
//   structure.
#define ET_NONE         0 // No file type.
#define ET_REL          1 // Relocatable file.
#define ET_EXEC         2 // Executable file.
#define ET_DYN          3 // Shared object file.
#define ET_CORE         4 // Core file.
#define ET_LOPROC       0xff00 // Processor-specific.

// The ELFMAG* macros are the values of e_ident[EI_MAG0-4]
#define ELFMAG0         0x7f      // magic number, byte 0 
#define ELFMAG1         'E'       // magic number, byte 1 
#define ELFMAG2         'L'       // magic number, byte 2 
#define ELFMAG3         'F'       // magic number, byte 3 
#define ELFMAG          "\177ELF" // magic string 
#define SELFMAG         4         // magic string length 

#define EI_MAG0         0
#define EI_MAG1         1
#define EI_MAG2         2
#define EI_MAG3         3
#define EI_CLASS        4
#define EI_DATA         5 
#define EI_VERSION      6
#define EI_PAD          7 

typedef struct 
{
    Elf32_Word sh_name; 		// section name.
    Elf32_Word sh_type;         // SHT_...
    Elf32_Word sh_flags;        // SHF_...
    Elf32_Addr sh_addr;         // virtual address
    Elf32_Off  sh_offset;       // file offset
    Elf32_Word sh_size;         // section size
    Elf32_Word sh_link;         // misc info 
    Elf32_Word sh_info;         // misc info 
    Elf32_Word sh_addralign;    // memory alignment
    Elf32_Word sh_entsize;      // entry size if table.
} Elf32_Shdr;

// Symbols table entry.
typedef struct
{
    Elf32_Word    st_name;                // Symbol name (string tbl index).
    Elf32_Addr    st_value;               // Symbol value.
    Elf32_Word    st_size;                // Symbol size.
    unsigned char st_info;                // Symbol type and binding.
    unsigned char st_other;               // Symbol visibility.
    Elf32_Half    st_shndx;               // Section index.
} Elf32_Sym;

typedef struct 
{
    Elf32_Addr r_offset;
    Elf32_Word r_info;
} Elf32_Rel;

typedef struct
{ 
    Elf32_Addr  r_offset;
    Elf32_Word  r_info;
    Elf32_Sword r_addend;
} Elf32_Rela;


#define SHT_NULL        0
#define SHT_PROGBITS    1
#define SHT_SYMTAB      2
#define SHT_STRTAB      3
#define SHT_RELA        4
#define SHT_HASH        5
#define SHT_DYNAMIC     6
#define SHT_NOTE        7
#define SHT_NOBITS      8
#define SHT_REL         9
#define SHT_SHLIB      10
#define SHT_DYNSYM     11

#define SHN_UNDEF       0
#define SHN_LORESERVE   0xff00
#define SHN_LOPROC      0xff00
#define SHN_HIPROC      0xff1f
#define SHN_ABS         0xfff1
#define SHN_COMMON      0xfff2
#define SHN_HIRESERVE   0xffff

#define STT_NOTYPE            0
#define STT_OBJECT            1
#define STT_FUNC              2
#define STT_SECTION           3
#define STT_FILE              4
#define STT_LOPROC           13
#define STT_HIPROC           15

#define STB_LOCAL            0
#define STB_GLOBAL           1
#define STB_WEAK             2
#define STB_LOPROC          13
#define STB_HIPROC          15

// The ELF_STRING_xxx macros are names of common sections
#define ELF_STRING_bss          ".bss"
#define ELF_STRING_hbss         ".hbss"
#define ELF_STRING_sbss         ".sbss"
#define ELF_STRING_tbss         ".tbss"
#define ELF_STRING_data         ".data"
#define ELF_STRING_hdata        ".hdata"
#define ELF_STRING_sdata        ".sdata"
#define ELF_STRING_sdata2       ".sdata2"
#define ELF_STRING_fini         ".fini"
#define ELF_STRING_init         ".init"
#define ELF_STRING_interp       ".interp"
#define ELF_STRING_rodata       ".rodata"
#define ELF_STRING_text         ".text"
#define ELF_STRING_comment      ".comment"
#define ELF_STRING_dynamic      ".dynamic"
#define ELF_STRING_dynstr       ".dynstr"
#define ELF_STRING_dynsym       ".dynsym"
#define ELF_STRING_dlt          ".dlt"
#define ELF_STRING_note         ".note"
#define ELF_STRING_opd          ".opd"
#define ELF_STRING_plt          ".plt"
#define ELF_STRING_bss_rela     ".rela.bss"
#define ELF_STRING_hbss_rela    ".rela.hbss"
#define ELF_STRING_data_rela    ".rela.data"
#define ELF_STRING_dlt_rela     ".rela.dlt"
#define ELF_STRING_plt_rela     ".rela.plt"
#define ELF_STRING_sdata_rela   ".rela.sdata"
#define ELF_STRING_strtab       ".strtab"
#define ELF_STRING_symtab       ".symtab"
#define ELF_STRING_hash         ".hash"
#define ELF_STRING_shstrtab     ".shstrtab"
#define ELF_STRING_shsymtab     ".shsymtab"
#define ELF_STRING_rela         ".rela"
#define ELF_STRING_rel          ".rel"

#define ELF32_R_SYM(i)  ((i)>>8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))

#define ELF32_ST_BIND(i)  ((i)>>4)
#define ELF32_ST_TYPE(i)  ((i)&0x0F)


#endif

