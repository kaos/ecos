//==========================================================================
//
//      io/serial/sh/sh_sci_serial.c
//
//      SH Serial SCI I/O Interface Module (interrupt driven)
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jskov
// Contributors:gthomas, jskov
// Date:        1999-05-24
// Purpose:     SH Serial I/O module (interrupt driven version)
// Description: 
//
// Note: Since interrupt sources from the same SCI channel share the same
//       interrupt level, there is no risk of races when altering the
//       channel's control register from ISRs and DSRs. However, when 
//       altering the control register from user-level code, interrupts
//       must be disabled while the register is being accessed.
//
// FIXME: Receiving in polled mode prevents duplex transfers from working for
//        some reason.
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/io_serial.h>
#include <pkgconf/io.h>

#include <cyg/io/io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/io/devtab.h>
#include <cyg/infra/diag.h>
#include <cyg/io/serial.h>

#include <cyg/hal/sh_regs.h>

// Only compile driver if an inline file with driver details was selected.
#ifdef CYGDAT_IO_SERIAL_SH_SCI_INL

// Controller base (only one supported at the moment)
#define SH_SERIAL_SCI_BASE       0xfffffe80

// The SCI controller register layout on the SH3/7708.
#define SCI_SCSMR                0      // serial mode register
#define SCI_SCBRR                2      // bit rate register
#define SCI_SCSCR                4      // serial control register
#define SCI_SCTDR                6      // transmit data register
#define SCI_SCSSR                8      // serial status register
#define SCI_SCRDR                10     // receive data register
#define SCI_SCSPTR               -4     // serial port register

static short select_word_length[] = {
    -1,
    -1,
    CYGARC_REG_SCSMR_CHR,               // 7 bits
    0                                   // 8 bits
};

static short select_stop_bits[] = {
    -1,
    0,                                  // 1 stop bit
    -1,
    CYGARC_REG_SCSMR_STOP               // 2 stop bits
};

static short select_parity[] = {
    0,                                  // No parity
    CYGARC_REG_SCSMR_PE,                // Even parity
    CYGARC_REG_SCSMR_PE|CYGARC_REG_SCSMR_OE, // Odd parity
    -1,
    -1
};

static unsigned short select_baud[] = {
    0,    // Unused
    CYGARC_SCBRR_CKSx(50)<<8 | CYGARC_SCBRR_N(50),
    CYGARC_SCBRR_CKSx(75)<<8 | CYGARC_SCBRR_N(75),
    CYGARC_SCBRR_CKSx(110)<<8 | CYGARC_SCBRR_N(110),
    CYGARC_SCBRR_CKSx(134)<<8 | CYGARC_SCBRR_N(134),
    CYGARC_SCBRR_CKSx(150)<<8 | CYGARC_SCBRR_N(150),
    CYGARC_SCBRR_CKSx(200)<<8 | CYGARC_SCBRR_N(200),
    CYGARC_SCBRR_CKSx(300)<<8 | CYGARC_SCBRR_N(300),
    CYGARC_SCBRR_CKSx(600)<<8 | CYGARC_SCBRR_N(600),
    CYGARC_SCBRR_CKSx(1200)<<8 | CYGARC_SCBRR_N(1200),
    CYGARC_SCBRR_CKSx(1800)<<8 | CYGARC_SCBRR_N(1800),
    CYGARC_SCBRR_CKSx(2400)<<8 | CYGARC_SCBRR_N(2400),
    CYGARC_SCBRR_CKSx(3600)<<8 | CYGARC_SCBRR_N(3600),
    CYGARC_SCBRR_CKSx(4800)<<8 | CYGARC_SCBRR_N(4800),
    CYGARC_SCBRR_CKSx(7200)<<8 | CYGARC_SCBRR_N(7200),
    CYGARC_SCBRR_CKSx(9600)<<8 | CYGARC_SCBRR_N(9600),
    CYGARC_SCBRR_CKSx(14400)<<8 | CYGARC_SCBRR_N(14400),
    CYGARC_SCBRR_CKSx(19200)<<8 | CYGARC_SCBRR_N(19200),
    CYGARC_SCBRR_CKSx(38400)<<8 | CYGARC_SCBRR_N(38400),
    CYGARC_SCBRR_CKSx(57600)<<8 | CYGARC_SCBRR_N(57600),
    CYGARC_SCBRR_CKSx(115200)<<8 | CYGARC_SCBRR_N(115200),
    CYGARC_SCBRR_CKSx(230400)<<8 | CYGARC_SCBRR_N(230400)
};


