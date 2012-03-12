/*==========================================================================
//
//      a2f_misc.c
//
//      Cortex-M Actel A2F HAL functions
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2011 Free Software Foundation, Inc.
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
// Author(s):    ccoutand
// Contributors:
// Date:         2011-02-03
// Description:
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm.h>
#include <pkgconf/hal_cortexm_a2fxxx.h>

#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_intr.h>           // HAL header
#include <cyg/hal/hal_if.h>             // HAL header


//==========================================================================
// Clock Initialization values

#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLOCK

#define CLKx_SRC_CLKA    0
#define CLKx_SRC_CLKB    1
#define CLKx_SRC_CLKC    2
#define CLKx_SRC_CLKGA   3
#define CLKx_SRC_CLKGB   4
#define CLKx_SRC_CLKGC   5
#define CLKx_SRC_CLKGINT 6

#define CLKx_SRC_UIN     0
#define CLKx_SRC_DIP     1
#define CLKx_SRC_UIP     4
#define CLKx_SRC_GLxINT  5
#define CLKx_SRC_RCOSC   2
#define CLKx_SRC_MOSC    6
#define CLKx_SRC_32KOSC  6

// Select source of CLKA

#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKA_SRC_AUIN
# define CLKA_SRC_SEL CLKx_SRC_UIN
#elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKA_SRC_AUIP)
# define CLKA_SRC_SEL CLKx_SRC_UIP
#elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKA_SRC_ADIP)
# define CLKA_SRC_SEL CLKx_SRC_DIP
#elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKA_SRC_GLAINT)
# define CLKA_SRC_SEL CLKx_SRC_GLxINT
#elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKA_SRC_RCOSC)
# define CLKA_SRC_SEL CLKx_SRC_RCOSC
#else
# define CLKA_SRC_SEL CLKx_SRC_MOSC
#endif

// Select source of CLKB

#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKB
# ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKB_SRC_BUIN
#  define CLKB_SRC_SEL CLKx_SRC_UIN
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKB_SRC_BUIP)
#  define CLKB_SRC_SEL CLKx_SRC_UIP
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKB_SRC_BDIP)
#  define CLKB_SRC_SEL CLKx_SRC_DIP
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKB_SRC_GLBINT)
#  define CLKB_SRC_SEL CLKx_SRC_GLxINT
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKB_SRC_RCOSC)
#  define CLKB_SRC_SEL CLKx_SRC_RCOSC
# else
#  define CLKB_SRC_SEL CLKx_SRC_MOSC
# endif
#endif

// Select source of CLKC

#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKC
# ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKC_SRC_CUIN
#  define CLKC_SRC_SEL CLKx_SRC_UIN
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKC_SRC_CUIP)
#  define CLKC_SRC_SEL CLKx_SRC_UIP
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKC_SRC_CDIP)
#  define CLKC_SRC_SEL CLKx_SRC_DIP
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKC_SRC_GLCINT)
#  define CLKC_SRC_SEL CLKx_SRC_GLxINT
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKC_SRC_RCOSC)
#  define CLKC_SRC_SEL CLKx_SRC_RCOSC
# else
#  define CLKC_SRC_SEL CLKx_SRC_32KOSC
# endif
#endif

// Select source of CLKGA

#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKGA
# ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKGA_SRC_CLKA
#  define CLKGA_SRC_SEL 1
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGA_SRC_PLL_VCO0)
#  define CLKGA_SRC_SEL 4
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGA_SRC_PLL_VCO90)
#  define CLKGA_SRC_SEL 5
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGA_SRC_PLL_VCO180)
#  define CLKGA_SRC_SEL 6
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGA_SRC_PLL_VCO270)
#  define CLKGA_SRC_SEL 7
# else
#  define CLKGA_SRC_SEL 2
# endif
#endif

// Select source of CLKGB

#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKGB
# ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKGB_SRC_CLKB
#  define CLKGB_SRC_SEL 1
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGB_SRC_CLKGA
#  define CLKGB_SRC_SEL 3
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGB_SRC_PLL_VCO0)
#  define CLKGB_SRC_SEL 4
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGB_SRC_PLL_VCO90)
#  define CLKGB_SRC_SEL 5
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGB_SRC_PLL_VCO180)
#  define CLKGB_SRC_SEL 6
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGB_SRC_PLL_VCO270)
#  define CLKGB_SRC_SEL 7
# else
#  define CLKGB_SRC_SEL 2
# endif
#endif

// Select source of CLKGC

#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKGC
# ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKGC_SRC_CLKC
#  define CLKGC_SRC_SEL 1
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGC_SRC_CLKGA)
#  define CLKGC_SRC_SEL 3
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGC_SRC_PLL_VCO0)
#  define CLKGC_SRC_SEL 4
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGC_SRC_PLL_VCO90)
#  define CLKGC_SRC_SEL 5
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGC_SRC_PLL_VCO180)
#  define CLKGC_SRC_SEL 6
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGC_SRC_PLL_VCO270)
#  define CLKGC_SRC_SEL 7
# else
#  define CLKGC_SRC_SEL 2
# endif
#endif

#if CYGHWR_HAL_CORTEXM_A2FXXX_PCLK0_DIV   == 1
# define PCLK0DIV 0
#elif CYGHWR_HAL_CORTEXM_A2FXXX_PCLK0_DIV == 2
# define PCLK0DIV 1
#else
# define PCLK0DIV 2
#endif

#if CYGHWR_HAL_CORTEXM_A2FXXX_PCLK1_DIV   == 1
# define PCLK1DIV 0
#elif CYGHWR_HAL_CORTEXM_A2FXXX_PCLK1_DIV == 2
# define PCLK1DIV 1
#else
# define PCLK1DIV 2
#endif

#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_ACE_CLK
# if CYGHWR_HAL_CORTEXM_A2FXXX_ACLK_DIV   == 1
#  define ACLKDIV 0
# elif CYGHWR_HAL_CORTEXM_A2FXXX_ACLK_DIV == 2
#  define ACLKDIV 1
# else
#  define ACLKDIV 2
# endif
#endif

#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_PLL
# if CYGHWR_HAL_CORTEXM_A2FXXX_PLL_FREQ   < 43750000
#  define PLL_RANGE 0
# elif CYGHWR_HAL_CORTEXM_A2FXXX_PLL_FREQ < 87500000
#  define PLL_RANGE 1
# elif CYGHWR_HAL_CORTEXM_A2FXXX_PLL_FREQ < 175000000
#  define PLL_RANGE 2
# else
#  define PLL_RANGE 3
# endif
#endif

#define OXDIV_CLEAR 0x1F
#define OXMUX_CLEAR 0x07
#define DLYX_CLEAR  0x1F
#define DIV_CLEAR   0x03

#endif // CYGHWR_HAL_CORTEXM_A2FXXX_CLOCK

//==========================================================================
// IO settings

#ifdef CYGPKG_HAL_CORTEXM_A2FXXX_IO

# if defined(CYGHWR_HAL_CORTEXM_A2FXXX_IO_WEST_BANK_LVCMOS_3V3)
# define IO_WEST_BANK CYGHWR_HAL_A2FXXX_MSS_IO_BANK_CR_BTWEST(LVCMOS_3V3)
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_IO_WEST_BANK_LVCMOS_2V5)
# define IO_WEST_BANK CYGHWR_HAL_A2FXXX_MSS_IO_BANK_CR_BTWEST(LVCMOS_2V5)
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_IO_WEST_BANK_LVCMOS_1V8)
# define IO_WEST_BANK CYGHWR_HAL_A2FXXX_MSS_IO_BANK_CR_BTWEST(LVCMOS_1V8)
# else
# define IO_WEST_BANK CYGHWR_HAL_A2FXXX_MSS_IO_BANK_CR_BTWEST(LVCMOS_1V5)
# endif

# if defined(CYGHWR_HAL_CORTEXM_A2FXXX_IO_EAST_BANK_LVCMOS_3V3)
# define IO_EAST_BANK CYGHWR_HAL_A2FXXX_MSS_IO_BANK_CR_BTEAST(LVCMOS_3V3)
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_IO_EAST_BANK_LVCMOS_2V5)
# define IO_EAST_BANK CYGHWR_HAL_A2FXXX_MSS_IO_BANK_CR_BTEAST(LVCMOS_2V5)
# elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_IO_EAST_BANK_LVCMOS_1V8)
# define IO_EAST_BANK CYGHWR_HAL_A2FXXX_MSS_IO_BANK_CR_BTEAST(LVCMOS_1V8)
# else
# define IO_EAST_BANK CYGHWR_HAL_A2FXXX_MSS_IO_BANK_CR_BTEAST(LVCMOS_1V5)
# endif

#endif // CYGPKG_HAL_CORTEXM_A2FXXX_IO


#ifdef CYGSEM_HAL_CORTEXM_A2FXXX_CLOCK_CHATTER
# define A2FXXX_CLOCK_TRACE(args...)   diag_printf(args)
#else
# define A2FXXX_CLOCK_TRACE(args...)    /* NOOP */
#endif


