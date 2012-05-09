#ifndef CYGONCE_HAL_VAR_IO_DEVS_H
#define CYGONCE_HAL_VAR_IO_DEVS_H
//===========================================================================
//
//      var_io_devs.h
//
//      Variant specific registers
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Ilija Kocho <ilijak@siva.com.mk>
// Date:          2011-02-05
// Purpose:       Kinetis variant IO provided to various device drivers
// Description:
// Usage:         #include <cyg/hal/var_io.h> //var_io.h includes this file
//
//####DESCRIPTIONEND####
//
//===========================================================================


//=============================================================================
// DEVS:
// Following macros may be, and usually are borrwed by some device drivers.

// Peripheral clock [Hz];
__externC cyg_uint32 hal_get_peripheral_clock(void);

//-----------------------------------------------------------------------------
// Freescale UART
// Borrow some HAL resources to Freescale UART driver
// UART  macros are used by both:
//      src/hal_diag.c
//      devs/serial/<version>/src/ser_freescale_uart.c

#define CYGADDR_IO_SERIAL_FREESCALE_UART0_BASE  0x4006A000
#define CYGADDR_IO_SERIAL_FREESCALE_UART1_BASE  0x4006B000
#define CYGADDR_IO_SERIAL_FREESCALE_UART2_BASE  0x4006C000
#define CYGADDR_IO_SERIAL_FREESCALE_UART3_BASE  0x4006D000
#define CYGADDR_IO_SERIAL_FREESCALE_UART4_BASE  0x400EA000
#define CYGADDR_IO_SERIAL_FREESCALE_UART5_BASE  0x400EB000

// UART PIN configuration
// Note: May be overriden by plf_io.h

#define CYGHWR_HAL_KINETIS_PORT_PIN_NONE CYGHWR_HAL_KINETIS_PIN_NONE

#ifndef CYGHWR_IO_FREESCALE_UART0_PIN_RX
# define CYGHWR_IO_FREESCALE_UART0_PIN_RX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART0_PIN_TX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART0_PIN_RTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART0_PIN_CTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
#endif

#ifndef CYGHWR_IO_FREESCALE_UART1_PIN_RX
# define CYGHWR_IO_FREESCALE_UART1_PIN_RX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART1_PIN_TX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART1_PIN_RTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART1_PIN_CTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
#endif

#ifndef CYGHWR_IO_FREESCALE_UART2_PIN_RX
# define CYGHWR_IO_FREESCALE_UART2_PIN_RX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART2_PIN_TX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART2_PIN_RTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART2_PIN_CTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
#endif

#ifndef CYGHWR_IO_FREESCALE_UART3_PIN_RX
# define CYGHWR_IO_FREESCALE_UART3_PIN_RX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART3_PIN_TX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART3_PIN_RTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART3_PIN_CTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
#endif

#ifndef CYGHWR_IO_FREESCALE_UART4_PIN_RX
# define CYGHWR_IO_FREESCALE_UART4_PIN_RX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART4_PIN_TX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART4_PIN_RTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART4_PIN_CTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
#endif

#ifndef CYGHWR_IO_FREESCALE_UART5_PIN_RX
# define CYGHWR_IO_FREESCALE_UART5_PIN_RX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART5_PIN_TX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART5_PIN_RTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART5_PIN_CTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
#endif

// Lend some HAL dependent functions to the UART serial device driver

#ifndef __ASSEMBLER__

# define CYGHWR_IO_FREESCALE_UART_BAUD_SET(__uart_p, _baud_) \
        hal_freescale_uart_setbaud(__uart_p, _baud_)

# define CYGHWR_IO_FREESCALE_UART_PIN(__pin) \
        hal_set_pin_function(__pin)


// Set baud rate
__externC void hal_freescale_uart_setbaud( CYG_ADDRESS uart, cyg_uint32 baud );

#endif

//---------------------------------------------------------------------------
// ENET
// Lend some HAL dependent functions to the Ethernet device driver
#define CYGADDR_IO_ETH_FREESCALE_ENET0_BASE  (0x400C0000)

#ifndef __ASSEMBLER__

# define CYGHWR_IO_FREESCALE_ENET_PIN(__pin) \
        hal_set_pin_function(__pin)

#endif

