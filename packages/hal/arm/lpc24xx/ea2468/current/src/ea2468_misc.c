/*==========================================================================
//
//      ea2468_misc.c
//
//      HAL misc board support code for EA LPC2468 OEM board
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2008 eCosCentric Limited
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Uwe Kindler 
// Contributors: Uwe Kindler
// Date:         2008-06-15
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/


//===========================================================================
//                               INCLUDES
//===========================================================================
#include <pkgconf/hal.h>
#include <pkgconf/hal_arm_lpc24xx_ea2468.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_io.h>             // IO macros

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_ass.h>          // assertion macros
#include <cyg/hal/var_io.h>
#include <cyg/hal/plf_io.h>
#include <pkgconf/hal.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>

#ifdef CYGPKG_REDBOOT
#include <redboot.h>
#endif


//===========================================================================
//                               DEFINES
//===========================================================================
#define SCB_BASE   CYGARC_HAL_LPC24XX_REG_SCB_BASE
#define EMC_BASE   CYGARC_HAL_LPC24XX_REG_EMC_BASE
#define PIN_BASE   CYGARC_HAL_LPC24XX_REG_PIN_BASE
#define IO_BASE    CYGARC_HAL_LPC24XX_REG_IO_BASE
#define FIO_BASE   CYGARC_HAL_LPC24XX_REG_FIO_BASE
#define SDRAM_BASE 0xA0000000
 
extern void cyg_hal_plf_serial_init(void);


//===========================================================================
// Initialize communication channels
//===========================================================================
void cyg_hal_plf_comms_init(void)
{
    static int initialized = 0;
	
    if (initialized)
        return;
    initialized = 1;

    cyg_hal_plf_serial_init();
}


//===========================================================================
// Finalize hardware initialisation of platform
//===========================================================================
void hal_plf_hardware_init(void)
{

}


//===========================================================================
// hal_gpio_init 
//===========================================================================
void hal_gpio_init(void)
{
    //
    // Enable UART0 pins
    //
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL0, 0x00000050);
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL1, 0); 
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL2, 0);
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL3, 0);
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL4, 0);
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL5, 0);
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL6, 0);
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL7, 0x30003fff);
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL8, 0);
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL9, 0);
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL10,0);

    HAL_WRITE_UINT32(IO_BASE + CYGARC_HAL_LPC24XX_REG_IO0DIR, 0);
    HAL_WRITE_UINT32(IO_BASE + CYGARC_HAL_LPC24XX_REG_IO1DIR, 0);
    HAL_WRITE_UINT32(IO_BASE + CYGARC_HAL_LPC24XX_REG_IO0SET, 0xffffffff);
    HAL_WRITE_UINT32(IO_BASE + CYGARC_HAL_LPC24XX_REG_IO1SET, 0xffffffff);

    HAL_WRITE_UINT32(FIO_BASE + CYGARC_HAL_LPC24XX_REG_FIO0DIR, 0);
    HAL_WRITE_UINT32(FIO_BASE + CYGARC_HAL_LPC24XX_REG_FIO1DIR, 0);
    HAL_WRITE_UINT32(FIO_BASE + CYGARC_HAL_LPC24XX_REG_FIO2DIR, 0);
    HAL_WRITE_UINT32(FIO_BASE + CYGARC_HAL_LPC24XX_REG_FIO3DIR, 0);
    HAL_WRITE_UINT32(FIO_BASE + CYGARC_HAL_LPC24XX_REG_FIO4DIR, 0);
    
    HAL_WRITE_UINT32(FIO_BASE + CYGARC_HAL_LPC24XX_REG_FIO0SET, 0xffffffff);
    HAL_WRITE_UINT32(FIO_BASE + CYGARC_HAL_LPC24XX_REG_FIO1SET, 0xffffffff);
    HAL_WRITE_UINT32(FIO_BASE + CYGARC_HAL_LPC24XX_REG_FIO2SET, 0xffffffff);
    HAL_WRITE_UINT32(FIO_BASE + CYGARC_HAL_LPC24XX_REG_FIO3SET, 0xffffffff);
    HAL_WRITE_UINT32(FIO_BASE + CYGARC_HAL_LPC24XX_REG_FIO4SET, 0xffffffff);
}


//===========================================================================
// hal_pll_init - initialize pll and all clocks
//===========================================================================
void hal_pll_init(void)
{
    cyg_uint32 regval;
    
    HAL_READ_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLSTAT, regval);
    if (regval & CYGARC_HAL_LPC24XX_REG_PLLSTAT_PLLC)
    {
    	//
    	// Enable PLL, disconnected
    	//
    	HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLCON,  
    	                  CYGARC_HAL_LPC24XX_REG_PLLCON_PLLE);
    	HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLFEED, 0xaa);
    	HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLFEED, 0x55); 
    }
    
    //
    // Disable PLL, disconnected
  	//
   	HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLCON,  0x00);
   	HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLFEED, 0xaa);
   	HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLFEED, 0x55); 
   	                 
    //
    // Enables main oscillator and wait until it is usable
    //
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_SCS, 
                     CYGARC_HAL_LPC24XX_REG_SCS_OSCEN);
    do
    {
        HAL_READ_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_SCS, regval);    
    } while (!(regval & CYGARC_HAL_LPC24XX_REG_SCS_OSCSTAT));
    
    //
    // select main OSC, 12MHz, as the PLL clock source 
    //
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_CLKSRCSEL, 
                     CYGARC_HAL_LPC24XX_REG_CLKSRCSEL_MAIN);
    
    //
    // Configure PLL multiplier and pre divider according to
    // configuration values
    //                 
    regval = ((CYGNUM_HAL_ARM_LPC24XX_PLL_MUL - 1) | 
              (CYGNUM_HAL_ARM_LPC24XX_PLL_DIV - 1) << 16);
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLCFG,  regval);
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLFEED, 0xaa);
   	HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLFEED, 0x55); 
   	                 
    //
    // Enable PLL, disconnected
    //
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLCON,  
                     CYGARC_HAL_LPC24XX_REG_PLLCON_PLLE);
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLFEED, 0xaa);
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLFEED, 0x55); 
                     
    //
    // Set CPU clock divider
    //
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_CCLKCFG, 
                     CYGNUM_HAL_ARM_LPC24XX_CPU_CLK_DIV - 1);
                     
    //
    // Set USB clock divider
    //
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_CCLKCFG, 
                     CYGNUM_HAL_ARM_LPC24XX_USB_CLK_DIV - 1);
                     
    //
    // Check lock bit status
    //
    do
    {
        HAL_READ_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLSTAT, regval);   
    } while(!(regval & CYGARC_HAL_LPC24XX_REG_PLLSTAT_PLOCK));
    
    //
    // Enable PLL and connect
    //
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLCON, 
                     CYGARC_HAL_LPC24XX_REG_PLLCON_PLLE | 
                     CYGARC_HAL_LPC24XX_REG_PLLCON_PLLC);
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLFEED, 0xaa);
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLFEED, 0x55);  
                     
    //
    // Check connect bit status
    //
    do
    {
        HAL_READ_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PLLSTAT, regval);   
    } while(!(regval & CYGARC_HAL_LPC24XX_REG_PLLSTAT_PLLC));  
    
    //
    // entry for JTAG debugger- enable this while loop as a stop for
    // the JTAG debugger - the JTAG debugger only works after the PLL is
    // initialized properly
    //
    /*while (1)
    {
    }*/
}


