//==========================================================================
//
//      mn10300_serial.c
//
//      Serial device driver for mn10300 on-chip serial devices
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg
// Contributors: nickg
// Date:        1999-02-25
// Purpose:     MN10300 serial device driver
// Description: MN10300 serial device driver
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/io_serial.h>
#include <cyg/hal/hal_io.h>

#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/serial.h>
#include <cyg/hal/hal_intr.h>

#ifdef CYGPKG_IO_SERIAL_MN10300

//-------------------------------------------------------------------------

extern void diag_printf(const char *fmt, ...);

//-------------------------------------------------------------------------
// Forward definitions

static bool mn10300_serial_init(struct cyg_devtab_entry *tab);
static bool mn10300_serial_putc(serial_channel *chan, unsigned char c);
static Cyg_ErrNo mn10300_serial_lookup(struct cyg_devtab_entry **tab, 
                                   struct cyg_devtab_entry *sub_tab,
                                   const char *name);
static unsigned char mn10300_serial_getc(serial_channel *chan);
static bool mn10300_serial_set_config(serial_channel *chan, cyg_serial_info_t *config);
static void mn10300_serial_start_xmit(serial_channel *chan);
static void mn10300_serial_stop_xmit(serial_channel *chan);

#ifndef CYGPKG_IO_SERIAL_MN10300_POLLED_MODE
static cyg_uint32 mn10300_serial_rx_ISR(cyg_vector_t vector, cyg_addrword_t data);
static cyg_uint32 mn10300_serial_tx_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       mn10300_serial_rx_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
static void       mn10300_serial_tx_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
#endif

//-------------------------------------------------------------------------

#define BUFSIZE 128

//-------------------------------------------------------------------------
// MN10300 serial line control register values:

// Offsets to serial control registers from base
#define SERIAL_CTR      0x0
#define SERIAL_ICR      0x4
#define SERIAL_TXB      0x8
#define SERIAL_RXB      0x9
#define SERIAL_STR      0xc
#define SERIAL_TIM      0xd

// Status register bits
#define SR_RBF          0x10
#define SR_TBF          0x20
#define SR_RXF          0x40
#define SR_TXF          0x80

// Control register bits
#define LCR_SB1         0x00
#define LCR_SB1_5       0x00
#define LCR_SB2         0x04
#define LCR_PN          0x00    // Parity mode - none
#define LCR_PS          0x40    // Forced "space" parity
#define LCR_PM          0x50    // Forced "mark" parity
#define LCR_PE          0x60    // Parity mode - even
#define LCR_PO          0x70    // Parity mode - odd
#define LCR_WL5         0x00    // not supported - use 7bit
#define LCR_WL6         0x00    // not supported - use 7bit
#define LCR_WL7         0x00    // 7 bit chars
#define LCR_WL8         0x80    // 8 bit chars
#define LCR_RXE         0x4000  // receive enable
#define LCR_TXE         0x8000  // transmit enable
#define LCR_TWE         0x0100  // interrupt enable (only on serial2)

//-------------------------------------------------------------------------
// MN10300 timer registers:

#undef TIMER_BR
#undef TIMER_MD
#define TIMER_MD        0x00
#define TIMER_BR        0x10

//-------------------------------------------------------------------------
// IO Port:

#define PORT3_MD        0x36008025

//-------------------------------------------------------------------------
// Tables to map input values to hardware settings

static unsigned char select_word_length[] = {
    LCR_WL5,    // 5 bits / word (char)
    LCR_WL6,
    LCR_WL7,
    LCR_WL8
};

static unsigned char select_stop_bits[] = {
    0,
    LCR_SB1,    // 1 stop bit
    LCR_SB1_5,  // 1.5 stop bit
    LCR_SB2     // 2 stop bits
};

static unsigned char select_parity[] = {
    LCR_PN,     // No parity
    LCR_PE,     // Even parity
    LCR_PO,     // Odd parity
    LCR_PM,     // Mark parity
    LCR_PS,     // Space parity
};

static unsigned short select_baud_01[] = {
    0,          // Unused
    0,          // 50
    0,          // 75
    0,          // 110
    0,          // 134.5
    0,          // 150
    0,          // 200
    0,          // 300
    0,          // 600
    0,          // 1200
    0,          // 1800
    0,          // 2400
    0,          // 3600
    0,          // 4800
    0,          // 7200
    195,        // 9600
    130,        // 14400
    98,         // 19200
    48,         // 38400
    32,         // 57600
    16,         // 115200
    8,          // 230400
};

