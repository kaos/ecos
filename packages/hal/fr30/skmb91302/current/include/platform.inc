#ifndef CYGONCE_HAL_PLATFORM_INC
#define CYGONCE_HAL_PLATFORM_INC
##=============================================================================
##
##	platform.inc
##
##	Fujitsu Starterkit MB91302 board assembler header file
##
##=============================================================================
## ####ECOSGPLCOPYRIGHTBEGIN####                                            
## -------------------------------------------                              
## This file is part of eCos, the Embedded Configurable Operating System.   
## Copyright (C) 1998, 1999, 2000, 2001, 2002, 2007 Free Software Foundation, Inc.
##
## eCos is free software; you can redistribute it and/or modify it under    
## the terms of the GNU General Public License as published by the Free     
## Software Foundation; either version 2 or (at your option) any later      
## version.                                                                 
##
## eCos is distributed in the hope that it will be useful, but WITHOUT      
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
## FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
## for more details.                                                        
##
## You should have received a copy of the GNU General Public License        
## along with eCos; if not, write to the Free Software Foundation, Inc.,    
## 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
##
## As a special exception, if other files instantiate templates or use      
## macros or inline functions from this file, or you compile this file      
## and link it with other works to produce a work based on this file,       
## this file does not by itself cause the resulting work to be covered by   
## the GNU General Public License. However the source code for this file    
## must still be made available in accordance with section (3) of the GNU   
## General Public License v2.                                               
##
## This exception does not invalidate any other reasons why a work based    
## on this file might be covered by the GNU General Public License.         
## -------------------------------------------                              
## ####ECOSGPLCOPYRIGHTEND####                                              
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):   larsi
## Contributors:
## Date:        2007-07-09
## Purpose:     Fujitsu Starterkit MB91302 board definitions.
## Description: This file contains various definitions and macros that are
##              useful for writing assembly code for the skmb91302 board.
## Usage:
##		#include <cyg/hal/platform.inc>
##
##
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <cyg/hal/fr30.inc>


##------------------------------------------------------------------------------
## ext Bus (memory controller) initialisation macros
##
## flash part


#define CYGPKG_HAL_FR30_FLASH_INIT_DEFINED
## flash init is empty for this platform because it is initialized in the
## special hal_fr30_ram_startup_trampoline to map flash to 0x1000000
.macro hal_flash_init
.endm

## and the following macro is used in the special
## hal_fr30_ram_startup_trampoline for flash initialisation
.macro hal_flash_init_from_ram

    ldi:20  #FR30_MB91301_ASR0, r10 ; CS0 area starts at
    ldi:20  0x100,   r0             ; 0x01000000
    sth     r0,     @r10            ;

    ldi:20  #FR30_MB91301_ACR0, r11 ; configuration parameters for CS0
    ldi:20  #0x7422,    r1          ; 8MB(0x0-0x7FFFFF),16bit data bus,
    sth     r1,     @r11            ; pre-fetch off, single access,
                                    ; write enable, big endian,
                                    ; normal access(asynchronous),
                                    ; WR pin enabled for write,
                                    ; wait by RDY pin disabled

    ldi:20  #FR30_MB91301_AWR0, r12 ; first access auto-wait 3cyc
    ldi:20  #0x3378,    r2          ; inpage access auto-wait 3cyc
    sth     r2,     @r12            ; read/write idle 1cyc
                                    ; write recover 3cyc
                                    ; async write strobe outp enabled
                                    ; CS delay enabled
                                    ; CS read/write setup delay 0
                                    ; RD/WR -> CS hold extension 0 cyc

    ldi:20  #FR30_MB91301_PFR9, r13 ; WRn, BAAE, ASXE enable
    ldi:8   #0x7e,  r3              ;
    stb     r3,     @r13            ;

## following would chip select enable
## we only use cs0 until here, which is already set by reset

.endm


## sdram part


#define CYGPKG_HAL_FR30_MEMC_INIT_DEFINED

.macro hal_memc_init

## The following instruction is without function. It is only to reference
## hal_fr30_rom_startup_trampoline, because when not referenced the linker
## does not include the file platform.S in the link. If somewhen some other
## referenced code appears in platform.S the instruction here can be deleted
## (including the surrounding macro)!

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)
    ldi:32  #hal_fr30_rom_startup_trampoline, r10
#endif

    ldi:20  #FR30_MB91301_ASR6, r10 ; CS6 area starts at
    ldi:20  0x200,   r0             ; 0x02000000
    sth     r0,     @r10            ;

    ldi:20  #FR30_MB91301_ACR6, r11 ; configuration parameters for CS6
    ldi:20  #0x7868,    r1          ; 64MB(0x2000000-0x27FFFFF),32bit data bus,
    sth     r1,     @r11            ; pre-fetch on, single access(no burst),
                                    ; write enable, big endian,
                                    ; FCRAM setting

    ldi:20  #FR30_MB91301_AWR6, r12 ; first access auto-wait 1cyc ??
    ldi:20  #0x1159,    r2          ; inpage access auto-wait 1cyc ??
    sth     r2,     @r12            ; read/write idle 1cyc ??
                                    ; write recover 1cyc ??
                                    ; async write strobe outp enabled ??
                                    ; CS delay disabled ??
                                    ; CS read/write setup delay 0 ??
                                    ; RD/WR -> CS hold extension 1 cyc ??
                                    ; see Hardware Manual page 156ff

    ldi:20  #FR30_MB91301_MCRA, r13 ; 8 columns, single write,
    ldi:8   #0x07,  r3              ; 4 banks for burst write,
    stb     r3,     @r13            ; 4 active banks

    ldi:20  #FR30_MB91301_PFR9, r10 ; enable WRn, BAAE, ASXE,
    ldi:8   #0x7e,  r0              ; sysclk, MCKE, MCKEE
    stb     r0,     @r10

    ldi:20  #FR30_MB91301_PFR8, r11 ; enable WR3XE, WR2XE, WR1XE
    ldi:8   #0xe0,  r1
    stb     r1, @r11

    ldi:20  #FR30_MB91301_CSER, r12 ; switch on CS6 & CS0
    ldi:8   #0x41,  r2
    stb     r2,     @r12

    ldi:20  #FR30_MB91301_RCR,  r13 ; power on SDRAM I/F
    ldi:20  #0xe247,    r3          ; 0xe247 -> 0xe24f
    sth     r3,     @r13
    ldi:20  #0xe24f,    r3
    sth     r3,     @r13

.endm

#------------------------------------------------------------------------------
## Vector table for storage. platform.S defines a vector table and wants to
## override the one from variant.S with this define

#define CYGPKG_HAL_FR30_ROM_VECTORS_DEFINED

#------------------------------------------------------------------------------
# Difference of the flash memory from the linkers LMA (loadmemoryaddress) after
# the new mapping in (mapping is done in hal_fr30_ram_startup_trampoline).

#define CYGPKG_HAL_FR30_LMA_OFFSET  0x1000000



#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_PLATFORM_INC
# end of platform.inc