//===========================================================================
// hal_mem_init - initialize external memory interface
//===========================================================================
void hal_mem_init(void)
{
    volatile unsigned int i;
    volatile unsigned int dummy;
    volatile cyg_uint32   regval;
    
    //
    // Enable external memory interface
    //
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMC_CTRL, 
                     CYGARC_HAL_LPC24XX_REG_EMC_CTRL_EN);
    HAL_READ_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PCONP, regval);
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PCONP,
                     regval | CYGARC_HAL_LPC24XX_REG_PCONP_EMC);  
                    
    //
    // Setup pin functions
    //
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL4, 0x50000000);
#if defined(CYGHWR_HAL_ARM_LPC24XX_EA2468_DATA_BUS_WIDTH_32)
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL5, 0x55010115);
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL7, 0x55555555);
#else
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL5, 0x05050555);
#endif
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL6, 0x55555555);
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL8, 0x55555555);
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL9, 0x50555555);

#if defined(CYGHWR_HAL_ARM_LPC24XX_EA2468_DATA_BUS_WIDTH_32)  
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_RP,    1);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_RAS,   3);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_SREX,  5);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_APR,   1);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_DAL,   5);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_WR,    1);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_RC,    5);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_RFC,   5);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_XSR,   5);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_RRD,   1);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_MRD,   1);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_RDCFG, 1);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_RASCAS0, 
                     0x00000202);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_CONFIG0, 
                     0x00005480);
#else
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_RP,    2);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_RAS,   3);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_SREX,  7);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_APR,   2);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_DAL,   5);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_WR,    1);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_RC,    5);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_RFC,   5);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_XSR,   7);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_RRD,   1);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_MRD,   2);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_RDCFG, 1);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_RASCAS0, 
                     0x00000303);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_CONFIG0, 
                     0x00000680);