// Serial 2 has its own timer register in addition to using timer 2 to
// supply the baud rate generator. Both of these must be proframmed to
// get the right baud rate. The following values come from Matsushita
// with some modifications from Cygmon.
static struct
{
    cyg_uint8   serial2_val;
    cyg_uint8   timer2_val;
} select_baud_2[] = {
    {   0,   0 },       // Unused
    {   0,   0 },       // 50
    {   0,   0 },       // 75
    {   0,   0 },       // 110
    {   0,   0 },       // 134.5
    {   0,   0 },       // 150
    {   0,   0 },       // 200
    {   0,   0 },       // 300
    { 126, 196 },       // 600
    { 125,  98 },       // 1200
    {   0,   0 },       // 1800
    { 124,  49 },       // 2400
    { 0,     0 },       // 3600
    { 124,  24 },       // 4800
    {   0,   0 },       // 7200
    {  70,  21 },       // 9600
    {   0,   0 },       // 14400
    {  70,  10 },       // 19200
    {  22,  16 },       // 38400
    {  88,   2 },       // 57600
    {  64,   1 },       // 115200
    {  62,   0 },       // 230400
};


//-------------------------------------------------------------------------
// Info for each serial device controlled

typedef struct mn10300_serial_info {
    CYG_ADDRWORD   base;
    CYG_ADDRWORD   timer_base;
    CYG_WORD       rx_int;
    CYG_WORD       tx_int;
    cyg_bool       is_serial2;
    cyg_interrupt  rx_interrupt;
    cyg_interrupt  tx_interrupt;
    cyg_handle_t   rx_interrupt_handle;
    cyg_handle_t   tx_interrupt_handle;
} mn10300_serial_info;

//-------------------------------------------------------------------------
// Callback functions exported by this driver

static SERIAL_FUNS(mn10300_serial_funs, 
                   mn10300_serial_putc, 
                   mn10300_serial_getc,
                   mn10300_serial_set_config,
                   mn10300_serial_start_xmit,
                   mn10300_serial_stop_xmit
    );

//-------------------------------------------------------------------------
// Hardware info for each serial line

#ifndef CYG_HAL_MN10300_STDEVAL1
#ifdef CYGPKG_IO_SERIAL_MN10300_SERIAL0
static mn10300_serial_info mn10300_serial_info0 = {
    0x34000800,
    0x34001000,
    CYGNUM_HAL_INTERRUPT_SERIAL_0_RX,
    CYGNUM_HAL_INTERRUPT_SERIAL_0_TX,
    false
};
#if CYGNUM_IO_SERIAL_MN10300_SERIAL0_BUFSIZE > 0
static unsigned char mn10300_serial_out_buf0[CYGNUM_IO_SERIAL_MN10300_SERIAL0_BUFSIZE];
static unsigned char mn10300_serial_in_buf0[CYGNUM_IO_SERIAL_MN10300_SERIAL0_BUFSIZE];
#endif
#endif // CYGPKG_IO_SERIAL_MN10300_SERIAL0
#endif

#ifdef CYGPKG_IO_SERIAL_MN10300_SERIAL1
static mn10300_serial_info mn10300_serial_info1 = {
    0x34000810,
    0x34001001,    
    CYGNUM_HAL_INTERRUPT_SERIAL_1_RX,
    CYGNUM_HAL_INTERRUPT_SERIAL_1_TX,
    false
};
#if CYGNUM_IO_SERIAL_MN10300_SERIAL1_BUFSIZE > 0
static unsigned char mn10300_serial_out_buf1[CYGNUM_IO_SERIAL_MN10300_SERIAL1_BUFSIZE];
static unsigned char mn10300_serial_in_buf1[CYGNUM_IO_SERIAL_MN10300_SERIAL1_BUFSIZE];
#endif
#endif // CYGPKG_IO_SERIAL_MN10300_SERIAL1

