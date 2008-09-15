##=============================================================================
##
##      pati.S
##
##      PATI board hardware setup
##
##=============================================================================
#####ECOSGPLCOPYRIGHTBEGIN####
## -------------------------------------------
## This file is part of eCos, the Embedded Configurable Operating System.
## Copyright (C) 2008 Free Software Foundation, Inc.
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
## -------------------------------------------
#####ECOSGPLCOPYRIGHTEND####
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):   Steven Clugston
## Original:    Bob Koninckx
## Date:        2008-05-08
## Purpose:     pati board hardware setup
## Description: This file contains any code needed to initialize the
##              hardware on a PATI MPC555 board.
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>
        
#include <cyg/hal/ppc_regs.h>
#include <cyg/hal/arch.inc>
#include <cyg/hal/plf_misc.h>

#define PATI_CS0_FLASH_BASE_ADDRESS         (0xFFC00000)
#define PATI_CS1_SDRAM_BASE_ADDRESS         (0x00400000)
#define PATI_CS2_PCI_MASTER_BASE_ADDRESS    (0x01800000)
#define PATI_CS3_EPLD_CONFIG_BASE_ADDRESS   (0x02800000)

#define PATI_REG_PLD_BOARD_TIMING           (0x04)

#------------------------------------------------------------------------------
                
        .globl  hal_hardware_init
hal_hardware_init:
#if defined(CYGPKG_HAL_POWERPC_PATI) && defined(CYGPKG_HAL_POWERPC_MPC5xx)
        lwi     r3, CYGARC_REG_IMM_BASE
      
        // Disable the Watchdog (for now)
        lwi     r4, 0xffffff88
        stw     r4, (CYGARC_REG_IMM_SYPCR-CYGARC_REG_IMM_BASE)(r3)
        // Configure SIUMCR for LEDs
        lwi     r4, 0x80020C00
        stw     r4, (CYGARC_REG_IMM_SIUMCR-CYGARC_REG_IMM_BASE)(r3)

        // Unlock locked registers
        lwi     r4, 0x55ccaa33
        stw     r4, (CYGARC_REG_IMM_TBSCRK-CYGARC_REG_IMM_BASE)(r3)
        stw     r4, (CYGARC_REG_IMM_TBREF0K-CYGARC_REG_IMM_BASE)(r3)
        stw     r4, (CYGARC_REG_IMM_TBREF1K-CYGARC_REG_IMM_BASE)(r3)
        stw     r4, (CYGARC_REG_IMM_TBK-CYGARC_REG_IMM_BASE)(r3)
        stw     r4, (CYGARC_REG_IMM_RTCSCK-CYGARC_REG_IMM_BASE)(r3)
        stw     r4, (CYGARC_REG_IMM_RTCK-CYGARC_REG_IMM_BASE)(r3)
        stw     r4, (CYGARC_REG_IMM_RTSECK-CYGARC_REG_IMM_BASE)(r3)
        stw     r4, (CYGARC_REG_IMM_RTCALK-CYGARC_REG_IMM_BASE)(r3)
        stw     r4, (CYGARC_REG_IMM_PISCRK-CYGARC_REG_IMM_BASE)(r3)
        stw     r4, (CYGARC_REG_IMM_PITCK-CYGARC_REG_IMM_BASE)(r3)
        stw     r4, (CYGARC_REG_IMM_SCCRK-CYGARC_REG_IMM_BASE)(r3)
        stw     r4, (CYGARC_REG_IMM_PLPRCRK-CYGARC_REG_IMM_BASE)(r3)
        stw     r4, (CYGARC_REG_IMM_RSRK-CYGARC_REG_IMM_BASE)(r3)

        // Boost the clock to 40MHz
        lwi     r4, 0x03000000
        stw     r4, (CYGARC_REG_IMM_SCCR-CYGARC_REG_IMM_BASE)(r3)
        lwi     r4, 0x00910000
        stw     r4, (CYGARC_REG_IMM_PLPRCR-CYGARC_REG_IMM_BASE)(r3)
        lwi     r4, 0x0080
        sth     r4, (CYGARC_REG_IMM_COLIR-CYGARC_REG_IMM_BASE)(r3)

        // Set up the memory map
        // Do NOT write protect the flash memory, flash drivers won't work 
        // if we do
        
        // CS0, External flash, 4MByte, 16-bit port, Burst inhibit
        lwi     r4, (PATI_CS0_FLASH_BASE_ADDRESS | 0x00000807)
        stw     r4, (CYGARC_REG_IMM_BR0-CYGARC_REG_IMM_BASE)(r3)
        // 2 wait state, asynchronous operation 40 Mhz clock, 70 ns
        lwi     r4, 0xFFC00000
        stw     r4, (CYGARC_REG_IMM_OR0-CYGARC_REG_IMM_BASE)(r3)

        // CS1, SDRAM, 32-bit port, 16MByte, Burst inhibit
        lwi     r4, (PATI_CS1_SDRAM_BASE_ADDRESS | 0x00000007)
        stw     r4, (CYGARC_REG_IMM_BR1-CYGARC_REG_IMM_BASE)(r3)
        // 1 wait state, asynchronous operation
        lwi     r4, 0xFF000000
        stw     r4, (CYGARC_REG_IMM_OR1-CYGARC_REG_IMM_BASE)(r3)

        // CS2, PCI Master area, 32-bit port, 16Mbyte, Burst inhibit
        lwi     r4, (PATI_CS2_PCI_MASTER_BASE_ADDRESS | 0x00000007)
        stw     r4, (CYGARC_REG_IMM_BR2-CYGARC_REG_IMM_BASE)(r3)
        // 1 wait state, cycle length, asynchronous operation
        lwi     r4, 0xFF000000
        stw     r4, (CYGARC_REG_IMM_OR2-CYGARC_REG_IMM_BASE)(r3)

        // CS3, PCI Bridge Configuration Area / EPLD Config Address, 
        // 32-bit port, 4KByte / 32 Kbyte, Burst Inhibit
        lwi     r4, (PATI_CS3_EPLD_CONFIG_BASE_ADDRESS | 0x00000007)
        stw     r4, (CYGARC_REG_IMM_BR3-CYGARC_REG_IMM_BASE)(r3)
        // 1 wait state, cycle length, asynchronous operation
        lwi     r4, 0xFFFF0000 
        stw     r4, (CYGARC_REG_IMM_OR3-CYGARC_REG_IMM_BASE)(r3)

