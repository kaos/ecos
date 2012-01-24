//==========================================================================
//
//      lpc17xx_misc.c
//
//      Cortex-M LPC17XX HAL functions
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2010 Free Software Foundation, Inc.                        
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg, ilijak
// Date:         2010-12-12
// Description:  
//
//####DESCRIPTIONEND####
//
//========================================================================


#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm.h>
#include <pkgconf/hal_cortexm_lpc17xx.h>
#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
#endif

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros

#include <cyg/hal/hal_arch.h>          // HAL header
#include <cyg/hal/hal_intr.h>          // HAL header
#include <cyg/hal/hal_if.h>            // HAL header

#include <cyg/hal/lpc17xx_misc.h>


//===========================================================================
//                               DEFINES
//===========================================================================

cyg_uint32      hal_lpc17xx_sysclk;
cyg_uint32      hal_cortexm_systick_clock;

cyg_uint32      hal_get_lpc_cpu_clock(void);

void            hal_start_clocks(void);

void            hal_lpc_start_main_clock(void);
void            hal_lpc_start_usb_clock(void);
#if defined(CYGHWR_HAL_CORTEXM_LPC17XX_CLKOUT)
void            hal_lpc_clock_out(void);
#endif
//==========================================================================

#ifdef CYG_HAL_STARTUP_ROM

//===========================================================================
// LPC17xx Code Read Protection field field
//===========================================================================

// Note: LPC17xx Code Read Protection field field must be present in
//       LPC17xx flash image and ocupy a word at 0x000002FC

// For ".lpc17xx_crp" section definition see MLT files.

const cyg_uint32 LPC17XX_CRP __attribute__((section(".lpc17xxcrp"), used)) = 0xFFFFFFFF;

#endif // CYG_HAL_STARTUP_ROM

const cyg_uint32* hal_lpc17xx_crp_p(void)
{
    return &LPC17XX_CRP;
}

void
hal_variant_init(void)
{
#if 1                                  /* !defined(CYG_HAL_STARTUP_RAM) */
    hal_start_clocks();
#endif

#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    hal_if_init();
#endif
}

//==========================================================================
// Setup up system clocks
//
// Set up clocks from configuration. In the future this should be extended so
// that clock rates can be changed at runtime.


void CYGOPT_HAL_LPC17XX_MISC_FLASH_SECTION_ATTR
hal_start_clocks(void)
{
    // Main clock - for everything except USB
    hal_lpc_start_main_clock();

    // USB clock
    hal_lpc_start_usb_clock();

#if defined CYGHWR_HAL_CORTEXM_LPC17XX_CLKOUT
    hal_lpc_clock_out();
#endif
}


