//==========================================================================
//
//      io/serial/sh/scif/sh_scif_serial.c
//
//      SH Serial IRDA/SCIF I/O Interface Module (interrupt driven)
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
// Date:        2000-04-04
// Purpose:     SH3 Serial IRDA/SCIF I/O module (interrupt driven version)
// Description: 
//
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/io_serial.h>
#include <pkgconf/io.h>

// FIXME: This is necessary since the SCI driver may be overriding
// CYGDAT_IO_SERIAL_DEVICE_HEADER. Need a better way to include two
// different drivers.
#include <pkgconf/io_serial_sh_scif.h>

#include <cyg/io/io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/io/devtab.h>
#include <cyg/infra/diag.h>
#include <cyg/io/serial.h>

#include <cyg/hal/sh_regs.h>
#include <cyg/hal/hal_cache.h>

// Only compile driver if an inline file with driver details was selected.
#ifdef CYGDAT_IO_SERIAL_SH_SCIF_INL

// The SCIF controller register layout on the SH3
// The controller base is defined in the board specification file.
#define SCIF_SCSMR                0      // serial mode register
#define SCIF_SCBRR                2      // bit rate register
#define SCIF_SCSCR                4      // serial control register
#define SCIF_SCFTDR               6      // transmit data register
#define SCIF_SCSSR                8      // serial status register
#define SCIF_SCFRDR               10     // receive data register
#define SCIF_SCFCR                12     // FIFO control
#define SCIF_SCFDR                14     // FIFO data count register

static short select_word_length[] = {
    -1,
    -1,
    CYGARC_REG_SCSMR2_CHR,              // 7 bits
    0                                   // 8 bits
};

static short select_stop_bits[] = {
    -1,
    0,                                  // 1 stop bit
    -1,
    CYGARC_REG_SCSMR2_STOP              // 2 stop bits
};

