//==========================================================================
//
//      devs/serial/cortexm/stm32/stm32_serial.c
//
//      ST STM32 Serial I/O Interface Module
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998-2002, 2004-2006, 2008-2012 Free Software Foundation, Inc.
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
// Author(s):     nickg
// Date:          2008-09-10
// Purpose:       ST STM32 Serial I/O module
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/infra.h>
#include <pkgconf/system.h>
#include <pkgconf/io_serial.h>
#include <pkgconf/io.h>
#include <pkgconf/kernel.h>

#include <cyg/io/io.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/io/devtab.h>
#include <cyg/io/serial.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>


#ifdef CYGPKG_IO_SERIAL_CORTEXM_STM32

#include "stm32_serial.h"

//==========================================================================

#define STM32_RXBUFSIZE 16

typedef struct stm32_serial_info
{
    CYG_WORD            uart;
    CYG_ADDRWORD        base;
    CYG_WORD            int_num;
    cyg_int32           int_pri;
    cyg_int32           rx_pin;
    cyg_int32           tx_pin;
    cyg_int32           rts_pin;
    cyg_int32           cts_pin;
    cyg_int32           clk_enable;
#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
    cyg_int32           remap;
#endif

    cyg_bool            tx_active;
    
    volatile cyg_uint8  buf[STM32_RXBUFSIZE];
    volatile int        buf_head;
    volatile int        buf_tail;
    
    cyg_interrupt       serial_interrupt;
    cyg_handle_t        serial_interrupt_handle;
} stm32_serial_info;

//==========================================================================

static bool stm32_serial_init(struct cyg_devtab_entry *tab);
static bool stm32_serial_putc_interrupt(serial_channel *chan, unsigned char c);
#if (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL0) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL0_BUFSIZE == 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL1) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL1_BUFSIZE == 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL2) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL2_BUFSIZE == 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL3) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL3_BUFSIZE == 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL4) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL4_BUFSIZE == 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL5) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL5_BUFSIZE == 0)
static bool stm32_serial_putc_polled(serial_channel *chan, unsigned char c);
#endif
static Cyg_ErrNo stm32_serial_lookup(struct cyg_devtab_entry **tab, 
                                    struct cyg_devtab_entry *sub_tab,
                                    const char *name);
static unsigned char stm32_serial_getc_interrupt(serial_channel *chan);
#if (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL0) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL0_BUFSIZE == 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL1) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL1_BUFSIZE == 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL2) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL2_BUFSIZE == 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL3) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL3_BUFSIZE == 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL4) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL4_BUFSIZE == 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL5) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL5_BUFSIZE == 0)
static unsigned char stm32_serial_getc_polled(serial_channel *chan);
#endif
static Cyg_ErrNo stm32_serial_set_config(serial_channel *chan, cyg_uint32 key,
                                        const void *xbuf, cyg_uint32 *len);
static void stm32_serial_start_xmit(serial_channel *chan);
static void stm32_serial_stop_xmit(serial_channel *chan);

static cyg_uint32 stm32_serial_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       stm32_serial_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);

//==========================================================================

#if (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL0) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL0_BUFSIZE > 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL1) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL1_BUFSIZE > 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL2) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL2_BUFSIZE > 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL3) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL3_BUFSIZE > 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL4) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL4_BUFSIZE > 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL5) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL5_BUFSIZE > 0)
static SERIAL_FUNS(stm32_serial_funs_interrupt, 
                   stm32_serial_putc_interrupt, 
                   stm32_serial_getc_interrupt,
                   stm32_serial_set_config,
                   stm32_serial_start_xmit,
                   stm32_serial_stop_xmit
    );
#endif

#if (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL0) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL0_BUFSIZE == 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL1) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL1_BUFSIZE == 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL2) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL2_BUFSIZE == 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL3) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL3_BUFSIZE == 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL4) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL4_BUFSIZE == 0) \
 || (defined(CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL5) && CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL5_BUFSIZE == 0)
static SERIAL_FUNS(stm32_serial_funs_polled, 
                   stm32_serial_putc_polled, 
                   stm32_serial_getc_polled,
                   stm32_serial_set_config,
                   stm32_serial_start_xmit,
                   stm32_serial_stop_xmit
    );