//----------------------------------------------------------------------------
// DSPI
// Lend some HAL dependent macros to DSPI device driver

// DSPI - Peripheral instance base addresses
#define CYGADDR_IO_SPI_FREESCALE_DSPI0_P ((cyghwr_devs_freescale_dspi_t*)0x4002C000)
#define CYGADDR_IO_SPI_FREESCALE_DSPI1_P ((cyghwr_devs_freescale_dspi_t*)0x4002D000)
#define CYGADDR_IO_SPI_FREESCALE_DSPI2_P ((cyghwr_devs_freescale_dspi_t*)0x400AC000)

#define CYGHWR_IO_SPI_FREESCALE_DSPI_CLOCK hal_get_peripheral_clock();

#ifndef __ASSEMBLER__

# define CYGHWR_IO_FREESCALE_DSPI_PIN(__pin) \
        hal_set_pin_function(__pin)

#endif

#ifndef KINETIS_PIN_SPI0_OUT_OPT
#define KINETIS_PIN_SPI0_OUT_OPT (CYGHWR_HAL_KINETIS_PORT_PCR_DSE_M | \
                                 CYGHWR_HAL_KINETIS_PORT_PCR_SRE_M)
#endif
#ifndef KINETIS_PIN_SPI0_CS_OPT
#define KINETIS_PIN_SPI0_CS_OPT  (CYGHWR_HAL_KINETIS_PORT_PCR_DSE_M | \
                                 CYGHWR_HAL_KINETIS_PORT_PCR_SRE_M)
#endif
#ifndef KINETIS_PIN_SPI0_IN_OPT
#define KINETIS_PIN_SPI0_IN_OPT  (CYGHWR_HAL_KINETIS_PORT_PCR_PE_M |  \
                                 CYGHWR_HAL_KINETIS_PORT_PCR_PS_M)
#endif

#ifndef KINETIS_PIN_SPI1_OUT_OPT
#define KINETIS_PIN_SPI1_OUT_OPT (CYGHWR_HAL_KINETIS_PORT_PCR_DSE_M | \
                                 CYGHWR_HAL_KINETIS_PORT_PCR_SRE_M)
#endif
#ifndef KINETIS_PIN_SPI1_CS_OPT
#define KINETIS_PIN_SPI1_CS_OPT  (CYGHWR_HAL_KINETIS_PORT_PCR_DSE_M | \
                                 CYGHWR_HAL_KINETIS_PORT_PCR_SRE_M)
#endif
#ifndef KINETIS_PIN_SPI1_IN_OPT
#define KINETIS_PIN_SPI1_IN_OPT  (CYGHWR_HAL_KINETIS_PORT_PCR_PE_M |  \
                                 CYGHWR_HAL_KINETIS_PORT_PCR_PS_M)
#endif
#ifndef KINETIS_PIN_SPI2_OUT_OPT
#define KINETIS_PIN_SPI2_OUT_OPT (CYGHWR_HAL_KINETIS_PORT_PCR_DSE_M | \
                                 CYGHWR_HAL_KINETIS_PORT_PCR_SRE_M)
#endif
#ifndef KINETIS_PIN_SPI2_CS_OPT
#define KINETIS_PIN_SPI2_CS_OPT  (CYGHWR_HAL_KINETIS_PORT_PCR_DSE_M | \
                                 CYGHWR_HAL_KINETIS_PORT_PCR_SRE_M)
#endif
#ifndef KINETIS_PIN_SPI2_IN_OPT
#define KINETIS_PIN_SPI2_IN_OPT  (CYGHWR_HAL_KINETIS_PORT_PCR_PE_M |  \
                                 CYGHWR_HAL_KINETIS_PORT_PCR_PS_M)
#endif