static short select_parity[] = {
    0,                                  // No parity
    CYGARC_REG_SCSMR2_PE,               // Even parity
    CYGARC_REG_SCSMR2_PE|CYGARC_REG_SCSMR2_OE, // Odd parity
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

typedef struct sh3_scif_info {
    CYG_WORD          er_int_num,       // Error interrupt number
                      rx_int_num,       // Receive interrupt number
                      tx_int_num;       // Transmit interrupt number

    CYG_ADDRWORD      ctrl_base;        // Base address of SCI controller

    cyg_interrupt     serial_er_interrupt, 
                      serial_rx_interrupt, 
                      serial_tx_interrupt;
    cyg_handle_t      serial_er_interrupt_handle, 
                      serial_rx_interrupt_handle, 
                      serial_tx_interrupt_handle;

    volatile bool     tx_enabled;       // expect tx _serial_ interrupts

} sh3_scif_info;

static bool sh3_scif_init(struct cyg_devtab_entry *tab);
static bool sh3_scif_putc(serial_channel *chan, unsigned char c);
static Cyg_ErrNo sh3_scif_lookup(struct cyg_devtab_entry **tab, 
                                   struct cyg_devtab_entry *sub_tab,
                                   const char *name);
static unsigned char sh3_scif_getc(serial_channel *chan);
static Cyg_ErrNo sh3_scif_set_config(serial_channel *chan, cyg_uint32 key,
                                     const void *xbuf, cyg_uint32 *len);
static void sh3_scif_start_xmit(serial_channel *chan);
static void sh3_scif_stop_xmit(serial_channel *chan);

static cyg_uint32 sh3_scif_tx_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       sh3_scif_tx_DSR(cyg_vector_t vector, cyg_ucount32 count, 
                                   cyg_addrword_t data);
static cyg_uint32 sh3_scif_rx_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       sh3_scif_rx_DSR(cyg_vector_t vector, cyg_ucount32 count, 
                                   cyg_addrword_t data);
static cyg_uint32 sh3_scif_er_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       sh3_scif_er_DSR(cyg_vector_t vector, cyg_ucount32 count, 
                                   cyg_addrword_t data);

static SERIAL_FUNS(sh3_scif_funs, 
                   sh3_scif_putc, 
                   sh3_scif_getc,
                   sh3_scif_set_config,
                   sh3_scif_start_xmit,
                   sh3_scif_stop_xmit
    );

// Get the board specification
#include CYGDAT_IO_SERIAL_SH_SCIF_INL

// Internal function to actually configure the hardware to desired baud rate,
// etc.
static bool
sh3_scif_config_port(serial_channel *chan, cyg_serial_info_t *new_config, 
                     bool init)
{
    cyg_uint16 baud_divisor = select_baud[new_config->baud];
    sh3_scif_info *sh_chan = (sh3_scif_info *)chan->dev_priv;
    cyg_uint8 _scr, _smr;
    cyg_uint16 _sr;
    CYG_ADDRWORD base = sh_chan->ctrl_base;

    // Check configuration request
    if ((-1 == select_word_length[(new_config->word_length -
                                  CYGNUM_SERIAL_WORD_LENGTH_5)])
        || -1 == select_stop_bits[new_config->stop]
        || -1 == select_parity[new_config->parity]
        || baud_divisor == 0)
        return false;

    // Disable SCI interrupts while changing hardware
    HAL_READ_UINT8(base+SCIF_SCSCR, _scr);
    HAL_WRITE_UINT8(base+SCIF_SCSCR, 0);

    // Reset FIFO.
    HAL_WRITE_UINT8(base+SCIF_SCFCR, 
                    CYGARC_REG_SCFCR2_TFRST|CYGARC_REG_SCFCR2_RFRST);

    // Set databits, stopbits and parity.
    _smr = select_word_length[(new_config->word_length -
                               CYGNUM_SERIAL_WORD_LENGTH_5)] | 
        select_stop_bits[new_config->stop] |
        select_parity[new_config->parity];
    HAL_WRITE_UINT8(base+SCIF_SCSMR, _smr);

    // Set baud rate.
    _smr &= ~CYGARC_REG_SCSMR2_CKSx_MASK;
    _smr |= baud_divisor >> 8;
    HAL_WRITE_UINT8(base+SCIF_SCSMR, _smr);
    HAL_WRITE_UINT8(base+SCIF_SCBRR, baud_divisor & 0xff);

    // FIXME: Should delay 1/<baud> second here.

    // Clear the status register (clear first).
    HAL_READ_UINT16(base+SCIF_SCSSR, _sr);
    HAL_WRITE_UINT16(base+SCIF_SCSSR, 0);

    // Bring FIFO out of reset and set FIFO trigger marks
    HAL_WRITE_UINT8(base+SCIF_SCFCR, 
                    CYGARC_REG_SCFCR2_RTRG_14|CYGARC_REG_SCFCR2_TTRG_8);

    if (init) {
        // Always enable transmitter and receiver.
        _scr = CYGARC_REG_SCSCR2_TE | CYGARC_REG_SCSCR2_RE;

        if (chan->in_cbuf.len != 0)
            _scr |= CYGARC_REG_SCSCR2_RIE; // enable rx interrupts
    }
     
    HAL_WRITE_UINT8(base+SCIF_SCSCR, _scr);

    if (new_config != &chan->config) {
        chan->config = *new_config;
    }
    return true;
}

// Function to initialize the device.  Called at bootstrap time.
static bool 
sh3_scif_init(struct cyg_devtab_entry *tab)
{
    serial_channel *chan = (serial_channel *)tab->priv;
    sh3_scif_info *sh_chan = (sh3_scif_info *)chan->dev_priv;
#ifdef CYGDBG_IO_INIT
    diag_printf("SH SERIAL init - dev: %x.%d\n", 
                sh_chan->ctrl_base, sh_chan->rx_int_num);
#endif
    // Really only required for interrupt driven devices
    (chan->callbacks->serial_init)(chan);

    if (chan->out_cbuf.len != 0) {
        cyg_drv_interrupt_create(sh_chan->tx_int_num,
                                 3,
                                 (cyg_addrword_t)chan, // Data item passed to interrupt handler
                                 sh3_scif_tx_ISR,
                                 sh3_scif_tx_DSR,
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
                                 sh3_scif_rx_ISR,
                                 sh3_scif_rx_DSR,
                                 &sh_chan->serial_rx_interrupt_handle,
                                 &sh_chan->serial_rx_interrupt);
        cyg_drv_interrupt_attach(sh_chan->serial_rx_interrupt_handle);
        // Receive error interrupt
        cyg_drv_interrupt_create(sh_chan->er_int_num,
                                 3,
                                 (cyg_addrword_t)chan, // Data item passed to interrupt handler
                                 sh3_scif_er_ISR,
                                 sh3_scif_er_DSR,
                                 &sh_chan->serial_er_interrupt_handle,
                                 &sh_chan->serial_er_interrupt);
        cyg_drv_interrupt_attach(sh_chan->serial_er_interrupt_handle);
        // This unmasks both interrupt sources.
        cyg_drv_interrupt_unmask(sh_chan->rx_int_num);
    }
    sh3_scif_config_port(chan, &chan->config, true);
    return true;
}

// This routine is called when the device is "looked" up (i.e. attached)
static Cyg_ErrNo 
sh3_scif_lookup(struct cyg_devtab_entry **tab,
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
sh3_scif_putc(serial_channel *chan, unsigned char c)
{
    cyg_uint16 _fdr, _sr;
    sh3_scif_info *sh_chan = (sh3_scif_info *)chan->dev_priv;

    HAL_READ_UINT16(sh_chan->ctrl_base+SCIF_SCFDR, _fdr);
    if (((_fdr & CYGARC_REG_SCFDR2_TCOUNT_MASK) >> CYGARC_REG_SCFDR2_TCOUNT_shift) < 15) {
// Transmit FIFO has room for another char
        HAL_WRITE_UINT8(sh_chan->ctrl_base+SCIF_SCFTDR, c);
        // Clear FIFO-empty/transmit end flags (read back sr first)
        HAL_READ_UINT16(sh_chan->ctrl_base+SCIF_SCSSR, _sr);
        HAL_WRITE_UINT16(sh_chan->ctrl_base+SCIF_SCSSR,
                        CYGARC_REG_SCSSR2_CLEARMASK & ~CYGARC_REG_SCSSR2_TDFE);
        return true;
    } else {
// No space
        return false;
    }
}

// Fetch a character from the device input buffer, waiting if necessary
// Note: Input is running wo FIFO enabled, so the counter is not checked here.
static unsigned char 
sh3_scif_getc(serial_channel *chan)
{
    sh3_scif_info *sh_chan = (sh3_scif_info *)chan->dev_priv;
    unsigned char c;
    cyg_uint16 _sr;

    do {
        HAL_READ_UINT16(sh_chan->ctrl_base+SCIF_SCSSR, _sr);
    } while ((_sr & CYGARC_REG_SCSSR2_RDF) == 0);

    HAL_READ_UINT8(sh_chan->ctrl_base+SCIF_SCFRDR, c);

    // Clear buffer full flag (read back first)
    HAL_READ_UINT16(sh_chan->ctrl_base+SCIF_SCSSR, _sr);
    HAL_WRITE_UINT16(sh_chan->ctrl_base+SCIF_SCSSR, 
                     CYGARC_REG_SCSSR2_CLEARMASK & ~CYGARC_REG_SCSSR2_RDF);

    return c;
}

// Set up the device characteristics; baud rate, etc.
static Cyg_ErrNo
sh3_scif_set_config(serial_channel *chan, cyg_uint32 key,
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
        if ( true != sh3_scif_config_port(chan, config, false) )
            return -EINVAL;
      }
      break;
#ifdef CYGOPT_IO_SERIAL_FLOW_CONTROL_HW
    case CYG_IO_SET_CONFIG_SERIAL_HW_RX_FLOW_THROTTLE:
      {
          cyg_uint8 _scfcr;
          sh3_scif_info *ser_chan = (sh3_scif_info *)chan->dev_priv;
          cyg_addrword_t base = ser_chan->ctrl_base;
          cyg_uint8 *f = (cyg_uint8 *)xbuf;
          unsigned char mask=0;
          if ( *len < *f )
              return -EINVAL;
          
          if ( chan->config.flags & CYGNUM_SERIAL_FLOW_RTSCTS_RX )
              mask |= CYGARC_REG_SCFCR1_MCE;
          HAL_READ_UINT8(base+SCIF_SCFCR, _scfcr);
          if (*f) // we should throttle
              _scfcr &= ~mask;
          else // we should no longer throttle
              _scfcr |= mask;
          HAL_WRITE_UINT8(base+SCIF_SCFCR, _scfcr);
      }
      break;
    case CYG_IO_SET_CONFIG_SERIAL_HW_FLOW_CONFIG:
      {
        // Clear DSR/DTR flag as it's not supported.
        if (chan->config.flags &
            (CYGNUM_SERIAL_FLOW_DSRDTR_RX|CYGNUM_SERIAL_FLOW_DSRDTR_TX)) {
            chan->config.flags &= ~(CYGNUM_SERIAL_FLOW_DSRDTR_RX|
                                    CYGNUM_SERIAL_FLOW_DSRDTR_TX);
            return -EINVAL;
        }
      }
      break;
#endif
    default:
        return -EINVAL;
    }
    return ENOERR;
}