void CYGOPT_HAL_LPC17XX_MISC_FLASH_SECTION_ATTR
hal_lpc_start_main_clock(void)
{
    CYG_ADDRESS     scb_base_p;
    cyg_uint32      regval;

    scb_base_p = CYGHWR_HAL_LPC17XX_REG_SCB_BASE;

    HAL_READ_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0STAT, regval);
    if (regval & CYGHWR_HAL_LPC17XX_REG_PLL0STAT_PLLC) {

        // Enable PLL, disconnected
        HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0CON,
                         CYGHWR_HAL_LPC17XX_REG_PLLCON_PLLE);
        HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0FEED, 0xaa);
        HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0FEED, 0x55);
    }
    // Disable PLL, disconnected
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0CON, 0x00);
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0FEED, 0xaa);
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0FEED, 0x55);

    // Enables main oscillator and wait until it is usable
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_SCS,
                     CYGHWR_HAL_LPC17XX_REG_SCS_OSCEN);
    do {
        HAL_READ_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_SCS, regval);
    } while (!(regval & CYGHWR_HAL_LPC17XX_REG_SCS_OSCSTAT));


    // Select main OSC, 12MHz, as the PLL clock source
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_CLKSRCSEL,
                     CYGHWR_HAL_LPC17XX_REG_CLKSRCSEL_MAIN);

    // Configure PLL multiplier and pre divider according to configuration
    // values
    regval = ((CYGHWR_HAL_CORTEXM_LPC17XX_PLL0_MUL - 1) |
              (CYGHWR_HAL_CORTEXM_LPC17XX_PLL0_DIV - 1) << 16);
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0CFG, regval);
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0FEED, 0xaa);
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0FEED, 0x55);

    // Enable PLL, disconnected
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0CON,
                     CYGHWR_HAL_LPC17XX_REG_PLLCON_PLLE);
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0FEED, 0xaa);
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0FEED, 0x55);

    // Set CPU clock divider
    regval = CYGHWR_HAL_CORTEXM_LPC17XX_CPU_CLK_DIV - 1;
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_CCLKCFG, regval);

    // Check lock bit status
    do {
        HAL_READ_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0STAT, regval);
    } while (!(regval & CYGHWR_HAL_LPC17XX_REG_PLL0STAT_PLOCK));

    // Connect CPU clock
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0CON,
                     CYGHWR_HAL_LPC17XX_REG_PLLCON_PLLE |
                     CYGHWR_HAL_LPC17XX_REG_PLLCON_PLLC);
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0FEED, 0xaa);
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0FEED, 0x55);

    // Check connect bit status
    do {
        HAL_READ_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL0STAT, regval);
    } while (!(regval & CYGHWR_HAL_LPC17XX_REG_PLL0STAT_PLLC));

    // Calculate system clock from configuration
    hal_lpc17xx_sysclk = CYGHWR_HAL_CORTEXM_LPC17XX_CLOCK_SPEED;
    hal_cortexm_systick_clock = hal_lpc17xx_sysclk;
}

void CYGOPT_HAL_LPC17XX_MISC_FLASH_SECTION_ATTR
hal_lpc_start_usb_clock(void)
{
    CYG_ADDRESS     scb_base_p;
    cyg_uint32      regval;

    scb_base_p = CYGHWR_HAL_LPC17XX_REG_SCB_BASE;

    // Configure PLL multiplier and pre divider according to
    // configuration values
    regval = ((CYGHWR_HAL_CORTEXM_LPC17XX_PLL1_MUL - 1) |
              (CYGHWR_HAL_CORTEXM_LPC17XX_PLL1_DIV - 1) << 5);
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL1CFG, regval);
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL1FEED, 0xaa);
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL1FEED, 0x55);

    // Enable PLL, disconnected
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL1CON,
                     CYGHWR_HAL_LPC17XX_REG_PLLCON_PLLE);
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL1FEED, 0xaa);
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL1FEED, 0x55);

    do {
        HAL_READ_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL1STAT, regval);
    } while (!(regval & CYGHWR_HAL_LPC17XX_REG_PLL1STAT_PLOCK));

    // Connect USB clock
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL1CON,
                     CYGHWR_HAL_LPC17XX_REG_PLLCON_PLLE |
                     CYGHWR_HAL_LPC17XX_REG_PLLCON_PLLC);
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL1FEED, 0xaa);
    HAL_WRITE_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL1FEED, 0x55);

    // Check connect bit status
    do {
        HAL_READ_UINT32(scb_base_p + CYGHWR_HAL_LPC17XX_REG_PLL1STAT, regval);
    } while (!(regval & CYGHWR_HAL_LPC17XX_REG_PLL1STAT_PLLC));
}

#ifdef CYGHWR_HAL_CORTEXM_LPC17XX_CLKOUT
void CYGOPT_HAL_LPC17XX_MISC_FLASH_SECTION_ATTR
hal_lpc_clock_out(void)
{
    cyg_uint32      regval;

    CYGHWR_HAL_LPC17XX_PIN_GET(CYGHWR_HAL_LPC17XX_REG_PINSEL3, regval);
    regval &= ~0x00c00000;
    regval |= 0x00400000;
    CYGHWR_HAL_LPC17XX_PIN_SET(CYGHWR_HAL_LPC17XX_REG_PINSEL3, regval);

    HAL_WRITE_UINT32(CYGHWR_HAL_LPC17XX_REG_SCB_BASE +
                     CYGHWR_HAL_LPC17XX_REG_CLKOUTCFG,
                     CYGHWR_HAL_CORTEXM_LPC17XX_CLKOUT_SET);
}
#endif // CYGHWR_HAL_CORTEXM_LPC17XX_CLKOUT


