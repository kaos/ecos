/* =================================================================
 *
 *      objelf.c
 *
 *      Relocation routine for eCos loader.
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
 *  Contributors: nickg@ecoscentric.com
 *  Date:         2005-05-13
 *  Purpose:      
 *  Description:  
 *               
 * ####DESCRIPTIONEND####
 * 
 * =================================================================
 */

#include <cyg/infra/diag.h>     // For diagnostic printing.
#include <pkgconf/io_fileio.h>
#include <cyg/hal/hal_tables.h>
#include <stdio.h>

#include <pkgconf/objloader.h>
#include <cyg/objloader/elf.h>
#include <cyg/objloader/objelf.h>

CYG_HAL_TABLE_BEGIN(cyg_ldr_table, ldr_table);
CYG_HAL_TABLE_END(cyg_ldr_table_end, ldr_table);

__externC cyg_ldr_table_entry cyg_ldr_table[];
__externC cyg_ldr_table_entry cyg_ldr_table_end[];

#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 0
void 
cyg_ldr_print_section_data(PELF_OBJECT p)
{
    int    i;
    char   strname[32];
    char  *p_strtab = (char*)p->sections[p->p_elfhdr->e_shstrndx];
    
    diag_printf("\n\nSection Headers:\n"); 
    diag_printf("[Nr]  Name                  Addr    Offset    Size     Info\n");
    for (i = 0; i < p->p_elfhdr->e_shnum; i++)
    {
        sprintf(strname, "%s", p_strtab + p->p_sechdr[i].sh_name);
        while (strlen(strname) < 20)
            strcat(strname, " ");
        diag_printf("[%02d] %s %08X %08X %08X %08X\n",  
                     i, 
                     strname,
                     p->p_sechdr[i].sh_addr,
                     p->p_sechdr[i].sh_offset,
                     p->p_sechdr[i].sh_size,
                     p->p_sechdr[i].sh_info);
    }                 
    diag_printf("\n"); 
}

void 
cyg_ldr_print_symbol_names(PELF_OBJECT p)
{
    int        i;
    Elf32_Sym *p_symtab = (Elf32_Sym*)p->sections[p->hdrndx_symtab];
    char      *p_strtab = (char*)p->sections[p->hdrndx_strtab];
    char       strname[32];

    // Total number of entries in the symbol table.
    int symtab_entries = p->p_sechdr[p->hdrndx_symtab].sh_size / 
                                p->p_sechdr[p->hdrndx_symtab].sh_entsize;
    diag_printf("Num  Value     Size Ndx   Name\n"); 
    for (i = 1; i < symtab_entries; i++)
    {
        sprintf(strname, "%d", i);
        while (strlen(strname) < 5)
            strcat(strname, " ");
        diag_printf(strname);         
        
        sprintf(strname, 
                 "%08X  %d", 
                 p_symtab[i].st_value, 
                 p_symtab[i].st_size);
        while (strlen(strname) < 15)
            strcat(strname, " ");
        diag_printf(strname);         
        
        sprintf(strname, "%d", p_symtab[i].st_shndx);
        while (strlen(strname) < 6)
            strcat(strname, " ");
        diag_printf(strname);         
        
        strncpy(strname, 
                 p_strtab + p_symtab[i].st_name, 
                 sizeof(strname) - 1);
        strname[strlen(p_strtab + p_symtab[i].st_name)] = '\0';
        diag_printf(strname);         
        diag_printf("\n");         
    }
}

