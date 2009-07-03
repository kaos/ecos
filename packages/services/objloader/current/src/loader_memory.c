/* =================================================================
 *
 *      loader_memory.c
 *
 *      Routines to read a library from memory.
 *
 * ================================================================= 
 * ####ECOSGPLCOPYRIGHTBEGIN####
 * -------------------------------------------
 * This file is part of eCos, the Embedded Configurable Operating
 * System.
 * Copyright (C) 2006, 2008 Free Software Foundation, Inc.
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
 *  Author(s):    Gernot Zankl zankl@decomsys.com
 *  Contributors: 
 *  Date:         2006-11-21
 *  Purpose:      
 *  Description:  
 *               
 * ####DESCRIPTIONEND####
 * 
 * =================================================================
 */

#include <cyg/infra/diag.h>     // For diagnostic printing.
#include <cyg/infra/cyg_ass.h>  // CYG_ASSERT

#include <stdio.h>
#include <stdlib.h>

#include <pkgconf/objloader.h>
#include <cyg/objloader/elf.h>
#include <cyg/objloader/objelf.h>
#include <cyg/objloader/loader_memory.h>

typedef struct
{
    CYG_ADDRWORD nBufferBase;
    cyg_uint32   nOffset;
} ObjLoader_MemInfoType;

static size_t 
cyg_ldr_memory_read(PELF_OBJECT p, size_t s, size_t n, void *mem)
{
    ObjLoader_MemInfoType * const pMemInfo = (ObjLoader_MemInfoType *)p->ptr;

    cyg_uint8 * const pSource = 
        (cyg_uint8 *)pMemInfo->nBufferBase + pMemInfo->nOffset;

    memcpy(mem, (void*)pSource, s*n);
    return n;
}

static cyg_int32 
cyg_ldr_memory_seek(PELF_OBJECT p, cyg_uint32 offs)
{
    ObjLoader_MemInfoType * const pMemInfo = (ObjLoader_MemInfoType *)p->ptr;
    pMemInfo->nOffset = offs;
    return 0;
}

static cyg_int32 
cyg_ldr_memory_close(PELF_OBJECT p)
{
    return 0;
}

PELF_OBJECT
cyg_ldr_open_library_memory(CYG_ADDRWORD ptr)
{
    PELF_OBJECT  e_obj; 
    ObjLoader_MemInfoType * pMemInfo;

    if (ptr == 0)
    {
        cyg_ldr_last_error = "ERROR INVALID POINTER";
        return (void*)0;
    }
    
    // Create a file object to keep track of this library.
    e_obj = (PELF_OBJECT)malloc(sizeof(ELF_OBJECT));
    CYG_ASSERT(e_obj != 0, "Cannot malloc() e_obj");
    if (e_obj == 0)
    {
        cyg_ldr_last_error = "ERROR IN MALLOC";
        return (void*)0;
    }
    memset(e_obj, 0, sizeof(ELF_OBJECT));

    pMemInfo = (ObjLoader_MemInfoType *)malloc(sizeof(ObjLoader_MemInfoType));
    CYG_ASSERT(pMemInfo != 0, "Cannot malloc() pMemInfo");
    if (pMemInfo == 0)
    {
        cyg_ldr_last_error = "ERROR IN MALLOC";
        return (void*)0;
    }

    pMemInfo->nBufferBase = ptr;
    pMemInfo->nOffset = 0;

    e_obj->ptr   = (CYG_ADDRWORD)pMemInfo;
    e_obj->mode  = CYG_LDR_MODE_MEMORY;

    // Handlers for the file system open.
    e_obj->read  = cyg_ldr_memory_read;
    e_obj->seek  = cyg_ldr_memory_seek;
    e_obj->close = cyg_ldr_memory_close;
    return e_obj;
}

void
cyg_ldr_close_library_memory(PELF_OBJECT p)
{
    free((ObjLoader_MemInfoType *) p->ptr);
}