// Enable the transmitter on the device
static void
sh3_scif_start_xmit(serial_channel *chan)
{
    cyg_uint8 _scr;
    sh3_scif_info *sh_chan = (sh3_scif_info *)chan->dev_priv;
    xmt_req_reply_t _block_status = CYG_XMT_DISABLED;

    if (sh_chan->tx_enabled)
        return;

    if (CYG_XMT_DISABLED == _block_status) {
        // Mask interrupts while changing the CR since a rx
        // interrupt or another thread doing the same in the
        // middle of this would result in a bad CR state.
        cyg_drv_isr_lock();
        {
            HAL_READ_UINT8(sh_chan->ctrl_base+SCIF_SCSCR, _scr);
            _scr |= CYGARC_REG_SCSCR2_TIE;       // Enable xmit interrupt
            HAL_WRITE_UINT8(sh_chan->ctrl_base+SCIF_SCSCR, _scr);
            sh_chan->tx_enabled = true;
        }
        cyg_drv_isr_unlock();
    }
}

// Disable the transmitter on the device
static void 
sh3_scif_stop_xmit(serial_channel *chan)
{
    cyg_uint8 _scr;
    sh3_scif_info *sh_chan = (sh3_scif_info *)chan->dev_priv;

    // Mask interrupts while changing the CR since a rx interrupt or
    // another thread doing the same in the middle of this would
    // result in a bad CR state.
    cyg_drv_isr_lock();
    {
            HAL_READ_UINT8(sh_chan->ctrl_base+SCIF_SCSCR, _scr);
            _scr &= ~CYGARC_REG_SCSCR2_TIE;      // Disable xmit interrupt
            HAL_WRITE_UINT8(sh_chan->ctrl_base+SCIF_SCSCR, _scr);
    }
    cyg_drv_isr_unlock();

        sh_chan->tx_enabled = false;
}