typedef struct sh_sci_info {
    CYG_ADDRWORD   data;                // Pointer to data register
    
    CYG_WORD       er_int_num,          // Error interrupt number
                   rx_int_num,          // Receive interrupt number
                   tx_int_num;          // Transmit interrupt number

    CYG_ADDRWORD   ctrl_base;           // Base address of SCI controller

    cyg_interrupt  serial_er_interrupt, 
                   serial_rx_interrupt, 
                   serial_tx_interrupt;
    cyg_handle_t   serial_er_interrupt_handle, 
                   serial_rx_interrupt_handle, 
                   serial_tx_interrupt_handle;

    bool           tx_enabled;
} sh_sci_info;

static bool sh_serial_init(struct cyg_devtab_entry *tab);
static bool sh_serial_putc(serial_channel *chan, unsigned char c);
static Cyg_ErrNo sh_serial_lookup(struct cyg_devtab_entry **tab, 
                                   struct cyg_devtab_entry *sub_tab,
                                   const char *name);
static unsigned char sh_serial_getc(serial_channel *chan);
static Cyg_ErrNo sh_serial_set_config(serial_channel *chan, cyg_uint32 key,
                                      const void *xbuf, cyg_uint32 *len);
static void sh_serial_start_xmit(serial_channel *chan);
static void sh_serial_stop_xmit(serial_channel *chan);

static cyg_uint32 sh_serial_tx_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       sh_serial_tx_DSR(cyg_vector_t vector, cyg_ucount32 count, 
                                   cyg_addrword_t data);
static cyg_uint32 sh_serial_rx_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       sh_serial_rx_DSR(cyg_vector_t vector, cyg_ucount32 count, 
                                   cyg_addrword_t data);
static cyg_uint32 sh_serial_er_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       sh_serial_er_DSR(cyg_vector_t vector, cyg_ucount32 count, 
                                   cyg_addrword_t data);

static SERIAL_FUNS(sh_serial_funs, 
                   sh_serial_putc, 
                   sh_serial_getc,
                   sh_serial_set_config,
                   sh_serial_start_xmit,
                   sh_serial_stop_xmit
    );

#include CYGDAT_IO_SERIAL_SH_SCI_INL

// Internal function to actually configure the hardware to desired baud rate,
// etc.
static bool
sh_serial_config_port(serial_channel *chan, cyg_serial_info_t *new_config, 
                      bool init)
{
    cyg_uint16 baud_divisor = select_baud[new_config->baud];
    sh_sci_info *sh_chan = (sh_sci_info *)chan->dev_priv;
    cyg_uint8 _scr, _smr;

    // Check configuration request
    if ((-1 == select_word_length[(new_config->word_length -
                                  CYGNUM_SERIAL_WORD_LENGTH_5)])
        || -1 == select_stop_bits[new_config->stop]
        || -1 == select_parity[new_config->parity]
        || baud_divisor == 0)
        return false;

    // Disable SCI interrupts while changing hardware
    HAL_READ_UINT8(sh_chan->ctrl_base+SCI_SCSCR, _scr);
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCSCR, 0);

    // Set databits, stopbits and parity.
    _smr = select_word_length[(new_config->word_length -
                               CYGNUM_SERIAL_WORD_LENGTH_5)] | 
        select_stop_bits[new_config->stop] |
        select_parity[new_config->parity];
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCSMR, _smr);

    // Set baud rate.
    _smr &= ~CYGARC_REG_SCSMR_CKSx_MASK;
    _smr |= baud_divisor >> 8;
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCSMR, _smr);
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCBRR, baud_divisor & 0xff);

    // Clear the status register.
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCSSR, 0);

    if (init) {
        // Always enable transmitter and receiver.
        _scr = CYGARC_REG_SCSCR_TE | CYGARC_REG_SCSCR_RE;

        if (chan->out_cbuf.len != 0)
            _scr |= CYGARC_REG_SCSCR_TIE; // enable tx interrupts

        if (chan->in_cbuf.len != 0)
            _scr |= CYGARC_REG_SCSCR_RIE; // enable rx interrupts
    }
     
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCSCR, _scr);

    if (new_config != &chan->config) {
        chan->config = *new_config;
    }
    return true;
}