//==========================================================================
// Clock frequencies
//
// These are set to the frequencies of the various system clocks.

cyg_uint32 hal_a2fxxx_pclk0;           // Peripheral clock (connected to APB_0)
cyg_uint32 hal_a2fxxx_pclk1;           // Peripheral clock (connected to APB_1)
cyg_uint32 hal_a2fxxx_aclk;            // Analog Peripheral clock (ADC /DAC etc..)
cyg_uint32 hal_a2fxxx_pclk_fpga;       // FPGA clock

cyg_uint32 hal_a2fxxx_glb;             // Global Clock B
cyg_uint32 hal_a2fxxx_glc;             // Global Clock C

cyg_uint32 hal_cortexm_systick_clock;  // NGMUx output or FCLK or SysTick clock
cyg_uint32 hal_a2fxxx_fclk;            // FCLK


//==========================================================================
// Local prototypes

static void hal_a2fxxx_clk_network_chatter( cyg_uint8 );

static cyg_uint8 hal_start_clocks( void );
static cyg_uint8 hal_a2fxxx_get_system_boot_clk( void );

#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLOCK
static void hal_a2fxxx_set_clk_network( void );
static void hal_a2fxxx_set_envm_timing( void );
static void hal_a2fxxx_set_clkx( cyg_uint8 x, cyg_uint8 src );
static void hal_a2fxxx_set_clkgx( cyg_uint8 x );
static void hal_a2fxxx_set_clkgx_default( cyg_uint8 x );
static void hal_a2fxxx_set_ngmux( cyg_uint8 );
static cyg_uint8 hal_a2fxxx_get_ngmux( void );

# if defined(CYGHWR_HAL_CORTEXM_A2FXXX_PLL)
static inline void hal_a2fxxx_set_pll( void );
# endif
#endif


//==========================================================================

void
hal_variant_init( void )
{
#if !defined(CYG_HAL_STARTUP_RAM)
    cyg_uint8 sysboot_miss;
#endif
    cyg_uint32 base = CYGHWR_HAL_A2FXXX_WD;

    // Disable Watch-dog
    HAL_WRITE_UINT32(base + CYGHWR_HAL_A2FXXX_WD_ENABLE,
                                           CYGHWR_HAL_A2FXXX_WD_DISABLE_KEY);

#if !defined(CYG_HAL_STARTUP_RAM)
    sysboot_miss = hal_start_clocks();
#endif

    // Release GPIO
    CYGHWR_HAL_A2FXXX_PERIPH_RELEASE( CYGHWR_HAL_A2FXXX_PERIPH_SOFTRST(GPIO) );

    // Setup IO banks
#ifdef CYGPKG_HAL_CORTEXM_A2FXXX_IO
    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_MSS_IO_BANK_CR, (IO_WEST_BANK | IO_EAST_BANK) );
