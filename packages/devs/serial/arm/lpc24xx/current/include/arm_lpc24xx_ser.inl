//==========================================================================
//
//      io/serial/arm/arm_lpc24xx_ser.inl
//
//      ARM LPC24XX Serial I/O definitions
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004 Free Software Foundation, Inc.
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
// Author(s):    Uwe Kindler
// Contributors: gthomas, jlarmour
// Date:         2008-06-07
// Purpose:      LPC24XX Serial I/O module (interrupt driven version)
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================


//==========================================================================
//                               INCLUDES
//==========================================================================
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/lpc24xx_misc.h>


//==========================================================================
//                              STATIC DATA
//==========================================================================
// Baud rate specification
static const unsigned int select_baud[] = 
{
    9999,    // Unused
    50,
    75,
    110,
    134.5,
    150,
    200,
    300,
    600,
    1200,
    1800,
    2400,
    3600,
    4800,
    7200,
    9600,
    14400,
    19200,
    38400,
    57600,
    115200,
    230400
};


//==========================================================================
// Return baudrate devisor for certain baudrate
//==========================================================================
unsigned short lpc24xx_baud_generator(pc_serial_info         *ser_chan, 
                                      cyg_serial_baud_rate_t  baud)
{
    cyg_uint8 pclk_id = CYNUM_HAL_LPC24XX_PCLK_UART0;
    switch (ser_chan->base)
    {
        case CYGARC_HAL_LPC24XX_REG_UART0_BASE:
             pclk_id = CYNUM_HAL_LPC24XX_PCLK_UART0;
             break;
             
        case CYGARC_HAL_LPC24XX_REG_UART1_BASE:
             pclk_id = CYNUM_HAL_LPC24XX_PCLK_UART1;
             break;
             
        case CYGARC_HAL_LPC24XX_REG_UART2_BASE:
             pclk_id = CYNUM_HAL_LPC24XX_PCLK_UART2;
             break;
             
        case CYGARC_HAL_LPC24XX_REG_UART3_BASE:
             pclk_id = CYNUM_HAL_LPC24XX_PCLK_UART3;
             break;
             
        default:
            CYG_FAIL("Invalid UART base address");
    } // (ser_chan->base)
    
    return CYG_HAL_ARM_LPC24XX_BAUD_GENERATOR(pclk_id, select_baud[baud]); 
}


#define CYG_IO_SERIAL_GENERIC_16X5X_CHAN_BAUD_GENERATOR(_ser_chan_, _baud_) \
        lpc24xx_baud_generator((_ser_chan_), (_baud_))



//==========================================================================
//                          SERIAL CHANNEL 0
//==========================================================================
#ifdef CYGPKG_IO_SERIAL_ARM_LPC24XX_SERIAL0
static pc_serial_info lpc24xx_serial_info0 = 
{ 
    CYGARC_HAL_LPC24XX_REG_UART0_BASE,
    CYGNUM_HAL_INTERRUPT_UART0,
    CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL0_INTPRIO
};