// Function to initialize the device.  Called at bootstrap time.
static bool 
sh_serial_init(struct cyg_devtab_entry *tab)
{
    serial_channel *chan = (serial_channel *)tab->priv;
    sh_sci_info *sh_chan = (sh_sci_info *)chan->dev_priv;
#ifdef CYGDBG_IO_INIT
    diag_printf("SH SERIAL init - dev: %x.%d\n", 
                sh_chan->data, sh_chan->rx_int_num);
#endif
    // Really only required for interrupt driven devices
    (chan->callbacks->serial_init)(chan);

    if (chan->out_cbuf.len != 0) {
        cyg_drv_interrupt_create(sh_chan->tx_int_num,
                                 3,
                                 (cyg_addrword_t)chan, // Data item passed to interrupt handler
                                 sh_serial_tx_ISR,
                                 sh_serial_tx_DSR,
                                 &sh_chan->serial_tx_interrupt_handle,
                                 &sh_chan->serial_tx_interrupt);
        cyg_drv_interrupt_attach(sh_chan->serial_tx_interrupt_handle);
        cyg_drv_interrupt_unmask(sh_chan->tx_int_num);
        sh_chan->tx_enabled = false;
    }
    if (chan->in_cbuf.len != 0) {
        // Receive interrupt
        cyg_drv_interrupt_create(sh_chan->rx_int_num,
                                 3,
                                 (cyg_addrword_t)chan, // Data item passed to interrupt handler
                                 sh_serial_rx_ISR,
                                 sh_serial_rx_DSR,
                                 &sh_chan->serial_rx_interrupt_handle,
                                 &sh_chan->serial_rx_interrupt);
        cyg_drv_interrupt_attach(sh_chan->serial_rx_interrupt_handle);
        // Receive error interrupt
        cyg_drv_interrupt_create(sh_chan->er_int_num,
                                 3,
                                 (cyg_addrword_t)chan, // Data item passed to interrupt handler
                                 sh_serial_er_ISR,
                                 sh_serial_er_DSR,
                                 &sh_chan->serial_er_interrupt_handle,
                                 &sh_chan->serial_er_interrupt);
        cyg_drv_interrupt_attach(sh_chan->serial_er_interrupt_handle);
        // This unmasks both interrupt sources.
        cyg_drv_interrupt_unmask(sh_chan->rx_int_num);
    }
    sh_serial_config_port(chan, &chan->config, true);
    return true;
}

// This routine is called when the device is "looked" up (i.e. attached)
static Cyg_ErrNo 
sh_serial_lookup(struct cyg_devtab_entry **tab, 
                  struct cyg_devtab_entry *sub_tab,
                  const char *name)
{
    serial_channel *chan = (serial_channel *)(*tab)->priv;

    // Really only required for interrupt driven devices
    (chan->callbacks->serial_init)(chan);
    return ENOERR;
}

