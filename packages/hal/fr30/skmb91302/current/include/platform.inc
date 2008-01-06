#ifndef CYGONCE_HAL_PLATFORM_INC
#define CYGONCE_HAL_PLATFORM_INC
##=============================================================================
##
##	platform.inc
##
##	Fujitsu Starterkit MB91302 board assembler header file
##
##=============================================================================
#####ECOSGPLCOPYRIGHTBEGIN####
## -------------------------------------------
## This file is part of eCos, the Embedded Configurable Operating System.
## Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
## Copyright (C) 2007 eCosCentric Ltd.
##
## eCos is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free
## Software Foundation; either version 2 or (at your option) any later version.
##
## eCos is distributed in the hope that it will be useful, but WITHOUT ANY
## WARRANTY; without even the implied warranty of MERCHANTABILITY or
## FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
## for more details.
##
## You should have received a copy of the GNU General Public License along
## with eCos; if not, write to the Free Software Foundation, Inc.,
## 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
##
## As a special exception, if other files instantiate templates or use macros
## or inline functions from this file, or you compile this file and link it
## with other works to produce a work based on this file, this file does not
## by itself cause the resulting work to be covered by the GNU General Public
## License. However the source code for this file must still be made available
## in accordance with section (3) of the GNU General Public License.
##
## This exception does not invalidate any other reasons why a work based on
## this file might be covered by the GNU General Public License.
##
## Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
## at http://sources.redhat.com/ecos/ecos-license/
## -------------------------------------------
#####ECOSGPLCOPYRIGHTEND####
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

#------------------------------------------------------------------------------

#define CYGPKG_HAL_RESET_VECTOR_FIRST_CODE
	.macro hal_reset_vector_first_code
	.endm


##-----------------------------------------------------------------------------
## ext bus interface registers
## flash part
    .equ FR30_MB91301_ASR0,     0x640
    .equ FR30_MB91301_ACR0,     0x642
    .equ FR30_MB91301_AWR0,     0x660
    .equ FR30_MB91301_CSER,     0x680

    .equ FR30_MB91301_PDR9,     0x9
    .equ FR30_MB91301_DDR9,     0x609
    .equ FR30_MB91301_PFR9,     0x619
    .equ FR30_MB91301_PCR9,     0x629

    .equ FR30_MB91301_PDR8,     0x8
    .equ FR30_MB91301_DDR8,     0x608
    .equ FR30_MB91301_PFR8,     0x618
    .equ FR30_MB91301_PCR8,     0x628

## sdram part
    .equ FR30_MB91301_ASR6,     0x658
    .equ FR30_MB91301_ACR6,     0x65a
    .equ FR30_MB91301_AWR6,     0x66c
    .equ FR30_MB91301_MCRA,     0x670
    .equ FR30_MB91301_MCRB,     0x671
    .equ FR30_MB91301_RCR,      0x684

##------------------------------------------------------------------------------
## ext Bus (memory controller) initialisation macros
##
## flash part


#define CYGPKG_HAL_FR30_FLASH_INIT_DEFINED
.macro hal_flash_init

    ldi:20  #FR30_MB91301_ASR0, r10 ; CS0 area starts at
    ldi:8   0x000,   r0             ; 0x00000000
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
#endif // ifndef CYGONCE_HAL_PLATFORM_INC
# end of platform.inc
