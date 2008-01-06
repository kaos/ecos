#ifndef CYGONCE_HAL_VARIANT_INC
#define CYGONCE_HAL_VARIANT_INC
##=============================================================================
##
##	variant.inc
##
##	MB91301 family assembler header file
##
##=============================================================================
#####ECOSGPLCOPYRIGHTBEGIN####
## -------------------------------------------
## This file is part of eCos, the Embedded Configurable Operating System.
## Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
## Author(s): 	larsi
## Contributors:larsi
## Date:	2006-07-24
## Purpose:	MB91301 family definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the TX39 CPU family.
## Usage:
##		#include <cyg/hal/variant.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/fr30.inc>

#include <cyg/hal/platform.inc>

##-----------------------------------------------------------------------------
## Define CPU variant for architecture HAL.

#define	CYG_HAL_FR30_MB91301

##-----------------------------------------------------------------------------
## Indicate that the ISR tables are defined in variant.S

#ifndef CYG_HAL_FR30_ISR_TABLES_DEFINED
#define CYG_HAL_FR30_ISR_TABLES_DEFINED
#endif

##-----------------------------------------------------------------------------
## CPU initialisation, we set the clock to PLL 48 Mhz (12 * 4Mhz) here.
## PLL lock waiting time is implemented as a busy loop.

#ifndef CYGPKG_HAL_FR30_CPU_INIT_DEFINED
#define CYGPKG_HAL_FR30_CPU_INIT_DEFINED
#
# busy wait loop
# pollutes r11, r12
#
.macro wait_loop no=0x1
    ldi:20  #\no,   r12
    ldi:8   #0x1,   r11
8:
    sub     r11,    r12
    bne     8b
.endm

##-----------------------------------------------------------------------------
## Clock Modulator control registers

    .equ FR30_MB91301_CMCR,     0x164
    .equ FR30_MB91301_CMCRH,    0x164
    .equ FR30_MB91301_CMCRL,    0x165
    .equ FR30_MB91301_CMPR,     0x166
    .equ FR30_MB91301_CMLS0,    0x168
    .equ FR30_MB91301_CMLS1,    0x16a
    .equ FR30_MB91301_CMLS2,    0x16c
    .equ FR30_MB91301_CMLT0,    0x170
    .equ FR30_MB91301_CMLT1,    0x172
    .equ FR30_MB91301_CMLT2,    0x174
    .equ FR30_MB91301_CMAC,     0x176
    .equ FR30_MB91301_CMACH,    0x178
    .equ FR30_MB91301_CMACL,    0x179
    .equ FR30_MB91301_CMTS,     0x17a
    .equ FR30_MB91301_CMTSH,    0x17a
    .equ FR30_MB91301_CMTSL,    0x17b
    .equ FR30_MB91301_ICR31,    0x45f

##-----------------------------------------------------------------------------
## Registers for Clock Generation and Reset

    .equ FR30_MB91301_RSRR,     0x480
    .equ FR30_MB91301_STCR,     0x481
    .equ FR30_MB91301_TBCR,     0x482
    .equ FR30_MB91301_CTBR,     0x483
    .equ FR30_MB91301_CLKR,     0x484
    .equ FR30_MB91301_WPR,      0x485
    .equ FR30_MB91301_DIVR0,    0x486
    .equ FR30_MB91301_DIVR1,    0x487

##-----------------------------------------------------------------------------
## ext bus interface registers

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

##-----------------------------------------------------------------------------
## registers for serial0 and U-timer settings
##
    .equ FR30_MB91301_PDRJ,     0x13
    .equ FR30_MB91301_DDRJ,     0x403
    .equ FR30_MB91301_PFRJ,     0x413
    .equ FR30_MB91301_UTIM0,    0x64
    .equ FR30_MB91301_UTIMR0,   0x64
    .equ FR30_MB91301_UTIMC0,   0x67
    .equ FR30_MB91301_DRCL,     0x66
    .equ FR30_MB91301_SMR0,     0x63
    .equ FR30_MB91301_SCR0,     0x62
    .equ FR30_MB91301_SIDR0,    0x61
    .equ FR30_MB91301_SODR0,    0x61
    .equ FR30_MB91301_SSR0,     0x60

##-----------------------------------------------------------------------------
## registers for clock settings
##
    .equ FR30_MB91301_RTC_TMRLR,      0x50
    .equ FR30_MB91301_RTC_TMR,        0x52
    .equ FR30_MB91301_RTC_TMCSR,      0x56