// DMA MUX ------------------------------------------------------------------
// DMAMUX DMA request sources
#define FREESCALE_DMAMUX_SRC_KINETIS_DISABLE      0
#define FREESCALE_DMAMUX_SRC_KINETIS_RESERVE      1
#define FREESCALE_DMAMUX_SRC_KINETIS_UART0R       2
#define FREESCALE_DMAMUX_SRC_KINETIS_UART0T       3
#define FREESCALE_DMAMUX_SRC_KINETIS_UART1R       4
#define FREESCALE_DMAMUX_SRC_KINETIS_UART1T       5
#define FREESCALE_DMAMUX_SRC_KINETIS_UART2R       6
#define FREESCALE_DMAMUX_SRC_KINETIS_UART2T       7
#define FREESCALE_DMAMUX_SRC_KINETIS_UART3R       8
#define FREESCALE_DMAMUX_SRC_KINETIS_UART3T       9
#define FREESCALE_DMAMUX_SRC_KINETIS_UART4R      10
#define FREESCALE_DMAMUX_SRC_KINETIS_UART4T      11
#define FREESCALE_DMAMUX_SRC_KINETIS_UART5R      12
#define FREESCALE_DMAMUX_SRC_KINETIS_UART5T      13
#define FREESCALE_DMAMUX_SRC_KINETIS_I2S0R       14
#define FREESCALE_DMAMUX_SRC_KINETIS_I3S0T       15
#define FREESCALE_DMAMUX_SRC_KINETIS_SPI0R       16
#define FREESCALE_DMAMUX_SRC_KINETIS_SPI0T       17
#define FREESCALE_DMAMUX_SRC_KINETIS_SPI1R       18
#define FREESCALE_DMAMUX_SRC_KINETIS_SPI1T       19
#define FREESCALE_DMAMUX_SRC_KINETIS_SPI2R       20
#define FREESCALE_DMAMUX_SRC_KINETIS_SPI2T       21
#define FREESCALE_DMAMUX_SRC_KINETIS_I2C0        22
#define FREESCALE_DMAMUX_SRC_KINETIS_I2C1        23 // Either I2C1
#define FREESCALE_DMAMUX_SRC_KINETIS_I2C2        23 // or I2C2
#define FREESCALE_DMAMUX_SRC_KINETIS_FTM0C0      24
#define FREESCALE_DMAMUX_SRC_KINETIS_FTM0C1      25
#define FREESCALE_DMAMUX_SRC_KINETIS_FTM0C2      26
#define FREESCALE_DMAMUX_SRC_KINETIS_FTM0C3      27
#define FREESCALE_DMAMUX_SRC_KINETIS_FTM0C4      28
#define FREESCALE_DMAMUX_SRC_KINETIS_FTM0C5      29
#define FREESCALE_DMAMUX_SRC_KINETIS_FTM0C6      30
#define FREESCALE_DMAMUX_SRC_KINETIS_FTM0C7      31
#define FREESCALE_DMAMUX_SRC_KINETIS_FTM1C0      32
#define FREESCALE_DMAMUX_SRC_KINETIS_FTM1C1      33
#define FREESCALE_DMAMUX_SRC_KINETIS_FTM2C0      34
#define FREESCALE_DMAMUX_SRC_KINETIS_FTM2C1      35
#define FREESCALE_DMAMUX_SRC_KINETIS_1588T0      36
#define FREESCALE_DMAMUX_SRC_KINETIS_1588T1      37
#define FREESCALE_DMAMUX_SRC_KINETIS_1588T2      38
#define FREESCALE_DMAMUX_SRC_KINETIS_1588T3      39
#define FREESCALE_DMAMUX_SRC_KINETIS_ADC0        40
#define FREESCALE_DMAMUX_SRC_KINETIS_ADC1        41
#define FREESCALE_DMAMUX_SRC_KINETIS_CMP0        42
#define FREESCALE_DMAMUX_SRC_KINETIS_CMP1        43
#define FREESCALE_DMAMUX_SRC_KINETIS_CMP2        44
#define FREESCALE_DMAMUX_SRC_KINETIS_DAC0        45
#define FREESCALE_DMAMUX_SRC_KINETIS_DAC1        46
#define FREESCALE_DMAMUX_SRC_KINETIS_CMT         47
#define FREESCALE_DMAMUX_SRC_KINETIS_PDB         48
#define FREESCALE_DMAMUX_SRC_KINETIS_PORTA       49
#define FREESCALE_DMAMUX_SRC_KINETIS_PORTB       50
#define FREESCALE_DMAMUX_SRC_KINETIS_PORTC       51
#define FREESCALE_DMAMUX_SRC_KINETIS_PORTD       52
#define FREESCALE_DMAMUX_SRC_KINETIS_PORTE       53
#define FREESCALE_DMAMUX_SRC_KINETIS_DMAMUX0     54
#define FREESCALE_DMAMUX_SRC_KINETIS_DMAMUX1     55
#define FREESCALE_DMAMUX_SRC_KINETIS_DMAMUX2     56
#define FREESCALE_DMAMUX_SRC_KINETIS_DMAMUX3     57
#define FREESCALE_DMAMUX_SRC_KINETIS_DMAMUX4     58
#define FREESCALE_DMAMUX_SRC_KINETIS_DMAMUX5     59
#define FREESCALE_DMAMUX_SRC_KINETIS_DMAMUX6     60
#define FREESCALE_DMAMUX_SRC_KINETIS_DMAMUX7     61
#define FREESCALE_DMAMUX_SRC_KINETIS_DMAMUX8     62
#define FREESCALE_DMAMUX_SRC_KINETIS_DMAMUX9     63