#endif
    
    //  
    // Wait 100 ms and then send command: NOP
    //
    HAL_DELAY_US(100000);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_CONTROL, 
                     0x00000183);

    //
    // wait 200 ms and then send command: PRECHARGE-ALL, shortest
    // possible refresh period
    //
    HAL_DELAY_US(200000);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_CONTROL, 
                     0x00000103);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_REFRESH, 
                     0x00000002);
    
    //
    // wait 128 ABH clock cycles
    //
    for(i = 0; i < 64; i++)
    {
        asm volatile(" nop");
    }  
    
    //  
    // Set correct refresh period and the send command MODE
    //
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_REFRESH, 28);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_CONTROL, 
                     0x00000083);

    //
    // Set mode register in SDRAM
    //
#if defined(CYGHWR_HAL_ARM_LPC24XX_EA2468_DATA_BUS_WIDTH_32)
    dummy = *((volatile unsigned int*)(SDRAM_BASE | (0x22 << 11)));
#else
    dummy = *((volatile unsigned int*)(SDRAM_BASE | (0x33 << 12)));
#endif
    
    //
    //Send command: NORMAL, enable buffer and wait for 1 second
    //
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_CONTROL, 
                     0x00000000);
    HAL_READ_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_CONFIG0, regval);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCD_CONFIG0, 
                     regval | 0x00080000);
    HAL_DELAY_US(1000);

    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCS_WAITW_EN0, 0x2);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCS_WAITO_EN0, 0x2);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCS_WAITRD0,   0x1f);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCS_WAITPAGE0, 0x1f);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCS_WAITWR0,   0x1f);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCS_WAITTURN0, 0xf);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCS_CONFIG0,   
                     0x00000081);
  
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCS_WAITW_EN1, 0x2);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCS_WAITO_EN1, 0x2);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCS_WAITRD1,   0x8);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCS_WAITPAGE1, 0x1f);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCS_WAITWR1,   0x8);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCS_WAITTURN1, 0xf);
    HAL_WRITE_UINT32(EMC_BASE + CYGARC_HAL_LPC24XX_REG_EMCS_CONFIG1,   
                     0x00000080);   
}


//===========================================================================
// hal_plf_startup
//===========================================================================
void hal_plf_startup(void)
{
    hal_pll_init();
    
    //
    // Set clock speed of all peripherals to reset value (CPU speed / 4)
    //
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PCLKSEL0, 0x00000000);
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PCLKSEL1, 0x00000000);
    
    //
    // Setup memory acceleration module
    //
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_MAMCR, 0);
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_MAMTIM, 4);
    
    hal_gpio_init();
    HAL_DELAY_US(20000);
    hal_mem_init();
}


//===========================================================================
// hal_lpc2xxx_set_leds
//===========================================================================
void hal_lpc24xx_set_leds (int mask)
{
    //
    // implement function for setting diagnostic leds
    //
}


#ifdef CYGPKG_DEVS_CAN_LPC2XXX
//===========================================================================
// Configure CAN pins
//===========================================================================
void hal_lpc_can_init(cyg_uint8 can_chan_no)
{
    CYG_ASSERT(can_chan_no < 2, "CAN channel number out of bounds");
    
    cyg_uint32 pinsel0_val;
    cyg_uint32 pconp_val;
    HAL_READ_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL0, pinsel0_val);
    HAL_READ_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PCONP, pconp_val);
    switch (can_chan_no)
    {
        case 0:
             CYGARC_HAL_LPC24XX_SET_PIN_FUN(pinsel0_val, 0, 1);
             CYGARC_HAL_LPC24XX_SET_PIN_FUN(pinsel0_val, 1, 1);
             pconp_val |= CYGARC_HAL_LPC24XX_REG_PCONP_CAN1;
             break;
        
        case 1:
             CYGARC_HAL_LPC24XX_SET_PIN_FUN(pinsel0_val, 4, 2);
             CYGARC_HAL_LPC24XX_SET_PIN_FUN(pinsel0_val, 5, 2); 
             pconp_val |= CYGARC_HAL_LPC24XX_REG_PCONP_CAN2;
             break;
    }
    HAL_WRITE_UINT32(PIN_BASE + CYGARC_HAL_LPC24XX_REG_PINSEL0, pinsel0_val);
    HAL_WRITE_UINT32(SCB_BASE + CYGARC_HAL_LPC24XX_REG_PCONP, pconp_val);
}
#endif // #ifdef CYGPKG_DEVS_CAN_LPC2XXX

//--------------------------------------------------------------------------
// EOF ea2468_misc.c