#if defined(CYGSEM_HAL_POWERPC_MPC5XX_IFLASH_DUAL_MAP)
        lwi     r4, 1
#else
        lwi     r4, 0
#endif
        stw     r4, (CYGARC_REG_IMM_DMBR-CYGARC_REG_IMM_BASE)(r3)
        lwi     r4, 0
        stw     r4, (CYGARC_REG_IMM_DMOR-CYGARC_REG_IMM_BASE)(r3)

        // Enable the time base and set the freeze flag
        lwi     r4, 0xc3
        sth     r4, (CYGARC_REG_IMM_TBSCR-CYGARC_REG_IMM_BASE)(r3)

        // RTC is clocked by 4MHz crystal, set the freeze flag
        lwi     r4, 0xd2
        sth     r4, (CYGARC_REG_IMM_RTCSC-CYGARC_REG_IMM_BASE)(r3)

        // Set the freeze flag for the Periodic interrupt timer
        lwi     r4, 0x82
        sth     r4, (CYGARC_REG_IMM_PISCR-CYGARC_REG_IMM_BASE)(r3)

        // USIU rest.
        // Set outputs in data direction register
        lwi     r4, 0x27000000
        lwi     r5, (CYGARC_REG_IMM_SGPIOCR-CYGARC_REG_IMM_BASE)
        stwx    r4, r3, r5
        lwi     r4, 0x00000000
        lwi     r5, (CYGARC_REG_IMM_SGPIODT1-CYGARC_REG_IMM_BASE)
        stwx    r4, r3, r5
        // Set GPIOs, set IRQ and USERi to 1, LEDs off 
        lwi     r4, 0x28000000
        lwi     r5, (CYGARC_REG_IMM_SGPIODT2-CYGARC_REG_IMM_BASE)
        stwx    r4, r3, r5
        lwi     r4, 0x00000ac6
        lwi     r5, (CYGARC_REG_IMM_EMCR-CYGARC_REG_IMM_BASE)
        stwx    r4, r3, r5

        // Dual ported TPU RAM
        lwi     r4, 0x0000
        lwi     r5, (CYGARC_REG_IMM_DPTMCR-CYGARC_REG_IMM_BASE)
        sthx    r4, r3, r5

        lwi     r4, 0xffa0
        lwi     r5, (CYGARC_REG_IMM_RAMBAR-CYGARC_REG_IMM_BASE)
        sthx    r4, r3, r5

        lwi     r4, 0x00
        lwi     r5, (CYGARC_REG_IMM_PORTQS-CYGARC_REG_IMM_BASE)
        sthx    r4, r3, r5

        lwi     r4, 0x00
        lwi     r5, (CYGARC_REG_IMM_PQSPAR_DDRQST-CYGARC_REG_IMM_BASE)
        sthx    r4, r3, r5
        lwi     r5, (CYGARC_REG_IMM_MPIOSMDR-CYGARC_REG_IMM_BASE)
        sthx    r4, r3, r5
        lwi     r5, (CYGARC_REG_IMM_MPIOSMDDR-CYGARC_REG_IMM_BASE)
        sthx    r4, r3, r5
        lwi     r5, (CYGARC_REG_IMM_SRAMMCR_A-CYGARC_REG_IMM_BASE)
        sthx    r4, r3, r5

        // Enable 32 interrupt priorities on the IMB3 unit
        lwi     r4, 0x60000000
        lwi     r5, (CYGARC_REG_IMM_UMCR-CYGARC_REG_IMM_BASE)
        stwx    r4, r3, r5
