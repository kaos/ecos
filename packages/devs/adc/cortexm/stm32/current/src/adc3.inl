//==========================================================================
//
//      adc3.inl
//
//      Parameters for ADC device 3
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
static const cyg_uint32 stm32_adc_pins3[] = {
    CYGHWR_HAL_STM32_ADC3_IN0,
    CYGHWR_HAL_STM32_ADC3_IN1,
    CYGHWR_HAL_STM32_ADC3_IN2,
    CYGHWR_HAL_STM32_ADC3_IN3,
    CYGHWR_HAL_STM32_ADC3_IN4,
    CYGHWR_HAL_STM32_ADC3_IN5,
    CYGHWR_HAL_STM32_ADC3_IN6,
    CYGHWR_HAL_STM32_ADC3_IN7,
    CYGHWR_HAL_STM32_ADC3_IN8,
    CYGHWR_HAL_STM32_ADC3_IN9,
    CYGHWR_HAL_STM32_ADC3_IN10,
    CYGHWR_HAL_STM32_ADC3_IN11,
    CYGHWR_HAL_STM32_ADC3_IN12,
    CYGHWR_HAL_STM32_ADC3_IN13,
    CYGHWR_HAL_STM32_ADC3_IN14,
    CYGHWR_HAL_STM32_ADC3_IN15,
};

// ADC setup
static const stm32_adc_setup stm32_adc_setup3 = {
    .adc_base       = CYGHWR_HAL_STM32_ADC3,
    .dma_base       = CYGHWR_HAL_STM32_DMA2,
    .dma_int_vector = CYGNUM_HAL_INTERRUPT_DMA2_CH4_5,
    .dma_int_pri    = 0x80,
    .dma_channel    = 5,
    .tim_base       = CYGHWR_HAL_STM32_TIM8,
    .pins           = stm32_adc_pins3,
    .extsel         = 4,
    .sample_time    = CYGNUM_DEVS_ADC_CORTEXM_STM32_ADC3_SAMPLE_TIME,
};

// ADC DMA buffer
static cyg_uint16
    stm32_adc_dma_buf3[CYGINT_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNELS]
    __attribute__((aligned(2), section(".sram")));

// ADC device info
static stm32_adc_info stm32_adc_info3 = {
    .setup          = &stm32_adc_setup3,
    .dma_buf        = stm32_adc_dma_buf3,
};

// ADC device instance
CYG_ADC_DEVICE(stm32_adc_device3,
               &stm32_adc_funs,
               &stm32_adc_info3,
               CYGNUM_DEVS_ADC_CORTEXM_STM32_ADC3_DEFAULT_RATE);

// ADC channels
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNEL0
STM32_ADC_CHANNEL(3, 0)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNEL1
STM32_ADC_CHANNEL(3, 1)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNEL2
STM32_ADC_CHANNEL(3, 2)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNEL3
STM32_ADC_CHANNEL(3, 3)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNEL4
STM32_ADC_CHANNEL(3, 4)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNEL5
STM32_ADC_CHANNEL(3, 5)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNEL6
STM32_ADC_CHANNEL(3, 6)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNEL7
STM32_ADC_CHANNEL(3, 7)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNEL8
STM32_ADC_CHANNEL(3, 8)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNEL9
STM32_ADC_CHANNEL(3, 9)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNEL10
STM32_ADC_CHANNEL(3, 10)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNEL11
STM32_ADC_CHANNEL(3, 11)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNEL12
STM32_ADC_CHANNEL(3, 12)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNEL13
STM32_ADC_CHANNEL(3, 13)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNEL14
STM32_ADC_CHANNEL(3, 14)
#endif
#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3_CHANNEL15
STM32_ADC_CHANNEL(3, 15)
#endif

//-----------------------------------------------------------------------------
// End of adc3.inl