// Send a character to the device output buffer.
// Return 'true' if character is sent to device
static bool
sh_serial_putc(serial_channel *chan, unsigned char c)
{
    cyg_uint8 _ssr;
    sh_sci_info *sh_chan = (sh_sci_info *)chan->dev_priv;

    HAL_READ_UINT8(sh_chan->ctrl_base+SCI_SCSSR, _ssr);
    if (_ssr & CYGARC_REG_SCSSR_TDRE) {
// Transmit buffer is empty
        HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCTDR, c);
        // Clear empty flag.
        HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCSSR, 
                        CYGARC_REG_SCSSR_CLEARMASK & ~CYGARC_REG_SCSSR_TDRE);
        return true;
    } else {
// No space
        return false;
    }
}

// Fetch a character from the device input buffer, waiting if necessary
static unsigned char 
sh_serial_getc(serial_channel *chan)
{
    sh_sci_info *sh_chan = (sh_sci_info *)chan->dev_priv;
    unsigned char c;
    cyg_uint8 _ssr;

    do {
        HAL_READ_UINT8(sh_chan->ctrl_base+SCI_SCSSR, _ssr);
    } while ((_ssr & CYGARC_REG_SCSSR_RDRF) == 0);

    HAL_READ_UINT8(sh_chan->ctrl_base+SCI_SCRDR, c);

    // Clear buffer full flag.
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCSSR, 
                    CYGARC_REG_SCSSR_CLEARMASK & ~CYGARC_REG_SCSSR_RDRF);

    return c;
}

// Set up the device characteristics; baud rate, etc.
static Cyg_ErrNo
sh_serial_set_config(serial_channel *chan, cyg_uint32 key,
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
        if ( true != sh_serial_config_port(chan, config, false) )
            return -EINVAL;
      }
      break;
    default:
        return -EINVAL;
    }
    return ENOERR;
}

// Enable the transmitter on the device
static void
sh_serial_start_xmit(serial_channel *chan)
{
    cyg_uint8 _scr;
    sh_sci_info *sh_chan = (sh_sci_info *)chan->dev_priv;

    sh_chan->tx_enabled = true;

    // Mask the interrupts (all sources of the unit) while changing
    // the CR since a rx interrupt in the middle of this would result
    // in a bad CR state.
    cyg_drv_interrupt_mask(sh_chan->rx_int_num);

    HAL_READ_UINT8(sh_chan->ctrl_base+SCI_SCSCR, _scr);
    _scr |= CYGARC_REG_SCSCR_TIE;       // Enable xmit interrupt
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCSCR, _scr);

    cyg_drv_interrupt_unmask(sh_chan->rx_int_num);
}

// Disable the transmitter on the device
static void 
sh_serial_stop_xmit(serial_channel *chan)
{
    cyg_uint8 _scr;
    sh_sci_info *sh_chan = (sh_sci_info *)chan->dev_priv;

    sh_chan->tx_enabled = false;

    // Mask the interrupts (all sources of the unit) while changing
    // the CR since a rx interrupt in the middle of this would result
    // in a bad CR state.
    cyg_drv_interrupt_mask(sh_chan->rx_int_num);

    HAL_READ_UINT8(sh_chan->ctrl_base+SCI_SCSCR, _scr);
    _scr &= ~CYGARC_REG_SCSCR_TIE;      // Disable xmit interrupt
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCSCR, _scr);

    cyg_drv_interrupt_unmask(sh_chan->rx_int_num);
}

// Serial I/O - low level tx interrupt handler (ISR)
static cyg_uint32 
sh_serial_tx_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    sh_sci_info *sh_chan = (sh_sci_info *)chan->dev_priv;
    cyg_uint8 _scr;

    HAL_READ_UINT8(sh_chan->ctrl_base+SCI_SCSCR, _scr);
    _scr &= ~CYGARC_REG_SCSCR_TIE;      // mask out tx interrupts
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCSCR, _scr);

    return CYG_ISR_CALL_DSR;  // Cause DSR to be run
}

