//==========================================================================
//
//      adc1.inl
//
//      Parameters for ADC device 1
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2009 Free Software Foundation, Inc.                        
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
// Author(s):    Simon Kallweit <simon.kallweit@intefo.ch>
// Contributors:
// Date:         2009-02-24
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

// ADC input pins
static const cyg_uint32 stm32_adc_pins1[] = {
    CYGHWR_HAL_STM32_ADC1_IN0,
    CYGHWR_HAL_STM32_ADC1_IN1,
    CYGHWR_HAL_STM32_ADC1_IN2,
    CYGHWR_HAL_STM32_ADC1_IN3,
    CYGHWR_HAL_STM32_ADC1_IN4,
    CYGHWR_HAL_STM32_ADC1_IN5,
    CYGHWR_HAL_STM32_ADC1_IN6,
    CYGHWR_HAL_STM32_ADC1_IN7,
    CYGHWR_HAL_STM32_ADC1_IN8,
    CYGHWR_HAL_STM32_ADC1_IN9,
    CYGHWR_HAL_STM32_ADC1_IN10,
    CYGHWR_HAL_STM32_ADC1_IN11,
    CYGHWR_HAL_STM32_ADC1_IN12,
    CYGHWR_HAL_STM32_ADC1_IN13,
    CYGHWR_HAL_STM32_ADC1_IN14,
    CYGHWR_HAL_STM32_ADC1_IN15,
    CYGHWR_HAL_STM32_GPIO_NONE,
    CYGHWR_HAL_STM32_GPIO_NONE,
};

// ADC setup
static const stm32_adc_setup stm32_adc_setup1 = {
    .adc_base       = CYGHWR_HAL_STM32_ADC1,
    .dma_base       = CYGHWR_HAL_STM32_DMA1,
    .dma_int_vector = CYGNUM_HAL_INTERRUPT_DMA1_CH1,
    .dma_int_pri    = CYGNUM_DEVS_ADC_CORTEXM_STM32_ADC1_DMA_INT_PRI,
    .dma_channel    = 1,
    .tim_base       = CYGHWR_HAL_STM32_TIM3,
    .pins           = stm32_adc_pins1,
    .extsel         = 4,
    .sample_time    = CYGNUM_DEVS_ADC_CORTEXM_STM32_ADC1_SAMPLE_TIME,
    .adc_clkena     = CYGHWR_HAL_STM32_ADC1_CLOCK,
    .tim_clkena     = CYGHWR_HAL_STM32_TIM3_CLOCK,
};

// ADC DMA buffer
static cyg_uint16
    stm32_adc_dma_buf1[CYGINT_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNELS]
    __attribute__((aligned(2), section(".sram")));

// ADC device info
static stm32_adc_info stm32_adc_info1 = {
    .setup          = &stm32_adc_setup1,
    .dma_buf        = stm32_adc_dma_buf1,
};

// ADC device instance
CYG_ADC_DEVICE(stm32_adc_device1,
               &stm32_adc_funs,
               &stm32_adc_info1,
               CYGNUM_DEVS_ADC_CORTEXM_STM32_ADC1_DEFAULT_RATE);

// ADC channels
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL0
STM32_ADC_CHANNEL(1, 0)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL1
STM32_ADC_CHANNEL(1, 1)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL2
STM32_ADC_CHANNEL(1, 2)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL3
STM32_ADC_CHANNEL(1, 3)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL4
STM32_ADC_CHANNEL(1, 4)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL5
STM32_ADC_CHANNEL(1, 5)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL6
STM32_ADC_CHANNEL(1, 6)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL7
STM32_ADC_CHANNEL(1, 7)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL8
STM32_ADC_CHANNEL(1, 8)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL9
STM32_ADC_CHANNEL(1, 9)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL10
STM32_ADC_CHANNEL(1, 10)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL11
STM32_ADC_CHANNEL(1, 11)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL12
STM32_ADC_CHANNEL(1, 12)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL13
STM32_ADC_CHANNEL(1, 13)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL14
STM32_ADC_CHANNEL(1, 14)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL15
STM32_ADC_CHANNEL(1, 15)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL16
STM32_ADC_CHANNEL(1, 16)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1_CHANNEL17
STM32_ADC_CHANNEL(1, 17)
#endif

//-----------------------------------------------------------------------------
// End of adc1.inl
