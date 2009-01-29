//==========================================================================
//
//      ser_mcfxxxx.c
//
//      Serial driver for Freescale coldfire processors
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2003, 2004, 2006, 2008 Free Software Foundation, Inc.      
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
// Author(s):    bartv
// Contributors: bartv
// Date:         2003-06-04
// Purpose:      support coldfire on-chip uart's
// Description:  The various coldfire mcfxxxx processors all use the same
//               basic UART. There are some variations, e.g. different
//               fifo sizes, autobaud capability, and calculating baud
//               rates requires platform-specific knowledge such as the
//               cpu speed. Also there is no standardization of base
//               addresses or interrupt vectors. Never the less a single
//               driver should be able to support most devices, with
//               various processor-specific or platform-specific #define's
//               and other support.
//
//####DESCRIPTIONEND####
//==========================================================================

// NOTE: some platforms may use GPIO pins for other modem lines such as
// ring and DSR/DTR/DCD. This code could check for #ifdef HAL_MCF52xx_UART_SET_DCD()
// and incorporate support from the platform HAL.

#include <pkgconf/system.h>
#include <pkgconf/io_serial.h>
#include CYGBLD_HAL_VARIANT_H
#include CYGBLD_HAL_PROC_H
#include CYGBLD_HAL_PLATFORM_H
#include <pkgconf/devs_serial_mcfxxxx.h>

#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/serial.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_io.h>

//#define MCFxxxx_SERIAL_STATS    1
#undef MCFxxxx_SERIAL_STATS

#ifdef MCFxxxx_SERIAL_STATS
# define INCR_STAT(_info_, _field_, _amount_)           \
    CYG_MACRO_START                                     \
    (_info_)->_field_ += _amount_;                      \
    CYG_MACRO_END
#else
# define INCR_STAT(_info_, _field_, _amount_)           \
    CYG_MACRO_START                                     \
    CYG_MACRO_END
#endif
    
// ----------------------------------------------------------------------------
// devtab entries for the supported devices.

static bool             mcfxxxx_serial_init(struct cyg_devtab_entry*);
static Cyg_ErrNo        mcfxxxx_serial_lookup(struct cyg_devtab_entry**, struct cyg_devtab_entry*, const char*);
static Cyg_ErrNo        mcfxxxx_serial_set_config(serial_channel*, cyg_uint32, const void*, cyg_uint32*);
static bool             mcfxxxx_serial_putc(serial_channel*, unsigned char);
static unsigned char    mcfxxxx_serial_getc(serial_channel*);
static void             mcfxxxx_serial_start_xmit(serial_channel*);
static void             mcfxxxx_serial_stop_xmit(serial_channel*);
static cyg_uint32       mcfxxxx_serial_isr(cyg_vector_t, cyg_addrword_t);
static void             mcfxxxx_serial_dsr(cyg_vector_t, cyg_ucount32, cyg_addrword_t);

typedef struct mcfxxxx_serial_info {
    cyg_uint8*      base;
    cyg_vector_t    isr_vec;
    int             isr_priority;
    cyg_uint8       uimr_shadow;
    cyg_uint8       umr1_shadow;
    cyg_uint8       umr2_shadow;
    cyg_uint8       flags;
    cyg_interrupt   serial_interrupt;
    cyg_handle_t    serial_interrupt_handle;
#ifdef MCFxxxx_SERIAL_STATS
    cyg_uint32      isr_count;
    cyg_uint32      dsr_count;
    cyg_uint32      rx_bytes;
    cyg_uint32      tx_bytes;
    cyg_uint32      rx_errors;
#endif    
} mcfxxxx_serial_info;

#define MCFxxxx_SERIAL_RTS              (0x01 << 0)
#define MCFxxxx_SERIAL_CTS              (0x01 << 1)
#define MCFxxxx_SERIAL_RS485_RTS        (0x01 << 2)

static SERIAL_FUNS(mcfxxxx_serial_funs,
                   mcfxxxx_serial_putc,
                   mcfxxxx_serial_getc,
                   mcfxxxx_serial_set_config,
                   mcfxxxx_serial_start_xmit,
                   mcfxxxx_serial_stop_xmit
    );

                   
#ifdef CYGPKG_DEVS_SERIAL_MCFxxxx_SERIAL0
static mcfxxxx_serial_info  mcfxxxx_serial0_info = {
    base:           (cyg_uint8*)HAL_MCFxxxx_UART0_BASE,
    isr_vec:        CYGNUM_HAL_ISR_UART0,
    isr_priority:   CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL0_ISR_PRIORITY,
#ifdef MCFxxxx_SERIAL_STATS
    isr_count:      0,
    dsr_count:      0,
    rx_bytes:       0,
    tx_bytes:       0,
    rx_errors:      0,
#endif
    flags:
#if defined(CYGHWR_HAL_M68K_MCFxxxx_UART0_RS485_RTS)
                    MCFxxxx_SERIAL_RS485_RTS |
#elif defined(CYGHWR_HAL_M68K_MCFxxxx_UART0_RTS)
                    MCFxxxx_SERIAL_RTS |
#endif    
#if defined(CYGHWR_HAL_M68K_MCFxxxx_UART0_CTS)
                    MCFxxxx_SERIAL_CTS |
#endif    
                    0x00
};

