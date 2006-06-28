/* =================================================================
 *
 *      loader_fs.c
 *
 *      Routines to read a library from a file system.
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
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <pkgconf/objloader.h>
#include <cyg/objloader/elf.h>
#include <cyg/objloader/objelf.h>
#include <cyg/objloader/loader_fs.h>

size_t 
cyg_ldr_fs_read(PELF_OBJECT p, size_t s, size_t n, void *mem)
{
    return fread(mem, s, n, (FILE*)p->ptr);
}

cyg_int32 
cyg_ldr_fs_seek(PELF_OBJECT p, cyg_uint32 offs)
{
    return fseek((FILE*)p->ptr, offs, SEEK_SET);
}

cyg_int32 
cyg_ldr_fs_close(PELF_OBJECT p)
{
    return fclose((FILE*)p->ptr);
}

PELF_OBJECT
cyg_ldr_open_library_fs(char *ptr)
{
    FILE *fp = fopen(ptr, "rb");
    if (fp == NULL)
    {
        cyg_ldr_last_error = "FILE NOT FOUND";
        return (void*)0;
    }
    
    // Create a file object to keep track of this library.
    PELF_OBJECT e_obj = (PELF_OBJECT)malloc(sizeof(ELF_OBJECT));
    CYG_ASSERT(e_obj != 0, "Cannot malloc() e_obj");
    if (e_obj == 0)
    {
        cyg_ldr_last_error = "ERROR IN MALLOC";
        fclose(fp); 
        return (void*)0;
    }
    memset(e_obj, 0, sizeof(ELF_OBJECT));
    e_obj->ptr   = (CYG_ADDRWORD)fp;
    e_obj->mode  = CYG_LDR_MODE_FILESYSTEM;

    // Handlers for the file system open.
    e_obj->read  = cyg_ldr_fs_read;
    e_obj->seek  = cyg_ldr_fs_seek;
    e_obj->close = cyg_ldr_fs_close;
    return e_obj;
}

void
cyg_ldr_close_library_fs(PELF_OBJECT p)
{
}