// Serial I/O - low level tx interrupt handler (ISR)
static cyg_uint32 
sh3_scif_tx_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    sh3_scif_info *sh_chan = (sh3_scif_info *)chan->dev_priv;
    cyg_uint8 _scr;

    HAL_READ_UINT8(sh_chan->ctrl_base+SCIF_SCSCR, _scr);
    _scr &= ~CYGARC_REG_SCSCR2_TIE;      // mask out tx interrupts
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCIF_SCSCR, _scr);

    return CYG_ISR_CALL_DSR;  // Cause DSR to be run
}

// Serial I/O - high level tx interrupt handler (DSR)
static void       
sh3_scif_tx_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    sh3_scif_info *sh_chan = (sh3_scif_info *)chan->dev_priv;
    xmt_req_reply_t _block_status = CYG_XMT_DISABLED;
    cyg_uint16 _fdr, _sr;
    int _space, _chars_avail;
    unsigned char* _chars;
    CYG_ADDRWORD _base = sh_chan->ctrl_base;

    // Always check if we're supposed to be enabled; the driver runs
    // with DSRs disabled, and a DSR may have been posted (but not
    // executed) before the interrupt was masked.
    if (!sh_chan->tx_enabled)
        return;

    // How many chars can we stuff into the FIFO?
    HAL_READ_UINT16(_base+SCIF_SCFDR, _fdr);
    _space = 16 - ((_fdr & CYGARC_REG_SCFDR2_TCOUNT_MASK) >> CYGARC_REG_SCFDR2_TCOUNT_shift);

    // Try to do the transfer most efficiently
    _block_status = (chan->callbacks->data_xmt_req)(chan, _space,
                                                    &_chars_avail, &_chars);
    if (CYG_XMT_OK == _block_status) {
        // Transfer the data in block(s).
        do {
            int i = _chars_avail;
            while (i--) {
                HAL_WRITE_UINT8(sh_chan->ctrl_base+SCIF_SCFTDR, *_chars++);
                _space--;
            }
            (chan->callbacks->data_xmt_done)(chan, _chars_avail);
        } while (_space > 0 && 
                 (CYG_XMT_OK == (chan->callbacks->data_xmt_req)(chan, _space,
                                                                &_chars_avail,
                                                                &_chars)));
    } else if (CYG_XMT_DISABLED == _block_status) {
        // Transfer char-by-char
        while (_space--)
            (chan->callbacks->xmt_char)(chan);
    }

    // Clear FIFO-empty/transmit end flags (read back sr first)
    HAL_READ_UINT16(sh_chan->ctrl_base+SCIF_SCSSR, _sr);
    HAL_WRITE_UINT16(sh_chan->ctrl_base+SCIF_SCSSR,
                     CYGARC_REG_SCSSR2_CLEARMASK & ~CYGARC_REG_SCSSR2_TDFE);

    if (sh_chan->tx_enabled) {
        cyg_uint8 _scr;
        HAL_READ_UINT8(sh_chan->ctrl_base+SCIF_SCSCR, _scr);
        _scr |= CYGARC_REG_SCSCR2_TIE;       // unmask tx interrupts
        HAL_WRITE_UINT8(sh_chan->ctrl_base+SCIF_SCSCR, _scr);
    }
}

