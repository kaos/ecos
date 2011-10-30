#ifndef CYGONCE_HAL_VAR_INTR_H
#define CYGONCE_HAL_VAR_INTR_H
//==========================================================================
//
//      var_intr.h
//
//      HAL Interrupt and clock assignments for Kinetis variants
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
// Author(s):     ilijak
// Date:          2011-02-05
// Purpose:       Define Interrupt support
// Description:   The interrupt specifics for Freescale Kinetis variants are
//                defined here.
//
// Usage:         #include <cyg/hal/var_intr.h>
//                However applications should include using <cyg/hal/hal_intr.h>
//                instead to allow for platform overrides.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/hal/plf_intr.h>

//==========================================================================

typedef enum {
    CYGNUM_HAL_INTERRUPT_DMA0
        = CYGNUM_HAL_INTERRUPT_EXTERNAL,  // DMA Channel 0 Transfer Complete
    CYGNUM_HAL_INTERRUPT_DMA1,            // DMA Channel 1 Transfer Complete
    CYGNUM_HAL_INTERRUPT_DMA2,            // DMA Channel 2 Transfer Complete
    CYGNUM_HAL_INTERRUPT_DMA3,            // DMA Channel 3 Transfer Complete
    CYGNUM_HAL_INTERRUPT_DMA4,            // DMA Channel 4 Transfer Complete
    CYGNUM_HAL_INTERRUPT_DMA5,            // DMA Channel 5 Transfer Complete
    CYGNUM_HAL_INTERRUPT_DMA6,            // DMA Channel 6 Transfer Complete
    CYGNUM_HAL_INTERRUPT_DMA7,            // DMA Channel 7 Transfer Complete
    CYGNUM_HAL_INTERRUPT_DMA8,            // DMA Channel 8 Transfer Complete
    CYGNUM_HAL_INTERRUPT_DMA9,            // DMA Channel 9 Transfer Complete
    CYGNUM_HAL_INTERRUPT_DMA10,           // DMA Channel 10 Transfer Complete
    CYGNUM_HAL_INTERRUPT_DMA11,           // DMA Channel 11 Transfer Complete
    CYGNUM_HAL_INTERRUPT_DMA12,           // DMA Channel 12 Transfer Complete
    CYGNUM_HAL_INTERRUPT_DMA13,           // DMA Channel 13 Transfer Complete
    CYGNUM_HAL_INTERRUPT_DMA14,           // DMA Channel 14 Transfer Complete
    CYGNUM_HAL_INTERRUPT_DMA15,           // DMA Channel 15 Transfer Complete
    CYGNUM_HAL_INTERRUPT_DMA_ERROR,       // DMA Error Int
    CYGNUM_HAL_INTERRUPT_MCM,             // Normal Int
    CYGNUM_HAL_INTERRUPT_FTFL,            // FTFL Int
    CYGNUM_HAL_INTERRUPT_READ_COLLISION,  // Read Collision Int
    CYGNUM_HAL_INTERRUPT_LVD_LVW,         // Low Volt Detect, Low Volt Warn
    CYGNUM_HAL_INTERRUPT_LLW,             // Low Leakage Wakeup
    CYGNUM_HAL_INTERRUPT_WDOG,            // WDOG Int
    CYGNUM_HAL_INTERRUPT_RNGB,            // RNGB Int
    CYGNUM_HAL_INTERRUPT_I2C0,            // I2C0 int
    CYGNUM_HAL_INTERRUPT_I2C1,            // I2C1 int
    CYGNUM_HAL_INTERRUPT_SPI0,            // SPI0 Int
    CYGNUM_HAL_INTERRUPT_SPI1,            // SPI1 Int
    CYGNUM_HAL_INTERRUPT_SPI2,            // SPI2 Int
    CYGNUM_HAL_INTERRUPT_CAN0_ORED_MESSAGE_BUFFER,// CAN0 OR'd Msg Buffs Int
    CYGNUM_HAL_INTERRUPT_CAN0_BUS_OFF,    // CAN0 Bus Off Int
    CYGNUM_HAL_INTERRUPT_CAN0_ERROR,      // CAN0 Error Int
    CYGNUM_HAL_INTERRUPT_CAN0_TX_WARNING, // CAN0 Tx Warning Int
    CYGNUM_HAL_INTERRUPT_CAN0_RX_WARNING, // CAN0 Rx Warning Int
    CYGNUM_HAL_INTERRUPT_CAN0_WAKE_UP,    // CAN0 Wake Up Int
    CYGNUM_HAL_INTERRUPT_CAN0_IMEU,       // CAN0 Ind. Match El Update (IMEU) Int
    CYGNUM_HAL_INTERRUPT_CAN0_LOST_RX,    // CAN0 Lost Receive Int
    CYGNUM_HAL_INTERRUPT_CAN1_ORED_MESSAGE_BUFFER, // CAN1 OR'd Msg Buffs Int
    CYGNUM_HAL_INTERRUPT_CAN1_BUS_OFF,     // CAN1 Bus Off Int
    CYGNUM_HAL_INTERRUPT_CAN1_ERROR,      // CAN1 Error Int
    CYGNUM_HAL_INTERRUPT_CAN1_TX_WARNING, // CAN1 Tx Warning Int
    CYGNUM_HAL_INTERRUPT_CAN1_RX_WARNING, // CAN1 Rx Warning Int
    CYGNUM_HAL_INTERRUPT_CAN1_WAKE_UP,    // CAN1 Wake Up Int
    CYGNUM_HAL_INTERRUPT_CAN1_IMEU,       // CAN1 Ind. Match El Update (IMEU) Int
    CYGNUM_HAL_INTERRUPT_CAN1_LOST_RX,    // CAN1 Lost Receive Int
    CYGNUM_HAL_INTERRUPT_UART0_RX_TX,     // UART0 Receive/Transmit int
    CYGNUM_HAL_INTERRUPT_UART0_ERR,       // UART0 Error int
    CYGNUM_HAL_INTERRUPT_UART1_RX_TX,     // UART1 Receive/Transmit int
    CYGNUM_HAL_INTERRUPT_UART1_ERR,       // UART1 Error int
    CYGNUM_HAL_INTERRUPT_UART2_RX_TX,     // UART2 Receive/Transmit int
    CYGNUM_HAL_INTERRUPT_UART2_ERR,       // UART2 Error int
    CYGNUM_HAL_INTERRUPT_UART3_RX_TX,     // UART3 Receive/Transmit int
    CYGNUM_HAL_INTERRUPT_UART3_ERR,       // UART3 Error int
    CYGNUM_HAL_INTERRUPT_UART4_RX_TX,     // UART4 Receive/Transmit int
    CYGNUM_HAL_INTERRUPT_UART4_ERR,       // UART4 Error int
    CYGNUM_HAL_INTERRUPT_UART5_RX_TX,     // UART5 Receive/Transmit int
    CYGNUM_HAL_INTERRUPT_UART5_ERR,       // UART5 Error int
    CYGNUM_HAL_INTERRUPT_ADC0,            // ADC0 int
    CYGNUM_HAL_INTERRUPT_ADC1,            // ADC1 int
    CYGNUM_HAL_INTERRUPT_CMP0,            // CMP0 int
    CYGNUM_HAL_INTERRUPT_CMP1,            // CMP1 int
    CYGNUM_HAL_INTERRUPT_CMP2,            // CMP2 int
    CYGNUM_HAL_INTERRUPT_FTM0,            // FTM0 fault, overflow and channels int
    CYGNUM_HAL_INTERRUPT_FTM1,            // FTM1 fault, overflow and channels int
    CYGNUM_HAL_INTERRUPT_FTM2,            // FTM2 fault, overflow and channels int
    CYGNUM_HAL_INTERRUPT_CMT,             // CMT int
    CYGNUM_HAL_INTERRUPT_RTC,             // RTC int
    CYGNUM_HAL_INTERRUPT_Reserved83,      // Reserved int 83
    CYGNUM_HAL_INTERRUPT_PIT0,            // PIT timer channel 0 int
    CYGNUM_HAL_INTERRUPT_PIT1,            // PIT timer channel 1 int
    CYGNUM_HAL_INTERRUPT_PIT2,            // PIT timer channel 2 int
    CYGNUM_HAL_INTERRUPT_PIT3,            // PIT timer channel 3 int
    CYGNUM_HAL_INTERRUPT_PDB0,            // PDB0 Int
    CYGNUM_HAL_INTERRUPT_USB0,            // USB0 int
    CYGNUM_HAL_INTERRUPT_USBDCD,          // USBDCD Int
    CYGNUM_HAL_INTERRUPT_ENET_1588_TIMER, // ENET MAC IEEE 1588 Timer Int
    CYGNUM_HAL_INTERRUPT_ENET_TRANSMIT,   // ENET MAC Transmit Int
    CYGNUM_HAL_INTERRUPT_ENET_RECEIVE,    // ENET MAC Receive Int
    CYGNUM_HAL_INTERRUPT_ENET_ERROR,      // ENET MAC Error and miscelaneous Int
    CYGNUM_HAL_INTERRUPT_I2S0,            // I2S0 Int
    CYGNUM_HAL_INTERRUPT_SDHC,            // SDHC Int
    CYGNUM_HAL_INTERRUPT_DAC0,            // DAC0 int
    CYGNUM_HAL_INTERRUPT_DAC1,            // DAC1 int
    CYGNUM_HAL_INTERRUPT_TSI0,            // TSI0 Int
    CYGNUM_HAL_INTERRUPT_MCG,             // MCG Int
    CYGNUM_HAL_INTERRUPT_LPTIMER,         // LPTimer int
    CYGNUM_HAL_INTERRUPT_LCD,             // Segment LCD int
    CYGNUM_HAL_INTERRUPT_PORTA,           // Port A int
    CYGNUM_HAL_INTERRUPT_PORTB,           // Port B int
    CYGNUM_HAL_INTERRUPT_PORTC,           // Port C int
    CYGNUM_HAL_INTERRUPT_PORTD,           // Port D int
    CYGNUM_HAL_INTERRUPT_PORTE,           // Port E int
    CYGNUM_HAL_INTERRUPT_Reserved108,     // Reserved int 108
    CYGNUM_HAL_INTERRUPT_Reserved109,     // Reserved int 109
    CYGNUM_HAL_INTERRUPT_Reserved110,     // Reserved int 110
    CYGNUM_HAL_INTERRUPT_Reserved111,     // Reserved int 111
    CYGNUM_HAL_INTERRUPT_Reserved112,     // Reserved int 112
    CYGNUM_HAL_INTERRUPT_Reserved113,     // Reserved int 113
    CYGNUM_HAL_INTERRUPT_Reserved114,     // Reserved int 114
    CYGNUM_HAL_INTERRUPT_Reserved115,     // Reserved int 115
    CYGNUM_HAL_INTERRUPT_Reserved116,     // Reserved int 116
    CYGNUM_HAL_INTERRUPT_Reserved117,     // Reserved int 117
    CYGNUM_HAL_INTERRUPT_Reserved118,     // Reserved int 118
    CYGNUM_HAL_INTERRUPT_Reserved119      // Reserved int 119
} KinetisExtInterrupt_e;