#endif

    // Reset UART0
#if CYGINT_HAL_A2FXXX_UART0>0
    CYGHWR_HAL_A2FXXX_PERIPH_RELEASE( CYGHWR_HAL_A2FXXX_PERIPH_SOFTRST(UART0) );
#endif

    // Reset UART1
#if CYGINT_HAL_A2FXXX_UART1>0
    CYGHWR_HAL_A2FXXX_PERIPH_RELEASE( CYGHWR_HAL_A2FXXX_PERIPH_SOFTRST(UART1) );
#endif

#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    hal_if_init();
#endif

#if !defined(CYG_HAL_STARTUP_RAM)
    hal_a2fxxx_clk_network_chatter( sysboot_miss );
#endif
}

//==========================================================================
// Setup up system clocks
//
// Set up clocks from configuration. In the future this should be extended so
// that clock rates can be changed at runtime.

static cyg_uint8
hal_start_clocks( void )
{
    if( ! hal_a2fxxx_get_system_boot_clk( ) )
        return 0;

    //
    // User has chosen to re-configure the clock network and
    // overwrite the settings provided by the system boot
    //
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLOCK
    hal_a2fxxx_set_clk_network( );

    // Adjust the ENVM timing according to the core clock
    // frequency
    hal_a2fxxx_set_envm_timing( );
#endif

    return 1;
}


//==========================================================================
// Read clock configuration as setup by the system boot

static cyg_uint8
hal_a2fxxx_get_system_boot_clk( void )
{
    cyg_uint32 sysboot_key, sysboot_ver, clk_cr, clkga0;

    sysboot_ver = CYGHWR_HAL_A2FXXX_GET_SYSBOOT_VERSION();
    sysboot_key = CYGHWR_HAL_A2FXXX_GET_SYSBOOT_KEY();

    // Verify that the system boot is present, if not we must give up?
    if( CYGHWR_HAL_A2FXXX_ENVM_SPARE_PAGES_SYSBOOT_KEY_VALUE
             != sysboot_key )
       return 0;

    // Read the MSS CLGA0 frequency value. Address varies with system boot
    // version
    if ( sysboot_ver < CYGHWR_HAL_A2FXXX_SYSBOOT_VERSION(2, 0, 0) ) {
       clkga0 = CYGHWR_HAL_A2FXXX_GET_SYSBOOT_1_3_FCLK();
    }
    else {
       clkga0 = CYGHWR_HAL_A2FXXX_GET_SYSBOOT_2_0_FCLK();
    }

    // Compute the BUS and fabric clock frequency
    hal_cortexm_systick_clock = clkga0;

    HAL_READ_UINT32( CYGHWR_HAL_A2FXXX_SC + CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR, clk_cr);

    hal_a2fxxx_pclk0     =
          clkga0 >> CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_GET_PCLK0DIV( clk_cr );
    hal_a2fxxx_pclk1     =
          clkga0 >> CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_GET_PCLK1DIV( clk_cr );
    hal_a2fxxx_pclk_fpga =
          clkga0 >> CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_GET_GLBDIV( clk_cr );

    return 1;
}


//==========================================================================
// Chatter function

static void
hal_a2fxxx_clk_network_chatter( cyg_uint8 hal_clk_network_chatter )
{
    if ( hal_clk_network_chatter == 0 )
        diag_printf("Actel System Boot Missing\n Clock network configuration failed!!\n");

    A2FXXX_CLOCK_TRACE("Device clock network configuration:\n");
    A2FXXX_CLOCK_TRACE("Global Clock A (Systick): %d [Hz]\n",
                                                  hal_cortexm_systick_clock);
    A2FXXX_CLOCK_TRACE("PCLK0: %d [Hz]\n", hal_a2fxxx_pclk0);
    A2FXXX_CLOCK_TRACE("PCLK1: %d [Hz]\n", hal_a2fxxx_pclk1);
}


//==========================================================================
// Setup device clock network

#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLOCK

