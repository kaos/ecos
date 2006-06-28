#ifndef __RELOCATE_I386_H__
#define __RELOCATE_I386_H__

/* =================================================================
 *
 *      relocate_i386.h
 *
 *      Architecture dependent relocation routines for the i386
 *
 * ================================================================= 
 * ####ECOSGPLCOPYRIGHTBEGIN####
 * -------------------------------------------
 * This file is part of eCos, the Embedded Configurable Operating
 * System.
 * Copyright (C) 2005 Andrew Lunn <andrew.lunn@ascom.ch>
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
 *  Author(s):    andrew.lunn@ascom.ch
 *  Date:         2005-07-07
 *  Purpose:      
 *  Description:  
 *               
 * ####DESCRIPTIONEND####
 * 
 * =================================================================
 */

#define Elf_Rel                0
#define Elf_Rela               1

#define ELF_ARCH_MACHINE_TYPE  3    // i386
#define ELF_ARCH_ENDIANNESS    ELFDATA2LSB
#define ELF_ARCH_RELTYPE       Elf_Rel

#define R_386_32               1
#define R_386_PC32             2

void      cyg_ldr_flush_cache(void);
cyg_int32 cyg_ldr_relocate(cyg_int32, cyg_uint32, cyg_int32);

#endif //__RELOCATE_I386_H__
