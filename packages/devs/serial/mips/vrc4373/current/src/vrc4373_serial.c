//==========================================================================
//
//      io/serial/mips/vrc4373_serial.c
//
//      Mips VRC4373 Serial I/O Interface Module (interrupt driven)
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
// Author(s):    gthomas
// Contributors: gthomas
// Date:         1999-04-15
// Purpose:      VRC4373 Serial I/O module (interrupt driven version)
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/io_serial.h>
#include <pkgconf/io.h>
#include <cyg/io/io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/io/devtab.h>
#include <cyg/io/serial.h>

#ifdef CYGPKG_IO_SERIAL_MIPS_VRC4373

#include "vrc4373_serial.h"

#if defined(CYGPKG_HAL_MIPS_LSBFIRST)
#define VRC4373_SCC_BASE 0xC1000000
#elif defined(CYGPKG_HAL_MIPS_MSBFIRST)
#define VRC4373_SCC_BASE 0xC1000003
#else
#error MIPS endianness not defined by configuration
#endif

#define VRC4373_SCC_INT  CYGNUM_HAL_INTERRUPT_DUART
#define SCC_CHANNEL_A             4
#define SCC_CHANNEL_B             0

extern void diag_printf(const char *fmt, ...);

typedef struct vrc4373_serial_info {
    CYG_ADDRWORD   base;
    unsigned char  regs[16];   // Known register state (since hardware is write-only!)
} vrc4373_serial_info;

static bool vrc4373_serial_init(struct cyg_devtab_entry *tab);
static bool vrc4373_serial_putc(serial_channel *chan, unsigned char c);
static Cyg_ErrNo vrc4373_serial_lookup(struct cyg_devtab_entry **tab, 
                                       struct cyg_devtab_entry *sub_tab,
                                       const char *name);
static unsigned char vrc4373_serial_getc(serial_channel *chan);
static bool vrc4373_serial_set_config(serial_channel *chan, cyg_serial_info_t *config);
static void vrc4373_serial_start_xmit(serial_channel *chan);
static void vrc4373_serial_stop_xmit(serial_channel *chan);

static cyg_uint32 vrc4373_serial_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       vrc4373_serial_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);

static SERIAL_FUNS(vrc4373_serial_funs, 
                   vrc4373_serial_putc, 
                   vrc4373_serial_getc,
                   vrc4373_serial_set_config,
                   vrc4373_serial_start_xmit,
                   vrc4373_serial_stop_xmit
    );