static void
hal_a2fxxx_set_clk_network( void )
{
    cyg_uint32 mss_ccc_mux;
    cyg_uint32 mss_clk_cr;
    cyg_uint8  ngmux;
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_PLL
    cyg_bool   pll = false;
#endif
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKA
    cyg_bool  clka = false;
#endif
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKC
    cyg_bool  clkc = false;
#endif
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKGA
    cyg_bool  clkga = false;
#endif
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKGC
    cyg_bool  clkgc = false;
#endif

    // Find out which CLKGx is currently used as FLCK
    ngmux = hal_a2fxxx_get_ngmux( );

    //
    // Start by configuring NGMUX. In the following case, if
    // the current NGMUX output is already CLKGA, first move
    // to CLKGC to keep a stable ground otherwise go ahead and
    // configure CLKGA.
    //
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_NGMUX_CLKOUT_CLKGA
    if( ngmux == CYGHWR_HAL_A2FXXX_SC_MSS_CCC_GLMUX_GLA )
    {
        // Setup CLKGC to use the On-Chip RC oscillator and move
        // the MSS clock to CLKGC. No division factor is applied
        // therefore FCLK = 100MHz

        // CLKC = RCOSC
        hal_a2fxxx_set_clkx( CLKx_SRC_CLKC, CLKx_SRC_RCOSC );

        // CLKGC = CLKC
        hal_a2fxxx_set_clkgx_default( CLKx_SRC_CLKGC );

        // FCLK = CLKGC
        hal_a2fxxx_set_ngmux( CLKx_SRC_CLKGC );
    }
    else {
        HAL_READ_UINT32( CYGHWR_HAL_A2FXXX_SC +
                         CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR, mss_ccc_mux);
        mss_ccc_mux &= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_OCMUX(OXMUX_CLEAR);
        mss_ccc_mux = mss_ccc_mux >> 19;

        // Make sure PLL is bypassed.
        if ( mss_ccc_mux != 1 ){
            hal_a2fxxx_set_clkx( CLKx_SRC_CLKC, CLKx_SRC_RCOSC );
            hal_a2fxxx_set_clkgx_default( CLKx_SRC_CLKGC );
        }
    }

    // Now we can configure CLKGA as specified by the
    // user. Check is PLL should be configured.
    hal_a2fxxx_set_clkx( CLKx_SRC_CLKA, CLKA_SRC_SEL );

# if defined(CYGHWR_HAL_CORTEXM_A2FXXX_PLL)
    hal_a2fxxx_set_pll( );
    pll = true;
# endif

    // Move to CLKGA
    hal_a2fxxx_set_clkgx( CLKx_SRC_CLKGA );
    clka = clkga = true;
    hal_a2fxxx_set_ngmux( CLKx_SRC_CLKGA );

#endif  // CYGHWR_HAL_CORTEXM_A2FXXX_NGMUX_CLKOUT_CLKGA


    //
    // In the following case, NGMUX is configured to use CLKGC.
    // This can be tricky as CLKGC can be routed from CLKGA...
    //
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_NGMUX_CLKOUT_CLKGC

    HAL_READ_UINT32( CYGHWR_HAL_A2FXXX_SC +
                           CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR, mss_ccc_mux);
    mss_ccc_mux &= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_OCMUX(OXMUX_CLEAR);
    mss_ccc_mux = mss_ccc_mux >> 19;

    // If CLKGC is not routed form CLKC or if CLKGC is not used
    // as NGMUX output, proceed to configure CLKGC = CLKC
    if( ((ngmux == CYGHWR_HAL_A2FXXX_SC_MSS_CCC_GLMUX_GLC) && (mss_ccc_mux != 1)) ||
                (ngmux == CYGHWR_HAL_A2FXXX_SC_MSS_CCC_GLMUX_GLA) )
    {

        // Set CLKC source, use on-chip oscillator as default
        // value
# ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKGC_SRC_CLKC
        hal_a2fxxx_set_clkx( CLKx_SRC_CLKC, CLKC_SRC_SEL );
        clkc = true;
# else
        hal_a2fxxx_set_clkx( CLKx_SRC_CLKC, CLKx_SRC_RCOSC );
# endif

        // Set CLKGC = CLKC
        hal_a2fxxx_set_clkgx_default( CLKx_SRC_CLKGC );

        // Set NGMUX = CLKGC
        hal_a2fxxx_set_ngmux( CLKx_SRC_CLKGC );

        // Configure PLL input if in use
# ifdef CYGHWR_HAL_CORTEXM_A2FXXX_PLL
        hal_a2fxxx_set_clkx( CLKx_SRC_CLKA, CLKA_SRC_SEL );
        clka = true;
# endif

    }
    else {

        // Set CLKA source, use on-chip oscillator as default
        // value
# ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKA
        hal_a2fxxx_set_clkx( CLKx_SRC_CLKA, CLKA_SRC_SEL );
        clka = true;
# else
        hal_a2fxxx_set_clkx( CLKx_SRC_CLKA, CLKx_SRC_RCOSC );
# endif

# ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKGC_SRC_CLKC
        // CLKGA = CLKA
        hal_a2fxxx_set_clkgx_default( CLKx_SRC_CLKGA );

        // Move NGMUX = CLKGA
        hal_a2fxxx_set_ngmux( CLKx_SRC_CLKGA );

        // Now we can Configure CLKC
        hal_a2fxxx_set_clkx( CLKx_SRC_CLKC, CLKC_SRC_SEL );
# endif

    }

# ifdef CYGHWR_HAL_CORTEXM_A2FXXX_PLL
    hal_a2fxxx_set_pll( );
    pll = true;
# endif

    clkgc = true;
    hal_a2fxxx_set_clkgx( CLKx_SRC_CLKGC );
    hal_a2fxxx_set_ngmux( CLKx_SRC_CLKGC );

#endif // CYGHWR_HAL_CORTEXM_A2FXXX_NGMUX_CLKOUT_CLKGC


    // Configure CLKA
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKA
    if ( clka == false ){
       hal_a2fxxx_set_clkx( CLKx_SRC_CLKA, CLKA_SRC_SEL );
    }
#endif

    // Configure the PLL
#if defined(CYGHWR_HAL_CORTEXM_A2FXXX_PLL)
    if ( pll == false ){
        hal_a2fxxx_set_pll( );
    }
#endif

    // Configure CLKB
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKB
    hal_a2fxxx_set_clkx( CLKx_SRC_CLKB, CLKB_SRC_SEL );
#endif

    // Configure CLKC
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKC
    if ( clkc == false ){
        hal_a2fxxx_set_clkx( CLKx_SRC_CLKC, CLKC_SRC_SEL );
    }
#endif

    // Configure CLKGA
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKGA
    if ( clkga == false ){
        hal_a2fxxx_set_clkgx( CLKx_SRC_CLKA );
    }
#endif

    // Configure CLKGB
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKGB
    hal_a2fxxx_set_clkgx( CLKx_SRC_CLKB );
#endif

    // Configure CLKGC
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKGC
    if ( clkgc == false ){
        hal_a2fxxx_set_clkgx( CLKx_SRC_CLKC );
    }
#endif

    // Now, configure all MSS clock dividers
    //
    HAL_READ_UINT32( CYGHWR_HAL_A2FXXX_SC +
                           CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR, mss_ccc_mux);
    HAL_READ_UINT32( CYGHWR_HAL_A2FXXX_SC +
                           CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR, mss_clk_cr);

#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_ACE_CLK
    mss_clk_cr &= ~( CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_ACLKDIV(DIV_CLEAR) );
    mss_clk_cr |=  ( CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_ACLKDIV(ACLKDIV) );
#endif

    mss_clk_cr &= ~( CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_PCLK1DIV(DIV_CLEAR) |
                     CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_PCLK0DIV(DIV_CLEAR) );

    mss_clk_cr |=  ( CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_PCLK1DIV(PCLK1DIV) |
                     CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_PCLK0DIV(PCLK0DIV) );

    // Record the different clock frequencies
    hal_cortexm_systick_clock = CYGHWR_HAL_CORTEXM_A2FXXX_NGMUX_CLKOUT_FREQ;
    hal_a2fxxx_fclk  = CYGHWR_HAL_CORTEXM_A2FXXX_NGMUX_CLKOUT_FREQ;
    hal_a2fxxx_pclk0 = CYGHWR_HAL_CORTEXM_A2FXXX_PCLK0_FREQ;
    hal_a2fxxx_pclk1 = CYGHWR_HAL_CORTEXM_A2FXXX_PCLK1_FREQ;

#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_FAB_CLK
    hal_a2fxxx_pclk_fpga = CYGHWR_HAL_CORTEXM_A2FXXX_FAB_CLK_FREQ;
#endif

    // Configure the MAC clock
#if defined(CYGHWR_HAL_CORTEXM_A2FXXX_MAC_CLK) && \
          defined(CYGHWR_HAL_CORTEXM_A2FXXX_MAC_SRC_CLKGC)

    mss_clk_cr |= CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_RMIICLKSEL;

#elif defined(CYGHWR_HAL_CORTEXM_A2FXXX_MAC_CLK) && \
          defined(CYGHWR_HAL_CORTEXM_A2FXXX_MAC_SRC_EXTERNAL)

    mss_clk_cr &= ~CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_RMIICLKSEL;

#endif

    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_SC + CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR,
                      mss_clk_cr);

    // PLL is disable if not in used