#define CYGNUM_HAL_INTERRUPT_NVIC_MAX (CYGNUM_HAL_INTERRUPT_Reserved119)

#define CYGNUM_HAL_ISR_MIN            0
#define CYGNUM_HAL_ISR_MAX            CYGNUM_HAL_INTERRUPT_Reserved119
#define CYGNUM_HAL_ISR_COUNT          (CYGNUM_HAL_ISR_MAX + 1)

#define CYGNUM_HAL_VSR_MIN            0
#ifndef CYGNUM_HAL_VSR_MAX
# define CYGNUM_HAL_VSR_MAX           (CYGNUM_HAL_VECTOR_SYS_TICK+ \
                                       CYGNUM_HAL_INTERRUPT_NVIC_MAX)
#endif

#define CYGNUM_HAL_VSR_COUNT          (CYGNUM_HAL_VSR_MAX+1)

//==========================================================================
// Interrupt mask and config for variant-specific devices

// PORT Pin interrupts

#define CYGHWR_HAL_KINETIS_PIN_IRQ_VECTOR(__pin) \
    (CYGNUM_HAL_INTERRUPT_PORTA + CYGHWR_HAL_KINETIS_PIN_PORT(__pin))

//===========================================================================
// Interrupt resources exported by HAL to device drivers