void 
cyg_ldr_print_rel_names(PELF_OBJECT p)
{
    int        i, j, r_entries, sym_index;
    Elf32_Sym *p_symtab = (Elf32_Sym*)p->sections[p->hdrndx_symtab];
    char      *p_strtab = (char*)p->sections[p->hdrndx_strtab];
    char      *p_shstrtab = (char*)p->sections[p->p_elfhdr->e_shstrndx];
#if ELF_ARCH_RELTYPE == Elf_Rela        
    Elf32_Rela*   p_rela;
#else
    Elf32_Rel*    p_rel;
#endif
    char       strname[32];

    for (i = 1; i < p->p_elfhdr->e_shnum; i++)
    {
        if ((p->p_sechdr[i].sh_type == SHT_REL) ||
                                  (p->p_sechdr[i].sh_type == SHT_RELA))
        {                                  
            // Calculate the total number of entries in the .rela section.
            r_entries = p->p_sechdr[i].sh_size / p->p_sechdr[i].sh_entsize;

            diag_printf("\n\nSymbols at: %s\n\n", 
                         p_shstrtab + p->p_sechdr[i].sh_name);
#if ELF_ARCH_RELTYPE == Elf_Rela        
            p_rela = (Elf32_Rela*)cyg_ldr_load_elf_section(p, i);
            printf("Offset    Info      Name [+ Addend]\n");
#else
            p_rel = (Elf32_Rel*)cyg_ldr_load_elf_section(p, i);
            printf("Offset    Info     Name\n");
#endif

            for (j = 0; j < r_entries; j++)
            {
                sprintf(strname, 
                         "%08X  %08X  ", 
#if ELF_ARCH_RELTYPE == Elf_Rela        
                        p_rela[j].r_offset,
                        p_rela[j].r_info 
#else
                        p_rel[j].r_offset,
                        p_rel[j].r_info 
#endif
                        );

                diag_printf(strname);         

#if ELF_ARCH_RELTYPE == Elf_Rela        
                cyg_uint8 sym_type = ELF32_R_SYM(p_rela[j].r_info);
#else
                cyg_uint8 sym_type = ELF32_R_SYM(p_rel[j].r_info);
#endif
                if (strlen (p_strtab + p_symtab[sym_type].st_name) > 0)
                    diag_printf(p_strtab + p_symtab[sym_type].st_name);         
                else 
                {   
                    // If the symbol name is not available, then print
                    //  the name of the section.
                    sym_index = p_symtab[sym_type].st_shndx;                    
                    diag_printf(p_shstrtab + p->p_sechdr[sym_index].sh_name);         
                }    
#if ELF_ARCH_RELTYPE == Elf_Rela        
                if (p_rela[j].r_addend != 0)
                    diag_printf(" + %08X", p_rela[j].r_addend);
#endif
                diag_printf("\n");         
            }            
            // After all the printing is done, the relocation table can 
            //  be dumped.
            cyg_ldr_delete_elf_section(p, i);
        } 
    }    
}
#endif // DEBUG_PRINT

static void
*cyg_ldr_local_address(PELF_OBJECT p, cyg_uint32 sym_index)
{
    cyg_uint32 data_sec, addr;
    Elf32_Sym *p_symtab;

    p_symtab = (Elf32_Sym*)cyg_ldr_section_address(p, p->hdrndx_symtab);
    
    // Find out the section number in which the data for this symbol is 
    //  located.
    data_sec = p_symtab[sym_index].st_shndx;    

    // From the section number we get the start of the memory area in 
    //  memory.
    addr = (cyg_uint32)cyg_ldr_section_address(p, data_sec);

    // And now return the address of the data.
    return (void*)(addr + p_symtab[sym_index].st_value);
}    

void
*cyg_ldr_external_address(PELF_OBJECT p, cyg_uint32 sym_index)
{
    cyg_uint8*    tmp2;
    Elf32_Sym *p_symtab;
    cyg_uint8 *p_strtab;
    cyg_ldr_table_entry *entry = cyg_ldr_table;
  
  
    p_symtab = (Elf32_Sym*)cyg_ldr_section_address(p, p->hdrndx_symtab);
    p_strtab = (cyg_uint8*)cyg_ldr_section_address(p, p->hdrndx_strtab);
  
    // This is the name of the external reference to search.
    tmp2 = p_strtab + p_symtab[sym_index].st_name;
    while (entry != cyg_ldr_table_end)
    {
        if (!strcmp((const char*)tmp2, entry->symbol_name ))
            return entry->handler;
        entry++;
    }

    // Symbol not found.
    return 0;
}

// input:
// p          : Pointer to the elf file object
// sym_index  : Index of the symbol to be searched (in the SYMTAB)
//
// out:
// 0          : Symbol not found
// Other      : Address of the symbol in absolute memory.
void 
*cyg_ldr_symbol_address(PELF_OBJECT p, cyg_uint32 sym_index)
{
    cyg_uint32 addr;
    Elf32_Sym *p_symtab = (Elf32_Sym*)cyg_ldr_section_address(p, 
                                                              p->hdrndx_symtab);
    cyg_uint8 sym_info = p_symtab[sym_index].st_info;
    switch (ELF32_ST_TYPE(sym_info))
    {
    case STT_NOTYPE:
    case STT_FUNC:
    case STT_OBJECT:
        switch (ELF32_ST_BIND(sym_info))
        {
        case STB_LOCAL:
        case STB_GLOBAL:
            if (p_symtab[sym_index].st_shndx == SHN_UNDEF) 
                return cyg_ldr_external_address(p, sym_index);
            else
                return cyg_ldr_local_address(p, sym_index);
        case STB_WEAK:
            addr = (cyg_uint32)cyg_ldr_external_address(p, sym_index);
            if (addr != 0)
                return (void*)addr;
            else    
                return cyg_ldr_local_address(p, sym_index);
        default:
            return 0;
        }
        break;
    case STT_SECTION:
        // Return the starting address of a section, given its index.
        return (void*)cyg_ldr_section_address(p, p_symtab[sym_index].st_shndx);
    default:
        return 0;
    }
}