#if !defined(CYGHWR_HAL_CORTEXM_A2FXXX_PLL)
    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_SC +
                      CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR, 0x0);
#endif
}


//==========================================================================
// Get / Set NGMUX settings

static cyg_uint8
hal_a2fxxx_get_ngmux( void )
{
    cyg_uint32 mss_ccc_mux;
    cyg_uint8  ngmux_setting;
    HAL_READ_UINT32( CYGHWR_HAL_A2FXXX_SC +
                     CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR, mss_ccc_mux);

    mss_ccc_mux = mss_ccc_mux >> 24;
    switch( mss_ccc_mux & 0xf ){
    case  0:
    case  2:
    case  4:
    case  6:
    case 12:
        ngmux_setting = CYGHWR_HAL_A2FXXX_SC_MSS_CCC_GLMUX_GLA;
        break;
    case  1:
    case  3:
    case  8:
    case 10:
    case 13:
        ngmux_setting = CYGHWR_HAL_A2FXXX_SC_MSS_CCC_GLMUX_GLC;
        break;
    case  5:
    case  7:
    case  9:
    case 11:
    case 14:
        ngmux_setting = CYGHWR_HAL_A2FXXX_SC_MSS_CCC_GLMUX_GLINT;
        break;
    default:
        ngmux_setting = CYGHWR_HAL_A2FXXX_SC_MSS_CCC_GLMUX_GND;
        break;
    }

    return ngmux_setting;
}


static void
hal_a2fxxx_set_ngmux( cyg_uint8 x )
{
    cyg_uint32 mss_ccc_mux;
    HAL_READ_UINT32( CYGHWR_HAL_A2FXXX_SC +
                     CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR, mss_ccc_mux);

    // Clear bits
    mss_ccc_mux &= ~( CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_GLMUXSEL(0x3) |
                      CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_GLMUXCFG(0x3) );

    if( x == CLKx_SRC_CLKGA ){
        mss_ccc_mux |= ( CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_GLMUXSEL(0) |
                         CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_GLMUXCFG(0) );
    }
    else if (x == CLKx_SRC_CLKGC ){
        mss_ccc_mux |= ( CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_GLMUXSEL(1) |
                         CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_GLMUXCFG(0) );
    }
    else{
        mss_ccc_mux |= ( CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_GLMUXSEL(2) |
                         CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_GLMUXCFG(3) );
    }

    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_SC +
                      CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR, mss_ccc_mux);
}


//==========================================================================
// Setup PLL

#if defined(CYGHWR_HAL_CORTEXM_A2FXXX_PLL)
static inline void
hal_a2fxxx_set_pll( void )
{
    cyg_uint32 x =
    CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR_FINDIV((CYGHWR_HAL_CORTEXM_A2FXXX_PLL_DIV-1)) |
    CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR_FBDIV((CYGHWR_HAL_CORTEXM_A2FXXX_PLL_MULT-1)) |
    CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR_FBSEL(1) |
    CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR_FBDLY(CYGHWR_HAL_CORTEXM_A2FXXX_PLL_FB_DELAY) |
    CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR_VCOSEL0 |
    CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR_VCOSEL2_1(PLL_RANGE) |
    CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR_PLLEN;

    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_SC + CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR, x);

    // Wait PLL lock
    do {
        HAL_READ_UINT32( CYGHWR_HAL_A2FXXX_SC + CYGHWR_HAL_A2FXXX_SC_MSS_CCC_SR, x);
    } while( (x & CYGHWR_HAL_A2FXXX_SC_MSS_CCC_SR_PLL_LOCK_SYNC) == 0);
}
#endif


//==========================================================================
// Setup CLKA, CLKB or CLKC input clock