##------------------------------------------------------------------------------
## CPU initialisation macro
## This is mainly for setting clock speeds.
##------------------------------------------------------------------------------
.macro  hal_cpu_init

    ldi:8   #0x00,  r0

    ldi:20  #FR30_MB91301_CLKR, r10     ; PLLx4 and enable it, still use source
    ldi:8   (CYGHWR_HAL_FR30_MB91301_CLKR - 1) * 16 + 4,  r1 ;
    stb     r1,     @r10                ; oscillation as clock source

    ldi:20  #FR30_MB91301_TBCR, r11     ; set time base counter to
    ldi:8   #0x18,  r1                  ; about 60 ms and disable
    stb     r1,     @r11                ; its interrupt (we poll below)

    ldi:20  #FR30_MB91301_CTBR, r12     ; and
    ldi:8   #0xa5,  r2                  ; start
    ldi:8   #0x5a,  r3                  ; the
    stb     r2,     @r12                ; time base
    stb     r3,     @r12                ; counter

    ldi:20  #FR30_MB91301_STCR, r13     ; set oscillation stabilisation time
    ldi:8   #0x17,  r1                  ; to about 250 us
    stb     r1,     @r13                ;

    ldi:20  #FR30_MB91301_DIVR0, r12    ; set CLKB divider
    ldi:8   (CYGHWR_HAL_FR30_MB91301_CLKB_DIVIDER - 1) * 16 + (CYGHWR_HAL_FR30_MB91301_CLKP_DIVIDER - 1),  r1 ; and
    stb     r1,     @r12                ; CLKP divider

    ldi:20  #FR30_MB91301_DIVR1, r13    ; CLKT divider
    ldi:8   (CYGHWR_HAL_FR30_MB91301_CLKT_DIVIDER - 1) * 16,    r1
    stb     r1,     @r13                ;

    ldi:8   #0x80,  r2                  ; wait the rest
1:                                      ; of the
    ldub    @r11,   r3                  ; time base counter time
    and     r2,     r3                  ; (we set it to
    beq     1b                          ; 60 ms above)

    ldi:8   #0x36,  r1                  ; and now we are ready to
    stb     r1,     @r10                ; switch clock to PLL

.endm

#endif /* !CYGPKG_HAL_FR30_CPU_INIT_DEFINED */

##-----------------------------------------------------------------------------
## FR30 interrupt handling.
## nothing is here because the intc is initialized correctly by hardware reset
## if something is needed it should be implemented in arch.inc with define'd
## adresses to the registers. It should be the same for all FR30s


##------------------------------------------------------------------------------
## Diagnostics macros.
## Indicate that the diagnostic macros are defined in variant.S / hal_diag.c

#ifndef CYGPKG_HAL_FR30_DIAG_DEFINED

##-----------------------------------------------------------------------------
## registers for led settings
##

    .equ FR30_MB91301_PDRG,     0x10
    .equ FR30_MB91301_DDRG,     0x400
    .equ FR30_MB91301_PFRG,     0x410
## our MB91301A does not have PCRG (pull up resistor register G)
## but it is here anyway
    .equ FR30_MB91301_PCRG,     0x420


.macro hal_diag_init_led
    ldi:8   #0xff,      r4
    ldi:20  #FR30_MB91301_DDRG, r5
    stb     r4,         @r5
    ldi:8   #0x00,      r13
    ldi:20  #FR30_MB91301_PFRG, r5
    stb     r13,         @r5
    dmovb   r13,        @FR30_MB91301_PDRG
.endm

##
## switch on led on "hardcoded" value supplied after this macro
## pollutes r13
##
.macro hal_diag_led led=0x0
##st      r13, @-r15
    ldi:8   #\led , r13
    dmovb   r13,    @FR30_MB91301_PDRG
##ld      @r15+,  r13
.endm

##------------------------------------------------------------------------------
## UARTa macros
##

## output a value to UART a
## the value has to be in r4
## register r1 will be polluted
.macro uarta_putc
    ldi:8   #FR30_MB91301_SODR0,  r1
    stb     r4, @r1
.endm


## wait for the data in UART a SODR0 register to be drained
## registers r1, r2, r3 will be polluted
.macro uarta_drain
    ldi:8   #FR30_MB91301_SSR0,     r3  ;
    ldi:8   #0x08,  r1                  ; TDRE bit of SSR0
1:                                      ;
    ldub    @r3,    r2                  ;
    and     r1,     r2                  ;
    beq     1b
.endm

## receive a value from UART a
## value is returned in r4
## register r1, r2 and r3 will be polluted
.macro uarta_getc
    ldi:8   #FR30_MB91301_SSR0,     r3  ;
    ldi:8   #0x10,  r1                  ; RDRF bit of SSR0
2:
    ldub    @r3,    r2                  ;
    and     r1,     r2                  ;
    beq     2b                          ; wait until a byte is received

    ldi:8   #FR30_MB91301_SIDR0,    r3  ;
    ldub    @r3,    r4                  ; and get the value
.endm

#define CYGPKG_HAL_FR30_DIAG_DEFINED

#endif

#------------------------------------------------------------------------------
# Timer initialization.

#ifndef CYGPKG_HAL_FR30_TIMER_DEFINED

    .macro  hal_timer_init

    # load reload value into reload register
        ldi:8   #FR30_MB91301_RTC_TMRLR,  r4
        ldi:20  #CYGNUM_HAL_RTC_PERIOD, r5
        sth     r5,     @r4
    # set parameters to TODO
        ldi:8   #FR30_MB91301_RTC_TMCSR,  r4
        ldi:20  #0x0813,    r5
        sth     r5,     @r4
    .endm

#define CYGPKG_HAL_FR30_TIMER_DEFINED

#endif

#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_VARIANT_INC
# end of variant.inc