// DMAMUX1 DMA request sources
#define FREESCALE_DMAMUX1_SRC_KINETIS_DISABLE     0
#define FREESCALE_DMAMUX1_SRC_KINETIS_RESERVE     1
#define FREESCALE_DMAMUX1_SRC_KINETIS_UART0R      2
#define FREESCALE_DMAMUX1_SRC_KINETIS_UART0T      3
#define FREESCALE_DMAMUX1_SRC_KINETIS_UART1R      4
#define FREESCALE_DMAMUX1_SRC_KINETIS_UART1T      5
#define FREESCALE_DMAMUX1_SRC_KINETIS_UART2R      6
#define FREESCALE_DMAMUX1_SRC_KINETIS_UART2T      7
#define FREESCALE_DMAMUX1_SRC_KINETIS_UART3R      8
#define FREESCALE_DMAMUX1_SRC_KINETIS_UART3T      9
#define FREESCALE_DMAMUX1_SRC_KINETIS_UART4R     10
#define FREESCALE_DMAMUX1_SRC_KINETIS_UART4T     11
#define FREESCALE_DMAMUX1_SRC_KINETIS_UART5R     12
#define FREESCALE_DMAMUX1_SRC_KINETIS_UART5T     13
#define FREESCALE_DMAMUX1_SRC_KINETIS_I2S0R      14
#define FREESCALE_DMAMUX1_SRC_KINETIS_I3S0T      15
#define FREESCALE_DMAMUX1_SRC_KINETIS_SPI0R      16
#define FREESCALE_DMAMUX1_SRC_KINETIS_SPI0T      17
#define FREESCALE_DMAMUX1_SRC_KINETIS_SPI1R      18
#define FREESCALE_DMAMUX1_SRC_KINETIS_SPI1T      19
#define FREESCALE_DMAMUX1_SRC_KINETIS_SPI2R      20
#define FREESCALE_DMAMUX1_SRC_KINETIS_SPI2T      21
#define FREESCALE_DMAMUX1_SRC_KINETIS_RESERVE_22 22
#define FREESCALE_DMAMUX1_SRC_KINETIS_RESERVE_23 23
#define FREESCALE_DMAMUX1_SRC_KINETIS_FTM3C0     24
#define FREESCALE_DMAMUX1_SRC_KINETIS_FTM3C1     25
#define FREESCALE_DMAMUX1_SRC_KINETIS_FTM3C2     26
#define FREESCALE_DMAMUX1_SRC_KINETIS_FTM3C3     27
#define FREESCALE_DMAMUX1_SRC_KINETIS_FTM3C4     28
#define FREESCALE_DMAMUX1_SRC_KINETIS_FTM3C5     29
#define FREESCALE_DMAMUX1_SRC_KINETIS_FTM3C6     30
#define FREESCALE_DMAMUX1_SRC_KINETIS_FTM3C7     31
#define FREESCALE_DMAMUX1_SRC_KINETIS_RESERVE_32 32
#define FREESCALE_DMAMUX1_SRC_KINETIS_RESERVE_33 33
#define FREESCALE_DMAMUX1_SRC_KINETIS_RESERVE_34 34
#define FREESCALE_DMAMUX1_SRC_KINETIS_RESERVE_35 35
#define FREESCALE_DMAMUX1_SRC_KINETIS_1588T0     36
#define FREESCALE_DMAMUX1_SRC_KINETIS_1588T1     37
#define FREESCALE_DMAMUX1_SRC_KINETIS_1588T2     38
#define FREESCALE_DMAMUX1_SRC_KINETIS_1588T3     39
#define FREESCALE_DMAMUX1_SRC_KINETIS_ADC0       40
#define FREESCALE_DMAMUX1_SRC_KINETIS_ADC1       41
#define FREESCALE_DMAMUX1_SRC_KINETIS_ADC2       42
#define FREESCALE_DMAMUX1_SRC_KINETIS_ADC3       43
#define FREESCALE_DMAMUX1_SRC_KINETIS_RESERVE_44 44
#define FREESCALE_DMAMUX1_SRC_KINETIS_DAC0       45
#define FREESCALE_DMAMUX1_SRC_KINETIS_DAC1       46
#define FREESCALE_DMAMUX1_SRC_KINETIS_CMP0       47
#define FREESCALE_DMAMUX1_SRC_KINETIS_CMP1       48
#define FREESCALE_DMAMUX1_SRC_KINETIS_CMP2       49
#define FREESCALE_DMAMUX1_SRC_KINETIS_CMP3       50
#define FREESCALE_DMAMUX1_SRC_KINETIS_RESERVE_51 51
#define FREESCALE_DMAMUX1_SRC_KINETIS_RESERVE_52 52
#define FREESCALE_DMAMUX1_SRC_KINETIS_PORTF      53
#define FREESCALE_DMAMUX1_SRC_KINETIS_DMAMUX0    54
#define FREESCALE_DMAMUX1_SRC_KINETIS_DMAMUX1    55
#define FREESCALE_DMAMUX1_SRC_KINETIS_DMAMUX2    56
#define FREESCALE_DMAMUX1_SRC_KINETIS_DMAMUX3    57
#define FREESCALE_DMAMUX1_SRC_KINETIS_DMAMUX4    58
#define FREESCALE_DMAMUX1_SRC_KINETIS_DMAMUX5    59
#define FREESCALE_DMAMUX1_SRC_KINETIS_DMAMUX6    60
#define FREESCALE_DMAMUX1_SRC_KINETIS_DMAMUX7    61
#define FREESCALE_DMAMUX1_SRC_KINETIS_DMAMUX8    62
#define FREESCALE_DMAMUX1_SRC_KINETIS_DMAMUX9    63