// Serial I/O - high level tx interrupt handler (DSR)
static void       
sh_serial_tx_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    sh_sci_info *sh_chan = (sh_sci_info *)chan->dev_priv;

    (chan->callbacks->xmt_char)(chan);

    if (sh_chan->tx_enabled) {
        cyg_uint8 _scr;

        HAL_READ_UINT8(sh_chan->ctrl_base+SCI_SCSCR, _scr);
        _scr |= CYGARC_REG_SCSCR_TIE;       // unmask tx interrupts
        HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCSCR, _scr);
    }
}

// Serial I/O - low level RX interrupt handler (ISR)
static cyg_uint32 
sh_serial_rx_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    sh_sci_info *sh_chan = (sh_sci_info *)chan->dev_priv;
    cyg_uint8 _scr;

    HAL_READ_UINT8(sh_chan->ctrl_base+SCI_SCSCR, _scr);
    _scr &= ~CYGARC_REG_SCSCR_RIE;      // mask rx interrupts
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCSCR, _scr);
    return CYG_ISR_CALL_DSR;  // Cause DSR to be run
}

// Serial I/O - high level rx interrupt handler (DSR)
static void       
sh_serial_rx_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    sh_sci_info *sh_chan = (sh_sci_info *)chan->dev_priv;
    cyg_uint8 _ssr, _scr;

    HAL_READ_UINT8(sh_chan->ctrl_base+SCI_SCSSR, _ssr);
    if (_ssr & CYGARC_REG_SCSSR_RDRF) {
        cyg_uint8 _c;
        HAL_READ_UINT8(sh_chan->ctrl_base+SCI_SCRDR, _c);
        // Clear buffer full flag.
        HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCSSR, 
                        CYGARC_REG_SCSSR_CLEARMASK & ~CYGARC_REG_SCSSR_RDRF);

        (chan->callbacks->rcv_char)(chan, _c);
    }

    HAL_READ_UINT8(sh_chan->ctrl_base+SCI_SCSCR, _scr);
    _scr |= CYGARC_REG_SCSCR_RIE;       // unmask rx interrupts
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCSCR, _scr);
}

static volatile int sh_serial_error_orer = 0;
static volatile int sh_serial_error_fer = 0;
static volatile int sh_serial_error_per = 0;

// Serial I/O - low level error interrupt handler (ISR)
static cyg_uint32 
sh_serial_er_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    sh_sci_info *sh_chan = (sh_sci_info *)chan->dev_priv;
    cyg_uint8 _scr;

    HAL_READ_UINT8(sh_chan->ctrl_base+SCI_SCSCR, _scr);
    _scr &= ~CYGARC_REG_SCSCR_RIE;      // mask rx interrupts
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCSCR, _scr);
    return CYG_ISR_CALL_DSR;  // Cause DSR to be run
}

// Serial I/O - high level error interrupt handler (DSR)
static void       
sh_serial_er_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    sh_sci_info *sh_chan = (sh_sci_info *)chan->dev_priv;
    cyg_uint8 _ssr, _ssr2;

    HAL_READ_UINT8(sh_chan->ctrl_base+SCI_SCSSR, _ssr);
    _ssr2 = CYGARC_REG_SCSSR_CLEARMASK;

    if (_ssr & CYGARC_REG_SCSSR_ORER) {
        _ssr2 &= ~CYGARC_REG_SCSSR_ORER;
        sh_serial_error_orer++;
    }
    if (_ssr & CYGARC_REG_SCSSR_FER) {
        _ssr2 &= ~CYGARC_REG_SCSSR_FER;
        sh_serial_error_fer++;
    }
    if (_ssr & CYGARC_REG_SCSSR_PER) {
        _ssr2 &= ~CYGARC_REG_SCSSR_PER;
        sh_serial_error_per++;
    }
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCI_SCSSR, _ssr2);
}

#endif // ifdef CYGDAT_IO_SERIAL_SH_SCI_INL