// Export Interrupt vectors to serial driver.

#define CYGNUM_IO_SERIAL_FREESCALE_UART0_INT_VECTOR \
            CYGNUM_HAL_INTERRUPT_UART0_RX_TX
#define CYGNUM_IO_SERIAL_FREESCALE_UART1_INT_VECTOR \
            CYGNUM_HAL_INTERRUPT_UART1_RX_TX
#define CYGNUM_IO_SERIAL_FREESCALE_UART2_INT_VECTOR \
            CYGNUM_HAL_INTERRUPT_UART2_RX_TX
#define CYGNUM_IO_SERIAL_FREESCALE_UART3_INT_VECTOR \
            CYGNUM_HAL_INTERRUPT_UART3_RX_TX
#define CYGNUM_IO_SERIAL_FREESCALE_UART4_INT_VECTOR \
            CYGNUM_HAL_INTERRUPT_UART4_RX_TX
#define CYGNUM_IO_SERIAL_FREESCALE_UART5_INT_VECTOR \
            CYGNUM_HAL_INTERRUPT_UART5_RX_TX

// Export Interrupt vectors to ENET driver.

#define CYGNUM_FREESCALE_ENET0_1588_TIMER_INT_VECTOR \
            CYGNUM_HAL_INTERRUPT_ENET_1588_TIMER
#define CYGNUM_FREESCALE_ENET0_TRANSMIT_INT_VECTOR   \
            CYGNUM_HAL_INTERRUPT_ENET_TRANSMIT
#define CYGNUM_FREESCALE_ENET0_RECEIVE_INT_VECTOR    \
            CYGNUM_HAL_INTERRUPT_ENET_RECEIVE
#define CYGNUM_FREESCALE_ENET0_ERROR_INT_VECTOR      \
            CYGNUM_HAL_INTERRUPT_ENET_ERROR

//----------------------------------------------------------------------------
#endif // CYGONCE_HAL_VAR_INTR_H
// EOF var_intr.h