#define FREESCALE_DMAMUX_SRC_SPI0_RX FREESCALE_DMAMUX_SRC_KINETIS_SPI0R
#define FREESCALE_DMAMUX_SRC_SPI0_TX FREESCALE_DMAMUX_SRC_KINETIS_SPI0T
#define FREESCALE_DMAMUX_SRC_SPI1_RX FREESCALE_DMAMUX_SRC_KINETIS_SPI1R
#define FREESCALE_DMAMUX_SRC_SPI1_TX FREESCALE_DMAMUX_SRC_KINETIS_SPI1T
#define FREESCALE_DMAMUX_SRC_SPI2_RX FREESCALE_DMAMUX_SRC_KINETIS_SPI2R
#define FREESCALE_DMAMUX_SRC_SPI2_TX FREESCALE_DMAMUX_SRC_KINETIS_SPI2T

//----------------------------------------------------------------------------
// eDMA
// Lend some eDMA macros to device driver that use DMA

// Base address
#define CYGHWR_HAL_FREESCALE_EDMA0_P  ((cyghwr_hal_freescale_edma_t *)0x40008000)
// DMAMUX base addresses
#define CYGHWR_HAL_FREESCALE_DMAMUX0_P ((cyghwr_hal_freescale_dmamux_t *) 0x40021000)
#define CYGHWR_HAL_FREESCALE_DMAMUX1_P ((cyghwr_hal_freescale_dmamux_t *) 0x40022000)

#define CYGHWR_IO_FREESCALE_EDMA0_P CYGHWR_HAL_FREESCALE_EDMA0_P
#define CYGHWR_IO_FREESCALE_DMAMUX0_P CYGHWR_HAL_FREESCALE_DMAMUX0_P
#define CYGHWR_IO_FREESCALE_DMAMUX1_P CYGHWR_HAL_FREESCALE_DMAMUX1_P

//-----------------------------------------------------------------------------
// end of var_io_devs.h
#endif // CYGONCE_HAL_VAR_IO_DEVS_H