#endif

//==========================================================================

#ifdef CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL0
static stm32_serial_info stm32_serial_info0 = {
    uart            : 0,
    base            : (CYG_ADDRWORD) CYGHWR_HAL_STM32_UART1,
    int_num         : CYGNUM_HAL_INTERRUPT_UART1,
    int_pri         : CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL0_INT_PRI,
    rx_pin          : CYGHWR_HAL_STM32_UART1_RX,
    tx_pin          : CYGHWR_HAL_STM32_UART1_TX,
    rts_pin         : CYGHWR_HAL_STM32_UART1_RTS,
    cts_pin         : CYGHWR_HAL_STM32_UART1_CTS,
    clk_enable      : CYGHWR_HAL_STM32_UART1_CLOCK,
#ifdef CYGHWR_HAL_STM32_UART1_REMAP_CONFIG
    remap           : CYGHWR_HAL_STM32_UART1_REMAP_CONFIG,
#endif
};

#if CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL0_BUFSIZE > 0
static unsigned char stm32_serial_out_buf0[CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL0_BUFSIZE];
static unsigned char stm32_serial_in_buf0[CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL0_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(stm32_serial_channel0,
                                       stm32_serial_funs_interrupt, 
                                       stm32_serial_info0,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL0_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &stm32_serial_out_buf0[0], sizeof(stm32_serial_out_buf0),
                                       &stm32_serial_in_buf0[0], sizeof(stm32_serial_in_buf0)
    );
#else
static SERIAL_CHANNEL(stm32_serial_channel0,
                      stm32_serial_funs_polled, 
                      stm32_serial_info0,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL0_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(stm32_serial_io0, 
             CYGDAT_IO_SERIAL_CORTEXM_STM32_SERIAL0_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             stm32_serial_init, 
             stm32_serial_lookup,     // Serial driver may need initializing
             &stm32_serial_channel0
    );
#endif //  CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL0

//==========================================================================

#ifdef CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL1
static stm32_serial_info stm32_serial_info1 = {
    uart            : 1,
    base            : (CYG_ADDRWORD) CYGHWR_HAL_STM32_UART2,
    int_num         : CYGNUM_HAL_INTERRUPT_UART2,
    int_pri         : CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL1_INT_PRI,
    rx_pin          : CYGHWR_HAL_STM32_UART2_RX,
    tx_pin          : CYGHWR_HAL_STM32_UART2_TX,
    rts_pin         : CYGHWR_HAL_STM32_UART2_RTS,
    cts_pin         : CYGHWR_HAL_STM32_UART2_CTS,
    clk_enable      : CYGHWR_HAL_STM32_UART2_CLOCK,
#ifdef CYGHWR_HAL_STM32_UART2_REMAP_CONFIG
    remap           : CYGHWR_HAL_STM32_UART2_REMAP_CONFIG,    
#endif
};

#if CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL1_BUFSIZE > 0
static unsigned char stm32_serial_out_buf1[CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL1_BUFSIZE];
static unsigned char stm32_serial_in_buf1[CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL1_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(stm32_serial_channel1,
                                       stm32_serial_funs_interrupt, 
                                       stm32_serial_info1,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL1_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &stm32_serial_out_buf1[0], sizeof(stm32_serial_out_buf1),
                                       &stm32_serial_in_buf1[0], sizeof(stm32_serial_in_buf1)
    );
#else
static SERIAL_CHANNEL(stm32_serial_channel1,
                      stm32_serial_funs_polled, 
                      stm32_serial_info1,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL1_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(stm32_serial_io1, 
             CYGDAT_IO_SERIAL_CORTEXM_STM32_SERIAL1_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             stm32_serial_init, 
             stm32_serial_lookup,     // Serial driver may need initializing
             &stm32_serial_channel1
    );
#endif //  CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL1

//==========================================================================

#ifdef CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL2
static stm32_serial_info stm32_serial_info2 = {
    uart            : 2,
    base            : (CYG_ADDRWORD) CYGHWR_HAL_STM32_UART3,
    int_num         : CYGNUM_HAL_INTERRUPT_UART3,
    int_pri         : CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL2_INT_PRI,
    rx_pin          : CYGHWR_HAL_STM32_UART3_RX,
    tx_pin          : CYGHWR_HAL_STM32_UART3_TX,
    rts_pin         : CYGHWR_HAL_STM32_UART3_RTS,
    cts_pin         : CYGHWR_HAL_STM32_UART3_CTS,
    clk_enable      : CYGHWR_HAL_STM32_UART3_CLOCK,
#ifdef CYGHWR_HAL_STM32_UART3_REMAP_CONFIG
    remap           : CYGHWR_HAL_STM32_UART3_REMAP_CONFIG,    
#endif
};

#if CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL2_BUFSIZE > 0
static unsigned char stm32_serial_out_buf2[CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL2_BUFSIZE];
static unsigned char stm32_serial_in_buf2[CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL2_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(stm32_serial_channel2,
                                       stm32_serial_funs_interrupt, 
                                       stm32_serial_info2,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL2_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &stm32_serial_out_buf2[0], sizeof(stm32_serial_out_buf2),
                                       &stm32_serial_in_buf2[0], sizeof(stm32_serial_in_buf2)
    );
#else
static SERIAL_CHANNEL(stm32_serial_channel2,
                      stm32_serial_funs_polled, 
                      stm32_serial_info2,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL2_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(stm32_serial_io2, 
             CYGDAT_IO_SERIAL_CORTEXM_STM32_SERIAL2_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             stm32_serial_init, 
             stm32_serial_lookup,     // Serial driver may need initializing
             &stm32_serial_channel2
    );
#endif //  CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL2

//==========================================================================

#ifdef CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL3
static stm32_serial_info stm32_serial_info3 = {
    uart            : 3,
    base            : (CYG_ADDRWORD) CYGHWR_HAL_STM32_UART4,
    int_num         : CYGNUM_HAL_INTERRUPT_UART4,
    int_pri         : CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL3_INT_PRI,
    rx_pin          : CYGHWR_HAL_STM32_UART4_RX,
    tx_pin          : CYGHWR_HAL_STM32_UART4_TX,
    rts_pin         : CYGHWR_HAL_STM32_UART4_RTS,
    cts_pin         : CYGHWR_HAL_STM32_UART4_CTS,
    clk_enable      : CYGHWR_HAL_STM32_UART4_CLOCK,
#ifdef CYGHWR_HAL_STM32_UART4_REMAP_CONFIG
    remap           : CYGHWR_HAL_STM32_UART4_REMAP_CONFIG,    
#endif
};

#if CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL3_BUFSIZE > 0
static unsigned char stm32_serial_out_buf3[CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL3_BUFSIZE];
static unsigned char stm32_serial_in_buf3[CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL3_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(stm32_serial_channel3,
                                       stm32_serial_funs_interrupt, 
                                       stm32_serial_info3,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL3_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &stm32_serial_out_buf3[0], sizeof(stm32_serial_out_buf3),
                                       &stm32_serial_in_buf3[0], sizeof(stm32_serial_in_buf3)
    );
#else
static SERIAL_CHANNEL(stm32_serial_channel3,
                      stm32_serial_funs_polled, 
                      stm32_serial_info3,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL3_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(stm32_serial_io3, 
             CYGDAT_IO_SERIAL_CORTEXM_STM32_SERIAL3_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             stm32_serial_init, 
             stm32_serial_lookup,     // Serial driver may need initializing
             &stm32_serial_channel3
    );
#endif //  CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL3

//==========================================================================

#ifdef CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL4
static stm32_serial_info stm32_serial_info4 = {
    uart            : 4,
    base            : (CYG_ADDRWORD) CYGHWR_HAL_STM32_UART5,
    int_num         : CYGNUM_HAL_INTERRUPT_UART5,
    int_pri         : CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL4_INT_PRI,
    rx_pin          : CYGHWR_HAL_STM32_UART5_RX,
    tx_pin          : CYGHWR_HAL_STM32_UART5_TX,
    rts_pin         : CYGHWR_HAL_STM32_UART5_RTS,
    cts_pin         : CYGHWR_HAL_STM32_UART5_CTS,
    clk_enable      : CYGHWR_HAL_STM32_UART5_CLOCK,
#ifdef CYGHWR_HAL_STM32_UART5_REMAP_CONFIG
    remap           : CYGHWR_HAL_STM32_UART5_REMAP_CONFIG,    
#endif
};

#if CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL4_BUFSIZE > 0
static unsigned char stm32_serial_out_buf4[CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL4_BUFSIZE];
static unsigned char stm32_serial_in_buf4[CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL4_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(stm32_serial_channel4,
                                       stm32_serial_funs_interrupt, 
                                       stm32_serial_info4,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL4_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &stm32_serial_out_buf4[0], sizeof(stm32_serial_out_buf4),
                                       &stm32_serial_in_buf4[0], sizeof(stm32_serial_in_buf4)
    );
#else
static SERIAL_CHANNEL(stm32_serial_channel4,
                      stm32_serial_funs_polled, 
                      stm32_serial_info4,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL4_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(stm32_serial_io4, 
             CYGDAT_IO_SERIAL_CORTEXM_STM32_SERIAL4_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             stm32_serial_init, 
             stm32_serial_lookup,     // Serial driver may need initializing
             &stm32_serial_channel4
    );
#endif //  CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL4


//==========================================================================

#ifdef CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL5
static stm32_serial_info stm32_serial_info5 = {
    uart            : 5,
    base            : (CYG_ADDRWORD) CYGHWR_HAL_STM32_UART6,
    int_num         : CYGNUM_HAL_INTERRUPT_UART6,
    int_pri         : CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL5_INT_PRI,
    rx_pin          : CYGHWR_HAL_STM32_UART6_RX,
    tx_pin          : CYGHWR_HAL_STM32_UART6_TX,
    rts_pin         : CYGHWR_HAL_STM32_UART6_RTS,
    cts_pin         : CYGHWR_HAL_STM32_UART6_CTS,
    clk_enable      : CYGHWR_HAL_STM32_UART6_CLOCK,
#ifdef CYGHWR_HAL_STM32_UART6_REMAP_CONFIG
    remap           : CYGHWR_HAL_STM32_UART6_REMAP_CONFIG,    
#endif
};

#if CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL5_BUFSIZE > 0
static unsigned char stm32_serial_out_buf5[CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL5_BUFSIZE];
static unsigned char stm32_serial_in_buf5[CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL5_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(stm32_serial_channel5,
                                       stm32_serial_funs_interrupt, 
                                       stm32_serial_info5,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL5_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &stm32_serial_out_buf5[0], sizeof(stm32_serial_out_buf5),
                                       &stm32_serial_in_buf5[0], sizeof(stm32_serial_in_buf5)
    );
#else
static SERIAL_CHANNEL(stm32_serial_channel5,
                      stm32_serial_funs_polled, 
                      stm32_serial_info5,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_CORTEXM_STM32_SERIAL5_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(stm32_serial_io5, 
             CYGDAT_IO_SERIAL_CORTEXM_STM32_SERIAL5_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             stm32_serial_init, 
             stm32_serial_lookup,     // Serial driver may need initializing
             &stm32_serial_channel5
    );
#endif //  CYGPKG_IO_SERIAL_CORTEXM_STM32_SERIAL5


//==========================================================================
// Internal function to actually configure the hardware to desired baud
// rate, etc.

static bool
stm32_serial_config_port(serial_channel *chan, cyg_serial_info_t *new_config, bool init)
{
    stm32_serial_info * const stm32_chan = (stm32_serial_info *)chan->dev_priv;
    const CYG_ADDRWORD base = stm32_chan->base;
    cyg_uint32 parity = select_parity[new_config->parity];
    cyg_uint32 word_length = ((new_config->word_length == CYGNUM_SERIAL_WORD_LENGTH_8) &&
        (new_config->parity != CYGNUM_SERIAL_PARITY_NONE)) ?
        CYGHWR_HAL_STM32_UART_CR1_M_9 : CYGHWR_HAL_STM32_UART_CR1_M_8;
    cyg_uint32 stop_bits = select_stop_bits[new_config->stop];
    cyg_uint32 cr1 = 0;
    cyg_uint32 cr2 = 0;
    cyg_uint32 cr3 = 0;

    // Set up FIFO buffer
    stm32_chan->buf_head = stm32_chan->buf_tail = 0;
    
    // Set up GPIO pins
    CYGHWR_HAL_STM32_GPIO_SET( stm32_chan->rx_pin );
    CYGHWR_HAL_STM32_GPIO_SET( stm32_chan->tx_pin );
    CYGHWR_HAL_STM32_GPIO_SET( stm32_chan->rts_pin );
    CYGHWR_HAL_STM32_GPIO_SET( stm32_chan->cts_pin );

    // Enable clock
    CYGHWR_HAL_STM32_CLOCK_ENABLE( stm32_chan->clk_enable );

    // Handle any pin remapping
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
    if( stm32_chan->remap != 0 )
    {
        CYG_ADDRESS afio = CYGHWR_HAL_STM32_AFIO;
        cyg_uint32 mapr;
        CYGHWR_HAL_STM32_CLOCK_ENABLE( CYGHWR_HAL_STM32_AFIO_CLOCK );
        HAL_READ_UINT32( afio+CYGHWR_HAL_STM32_AFIO_MAPR, mapr );
        mapr |= stm32_chan->remap;
        HAL_WRITE_UINT32( afio+CYGHWR_HAL_STM32_AFIO_MAPR, mapr );
    }
#endif
    
    // Select line parameters
    cr1 |= parity|word_length;
    cr2 |= stop_bits;

    cr1 |= CYGHWR_HAL_STM32_UART_CR1_TE | CYGHWR_HAL_STM32_UART_CR1_RE;

    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_UART_CR1, cr1 );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_UART_CR2, cr2 );

    // Set up baud rate
    hal_stm32_uart_setbaud( base, select_baud[new_config->baud] );

    // Enable the uart
    cr1 |= CYGHWR_HAL_STM32_UART_CR1_UE;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_UART_CR1, cr1 );    


#ifdef CYGOPT_IO_SERIAL_FLOW_CONTROL_HW
    // Handle RTS by hand but leave CTS to be handled by the UART hardware
     if ( (new_config->flags & CYGNUM_SERIAL_FLOW_RTSCTS_TX) && stm32_chan->cts_pin != CYGHWR_HAL_STM32_GPIO_NONE )
     {
         cr3 |= CYGHWR_HAL_STM32_UART_CR3_CTSE;
     }
#endif

    if(1)
    {
        // Enable receive and error interrupts

        cr1 |= CYGHWR_HAL_STM32_UART_CR1_RXNEIE;
        cr3 |= CYGHWR_HAL_STM32_UART_CR3_EIE;
        
        HAL_WRITE_UINT32( base + CYGHWR_HAL_STM32_UART_CR1, cr1 );
    }

    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_UART_CR3, cr3 );         

    stm32_chan->tx_active = false;
    
    if (new_config != &chan->config)
        chan->config = *new_config;

    return true;
}

//==========================================================================
// Function to initialize the device.  Called at bootstrap time.

static bool 
stm32_serial_init(struct cyg_devtab_entry *tab)
{
    serial_channel * const chan = (serial_channel *) tab->priv;
    stm32_serial_info * const stm32_chan = (stm32_serial_info *) chan->dev_priv;
    int res;

    (chan->callbacks->serial_init)(chan);  // Really only required for interrupt driven devices
    if (chan->out_cbuf.len != 0) {
        cyg_drv_interrupt_create(stm32_chan->int_num,
                                 stm32_chan->int_pri,
                                 (cyg_addrword_t)chan,
                                 stm32_serial_ISR,
                                 stm32_serial_DSR,
                                 &stm32_chan->serial_interrupt_handle,
                                 &stm32_chan->serial_interrupt);
        cyg_drv_interrupt_attach(stm32_chan->serial_interrupt_handle);
        cyg_drv_interrupt_unmask(stm32_chan->int_num);

    }

    res = stm32_serial_config_port(chan, &chan->config, true);
    return res;
}

//==========================================================================
// This routine is called when the device is "looked" up (i.e. attached)

static Cyg_ErrNo 
stm32_serial_lookup(struct cyg_devtab_entry **tab, 
                  struct cyg_devtab_entry *sub_tab,
                  const char *name)
{
    serial_channel * const chan = (serial_channel *) (*tab)->priv;

    (chan->callbacks->serial_init)(chan);  // Really only required for interrupt driven devices
    return ENOERR;
}

//==========================================================================
// Send a character to the device output buffer.
// Return 'true' if character is sent to device

static bool
stm32_serial_putc_interrupt(serial_channel *chan, unsigned char c)
{
    stm32_serial_info * const stm32_chan = (stm32_serial_info *) chan->dev_priv;
    const CYG_ADDRWORD base = stm32_chan->base;
    cyg_uint32 status;
    
    HAL_READ_UINT32( base + CYGHWR_HAL_STM32_UART_SR, status );

    if (status & CYGHWR_HAL_STM32_UART_SR_TXE)
    {
        HAL_WRITE_UINT32( base + CYGHWR_HAL_STM32_UART_DR, c );
        return true;
    }
    
    return false;
}

//==========================================================================

static bool
stm32_serial_putc_polled(serial_channel *chan, unsigned char c)
{
    stm32_serial_info * const stm32_chan = (stm32_serial_info *) chan->dev_priv;
    const CYG_ADDRWORD base = stm32_chan->base;
    cyg_uint32 status;

     do {
         HAL_READ_UINT32( base + CYGHWR_HAL_STM32_UART_SR, status );
     } while ((status & CYGHWR_HAL_STM32_UART_SR_TXE) == 0);

     HAL_WRITE_UINT32( base + CYGHWR_HAL_STM32_UART_DR, c );

    return true;
}

//==========================================================================
// Fetch a character from the device input buffer

static unsigned char 
stm32_serial_getc_interrupt(serial_channel *chan)
{
    stm32_serial_info * const stm32_chan = (stm32_serial_info *) chan->dev_priv;
    const CYG_ADDRWORD base = stm32_chan->base;
    CYG_WORD32 c;

    // Read data
    HAL_READ_UINT32( base + CYGHWR_HAL_STM32_UART_DR, c);
    return (unsigned char) (c&0xFF);
}

//==========================================================================

static unsigned char 
stm32_serial_getc_polled(serial_channel *chan)
{
    stm32_serial_info * const stm32_chan = (stm32_serial_info *) chan->dev_priv;
    const CYG_ADDRWORD base = stm32_chan->base;
    cyg_uint32 stat;
    cyg_uint32 c;

    do {
        HAL_READ_UINT32( base + CYGHWR_HAL_STM32_UART_SR, stat );
    } while ((stat & CYGHWR_HAL_STM32_UART_SR_RXNE) == 0);
        
    HAL_READ_UINT32( base + CYGHWR_HAL_STM32_UART_DR, c);
    
    return (unsigned char) (c&0xFF);
}

//==========================================================================
// Set up the device characteristics; baud rate, etc.

static Cyg_ErrNo
stm32_serial_set_config(serial_channel *chan, cyg_uint32 key,
                       const void *xbuf, cyg_uint32 *len)
{
    switch (key) {
    case CYG_IO_SET_CONFIG_SERIAL_INFO:
      {
        cyg_serial_info_t *config = (cyg_serial_info_t *)xbuf;
        if ( *len < sizeof(cyg_serial_info_t) ) {
            return -EINVAL;
        }
        *len = sizeof(cyg_serial_info_t);
        if ( true != stm32_serial_config_port(chan, config, false) )
            return -EINVAL;
      }
      break;

#ifdef CYGOPT_IO_SERIAL_FLOW_CONTROL_HW

    case CYG_IO_SET_CONFIG_SERIAL_HW_RX_FLOW_THROTTLE:
      {
          stm32_serial_info * const stm32_chan = (stm32_serial_info *) chan->dev_priv;          
          cyg_uint32 *f = (cyg_uint32 *)xbuf;

          if ( *len < sizeof(*f) )
              return -EINVAL;
          
          if ( chan->config.flags & CYGNUM_SERIAL_FLOW_RTSCTS_RX )
          {
              stm32_serial_info * const stm32_chan = (stm32_serial_info *) chan->dev_priv;

              // Note that the RTS line is active-low, so set it to 1
              // to throttle and 0 to allow the data to flow.
              if( *f )
                  CYGHWR_HAL_STM32_GPIO_OUT( stm32_chan->rts_pin, 1 );
              else
                  CYGHWR_HAL_STM32_GPIO_OUT( stm32_chan->rts_pin, 0 );
          }
      }
      break;
      
    case CYG_IO_SET_CONFIG_SERIAL_HW_FLOW_CONFIG:
      {
        stm32_serial_info * const stm32_chan = (stm32_serial_info *) chan->dev_priv;
        Cyg_ErrNo result = ENOERR;

        // If the client is asking for DSR/DTR, refuse to do it.
        if (0 != (chan->config.flags & (CYGNUM_SERIAL_FLOW_DSRDTR_RX | CYGNUM_SERIAL_FLOW_DSRDTR_TX)))
        {
            chan->config.flags &= ~(CYGNUM_SERIAL_FLOW_DSRDTR_RX | CYGNUM_SERIAL_FLOW_DSRDTR_TX);
            result = -ENOSUPP;
        }

        // If the client is asking for RTS/CTS then only allow it if
        // the port has RTS/CTS lines attached to it.
        if (0 != (chan->config.flags & (CYGNUM_SERIAL_FLOW_RTSCTS_RX | CYGNUM_SERIAL_FLOW_RTSCTS_TX)))
        {
            if( stm32_chan->rts_pin != CYGHWR_HAL_STM32_GPIO_NONE &&
                stm32_chan->cts_pin != CYGHWR_HAL_STM32_GPIO_NONE )
            {
                chan->config.flags &= (CYGNUM_SERIAL_FLOW_RTSCTS_RX | CYGNUM_SERIAL_FLOW_RTSCTS_TX);
            }
            else
            {
                chan->config.flags &= ~(CYGNUM_SERIAL_FLOW_RTSCTS_RX | CYGNUM_SERIAL_FLOW_RTSCTS_TX);
                result = -ENOSUPP;
            }
        }
        return result;
      }
      
#endif
      
    default:
        return -EINVAL;
    }
    return ENOERR;
}

//==========================================================================
// Enable the transmitter on the device

static void
stm32_serial_start_xmit(serial_channel *chan)
{
    stm32_serial_info * const stm32_chan = (stm32_serial_info *) chan->dev_priv;
    const CYG_ADDRWORD base = stm32_chan->base;
    cyg_uint32 cr1;

    if( !stm32_chan->tx_active )
    {
        stm32_chan->tx_active = true;
        HAL_READ_UINT32( base + CYGHWR_HAL_STM32_UART_CR1, cr1 );
        cr1 |= CYGHWR_HAL_STM32_UART_CR1_TXEIE;
        HAL_WRITE_UINT32( base + CYGHWR_HAL_STM32_UART_CR1, cr1 );
    }

}

//==========================================================================
// Disable the transmitter on the device

static void 
stm32_serial_stop_xmit(serial_channel *chan)
{
    stm32_serial_info * const stm32_chan = (stm32_serial_info *) chan->dev_priv;
    const CYG_ADDRWORD base = stm32_chan->base;
    cyg_uint32 cr1;

    if( stm32_chan->tx_active )
    {
        stm32_chan->tx_active = false;
        HAL_READ_UINT32( base + CYGHWR_HAL_STM32_UART_CR1, cr1 );
        cr1 &= ~CYGHWR_HAL_STM32_UART_CR1_TXEIE;
        HAL_WRITE_UINT32( base + CYGHWR_HAL_STM32_UART_CR1, cr1 );
    }

}

//==========================================================================
// Serial I/O - low level interrupt handler (ISR)
//
// This ISR does rather more than other serial driver ISRs. Normally,
// the ISR just masks the interrupt vector and schedules the DSR,
// which then handles all IO. However, if the processor is running out
// of external RAM it is too slow to handle higher baud rates using
// that technique. Something that is exacerbated by the lack of FIFOs
// in the USART hardware.
//
// Instead, this ISR receives any incoming data into a circular
// buffer, essentially providing the FIFO lacking in the
// hardware. Transmission is still offloaded to the DSR. Only TX
// interrupts are masked while this is done to prevent an interrupt
// loop, and to avoid blocking RX interrupts.

static cyg_uint32 
stm32_serial_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel * const chan = (serial_channel *) data;
    stm32_serial_info * const stm32_chan = (stm32_serial_info *) chan->dev_priv;
    const CYG_ADDRWORD base = stm32_chan->base;
    cyg_uint32 stat;
    cyg_uint32 ret = CYG_ISR_HANDLED;
    cyg_drv_interrupt_acknowledge(vector);

    HAL_READ_UINT32(base + CYGHWR_HAL_STM32_UART_SR, stat);
    
    if( stat & CYGHWR_HAL_STM32_UART_SR_RXNE )
    {
        cyg_uint32 c;

        while( stat & CYGHWR_HAL_STM32_UART_SR_RXNE )
        {
            int next = stm32_chan->buf_head+1;

            if( next == STM32_RXBUFSIZE ) next = 0;
        
            HAL_READ_UINT32( base + CYGHWR_HAL_STM32_UART_DR, c);
        
            if( next != stm32_chan->buf_tail )
            {
                stm32_chan->buf[stm32_chan->buf_head] = c&0xFF;
                stm32_chan->buf_head = next;
                ret |= CYG_ISR_CALL_DSR;
            }
            else
            {
                // TODO: deal with buffer overflow
            }

            HAL_READ_UINT32(base + CYGHWR_HAL_STM32_UART_SR, stat);
        }
    }
    else if( stat & CYGHWR_HAL_STM32_UART_SR_TXE )
    {
        cyg_uint32 cr1;
        HAL_READ_UINT32( base + CYGHWR_HAL_STM32_UART_CR1, cr1 );
        cr1 &= ~CYGHWR_HAL_STM32_UART_CR1_TXEIE;
        HAL_WRITE_UINT32( base + CYGHWR_HAL_STM32_UART_CR1, cr1 );
        
        ret |= CYG_ISR_CALL_DSR;
    }


    if( stat & CYGHWR_HAL_STM32_UART_SR_CTS )
    {
        // Clear CTS status if we see it.
        stat &= ~CYGHWR_HAL_STM32_UART_SR_CTS;
        HAL_WRITE_UINT32( base + CYGHWR_HAL_STM32_UART_SR, stat );
    }
    
    if( stat & (CYGHWR_HAL_STM32_UART_SR_FE|CYGHWR_HAL_STM32_UART_SR_NE|CYGHWR_HAL_STM32_UART_SR_ORE) )
    {
        // TODO: Handle hardware errors
    }
            
    return ret;
}

//==========================================================================
// Serial I/O - high level interrupt handler (DSR)

static void       
stm32_serial_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel * const chan = (serial_channel *) data;
    stm32_serial_info * const stm32_chan = (stm32_serial_info *) chan->dev_priv;
    const CYG_ADDRWORD base = stm32_chan->base;
    CYG_WORD32 stat;

    while( stm32_chan->buf_head != stm32_chan->buf_tail )
    {
        int next = stm32_chan->buf_tail+1;
        cyg_uint8 c;

        if( next == STM32_RXBUFSIZE ) next = 0;
        c = stm32_chan->buf[stm32_chan->buf_tail];
        stm32_chan->buf_tail = next;
        
        (chan->callbacks->rcv_char)(chan, c);
    }

    HAL_READ_UINT32(base + CYGHWR_HAL_STM32_UART_SR, stat);
    
    if( stm32_chan->tx_active && stat & CYGHWR_HAL_STM32_UART_SR_TXE )
    {
        cyg_uint32 cr1;

        (chan->callbacks->xmt_char)(chan);

        if( stm32_chan->tx_active )
        {
            HAL_READ_UINT32( base + CYGHWR_HAL_STM32_UART_CR1, cr1 );
            cr1 |= CYGHWR_HAL_STM32_UART_CR1_TXEIE;
            HAL_WRITE_UINT32( base + CYGHWR_HAL_STM32_UART_CR1, cr1 );
        }
    }
}

//==========================================================================
#endif // CYGPKG_IO_SERIAL_CORTEXM_STM32
// end of stm32_serial.c