cyg_uint32
hal_lpc_get_cpu_clock(void)
{
    cyg_uint32      regval,
                    pll0stat_div,
                    pll0stat_mul,
                    cclkcfg;

    HAL_READ_UINT32(CYGHWR_HAL_LPC17XX_REG_SCB_BASE +
                    CYGHWR_HAL_LPC17XX_REG_PLL0STAT, pll0stat_div);
    pll0stat_mul = ((pll0stat_div >> 16) & 0xff) + 1;
    pll0stat_div = 2 * ((pll0stat_div & 0x7fff) + 1);
    HAL_READ_UINT32(CYGHWR_HAL_LPC17XX_REG_SCB_BASE +
                    CYGHWR_HAL_LPC17XX_REG_CCLKCFG, cclkcfg);
    cclkcfg = (cclkcfg & 0xff) + 1;

    regval =
        CYGHWR_HAL_CORTEXM_LPC17XX_XTAL_FREQ * pll0stat_div / pll0stat_mul /
        cclkcfg;

    return regval;
}


//===========================================================================
// Get peripheral clock for a certain peripheral
//===========================================================================
cyg_uint32
hal_lpc_get_pclk(cyg_uint32 pclk_id)
{
    static const cyg_uint8 divider_tbl[4] = {
        4, 1, 2, 8
    };
    cyg_uint32      pclkselreg;
    cyg_uint32      regval;
    cyg_uint8       divider;

    CYG_ASSERT(pclk_id >= CYNUM_HAL_LPC17XX_PCLK_WDT &&
               pclk_id <= CYNUM_HAL_LPC17XX_PCLK_SYSCON,
               "Invalid peripheral clock ID");

    // Decide if we need PCLKSEL0 or PCLKSEL1
    pclkselreg = ((pclk_id <= CYNUM_HAL_LPC17XX_PCLK_ACF) ?
                  CYGHWR_HAL_LPC17XX_REG_PCLKSEL0 :
                  CYGHWR_HAL_LPC17XX_REG_PCLKSEL1);
    HAL_READ_UINT32(CYGHWR_HAL_LPC17XX_REG_SCB_BASE + pclkselreg, regval);
    regval = (regval >> ((pclk_id & 0xF) << 1)) & 0x03;
    divider = divider_tbl[regval];
    if ((8 == divider) && (pclk_id >= CYNUM_HAL_LPC17XX_PCLK_CAN1)
        && (pclk_id <= CYNUM_HAL_LPC17XX_PCLK_ACF)) {
        divider = 6;
    }
    return CYGHWR_HAL_CORTEXM_LPC17XX_CLOCK_SPEED / divider;
}


//===========================================================================
// Set peripheral clock
//===========================================================================
void
hal_lpc_set_pclk(cyg_uint32 peripheral_id, cyg_uint8 divider)
{
    static const cyg_uint8 clock_tbl[5] = {
        0x01,                          // divider 1
        0x02,                          // divider 2
        0x00,                          // divider 4
        0x03,                          // divider 6
        0x03                           // divider 8
    };
    cyg_uint32      clock;
    cyg_uint32      pclkselreg;
    cyg_uint32      regval;

    CYG_ASSERT(peripheral_id >= CYNUM_HAL_LPC17XX_PCLK_WDT &&
               peripheral_id <= CYNUM_HAL_LPC17XX_PCLK_SYSCON,
               "Invalid peripheral clock ID");
    CYG_ASSERT(divider <= 8, "Wrong peripheral clock divider value");

    // Decide if we need PCLKSEL0 or PCLKSEL1
    pclkselreg = (peripheral_id <= CYNUM_HAL_LPC17XX_PCLK_ACF) ?
        CYGHWR_HAL_LPC17XX_REG_PCLKSEL0 : CYGHWR_HAL_LPC17XX_REG_PCLKSEL1;
    HAL_READ_UINT32(CYGHWR_HAL_LPC17XX_REG_SCB_BASE + pclkselreg, regval);
    clock = clock_tbl[divider >> 1];
    regval &= ~(0x03 << ((peripheral_id & 0xF) << 1));
    regval |= (clock << ((peripheral_id & 0xF) << 1));
    HAL_WRITE_UINT32(CYGHWR_HAL_LPC17XX_REG_SCB_BASE + pclkselreg, regval);
}