// section BSS may not fit into iram so the SDRAM has to be setup here 
// so it can be safely used instead by early startup code
#ifdef CYG_HAL_STARTUP_ROM
        // Setup SDRAM
        // Load SDRAM Controller register contents
        // Mask SDRAM_CAL, SDRAM_RCD, SDRAM_WREQ, SDRAM_PR, SDRAM_RC, 
        // SDRAM_LMR, and SDRAM_IIP bits
	lwi     r3,CYGPLF_REG_PLD_REGISTER_BASE
        lwz     r6,(PATI_REG_PLD_BOARD_TIMING)(r3)
        lwi     r4,0xff81efff
        and     r4,r4,r6
        // Set CAL bit
        lwi     r6,0x00400000
        or      r4,r4,r6
        stw     r4, (PATI_REG_PLD_BOARD_TIMING)(r3)
// Set IIP Precharge bit
        lwi     r6,0x00001000
        or      r4,r4,r6
        stw     r4, (PATI_REG_PLD_BOARD_TIMING)(r3)
// wait for IIP bit to clear
        li      r5, 0x7fff
precharge_loop:
        eieio
        lwz     r6,(PATI_REG_PLD_BOARD_TIMING)(r3)
        lwi     r4,0x00001000
        or      r6,r6,r4
        cmpwi   r6, 0
        beq     wait_refresh
        subi    r5, r5, 1
        cmpwi   r5, 0
        bgt     precharge_loop
wait_refresh:
// wait for about 200us
        li      r5, 0x0a8c
wait_refresh_loop:
        subi    r5, r5, 1
        cmpwi   r5, 0
        bgt     wait_refresh_loop
// Set LMR load mode register bit
        lwi     r4,0x00020000
        or      r6,r6,r4
        stw     r6,(PATI_REG_PLD_BOARD_TIMING)(r3)        
        eieio
// Calc LMR address for CAS = 3 so LMR = 0x00000032 << 2, 0x000000c8
        lwi     r3,PATI_CS1_SDRAM_BASE_ADDRESS
        addi    r3,r3,0x00c8
// Do a dummy read from Mode Register address, to load the value into SDRAM
        lwz     r6,0(r3)
// The SDRAM should now be accessible
#endif
	
#endif                
        sync
        blr
#------------------------------------------------------------------------------
# end of pati.S
