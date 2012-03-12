#ifndef CYGONCE_HAL_VAR_IO_H
#define CYGONCE_HAL_VAR_IO_H
//=============================================================================
//
//      var_io.h
//
//      Variant specific registers
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   ccoutand
// Date:        2011-02-03
// Purpose:     Smartfusion Cortex-M3 variant specific registers
// Description:
// Usage:       #include <cyg/hal/var_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal_cortexm_a2fxxx.h>

#include <cyg/hal/plf_io.h>

//=============================================================================
// Peripherals

#define CYGHWR_HAL_A2FXXX_SC                               0xE0042000

// Device peripherals
#define CYGHWR_HAL_A2FXXX_UART0                            0x40000000
#define CYGHWR_HAL_A2FXXX_SPI0                             0x40001000
#define CYGHWR_HAL_A2FXXX_I2C0                             0x40002000
#define CYGHWR_HAL_A2FXXX_MAC                              0x40003000
#define CYGHWR_HAL_A2FXXX_DMA                              0x40004000
#define CYGHWR_HAL_A2FXXX_TIMER                            0x40005000
#define CYGHWR_HAL_A2FXXX_WD                               0x40006000
#define CYGHWR_HAL_A2FXXX_FABRIC_IIC                       0x40007000
#define CYGHWR_HAL_A2FXXX_UART1                            0x40010000
#define CYGHWR_HAL_A2FXXX_SPI1                             0x40011000
#define CYGHWR_HAL_A2FXXX_I2C1                             0x40012000
#define CYGHWR_HAL_A2FXXX_MSS_GPIO                         0x40013000
#define CYGHWR_HAL_A2FXXX_RTC                              0x40014000
#define CYGHWR_HAL_A2FXXX_EFROM                            0x40015000
#define CYGHWR_HAL_A2FXXX_IAP                              0x40016000
#define CYGHWR_HAL_A2FXXX_ANALOG_ENGINE                    0x40020000
#define CYGHWR_HAL_A2FXXX_FPAG_ESRAM                       0x40040000
#define CYGHWR_HAL_A2FXXX_FPGA                             0x40050000
#define CYGHWR_HAL_A2FXXX_PERIPH                           0x42000000

// Internal flash (Embedded Non-volatile Memory, ENVM)
#define CYGHWR_HAL_A2FXXX_ENVM_ARRAY                       0x60000000
#define CYGHWR_HAL_A2FXXX_ENVM_SPARE_PAGES                 0x60080000
#define CYGHWR_HAL_A2FXXX_ENVM_AUX_BLOCK_A                 0x60084000
#define CYGHWR_HAL_A2FXXX_ENVM_AUX_BLOCK_SP                0x60088000
#define CYGHWR_HAL_A2FXXX_ENVM                             0x60100000

// External memory base address
#define CYGHWR_HAL_A2FXXX_EMEM_TYPE0                       0x70000000
#define CYGHWR_HAL_A2FXXX_EMEM_TYPE1                       0x74000000

//=============================================================================
// Bit-Band base address
#define CYGHWR_HAL_A2FXXX_I2C0_BB                          ( CYGHWR_HAL_A2FXXX_PERIPH + 0x00040000 )
#define CYGHWR_HAL_A2FXXX_I2C1_BB                          ( CYGHWR_HAL_A2FXXX_PERIPH + 0x00240000 )
#define CYGHWR_HAL_A2FXXX_MSS_GPIO_BB                      ( CYGHWR_HAL_A2FXXX_PERIPH + 0x00260000 )
#define CYGHWR_HAL_A2FXXX_MAC_BB                           ( CYGHWR_HAL_A2FXXX_PERIPH + 0x00060000 )

//=============================================================================
// Device signature and ID registers

#define CYGHWR_HAL_A2FXXX_MCU_ID                           (CYGHWR_HAL_A2FXXX_CORTEXM3 + 0xD00)
#define CYGHWR_HAL_A2FXXX_MCU_ID_REV(__x)                  ((__x)&0xF)
#define CYGHWR_HAL_A2FXXX_MCU_PART_NO(__x)                 (((__x)>>4)&0x0FFF)
#define CYGHWR_HAL_A2FXXX_MCU_VAR_NO(__x)                  (((__x)>>20)&0xF)


//=============================================================================
// System Control
//
#define CYGHWR_HAL_A2FXXX_SC_ESRAM_CR                      0x000
#define CYGHWR_HAL_A2FXXX_SC_ENVM_CR                       0x004
#define CYGHWR_HAL_A2FXXX_SC_ENVM_REMAP_SYS_CR             0x008
#define CYGHWR_HAL_A2FXXX_SC_ENVM_REMAP_FAB_CR             0x00C
#define CYGHWR_HAL_A2FXXX_SC_FAB_PROT_SIZE_CR              0x010
#define CYGHWR_HAL_A2FXXX_SC_FAB_PROT_BASE_CR              0x014
#define CYGHWR_HAL_A2FXXX_SC_AHB_MATRIX_CR                 0x018
#define CYGHWR_HAL_A2FXXX_SC_MSS_SR                        0x01C
#define CYGHWR_HAL_A2FXXX_SC_CLR_MSS_SR                    0x020
#define CYGHWR_HAL_A2FXXX_SC_EFROM_CR                      0x024
#define CYGHWR_HAL_A2FXXX_SC_IAP_CR                        0x028
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTIRQ_CR                0x02C
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR                0x030
#define CYGHWR_HAL_A2FXXX_SC_DEVICE_SR                     0x034
#define CYGHWR_HAL_A2FXXX_SC_SYSTICK_CR                    0x038
#define CYGHWR_HAL_A2FXXX_SC_EMC_MUX_CR                    0x03C
#define CYGHWR_HAL_A2FXXX_SC_EMC_CS0_CR                    0x040
#define CYGHWR_HAL_A2FXXX_SC_EMC_CS1_CR                    0x044
#define CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR                    0x048
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR                0x04C
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR                0x050
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR                0x054
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR                0x058
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_SR                    0x05C
#define CYGHWR_HAL_A2FXXX_SC_VRPSM_CR                      0x064
#define CYGHWR_HAL_A2FXXX_SC_FAB_APB_HW_DR                 0x070
#define CYGHWR_HAL_A2FXXX_SC_LOOPBACK_CR                   0x074
#define CYGHWR_HAL_A2FXXX_SC_MSS_IO_BANK_CR                0x078
#define CYGHWR_HAL_A2FXXX_SC_GPIN_SOURCE_CR                0x07C
#define CYGHWR_HAL_A2FXXX_SC_IOMUX(_x_)                    ( 0x100 + (_x_ << 0x2) )

// Reset register
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_PAD            BIT_(19)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_F2M            BIT_(18)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_FPGA           BIT_(17)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_EXT            BIT_(16)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_IAP            BIT_(15)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_GPIO           BIT_(14)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_ACE            BIT_(13)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_I2C1           BIT_(12)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_I2C0           BIT_(11)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_SPI1           BIT_(10)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_SPI0           BIT_(9)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_UART1          BIT_(8)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_UART0          BIT_(7)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_TIMER          BIT_(6)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_PDMA           BIT_(5)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_MAC            BIT_(4)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_EMC            BIT_(3)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_ESRAM1         BIT_(2)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_ESRAM0         BIT_(1)
#define CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_ENVM           BIT_(0)

#define CYGHWR_HAL_A2FXXX_PERIPH_SOFTRST(_periph)          CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_##_periph

void hal_a2fxxx_periph_reset( cyg_uint32 );
void hal_a2fxxx_periph_release( cyg_uint32 );

#define CYGHWR_HAL_A2FXXX_PERIPH_RESET           hal_a2fxxx_periph_reset
#define CYGHWR_HAL_A2FXXX_PERIPH_RELEASE         hal_a2fxxx_periph_release

#if 0
#define CYGHWR_HAL_A2FXXX_PERIPH_START(_x)                 \
{                                                          \
    volatile int c = 0;                                    \
    hal_a2fxxx_periph_reset(_x);                           \
    while( c++ < 20 );                                     \
    hal_a2fxxx_periph_release(_x);                         \
}
#endif

// MSS CLK register
#define CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_GLBDIV(_x)         VALUE_(12, ((_x)&0x3))
#define CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_RTCIF(_x)          VALUE_( 8, ((_x)&0xf))
#define CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_ACLKDIV(_x)        VALUE_( 6, ((_x)&0x3))
#define CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_PCLK1DIV(_x)       VALUE_( 4, ((_x)&0x3))
#define CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_PCLK0DIV(_x)       VALUE_( 2, ((_x)&0x3))
#define CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_GET_PCLK0DIV(_x)   ((_x & 0x000C) >> 2)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_GET_PCLK1DIV(_x)   ((_x & 0x0030) >> 4)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_GET_GLBDIV(_x)     ((_x & 0x0C00) >> 12)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CLK_CR_RMIICLKSEL         BIT_(0)