// Serial I/O - low level RX interrupt handler (ISR)
static cyg_uint32 
sh3_scif_rx_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    sh3_scif_info *sh_chan = (sh3_scif_info *)chan->dev_priv;
    cyg_uint8 _scr;

    HAL_READ_UINT8(sh_chan->ctrl_base+SCIF_SCSCR, _scr);
    _scr &= ~CYGARC_REG_SCSCR2_RIE;     // mask rx interrupts
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCIF_SCSCR, _scr);
    return CYG_ISR_CALL_DSR;            // Cause DSR to be run
}

// Serial I/O - high level rx interrupt handler (DSR)
static void       
sh3_scif_rx_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    sh3_scif_info *sh_chan = (sh3_scif_info *)chan->dev_priv;
    cyg_uint8 _scr;
    cyg_uint16 _fdr, _sr;
    int _avail, _space_avail;
    unsigned char* _space;
    rcv_req_reply_t _block_status;

    HAL_READ_UINT16(sh_chan->ctrl_base+SCIF_SCFDR, _fdr);
    HAL_READ_UINT16(sh_chan->ctrl_base+SCIF_SCSSR, _sr);

    _avail = _fdr & CYGARC_REG_SCFDR2_RCOUNT_MASK;
    CYG_ASSERT(_avail > 0, "No data to be read in RX DSR");

    _block_status = (chan->callbacks->data_rcv_req)(chan, _avail, 
                                                    &_space_avail, &_space);
    if (CYG_RCV_OK == _block_status) {
        // Transfer the data in block(s).
        do {
            int i = _space_avail;
            while(i--) {
                cyg_uint8 _c;
                HAL_READ_UINT8(sh_chan->ctrl_base+SCIF_SCFRDR, _c);
                *_space++ = _c;
                _avail--;
            }
            (chan->callbacks->data_rcv_done)(chan, _space_avail);
        } while (_avail > 0 &&
                 (CYG_RCV_OK == (chan->callbacks->data_rcv_req)(chan, _avail, 
                                                                &_space_avail,
                                                                &_space)));
    } else {
        // Transfer the data char-by-char both for CYG_RCV_FULL and
        // CYG_RCV_DISABLED, leaving all policy decisions with the IO
        // driver.
        while(_avail--) {
            cyg_uint8 _c;
            HAL_READ_UINT8(sh_chan->ctrl_base+SCIF_SCFRDR, _c);
            (chan->callbacks->rcv_char)(chan, _c);
        }
    }

    // Clear buffer full flag (read back first)
    HAL_READ_UINT16(sh_chan->ctrl_base+SCIF_SCSSR, _sr);
    HAL_WRITE_UINT16(sh_chan->ctrl_base+SCIF_SCSSR, 
                     CYGARC_REG_SCSSR2_CLEARMASK & ~(CYGARC_REG_SCSSR2_RDF|CYGARC_REG_SCSSR2_DR));

    HAL_READ_UINT8(sh_chan->ctrl_base+SCIF_SCSCR, _scr);
    _scr |= CYGARC_REG_SCSCR2_RIE;       // unmask rx interrupts
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCIF_SCSCR, _scr);
}

