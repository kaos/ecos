/* =================================================================
 *
 *      objelf.c
 *
 *      An object loader for eCos
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
#include <cyg/infra/cyg_ass.h>
#include <stdlib.h>
#include <string.h>

#include <pkgconf/objloader.h>

#include <cyg/objloader/elf.h>
#include <cyg/objloader/objelf.h>
#include <cyg/objloader/loader_fs.h>

char *cyg_ldr_last_error;

void *cyg_ldr_malloc(size_t) CYGBLD_ATTRIB_WEAK;
void
*cyg_ldr_malloc(size_t s)
{
    return malloc(s);
}

void cyg_ldr_free(void *) CYGBLD_ATTRIB_WEAK;
void
cyg_ldr_free(void *s)
{
    free(s);
}

void
cyg_ldr_delete_elf_section(PELF_OBJECT p, cyg_uint32 idx)
{
    cyg_ldr_free(p->sections[idx]);
    p->sections[idx] = 0; 
}    

// Frees all the memory allocated for a particular ELF object. Also calls
//  the close() function to close files or sockets, and finally frees up
//  the ELF object altogether.
static void
cyg_ldr_free_elf_object(PELF_OBJECT p)
{
    cyg_int32 i;
        
    for (i = 0; i < p->p_elfhdr->e_shnum + 1; i++)
        if (p->sections[i])
            cyg_ldr_delete_elf_section(p, i);

    if (p->sections != 0)
        cyg_ldr_free(p->sections); 

    if (p->p_sechdr != 0)
        cyg_ldr_free(p->p_sechdr); 

    if (p->p_elfhdr != 0)
        cyg_ldr_free(p->p_elfhdr); 

    p->close(p);    
    cyg_ldr_free(p);
}        

static cyg_uint32 
cyg_ldr_find_common_size(PELF_OBJECT p)
{
    cyg_int32 i, common_size = 0;
    Elf32_Sym *p_symtab = (Elf32_Sym*)p->sections[p->hdrndx_symtab];
     
    // Total number of entries in the symbol table.
    int symtab_entries = p->p_sechdr[p->hdrndx_symtab].sh_size / 
                                p->p_sechdr[p->hdrndx_symtab].sh_entsize;
    for (i = 1; i < symtab_entries; i++)
        if (p_symtab[i].st_shndx == SHN_COMMON)
        {
            // In the case of an SHN_COMMON symbol the st_value field holds 
            //  alignment constraints.
            cyg_uint32 boundary = p_symtab[i].st_value - 1;

            // Calculate the next byte boundary.
            common_size = (common_size + boundary) & ~boundary;
            common_size += p_symtab[i].st_size;
        }    

#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 0
    diag_printf("common_size = %d\n", common_size);
#endif
    return common_size;
}

// Allocates memory and loads the contents of a specific ELF section.
// Returns the address of the newly allocated memory, of 0 for any error.
cyg_uint32 
*cyg_ldr_load_elf_section(PELF_OBJECT p, cyg_uint32 idx)
{
    cyg_uint32 *addr = (cyg_uint32 *)cyg_ldr_malloc(p->p_sechdr[idx].sh_size);
    CYG_ASSERT(addr != 0, "Cannot malloc() section");
    if (addr == 0)
    {
        cyg_ldr_last_error = "ERROR IN MALLOC";
        return (void*)0;
    }
    p->seek(p, p->p_sechdr[idx].sh_offset);
    p->read(p, sizeof(char), p->p_sechdr[idx].sh_size, addr);
    return addr;
}    

// Returns the starting address of a section. If the section is not already
//  loaded in memory, area for it will be allocated and the section will be
//  loaded.
cyg_uint32 
*cyg_ldr_section_address(PELF_OBJECT p, cyg_uint32 idx)
{
    if (p->sections[idx] == 0)
        p->sections[idx] = cyg_ldr_load_elf_section(p, idx);

    return p->sections[idx];
}

void
*cyg_ldr_find_symbol(void* handle, char* sym_name)
{
    PELF_OBJECT p = (PELF_OBJECT)handle;
    int         i;
    char *p_strtab = (char*)p->sections[p->hdrndx_strtab];
    Elf32_Sym *p_symtab = (Elf32_Sym*)p->sections[p->hdrndx_symtab];
 
    int symtab_entries = p->p_sechdr[p->hdrndx_symtab].sh_size / 
                               p->p_sechdr[p->hdrndx_symtab].sh_entsize;

    for (i = 0; i < symtab_entries; i++)
    {
        char* tmp2 = p_strtab + p_symtab[i].st_name;
        if (!strcmp(tmp2, sym_name))
            return cyg_ldr_symbol_address(p, i);
    }

    // Symbol not found.
    cyg_ldr_last_error = "SYMBOL NOT FOUND";
    return 0;
}

static char
*cyg_ldr_sanity_check(PELF_OBJECT p)
{
    if ((p->p_elfhdr->e_ident[EI_MAG0] != ELFMAG0)  || 
         (p->p_elfhdr->e_ident[EI_MAG1] != ELFMAG1)  ||
         (p->p_elfhdr->e_ident[EI_MAG2] != ELFMAG2 ) || 
         (p->p_elfhdr->e_ident[EI_MAG3] != ELFMAG3)  || 
         (p->p_elfhdr->e_ident[EI_CLASS] != ELFCLASS32))
        return "INVALID ELF HEADER";

    // We only work with relocatable files. No dynamic linking.
    if (p->p_elfhdr->e_type != ET_REL)
        return "NOT RELOCATABLE";
        
    // These #defines are sitting in the hal.
    if (p->p_elfhdr->e_machine != ELF_ARCH_MACHINE_TYPE)
        return "INVALID ARCHITECTURE";

    if (p->p_elfhdr->e_ident[EI_DATA] != ELF_ARCH_ENDIANNESS)
        return "INVALID ENDIAN";
    return 0;  }     

// Load only the ELF header and the sections header. These are the only
//  sections loaded during library initialization. All the other sections
//  will be loaded on demand when needed during the relocation process and,
//  when possible, dumped after use.
static cyg_int32 
cyg_ldr_load_sections(PELF_OBJECT p)
{
    char      *error_string;
    cyg_int32  idx;

    // Load the ELF header.
    p->p_elfhdr = (Elf32_Ehdr*)cyg_ldr_malloc(sizeof(Elf32_Ehdr));
    CYG_ASSERT(p->p_elfhdr != 0, "Cannot malloc() p->p_elfhdr");
    if (p->p_elfhdr == 0)
        return -1;
    p->seek(p, 0);
    p->read(p, sizeof(char), sizeof(Elf32_Ehdr), p->p_elfhdr );
    error_string = cyg_ldr_sanity_check(p);
    if (error_string != 0)
    {
        cyg_ldr_last_error = "ERROR IN ELF HEADER";
        return -1;
    }    
    
    // Allocate an array that can hold an address to all the section of this
    //  library. This is not strictly optimal, since some sections do not
    //  need to be loaded all the time. Allocate an extra pointer for the
    //  COMMON area. 
    p->sections = cyg_ldr_malloc((p->p_elfhdr->e_shnum + 1) * 
                                                     sizeof(cyg_uint32));
    CYG_ASSERT(p->sections != 0, "Cannot malloc() p->sections");
    if (p->sections == 0)
    {
        cyg_ldr_last_error = "ERROR IN MALLOC";
        return -1;
    }
    memset(p->sections, 0, (p->p_elfhdr->e_shnum + 1) * 
                                                     sizeof(cyg_uint32));
    
    // Now that the header is loaded, load the sections header.
    p->p_sechdr = (Elf32_Shdr*)cyg_ldr_malloc(
                             p->p_elfhdr->e_shnum * p->p_elfhdr->e_shentsize);
    CYG_ASSERT(p->p_sechdr != 0, "Cannot malloc() p->p_sechdr");
    if (p->p_sechdr == 0)
    {
        cyg_ldr_last_error = "ERROR IN MALLOC";
        return -1;
    }
    p->seek(p, p->p_elfhdr->e_shoff);
    p->read(p, p->p_elfhdr->e_shentsize, p->p_elfhdr->e_shnum, p->p_sechdr);
    
    // Load the section header string table. This is a byte oriented table,
    //  so alignment is not an issue.
    idx = p->p_elfhdr->e_shstrndx;
    p->sections[idx] = cyg_ldr_load_elf_section(p, idx);
    return 0;
}

PELF_OBJECT
cyg_ldr_open_library(CYG_ADDRWORD ptr, cyg_int32 mode)
{
    int (*fn)(void);
    int         i;

    // In the future there might be a switch() (against 'mode') that calls an
    //  open function other than cyg_ldr_open_library_fs(). These function
    //  fetch and open a library using ftp, http or libraries that are already 
    //  in ROM.
    PELF_OBJECT e_obj = cyg_ldr_open_library_fs((char*)ptr);
    if (e_obj == 0)
        return 0;
    int rc = cyg_ldr_load_sections(e_obj);
    if (rc != 0)
    {
        cyg_ldr_free_elf_object(e_obj);
        return 0;
    }    

    // Find the section index for the .shstrtab section. The names of the 
    //  sections are held here, and are the only way to identify them.
    char *p_shstrtab = (char*)cyg_ldr_section_address(e_obj, 
                                                e_obj->p_elfhdr->e_shstrndx);
    if (p_shstrtab == 0)
    {
        cyg_ldr_free_elf_object(e_obj);
        return 0;
    }    

    // .symtab section and .strtab. We have to go through the section names
    //  to find where they are.
    for (i = 1; i < e_obj->p_elfhdr->e_shnum; i++)
    {
        // Now look for the index of .symtab. These are the symbols needed for 
        //  the symbol retrieval as well as relocation.
        if (!strcmp(p_shstrtab + e_obj->p_sechdr[i].sh_name, ELF_STRING_symtab))
        {              
            e_obj->hdrndx_symtab = i;
            e_obj->sections[i] = cyg_ldr_load_elf_section(e_obj, i);
            if (e_obj->sections[i] == 0)
            {
                cyg_ldr_free_elf_object(e_obj);
                return 0;
            }    
        }    
        
        // Load the table with the names of all the symbols. We need this
        //  to compare the name of external references symbols against the
        //  names in the in the CYG_HAL_TABLE provided by the user.
        if (!strcmp(p_shstrtab + e_obj->p_sechdr[i].sh_name, ELF_STRING_strtab))
        {              
            e_obj->hdrndx_strtab = i;
            e_obj->sections[i] = cyg_ldr_load_elf_section(e_obj, i);
            if (e_obj->sections[i] == 0)
            {
                cyg_ldr_free_elf_object(e_obj);
                return 0;
            }    
        }    
    }                                              

    CYG_ASSERT(e_obj->hdrndx_symtab != 0, "No symtab index found");
    CYG_ASSERT(e_obj->hdrndx_strtab != 0, "No strtab index found");

    // Now look for symbols in the COMMON area. The COMMON symbols are a 
    //  special case, because the area they reside in must be sized up
    //  and allocated separately from the other sections, which appear in
    //  the sections header and can be read out of the library itself.
    // Extra room in the 'sections' array has already been allocated to hold 
    //  the pointer to the commons area.
    cyg_uint32 common_size = cyg_ldr_find_common_size(e_obj); 
    if (common_size != 0)
    {
        cyg_uint32 com_shndx = e_obj->p_elfhdr->e_shnum;
        cyg_int32  com_offset = 0;
        
        e_obj->sections[com_shndx] = (cyg_uint32*)cyg_ldr_malloc(common_size);
        CYG_ASSERT(e_obj->sections[com_shndx] != 0, 
                    "Cannot malloc() the COMMONS");
   
        if (e_obj->sections[com_shndx] == 0)
        {
            cyg_ldr_free_elf_object(e_obj);
            return 0;
        }    

        // Now find all the symbols in the SHN_COMMON area and make 
        //  them  point to the newly allocated COM area.
        int symtab_entries = e_obj->p_sechdr[e_obj->hdrndx_symtab].sh_size / 
                              e_obj->p_sechdr[e_obj->hdrndx_symtab].sh_entsize;
        Elf32_Sym *p_symtab = (Elf32_Sym*)e_obj->sections[e_obj->hdrndx_symtab];
    
#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 1
        diag_printf("Num   Value   Size Ndx Name\n"); 
#endif
        for (i = 1; i < symtab_entries; i++)
        {
#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 1
            cyg_uint8 *p_strtab = (cyg_uint8*)cyg_ldr_section_address(e_obj, 
                                                        e_obj->hdrndx_strtab);
#endif        
            if (p_symtab[i].st_shndx == SHN_COMMON)
            {             
                cyg_uint32 boundary = p_symtab[i].st_value - 1;
                // Calculate the next byte boundary.
                com_offset = (com_offset + boundary) & ~boundary;
                p_symtab[i].st_shndx = com_shndx;
                p_symtab[i].st_value = com_offset;
                com_offset += p_symtab[i].st_size;
            }
    
#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 1
            diag_printf("%03d  %08X %04X %03d %s\n", 
                         i, 
                         p_symtab[i].st_value,
                         p_symtab[i].st_size,
                         p_symtab[i].st_shndx,
                         p_strtab + p_symtab[i].st_name);
#endif        
        }    
    }

#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 0
    cyg_ldr_print_section_data(e_obj);
#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 1
    cyg_ldr_print_symbol_names(e_obj);
#endif    
#endif    

    for (i = 1; i < e_obj->p_elfhdr->e_shnum; i++)
    {
        // Find all the '.rel' or '.rela' sections and relocate them.
        if ((e_obj->p_sechdr[i].sh_type == SHT_REL) ||
                                  (e_obj->p_sechdr[i].sh_type == SHT_RELA))
        {
            // Load and relocate the section.
            rc = cyg_ldr_relocate_section(e_obj, i);
            if (rc < 0)
            { 
#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 0
                ELFDEBUG("Relocation unsuccessful\n");
#endif
                cyg_ldr_free_elf_object(e_obj);
                return 0;
            }    
        }    
    }    

    // Synch up the caches before calling any function in the library.
    cyg_ldr_flush_cache();

    // Run the library initialization code.
    fn = cyg_ldr_find_symbol(e_obj, "library_open");
    if (fn != 0)
        fn();
    return ((void*)e_obj);
}

char
*cyg_ldr_error(void)
{
    char* p = cyg_ldr_last_error;
    cyg_ldr_last_error = NULL;
    return p;
}

void cyg_ldr_close_library(void* handle)
{
    void (*fn)(void);

    PELF_OBJECT p = (PELF_OBJECT)handle;
    fn = cyg_ldr_find_symbol(p, "library_close");
    if (fn != 0)
        fn();

    cyg_ldr_free_elf_object(p);
    p = 0;
}