//===========================================================================
// Set pin function
//===========================================================================
void
hal_lpc_set_pin_function(cyg_uint8 port, cyg_uint8 pin, cyg_uint8 function)
{
    cyg_uint32      regval;
    cyg_uint8       pinsel_reg = port << 1;

    CYG_ASSERT(port <= 4, "Port value out of bounds");
    CYG_ASSERT(pin <= 31, "Pin value out of bounds");
    CYG_ASSERT(function <= 3, "Invalid function value");

    pinsel_reg += (pin > 15) ? 1 : 0;
    pinsel_reg <<= 2;
    HAL_READ_UINT32(CYGHWR_HAL_LPC17XX_REG_PIN_BASE + pinsel_reg, regval);
    regval &= ~(0x03 << ((pin & 0xF) << 1));
    regval |= (function << ((pin & 0xF) << 1));
    HAL_WRITE_UINT32(CYGHWR_HAL_LPC17XX_REG_PIN_BASE + pinsel_reg, regval);
}

//===========================================================================
// Enable/Disable power for certain peripheral
//===========================================================================
void
hal_lpc_set_power(cyg_uint8 pconp_id, int on)
{
    cyg_uint32      regval;

    CYG_ASSERT(pconp_id >= CYNUM_HAL_LPC17XX_PCONP_TIMER0 &&
               pconp_id <= CYNUM_HAL_LPC17XX_PCONP_USB,
               "Invalid peripheral power ID");
    HAL_READ_UINT32(CYGHWR_HAL_LPC17XX_REG_SCB_BASE +
                    CYGHWR_HAL_LPC17XX_REG_PCONP, regval);
    if (on) {
        regval |= (0x01 << pconp_id);
    } else {
        regval &= ~(0x01 << pconp_id);
    }

    HAL_WRITE_UINT32(CYGHWR_HAL_LPC17XX_REG_SCB_BASE +
                     CYGHWR_HAL_LPC17XX_REG_PCONP, regval);
}


//==========================================================================
// UART baud rate
//
// Set the baud rate divider of a UART based on the requested rate and
// the current APB clock settings.

void
hal_plf_uart_setbaud(cyg_uint32 base, cyg_uint32 baud)
{
    cyg_uint32      periph_id = CYNUM_HAL_LPC17XX_PCLK_UART0;
    cyg_uint16      divider;

    if (CYGHWR_HAL_LPC17XX_REG_UART1_BASE == base)
        periph_id = CYNUM_HAL_LPC17XX_PCLK_UART1;
    divider = CYG_HAL_CORTEXM_LPC17XX_BAUD_GENERATOR(periph_id, baud);
    // Set baudrate
    HAL_WRITE_UINT32(base + CYGHWR_HAL_LPC17XX_REG_UxLCR,
                     CYGHWR_HAL_LPC17XX_REG_UxLCR_DLAB);
    HAL_WRITE_UINT32(base + CYGHWR_HAL_LPC17XX_REG_UxDLM, divider >> 8);
    HAL_WRITE_UINT32(base + CYGHWR_HAL_LPC17XX_REG_UxDLL, divider & 0xFF);
}

//==========================================================================
// EOF lpc17xx_misc.c