// Loads the relocation information, relocates, and dumps the relocation
//  information once the process is complete.
cyg_int32 
cyg_ldr_relocate_section(PELF_OBJECT p, cyg_uint32 r_shndx)
{
    int         i, rc;
#if ELF_ARCH_RELTYPE == Elf_Rela        
    Elf32_Rela* p_rela = (Elf32_Rela*)cyg_ldr_load_elf_section(p, r_shndx);
#else
    Elf32_Rel*  p_rel = (Elf32_Rel*)cyg_ldr_load_elf_section(p, r_shndx);
#endif

#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 0
    Elf32_Sym *p_symtab = (Elf32_Sym*)cyg_ldr_section_address(p, 
                                                           p->hdrndx_symtab);
    char      *p_strtab = (char*)cyg_ldr_section_address(p, p->hdrndx_strtab);
    char      *p_shstrtab = (char*)cyg_ldr_section_address(p, 
                                                     p->p_elfhdr->e_shstrndx);
#endif

    // Now we can get the address of the contents of the section to modify.
    cyg_uint32 r_target_shndx = p->p_sechdr[r_shndx].sh_info;
    cyg_uint32 r_target_addr  = (cyg_uint32)cyg_ldr_section_address(p, 
                                                                r_target_shndx);

#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 0
    diag_printf("Relocating section \"%s\"\n",
            p_shstrtab + p->p_sechdr[r_target_shndx].sh_name);
#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 1
    diag_printf("Ndx   Type   Offset    Name\"\n");
#endif
#endif

    // Perform relocatation for each of the members of this table.
    cyg_uint32 r_entries = p->p_sechdr[r_shndx].sh_size / 
                                             p->p_sechdr[r_shndx].sh_entsize;
    for (i = 0; i < r_entries; i++)
    {
#if ELF_ARCH_RELTYPE == Elf_Rela        
        Elf32_Addr  r_offset = p_rela[i].r_offset; 
        Elf32_Word  r_type   = ELF32_R_TYPE(p_rela[i].r_info); 
        cyg_uint32  sym_index = ELF32_R_SYM(p_rela[i].r_info);
        Elf32_Sword r_addend  = p_rela[i].r_addend; 
#else
        Elf32_Addr  r_offset  = p_rel[i].r_offset; 
        Elf32_Word  r_type    = ELF32_R_TYPE(p_rel[i].r_info); 
        cyg_uint32  sym_index = ELF32_R_SYM(p_rel[i].r_info);
        Elf32_Sword r_addend  = 0; 
#endif

        cyg_uint32 sym_value = (cyg_uint32)cyg_ldr_symbol_address(p, sym_index);
        if (sym_value == 0)
        {
#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 0
            diag_printf("Unknown symbol value: %s Index: %d\n",
                         p_strtab + p_symtab[sym_index].st_name,
                         sym_index);
#endif
            return -1;
        }    
        
        // This is architecture dependent, and deals with whether we have
        //  '.rel' or '.rela' sections.
#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 1
        diag_printf("%04X  %04X  %08X  ",
                     sym_index,
                     r_type,
                     r_offset);
        if (strlen(p_strtab + p_symtab[sym_index].st_name) > 0)
            diag_printf(p_strtab + p_symtab[sym_index].st_name);         
        else 
        {   
            // If the symbol name is not available, then print
            //  the name of the section.
            cyg_uint32 sec_ndx = p_symtab[sym_index].st_shndx;                    
            diag_printf(p_shstrtab + p->p_sechdr[sec_ndx].sh_name);         
        }    
        diag_printf("\n");         
#endif
        rc = cyg_ldr_relocate(r_type,
                               r_target_addr + r_offset, 
                               sym_value + r_addend);
        if (rc != 0)
        {
#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 0
            diag_printf("Relocation error: Cannot find symbol: %s\n",
                      p_strtab + p_symtab[sym_index].st_name);
#endif
            return -1;
        }    
    }

    // After the relocation is done, the relocation table can be dumped.
    cyg_ldr_delete_elf_section(p, r_shndx);
    return 0;
}