#ifdef CYGPKG_IO_SERIAL_MIPS_VRC4373_SERIAL0
static vrc4373_serial_info vrc4373_serial_info0 = {VRC4373_SCC_BASE+SCC_CHANNEL_A};
#if CYGNUM_IO_SERIAL_MIPS_VRC4373_SERIAL0_BUFSIZE > 0
static unsigned char vrc4373_serial_out_buf0[CYGNUM_IO_SERIAL_MIPS_VRC4373_SERIAL0_BUFSIZE];
static unsigned char vrc4373_serial_in_buf0[CYGNUM_IO_SERIAL_MIPS_VRC4373_SERIAL0_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(vrc4373_serial_channel0,
                                       vrc4373_serial_funs, 
                                       vrc4373_serial_info0,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_MIPS_VRC4373_SERIAL0_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &vrc4373_serial_out_buf0[0], sizeof(vrc4373_serial_out_buf0),
                                       &vrc4373_serial_in_buf0[0], sizeof(vrc4373_serial_in_buf0)
    );
#else
static SERIAL_CHANNEL(vrc4373_serial_channel0,
                      vrc4373_serial_funs, 
                      vrc4373_serial_info0,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_MIPS_VRC4373_SERIAL0_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(vrc4373_serial_io0, 
             CYGDAT_IO_SERIAL_MIPS_VRC4373_SERIAL0_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             vrc4373_serial_init, 
             vrc4373_serial_lookup,     // Serial driver may need initializing
             &vrc4373_serial_channel0
    );
#endif //  CYGPKG_IO_SERIAL_MIPS_VRC4373_SERIAL0

#ifdef CYGPKG_IO_SERIAL_MIPS_VRC4373_SERIAL1
static vrc4373_serial_info vrc4373_serial_info1 = {VRC4373_SCC_BASE+SCC_CHANNEL_B};
#if CYGNUM_IO_SERIAL_MIPS_VRC4373_SERIAL1_BUFSIZE > 0
static unsigned char vrc4373_serial_out_buf1[CYGNUM_IO_SERIAL_MIPS_VRC4373_SERIAL1_BUFSIZE];
static unsigned char vrc4373_serial_in_buf1[CYGNUM_IO_SERIAL_MIPS_VRC4373_SERIAL1_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(vrc4373_serial_channel1,
                                       vrc4373_serial_funs, 
                                       vrc4373_serial_info1,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_MIPS_VRC4373_SERIAL1_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &vrc4373_serial_out_buf1[0], sizeof(vrc4373_serial_out_buf1),
                                       &vrc4373_serial_in_buf1[0], sizeof(vrc4373_serial_in_buf1)
    );
#else
static SERIAL_CHANNEL(vrc4373_serial_channel1,
                      vrc4373_serial_funs, 
                      vrc4373_serial_info1,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_MIPS_VRC4373_SERIAL1_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(vrc4373_serial_io1, 
             CYGDAT_IO_SERIAL_MIPS_VRC4373_SERIAL1_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             vrc4373_serial_init, 
             vrc4373_serial_lookup,     // Serial driver may need initializing
             &vrc4373_serial_channel1
    );
#endif //  CYGPKG_IO_SERIAL_MIPS_VRC4373_SERIAL1

static cyg_interrupt  vrc4373_serial_interrupt;
static cyg_handle_t   vrc4373_serial_interrupt_handle;

// Table which maps hardware channels (A,B) to software ones
struct serial_channel *vrc4373_chans[] = {
#ifdef CYGPKG_IO_SERIAL_MIPS_VRC4373_SERIAL0    // Hardware channel A
    &vrc4373_serial_channel0,        
#else
    0,
#endif
#ifdef CYGPKG_IO_SERIAL_MIPS_VRC4373_SERIAL1    // Hardware channel B
    &vrc4373_serial_channel1,
#else
    0,
#endif
};

// Support functions which access the serial device.  Note that this chip requires
// a substantial delay after each access. 

#define SCC_DELAY 100
inline static void
scc_delay(void)
{
    int i;
    for (i = 0;  i < SCC_DELAY;  i++) ;
}

inline static void
scc_write_reg(volatile unsigned char *reg, unsigned char val)
{
    scc_delay();
    *reg = val;
}

inline static unsigned char
scc_read_reg(volatile unsigned char *reg)
{
    unsigned char val;
    scc_delay();
    val = *reg;
    return (val);
}

inline static unsigned char
scc_read_ctl(volatile struct serial_port *port, int reg)
{
    if (reg != 0) {
        scc_write_reg(&port->scc_ctl, reg);
    }       
    return (scc_read_reg(&port->scc_ctl));
}

inline static void
scc_write_ctl(volatile struct serial_port *port, int reg, unsigned char val)
{
    if (reg != 0) {
        scc_write_reg(&port->scc_ctl, reg);
    }       
    scc_write_reg(&port->scc_ctl, val);
}

inline static unsigned char
scc_read_dat(volatile struct serial_port *port)
{
    return (scc_read_reg(&port->scc_dat));
}

inline static void
scc_write_dat(volatile struct serial_port *port, unsigned char val)
{
    scc_write_reg(&port->scc_dat, val);
}

// Internal function to actually configure the hardware to desired baud rate, etc.
static bool
vrc4373_serial_config_port(serial_channel *chan, cyg_serial_info_t *new_config, bool init)
{
    vrc4373_serial_info *vrc4373_chan = (vrc4373_serial_info *)chan->dev_priv;
    volatile struct serial_port *port = (volatile struct serial_port *)vrc4373_chan->base;
    cyg_int32 baud_rate = select_baud[new_config->baud];
    cyg_int32 baud_divisor;
    unsigned char *regs = &vrc4373_chan->regs[0];
    if (baud_rate == 0) return false;
    // Compute state of registers.  The register/control state needs to be kept in
    // the shadow variable 'regs' because the hardware registers can only be written,
    // not read (in general).
    if (init) {
        // Insert appropriate resets?
        if (chan->out_cbuf.len != 0) {
            regs[R1] = WR1_IntAllRx;
            regs[R9] = WR9_MIE | WR9_NoVector;
        } else {
            regs[R1] = 0;
            regs[R9] = 0;
        }
        // Clocks are from the baud rate generator
        regs[R11] = WR11_TRxCBR | WR11_TRxCOI | WR11_TxCBR | WR11_RxCBR;  
        regs[R14] = WR14_BRenable | WR14_BRSRC;
        regs[R10] = 0;    // Unused in this [async] mode
        regs[R15] = 0;
    }
    regs[R3] = WR3_RxEnable | select_word_length_WR3[new_config->word_length - CYGNUM_SERIAL_WORD_LENGTH_5];
    regs[R4] = WR4_X16CLK | select_stop_bits[new_config->stop] | select_parity[new_config->parity];
    regs[R5] = WR5_TxEnable  | select_word_length_WR5[new_config->word_length - CYGNUM_SERIAL_WORD_LENGTH_5];
    baud_divisor = BRTC(baud_rate);
    regs[R12] = baud_divisor & 0xFF;
    regs[R13] = baud_divisor >> 8;
    // Now load the registers
    scc_write_ctl(port, R4, regs[R4]);
    scc_write_ctl(port, R10, regs[R10]);
    scc_write_ctl(port, R3, regs[R3] & ~WR3_RxEnable);
    scc_write_ctl(port, R5, regs[R5] & ~WR5_TxEnable);
    scc_write_ctl(port, R1, regs[R1]);
    scc_write_ctl(port, R9, regs[R9]);
    scc_write_ctl(port, R11, regs[R11]);
    scc_write_ctl(port, R12, regs[R12]);
    scc_write_ctl(port, R13, regs[R13]);
    scc_write_ctl(port, R14, regs[R14]);
    scc_write_ctl(port, R15, regs[R15]);
    scc_write_ctl(port, R3, regs[R3]);
    scc_write_ctl(port, R5, regs[R5]);
    // Update configuration
    if (new_config != &chan->config) {
        chan->config = *new_config;
    }
    return true;
}

// Function to initialize the device.  Called at bootstrap time.
static bool 
vrc4373_serial_init(struct cyg_devtab_entry *tab)
{
    serial_channel *chan = (serial_channel *)tab->priv;
    vrc4373_serial_info *vrc4373_chan = (vrc4373_serial_info *)chan->dev_priv;
    static bool init = false;
#ifdef CYGDBG_IO_INIT
    diag_printf("VRC4373 SERIAL init '%s' - dev: %x\n", tab->name, vrc4373_chan->base);
#endif
    (chan->callbacks->serial_init)(chan);  // Really only required for interrupt driven devices
    if (!init && chan->out_cbuf.len != 0) {
        init = true;
        cyg_drv_interrupt_create(VRC4373_SCC_INT,
                                 99,                     
                                 (cyg_addrword_t)VRC4373_SCC_BASE+SCC_CHANNEL_A,
                                 vrc4373_serial_ISR,
                                 vrc4373_serial_DSR,
                                 &vrc4373_serial_interrupt_handle,
                                 &vrc4373_serial_interrupt);
        cyg_drv_interrupt_attach(vrc4373_serial_interrupt_handle);
        cyg_drv_interrupt_unmask(VRC4373_SCC_INT);
    }
    vrc4373_serial_config_port(chan, &chan->config, true);
    return true;
}

// This routine is called when the device is "looked" up (i.e. attached)
static Cyg_ErrNo 
vrc4373_serial_lookup(struct cyg_devtab_entry **tab, 
                  struct cyg_devtab_entry *sub_tab,
                  const char *name)
{
    serial_channel *chan = (serial_channel *)(*tab)->priv;
    (chan->callbacks->serial_init)(chan);  // Really only required for interrupt driven devices
    return ENOERR;
}

// Send a character to the device output buffer.
// Return 'true' if character is sent to device
static bool
vrc4373_serial_putc(serial_channel *chan, unsigned char c)
{
    vrc4373_serial_info *vrc4373_chan = (vrc4373_serial_info *)chan->dev_priv;
    volatile struct serial_port *port = (volatile struct serial_port *)vrc4373_chan->base;
    if (scc_read_ctl(port, R0) & RR0_TxEmpty) {
// Transmit buffer is empty
        scc_write_dat(port, c);
        return true;
    } else {
// No space
        return false;
    }
}

// Fetch a character from the device input buffer, waiting if necessary
static unsigned char 
vrc4373_serial_getc(serial_channel *chan)
{
    unsigned char c;
    vrc4373_serial_info *vrc4373_chan = (vrc4373_serial_info *)chan->dev_priv;
    volatile struct serial_port *port = (volatile struct serial_port *)vrc4373_chan->base;
    while ((scc_read_ctl(port, R0) & RR0_RxAvail) == 0) ;   // Wait for char
    c = scc_read_dat(port);
    return c;
}

// Set up the device characteristics; baud rate, etc.
static bool 
vrc4373_serial_set_config(serial_channel *chan, cyg_serial_info_t *config)
{
    return vrc4373_serial_config_port(chan, config, false);
}

// Enable the transmitter on the device
static void
vrc4373_serial_start_xmit(serial_channel *chan)
{
    vrc4373_serial_info *vrc4373_chan = (vrc4373_serial_info *)chan->dev_priv;
    volatile struct serial_port *port = (volatile struct serial_port *)vrc4373_chan->base;
    if ((vrc4373_chan->regs[R1] & WR1_TxIntEnab) == 0) {
        CYG_INTERRUPT_STATE old;
        HAL_DISABLE_INTERRUPTS(old);
        vrc4373_chan->regs[R1] |= WR1_TxIntEnab;  // Enable Tx interrupt
        scc_write_ctl(port, R1, vrc4373_chan->regs[R1]);
        (chan->callbacks->xmt_char)(chan);  // Send first character to start xmitter
        HAL_RESTORE_INTERRUPTS(old);
    }
}

// Disable the transmitter on the device
static void 
vrc4373_serial_stop_xmit(serial_channel *chan)
{
    vrc4373_serial_info *vrc4373_chan = (vrc4373_serial_info *)chan->dev_priv;
    volatile struct serial_port *port = (volatile struct serial_port *)vrc4373_chan->base;
    if ((vrc4373_chan->regs[R1] & WR1_TxIntEnab) != 0) {
        CYG_INTERRUPT_STATE old;
        HAL_DISABLE_INTERRUPTS(old);
        vrc4373_chan->regs[R1] &= ~WR1_TxIntEnab;  // Disable Tx interrupt
        scc_write_ctl(port, R1, vrc4373_chan->regs[R1]);
        HAL_RESTORE_INTERRUPTS(old);
    }
}

// Serial I/O - low level interrupt handler (ISR)
static cyg_uint32 
vrc4373_serial_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_drv_interrupt_mask(VRC4373_SCC_INT);
    cyg_drv_interrupt_acknowledge(VRC4373_SCC_INT);
    return CYG_ISR_CALL_DSR;  // Cause DSR to be run
}

inline static void
vrc4373_int(serial_channel *chan, unsigned char stat)
{
    vrc4373_serial_info *vrc4373_chan = (vrc4373_serial_info *)chan->dev_priv;
    volatile struct serial_port *port = (volatile struct serial_port *)vrc4373_chan->base;
    // Note: 'stat' value is interrupt status register, shifted into "B" position
    if (stat & RR3_BRxIP) {
        // Receive interrupt
        unsigned char c;
        c = scc_read_dat(port);
        (chan->callbacks->rcv_char)(chan, c);
    }
    if (stat & RR3_BTxIP) {
        // Transmit interrupt
        (chan->callbacks->xmt_char)(chan);
    }
    if (stat & RR3_BExt) {
        // Status interrupt (parity error, framing error, etc)
    }
}

// Serial I/O - high level interrupt handler (DSR)
// Note: This device presents a single interrupt for both channels.  Thus the
// interrupt handler has to query the device and decide which channel needs service.
// Additionally, more than one interrupt condition may be present so this needs to
// be done in a loop until all interrupt requests have been handled.
// Also note that the hardware is rather broken.  The interrupt status needs to
// be read using only channel A (pointed to by 'data')
static void       
vrc4373_serial_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    serial_channel *chan;
    volatile struct serial_port *port = (volatile struct serial_port *)data;
    unsigned char stat;
    while (true) {
        stat = scc_read_ctl(port, R3);
        if (stat & (RR3_AExt | RR3_ATxIP | RR3_ARxIP)) {
            chan = vrc4373_chans[0];  // Hardware channel A
            vrc4373_int(chan, stat>>3);  // Handle interrupt
        } else if (stat & (RR3_BExt | RR3_BTxIP | RR3_BRxIP)) {
            chan = vrc4373_chans[1];  // Hardware channel A
            vrc4373_int(chan, stat);  // Handle interrupt
        } else {
            // No more interrupts, all done
            break;
        }
    }
    cyg_drv_interrupt_unmask(VRC4373_SCC_INT);
}
#endif