// MSS CCC DIV register
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OCDIVRST       BIT_(22)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OCDIVHALF      BIT_(21)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OCDIV(_x)      VALUE_(16, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OBDIVRST       BIT_(14)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OBDIVHALF      BIT_(13)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OBDIV(_x)      VALUE_( 8, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OADIVRST       BIT_(6)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OADIVHALF      BIT_(5)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DIV_CR_OADIV(_x)      VALUE_( 0, _x)

// MSS CCC MUX register
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_MAINOSCM(_x)   VALUE_(30, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_MAINOSCEN      BIT_(29)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_GLMUXCFG(_x)   VALUE_(26, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_GLMUXSEL(_x)   VALUE_(24, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_BYPASSC        BIT_(22)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_OCMUX(_x)      VALUE_(19, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_DYNCSEL        BIT_(18)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_RXCSEL         BIT_(17)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_STATCSEL       BIT_(16)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_BYPASSB        BIT_(14)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_OBMUX(_x)      VALUE_(11, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_DYNBSEL        BIT_(10)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_RXBSEL         BIT_(9)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_STATBSEL       BIT_(8)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_BYPASSA        BIT_(6)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_OAMUX(_x)      VALUE_( 3, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_DYNASEL        BIT_(2)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_RXASEL         BIT_(1)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_STATASEL       BIT_(0)

#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_GLMUX_GLA             0
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_GLMUX_GLC             1
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_GLMUX_GLINT           2
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_GLMUX_GND             3

#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_CLKA_SEL(_x_)  \
({                                                         \
   cyg_uint32 _i = _x_;                                    \
   _i;                                                     \
})

#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_CLKB_SEL(_x_)  \
({                                                         \
   cyg_uint32 _i = _x_;                                    \
   _i = _i << 8;                                           \
   _i;                                                     \
})

#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_MUX_CR_CLKC_SEL(_x_)  \
({                                                         \
   cyg_uint32 _i = _x_;                                    \
   _i = _i << 16;                                          \
   _i;                                                     \
})

// MSS CCC PLL register
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR_PLLEN          BIT_(31)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR_VCOSEL2_1(_x)  VALUE_(23, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR_VCOSEL0        BIT_(22)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR_XDLYSEL(_x)    VALUE_(21, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR_FBDLY(_x)      VALUE_(16, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR_FBSEL(_x)      VALUE_(14, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR_FBDIV(_x)      VALUE_(7, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_PLL_CR_FINDIV(_x)     VALUE_(0, _x)

// Delay Configuration Register
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR_DLYA1(_x)      VALUE_(20, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR_DLYA0(_x)      VALUE_(15, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR_DLYC(_x)       VALUE_(10, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR_DLYB(_x)       VALUE_(5, _x)
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_DLY_CR_DLYA(_x)       VALUE_(0, _x)

// CCC Status Register
#define CYGHWR_HAL_A2FXXX_SC_MSS_CCC_SR_PLL_LOCK_SYNC      BIT_(0)

// ESRAM control register
#define CYGHWR_HAL_A2FXXX_SC_ESRAM_CR_REMAP                BIT_(0)

// ENVM control register
#define CYGHWR_HAL_A2FXXX_SC_ENVM_CR_SIX_CYCLE             BIT_(7)
#define CYGHWR_HAL_A2FXXX_SC_ENVM_CR_PIPE_BYPASS           BIT_(6)
#define CYGHWR_HAL_A2FXXX_SC_ENVM_CR_REMAP_SIZE(_s_)       VALUE_(0, _s_)

// ENVM re-mapping in system or fabric master space register
#define CYGHWR_HAL_A2FXXX_SC_ENVM_REMAP_BASE(_b_)          VALUE_(1, _b_)
#define CYGHWR_HAL_A2FXXX_SC_ENVM_REMAP_EN                 BIT_(0)

// Fabric protect size
#define CYGHWR_HAL_A2FXXX_SC_FAB_PROT_SIZE_128B            VALUE_(0, 6)
#define CYGHWR_HAL_A2FXXX_SC_FAB_PROT_SIZE_2KB             VALUE_(0, 10)
#define CYGHWR_HAL_A2FXXX_SC_FAB_PROT_SIZE_16KB            VALUE_(0, 13)
#define CYGHWR_HAL_A2FXXX_SC_FAB_PROT_SIZE_32KB            VALUE_(0, 14)
#define CYGHWR_HAL_A2FXXX_SC_FAB_PROT_SIZE_64KB            VALUE_(0, 15)
#define CYGHWR_HAL_A2FXXX_SC_FAB_PROT_SIZE_128KB           VALUE_(0, 16)
#define CYGHWR_HAL_A2FXXX_SC_FAB_PROT_SIZE_256KB           VALUE_(0, 17)
#define CYGHWR_HAL_A2FXXX_SC_FAB_PROT_SIZE_512KB           VALUE_(0, 18)
#define CYGHWR_HAL_A2FXXX_SC_FAB_PROT_SIZE_8MB             VALUE_(0, 22)
#define CYGHWR_HAL_A2FXXX_SC_FAB_PROT_SIZE_128MB           VALUE_(0, 26)
#define CYGHWR_HAL_A2FXXX_SC_FAB_PROT_SIZE_2GB             VALUE_(0, 30)

// Fabric protect control register
#define CYGHWR_HAL_A2FXXX_SC_FAB_PROT_BASE_CR_VAL(_x)      VALUE_( 1, _x)
#define CYGHWR_HAL_A2FXXX_SC_FAB_PROT_BASE_CR_EN           BIT_(0)

// Embedded FlashROM control register
#define CYGHWR_HAL_A2FXXX_SC_EFROM_CR_SYS_TOPT3_1(_v_)     VALUE_(1, _v_)
#define CYGHWR_HAL_A2FXXX_SC_EFROM_CR_SYS_TOPT0            BIT_(0)

//=============================================================================
// DMA
//

#define CYGHWR_HAL_A2FXXX_DMA_RATIO                         0x0
#define CYGHWR_HAL_A2FXXX_DMA_BUFFER_STATUS                 0x4
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL(_x_)                (0x20 + (_x_ * 0x20))
#define CYGHWR_HAL_A2FXXX_DMA_CHx_STATUS(_x_)              (0x24 + (_x_ * 0x20))
#define CYGHWR_HAL_A2FXXX_DMA_CHx_BA_SRC(_x_)              (0x28 + (_x_ * 0x20))
#define CYGHWR_HAL_A2FXXX_DMA_CHx_BA_DST(_x_)              (0x2C + (_x_ * 0x20))
#define CYGHWR_HAL_A2FXXX_DMA_CHx_BA_COUNT(_x_)            (0x30 + (_x_ * 0x20))
#define CYGHWR_HAL_A2FXXX_DMA_CHx_BB_SRC(_x_)              (0x34 + (_x_ * 0x20))
#define CYGHWR_HAL_A2FXXX_DMA_CHx_BB_DST(_x_)              (0x38 + (_x_ * 0x20))
#define CYGHWR_HAL_A2FXXX_DMA_CHx_BB_COUNT(_x_)            (0x3C + (_x_ * 0x20))

// Ratio
#define CYGHWR_HAL_A2FXXX_DMA_RATIO_HILO(_x_)              VALUE_(0, _x_)

// Buffer status
#define CYGHWR_HAL_A2FXXX_DMA_BUFFER_STATUS_CHAxx(_x_)     VALUE_( (_x_ << 1), 1)
#define CYGHWR_HAL_A2FXXX_DMA_BUFFER_STATUS_CHAx(_x_)      VALUE_(((_x_ << 1)+1), 1)

// Channel control
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_PERIPH_SEL(_p_)     VALUE_(23, _p_)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_WR_ADJ(_v_)         VALUE_(14, _v_)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_DST_INCR_4B         VALUE_(12, 0x3)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_DST_INCR_2B         VALUE_(12, 0x2)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_DST_INCR_1B         VALUE_(12, 0x1)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_DST_INCR_0B         VALUE_(12, 0x0)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_DST_INCR(_x_)       VALUE_(12, ((_x_) & 0x3))
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_SRC_INCR_4B         VALUE_(10, 0x3)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_SRC_INCR_2B         VALUE_(10, 0x2)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_SRC_INCR_1B         VALUE_(10, 0x1)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_SRC_INCR_0B         VALUE_(10, 0x0)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_SRC_INCR(_x_)       VALUE_(10, ((_x_) & 0x3))
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_CLR_HI_PRI          BIT_(9)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_CLR_COMPB           BIT_(8)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_CLR_COMPA           BIT_(7)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_INTEN               BIT_(6)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_RESET               BIT_(5)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_PAUSE               BIT_(4)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_XFER_WORD           VALUE_(2, 0x2)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_XFER_HWORD          VALUE_(2, 0x1)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_XFER_BYTE           VALUE_(2, 0x0)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_DIR                 BIT_(1)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_PERIPH              BIT_(0)

// DMA transfer type
#define CYGHWR_HAL_A2FXXX_DMA_XFER_FROM_UART0              0x0
#define CYGHWR_HAL_A2FXXX_DMA_XFER_TO_UART0                0x1
#define CYGHWR_HAL_A2FXXX_DMA_XFER_FROM_UART1              0x2
#define CYGHWR_HAL_A2FXXX_DMA_XFER_TO_UART1                0x3
#define CYGHWR_HAL_A2FXXX_DMA_XFER_FROM_SPI0               0x4
#define CYGHWR_HAL_A2FXXX_DMA_XFER_TO_SPI0                 0x5
#define CYGHWR_HAL_A2FXXX_DMA_XFER_FROM_SPI1               0x6
#define CYGHWR_HAL_A2FXXX_DMA_XFER_TO_SPI1                 0x7
#define CYGHWR_HAL_A2FXXX_DMA_XFER_FROM_FPGA               0x8
#define CYGHWR_HAL_A2FXXX_DMA_XFER_TO_FPGA                 0x9
#define CYGHWR_HAL_A2FXXX_DMA_XFER_FROM_ACE                0x10
#define CYGHWR_HAL_A2FXXX_DMA_XFER_TO_ACE                  0x11
#define CYGHWR_HAL_A2FXXX_DMA_XFER_MEMORY                  0xff

#define CYGHWR_HAL_A2FXXX_DMA_XFER(_x)                     CYGHWR_HAL_A2FXXX_DMA_XFER_##_x

#define CYGHWR_HAL_A2FXXX_DMA_GET_SUB_ID(_x)               ((_x>>2) & 0x01)

// Channel status
#define CYGHWR_HAL_A2FXXX_DMA_CHx_STATUS_BUF_SEL           BIT_(2)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_STATUS_COMPB             BIT_(1)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_STATUS_COMPA             BIT_(0)

// Buffer count
#define CYGHWR_HAL_A2FXXX_DMA_CHx_BA_COUNT_FIELD(_c)       VALUE_(0, _c)
#define CYGHWR_HAL_A2FXXX_DMA_CHx_BB_COUNT_FIELD(_c)       VALUE_(0, _c)

#define CYGHWR_HAL_A2FXXX_DMA_MAX_CHANNEL                  8
#define CYGHWR_HAL_A2FXXX_DMA_MAX_SUB_CHANNEL              2

// DMA access prototypes
void             hal_dma_init ( void );
cyg_uint32       a2fxxx_dma_xfer (cyg_uint8 , cyg_bool , cyg_uint32 len, cyg_uint8* , cyg_uint8* );
cyg_uint32       a2fxxx_dma_ch_setup (cyg_uint8 , cyg_uint8 , cyg_bool , cyg_uint8 , cyg_uint8 , cyg_bool, cyg_uint8);
void             a2fxxx_dma_ch_detach (cyg_uint8 );
void             a2fxxx_dma_update_incr (cyg_uint8 , cyg_bool , cyg_uint8 );
void             a2fxxx_dma_clear_interrupt (cyg_uint8 );
cyg_uint8        a2fxxx_dma_get_comp_flag (cyg_uint8 );

#define CYGHWR_HAL_A2FXXX_DMA_OUTBOUND                     true
#define CYGHWR_HAL_A2FXXX_DMA_INBOUND                      false

//=============================================================================
// ENVM
//
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS                      0x00
#define CYGHWR_HAL_A2FXXX_ENVM_CTRL                        0x04
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ                         0x08
#define CYGHWR_HAL_A2FXXX_ENVM0_CR                         0x10
#define CYGHWR_HAL_A2FXXX_ENVM1_CR                         0x14
#define CYGHWR_HAL_A2FXXX_ENVM0_PAGE_STATUS                0x18
#define CYGHWR_HAL_A2FXXX_ENVM1_PAGE_STATUS                0x1C

// Status Register
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_ILLEGAL_CMD_1        BIT_(31)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_1                    VALUE_(24, 0x3)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_OP_DONE_1            BIT_(23)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_ECC2_ERR_1           BIT_(22)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_ECC1_ERR_1           BIT_(21)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_OVER_THR_1           BIT_(20)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_ERASE_ERR_1          BIT_(19)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_PROG_ERR_1           BIT_(18)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_PROT_ERR_1           BIT_(17)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_BSY_1                BIT_(16)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_ILLEGAL_CMD_0        BIT_(15)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_0                    VALUE_( 8, 0x3)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_OP_DONE_0            BIT_(7)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_ECC2_ERR_0           BIT_(6)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_ECC1_ERR_0           BIT_(5)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_OVER_THR_0           BIT_(4)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_ERASE_ERR_0          BIT_(3)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_PROG_ERR_0           BIT_(2)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_PROT_ERR_0           BIT_(1)
#define CYGHWR_HAL_A2FXXX_ENVM_STATUS_BSY_0                BIT_(0)

// Control Register
#define CYGHWR_HAL_A2FXXX_ENVM_CTRL_COMMAND(_x_)           VALUE_(24, _x_)
#define CYGHWR_HAL_A2FXXX_ENVM_CTRL_PAGE_ADDR(_x_)         VALUE_( 0, _x_)

// Interrupt Enable
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ_ILLEGAL_CMD_1           BIT_(31)
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ_OP_DONE_1               BIT_(23)
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ_ECC2_ERR_1              BIT_(22)
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ_ECC1_ERR_1              BIT_(21)
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ_OVER_THR_1              BIT_(20)
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ_ERASE_ERR_1             BIT_(19)
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ_PROG_ERR_1              BIT_(18)
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ_PROT_ERR_1              BIT_(17)
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ_ILLEGAL_CMD_0           BIT_(15)
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ_OP_DONE_0               BIT_(7)
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ_ECC2_ERR_0              BIT_(6)
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ_ECC1_ERR_0              BIT_(5)
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ_OVER_THR_0              BIT_(4)
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ_ERASE_ERR_0             BIT_(3)
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ_PROG_ERR_0              BIT_(2)
#define CYGHWR_HAL_A2FXXX_ENVM_IRQ_PROT_ERR_0              BIT_(1)

// Configuration Register
#define CYGHWR_HAL_A2FXXX_ENVMx_CR_LOCK                    BIT_(2)
#define CYGHWR_HAL_A2FXXX_ENVMx_CR_PAGE_LOSS               BIT_(1)
#define CYGHWR_HAL_A2FXXX_ENVMx_CR_READ_NEXT               BIT_(0)

// Page status
#define CYGHWR_HAL_A2FXXX_ENVMx_PAGE_STATUS_WCOUNT         VALUE_(8, 0x00ffffffff)
#define CYGHWR_HAL_A2FXXX_ENVMx_PAGE_STATUS_OVER_THR       BIT_(3)
#define CYGHWR_HAL_A2FXXX_ENVMx_PAGE_STATUS_RPROT          BIT_(2)
#define CYGHWR_HAL_A2FXXX_ENVMx_PAGE_STATUS_WPROT          BIT_(1)
#define CYGHWR_HAL_A2FXXX_ENVMx_PAGE_STATUS_OPROT          BIT_(0)


//=============================================================================
// eSRAM
//

#define CYGHWR_HAL_A2FXXX_ESRAM_CR                         0xE0002000
#define CYGHWR_HAL_A2FXXX_AHB_MATRIX_CR                    0xE0002018

#define CYGHWR_HAL_A2FXXX_ESRAM_CR_REMAP                   BIT_(0)


//=============================================================================
// SPI
//
#define CYGHWR_HAL_A2FXXX_SPI_CTRL                         0x00
#define CYGHWR_HAL_A2FXXX_SPI_TXRXDF_SIZE                  0x04
#define CYGHWR_HAL_A2FXXX_SPI_STATUS                       0x08
#define CYGHWR_HAL_A2FXXX_SPI_IRQ_CLEAR                    0x0C
#define CYGHWR_HAL_A2FXXX_SPI_RX                           0x10
#define CYGHWR_HAL_A2FXXX_SPI_TX                           0x14
#define CYGHWR_HAL_A2FXXX_SPI_CLK_GEN                      0x18
#define CYGHWR_HAL_A2FXXX_SPI_SLAVE_SEL                    0x1C
#define CYGHWR_HAL_A2FXXX_SPI_MIS                          0x20
#define CYGHWR_HAL_A2FXXX_SPI_RIS                          0x24

// Control Register
#define CYGHWR_HAL_A2FXXX_SPI_CTRL_SPH                     BIT_(25)
#define CYGHWR_HAL_A2FXXX_SPI_CTRL_SPO                     BIT_(24)
#define CYGHWR_HAL_A2FXXX_SPI_CTRL_COUNT(_x)               VALUE_(8, _x)
#define CYGHWR_HAL_A2FXXX_SPI_CTRL_IRQ_TX_UNRRUN           BIT_(7)
#define CYGHWR_HAL_A2FXXX_SPI_CTRL_IRQ_RX_OVRFLO           BIT_(6)
#define CYGHWR_HAL_A2FXXX_SPI_CTRL_IRQ_TX                  BIT_(5)
#define CYGHWR_HAL_A2FXXX_SPI_CTRL_IRQ_RX                  BIT_(4)
#define CYGHWR_HAL_A2FXXX_SPI_CTRL_PROTO_NS                VALUE_(2, 0x2)
#define CYGHWR_HAL_A2FXXX_SPI_CTRL_PROTO_TI                VALUE_(2, 0x1)
#define CYGHWR_HAL_A2FXXX_SPI_CTRL_PROTO_MOTOROLA          VALUE_(2, 0x0)
#define CYGHWR_HAL_A2FXXX_SPI_CTRL_MASTER                  BIT_(1)
#define CYGHWR_HAL_A2FXXX_SPI_CTRL_EN                      BIT_(0)

// Status Register
#define CYGHWR_HAL_A2FXXX_SPI_STATUS_TX_FIFO_EMPTY_N       BIT_(11)
#define CYGHWR_HAL_A2FXXX_SPI_STATUS_TX_FIFO_EMPTY         BIT_(10)
#define CYGHWR_HAL_A2FXXX_SPI_STATUS_TX_FIFO_FULL_N        BIT_(9)
#define CYGHWR_HAL_A2FXXX_SPI_STATUS_TX_FIFO_FULL          BIT_(8)
#define CYGHWR_HAL_A2FXXX_SPI_STATUS_RX_FIFO_EMPTY_N       BIT_(7)
#define CYGHWR_HAL_A2FXXX_SPI_STATUS_RX_FIFO_EMPTY         BIT_(6)
#define CYGHWR_HAL_A2FXXX_SPI_STATUS_RX_FIFO_FULL_N        BIT_(5)
#define CYGHWR_HAL_A2FXXX_SPI_STATUS_RX_FIFO_FULL          BIT_(4)
#define CYGHWR_HAL_A2FXXX_SPI_STATUS_TX_UNDERRUN           BIT_(3)
#define CYGHWR_HAL_A2FXXX_SPI_STATUS_RX_OVERFLOW           BIT_(2)
#define CYGHWR_HAL_A2FXXX_SPI_STATUS_RX_RCED               BIT_(1)
#define CYGHWR_HAL_A2FXXX_SPI_STATUS_TX_SENT               BIT_(0)

// Interrupt Clear Register
#define CYGHWR_HAL_A2FXXX_SPI_IRQ_CLEAR_TXUNDRUN           BIT_(3)
#define CYGHWR_HAL_A2FXXX_SPI_IRQ_CLEAR_RXOVRFLW           BIT_(2)
#define CYGHWR_HAL_A2FXXX_SPI_IRQ_CLEAR_RXRDY              BIT_(1)
#define CYGHWR_HAL_A2FXXX_SPI_IRQ_CLEAR_TXDONE             BIT_(0)

// Masked Interrupt
#define CYGHWR_HAL_A2FXXX_SPI_MIS_TXUNDRUN                 BIT_(3)
#define CYGHWR_HAL_A2FXXX_SPI_MIS_RXOVRFLW                 BIT_(2)
#define CYGHWR_HAL_A2FXXX_SPI_MIS_RXRDY                    BIT_(1)
#define CYGHWR_HAL_A2FXXX_SPI_MIS_TXDONE                   BIT_(0)

// Raw Interrupt Status
#define CYGHWR_HAL_A2FXXX_SPI_RIS_TXUNDRUN                 BIT_(3)
#define CYGHWR_HAL_A2FXXX_SPI_RIS_RXOVRFLW                 BIT_(2)
#define CYGHWR_HAL_A2FXXX_SPI_RIS_RXRDY                    BIT_(1)
#define CYGHWR_HAL_A2FXXX_SPI_RIS_TXDONE                   BIT_(0)

// Chip select selection
#define CYGHWR_HAL_A2FXXX_SPI_CS_SEL(_dev)                 (0x1 << _dev)

// Peripherals IOs mapping
#define CYGHWR_HAL_A2FXXX_SPI0_DO  CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI0_DO,   DISABLE )
#define CYGHWR_HAL_A2FXXX_SPI0_DI  CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI0_DI,   DISABLE )
#define CYGHWR_HAL_A2FXXX_SPI0_CLK CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI0_CLK,  DISABLE )
#define CYGHWR_HAL_A2FXXX_SPI0_SS0 CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI0_SS_0, DISABLE )
#define CYGHWR_HAL_A2FXXX_SPI0_SS1 CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI0_SS_1, DISABLE )
#define CYGHWR_HAL_A2FXXX_SPI0_SS2 CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI0_SS_2, DISABLE )
#define CYGHWR_HAL_A2FXXX_SPI0_SS3 CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI0_SS_3, DISABLE )

#define CYGHWR_HAL_A2FXXX_SPI1_DO  CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI1_DO,   DISABLE )
#define CYGHWR_HAL_A2FXXX_SPI1_DI  CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI1_DI,   DISABLE )
#define CYGHWR_HAL_A2FXXX_SPI1_CLK CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI1_CLK,  DISABLE )
#define CYGHWR_HAL_A2FXXX_SPI1_SS0 CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI1_SS_0, DISABLE )
#define CYGHWR_HAL_A2FXXX_SPI1_SS1 CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI1_SS_1, DISABLE )
#define CYGHWR_HAL_A2FXXX_SPI1_SS2 CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI1_SS_2, DISABLE )
#define CYGHWR_HAL_A2FXXX_SPI1_SS3 CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI1_SS_3, DISABLE )
#define CYGHWR_HAL_A2FXXX_SPI1_SS4 CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI1_SS_4, DISABLE )
#define CYGHWR_HAL_A2FXXX_SPI1_SS5 CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI1_SS_5, DISABLE )
#define CYGHWR_HAL_A2FXXX_SPI1_SS6 CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI1_SS_6, DISABLE )
#define CYGHWR_HAL_A2FXXX_SPI1_SS7 CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, SPI1_SS_7, DISABLE )

// This is a special GPIO macro accommodated for SPI slave select
#define CYGHWR_HAL_A2FXXX_CS_GPIO(                        \
    __bit,                                                \
    __mode,                                               \
    __p_io,                                               \
    __irq)                                                \
(                                                         \
      (__bit << 23) |                                     \
      (CYGHWR_HAL_A2FXXX_GPIO_MODE_ ## __mode) |          \
       CYGHWR_HAL_A2FXXX_GPIO_PERIPH_GPIO(__p_io) |       \
      (CYGHWR_HAL_A2FXXX_GPIO_IRQ_ ## __irq)              \
)

__externC cyg_uint32 hal_a2fxxx_spi_clock( cyg_uint32 );

//=============================================================================
// I2C
//
#define CYGHWR_HAL_A2FXXX_I2C_CTRL                         0x00
#define CYGHWR_HAL_A2FXXX_I2C_STATUS                       0x04
#define CYGHWR_HAL_A2FXXX_I2C_DATA                         0x08
#define CYGHWR_HAL_A2FXXX_I2C_ADDR                         0x0C
#define CYGHWR_HAL_A2FXXX_I2C_SMBUS                        0x10
#define CYGHWR_HAL_A2FXXX_I2C_FREQ                         0x14
#define CYGHWR_HAL_A2FXXX_I2C_GLITCH                       0x18

// Control Register
#define CYGHWR_HAL_A2FXXX_I2C_CTRL_CR2                     BIT_(7)
#define CYGHWR_HAL_A2FXXX_I2C_CTRL_ENS1                    BIT_(6)
#define CYGHWR_HAL_A2FXXX_I2C_CTRL_STA                     BIT_(5)
#define CYGHWR_HAL_A2FXXX_I2C_CTRL_STO                     BIT_(4)
#define CYGHWR_HAL_A2FXXX_I2C_CTRL_SI                      BIT_(3)
#define CYGHWR_HAL_A2FXXX_I2C_CTRL_AA                      BIT_(2)
#define CYGHWR_HAL_A2FXXX_I2C_CTRL_CR1                     BIT_(1)
#define CYGHWR_HAL_A2FXXX_I2C_CTRL_CR0                     BIT_(0)

// I2C clock divider
#define CYGHWR_HAL_A2FXXX_I2C_DIV_8                        0x7
#define CYGHWR_HAL_A2FXXX_I2C_DIV_60                       0x6
#define CYGHWR_HAL_A2FXXX_I2C_DIV_120                      0x5
#define CYGHWR_HAL_A2FXXX_I2C_DIV_960                      0x4
#define CYGHWR_HAL_A2FXXX_I2C_DIV_160                      0x3
#define CYGHWR_HAL_A2FXXX_I2C_DIV_192                      0x2
#define CYGHWR_HAL_A2FXXX_I2C_DIV_224                      0x1
#define CYGHWR_HAL_A2FXXX_I2C_DIV_256                      0x0

// Status Registers
#define CYGHWR_HAL_A2FXXX_I2C_STATUS_MTX_START             0x08
#define CYGHWR_HAL_A2FXXX_I2C_STATUS_MTX_REPEAT_START      0x10
#define CYGHWR_HAL_A2FXXX_I2C_STATUS_MTX_ADDR_ACK          0x18
#define CYGHWR_HAL_A2FXXX_I2C_STATUS_MTX_ADDR_NACK         0x20
#define CYGHWR_HAL_A2FXXX_I2C_STATUS_MTX_DATA_ACK          0x28
#define CYGHWR_HAL_A2FXXX_I2C_STATUS_MTX_DATA_NACK         0x30
#define CYGHWR_HAL_A2FXXX_I2C_STATUS_MTX_ARBLOST           0x38

#define CYGHWR_HAL_A2FXXX_I2C_STATUS_MRX_START             0x08
#define CYGHWR_HAL_A2FXXX_I2C_STATUS_MRX_REPEAT_START      0x10
#define CYGHWR_HAL_A2FXXX_I2C_STATUS_MRX_ARBLOST           0x38
#define CYGHWR_HAL_A2FXXX_I2C_STATUS_MRX_ADDR_ACK          0x40
#define CYGHWR_HAL_A2FXXX_I2C_STATUS_MRX_ADDR_NACK         0x48
#define CYGHWR_HAL_A2FXXX_I2C_STATUS_MRX_DATA_ACK          0x50
#define CYGHWR_HAL_A2FXXX_I2C_STATUS_MRX_DATA_NACK         0x58

#define CYGHWR_HAL_A2FXXX_I2C_STATUS_MRESET                0xD0

// SMB
#define CYGHWR_HAL_A2FXXX_I2C_SMBUS_RST                    BIT_(7)
#define CYGHWR_HAL_A2FXXX_I2C_SMBUS_NO                     BIT_(6)
#define CYGHWR_HAL_A2FXXX_I2C_SMBUS_NI                     BIT_(5)
#define CYGHWR_HAL_A2FXXX_I2C_SMBALERT_NO                  BIT_(4)
#define CYGHWR_HAL_A2FXXX_I2C_SMBALERT_NI                  BIT_(3)
#define CYGHWR_HAL_A2FXXX_I2C_SMBUS_EN                     BIT_(2)
#define CYGHWR_HAL_A2FXXX_I2C_SMBUS_IRQ_EN                 BIT_(1)
#define CYGHWR_HAL_A2FXXX_I2C_SMBUS_ALERT_IRQ_EN           BIT_(0)

// Peripherals IOs mapping
#define CYGHWR_HAL_A2FXXX_I2C0_SDA CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, I2C0_SDA, DISABLE )
#define CYGHWR_HAL_A2FXXX_I2C0_SCL CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, I2C0_SCL, DISABLE )

#define CYGHWR_HAL_A2FXXX_I2C1_SDA CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, I2C1_SDA, DISABLE )
#define CYGHWR_HAL_A2FXXX_I2C1_SCL CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, I2C1_SCL, DISABLE )

__externC cyg_uint32 hal_a2fxxx_i2c_clock( cyg_uint32 );

//=============================================================================
// GPIO
//
#define CYGHWR_HAL_A2FXXX_MSS_GPIO_CFG(_io)                (_io << 2)
#define CYGHWR_HAL_A2FXXX_MSS_GPIO_INT                     0x080
#define CYGHWR_HAL_A2FXXX_MSS_GPIO_DIN                     0x084
#define CYGHWR_HAL_A2FXXX_MSS_GPIO_DOUT                    0x088

#define CYGHWR_HAL_A2FXXX_MSS_IO_BANK_CR                   0xe0042078
#define CYGHWR_HAL_A2FXXX_GPIN_SOURCE_CR                   0xe004207c
#define CYGHWR_HAL_A2FXXX_IOMUX_CR                         0xe0042100

// CFG
#define CYGHWR_HAL_A2FXXX_GPIO_CFG_INTYPE(_t)              VALUE_(5, (_t & 0x7))
#define CYGHWR_HAL_A2FXXX_GPIO_CFG_GPINTEN                 BIT_(3)
#define CYGHWR_HAL_A2FXXX_GPIO_CFG_OUTBUFEN                BIT_(2)
#define CYGHWR_HAL_A2FXXX_GPIO_CFG_GPINEN                  BIT_(1)
#define CYGHWR_HAL_A2FXXX_GPIO_CFG_GPOUTEN                 BIT_(0)

// MSS GPIO
#define CYGHWR_HAL_A2FXXX_MSS_IO_BANK_LVCMOS_3V3           0x0
#define CYGHWR_HAL_A2FXXX_MSS_IO_BANK_LVCMOS_2V5           0x1
#define CYGHWR_HAL_A2FXXX_MSS_IO_BANK_LVCMOS_1V8           0x2
#define CYGHWR_HAL_A2FXXX_MSS_IO_BANK_LVCMOS_1V5           0x3

#define CYGHWR_HAL_A2FXXX_MSS_IO_BANK_CR_BTEAST(_t)        VALUE_(0, (CYGHWR_HAL_A2FXXX_MSS_IO_BANK_##_t & 0x3))
#define CYGHWR_HAL_A2FXXX_MSS_IO_BANK_CR_BTWEST(_t)        VALUE_(2, (CYGHWR_HAL_A2FXXX_MSS_IO_BANK_##_t & 0x3))

// HAL definitions
#define CYGHWR_HAL_A2FXXX_GPIO_MODE_IN                     VALUE_(0,0)     // Input mode
#define CYGHWR_HAL_A2FXXX_GPIO_MODE_OUT                    VALUE_(0,1)     // Output mode
#define CYGHWR_HAL_A2FXXX_GPIO_MODE_PERIPH                 VALUE_(0,2)     // Peripheral function mode

#define CYGHWR_HAL_A2FXXX_GPIO_PERIPH_IO(_name_)           VALUE_(2, CYGHWR_HAL_A2FXXX_IOMUX_##_name_)
#define CYGHWR_HAL_A2FXXX_GPIO_PERIPH_GPIO(_id_)           VALUE_(2, (CYGHWR_HAL_A2FXXX_IOMUX_GPIO_0+_id_))

#define CYGHWR_HAL_A2FXXX_GPIO_IRQ_DISABLE                 VALUE_(10,0)     // Interrupt disable
#define CYGHWR_HAL_A2FXXX_GPIO_IRQ_FALLING_EDGE            VALUE_(10,1)     // Interrupt on falling edge
#define CYGHWR_HAL_A2FXXX_GPIO_IRQ_RISING_EDGE             VALUE_(10,2)     // Interrupt on rising edge
#define CYGHWR_HAL_A2FXXX_GPIO_IRQ_BOTH_EDGES              VALUE_(10,3)     // Interrupt on both edges
#define CYGHWR_HAL_A2FXXX_GPIO_IRQ_LOW_LEVEL               VALUE_(10,4)     // Interrupt on low level
#define CYGHWR_HAL_A2FXXX_GPIO_IRQ_HIGH_LEVEL              VALUE_(10,5)     // Interrupt on high level

typedef struct
{
   cyg_uint32 val;
} default_io_mux_type;

// IOMUX index setting for peripherals
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI0_DO                    0
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI0_DI                    1
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI0_CLK                   2
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI0_SS_0                  3
#define CYGHWR_HAL_A2FXXX_IOMUX_UART0_TX                   4
#define CYGHWR_HAL_A2FXXX_IOMUX_UART0_RX                   5
#define CYGHWR_HAL_A2FXXX_IOMUX_I2C0_SDA                   6
#define CYGHWR_HAL_A2FXXX_IOMUX_I2C0_SCL                   7
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI1_DO                    8
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI1_DI                    9
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI1_CLK                   10
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI1_SS_0                  11
#define CYGHWR_HAL_A2FXXX_IOMUX_UART1_TX                   12
#define CYGHWR_HAL_A2FXXX_IOMUX_UART1_RX                   13
#define CYGHWR_HAL_A2FXXX_IOMUX_I2C1_SDA                   14
#define CYGHWR_HAL_A2FXXX_IOMUX_I2C1_SCL                   15
#define CYGHWR_HAL_A2FXXX_IOMUX_MAC0_TXD0                  16
#define CYGHWR_HAL_A2FXXX_IOMUX_MAC0_TXD1                  17
#define CYGHWR_HAL_A2FXXX_IOMUX_MAC0_RXD0                  18
#define CYGHWR_HAL_A2FXXX_IOMUX_MAC0_RXD1                  19
#define CYGHWR_HAL_A2FXXX_IOMUX_MAC0_TXEN                  20
#define CYGHWR_HAL_A2FXXX_IOMUX_MAC0_CRSDV                 21
#define CYGHWR_HAL_A2FXXX_IOMUX_MAC0_RXER                  22
#define CYGHWR_HAL_A2FXXX_IOMUX_MAC0_MDIO                  23
#define CYGHWR_HAL_A2FXXX_IOMUX_MAC0_MDC                   24
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_0                     25
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_1                     26
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_2                     27
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_3                     28
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_4                     29
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_5                     30
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_6                     31
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_7                     32
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_8                     33
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_9                     34
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_10                    35
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_11                    36
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_12                    37
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_13                    38
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_14                    39
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_15                    40
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_16                    41
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_17                    42
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_18                    43
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_19                    44
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_20                    45
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_21                    46
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_22                    47
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_23                    48
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_24                    49
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_25                    50
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_26                    51
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_27                    52
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_28                    53
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_29                    54
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_30                    55
#define CYGHWR_HAL_A2FXXX_IOMUX_GPIO_31                    56
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI0_SS_1                  57
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI0_SS_2                  58
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI0_SS_3                  59
// 60 to 63 not used
#define CYGHWR_HAL_A2FXXX_IOMUX_UART0_RTS                  64
#define CYGHWR_HAL_A2FXXX_IOMUX_UART0_DTR                  65
#define CYGHWR_HAL_A2FXXX_IOMUX_UART0_CTS                  66
#define CYGHWR_HAL_A2FXXX_IOMUX_UART0_DSR                  67
#define CYGHWR_HAL_A2FXXX_IOMUX_UART0_RI                   68
#define CYGHWR_HAL_A2FXXX_IOMUX_UART0_DCD                  69
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI1_SS_1                  70
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI1_SS_2                  71
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI1_SS_3                  72
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI1_SS_4                  73
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI1_SS_5                  74
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI1_SS_6                  75
#define CYGHWR_HAL_A2FXXX_IOMUX_SPI1_SS_7                  76
#define CYGHWR_HAL_A2FXXX_IOMUX_UART1_RTS                  77
#define CYGHWR_HAL_A2FXXX_IOMUX_UART1_DTR                  78
#define CYGHWR_HAL_A2FXXX_IOMUX_UART1_CTS                  79
#define CYGHWR_HAL_A2FXXX_IOMUX_UART1_DSR                  80
#define CYGHWR_HAL_A2FXXX_IOMUX_UART1_RI                   81
#define CYGHWR_HAL_A2FXXX_IOMUX_UART1_DCD                  82
#define CYGHWR_HAL_A2FXXX_IOMUX_NONE                       83

// This macro packs the port, bit number, mode, buffer and irq
// for a GPIO pin into a single word.
// The packing puts:

#define CYGHWR_HAL_A2FXXX_GPIO(                             \
    __bit,                                                  \
    __mode,                                                 \
    __name,                                                 \
    __irq)                                                  \
(                                                           \
         (__bit << 23) |                                    \
         (CYGHWR_HAL_A2FXXX_GPIO_MODE_ ## __mode) |         \
          CYGHWR_HAL_A2FXXX_GPIO_PERIPH_IO(__name) |        \
         (CYGHWR_HAL_A2FXXX_GPIO_IRQ_ ## __irq)             \
)


// Macros to extract encoded values
#define CYGHWR_HAL_A2FXXX_GPIO_BIT(__pin)                  (((__pin)>>23) & 0x1F)
#define CYGHWR_HAL_A2FXXX_GPIO_MODE(__pin)                 ((__pin)&0x0003)
#define CYGHWR_HAL_A2FXXX_GPIO_IOMUX_IDX(__pin)            ((__pin>>2)  & 0xff)
#define CYGHWR_HAL_A2FXXX_GPIO_IRQ(__pin)                  ((__pin>>10) & 0x07)
#define CYGHWR_HAL_A2FXXX_GPIO_NONE                        (0xFFFFFFFF)

__externC void hal_a2fxxx_gpio_set( cyg_uint32 pin );
__externC void hal_a2fxxx_gpio_out( cyg_uint32 pin, int val );
__externC void hal_a2fxxx_gpio_in ( cyg_uint32 pin, int *val );

#define CYGHWR_HAL_A2FXXX_GPIO_SET(__pin )        hal_a2fxxx_gpio_set( __pin )
#define CYGHWR_HAL_A2FXXX_GPIO_OUT(__pin, __val ) hal_a2fxxx_gpio_out( __pin, __val )
#define CYGHWR_HAL_A2FXXX_GPIO_IN(__pin,  __val ) hal_a2fxxx_gpio_in( __pin, __val )


//=============================================================================
// UART
//
#define CYGHWR_HAL_A2FXXX_UART16550_RBR                    0x00
#define CYGHWR_HAL_A2FXXX_UART16550_THR                    0x00
#define CYGHWR_HAL_A2FXXX_UART16550_DLR                    0x00
#define CYGHWR_HAL_A2FXXX_UART16550_DMR                    0x04
#define CYGHWR_HAL_A2FXXX_UART16550_IER                    0x04
#define CYGHWR_HAL_A2FXXX_UART16550_IIR                    0x08
#define CYGHWR_HAL_A2FXXX_UART16550_FCR                    0x08
#define CYGHWR_HAL_A2FXXX_UART16550_LCR                    0x0C
#define CYGHWR_HAL_A2FXXX_UART16550_MCR                    0x10
#define CYGHWR_HAL_A2FXXX_UART16550_LSR                    0x14
#define CYGHWR_HAL_A2FXXX_UART16550_MSR                    0x18
#define CYGHWR_HAL_A2FXXX_UART16550_SR                     0x1C

// IER
#define CYGHWR_HAL_A2FXXX_UART16550_IER_EDSSI              BIT_(3)
#define CYGHWR_HAL_A2FXXX_UART16550_IER_ELSI               BIT_(2)
#define CYGHWR_HAL_A2FXXX_UART16550_IER_ETBEI              BIT_(1)
#define CYGHWR_HAL_A2FXXX_UART16550_IER_ERBFI              BIT_(0)

// IIR (by priority)
#define CYGHWR_HAL_A2FXXX_UART16550_IIR_MODEM_STATUS       VALUE_(0, 0x0)
#define CYGHWR_HAL_A2FXXX_UART16550_IIR_THRE               VALUE_(0, 0x2)
#define CYGHWR_HAL_A2FXXX_UART16550_IIR_CTI                VALUE_(0, 0xC)
#define CYGHWR_HAL_A2FXXX_UART16550_IIR_RXD                VALUE_(0, 0x4)
#define CYGHWR_HAL_A2FXXX_UART16550_IIR_RLS                VALUE_(0, 0x6)

// Fifo control
#define CYGHWR_HAL_A2FXXX_UART16550_FCR_RX_TRIG_1BYTE      VALUE_(6, 0x0)
#define CYGHWR_HAL_A2FXXX_UART16550_FCR_RX_TRIG_4BYTE      VALUE_(6, 0x1)
#define CYGHWR_HAL_A2FXXX_UART16550_FCR_RX_TRIG_8BYTE      VALUE_(6, 0x2)
#define CYGHWR_HAL_A2FXXX_UART16550_FCR_RX_TRIG_14BYTE     VALUE_(6, 0x3)
#define CYGHWR_HAL_A2FXXX_UART16550_FCR_EN_TXRX_DY         BIT_(3)
#define CYGHWR_HAL_A2FXXX_UART16550_FCR_CLEAR_TX_FIFO      BIT_(2)
#define CYGHWR_HAL_A2FXXX_UART16550_FCR_CLEAR_RX_FIFO      BIT_(1)
#define CYGHWR_HAL_A2FXXX_UART16550_FCR_RESERVED           BIT_(0)

// Line control
#define CYGHWR_HAL_A2FXXX_UART16550_LCR_DLAB               BIT_(7)
#define CYGHWR_HAL_A2FXXX_UART16550_LCR_SB                 BIT_(6)
#define CYGHWR_HAL_A2FXXX_UART16550_LCR_SP                 BIT_(5)
#define CYGHWR_HAL_A2FXXX_UART16550_LCR_EPS                BIT_(4)
#define CYGHWR_HAL_A2FXXX_UART16550_LCR_PEN                BIT_(3)
#define CYGHWR_HAL_A2FXXX_UART16550_LCR_STOP_1             VALUE_(2,0)
#define CYGHWR_HAL_A2FXXX_UART16550_LCR_STOP_2             VALUE_(2,1)
#define CYGHWR_HAL_A2FXXX_UART16550_LCR_WLS_5BITS          VALUE_(0, 0x0)
#define CYGHWR_HAL_A2FXXX_UART16550_LCR_WLS_6BITS          VALUE_(0, 0x1)
#define CYGHWR_HAL_A2FXXX_UART16550_LCR_WLS_7BITS          VALUE_(0, 0x2)
#define CYGHWR_HAL_A2FXXX_UART16550_LCR_WLS_8BITS          VALUE_(0, 0x3)

// MCR register
#define CYGHWR_HAL_A2FXXX_UART16550_MCR_LOOP               BIT_(4)
#define CYGHWR_HAL_A2FXXX_UART16550_MCR_OUT2               BIT_(3)
#define CYGHWR_HAL_A2FXXX_UART16550_MCR_OUT1               BIT_(2)
#define CYGHWR_HAL_A2FXXX_UART16550_MCR_RTS                BIT_(1)
#define CYGHWR_HAL_A2FXXX_UART16550_MCR_DTR                BIT_(0)

// LSR register
#define CYGHWR_HAL_A2FXXX_UART16550_LSR_FIER               BIT_(6)
#define CYGHWR_HAL_A2FXXX_UART16550_LSR_TEMT               BIT_(6)
#define CYGHWR_HAL_A2FXXX_UART16550_LSR_THRE               BIT_(5)
#define CYGHWR_HAL_A2FXXX_UART16550_LSR_BI                 BIT_(4)
#define CYGHWR_HAL_A2FXXX_UART16550_LSR_FE                 BIT_(3)
#define CYGHWR_HAL_A2FXXX_UART16550_LSR_PE                 BIT_(2)
#define CYGHWR_HAL_A2FXXX_UART16550_LSR_OE                 BIT_(1)
#define CYGHWR_HAL_A2FXXX_UART16550_LSR_DR                 BIT_(0)

// MSR
#define CYGHWR_HAL_A2FXXX_UART16550_MSR_DCD                BIT_(7)
#define CYGHWR_HAL_A2FXXX_UART16550_MSR_RI                 BIT_(6)
#define CYGHWR_HAL_A2FXXX_UART16550_MSR_DSR                BIT_(5)
#define CYGHWR_HAL_A2FXXX_UART16550_MSR_CTS                BIT_(4)
#define CYGHWR_HAL_A2FXXX_UART16550_MSR_DDCD               BIT_(3)
#define CYGHWR_HAL_A2FXXX_UART16550_MSR_TERI               BIT_(2)
#define CYGHWR_HAL_A2FXXX_UART16550_MSR_DDSR               BIT_(1)
#define CYGHWR_HAL_A2FXXX_UART16550_MSR_DCTS               BIT_(0)

__externC cyg_uint32 hal_a2fxxx_pclk0;
__externC cyg_uint32 hal_a2fxxx_pclk1;

__externC void hal_a2fxxx_uart_setbaud(cyg_uint32 , cyg_uint32 );

// Consider adding rounding ?
#define CYG_HAL_CORTEXM_A2FXXX_BAUD_GENERATOR( id, baud )  \
({                                                         \
   cyg_uint16 _divider;                                    \
   if( id == 0 )                                           \
     _divider = (hal_a2fxxx_pclk0/(baud << 4));            \
   else                                                    \
     _divider = (hal_a2fxxx_pclk1/(baud << 4));            \
   _divider;                                               \
})

// Peripherals IOs mapping
#define CYGHWR_HAL_A2FXXX_UART0_TX CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, UART0_TX, DISABLE )
#define CYGHWR_HAL_A2FXXX_UART0_RX CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, UART0_RX, DISABLE )

#define CYGHWR_HAL_A2FXXX_UART1_TX CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, UART1_TX, DISABLE )
#define CYGHWR_HAL_A2FXXX_UART1_RX CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, UART1_RX, DISABLE )


//=============================================================================
// Watchdog
//
#define CYGHWR_HAL_A2FXXX_WD_VALUE                         0x00
#define CYGHWR_HAL_A2FXXX_WD_LOAD                          0x04
#define CYGHWR_HAL_A2FXXX_WD_MVRP                          0x08
#define CYGHWR_HAL_A2FXXX_WD_REFRESH                       0x0C
#define CYGHWR_HAL_A2FXXX_WD_ENABLE                        0x10
#define CYGHWR_HAL_A2FXXX_WD_CTRL                          0x14
#define CYGHWR_HAL_A2FXXX_WD_STATUS                        0x18
#define CYGHWR_HAL_A2FXXX_WD_RIS                           0x1C
#define CYGHWR_HAL_A2FXXX_WD_MIS                           0x20

// Keys
#define CYGHWR_HAL_A2FXXX_WD_REFRESH_KEY                   0xAC15DE42
#define CYGHWR_HAL_A2FXXX_WD_DISABLE_KEY                   0x4C6E55FA


//=============================================================================
// Timer
//
#define CYGHWR_HAL_A2FXXX_TIMER1   CYGHWR_HAL_A2FXXX_TIMER
#define CYGHWR_HAL_A2FXXX_TIMER2   (CYGHWR_HAL_A2FXXX_TIMER + 0x18)

#define CYGHWR_HAL_A2FXXX_TIMER_TIMx_VAL                   0x00
#define CYGHWR_HAL_A2FXXX_TIMER_TIMx_LOADVAL               0x04
#define CYGHWR_HAL_A2FXXX_TIMER_TIMx_BGLOADVAL             0x08
#define CYGHWR_HAL_A2FXXX_TIMER_TIMx_CTRL                  0x0c
#define CYGHWR_HAL_A2FXXX_TIMER_TIMx_RIS                   0x10
#define CYGHWR_HAL_A2FXXX_TIMER_TIMx_MIS                   0x14
#define CYGHWR_HAL_A2FXXX_TIMER_TIM64_VAL_U                0x30
#define CYGHWR_HAL_A2FXXX_TIMER_TIM64_VAL_L                0x34
#define CYGHWR_HAL_A2FXXX_TIMER_TIM64_LOADVAL_U            0x38
#define CYGHWR_HAL_A2FXXX_TIMER_TIM64_LOADVAL_L            0x3C
#define CYGHWR_HAL_A2FXXX_TIMER_TIM64_BGLOADVAL_U          0x40
#define CYGHWR_HAL_A2FXXX_TIMER_TIM64_BGLOADVAL_L          0x44
#define CYGHWR_HAL_A2FXXX_TIMER_TIM64_CTRL                 0x48
#define CYGHWR_HAL_A2FXXX_TIMER_TIM64_RIS                  0x4C
#define CYGHWR_HAL_A2FXXX_TIMER_TIM64_MIS                  0x50
#define CYGHWR_HAL_A2FXXX_TIMER_TIM64_MODE                 0x54

// Timer control
#define CYGHWR_HAL_A2FXXX_TIMER_TIMx_CTRL_INTEN            BIT_(2)
#define CYGHWR_HAL_A2FXXX_TIMER_TIMx_CTRL_ONESHOT          BIT_(1)
#define CYGHWR_HAL_A2FXXX_TIMER_TIMx_CTRL_EN               BIT_(0)

// Timer control 64 bits
#define CYGHWR_HAL_A2FXXX_TIMER_TIM64_MODE_64BITS          BIT_(0)


//=============================================================================
// MAC
//
#if defined(CYGHWR_HAL_CORTEXM_A2FXXX_A2F200) || \
           defined(CYGHWR_HAL_CORTEXM_A2FXXX_A2F500)

#define CYGHWR_HAL_A2FXXX_MAC_CSR0                         0x00
#define CYGHWR_HAL_A2FXXX_MAC_CSR1                         0x08
#define CYGHWR_HAL_A2FXXX_MAC_CSR2                         0x10
#define CYGHWR_HAL_A2FXXX_MAC_CSR3                         0x18
#define CYGHWR_HAL_A2FXXX_MAC_CSR4                         0x20
#define CYGHWR_HAL_A2FXXX_MAC_CSR5                         0x28
#define CYGHWR_HAL_A2FXXX_MAC_CSR6                         0x30
#define CYGHWR_HAL_A2FXXX_MAC_CSR7                         0x38
#define CYGHWR_HAL_A2FXXX_MAC_CSR8                         0x40
#define CYGHWR_HAL_A2FXXX_MAC_CSR9                         0x48
#define CYGHWR_HAL_A2FXXX_MAC_CSR10                        0x50
#define CYGHWR_HAL_A2FXXX_MAC_CSR11                        0x58

// Bus Mode Register (CSR0)
#define CYGHWR_HAL_A2FXXX_MAC_CSR0_CLEAR                   0xFE000000
#define CYGHWR_HAL_A2FXXX_MAC_CSR0_SPD                     BIT_(21)
#define CYGHWR_HAL_A2FXXX_MAC_CSR0_DBO                     BIT_(20)
#define CYGHWR_HAL_A2FXXX_MAC_CSR0_TAP(_x)                 VALUE_(17, (_x & 0x7))
#define CYGHWR_HAL_A2FXXX_MAC_CSR0_PBL(_x)                 VALUE_(8, (_x & 0x2f))
#define CYGHWR_HAL_A2FXXX_MAC_CSR0_BLE                     BIT_(7)
#define CYGHWR_HAL_A2FXXX_MAC_CSR0_DSL(_x)                 VALUE_(2, (_x & 0x1f))
#define CYGHWR_HAL_A2FXXX_MAC_CSR0_BAR                     BIT_(1)
#define CYGHWR_HAL_A2FXXX_MAC_CSR0_SWR                     BIT_(0)

// Transmit Poll Demand Register (CSR1)
#define CYGHWR_HAL_A2FXXX_MAC_CSR1_TPD(_x)                 VALUE_(0, _x)

// Receive Poll Demand Register (CSR2)
#define CYGHWR_HAL_A2FXXX_MAC_CSR2_RPD(_x)                 VALUE_(0, _x)

// Receive Descriptor List Base Address Register (CSR3)
#define CYGHWR_HAL_A2FXXX_MAC_CSR3_RLA(_x)                 VALUE_(0, _x)

// Transmit Descriptor List Base Address Register (CSR4)
#define CYGHWR_HAL_A2FXXX_MAC_CSR4_TLA(_x)                 VALUE_(0, _x)

// Status and Control Register (CSR5)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_TS_CLOSE_TXD            VALUE_(20, 0x7)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_TS_SUSPEND              VALUE_(20, 0x6)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_TS_SETUP_PACKET         VALUE_(20, 0x5)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_TS_XFER                 VALUE_(20, 0x3)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_TS_WAIT_EOT             VALUE_(20, 0x2)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_TS_FETCH_TXD            VALUE_(20, 0x1)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_TS_STOP                 VALUE_(20, 0x0)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_TS_MASK                 VALUE_(20, 0x7)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_RS_XFER                 VALUE_(17, 0x7)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_RS_CLOSE_RXD            VALUE_(17, 0x5)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_RS_SUSPEND              VALUE_(17, 0x4)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_RS_WAIT_PACKET          VALUE_(17, 0x3)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_RS_WAIT_EOR             VALUE_(17, 0x2)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_RS_FETCH_RXD            VALUE_(17, 0x1)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_RS_STOP                 VALUE_(17, 0x0)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_RS_MASK                 VALUE_(17, 0x7)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_NIS                     BIT_(16)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_AIS                     BIT_(15)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_ERI                     BIT_(14)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_GTE                     BIT_(11)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_ETI                     BIT_(10)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_RPS                     BIT_(8)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_RU                      BIT_(7)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_RI                      BIT_(6)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_UNF                     BIT_(5)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_TU                      BIT_(2)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_TPS                     BIT_(1)
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_TI                      BIT_(0)

// Define MAC state
#define A2FXXX_MAC_TX_STATE(_x_)   CYGHWR_HAL_A2FXXX_MAC_CSR5_TS_##_x_
#define A2FXXX_MAC_RX_STATE(_x_)   CYGHWR_HAL_A2FXXX_MAC_CSR5_RS_##_x_
#define A2FXXX_MAC_TX_STATE_MASK                           VALUE_(20, 0x7)
#define A2FXXX_MAC_RX_STATE_MASK                           VALUE_(17, 0x7)

// Operation Mode Register (CSR6)
#define CYGHWR_HAL_A2FXXX_MAC_CSR6_CLEAR                   0xBF9F1D20
#define CYGHWR_HAL_A2FXXX_MAC_CSR6_RA                      BIT_(30)
#define CYGHWR_HAL_A2FXXX_MAC_CSR6_TTM                     BIT_(22)
#define CYGHWR_HAL_A2FXXX_MAC_CSR6_SF                      BIT_(21)
#define CYGHWR_HAL_A2FXXX_MAC_CSR6_TR(_x)                  VALUE_(14, _x)
#define CYGHWR_HAL_A2FXXX_MAC_CSR6_ST                      BIT_(13)
#define CYGHWR_HAL_A2FXXX_MAC_CSR6_FD                      BIT_(9)
#define CYGHWR_HAL_A2FXXX_MAC_CSR6_PM                      BIT_(7)
#define CYGHWR_HAL_A2FXXX_MAC_CSR6_PR                      BIT_(6)
#define CYGHWR_HAL_A2FXXX_MAC_CSR6_IF                      BIT_(4)
#define CYGHWR_HAL_A2FXXX_MAC_CSR6_PB                      BIT_(3)
#define CYGHWR_HAL_A2FXXX_MAC_CSR6_HO                      BIT_(2)
#define CYGHWR_HAL_A2FXXX_MAC_CSR6_SR                      BIT_(1)
#define CYGHWR_HAL_A2FXXX_MAC_CSR6_HP                      BIT_(0)

// Interrupt Enable Register (CSR7)
#define CYGHWR_HAL_A2FXXX_MAC_CSR7_NIE                     BIT_(16)
#define CYGHWR_HAL_A2FXXX_MAC_CSR7_AIE                     BIT_(15)
#define CYGHWR_HAL_A2FXXX_MAC_CSR7_ERE                     BIT_(14)
#define CYGHWR_HAL_A2FXXX_MAC_CSR7_GTE                     BIT_(11)
#define CYGHWR_HAL_A2FXXX_MAC_CSR7_ETE                     BIT_(10)
#define CYGHWR_HAL_A2FXXX_MAC_CSR7_RSE                     BIT_(8)
#define CYGHWR_HAL_A2FXXX_MAC_CSR7_RUE                     BIT_(7)
#define CYGHWR_HAL_A2FXXX_MAC_CSR7_RIE                     BIT_(6)
#define CYGHWR_HAL_A2FXXX_MAC_CSR7_UNE                     BIT_(5)
#define CYGHWR_HAL_A2FXXX_MAC_CSR7_TUE                     BIT_(2)
#define CYGHWR_HAL_A2FXXX_MAC_CSR7_TSE                     BIT_(1)
#define CYGHWR_HAL_A2FXXX_MAC_CSR7_TIE                     BIT_(0)

// Missed Frames and Overflow Counter Register (CSR8)
#define CYGHWR_HAL_A2FXXX_MAC_CSR8_OCO                     BIT_(28)
#define CYGHWR_HAL_A2FXXX_MAC_CSR8_MFO                     BIT_(16)

// RMII Management Interface Register (CSR9)
#define CYGHWR_HAL_A2FXXX_MAC_CSR9_MDI                     BIT_(19)
#define CYGHWR_HAL_A2FXXX_MAC_CSR9_MDEN                    BIT_(18)
#define CYGHWR_HAL_A2FXXX_MAC_CSR9_MDO                     BIT_(17)
#define CYGHWR_HAL_A2FXXX_MAC_CSR9_MDC                     BIT_(16)

// General-Purpose Timer and Interrupt Mitigation Control Register (CSR11)
#define CYGHWR_HAL_A2FXXX_MAC_CSR11_CS                     BIT_(31)
#define CYGHWR_HAL_A2FXXX_MAC_CSR11_TT(_x)                 VALUE_(27, (_x & 0xf))
#define CYGHWR_HAL_A2FXXX_MAC_CSR11_NTP(_x)                VALUE_(24, (_x & 0x7))
#define CYGHWR_HAL_A2FXXX_MAC_CSR11_RT(_x)                 VALUE_(20, (_x & 0xf))
#define CYGHWR_HAL_A2FXXX_MAC_CSR11_NRP(_x)                VALUE_(17, (_x & 0x7))
#define CYGHWR_HAL_A2FXXX_MAC_CSR11_CON                    BIT_(16)
#define CYGHWR_HAL_A2FXXX_MAC_CSR11_TIM(_x)                VALUE_(0, _x)

// Receive Descriptors (RDESx)
#define CYGHWR_HAL_A2FXXX_MAC_RDES0_OWN                    BIT_(31)
#define CYGHWR_HAL_A2FXXX_MAC_RDES0_FF                     BIT_(30)
#define CYGHWR_HAL_A2FXXX_MAC_RDES0_FL(_x)                 VALUE_(16, _x)
#define CYGHWR_HAL_A2FXXX_MAC_RDES0_ES                     BIT_(15)
#define CYGHWR_HAL_A2FXXX_MAC_RDES0_DE                     BIT_(14)
#define CYGHWR_HAL_A2FXXX_MAC_RDES0_RF                     BIT_(11)
#define CYGHWR_HAL_A2FXXX_MAC_RDES0_MF                     BIT_(10)
#define CYGHWR_HAL_A2FXXX_MAC_RDES0_FS                     BIT_(9)
#define CYGHWR_HAL_A2FXXX_MAC_RDES0_LS                     BIT_(8)
#define CYGHWR_HAL_A2FXXX_MAC_RDES0_TL                     BIT_(7)
#define CYGHWR_HAL_A2FXXX_MAC_RDES0_CS                     BIT_(6)
#define CYGHWR_HAL_A2FXXX_MAC_RDES0_FT                     BIT_(5)
#define CYGHWR_HAL_A2FXXX_MAC_RDES0_RE                     BIT_(3)
#define CYGHWR_HAL_A2FXXX_MAC_RDES0_DB                     BIT_(2)
#define CYGHWR_HAL_A2FXXX_MAC_RDES0_CE                     BIT_(1)
#define CYGHWR_HAL_A2FXXX_MAC_RDES0_ZERO                   BIT_(0)

#define CYGHWR_HAL_A2FXXX_MAC_RDES1_RER                    BIT_(25)
#define CYGHWR_HAL_A2FXXX_MAC_RDES1_RCH                    BIT_(24)
#define CYGHWR_HAL_A2FXXX_MAC_RDES1_RBS2(_x)               VALUE_(11, _x)
#define CYGHWR_HAL_A2FXXX_MAC_RDES1_RBS1(_x)               VALUE_(0, _x)

// Receive Descriptors (TDESx)
#define CYGHWR_HAL_A2FXXX_MAC_TDES0_OWN                    BIT_(31)
#define CYGHWR_HAL_A2FXXX_MAC_TDES0_ES                     BIT_(15)
#define CYGHWR_HAL_A2FXXX_MAC_TDES0_LO                     BIT_(11)
#define CYGHWR_HAL_A2FXXX_MAC_TDES0_NC                     BIT_(10)
#define CYGHWR_HAL_A2FXXX_MAC_TDES0_LC                     BIT_(9)
#define CYGHWR_HAL_A2FXXX_MAC_TDES0_EC                     BIT_(8)
#define CYGHWR_HAL_A2FXXX_MAC_TDES0_CC(_x)                 VALUE_(3, _x)
#define CYGHWR_HAL_A2FXXX_MAC_TDES0_UF                     BIT_(1)
#define CYGHWR_HAL_A2FXXX_MAC_TDES0_DE                     BIT_(0)

#define CYGHWR_HAL_A2FXXX_MAC_TDES1_IC                     BIT_(31)
#define CYGHWR_HAL_A2FXXX_MAC_TDES1_LS                     BIT_(30)
#define CYGHWR_HAL_A2FXXX_MAC_TDES1_FS                     BIT_(29)
#define CYGHWR_HAL_A2FXXX_MAC_TDES1_FT1                    BIT_(28)
#define CYGHWR_HAL_A2FXXX_MAC_TDES1_SET                    BIT_(27)
#define CYGHWR_HAL_A2FXXX_MAC_TDES1_AC                     BIT_(26)
#define CYGHWR_HAL_A2FXXX_MAC_TDES1_TER                    BIT_(25)
#define CYGHWR_HAL_A2FXXX_MAC_TDES1_TCH                    BIT_(24)
#define CYGHWR_HAL_A2FXXX_MAC_TDES1_DPD                    BIT_(23)
#define CYGHWR_HAL_A2FXXX_MAC_TDES1_FT0                    BIT_(22)
#define CYGHWR_HAL_A2FXXX_MAC_TDES1_TBS2(_x)               VALUE_(11, _x)
#define CYGHWR_HAL_A2FXXX_MAC_TDES1_TBS1(_x)               VALUE_(0, _x)

#define CYGHWR_HAL_A2FXXX_MAC0_MDIO   CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, MAC0_MDIO,  DISABLE )
#define CYGHWR_HAL_A2FXXX_MAC0_MDC    CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, MAC0_MDC,   DISABLE )
#define CYGHWR_HAL_A2FXXX_MAC0_TXD0   CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, MAC0_TXD0,  DISABLE )
#define CYGHWR_HAL_A2FXXX_MAC0_TXD1   CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, MAC0_TXD1,  DISABLE )
#define CYGHWR_HAL_A2FXXX_MAC0_RXD0   CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, MAC0_RXD0,  DISABLE )
#define CYGHWR_HAL_A2FXXX_MAC0_RXD1   CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, MAC0_RXD1,  DISABLE )
#define CYGHWR_HAL_A2FXXX_MAC0_TXEN   CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, MAC0_TXEN,  DISABLE )
#define CYGHWR_HAL_A2FXXX_MAC0_CRSDV  CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, MAC0_CRSDV, DISABLE )
#define CYGHWR_HAL_A2FXXX_MAC0_RXER   CYGHWR_HAL_A2FXXX_GPIO( 0, PERIPH, MAC0_RXER,  DISABLE )

#endif

//=============================================================================
// Spare page

#define CYGHWR_HAL_A2FXXX_ENVM_SPARE_PAGES_SYSBOOT_KEY       0x081C
#define CYGHWR_HAL_A2FXXX_ENVM_SPARE_PAGES_SYSBOOT_VERSION   0x0840
#define CYGHWR_HAL_A2FXXX_ENVM_SPARE_PAGES_SYSBOOT_1_3_FCLK  0x162C
#define CYGHWR_HAL_A2FXXX_ENVM_SPARE_PAGES_SYSBOOT_2_x_FCLK  0x1EAC

// System boot key value
#define CYGHWR_HAL_A2FXXX_ENVM_SPARE_PAGES_SYSBOOT_KEY_VALUE 0x4C544341

// System boot version
#define CYGHWR_HAL_A2FXXX_SYSBOOT_VERSION(_x, _y, _z)        ((_x << 16) | (_y << 8) | (_z))

#define CYGHWR_HAL_A2FXXX_GET_SYSBOOT_VERSION()                                      \
({                                                                                   \
   cyg_uint32 _i;                                                                    \
   cyg_uint32 base = CYGHWR_HAL_A2FXXX_ENVM_SPARE_PAGES;                             \
   HAL_READ_UINT32( base + CYGHWR_HAL_A2FXXX_ENVM_SPARE_PAGES_SYSBOOT_VERSION, _i);  \
   _i;                                                                               \
})

#define CYGHWR_HAL_A2FXXX_GET_SYSBOOT_KEY()                                          \
({                                                                                   \
   cyg_uint32 _i;                                                                    \
   cyg_uint32 base = CYGHWR_HAL_A2FXXX_ENVM_SPARE_PAGES;                             \
   HAL_READ_UINT32( base + CYGHWR_HAL_A2FXXX_ENVM_SPARE_PAGES_SYSBOOT_KEY, _i);      \
   _i;                                                                               \
})

#define CYGHWR_HAL_A2FXXX_GET_SYSBOOT_1_3_FCLK()                                     \
({                                                                                   \
   cyg_uint32 _i;                                                                    \
   cyg_uint32 base = CYGHWR_HAL_A2FXXX_ENVM_SPARE_PAGES;                             \
   HAL_READ_UINT32( base + CYGHWR_HAL_A2FXXX_ENVM_SPARE_PAGES_SYSBOOT_1_3_FCLK, _i); \
   _i;                                                                               \
})

#define CYGHWR_HAL_A2FXXX_GET_SYSBOOT_2_0_FCLK()                                     \
({                                                                                   \
   cyg_uint32 _i;                                                                    \
   cyg_uint32 base = CYGHWR_HAL_A2FXXX_ENVM_SPARE_PAGES;                             \
   HAL_READ_UINT32( base + CYGHWR_HAL_A2FXXX_ENVM_SPARE_PAGES_SYSBOOT_2_x_FCLK, _i); \
   _i;                                                                               \
})

#define CYGHWR_HAL_A2FXXX_BITSET                           0x1
#define CYGHWR_HAL_A2FXXX_BITCLEAR                         0x0
//
//-----------------------------------------------------------------------------
// end of var_io.h
#endif // CYGONCE_HAL_VAR_IO_H