static void
hal_a2fxxx_set_clkx( cyg_uint8 x, cyg_uint8 src )
{
    cyg_uint32 mss_ccc_mux;
    HAL_READ_UINT32( CYGHWR_HAL_A2FXXX_SC +
                     CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR, mss_ccc_mux);
    if( x == CLKx_SRC_CLKA ){
        mss_ccc_mux &= ~( CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_DYNASEL |
                      CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_RXASEL  |
                      CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_STATASEL );
        mss_ccc_mux |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_CLKA_SEL (src);
    }
    else if (x == CLKx_SRC_CLKB ){
        mss_ccc_mux &= ~( CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_DYNBSEL |
                      CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_RXBSEL  |
                      CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_STATBSEL );
        mss_ccc_mux |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_CLKB_SEL (src);
    }
    else{
        mss_ccc_mux &= ~( CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_DYNCSEL |
                      CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_RXCSEL  |
                      CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_STATCSEL );
        mss_ccc_mux |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_CLKC_SEL (src);
    }
    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_SC +
                      CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR, mss_ccc_mux);
}


//==========================================================================
// Setup CLKGA, CLKGB or CLKGC output clock

static void
hal_a2fxxx_set_clkgx( cyg_uint8 x )
{
    cyg_uint32 mss_ccc_mux;
    cyg_uint32 mss_ccc_dly;
    cyg_uint32 mss_ccc_div;
    cyg_uint8  div10;
    cyg_uint8  div;
    HAL_READ_UINT32( CYGHWR_HAL_A2FXXX_SC +
                     CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR, mss_ccc_mux);
    HAL_READ_UINT32( CYGHWR_HAL_A2FXXX_SC +
                     CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR, mss_ccc_dly);
    HAL_READ_UINT32( CYGHWR_HAL_A2FXXX_SC +
                     CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR, mss_ccc_div);

    if( x == CLKx_SRC_CLKGA ){
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKGA
        // Clear bits
        mss_ccc_div &= ~( CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OADIV(OXDIV_CLEAR) |
                          CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OADIVHALF );
        mss_ccc_dly &= ~CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR_DLYA(DLYX_CLEAR);
        mss_ccc_mux &= ~CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_OAMUX(OXMUX_CLEAR);
        mss_ccc_mux &= ~CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_BYPASSA;
        // Compute divider
        div10 = CYGHWR_HAL_CORTEXM_A2FXXX_CLKGA_DIV10;
        div = (cyg_uint8) CYGHWR_HAL_CORTEXM_A2FXXX_CLKGA_DIV;
        if( div10 % 10 ){
            div += 1;
            mss_ccc_div |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OADIVHALF;
        }
        else {
           div -= 1;
        }
        // CLKA = CLGA, bypass MUX
        if (div == 0 && CLKGA_SRC_SEL == 1) {
            mss_ccc_mux |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_BYPASSA;
        }
        else {
            mss_ccc_mux |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_OAMUX(CLKGA_SRC_SEL);
            mss_ccc_div |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OADIV(div);
        }
        mss_ccc_dly |=
        CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR_DLYA(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGA_DELAY);
#endif
    }
    if ( x == CLKx_SRC_CLKGB ){
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKGB
        // Clear bits
        mss_ccc_div &= ~( CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OBDIV(OXDIV_CLEAR) |
                          CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OBDIVHALF );
        mss_ccc_mux &= ~CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_OBMUX(OXMUX_CLEAR);
        mss_ccc_dly &= ~CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR_DLYB(DLYX_CLEAR);
        mss_ccc_mux &= ~CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_BYPASSB;
        // Compute divider
        div10 = CYGHWR_HAL_CORTEXM_A2FXXX_CLKGB_DIV10;
        div = (cyg_uint8) CYGHWR_HAL_CORTEXM_A2FXXX_CLKGB_DIV;
        if( div10 % 10 ){
            div += 1;
            mss_ccc_div |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OBDIVHALF;
        }
        else {
           div -= 1;
        }
        // CLKB = CLGB, bypass MUX
        if (div == 0 && CLKGB_SRC_SEL == 1) {
            mss_ccc_mux |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_BYPASSB;
        }
        else {
            mss_ccc_mux |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_OBMUX(CLKGB_SRC_SEL);
            mss_ccc_div |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OBDIV(div);
            mss_ccc_dly |=
            CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR_DLYB(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGB_DELAY);
        }
#endif
    }
    if ( x == CLKx_SRC_CLKGC ){
#ifdef CYGHWR_HAL_CORTEXM_A2FXXX_CLKGC
        // Clear bits
        mss_ccc_div &= ~( CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OCDIV(OXDIV_CLEAR) |
                          CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OCDIVHALF );
        mss_ccc_mux &= ~CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_OCMUX(OXMUX_CLEAR);
        mss_ccc_dly &= ~CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR_DLYC(DLYX_CLEAR);
        mss_ccc_mux &= ~CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_BYPASSC;
        // Compute divider
        div10 = CYGHWR_HAL_CORTEXM_A2FXXX_CLKGC_DIV10;
        div = (cyg_uint8) CYGHWR_HAL_CORTEXM_A2FXXX_CLKGC_DIV;
        if( div10 % 10 ){
            div += 1;
            mss_ccc_div |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OCDIVHALF;
        }
        else {
           div -= 1;
        }
        // CLKC = CLGC, bypass MUX
        if (div == 0 && CLKGC_SRC_SEL == 1) {
            mss_ccc_mux |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_BYPASSC;
        }
        else {
            mss_ccc_mux |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_OCMUX(CLKGC_SRC_SEL);
            mss_ccc_div |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OCDIV(div);
        }
        mss_ccc_dly |=
        CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR_DLYC(CYGHWR_HAL_CORTEXM_A2FXXX_CLKGC_DELAY);
#endif
    }
    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_SC +
                      CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR, mss_ccc_div);
    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_SC +
                      CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR, mss_ccc_dly);
    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_SC +
                      CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR, mss_ccc_mux);
}

