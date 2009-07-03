#ifndef __RELOCATE_ARM_H__
#define __RELOCATE_ARM_H__

/* =================================================================
 *
 *      relocate_arm.h
 *
 *      Architecture dependent relocation routines for the ARM
 *
 * ================================================================= 
 * ####ECOSGPLCOPYRIGHTBEGIN####
 * -------------------------------------------
 * This file is part of eCos, the Embedded Configurable Operating System.
 * Copyright (C) 2008 Free Software Foundation, Inc.
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
 *  Contributors: Sergei Gavrikov (sergei.gavrikov@gmail.com)
 *  Date:         2008-12-01
 *  Purpose:      
 *  Description:  
 *               
 * ####DESCRIPTIONEND####
 * 
 * =================================================================
 */

#define Elf_Rel                0
#define Elf_Rela               1

#define ELF_ARCH_MACHINE_TYPE  40    // ARM
#define ELF_ARCH_ENDIANNESS    ELFDATA2LSB
#define ELF_ARCH_RELTYPE       Elf_Rel

#define R_ARM_PC24             1  // PC relative 26 bit branch.
#define R_ARM_ABS32            2  // Direct 32 bit.
#define R_ARM_CALL             28 // PC relative 26 bit call (EABI).
#define R_ARM_JUMP24           29 // PC relative 26 bit branch (EABI).
#define R_ARM_V4BX             40 // Fix of interworking for ARMv4 cores.

void      cyg_ldr_flush_cache(void);
cyg_int32 cyg_ldr_relocate(cyg_int32, cyg_uint32, cyg_int32);
extern char *relocation_name[];

#endif //__RELOCATE_ARM_H__