// Serial I/O - low level error interrupt handler (ISR)
static cyg_uint32 
sh3_scif_er_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    sh3_scif_info *sh_chan = (sh3_scif_info *)chan->dev_priv;
    cyg_uint8 _scr;

    HAL_READ_UINT8(sh_chan->ctrl_base+SCIF_SCSCR, _scr);
    _scr &= ~CYGARC_REG_SCSCR2_RIE;      // mask rx interrupts
    HAL_WRITE_UINT8(sh_chan->ctrl_base+SCIF_SCSCR, _scr);
    return CYG_ISR_CALL_DSR;  // Cause DSR to be run
}

// Serial I/O - high level error interrupt handler (DSR)
static void       
sh3_scif_er_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel *chan = (serial_channel *)data;
    sh3_scif_info *sh_chan = (sh3_scif_info *)chan->dev_priv;
    cyg_uint16 _ssr, _ssr2;
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
    cyg_serial_line_status_t stat;
#endif

    HAL_READ_UINT16(sh_chan->ctrl_base+SCIF_SCSSR, _ssr);
    _ssr2 = CYGARC_REG_SCSSR_CLEARMASK;

    if (_ssr & CYGARC_REG_SCSSR2_FER) {
        _ssr2 &= ~CYGARC_REG_SCSSR2_FER;
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
        stat.which = CYGNUM_SERIAL_STATUS_FRAMEERR;
        (chan->callbacks->indicate_status)(chan, &stat );
#endif
    }
    if (_ssr & CYGARC_REG_SCSSR2_PER) {
        _ssr2 &= ~CYGARC_REG_SCSSR2_PER;
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
        stat.which = CYGNUM_SERIAL_STATUS_PARITYERR;
        (chan->callbacks->indicate_status)(chan, &stat );
#endif
    }
    if (_ssr & CYGARC_REG_SCSSR2_BRK) {
        // don't bother clearing - it will be set immediately anyway
        // _ssr2 &= ~CYGARC_REG_SCSSR2_BRK;
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
        stat.which = CYGNUM_SERIAL_STATUS_BREAK;
        (chan->callbacks->indicate_status)(chan, &stat );
#endif
    }
    HAL_WRITE_UINT16(sh_chan->ctrl_base+SCIF_SCSSR, _ssr2);
}

#endif // ifdef CYGDAT_IO_SERIAL_SH_SCIF_INL