//==========================================================================
// Setup CLKGA CLKGC output clock

static void
hal_a2fxxx_set_clkgx_default( cyg_uint8 x )
{
    cyg_uint32 mss_ccc_mux;
    cyg_uint32 mss_ccc_dly;
    cyg_uint32 mss_ccc_div;

    HAL_READ_UINT32( CYGHWR_HAL_A2FXXX_SC +
                     CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR, mss_ccc_mux);
    HAL_READ_UINT32( CYGHWR_HAL_A2FXXX_SC +
                     CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR, mss_ccc_dly);
    HAL_READ_UINT32( CYGHWR_HAL_A2FXXX_SC +
                     CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR, mss_ccc_div);

    if( x == CLKx_SRC_CLKGA ){
        // Clear bits
        mss_ccc_div &= ~( CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OADIV(OXDIV_CLEAR) |
                          CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OADIVHALF );
        mss_ccc_dly &= ~CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR_DLYA(DLYX_CLEAR);
        mss_ccc_mux &= ~CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_OAMUX(OXMUX_CLEAR);
        // CLKGA = CLKA, Division factor = 1 and minimum delay of 535ps
        mss_ccc_mux |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_BYPASSA;
        mss_ccc_dly |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR_DLYA(0);
    }
    if ( x == CLKx_SRC_CLKGC ){
        // Clear bits
        mss_ccc_div &= ~( CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OCDIV(OXDIV_CLEAR) |
                          CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OCDIVHALF );
        mss_ccc_mux &= ~CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_OCMUX(OXMUX_CLEAR);
        mss_ccc_dly &= ~CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR_DLYC(DLYX_CLEAR);
        // CLKGC = CLKC, Division factor = 1 and minimum delay of 535ps
        mss_ccc_mux |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_BYPASSC;
        mss_ccc_dly |= CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR_DLYC(0);
    }
    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_SC +
                      CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR, mss_ccc_div);
    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_SC +
                      CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR, mss_ccc_dly);
    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_SC +
                      CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR, mss_ccc_mux);
}


//==========================================================================
// Adjust ENVM timing

static void
hal_a2fxxx_set_envm_timing( void ) {
    cyg_uint32 x;
    HAL_READ_UINT32( CYGHWR_HAL_A2FXXX_SC + CYGHWR_HAL_A2FXXX_SC_ENVM_CR, x);
    x &= ~CYGHWR_HAL_A2FXXX_SC_ENVM_CR_SIX_CYCLE;
    if( hal_a2fxxx_fclk <= 80e6 ){
        x |= CYGHWR_HAL_A2FXXX_SC_ENVM_CR_PIPE_BYPASS;
    } else {
        x &= ~CYGHWR_HAL_A2FXXX_SC_ENVM_CR_PIPE_BYPASS;
    }
    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_SC + CYGHWR_HAL_A2FXXX_SC_ENVM_CR, x);
}

#endif // CYGHWR_HAL_CORTEXM_A2FXXX_CLOCK


//==========================================================================
// Reset modules

__externC void
hal_a2fxxx_periph_release( cyg_uint32 bit )
{
    cyg_uint32 sc, base = CYGHWR_HAL_A2FXXX_SC;
    HAL_READ_UINT32( base + CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR, sc );
    sc &= ~( bit );
    HAL_WRITE_UINT32( base + CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR, sc );
}


__externC void
hal_a2fxxx_periph_reset( cyg_uint32 bit )
{
    cyg_uint32 sc, base = CYGHWR_HAL_A2FXXX_SC;
    HAL_READ_UINT32( base + CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR , sc );
    sc |= ( bit );
    HAL_WRITE_UINT32( base + CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR , sc );
}


//==========================================================================
// GPIO support
//

const default_io_mux_type default_io_mux[] = {
   // SPI0
   {0x00}, {0x0a}, {0x00}, {0x00},
   // UART0
   {0x18}, {0x00},
   // I2C0
   {0x00}, {0x00},
   // SPI1
   {0x00}, {0x0a}, {0x00}, {0x00},
   // UART1
   {0x18}, {0x00},
   // I2C1
   {0x00}, {0x00},
   // MAC0
   {0x18}, {0x18}, {0x0a}, {0x0a}, {0x18}, {0x0a}, {0x0a}, {0x00}, {0x18},
   // GPIO
   {0x00}, {0x00}, {0x00}, {0x00}, {0x00}, {0x00}, {0x00}, {0x00},
   {0x00}, {0x00}, {0x00}, {0x00}, {0x00}, {0x00}, {0x00}, {0x00},
   {0x00}, {0x00}, {0x00}, {0x00}, {0x00}, {0x00}, {0x00}, {0x00},
   {0x00}, {0x00}, {0x00}, {0x00}, {0x00}, {0x00}, {0x00}, {0x00},
   // SPI0 CS
   {0x00}, {0x00}, {0x00},
   // Unused
   {0x00}, {0x00}, {0x00}, {0x00},
   // UART0 misc
   {0x00}, {0x00}, {0x00}, {0x00}, {0x00}, {0x00},
   // SPI1 CS
   {0x00}, {0x00}, {0x00}, {0x00}, {0x00}, {0x00}, {0x00},
   // UART1 misc
   {0x00}, {0x00}, {0x00}, {0x00}, {0x00}, {0x00},
   {0x0}
};

// These functions provide configuration and IO for GPIO pins.