# ifdef CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL0_BUFSIZE
static unsigned char    mcfxxxx_serial0_tx_buf[CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL0_BUFSIZE];
static unsigned char    mcfxxxx_serial0_rx_buf[CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL0_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(mcfxxxx_serial0_chan,
                                       mcfxxxx_serial_funs, 
                                       mcfxxxx_serial0_info,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL0_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       mcfxxxx_serial0_tx_buf, CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL0_BUFSIZE,
                                       mcfxxxx_serial0_rx_buf, CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL0_BUFSIZE
    );
#else
static SERIAL_CHANNEL(mcfxxxx_serial0_chan,
                      mcfxxxx_serial_funs,
                      mcfxxxx_serial0_info,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL0_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
# endif

DEVTAB_ENTRY(mcfxxxx_serial0_devtab, 
             CYGDAT_DEVS_SERIAL_MCFxxxx_SERIAL0_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             mcfxxxx_serial_init, 
             mcfxxxx_serial_lookup,     // Serial driver may need initializing
             &mcfxxxx_serial0_chan
    );
#endif

#ifdef CYGPKG_DEVS_SERIAL_MCFxxxx_SERIAL1
static mcfxxxx_serial_info  mcfxxxx_serial1_info = {
    base:           (cyg_uint8*)HAL_MCFxxxx_UART1_BASE,
    isr_vec:        CYGNUM_HAL_ISR_UART1,
    isr_priority:   CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL1_ISR_PRIORITY,
#ifdef MCFxxxx_SERIAL_STATS
    isr_count:      0,
    dsr_count:      0,
    rx_bytes:       0,
    tx_bytes:       0,
    rx_errors:      0,
#endif    
    flags:
#if defined(CYGHWR_HAL_M68K_MCFxxxx_UART1_RS485_RTS)
                    MCFxxxx_SERIAL_RS485_RTS |
#elif defined(CYGHWR_HAL_M68K_MCFxxxx_UART1_RTS)
                    MCFxxxx_SERIAL_RTS |
#endif    
#if defined(CYGHWR_HAL_M68K_MCFxxxx_UART1_CTS)
                    MCFxxxx_SERIAL_CTS |
#endif    
                    0x00
};

# ifdef CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL1_BUFSIZE
static unsigned char    mcfxxxx_serial1_tx_buf[CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL1_BUFSIZE];
static unsigned char    mcfxxxx_serial1_rx_buf[CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL1_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(mcfxxxx_serial1_chan,
                                       mcfxxxx_serial_funs, 
                                       mcfxxxx_serial1_info,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL1_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       mcfxxxx_serial1_tx_buf, CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL1_BUFSIZE,
                                       mcfxxxx_serial1_rx_buf, CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL1_BUFSIZE
    );
#else
static SERIAL_CHANNEL(mcfxxxx_serial1_chan,
                      mcfxxxx_serial_funs,
                      mcfxxxx_serial1_info,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL1_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
# endif

DEVTAB_ENTRY(mcfxxxx_serial1_devtab, 
             CYGDAT_DEVS_SERIAL_MCFxxxx_SERIAL1_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             mcfxxxx_serial_init, 
             mcfxxxx_serial_lookup,     // Serial driver may need initializing
             &mcfxxxx_serial1_chan
    );
#endif

#ifdef CYGPKG_DEVS_SERIAL_MCFxxxx_SERIAL2
static mcfxxxx_serial_info  mcfxxxx_serial2_info = {
    base:           (cyg_uint8*)HAL_MCFxxxx_UART2_BASE,
    isr_vec:        CYGNUM_HAL_ISR_UART2,
    isr_priority:   CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL2_ISR_PRIORITY,
#ifdef MCFxxxx_SERIAL_STATS
    isr_count:      0,
    dsr_count:      0,
    rx_bytes:       0,
    tx_bytes:       0,
    rx_errors:      0,
#endif    
    flags:
#if defined(CYGHWR_HAL_M68K_MCFxxxx_UART2_RS485_RTS)
                    MCFxxxx_SERIAL_RS485_RTS |
#elif defined(CYGHWR_HAL_M68K_MCFxxxx_UART2_RTS)
                    MCFxxxx_SERIAL_RTS |
#endif    
#if defined(CYGHWR_HAL_M68K_MCFxxxx_UART2_CTS)
                    MCFxxxx_SERIAL_CTS |
#endif    
                    0x00
};

# ifdef CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL2_BUFSIZE
static unsigned char    mcfxxxx_serial2_tx_buf[CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL2_BUFSIZE];
static unsigned char    mcfxxxx_serial2_rx_buf[CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL2_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(mcfxxxx_serial2_chan,
                                       mcfxxxx_serial_funs, 
                                       mcfxxxx_serial2_info,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL2_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       mcfxxxx_serial2_tx_buf, CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL2_BUFSIZE,
                                       mcfxxxx_serial2_rx_buf, CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL2_BUFSIZE
    );
#else
static SERIAL_CHANNEL(mcfxxxx_serial2_chan,
                      mcfxxxx_serial_funs,
                      mcfxxxx_serial2_info,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_DEVS_SERIAL_MCFxxxx_SERIAL2_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
# endif

DEVTAB_ENTRY(mcfxxxx_serial2_devtab, 
             CYGDAT_DEVS_SERIAL_MCFxxxx_SERIAL2_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             mcfxxxx_serial_init, 
             mcfxxxx_serial_lookup,     // Serial driver may need initializing
             &mcfxxxx_serial2_chan
    );
#endif

// ----------------------------------------------------------------------------

static cyg_uint32   mcfxxxx_baud_rates[] = {
    0,      // Unused
    50,     // 50
    75,     // 75
    110,    // 110
    134,    // 134.5
    150,    // 150
    200,    // 200
    300,    // 300
    600,    // 600
    1200,   // 1200
    1800,   // 1800
    2400,   // 2400
    3600,   // 3600
    4800,   // 4800
    7200,   // 7200
    9600,   // 9600
    14400,  // 14400
    19200,  // 19200
    38400,  // 38400
    57600,  // 57600
    115200, // 115200
    230400  // 230400
};

static bool
mcfxxxx_serial_config(serial_channel* chan, cyg_serial_info_t* config, cyg_bool init)
{
    mcfxxxx_serial_info*    info    = (mcfxxxx_serial_info*) chan->dev_priv;
    cyg_uint8*              base    = info->base;

    if (init) {
#if defined(CYGPKG_DEVS_SERIAL_MCFxxxx_SERIAL0) && defined(HAL_MCFxxxx_UART0_PROC_INIT)
        if (info == &mcfxxxx_serial0_info) {
            HAL_MCFxxxx_UART0_PROC_INIT();
        }
#endif
#if defined(CYGPKG_DEVS_SERIAL_MCFxxxx_SERIAL1) && defined(HAL_MCFxxxx_UART1_PROC_INIT)
        if (info == &mcfxxxx_serial1_info) {
            HAL_MCFxxxx_UART1_PROC_INIT();
        }
#endif
#if defined(CYGPKG_DEVS_SERIAL_MCFxxxx_SERIAL2) && defined(HAL_MCFxxxx_UART2_PROC_INIT)
        if (info == &mcfxxxx_serial2_info) {
            HAL_MCFxxxx_UART2_PROC_INIT();
        }
#endif
        // Various resets to get the UART in a known good state
        HAL_WRITE_UINT8(&(base[HAL_MCFxxxx_UARTx_UCR]), HAL_MCFxxxx_UARTx_UCR_MISC_RR);
        HAL_WRITE_UINT8(&(base[HAL_MCFxxxx_UARTx_UCR]), HAL_MCFxxxx_UARTx_UCR_MISC_RT);
        HAL_WRITE_UINT8(&(base[HAL_MCFxxxx_UARTx_UCR]), HAL_MCFxxxx_UARTx_UCR_MISC_RES);
        HAL_WRITE_UINT8(&(base[HAL_MCFxxxx_UARTx_UCR]), HAL_MCFxxxx_UARTx_UCR_MISC_RBCI);

        // Initialize the interrupt mask register. We want to trigger on rxrdy() and
        // optionally on breaks. Tx interrupts are not enabled by default, only
        // when a transmit is in progress.
        //
        // Some processors may define HAL_MCFxxxx_UARTx_UIMR_RXFTO which can be
        // used instead of RXRDY, getting an interrupt only when the fifo is full
        // or when 64 bit times have elapsed without new data. This reduces the
        // number of rx interrupts by e.g. a factor of 12. It is not without
        // penalty: if higher-level code could start processing data before the
        // fifo has filled up then the latency is increased significantly; even
        // if a whole packet needs to be received first, unless the packet size
        // maps cleanly on to fifo boundaries the latency is increased by the
        // timeout; if software flow control is in use then this side may not
        // respond to XON/XOFF bytes for a while. For now rx fifos are used
        // by default if available, although this should probably be made configurable.
        info->uimr_shadow = 0;
        if (chan->out_cbuf.len != 0) {
# if defined(HAL_MCFxxxx_UARTx_UIMR_RXFIFO) && defined(HAL_MCFxxxx_UARTx_UIMR_RXFTO) && defined(HAL_MCFxxxx_UARTx_URF)
            info->uimr_shadow = HAL_MCFxxxx_UARTx_UIMR_RXFTO | HAL_MCFxxxx_UARTx_UIMR_RXFIFO;
# else            
            info->uimr_shadow = HAL_MCFxxxx_UARTx_UIMR_RXRDY;
#endif            
        }
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
        info->uimr_shadow |= HAL_MCFxxxx_UARTx_UIMR_DB;
#endif
        HAL_WRITE_UINT8(&(base[HAL_MCFxxxx_UARTx_UIMR]), info->uimr_shadow);

        // If the hardware supports tx fifo control, set it up so that
        // interrupts only occur when the fifo is more than 75% empty.
        // That cuts down on the number of interrupts without
        // affecting performance. The processor should service the interrupt
        // and replenish the fifo before the remaining bytes go out.
#ifdef HAL_MCFxxxx_UARTx_UTF
        HAL_WRITE_UINT8(&(base[HAL_MCFxxxx_UARTx_UTF]), HAL_MCFxxxx_UARTx_UTF_TXS_75);
#endif
        // Ditto for rx fifo, but trigger on 50%. That is a compromise between
        // latency and efficiency.
#ifdef HAL_MCFxxxx_UARTx_URF
        HAL_WRITE_UINT8(&(base[HAL_MCFxxxx_UARTx_URF]), HAL_MCFxxxx_UARTx_URF_RXS_50);
#endif        
        // Always use the internal prescaled CLKIN.
        HAL_WRITE_UINT8(&(base[HAL_MCFxxxx_UARTx_UCSR]), HAL_MCFxxxx_UARTx_UCSR_RCS_CLKIN | HAL_MCFxxxx_UARTx_UCSR_TCS_CLKIN);

        // Hardware flow control.
        //
        // Default: no TXRTS, no TXCTS, no RXRTS, no configurable RTS fifo level
        info->umr1_shadow   = 0x00;
        info->umr2_shadow   = 0x00;
        HAL_WRITE_UINT8(&(base[HAL_MCFxxxx_UARTx_UACR]), 0x00);

        // CTS, used to throttle the transmitter automatically. This involves
        // setting the TXCTS bit. However it is not the default, h/w flow control
        // has to be explicitly enabled by a set_config() call.
        
        // RTS. This may not be connected at all, or it may be used
        // for h/w control of an RS485 transceiver, or it may be used
        // for RS232 handshaking. If the latter then the uart provides
        // automatic support for throttling the other side when the
        // fifo starts filling up.
        if (info->flags & MCFxxxx_SERIAL_RS485_RTS) {
            info->umr2_shadow   = HAL_MCFxxxx_UARTx_UMR2_TXRTS;
        } else if (info->flags & MCFxxxx_SERIAL_RTS) {
            // RS232 h/w flow control.
            // See if the processor supports configurable RTS levels.
# ifdef HAL_MCFxxxx_UARTx_UACR_RTSL_25
            // Set up RTS to change when the fifo is 25% full. This means the
            // processor can accept another 18 bytes, more than the 16-byte
            // transmit fifo in a typical PC uart. Increasing the RTS level to
            // any more than this may cause overruns.
            HAL_WRITE_UINT8(&(base[HAL_MCFxxxx_UARTx_UACR]), HAL_MCFxxxx_UARTx_UACR_RTSL_25);
# else
            // Only RxRTS mode is supported, so use it.
            info->umr1_shadow   = HAL_MCFxxxx_UARTx_UMR1_RXRTS;
# endif
            // If RTS is connected assert it here, allowing the other side to transmit
            // data. This may be too early since the h/w is not fully set up yet, but
            // we only want to do this during init.
            HAL_WRITE_UINT8(&(base[HAL_MCFxxxx_UARTx_UOP1]), HAL_MCFxxxx_UARTx_UOP_RTS);
        } else {
            // RTS is not connected at all.
        }

        // Enable both RX and TX
        HAL_WRITE_UINT8(&(base[HAL_MCFxxxx_UARTx_UCR]), HAL_MCFxxxx_UARTx_UCR_TC_TE | HAL_MCFxxxx_UARTx_UCR_RC_RE);
    }
    
    info->umr1_shadow   &= ~(HAL_MCFxxxx_UARTx_UMR1_BC_MASK | HAL_MCFxxxx_UARTx_UMR1_PM_MASK);
    switch (config->word_length) {
      case CYGNUM_SERIAL_WORD_LENGTH_5:
        info->umr1_shadow |= HAL_MCFxxxx_UARTx_UMR1_BC_5;
        break;
      case CYGNUM_SERIAL_WORD_LENGTH_6:
        info->umr1_shadow |= HAL_MCFxxxx_UARTx_UMR1_BC_6;
        break;
      case CYGNUM_SERIAL_WORD_LENGTH_7:
        info->umr1_shadow |= HAL_MCFxxxx_UARTx_UMR1_BC_7;
        break;
      case CYGNUM_SERIAL_WORD_LENGTH_8:
      default:
        info->umr1_shadow |= HAL_MCFxxxx_UARTx_UMR1_BC_8;
        break;
    }
    switch (config->parity) {
      case CYGNUM_SERIAL_PARITY_EVEN:
        info->umr1_shadow |= HAL_MCFxxxx_UARTx_UMR1_PM_WITH;
        break;
      case CYGNUM_SERIAL_PARITY_ODD:
        info->umr1_shadow |= HAL_MCFxxxx_UARTx_UMR1_PM_WITH | HAL_MCFxxxx_UARTx_UMR1_PT;
        break;
      case CYGNUM_SERIAL_PARITY_MARK:
        info->umr1_shadow |= HAL_MCFxxxx_UARTx_UMR1_PM_FORCE  | HAL_MCFxxxx_UARTx_UMR1_PT;
        break;
      case CYGNUM_SERIAL_PARITY_SPACE:
        info->umr1_shadow |= HAL_MCFxxxx_UARTx_UMR1_PM_FORCE;
        break;
      case CYGNUM_SERIAL_PARITY_NONE:
      default:
        info->umr1_shadow |= HAL_MCFxxxx_UARTx_UMR1_PM_NO;
        break;
    }
    info->umr2_shadow &= ~HAL_MCFxxxx_UARTx_UMR2_SB_MASK;
    switch (config->stop) {
      case CYGNUM_SERIAL_STOP_2:
        info->umr2_shadow |= HAL_MCFxxxx_UARTx_UMR2_SB_2;
        break;
      case CYGNUM_SERIAL_STOP_1_5:
        info->umr2_shadow |= (CYGNUM_SERIAL_WORD_LENGTH_5 == config->word_length) ? 0x07 : 0x08;
        break;
      case CYGNUM_SERIAL_STOP_1:
      default:
        info->umr2_shadow |= (CYGNUM_SERIAL_WORD_LENGTH_5 == config->word_length) ? 0x00 : HAL_MCFxxxx_UARTx_UMR2_SB_1;
        break;
    }
    
    HAL_WRITE_UINT8(&(base[HAL_MCFxxxx_UARTx_UCR]), HAL_MCFxxxx_UARTx_UCR_MISC_RMRP);
    HAL_WRITE_UINT8(&(base[HAL_MCFxxxx_UARTx_UMR]), info->umr1_shadow);
    HAL_WRITE_UINT8(&(base[HAL_MCFxxxx_UARTx_UMR]), info->umr2_shadow);

    // Set the baud rate, using a processor or platform macro. That way the
    // calculation can depend on the clock speed.
    HAL_MCFxxxx_UARTx_SET_BAUD(base, mcfxxxx_baud_rates[config->baud]);
  
    if (config != &chan->config) {
        chan->config = *config;
    }
    
    return true;
}
                      
// ----------------------------------------------------------------------------
static bool
mcfxxxx_serial_init(struct cyg_devtab_entry* devtab_entry)
{
    serial_channel*         chan    = (serial_channel*) devtab_entry->priv;
    mcfxxxx_serial_info*    info    = (mcfxxxx_serial_info*) chan->dev_priv;

    mcfxxxx_serial_config(chan, &(chan->config), true);
    
    if (0 != chan->out_cbuf.len) {
        cyg_drv_interrupt_create(info->isr_vec,
                                 info->isr_priority,
                                 (cyg_addrword_t) chan,
                                 &mcfxxxx_serial_isr,
                                 &mcfxxxx_serial_dsr,
                                 &(info->serial_interrupt_handle),
                                 &(info->serial_interrupt));
        cyg_drv_interrupt_attach(info->serial_interrupt_handle);
        cyg_drv_interrupt_unmask(info->isr_vec);
    }
    return true;
}

// ----------------------------------------------------------------------------
static Cyg_ErrNo
mcfxxxx_serial_lookup(struct cyg_devtab_entry** tab, struct cyg_devtab_entry* sub_tab, const char* name)
{
    serial_channel* chan    = (serial_channel*) (*tab)->priv;
    (chan->callbacks->serial_init)(chan);
    return ENOERR;
}

// ----------------------------------------------------------------------------
static Cyg_ErrNo
mcfxxxx_serial_set_config(serial_channel* chan, cyg_uint32 key, const void* buf, cyg_uint32* len)
{
    Cyg_ErrNo result    = ENOERR;
    
    switch(key) {
      case CYG_IO_SET_CONFIG_SERIAL_INFO:
        {
            mcfxxxx_serial_info*    info    = (mcfxxxx_serial_info*) chan->dev_priv;
            cyg_serial_info_t*  config = (cyg_serial_info_t*) buf;
            if (*len < sizeof(cyg_serial_info_t)) {
                return -EINVAL;
            }
            *len = sizeof(cyg_serial_info_t);
            // DSR/DTR is never supported.
            if (config->flags & (CYGNUM_SERIAL_FLOW_DSRDTR_RX | CYGNUM_SERIAL_FLOW_DSRDTR_TX)) {
                result = -ENOSUPP;
                config->flags &= ~(CYGNUM_SERIAL_FLOW_DSRDTR_RX | CYGNUM_SERIAL_FLOW_DSRDTR_TX);
            }
            // RTS/CTS may be supported, if the appropriate pins are connected.
            if ((config->flags & CYGNUM_SERIAL_FLOW_RTSCTS_RX) && !(info->flags & MCFxxxx_SERIAL_RTS)) {
                result = -ENOSUPP;
                config->flags &= ~CYGNUM_SERIAL_FLOW_RTSCTS_RX;
            }
            if ((config->flags & CYGNUM_SERIAL_FLOW_RTSCTS_TX) && !(info->flags & MCFxxxx_SERIAL_CTS)) {
                result = -ENOSUPP;
                config->flags &= ~CYGNUM_SERIAL_FLOW_RTSCTS_TX;
            }
            if (ENOERR == result) {
                if (! mcfxxxx_serial_config(chan, config, false)) {
                    result = -EINVAL;
                }
            }
            break;
        }
#ifdef CYGOPT_IO_SERIAL_FLOW_CONTROL_HW
      case CYG_IO_SET_CONFIG_SERIAL_HW_RX_FLOW_THROTTLE:
        {
            // RX flow control involves just the RTS line. Most of the
            // work is done by the hardware depending on the state of
            // the fifo. This option serves mainly to drop RTS if
            // higher-level code is running out of buffer space, even
            // if the fifo is not yet full.
            mcfxxxx_serial_info*    info    = (mcfxxxx_serial_info*) chan->dev_priv;
            cyg_uint32*             flag    = (cyg_uint32*) buf;
            if (! (info->flags & MCFxxxx_SERIAL_RTS)) {
                return -ENOSUPP;
            }
            if (*flag) {
                HAL_WRITE_UINT8(info->base + HAL_MCFxxxx_UARTx_UOP0, HAL_MCFxxxx_UARTx_UOP_RTS);
            } else {
                HAL_WRITE_UINT8(info->base + HAL_MCFxxxx_UARTx_UOP1, HAL_MCFxxxx_UARTx_UOP_RTS);
            }
        }
        break;
        
      case CYG_IO_SET_CONFIG_SERIAL_HW_FLOW_CONFIG:
        {
            mcfxxxx_serial_info*    info    = (mcfxxxx_serial_info*) chan->dev_priv;
                
            // DSR/DTR is never supported.
            if (chan->config.flags & (CYGNUM_SERIAL_FLOW_DSRDTR_RX | CYGNUM_SERIAL_FLOW_DSRDTR_TX)) {
                result = -ENOSUPP;
                chan->config.flags &= ~(CYGNUM_SERIAL_FLOW_DSRDTR_RX | CYGNUM_SERIAL_FLOW_DSRDTR_TX);
            }
            // RTS/CTS may be supported, if the appropriate pins are connected.
            if ((chan->config.flags & CYGNUM_SERIAL_FLOW_RTSCTS_RX) && !(info->flags & MCFxxxx_SERIAL_RTS)) {
                result = -ENOSUPP;
                chan->config.flags &= ~CYGNUM_SERIAL_FLOW_RTSCTS_RX;
            }
            if ((chan->config.flags & CYGNUM_SERIAL_FLOW_RTSCTS_TX) && !(info->flags & MCFxxxx_SERIAL_CTS)) {
                result = -ENOSUPP;
                chan->config.flags &= ~CYGNUM_SERIAL_FLOW_RTSCTS_TX;
            }

            // RTS flow control for RX. Either UMR1 RxRTS or a UACR RTS trigger
            // level has been set during initialization. There is little point
            // changing either of these. If h/w flow control is being disabled
            // then the other side should start ignoring the RTS signal, even
            // if this side still thinks it is a good idea to change it depending
            // on the fifo level.

            // CTS flow control for TX just involves the UMR2 TxCTS bit.
            if (0 != (chan->config.flags & CYGNUM_SERIAL_FLOW_RTSCTS_TX)) {
                info->umr2_shadow |= HAL_MCFxxxx_UARTx_UMR2_TXCTS;
            } else {
                info->umr2_shadow &= ~HAL_MCFxxxx_UARTx_UMR2_TXCTS;
            }
            HAL_WRITE_UINT8(info->base + HAL_MCFxxxx_UARTx_UCR, HAL_MCFxxxx_UARTx_UCR_MISC_RMRP);
            HAL_WRITE_UINT8(info->base + HAL_MCFxxxx_UARTx_UMR, info->umr1_shadow);
            HAL_WRITE_UINT8(info->base + HAL_MCFxxxx_UARTx_UMR, info->umr2_shadow);
        }
        break;
#endif
      default:
        return -EINVAL;
    }

    return result;
}

// ----------------------------------------------------------------------------
// Non-blocking send, returning true if the character was consumed. This can
// be called in both interrupt and polled mode.

static bool
mcfxxxx_serial_putc(serial_channel* chan, unsigned char ch)
{
    mcfxxxx_serial_info*    info    = (mcfxxxx_serial_info*) chan->dev_priv;
    cyg_uint8               usr;

    HAL_READ_UINT8(info->base + HAL_MCFxxxx_UARTx_USR, usr);
    if (usr & HAL_MCFxxxx_UARTx_USR_TXRDY) {
        HAL_WRITE_UINT8(info->base + HAL_MCFxxxx_UARTx_UTB, ch);
        INCR_STAT(info, tx_bytes, 1);
        return true;
    }
    return false;
}

// Blocking receive, only called in polled mode.

static unsigned char
mcfxxxx_serial_getc(serial_channel* chan)
{
    mcfxxxx_serial_info*    info    = (mcfxxxx_serial_info*) chan->dev_priv;
    cyg_uint8               usr, data;

    do {
        HAL_READ_UINT8(info->base + HAL_MCFxxxx_UARTx_USR, usr);
    } while (! (usr & HAL_MCFxxxx_UARTx_USR_RXRDY));
    HAL_READ_UINT8(info->base + HAL_MCFxxxx_UARTx_URB, data);
    INCR_STAT(info, rx_bytes, 1);
    return data;
}

// Start transmitting, only called in interrupt mode. This just requires
// unmasking tx interrupts, with the interrupt handling code doing the
// rest. The UIMR register is write-only so this has to go via a shadow
// copy.
//
// If the processor supports interrupting on TXFIFO then that is used
// instead, raising interrupts only if the fifo >= 75% empty.
//
// In RS485 mode it is necessary to enable RTS here so that the transceiver
// is no longer tristated. RTS will be dropped automatically at the end of the
// transmit. It is assumed that the fifo will be refilled quickly enough
// that RTS does not get dropped too soon. Arguably RTS should be raised
// in the fifo fill code, but that would introduce problems if another node
// has decided a timeout has occurred and it should start transmitting now.

static void
mcfxxxx_serial_start_xmit(serial_channel* chan)
{
    mcfxxxx_serial_info*    info    = (mcfxxxx_serial_info*) chan->dev_priv;
    CYG_INTERRUPT_STATE     saved_state;

    if (info->flags & MCFxxxx_SERIAL_RS485_RTS) {
        HAL_WRITE_UINT8(info->base + HAL_MCFxxxx_UARTx_UOP1, HAL_MCFxxxx_UARTx_UOP_RTS);
    }
    
    HAL_DISABLE_INTERRUPTS(saved_state);
#ifdef HAL_MCFxxxx_UARTx_UIMR_TXFIFO
    info->uimr_shadow |= HAL_MCFxxxx_UARTx_UIMR_TXFIFO;
#else    
    info->uimr_shadow |= HAL_MCFxxxx_UARTx_UIMR_TXRDY;
#endif    
    HAL_WRITE_UINT8(info->base + HAL_MCFxxxx_UARTx_UIMR, info->uimr_shadow);
    HAL_RESTORE_INTERRUPTS(saved_state);
}

// Stop transmitting, only called in interrupt mode.
static void
mcfxxxx_serial_stop_xmit(serial_channel* chan)
{
    mcfxxxx_serial_info*    info    = (mcfxxxx_serial_info*) chan->dev_priv;
    CYG_INTERRUPT_STATE     saved_state;

    HAL_DISABLE_INTERRUPTS(saved_state);
#ifdef HAL_MCFxxxx_UARTx_UIMR_TXFIFO
    info->uimr_shadow &= ~HAL_MCFxxxx_UARTx_UIMR_TXFIFO;
#else    
    info->uimr_shadow &= ~HAL_MCFxxxx_UARTx_UIMR_TXRDY;
#endif    
    HAL_WRITE_UINT8(info->base + HAL_MCFxxxx_UARTx_UIMR, info->uimr_shadow);
    HAL_RESTORE_INTERRUPTS(saved_state);
}

// ----------------------------------------------------------------------------
// The main serial I/O callbacks expect to be called in DSR context, not
// ISR context, so it is not possible to do much processing in the ISR.
// Instead everything is deferred to the DSR.

static cyg_uint32
mcfxxxx_serial_isr(cyg_vector_t vec, cyg_addrword_t data)
{
    serial_channel*         chan    = (serial_channel*) data;
    mcfxxxx_serial_info*    info    = (mcfxxxx_serial_info*) chan->dev_priv;
    HAL_WRITE_UINT8(info->base + HAL_MCFxxxx_UARTx_UIMR, 0);

    INCR_STAT(info, isr_count, 1);
    
    return CYG_ISR_CALL_DSR;
}

// ----------------------------------------------------------------------------
static void
mcfxxxx_serial_dsr(cyg_vector_t vec, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel*         chan    = (serial_channel*) data;
    mcfxxxx_serial_info*    info    = (mcfxxxx_serial_info*) chan->dev_priv;
    cyg_uint8               uisr;

    INCR_STAT(info, dsr_count, 1);
    
    HAL_READ_UINT8(info->base + HAL_MCFxxxx_UARTx_UISR, uisr);

#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
    // This is not quite right, it will report a break event instead of a delta-break,
    // so higher-level code will see two breaks instead of start-break and end-break.
    // In practice that should be good enough.
    //
    // There is also a received-break bit in the usr register, indicating that a
    // break occurred in the middle of a character.
    if (uisr & HAL_MCFxxxx_UARTx_UISR_DB) {
        cyg_serial_line_status_t    stat;
        HAL_WRITE_UINT8(info->base + HAL_MCFxxxx_UARTx_UCR, HAL_MCFxxxx_UARTx_UCR_MISC_RBCI);
        stat.value  = 1;
        stat.which  = CYGNUM_SERIAL_STATUS_BREAK;
        (chan->callbacks->indicate_status)(chan, &stat);
    }
#endif

    // Do not report CTS changes to higher-level code. There is no point since flow
    // control should be handled by the hardware.
    
    if (uisr & HAL_MCFxxxx_UARTx_UISR_RXRDY) {
        cyg_uint8 usr, data;
        while (1) {
            HAL_READ_UINT8(info->base + HAL_MCFxxxx_UARTx_USR, usr);

            if (! (usr & HAL_MCFxxxx_UARTx_USR_RXRDY)) {
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS                
                // Now check for an overrun, so that the error is
                // reported in approximately the right place in the
                // data stream. It is possible that an extra byte
                // or so has come in after the overrun, but that
                // cannot be detected.
                if (usr & HAL_MCFxxxx_UARTx_USR_OE) {
                    cyg_serial_line_status_t    stat;
                    HAL_WRITE_UINT8(info->base + HAL_MCFxxxx_UARTx_UCR, HAL_MCFxxxx_UARTx_UCR_MISC_RES);
                    stat.value  = 1;
                    stat.which  = CYGNUM_SERIAL_STATUS_OVERRUNERR;
                    (chan->callbacks->indicate_status)(chan, &stat);
                    INCR_STAT(info, rx_errors, 1);
                }
#endif
                // There is no more data in the fifo, so look for transmits.
                break;
            }

            // RXRDY is set, so we have either a valid or a corrupted byte
            // in the current fifo position. First pass the byte up the stack,
            // then report the error.
            HAL_READ_UINT8(info->base + HAL_MCFxxxx_UARTx_URB, data);
            (chan->callbacks->rcv_char)(chan, data);
            INCR_STAT(info, rx_bytes, 1);
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
            if (usr & HAL_MCFxxxx_UARTx_USR_FE) {
                cyg_serial_line_status_t    stat;
                stat.value = 1;
                stat.which  = CYGNUM_SERIAL_STATUS_FRAMEERR;
                (chan->callbacks->indicate_status)(chan, &stat);
                INCR_STAT(info, rx_errors, 1);
            }
            if (usr & HAL_MCFxxxx_UARTx_USR_PE) {
                cyg_serial_line_status_t    stat;
                stat.value = 1;
                stat.which  = CYGNUM_SERIAL_STATUS_PARITYERR;
                (chan->callbacks->indicate_status)(chan, &stat);
                INCR_STAT(info, rx_errors, 1);
            }
#endif
        }
    }
    
    if (uisr & HAL_MCFxxxx_UARTx_UISR_TXRDY) {
        (chan->callbacks->xmt_char)(chan);
    }
    
    // Re-enable UART interrupts
    HAL_WRITE_UINT8(info->base + HAL_MCFxxxx_UARTx_UIMR, info->uimr_shadow);
}