#ifdef CYGPKG_IO_SERIAL_MN10300_SERIAL2
static mn10300_serial_info mn10300_serial_info2 = {
    0x34000820,
    0x34001002,    
    CYGNUM_HAL_INTERRUPT_SERIAL_2_RX,
    CYGNUM_HAL_INTERRUPT_SERIAL_2_TX,
    true
};
#if CYGNUM_IO_SERIAL_MN10300_SERIAL2_BUFSIZE > 0
static unsigned char mn10300_serial_out_buf2[CYGNUM_IO_SERIAL_MN10300_SERIAL2_BUFSIZE];
static unsigned char mn10300_serial_in_buf2[CYGNUM_IO_SERIAL_MN10300_SERIAL2_BUFSIZE];
#endif
#endif // CYGPKG_IO_SERIAL_MN10300_SERIAL2


//-------------------------------------------------------------------------
// Channel descriptions:

#ifdef CYGPKG_IO_SERIAL_MN10300_POLLED_MODE
#define SIZEOF_BUF(_x_) 0
#else
#define SIZEOF_BUF(_x_) sizeof(_x_)
#endif

#ifndef CYG_HAL_MN10300_STDEVAL1
#ifdef CYGPKG_IO_SERIAL_MN10300_SERIAL0
#if CYGNUM_IO_SERIAL_MN10300_SERIAL0_BUFSIZE > 0
static SERIAL_CHANNEL_USING_INTERRUPTS(mn10300_serial_channel0,
                                       mn10300_serial_funs, 
                                       mn10300_serial_info0,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_MN10300_SERIAL0_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &mn10300_serial_out_buf0[0],
                                       SIZEOF_BUF(mn10300_serial_out_buf0),
                                       &mn10300_serial_in_buf0[0],
                                       SIZEOF_BUF(mn10300_serial_in_buf0)
    );