#if CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL0_BUFSIZE > 0
static unsigned char 
lpc24xx_serial_out_buf0[CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL0_BUFSIZE];
static unsigned char 
lpc24xx_serial_in_buf0[CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL0_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(lpc24xx_serial_channel0,
                                       pc_serial_funs, 
                                       lpc24xx_serial_info0,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL0_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &lpc24xx_serial_out_buf0[0], 
                                       sizeof(lpc24xx_serial_out_buf0),
                                       &lpc24xx_serial_in_buf0[0], 
                                       sizeof(lpc24xx_serial_in_buf0)
    );
#else
static SERIAL_CHANNEL(lpc24xx_serial_channel0,
                      pc_serial_funs, 
                      lpc24xx_serial_info0,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL0_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(lpc24xx_serial_io0, 
             CYGDAT_IO_SERIAL_ARM_LPC24XX_SERIAL0_NAME,
             0,                     // Does not depend on a lower
                                    // level interface
             &cyg_io_serial_devio, 
             pc_serial_init, 
             pc_serial_lookup,     // Serial driver may need initializing
             &lpc24xx_serial_channel0
    );
#endif //  CYGPKG_IO_SERIAL_ARM_LPC24XX_SERIAL0


//==========================================================================
//                          SERIAL CHANNEL 1
//==========================================================================
#ifdef CYGPKG_IO_SERIAL_ARM_LPC24XX_SERIAL1
static pc_serial_info lpc24xx_serial_info1 = 
{ 
    CYGARC_HAL_LPC24XX_REG_UART1_BASE,
    CYGNUM_HAL_INTERRUPT_UART1,
    CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL1_INTPRIO
};

#if CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL1_BUFSIZE > 0
static unsigned char 
lpc24xx_serial_out_buf1[CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL1_BUFSIZE];
static unsigned char 
lpc24xx_serial_in_buf1[CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL1_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(lpc24xx_serial_channel1,
                                       pc_serial_funs, 
                                       lpc24xx_serial_info1,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL1_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &lpc24xx_serial_out_buf1[0], 
                                       sizeof(lpc24xx_serial_out_buf1),
                                       &lpc24xx_serial_in_buf1[0], 
                                       sizeof(lpc24xx_serial_in_buf1)
    );
#else
static SERIAL_CHANNEL(lpc24xx_serial_channel1,
                      pc_serial_funs, 
                      lpc24xx_serial_info1,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL1_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(lpc24xx_serial_io1, 
             CYGDAT_IO_SERIAL_ARM_LPC24XX_SERIAL1_NAME,
             0,                     // Does not depend on a lower
                                    // level interface
             &cyg_io_serial_devio, 
             pc_serial_init, 
             pc_serial_lookup,     // Serial driver may need initializing
             &lpc24xx_serial_channel1
    );
#endif //  CYGPKG_IO_SERIAL_ARM_LPC24XX_SERIAL1


//==========================================================================
//                          SERIAL CHANNEL 2
//==========================================================================
#ifdef CYGPKG_IO_SERIAL_ARM_LPC24XX_SERIAL2
static pc_serial_info lpc24xx_serial_info2 = 
{ 
    CYGARC_HAL_LPC24XX_REG_UART2_BASE,
    CYGNUM_HAL_INTERRUPT_UART2,
    CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL2_INTPRIO
};

#if CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL2_BUFSIZE > 0
static unsigned char 
lpc24xx_serial_out_buf2[CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL2_BUFSIZE];
static unsigned char 
lpc24xx_serial_in_buf2[CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL2_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(lpc24xx_serial_channel2,
                                       pc_serial_funs, 
                                       lpc24xx_serial_info2,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL2_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &lpc24xx_serial_out_buf2[0], 
                                       sizeof(lpc24xx_serial_out_buf2),
                                       &lpc24xx_serial_in_buf2[0], 
                                       sizeof(lpc24xx_serial_in_buf2)
    );
#else
static SERIAL_CHANNEL(lpc24xx_serial_channel2,
                      pc_serial_funs, 
                      lpc24xx_serial_info2,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL2_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(lpc24xx_serial_io2, 
             CYGDAT_IO_SERIAL_ARM_LPC24XX_SERIAL2_NAME,
             0,                     // Does not depend on a lower
                                    // level interface
             &cyg_io_serial_devio, 
             pc_serial_init, 
             pc_serial_lookup,     // Serial driver may need initializing
             &lpc24xx_serial_channel2
    );
#endif //  CYGPKG_IO_SERIAL_ARM_LPC24XX_SERIAL2


//==========================================================================
//                          SERIAL CHANNEL 3
//==========================================================================
#ifdef CYGPKG_IO_SERIAL_ARM_LPC24XX_SERIAL3
static pc_serial_info lpc24xx_serial_info3 = 
{ 
    CYGARC_HAL_LPC24XX_REG_UART3_BASE,
    CYGNUM_HAL_INTERRUPT_UART3,
    CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL3_INTPRIO
};

#if CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL3_BUFSIZE > 0
static unsigned char 
lpc24xx_serial_out_buf3[CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL3_BUFSIZE];
static unsigned char 
lpc24xx_serial_in_buf3[CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL3_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(lpc24xx_serial_channel3,
                                       pc_serial_funs, 
                                       lpc24xx_serial_info3,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL3_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &lpc24xx_serial_out_buf3[0], 
                                       sizeof(lpc24xx_serial_out_buf3),
                                       &lpc24xx_serial_in_buf3[0], 
                                       sizeof(lpc24xx_serial_in_buf3)
    );
#else
static SERIAL_CHANNEL(lpc24xx_serial_channel3,
                      pc_serial_funs, 
                      lpc24xx_serial_info3,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_LPC24XX_SERIAL3_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(lpc24xx_serial_io3, 
             CYGDAT_IO_SERIAL_ARM_LPC24XX_SERIAL3_NAME,
             0,                     // Does not depend on a lower
                                    // level interface
             &cyg_io_serial_devio, 
             pc_serial_init, 
             pc_serial_lookup,     // Serial driver may need initializing
             &lpc24xx_serial_channel3
    );
#endif //  CYGPKG_IO_SERIAL_ARM_LPC24XX_SERIAL3


//----------------------------------------------------------------------------
// EOF arm_lpc2xxx_ser.inl