__externC void
hal_a2fxxx_gpio_set( cyg_uint32 pin )
{
    cyg_uint32 bit  = CYGHWR_HAL_A2FXXX_GPIO_BIT( pin );
    cyg_uint32 mode = CYGHWR_HAL_A2FXXX_GPIO_MODE( pin );
    cyg_uint32 mux  = CYGHWR_HAL_A2FXXX_GPIO_IOMUX_IDX( pin );
    cyg_uint32 irq  = CYGHWR_HAL_A2FXXX_GPIO_IRQ( pin );
    cyg_uint32 reg    = 0;
    cyg_uint32 io_mux = 0;

    if (pin == CYGHWR_HAL_A2FXXX_GPIO_NONE)
        return;

    if(mode == CYGHWR_HAL_A2FXXX_GPIO_MODE_PERIPH){
       // Set IOMUX default value for peripheral use
       io_mux = default_io_mux[mux].val;
       HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_SC +
                         CYGHWR_HAL_A2FXXX_SC_IOMUX( mux ), io_mux );
    } else {
       // Set GPIO input/ output mode
       if (mode == CYGHWR_HAL_A2FXXX_GPIO_MODE_OUT) {
           reg = CYGHWR_HAL_A2FXXX_GPIO_CFG_GPOUTEN |
                 CYGHWR_HAL_A2FXXX_GPIO_CFG_OUTBUFEN;
       }

       // Configure IO as interrupt
       if (irq != CYGHWR_HAL_A2FXXX_GPIO_IRQ_DISABLE) {
           reg |= CYGHWR_HAL_A2FXXX_GPIO_CFG_GPINTEN;
           if(irq & CYGHWR_HAL_A2FXXX_GPIO_IRQ_FALLING_EDGE)
              reg |= CYGHWR_HAL_A2FXXX_GPIO_CFG_INTYPE(0);
           else if(irq & CYGHWR_HAL_A2FXXX_GPIO_IRQ_RISING_EDGE)
              reg |= CYGHWR_HAL_A2FXXX_GPIO_CFG_INTYPE(1);
           else if(irq & CYGHWR_HAL_A2FXXX_GPIO_IRQ_BOTH_EDGES)
              reg |= CYGHWR_HAL_A2FXXX_GPIO_CFG_INTYPE(2);
           else if(irq & CYGHWR_HAL_A2FXXX_GPIO_IRQ_LOW_LEVEL)
              reg |= CYGHWR_HAL_A2FXXX_GPIO_CFG_INTYPE(3);
           else
              reg |= CYGHWR_HAL_A2FXXX_GPIO_CFG_INTYPE(4);
       }

       // Write down result
       HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_MSS_GPIO +
                       ( CYGHWR_HAL_A2FXXX_MSS_GPIO_CFG( bit ) ), reg );

       // For GPIO 16 to 31, we must setup the IOMUX
       // TODO, this is for output only?
       if( bit > 15 ){
          bit = bit - 16;
          io_mux = 0x35;
          HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_SC +
                            CYGHWR_HAL_A2FXXX_SC_IOMUX( bit ), io_mux );
       }

    }
}


__externC void
hal_a2fxxx_gpio_out( cyg_uint32 pin, int val )
{
    cyg_uint32 bit = ( CYGHWR_HAL_A2FXXX_GPIO_BIT(pin) << 2 );
    cyg_uint32 port =  CYGHWR_HAL_A2FXXX_MSS_GPIO_BB +
                     ( CYGHWR_HAL_A2FXXX_MSS_GPIO_DOUT << 5 );

    port += bit;

    HAL_WRITE_UINT32( port, val );
}


__externC void
hal_a2fxxx_gpio_in ( cyg_uint32 pin, int *val )
{
    cyg_uint32 bit = ( CYGHWR_HAL_A2FXXX_GPIO_BIT(pin) << 2 );
    cyg_uint32 port =  CYGHWR_HAL_A2FXXX_MSS_GPIO_BB +
                     ( CYGHWR_HAL_A2FXXX_MSS_GPIO_DIN << 5 );
    cyg_uint32 pd;

    port += bit;

    HAL_READ_UINT32( port, pd );
    *val = 1 ? (bit & pd) : 0;
}


//==========================================================================
// I2C clock rate
//
__externC cyg_uint32
hal_a2fxxx_i2c_clock( cyg_uint32 base )
{
    return ( (base == CYGHWR_HAL_A2FXXX_I2C0) ? hal_a2fxxx_pclk0 : hal_a2fxxx_pclk1 );
}


//==========================================================================
// SPI clock rate
//
__externC cyg_uint32
hal_a2fxxx_spi_clock( cyg_uint32 base )
{
    return ( (base == CYGHWR_HAL_A2FXXX_SPI0) ? hal_a2fxxx_pclk0 : hal_a2fxxx_pclk1 );
}


//==========================================================================
// UART baud rate
//
// Set the baud rate divider of a UART based on the requested rate and
// the current APB clock settings.

__externC void
hal_a2fxxx_uart_setbaud(cyg_uint32 base, cyg_uint32 baud)
{
    cyg_uint16 divider =
         CYG_HAL_CORTEXM_A2FXXX_BAUD_GENERATOR( ((base == CYGHWR_HAL_A2FXXX_UART0)? 0 : 1) , baud );

    HAL_WRITE_UINT32(base + CYGHWR_HAL_A2FXXX_UART16550_LCR,
                                  CYGHWR_HAL_A2FXXX_UART16550_LCR_DLAB);
    HAL_WRITE_UINT32(base + CYGHWR_HAL_A2FXXX_UART16550_DMR, divider >> 8);
    HAL_WRITE_UINT32(base + CYGHWR_HAL_A2FXXX_UART16550_DLR, divider & 0xFF);
}


//==========================================================================
// Idle thread
//
#ifdef CYGSEM_HAL_CORTEXM_A2FXXX_DEFINES_IDLE_THREAD_ACTION
__externC void
hal_idle_thread_action( cyg_uint32 count )
{
    while(1);
}
#endif

//==========================================================================
// EOF a2fxxx_misc.c