#else
static SERIAL_CHANNEL(mn10300_serial_channel0,
                      mn10300_serial_funs, 
                      mn10300_serial_info0,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_MN10300_SERIAL0_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif
#endif // CYGPKG_IO_SERIAL_MN10300_SERIAL0
#endif
    
#ifdef CYGPKG_IO_SERIAL_MN10300_SERIAL1
#if CYGNUM_IO_SERIAL_MN10300_SERIAL1_BUFSIZE > 0
static SERIAL_CHANNEL_USING_INTERRUPTS(mn10300_serial_channel1,
                                       mn10300_serial_funs, 
                                       mn10300_serial_info1,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_MN10300_SERIAL1_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &mn10300_serial_out_buf1[0],
                                       SIZEOF_BUF(mn10300_serial_out_buf1),
                                       &mn10300_serial_in_buf1[0],
                                       SIZEOF_BUF(mn10300_serial_in_buf1)
    );
#else
static SERIAL_CHANNEL(mn10300_serial_channel1,
                      mn10300_serial_funs, 
                      mn10300_serial_info1,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_MN10300_SERIAL1_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif
#endif // CYGPKG_IO_SERIAL_MN10300_SERIAL1

#ifdef CYGPKG_IO_SERIAL_MN10300_SERIAL2
#if CYGNUM_IO_SERIAL_MN10300_SERIAL2_BUFSIZE > 0
static SERIAL_CHANNEL_USING_INTERRUPTS(mn10300_serial_channel2,
                                       mn10300_serial_funs, 
                                       mn10300_serial_info2,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_MN10300_SERIAL2_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &mn10300_serial_out_buf2[0],
                                       SIZEOF_BUF(mn10300_serial_out_buf2),
                                       &mn10300_serial_in_buf2[0],
                                       SIZEOF_BUF(mn10300_serial_in_buf2)
    );
#else
static SERIAL_CHANNEL(mn10300_serial_channel2,
                      mn10300_serial_funs, 
                      mn10300_serial_info2,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_MN10300_SERIAL2_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif
#endif // CYGPKG_IO_SERIAL_MN10300_SERIAL2
    
//-------------------------------------------------------------------------
// And finally, the device table entries:

#ifndef CYG_HAL_MN10300_STDEVAL1
#ifdef CYGPKG_IO_SERIAL_MN10300_SERIAL0
// On the standard eval board serial0 is not connected. If enabled, it
// generates continuous frame error and overrun interrupts. Hence we do
// not touch it.
DEVTAB_ENTRY(mn10300_serial_io0, 
             CYGDAT_IO_SERIAL_MN10300_SERIAL0_NAME,
             0,                     // Does not depend on a lower level interface
             &serial_devio, 
             mn10300_serial_init, 
             mn10300_serial_lookup,     // Serial driver may need initializing
             &mn10300_serial_channel0
    );
#endif // CYGPKG_IO_SERIAL_MN10300_SERIAL0
#endif

#ifdef CYGPKG_IO_SERIAL_MN10300_SERIAL1
DEVTAB_ENTRY(mn10300_serial_io1, 
             CYGDAT_IO_SERIAL_MN10300_SERIAL1_NAME,
             0,                     // Does not depend on a lower level interface
             &serial_devio, 
             mn10300_serial_init, 
             mn10300_serial_lookup,     // Serial driver may need initializing
             &mn10300_serial_channel1
    );
#endif // CYGPKG_IO_SERIAL_MN10300_SERIAL1

#ifdef CYGPKG_IO_SERIAL_MN10300_SERIAL2
DEVTAB_ENTRY(mn10300_serial_io2, 
             CYGDAT_IO_SERIAL_MN10300_SERIAL2_NAME,
             0,                     // Does not depend on a lower level interface
             &serial_devio, 
             mn10300_serial_init, 
             mn10300_serial_lookup,     // Serial driver may need initializing
             &mn10300_serial_channel2
    );
#endif // CYGPKG_IO_SERIAL_MN10300_SERIAL2

//-------------------------------------------------------------------------
// Read the serial line's status register. Serial 2 has an 8 bit status
// register while serials 0 and 1 have 16 bit registers. This function
// uses the correct size access, but passes back a 16 bit quantity for
// both.

static cyg_uint16 mn10300_read_sr( mn10300_serial_info *mn10300_chan )
{
    cyg_uint16 sr = 0;
    if( mn10300_chan->is_serial2 )
    {
        cyg_uint8 sr8;
        HAL_READ_UINT8(mn10300_chan->base+SERIAL_STR, sr8);
        sr = sr8;
    }
    else
    {
        HAL_READ_UINT16(mn10300_chan->base+SERIAL_STR, sr);        
    }

    return sr;
}

//-------------------------------------------------------------------------

static bool
mn10300_serial_config_port(serial_channel *chan, cyg_serial_info_t *new_config, bool init)
{
    mn10300_serial_info *mn10300_chan = (mn10300_serial_info *)chan->dev_priv;
    cyg_uint16 cr = 0;

    // Disable device entirely.
    HAL_WRITE_UINT16(mn10300_chan->base+SERIAL_CTR, 0);
    HAL_WRITE_UINT8(mn10300_chan->base+SERIAL_ICR, 0);

    // Set up baud rate
    if( mn10300_chan->is_serial2 )
    {
        // Serial 2 is a bit different from 0 and 1 in the way that the
        // baud rate is controlled.

        cyg_uint8 baud_divisor = select_baud_2[new_config->baud].timer2_val;
        
        if (baud_divisor == 0)
            return false; // Invalid baud rate selected
        
        HAL_WRITE_UINT8(mn10300_chan->timer_base+TIMER_BR, baud_divisor);

        HAL_WRITE_UINT8(mn10300_chan->timer_base+TIMER_MD, 0x80 );

        baud_divisor = select_baud_2[new_config->baud].serial2_val;

        HAL_WRITE_UINT8(mn10300_chan->base+SERIAL_TIM, baud_divisor);
        
        cr |= 0x0001;   // Source clock from timer 2
    }
    else
    {
        cyg_uint8 baud_divisor = select_baud_01[new_config->baud];
        
        if (baud_divisor == 0)
            return false; // Invalid baud rate selected

        HAL_WRITE_UINT8(mn10300_chan->timer_base+TIMER_BR, baud_divisor);

        HAL_WRITE_UINT8(mn10300_chan->timer_base+TIMER_MD, 0x80 );

        cr |= 0x0004;   // Source clock from timer 0 or 1
    }

    HAL_WRITE_UINT8( PORT3_MD, 0x01 );
    
    // set up other config values:

    cr |= select_word_length[new_config->word_length - CYGNUM_SERIAL_WORD_LENGTH_5];
    cr |= select_stop_bits[new_config->stop];
    cr |= select_parity[new_config->parity];

#ifdef CYGPKG_IO_SERIAL_MN10300_POLLED_MODE
    // enable RX and TX
    cr |= LCR_RXE | LCR_TXE;
#else
    if( mn10300_chan->is_serial2 ) {
        cr |= LCR_RXE | LCR_TXE;        // enable Rx and TX
        cr &= ~LCR_TWE;                 // disable transmit interrupts
    } else {
        // Enable RX only
        cr |= LCR_RXE;
    }
#endif
    
    // Write CR into hardware
    HAL_WRITE_UINT16(mn10300_chan->base+SERIAL_CTR, cr);    

    mn10300_read_sr(mn10300_chan);
    if (new_config != &chan->config) {
        chan->config = *new_config;
    }
    return true;
}

//-------------------------------------------------------------------------
// Function to initialize the device.  Called at bootstrap time.

bool mn10300_serial_init(struct cyg_devtab_entry *tab)
{
    serial_channel *chan = (serial_channel *)tab->priv;
    mn10300_serial_info *mn10300_chan = (mn10300_serial_info *)chan->dev_priv;

    (chan->callbacks->serial_init)(chan);  // Really only required for interrupt driven devices

#ifndef CYGPKG_IO_SERIAL_MN10300_POLLED_MODE    
    if (chan->out_cbuf.len != 0) {
        // Install and enable the receive interrupt
        cyg_drv_interrupt_create(mn10300_chan->rx_int,
                                 4,                      // Priority - what goes here?
                                 (cyg_addrword_t)chan,   //  Data item passed to interrupt handler
                                 mn10300_serial_rx_ISR,
                                 mn10300_serial_rx_DSR,
                                 &mn10300_chan->rx_interrupt_handle,
                                 &mn10300_chan->rx_interrupt);
        cyg_drv_interrupt_attach(mn10300_chan->rx_interrupt_handle);
        cyg_drv_interrupt_unmask(mn10300_chan->rx_int);

        // Install and enable the transmit interrupt
        cyg_drv_interrupt_create(mn10300_chan->tx_int,
                                 4,                      // Priority - what goes here?
                                 (cyg_addrword_t)chan,   //  Data item passed to interrupt handler
                                 mn10300_serial_tx_ISR,
                                 mn10300_serial_tx_DSR,
                                 &mn10300_chan->tx_interrupt_handle,
                                 &mn10300_chan->tx_interrupt);
        cyg_drv_interrupt_attach(mn10300_chan->tx_interrupt_handle);
        cyg_drv_interrupt_mask(mn10300_chan->tx_int);
    }
#endif
    
    mn10300_serial_config_port(chan, &chan->config, true);
    
    return true;
}

//-------------------------------------------------------------------------
// This routine is called when the device is "looked" up (i.e. attached)

static Cyg_ErrNo 
mn10300_serial_lookup(struct cyg_devtab_entry **tab, 
                  struct cyg_devtab_entry *sub_tab,
                  const char *name)
{
    serial_channel *chan = (serial_channel *)(*tab)->priv;
    (chan->callbacks->serial_init)(chan);  // Really only required for interrupt driven devices
    return ENOERR;
}

//-------------------------------------------------------------------------
// Return 'true' if character is sent to device

bool
mn10300_serial_putc(serial_channel *chan, unsigned char c)
{
    mn10300_serial_info *mn10300_chan = (mn10300_serial_info *)chan->dev_priv;
    cyg_uint8 sr = mn10300_read_sr( mn10300_chan);

    if( (sr & SR_TBF) == 0 )
    {
        HAL_WRITE_UINT8( mn10300_chan->base+SERIAL_TXB, c );

        return true;
    }
    else return false;
}

//-------------------------------------------------------------------------

unsigned char 
mn10300_serial_getc(serial_channel *chan)
{
    unsigned char c;
    mn10300_serial_info *mn10300_chan = (mn10300_serial_info *)chan->dev_priv;
    do
    {
        cyg_uint8 sr = mn10300_read_sr( mn10300_chan );

        if( (sr & SR_RBF) != 0 )
        {
            HAL_READ_UINT8( mn10300_chan->base+SERIAL_RXB, c );

            break;
        }
        
    } while(1);
    
    return c;
}

//-------------------------------------------------------------------------

bool 
mn10300_serial_set_config(serial_channel *chan, cyg_serial_info_t *config)
{
    return mn10300_serial_config_port(chan, config, false);
}

//-------------------------------------------------------------------------
// Enable the transmitter on the device

static void
mn10300_serial_start_xmit(serial_channel *chan)
{
#ifndef CYGPKG_IO_SERIAL_MN10300_POLLED_MODE    
    mn10300_serial_info *mn10300_chan = (mn10300_serial_info *)chan->dev_priv;
    cyg_uint16 cr;

    HAL_READ_UINT16( mn10300_chan->base+SERIAL_CTR, cr );

    if( mn10300_chan->is_serial2 )
        cr |= LCR_TWE;                  // enable transmit interrupts
    else
        cr |= LCR_TXE;                  // enable TX

    HAL_WRITE_UINT16( mn10300_chan->base+SERIAL_CTR, cr );

    cyg_drv_interrupt_unmask(mn10300_chan->tx_int);
    
    (chan->callbacks->xmt_char)(chan);
#endif    
}

//-------------------------------------------------------------------------
// Disable the transmitter on the device

static void 
mn10300_serial_stop_xmit(serial_channel *chan)
{
#ifndef CYGPKG_IO_SERIAL_MN10300_POLLED_MODE    
    mn10300_serial_info *mn10300_chan = (mn10300_serial_info *)chan->dev_priv;
    cyg_uint16 cr;
    cyg_uint16 sr;

    // Wait until the transmitter has actually stopped before turning it off.
    
    do
    {
        sr = mn10300_read_sr( mn10300_chan );
        
    } while( sr & SR_TXF );
    
    HAL_READ_UINT16( mn10300_chan->base+SERIAL_CTR, cr );

    if( mn10300_chan->is_serial2 )
        cr &= ~LCR_TWE;                 // disable transmit interrupts
    else
        cr &= ~LCR_TXE;                 // disable transmission

    HAL_WRITE_UINT16( mn10300_chan->base+SERIAL_CTR, cr );

    cyg_drv_interrupt_mask(mn10300_chan->tx_int);
    
#endif    
}

//-------------------------------------------------------------------------
// Serial I/O - low level interrupt handlers (ISR)

#ifndef CYGPKG_IO_SERIAL_MN10300_POLLED_MODE

static cyg_uint32 
mn10300_serial_rx_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    mn10300_serial_info *mn10300_chan = (mn10300_serial_info *)chan->dev_priv;

    cyg_drv_interrupt_mask(mn10300_chan->rx_int);
    cyg_drv_interrupt_acknowledge(mn10300_chan->rx_int);

    return CYG_ISR_CALL_DSR;  // Cause DSR to be run
}


static cyg_uint32 
mn10300_serial_tx_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    mn10300_serial_info *mn10300_chan = (mn10300_serial_info *)chan->dev_priv;

    cyg_drv_interrupt_mask(mn10300_chan->tx_int);
    cyg_drv_interrupt_acknowledge(mn10300_chan->tx_int);

    return CYG_ISR_CALL_DSR;  // Cause DSR to be run
}

#endif

//-------------------------------------------------------------------------
// Serial I/O - high level interrupt handler (DSR)

#ifndef CYGPKG_IO_SERIAL_MN10300_POLLED_MODE

static void       
mn10300_serial_rx_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    mn10300_serial_info *mn10300_chan = (mn10300_serial_info *)chan->dev_priv;
    cyg_uint8 sr = mn10300_read_sr( mn10300_chan);

    if( (sr & SR_RBF) != 0 )
    {
        cyg_uint8 rxb;
        HAL_READ_UINT8( mn10300_chan->base+SERIAL_RXB, rxb );

        (chan->callbacks->rcv_char)(chan, rxb);
    }

    cyg_drv_interrupt_unmask(mn10300_chan->rx_int);
}

static void       
mn10300_serial_tx_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    mn10300_serial_info *mn10300_chan = (mn10300_serial_info *)chan->dev_priv;
    cyg_uint8 sr = mn10300_read_sr( mn10300_chan);

    if( (sr & SR_TBF) == 0 )
    {
        (chan->callbacks->xmt_char)(chan);
    }
    
    cyg_drv_interrupt_unmask(mn10300_chan->tx_int);
}

#endif

#endif // CYGPKG_IO_SERIAL_MN10300

//-------------------------------------------------------------------------
// EOF mn10300.c
