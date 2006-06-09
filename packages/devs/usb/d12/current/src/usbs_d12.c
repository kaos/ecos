//==========================================================================
//
//      usbs_d12.c
//
//      Driver for the D12 USB Slave Board
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Ltd
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Frank M. Pagliughi (fmp)
// Date:         2004-05-22
//
// This code is a device driver for the SoRo Systems USB-D12-104, a PC/104
// (ISA) Full-Speed USB slave board, which turns a PC/104 stack into a USB
// slave device. The board contains a Philips PDIUSBD12 Peripheral Controller
// Chip mapped into the PC's I/O space, with jumper-selectable I/O base 
// address, IRQ, and DMA settings. The eCos config tool is used to adjust
// settings for this driver to match the physical jumper settings. The chip
// could run in polled mode without an IRQ, but this wouldn't be a great idea
// other than maybe a debug environment. 
//
// The board supports DMA transfers over the Main endpoint, but I temporarily
// removed that code to make the driver portable to other platforms.
//
// *** This driver should also work with the Philips ISA Eval Board
//     for the D12, but I couldn't get one of them from Philips, so
//     I couldn't test it.
//
// The D12 uses an indexed register set, which it describes as "commands." 
// You first write a command (index) to the command register then you can
// read or write data to that register. Each multi-byte command read or write
// must be dione atomically, so all access to the chip must be serialized.
// 
// The D12 requests service through a single interrupt. The driver can
// be configured to service the chip through a DSR or a thread. In either
// case, the "service" code assumes it has unfettered access to the chip.
// The interrupt, therefore never touches the chip. It just schedules the
// DSR or service thread.
// Currently, the code gets exclusive access to the chip by locking the
// scheduler. This is suboptimal (locking the whole OS to touch one I/O 
// chip), and better method should be explored.
//
// This version of the driver does not support Isocronous transfers.
// 
// Additional notes on the D12:
//
// - The D12 has 4 endpoints (2 IN, and 2 OUT) in addition to the main 
//   control endpoint:
//   - Endp 0 (Control In & Out, 16 byte buffer)
//   - Endp 1 (IN & OUT, Bulk or Interrupt, 16 byte ea)
//   - Endp 2 (IN and/or OUT, Bulk, Interrupt, or Isoc, 64 bytes ea)
//
// - The "main" endpoint (as Philips calls it) is Endp 2. It's double
//   buffered and has a DMA interface, and thus, is suited for high
//   throughput. For applications that perform either Isoc In or Out,
//   the buffers for Endp 2 can be combined for a 128 byte space.
//   This driver, however, currently does not support this.
//
// - There may be a flaw in the double buffering of the rx main endpoint. 
//   According to the documentation it should be invisible to the software,
//   but if both buffers fill (on an rx/OUT), they must both be read 
//   together, otherwise it appears that the buffers/packets are returned
//   in reverse order. ReadMainEndpointBuf() returns the data properly.
//
// - All the interrupt sources on the chip - individual endpoints, bus reset,
//   suspend, and DMA - are OR'ed together and can be checked via the 
//   interrupt status register. When using edge-sensitive interrupts, as
//   we do here, the ISR/DSR must be sure all interrupts are cleared before
//   returning otherwise no new interrupts will be latched.
//
// - If the DMA controller is not used for the Main Endpoint, you MUST enable
//   the main endpoint interrupts in the DMA register (bits 6 & 7).
//   Personally, I think this should be the default at reset, to make it
//   compatible with the other endpoints, but Philips didn't see it that
//   way.
// 
// - When a Setup (Device Request) packet arrives in the control endpoint, a
//   bit is set in the endpoint's status register indicating the packet is
//   setup and not data. By the USB standard, a setup packet can not be
//   NAK'ed or STALL'ed, so when the chip receives a setup packet, it 
//   flushes the Ctrl (EP0) IN buffer and disables the Validate and Clear
//   Buffer commands. We must send an "acknowledge setup" to both
//   EP0 IN and OUT before a Validate or Clear Buffer command is effective.
//   See ReadSetupPacket().
//
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/cyg_trac.h>
#include <cyg/infra/diag.h>

#include <pkgconf/devs_usb_d12.h>

#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/error/codes.h>

#include <cyg/io/usb/usb.h>
#include <cyg/io/usb/usbs.h>

#include <string.h>

// --------------------------------------------------------------------------
// Common Types
// --------------------------------------------------------------------------

typedef cyg_uint8       byte;
typedef cyg_uint8       uint8;
typedef cyg_int16       int16;
typedef cyg_uint16      uint16;
typedef cyg_int32       int32;
typedef cyg_uint32      uint32;

// --------------------------------------------------------------------------
// Tracing & Debug
// --------------------------------------------------------------------------
// If the driver is configured to use a thread to service the chip, then it
// can also be configured to dump a lot of debug output.
// Care must be taken that USB timing requirements are not violated by 
// dumping debug info. If the data is sent to a serial port, it should use
// a hardware driver and have a large output buffer (115200 baud & 2kB
// buffer works for me).

#if defined(CYGFUN_DEVS_USB_D12_DEBUG) && CYGFUN_DEVS_USB_D12_DEBUG
#define TRACE_D12 diag_printf
#else
#define TRACE_D12 (1) ? (void)0 : diag_printf
#endif

#if defined(CYGSEM_DEVS_USB_D12_DEBUG_DUMP_EP0_BUFS) && CYGSEM_DEVS_USB_D12_DEBUG_DUMP_EP0_BUFS
#define TRACE_EP0       1
#endif

#if defined(CYGSEM_DEVS_USB_D12_DEBUG_DUMP_BUFS) && CYGSEM_DEVS_USB_D12_DEBUG_DUMP_BUFS
#define TRACE_EP        1
#endif

#if defined(TRACE_EP0) || defined(TRACE_EP)
static void _trace_buf(const char *hdr, const byte* buf, unsigned n)
{
  unsigned i;
  
  if (buf != 0 && n != 0) {
    if (hdr && hdr[0])
      TRACE_D12("%s ", hdr);
    
    TRACE_D12("[");
    for (i=0; i<n; i++) 
      TRACE_D12(" x%02X", buf[i]);
    TRACE_D12("]\n");
  }
}
#endif

#if defined(TRACE_EP0)
#define TRACE_BUF0      _trace_buf
#else   
#define TRACE_BUF0(hdr, buf, n)
#endif

#if defined(TRACE_EP)
#define TRACE_BUF       _trace_buf
#else   
#define TRACE_BUF(hdr, buf, n)
#endif

// ==========================================================================
// Chip Wrapper
// ==========================================================================

// This section contains functions that wrapper the low-level access to the 
// chip. There's a function around each register access on the chip, and then
// some.

#if defined(CYGSEM_DEVS_USB_D12_IO_MAPPED)
typedef void* d12_addr_type;
#else
typedef byte* d12_addr_type;
#endif

#define D12_BASE_ADDR   ((d12_addr_type) CYGNUM_DEVS_USB_D12_BASEADDR)

#define D12_ENDP0_SIZE       16 // Size of Ctrl Endp
#define D12_MAIN_ENDP         2 // The D12's "Main" Endp is special, double buffered
#define D12_MAIN_ENDP_SIZE   64 // Size of each main endp buffer
#define D12_MAX_PACKET_SIZE 128 // Max packet is actually double main endp

#define D12_CHIP_ID      0x1012 // Value that's returned by a read of
                                //the D12's Chip ID register

// ----- Endpoint Indices -----

enum {
  D12_ENDP_INVALID = 0xFF,
  D12_ENDP_MIN     = 0,
  
  D12_RX_CTRL_ENDP = D12_ENDP_MIN,                // Rx/Tx Nomenclature
  D12_TX_CTRL_ENDP,
  
  D12_RX_ENDP0     = D12_ENDP_MIN,
  D12_TX_ENDP0,
  D12_RX_ENDP1,
  D12_TX_ENDP1,
  D12_RX_ENDP2,
  D12_TX_ENDP2,
  D12_RX_MAIN_ENDP = D12_RX_ENDP2,
  D12_TX_MAIN_ENDP = D12_TX_ENDP2,
  
  
  D12_CTRL_ENDP_OUT = D12_ENDP_MIN,               // IN/OUT Nomenclature
  D12_CTRL_ENDP_IN,
  
  D12_ENDP0_OUT     = D12_ENDP_MIN,
  D12_ENDP0_IN,
  D12_ENDP1_OUT,
  D12_ENDP1_IN,
  D12_ENDP2_OUT,
  D12_ENDP2_IN,
  D12_MAIN_ENDP_OUT = D12_ENDP2_OUT,
  D12_MAIN_ENDP_IN  = D12_ENDP2_IN,
  
  D12_ENDP_INSERT_BEFORE,
  D12_ENDP_MAX      = D12_ENDP_INSERT_BEFORE-1
};

// ----- Set Mode Reg configuration byte -----

enum {  
  D12_MODE_CFG_NO_LAZYCLOCK       = 0x02,
  D12_MODE_CFG_CLOCK_RUNNING      = 0x04,
  D12_MODE_CFG_INTERRUPT          = 0x08,
  D12_MODE_CFG_SOFT_CONNECT       = 0x10,
  
  D12_MODE_CFG_NON_ISO            = 0x00,
  D12_MODE_CFG_ISO_OUT            = 0x40,
  D12_MODE_CFG_ISO_IN             = 0x80,
  D12_MODE_CFG_ISO_IO             = 0xC0,
  
  D12_MODE_CFG_DFLT               = (D12_MODE_CFG_NO_LAZYCLOCK |
				     D12_MODE_CFG_CLOCK_RUNNING | 
				     D12_MODE_CFG_NON_ISO)
};

// ----- Set Mode Reg clock div factor -----

enum {
  D12_MODE_CLK_24_MHZ                     = 1,
  D12_MODE_CLK_16_MHZ                     = 2,
  D12_MODE_CLK_12_MHZ                     = 3,
  D12_MODE_CLK_8_MHZ                      = 5,
  D12_MODE_CLK_6_MHZ                      = 7,
  D12_MODE_CLK_4_MHZ                      = 11,
  
  D12_MODE_CLK_DIV_MASK                   = 0x0F,
  
  D12_MODE_CLK_SET_TO_ONE                 = 0x40,
  D12_MODE_CLK_SOF_ONLY_INTR              = 0x80,
  
  D12_MODE_CLK_DFLT                       = (D12_MODE_CLK_4_MHZ | 
					     D12_MODE_CLK_SET_TO_ONE)
};

// ----- Set DMA Register -----

enum {
  D12_DMA_SINGLE_CYCLE,
  D12_DMA_BURST_4_CYCLE,
  D12_DMA_BURST_8_CYCLE,
  D12_DMA_BURST_16_CYCLE,
  
  D12_DMA_ENABLE                          = 0x04,
  D12_DMA_DIR_WRITE                       = 0x08,
  D12_DMA_DIR_READ                        = 0x00,
  D12_DMA_AUTO_RELOAD                     = 0x10,
  D12_DMA_INTR_PIN_MODE                   = 0x20,
  
  D12_DMA_MAIN_ENDP_OUT_INTR_ENABLE       = 0x40,
  D12_DMA_MAIN_RX_ENDP_INTR_ENABLE        = 0x40,
  
  D12_DMA_MAIN_ENDP_IN_INTR_ENABLE        = 0x80,
  D12_DMA_MAIN_TX_ENDP_INTR_ENABLE        = 0x80,
  
  D12_DMA_MAIN_ENDP_INTR_ENABLE           = 0xC0  // Enables IN & OUT Intr
};

// ----- Interrupt Register Bits -----

enum {
  D12_INTR_RX_CTRL_ENDP           = 0x0001,
  D12_INTR_TX_CTRL_ENDP           = 0x0002,
  
  D12_INTR_RX_ENDP0               = D12_INTR_RX_CTRL_ENDP,
  D12_INTR_TX_ENDP0               = D12_INTR_TX_CTRL_ENDP,
  D12_INTR_RX_ENDP1               = 0x0004,
  D12_INTR_TX_ENDP1               = 0x0008,
  D12_INTR_RX_ENDP2               = 0x0010,
  D12_INTR_TX_ENDP2               = 0x0020,
  
  D12_INTR_BUS_RESET              = 0x0040,
  D12_INTR_SUSPEND_CHANGE         = 0x0080,
  D12_INTR_DMA_EOT                = 0x0100
};

// ----- Read Endpoint Status -----

enum {
  D12_ENDP_STAT_SETUP_PACKET      = 0x04,
  D12_ENDP_STAT_BUF0_FULL         = 0x20,
  D12_ENDP_STAT_BUF1_FULL         = 0x40,
  D12_ENDP_STAT_ANY_BUF_FULL      = 0x60,
  D12_ENDP_STAT_BOTH_BUF_FULL     = 0x60,
  D12_ENDP_STAT_STALL             = 0x80,
};

// ----- Last Transaction Status Bits -----

enum {
  D12_LAST_TRANS_DATA_SUCCESS             = 0x01,
  D12_LAST_TRANS_ERR_CODE_MASK            = 0x1E,
  D12_LAST_TRANS_SETUP_PACKET             = 0x20,
  D12_LAST_TRANS_DATA1_PACKET             = 0x40,
  D12_LAST_TRANS_PREV_STAT_NOT_READ       = 0x80
};

static const byte RX_ENDP_INDEX[] = 
  { D12_RX_ENDP0, D12_RX_ENDP1, D12_RX_ENDP2 };
static const byte TX_ENDP_INDEX[] = 
  { D12_TX_ENDP0, D12_TX_ENDP1, D12_TX_ENDP2 };

static const int RX_ENDP_SIZE[] = { 16, 16, 64 };
static const int TX_ENDP_SIZE[] = { 16, 16, 64 };

typedef void (*completion_fn)(void*, int);

#ifndef USB_SETUP_PACKET_LEN 
#define USB_SETUP_PACKET_LEN    8
#endif

// ----- Command Definitions -----

enum {  
  CMD_SET_ADDR_EN                 = 0xD0,   // Write 1 byte
  CMD_SET_ENDP_EN                 = 0xD8,   // Write 1 byte
  CMD_SET_MODE                    = 0xF3,   // Write 2 bytes
  CMD_SET_DMA                     = 0xFB,   // Write/Read 1 byte
  CMD_READ_INTR_REG               = 0xF4,   // Read 2 bytes
  CMD_SEL_ENDP                    = 0x00,   // (+ Endp Index) Read 1 byte (opt)
  CMD_READ_LAST_TRANS_STAT        = 0x40,   // (+ Endp Index) Read 1 byte (opt)
  CMD_READ_ENDP_STAT              = 0x80,   // (+ Endp Index) Read 1 byte
  CMD_READ_BUF                    = 0xF0,   // Read n bytes
  CMD_WRITE_BUF                   = 0xF0,   // Write n bytes
  CMD_SET_ENDP_STAT               = 0x40,   // (+ Endp Index) Write 1 byte
  CMD_ACK_SETUP                   = 0xF1,   // None
  CMD_CLEAR_BUF                   = 0xF2,   // None
  CMD_VALIDATE_BUF                = 0xFA,   // None
  CMD_SEND_RESUME                 = 0xF6,   // None
  CMD_READ_CURR_FRAME_NUM         = 0xF5,   // Read 1 or 2 bytes
  CMD_READ_CHIP_ID                = 0xFD    // Read 2 bytes
};

// ----- Set Endpoint Enable Register -----

enum {
  ENDP_DISABLE,
  ENDP_ENABLE
};

// ----- Select Endpoint Results -----

enum {
  SEL_ENDP_FULL   = 0x01,
  SEL_ENDP_STALL  = 0x02
};

// ----- Error Codes from ReadLastTrans (need to be bit shifter) -----

enum {
  ERROR_NO_ERROR,
  ERROR_PID_ENCODING,
  ERROR_PID_UNKNOWN,
  ERROR_UNEXPECTED_PACKET,
  ERROR_TOKEN_CRC,
  ERROR_DATA_CRC,
  ERROR_TIMEOUT,
  ERROR_BABBLE,
  ERROR_UNEXPECTED_EOP,
  ERROR_NAK,
  ERROR_PACKET_ON_STALL,
  ERROR_OVERFLOW,
  ERROR_BITSTUFF,
  ERROR_WRONG_DATA_PID
};

// ------------------------------------------------------------------------
// Routines to access the D12 registers. The hardware specific driver 
// provides 8bit access functions and block access functions.

#include CYGIMP_DEVS_USB_D12_HW_ACCESS_HEADER


static inline uint16 
make_word(byte hi, byte lo)
{
  return ((uint16) hi << 8) | lo;
}

// These routines read or write 16 bit values to the data area.

static inline uint16 
d12_read_data_word(d12_addr_type base_addr)
{
  uint16 val = d12_read_data_byte(base_addr);
  val |= ((uint16) d12_read_data_byte(base_addr)) << 8;
  return val;
}

static inline void 
d12_write_data_word(d12_addr_type base_addr, uint16 val)
{
  d12_write_data_byte(base_addr, (byte) val);
  d12_write_data_byte(base_addr, (byte) (val >> 8));
}

// ------------------------------------------------------------------------
// Command & Data I/O
// ------------------------------------------------------------------------
//
// These routines read & write the registers in the D12. The procedure is
// to write a register/command value to the command address (A0=1) then
// read or write any required data a byte at a time to the data address
// (A0=0). The data can be one byte or two. If two, the low byte is read/
// written first.

// NOTE: These MUST be atomic operations. It's up to the caller 
//       to insure this.

// The hardware specific driver provides the basic access function.
//      

static inline void 
d12_write_byte(d12_addr_type base_addr, byte cmd, byte val)
{
  d12_write_cmd(base_addr, cmd);
  d12_write_data_byte(base_addr, val);
}

static inline void 
d12_write_word(d12_addr_type base_addr, byte cmd, uint16 val)
{
  d12_write_cmd(base_addr, cmd);
  d12_write_data_word(base_addr, val);
}

static inline byte 
d12_read_byte(d12_addr_type base_addr, byte cmd)
{
  d12_write_cmd(base_addr, cmd);
  return d12_read_data_byte(base_addr);
}

static inline uint16 
d12_read_word(d12_addr_type base_addr, byte cmd)
{
  d12_write_cmd(base_addr, cmd);
  return d12_read_data_word(base_addr);
}

// ------------------------------------------------------------------------
// Higher Level Commands
// ------------------------------------------------------------------------

// Stalls or Unstalls the endpoint. Bit0=1 for stall, =0 to unstall.

static inline void 
d12_set_endp_status(d12_addr_type base_addr, byte endp_idx, byte stat)
{
  d12_write_byte(base_addr, CMD_SET_ENDP_STAT + endp_idx, stat);
}

// ------------------------------------------------------------------------
// Stalls the control endpoint (both in & out).

static void 
d12_stall_ctrl_endp(d12_addr_type base_addr, bool stall)
{
  d12_set_endp_status(base_addr, D12_TX_CTRL_ENDP, stall ? 1 : 0);
  d12_set_endp_status(base_addr, D12_RX_CTRL_ENDP, stall ? 1 : 0);
}

// ------------------------------------------------------------------------
// Stalls/unstalls the specified endpoint. 

void inline 
d12_stall_endp(d12_addr_type base_addr, byte endp_idx, bool stall)
{
  d12_set_endp_status(base_addr, endp_idx, stall ? 1 : 0);
}

// ------------------------------------------------------------------------ */
// Tells the chip that the selected endpoint buffer has been completely
// read. This should be called after the application reads all the data
// from an endpoint.  While there's data in the buffer the chip will 
// automatically NAK any additional OUT packets from the host. 

static inline void 
d12_clear_buffer(d12_addr_type base_addr)
{
  d12_write_cmd(base_addr, CMD_CLEAR_BUF);
}

// ------------------------------------------------------------------------
// Tells the chip that the data in the selected endpoint buffer is complete
// and ready to be sent to the host.

static inline void 
d12_validate_buffer(d12_addr_type base_addr)
{
  d12_write_cmd(base_addr, CMD_VALIDATE_BUF);
}

// ------------------------------------------------------------------------
// Sends an upstream resume signal for 10ms. This command is normally 
// issued when the device is in suspend.

static inline void 
d12_send_resume(d12_addr_type base_addr)
{
  d12_write_cmd(base_addr, CMD_SEND_RESUME);
}

// ------------------------------------------------------------------------
// Gets the frame number of the last successfully received 
// start-of-frame (SOF).

static inline uint16 
d12_read_curr_frame_num(d12_addr_type base_addr)
{
  return d12_read_word(base_addr, CMD_READ_CURR_FRAME_NUM);
}

// ------------------------------------------------------------------------
// This routine acknowledges a setup packet by writing an Ack Setup command
// to the currently selected Endpoint. This must be done for both EP0 out
// and EP0 IN whenever a setup packet is received.

static inline void 
d12_ack_setup(d12_addr_type base_addr)
{
  d12_write_cmd(base_addr, CMD_ACK_SETUP);
}

// ------------------------------------------------------------------------
// Gets the value of the 16-bit interrupt register, which indicates the 
// source of an interrupt (if interrupts are not used, this reg can be 
// polled to find when service is required).

static inline uint16 
d12_read_intr_reg(d12_addr_type base_addr)
{
  return d12_read_word(base_addr, CMD_READ_INTR_REG) & 0x01FF;
}

// ------------------------------------------------------------------------
// Gets/Sets the contents of the DMA register.

static inline byte 
d12_get_dma(d12_addr_type base_addr)
{
  return d12_read_byte(base_addr, CMD_SET_DMA);
}

static inline void 
d12_set_dma(d12_addr_type base_addr, byte mode)
{
  d12_write_byte(base_addr, CMD_SET_DMA, mode);
}

// ------------------------------------------------------------------------
// Sends the "Select Endpoint" command (0x00 - 0x0D) to the chip.
// This command initializes an internal pointer to the start of the 
// selected buffer.
// 
// Returns: Bitfield containing status of the endpoint

static byte 
d12_select_endp(d12_addr_type base_addr, byte endp_idx)
{
  return d12_read_byte(base_addr, CMD_SEL_ENDP + endp_idx);
}

// ------------------------------------------------------------------------
// Gets the status of the last transaction of the endpoint. It also resets 
// the corresponding interrupt flag in the interrupt register, and clears 
// the status, indicating that it was read.
//
// Returns: Bitfield containing the last transaction status.

static inline byte 
d12_read_last_trans_status(d12_addr_type base_addr, byte endp_idx)
{
  return d12_read_byte(base_addr, CMD_READ_LAST_TRANS_STAT + endp_idx);
}

// ------------------------------------------------------------------------
// Reads the status of the requested endpoint. 
// Just for the heck of it, we mask off the reserved bits.
//
// Returns: Bitfield containing the endpoint status.

static inline byte 
d12_read_endp_status(d12_addr_type base_addr, byte endp_idx)
{
  return d12_read_byte(base_addr, CMD_READ_ENDP_STAT + endp_idx) & 0xE4;
}

// ------------------------------------------------------------------------
// Returns true if there is data available in the specified endpoint's
// ram buffer. This is determined by the buf full flags in the endp status
// register.

static inline bool 
d12_data_available(d12_addr_type base_addr, byte endp_idx)
{
  byte by = d12_read_endp_status(base_addr, endp_idx);
  return (bool) (by & D12_ENDP_STAT_ANY_BUF_FULL);
}

// ------------------------------------------------------------------------
// Clears the transaction status for each of the endpoints by calling the
// d12_read_last_trans_status() function for each. 

static void 
d12_clear_all_intr(d12_addr_type base_addr)
{
  uint8 endp;
  
  d12_read_intr_reg(base_addr);
  
  for (endp=D12_ENDP_MIN; endp<=D12_ENDP_MAX; ++endp)
    d12_read_last_trans_status(base_addr, endp);
}

// ------------------------------------------------------------------------
// Loads a value into the Set Address / Enable register. This sets the 
// device's USB address (lower 7 bits) and enables/disables the function
// (msb).

static void 
d12_set_addr_enable(d12_addr_type base_addr, byte usb_addr, bool enable)
{
  if (enable) 
    usb_addr |= 0x80;
  
  d12_write_byte(base_addr, CMD_SET_ADDR_EN, usb_addr);
}

// ------------------------------------------------------------------------
// Enables/disables the generic endpoints.

static inline void 
d12_set_endp_enable(d12_addr_type base_addr, bool enable)
{
  d12_write_byte(base_addr, CMD_SET_ENDP_EN, 
		 (enable) ? ENDP_ENABLE : ENDP_DISABLE);
}

// ------------------------------------------------------------------------
// Sets the device's configuration and CLKOUT frequency.

static void 
d12_set_mode(d12_addr_type base_addr, byte config, byte clk_div)
{
  uint16 w = make_word(clk_div, config);
  d12_write_word(base_addr, CMD_SET_MODE, w);
}

// ------------------------------------------------------------------------
// Reads a setup packet from the control endpoint. This procedure is 
// somewhat different than reading a data packet. By the USB standard, a 
// setup packet can not be NAK'ed or STALL'ed, so when the chip receives a 
// setup packet, it flushes the Ctrl (EP0) IN buffer and disables the 
// Validate and Clear Buffer commands. The processor must send an 
// acknowledge setup to both EP0 IN and OUT before a Validate or Clear
// Buffer command is effective.
//
// Parameters:
//      buf     buffer to receive the contents of the setup packet. Must
//              be at least 8 bytes.
// Returns:
//      true    if there are 8 bytes waiting in the EP0 OUT RAM buffer
//              on the D12 (i.e., true if successful)
//      false   otherwise

static bool 
d12_read_setup_packet(d12_addr_type base_addr, byte *buf)
{
  uint8 n;
  
  d12_select_endp(base_addr, D12_RX_CTRL_ENDP);
  
  d12_read_byte(base_addr, CMD_READ_BUF);   // Read & discard reserved byte
  n = d12_read_data_byte(base_addr);        // # bytes available
  
  if (n > USB_SETUP_PACKET_LEN) {
    //TRACE("* Warning: Setup Packet too large: %u *\n", (unsigned) n);
    n = USB_SETUP_PACKET_LEN;
  }
  
  n = d12_read_data(base_addr, buf, n);
  
  d12_ack_setup(base_addr);
  d12_clear_buffer(base_addr);
  
  // ----- Ack Setup to EP0 IN ------
  
  d12_select_endp(base_addr, D12_TX_CTRL_ENDP);
  d12_ack_setup(base_addr);
  
  return n == USB_SETUP_PACKET_LEN;
}

// ------------------------------------------------------------------------
// Reads the contents of the currently selected endpoint's RAM buffer into 
// the buf[] array.
//
// The D12's buffer comes in as follows:
//     [0]     junk ("reserved" - can be anything). Just disregard
//     [1]     # data bytes to follow
//     [2] data byte 0, ...
//     up to
//     [N+2] data byte N-1
//
// Parameters:
//      buf  byte array to receive data. This MUST be at least the size
//           of the chip's RAM buffer for the currently selected endpoint.
//           If buf is NULL, the data is read & discarded.
//
// Returns: the actual number of bytes read (could be <= n)

static uint8 
d12_read_selected_endp_buf(d12_addr_type base_addr, byte *buf)
{
  uint8 n;

  d12_read_byte(base_addr, CMD_READ_BUF);   // Read & discard reserved byte
  n = d12_read_data_byte(base_addr);        // # bytes in chip's buf
  d12_read_data(base_addr, buf, n);
  d12_clear_buffer(base_addr);

  return n;
}

// ------------------------------------------------------------------------
// Selects the specified endpoint and reads the contents of it's RAM buffer
// into the buf[] array. For the Main OUT endpoint, it will check whether 
// both buffers are full, and if so, read them both.
//
// Side Effects:
//              - Leaves endp_idx as the currently selected endpoint.
//
// Parameters:
//      endp_idx    the endpoint from which to read
//      buf         buffer to receive the data. This MUST be at least the size
//                  of the chip's RAM buffer for the specified endpoint.
//                  For the Main endp, it must be 2x the buffer size (128 total)
//
// Returns: the # of bytes read.

static uint8 
d12_read_endp_buf(d12_addr_type base_addr, byte endp_idx, byte *buf)
{
  return (d12_select_endp(base_addr, endp_idx) & SEL_ENDP_FULL)
    ? d12_read_selected_endp_buf(base_addr, buf) : 0;
}

// ------------------------------------------------------------------------
// Does a read of the "main" endpoint (#2). Since it's double buffered,
// this will check if both buffers are full, and if so it will read them
// both. Thus the caller's buffer, buf, must be large enough to hold all
// the data - 128 bytes total.
// 
// If either buffer contains less than the full amount, the done flag
// is set indicating that a Bulk OUT transfer is complete.
// 
// This determines if a bulk transfer is done, since the caller can't 
// necessarily determine this from the size of the return buffer.
// If either buffer is less than full, '*done' is set to a non-zero value.

static uint8 
d12_read_main_endp_buf(d12_addr_type base_addr, byte *buf, int *done)
{
  int             nBuf = 1;
  uint8   n = 0;
  byte    stat = d12_read_endp_status(base_addr, D12_RX_MAIN_ENDP) & 
    D12_ENDP_STAT_ANY_BUF_FULL;
  
  if (stat == 0)
    return 0;
  
  if (stat == D12_ENDP_STAT_BOTH_BUF_FULL)
    nBuf++;
  
  *done = false;
  
  while (nBuf--) {
    if (d12_select_endp(base_addr, D12_RX_MAIN_ENDP) & SEL_ENDP_FULL) {
      uint8 n1 = d12_read_selected_endp_buf(base_addr, buf+n);
      n += n1;
      if (n1 < D12_MAIN_ENDP_SIZE) {
	*done = true;
	break;
      }
    }
    else
      *done = true;
  }
  return n;
}

// ------------------------------------------------------------------------
// Writes the contents of the buf[] array to the currently selected 
// endpoint's RAM buffer. The host will get the data on the on the next IN
// packet from the endpoint.
//
// Note:
//      - The length of the buffer, n, must be no more than the size of the
//      endpoint's RAM space, though, currently, this is not checked.
//      - It's feasible that the application needs to send an empty (NULL) 
//      packet. It's valid for 'n' to be zero, and/or buf NULL.

static uint8 
d12_write_selected_endp_buf(d12_addr_type base_addr, const byte *buf, uint8 n)
{
  d12_write_byte(base_addr, CMD_WRITE_BUF, 0);
  d12_write_data_byte(base_addr, n);
  d12_write_data(base_addr, buf, n);
  d12_validate_buffer(base_addr);
  
  return n;
}

// ------------------------------------------------------------------------
// Writes the contents of the buf[] array to the specified endoint's RAM
// buffer. The host will get this data on the next IN packet from the 
// endpoint.
//
// Side Effects:
//      - Leaves endp_idx as the currently selected endpoint.

static uint8 
d12_write_endp_buf(d12_addr_type base_addr, byte endp_idx, 
		   const byte *buf, uint8 n)
{
  d12_select_endp(base_addr, endp_idx);
  return d12_write_selected_endp_buf(base_addr, buf, n);
}

// ------------------------------------------------------------------------
// Reads & returns the contents of the Chip ID register.

static inline uint16 
d12_read_chip_id(d12_addr_type base_addr)
{
  return d12_read_word(base_addr, CMD_READ_CHIP_ID);
}


// ==========================================================================
// eCos-Specific Device Driver Code
// ==========================================================================

static void usbs_d12_reset(void);

// Make some abbreviations for the configuration options.

#if defined(CYGPKG_DEVS_USB_D12_RX_EP1)
#define _RX_EP1
#endif

#if defined(CYGPKG_DEVS_USB_D12_TX_EP1)
#define _TX_EP1
#endif

#if defined(CYGPKG_DEVS_USB_D12_RX_EP2)
#define _RX_EP2
#endif

#if defined(CYGPKG_DEVS_USB_D12_TX_EP2)
#define _TX_EP2
#endif

// --------------------------------------------------------------------------
// Endpoint 0 Data
// --------------------------------------------------------------------------

static cyg_interrupt    usbs_d12_intr_data;
static cyg_handle_t     usbs_d12_intr_handle;

static byte ep0_tx_buffer[CYGNUM_DEVS_USB_D12_EP0_TXBUFSIZE];

static void usbs_d12_start(usbs_control_endpoint*);
static void usbs_d12_poll(usbs_control_endpoint*);

typedef enum endp_state {
  ENDP_STATE_IDLE,
  ENDP_STATE_IN,
  ENDP_STATE_OUT
} endp_state;

typedef struct ep0_impl {
  usbs_control_endpoint   common;
  endp_state              ep_state;
  int                     length;
  int                     transmitted;
  bool                    tx_empty;
} ep0_impl;

static ep0_impl ep0 = {
 common:
 {
 state:                  USBS_STATE_POWERED,
 enumeration_data:       (usbs_enumeration_data*) 0,
 start_fn:               &usbs_d12_start,
 poll_fn:                &usbs_d12_poll,
 interrupt_vector:       CYGNUM_DEVS_USB_D12_IRQ,
 control_buffer:         { 0, 0, 0, 0, 0, 0, 0, 0 },
 state_change_fn:        0,
 state_change_data:      0,
 standard_control_fn:    0,
 standard_control_data:  0,
 class_control_fn:       0,
 class_control_data:     0,
 vendor_control_fn:      0,
 vendor_control_data:    0,
 reserved_control_fn:    0,
 reserved_control_data:  0,
 buffer:                 0,
 buffer_size:            0,
 fill_buffer_fn:         0,
 fill_data:              0,
 fill_index:             0,
 complete_fn:            0
 },
 ep_state:               ENDP_STATE_IDLE,
 length:                 0,
 transmitted:    0,
 tx_empty:               0
};

extern usbs_control_endpoint usbs_d12_ep0 __attribute__((alias ("ep0")));

// --------------------------------------------------------------------------
// Rx Endpoints 1 & 2 Data
// --------------------------------------------------------------------------

#if defined(_RX_EP1) || defined(_RX_EP2)

typedef struct rx_endpoint {
  usbs_rx_endpoint        common;
  int                     endp, received;
} rx_endpoint;

static void usbs_d12_api_start_rx_ep(usbs_rx_endpoint*);
static void usbs_d12_api_stall_rx_ep(usbs_rx_endpoint*, cyg_bool);

static void usbs_d12_ep_rx_complete(rx_endpoint *ep, int result);
static void usbs_d12_stall_rx_ep(rx_endpoint*, cyg_bool);

#endif


#if defined(_RX_EP1)

static rx_endpoint rx_ep1 = {
 common: {
  start_rx_fn:    &usbs_d12_api_start_rx_ep,
  set_halted_fn:  &usbs_d12_api_stall_rx_ep,
  halted:         0
 },
 endp:            1
};

extern usbs_rx_endpoint usbs_d12_rx_ep1 __attribute__((alias ("rx_ep1")));

#endif


#if defined(_RX_EP2)

static rx_endpoint rx_ep2 = {
 common: {
  start_rx_fn:    &usbs_d12_api_start_rx_ep,
  set_halted_fn:  &usbs_d12_api_stall_rx_ep,
  halted:         0
 },
 endp:            2
};

extern usbs_rx_endpoint usbs_d12_rx_ep2 __attribute__((alias ("rx_ep2")));

#endif

// --------------------------------------------------------------------------
// Tx Endpoints 1 & 2 Data
// --------------------------------------------------------------------------

#if defined(_TX_EP1) || defined(_TX_EP2)

typedef struct tx_endpoint {
  usbs_tx_endpoint        common;
  int                     endp, transmitted;
  bool                    tx_empty;
} tx_endpoint;

static void usbs_d12_api_start_tx_ep(usbs_tx_endpoint*);
static void usbs_d12_api_stall_tx_ep(usbs_tx_endpoint*, cyg_bool);

static void usbs_d12_ep_tx_complete(tx_endpoint *ep, int result);
static void usbs_d12_stall_tx_ep(tx_endpoint*, cyg_bool);
#endif

#if defined(_TX_EP1)

static tx_endpoint tx_ep1 = {
 common: {
  start_tx_fn:    &usbs_d12_api_start_tx_ep,
  set_halted_fn:  &usbs_d12_api_stall_tx_ep,
  halted:         0
 },
 endp:            1
};

extern usbs_tx_endpoint usbs_d12_tx_ep1 __attribute__((alias ("tx_ep1")));
#endif

#if defined(_TX_EP2)

static tx_endpoint tx_ep2 = {
 common: {
  start_tx_fn:    &usbs_d12_api_start_tx_ep,
  set_halted_fn:  &usbs_d12_api_stall_tx_ep,
  halted:         0
 },
 endp:            2
};

extern usbs_tx_endpoint usbs_d12_tx_ep2 __attribute__((alias ("tx_ep2")));

#endif

// --------------------------------------------------------------------------
// Synchronization

static inline void usbs_d12_lock(void)          { cyg_scheduler_lock(); }
static inline void usbs_d12_unlock(void)        { cyg_scheduler_unlock(); }

// --------------------------------------------------------------------------
// Control Endpoint
// --------------------------------------------------------------------------

// Fills the EP0 transmit buffer with a packet. Partial data packets are 
// retrieved by repeatedly calling the fill function.

static int 
ep0_fill_tx_buffer(void)
{
  int nFilled = 0;
  
  while (nFilled < CYGNUM_DEVS_USB_D12_EP0_TXBUFSIZE) {
    if (ep0.common.buffer_size != 0) {
      if ((nFilled + ep0.common.buffer_size) < 
	  CYGNUM_DEVS_USB_D12_EP0_TXBUFSIZE) {
	memcpy(&ep0_tx_buffer[nFilled], ep0.common.buffer, 
	       ep0.common.buffer_size);
	nFilled += ep0.common.buffer_size;
	ep0.common.buffer_size = 0;
      }
      else {
	break;
      }
    }
    else if (ep0.common.fill_buffer_fn) {
      (*ep0.common.fill_buffer_fn)(&ep0.common);
    }
    else {
      break;
    }
  }
  CYG_ASSERT((ep0.common.buffer_size == 0) && (!ep0.common.fill_buffer_fn), 
	     "EP0 transmit buffer overflow");
  TRACE_D12("EP0: Filled Tx Buf with %d bytes\n", nFilled);
  
  ep0.length = nFilled;
  
  ep0.common.fill_buffer_fn       = 0;
  ep0.common.fill_data            = 0;
  ep0.common.fill_index           = 0;
  
  return nFilled;
}

// --------------------------------------------------------------------------
// Called when a transfer is complete on the control endpoint EP0. 
// It resets the endpoint's data structure and calls the completion function,
// if any.
//
// PARAMETERS:
// result          0, on success
//                 -EPIPE or -EIO to indicate a cancellation

static usbs_control_return 
usbs_d12_ep0_complete(int result)
{
  usbs_control_return ret = USBS_CONTROL_RETURN_UNKNOWN;
  
  ep0.ep_state = ENDP_STATE_IDLE;
  
  if (ep0.common.complete_fn)
    ret = (*ep0.common.complete_fn)(&ep0.common, result);
  
  ep0.common.buffer                       = 0;
  ep0.common.buffer_size          = 0;
  ep0.common.complete_fn          = 0;
  //ep0.common.fill_buffer_fn     = 0;
  
  return ret;
}

// --------------------------------------------------------------------------
// This routine is called when we want to send the next packet to the tx ep0
// on the chip. It is used to start a new transfer, and is also called when
// the chip interrupts to indicate that the ep0 tx buffer is empty and ready
// to receive a new packet.
//
// NOTE:
//      On the D12, when you send a zero-length packet to a tx endpoint, the
//      chip transmits the empty packet to the host, but doesn't interrupt 
//      indicating that it is complete. So immediately after sending the
//      empty packet we complete the transfer.

static void 
usbs_d12_ep0_tx(void)
{
  int     nRemaining = ep0.length - ep0.transmitted;
  uint8   n;
  
  // ----- Intermittent interrupt? Get out -----
  
  if (!ep0.common.buffer) {
    TRACE_D12("EP0: Tx no buffer (%d)\n", nRemaining);
    return;
  }
  
  // ----- If prev packet was last, signal that we're done -----
  
  if (nRemaining == 0 && !ep0.tx_empty) {
    TRACE_D12("\tEP0: Tx Complete (%d) %p\n", ep0.transmitted, 
	      ep0.common.complete_fn);
    usbs_d12_ep0_complete(0);
    return;
  }
  
  // ----- Load the next tx packet onto the chip -----
  
  if (nRemaining < D12_ENDP0_SIZE) {
    n = (uint8) nRemaining;
    ep0.tx_empty = false;
  }
  else
    n = D12_ENDP0_SIZE;
  
  d12_write_endp_buf(D12_BASE_ADDR, D12_TX_ENDP0, 
		     &ep0_tx_buffer[ep0.transmitted], n);
  
  TRACE_D12("EP0: Wrote %u bytes\n", (unsigned) n);
  TRACE_BUF0("\t", &ep0_tx_buffer[ep0.transmitted], n);
  
  ep0.transmitted += n;
  
  // ----- If empty packet, D12 won't interrupt, so end now ----- */
  
  if (n == 0) {
    TRACE_D12("\tEP0: Tx Complete (%d) %p\n", ep0.transmitted, 
	      ep0.common.complete_fn);
    usbs_d12_ep0_complete(0);
  }
}

// --------------------------------------------------------------------------
// This function is called when a packet has been successfully sent on the
// primary control endpoint (ep0). It indicates that the chip is ready for 
// another packet. We read the LastTransStatus for the endpoint to clear 
// the interrupt bit, then call ep0_tx() to continue the transfer.

static void 
usbs_d12_ep0_tx_intr(void)
{
  d12_read_last_trans_status(D12_BASE_ADDR, D12_TX_ENDP0);
  usbs_d12_ep0_tx();
}

// --------------------------------------------------------------------------
// Try to handle standard requests. This is a three step process:
//     1.   If it's something we should handle internally we take care of it.
//          Currently we can handle SET_ADDRESS requests, and a few others.
//     2.   If the upper level code has installed a standard control handler
//          we let that function have a crack at it.
//     3.   If neither of those handle the packet we let 
//          usbs_handle_standard_control() have a last try at it.
//
// Locally:
//          SET_ADDRESS: The host is demanding that we change our USB address.
//          This is done by updating the Address/Enable register on the D12. 
//          Note, however that the USB protocol requires us to ack at the old 
//          address, change address, and then accept the next control message
//          at the new      address. The D12 address reg is buffered to do this 
//          automatically for us. The updated address on the chip won't take
//          affect until after the empty ack is sent. Nice.
//

static usbs_control_return 
usbs_d12_handle_std_req(usb_devreq *req)
{
  usbs_control_return result = USBS_CONTROL_RETURN_UNKNOWN;
  int recipient = req->type & USB_DEVREQ_RECIPIENT_MASK;

  if (req->request == USB_DEVREQ_SET_ADDRESS) {
    TRACE_D12("Setting Addr: %u\n", (unsigned) req->value_lo);
    d12_set_addr_enable(D12_BASE_ADDR, req->value_lo, true);
    result = USBS_CONTROL_RETURN_HANDLED;
  }
  else if (req->request == USB_DEVREQ_GET_STATUS) {
    if (recipient == USB_DEVREQ_RECIPIENT_DEVICE) {
      const usbs_enumeration_data *enum_data = ep0.common.enumeration_data;
      if (enum_data && enum_data->device.number_configurations == 1 &&
	  enum_data->configurations) {
	ep0.common.control_buffer[0]  = 
	  (enum_data->configurations[0].attributes
	   & USB_CONFIGURATION_DESCRIPTOR_ATTR_SELF_POWERED) ? 1 : 0;
	ep0.common.control_buffer[0] |= 
	  (enum_data->configurations[0].attributes
	   & USB_CONFIGURATION_DESCRIPTOR_ATTR_REMOTE_WAKEUP) ? 2 : 0;
	ep0.common.control_buffer[1] = 0;
	result = USBS_CONTROL_RETURN_HANDLED;
      }
    }
    else if (recipient == USB_DEVREQ_RECIPIENT_ENDPOINT) {
      bool halted = false;
      result = USBS_CONTROL_RETURN_HANDLED;

      switch (req->index_lo) {
#if defined(_RX_EP1)
      case 0x01 : halted = rx_ep1.common.halted;      break;
#endif
#if defined(_TX_EP1)
      case 0x81 : halted = tx_ep1.common.halted;      break;
#endif
#if defined(_RX_EP2)
      case 0x02 : halted = rx_ep2.common.halted;      break;
#endif
#if defined(_TX_EP2)
      case 0x82 : halted = tx_ep2.common.halted;      break;
#endif

      default:
	result = USBS_CONTROL_RETURN_STALL;
      }

      TRACE_D12("Get Status: Endp [0x%02X] %s\n", (unsigned) req->index_lo, 
		halted ? "Halt" : "Unhalt");
      if (result == USBS_CONTROL_RETURN_HANDLED) {
	ep0.common.control_buffer[0] = (halted) ? 1 : 0;
	ep0.common.control_buffer[1] = 0;
      }
    }

    if (result == USBS_CONTROL_RETURN_HANDLED) {
      ep0.common.buffer                       = ep0.common.control_buffer;
      ep0.common.buffer_size                  = 2;
      ep0.common.fill_buffer_fn               = 0;
      ep0.common.complete_fn                  = 0;
    }
  }
  else if ((req->request == USB_DEVREQ_SET_FEATURE || 
	    req->request == USB_DEVREQ_CLEAR_FEATURE) && 
	   recipient == USB_DEVREQ_RECIPIENT_ENDPOINT) {
    
    bool halt = (req->request == USB_DEVREQ_SET_FEATURE);
    result = USBS_CONTROL_RETURN_HANDLED;
    TRACE_D12("Endpoint [0x%02X] %s\n", (unsigned) req->index_lo, 
	      halt ? "Halt" : "Unhalt");

    switch (req->index_lo) {
#if defined(_RX_EP1)
    case 0x01 :     usbs_d12_stall_rx_ep(&rx_ep1, halt);    break;
#endif
#if defined(_TX_EP1)
    case 0x81 : usbs_d12_stall_tx_ep(&tx_ep1, halt);        break;
#endif
#if defined(_RX_EP2)
    case 0x02 :     usbs_d12_stall_rx_ep(&rx_ep2, halt);    break;
#endif
#if defined(_TX_EP2)
    case 0x82 : usbs_d12_stall_tx_ep(&tx_ep2, halt);        break;
#endif
      
    default:
      result = USBS_CONTROL_RETURN_STALL;
    }
  }
  else if (ep0.common.standard_control_fn != 0) {
    result = (*ep0.common.standard_control_fn)
      (&ep0.common,
       ep0.common.standard_control_data);
  }
  
  if (result == USBS_CONTROL_RETURN_UNKNOWN)
    result = usbs_handle_standard_control(&ep0.common);
  
  return result;
}

// --------------------------------------------------------------------------
// Handler for the receipt of a setup (dev request) packet from the host.
// We examine the packet to determine what function(s) should get a crack
// at trying to handle it, then pass control to the proper function. If
// the function handles the message we either ACK (len==0) or prepare for
// an IN or OUT data phase. If no one handled the message, we stall the
// control endpoint.

static void 
usbs_d12_ep0_setup_packet(usb_devreq* req)
{
  int             len, dir, protocol, recipient;
  usbs_control_return     result = USBS_CONTROL_RETURN_UNKNOWN;
  
  // ----- See who should take the request -----
  
  len = make_word(req->length_hi, req->length_lo);
  
  dir                     = req->type & USB_DEVREQ_DIRECTION_MASK;
  protocol    = req->type & USB_DEVREQ_TYPE_MASK;
  recipient   = req->type & USB_DEVREQ_RECIPIENT_MASK;
  
  TRACE_BUF0("DevReq: ", ep0.common.control_buffer, sizeof(usb_devreq));
  
  if (protocol == USB_DEVREQ_TYPE_STANDARD)
    result = usbs_d12_handle_std_req(req);
  else {
    // Pass on non-standard requests to registered handlers
    
    usbs_control_return     (*callback_fn)(usbs_control_endpoint*, void*);
    void *callback_arg;
    
    if (protocol == USB_DEVREQ_TYPE_CLASS) {
      callback_fn  = ep0.common.class_control_fn;
      callback_arg = ep0.common.class_control_data;
    }
    else if (protocol == USB_DEVREQ_TYPE_VENDOR) {
      callback_fn  = ep0.common.vendor_control_fn;
      callback_arg = ep0.common.vendor_control_data;
    }
    else {
      callback_fn  = ep0.common.reserved_control_fn;
      callback_arg = ep0.common.reserved_control_data;
    }
    
    result = (callback_fn)  ? (*callback_fn)(&ep0.common, callback_arg)
      : USBS_CONTROL_RETURN_STALL;
  }
  
  // ----- If handled prep/handle data phase, otherwise stall -----
  
  if (result == USBS_CONTROL_RETURN_HANDLED) {
    if (len == 0) {
      TRACE_D12("\tCtrl ACK\n");
      d12_write_endp_buf(D12_BASE_ADDR, D12_TX_ENDP0, 0, 0);
    }
    else {
      // Set EP0 state to  IN or OUT mode for data phase
      ep0.transmitted = 0;
      ep0.length = len;
      
      if (dir == USB_DEVREQ_DIRECTION_OUT) {
	// Wait for the next packet from the host.
	ep0.ep_state = ENDP_STATE_OUT;
	CYG_ASSERT(ep0.common.buffer != 0, 
		   "A rx buffer should have been provided for EP0");
	CYG_ASSERT(ep0.common.complete_fn != 0, 
		   "A completion function should be provided for EP0 OUT control messages");
      }
      else {
	ep0.tx_empty = true;
	ep0.ep_state = ENDP_STATE_IN;
	ep0_fill_tx_buffer();
	usbs_d12_ep0_tx();
      }
    }
  }
  else {
    TRACE_D12("\t*** Unhandled Device Request ***\n");
    // The request wasn't handled, so stall control endpoint
    d12_stall_ctrl_endp(D12_BASE_ADDR, true);
  }
}

// --------------------------------------------------------------------------
// This is called when the chip indicates that a packet has been received
// on control endpoint 0. If it's a setup packet, we handle it accordingly,
// otherwise it's a data packet coming in on ep0.
//

static void 
usbs_d12_ep0_rx_intr(void)
{
  byte byStat = d12_read_last_trans_status(D12_BASE_ADDR, D12_RX_ENDP0);
  TRACE_D12("\tEP0 Status: 0x%02X\n", (unsigned) byStat);
  
  if (byStat & D12_LAST_TRANS_SETUP_PACKET) {
    usb_devreq *req = (usb_devreq *) ep0.common.control_buffer;
    
    if (!d12_read_setup_packet(D12_BASE_ADDR, (byte*) req)) {
      TRACE_D12("ep0_rx_dsr: Error reading setup packet\n");
      d12_stall_ctrl_endp(D12_BASE_ADDR, true);
    }
    else
      usbs_d12_ep0_setup_packet(req);
  }
  else {
    if (ep0.common.buffer) {
      uint8 n = d12_read_endp_buf(D12_BASE_ADDR, D12_RX_ENDP0, 
				  ep0.common.buffer + ep0.transmitted);
      ep0.transmitted += n;
      
      TRACE_D12("EP0: Received %d bytes\n", (unsigned) n);
      
      if (n < D12_ENDP0_SIZE || 
	  ep0.common.buffer_size - ep0.transmitted < D12_ENDP0_SIZE) {
	TRACE_D12("\tEP0: Rx Complete (%d) %p\n", 
		  ep0.transmitted, ep0.common.complete_fn);
	
	if (usbs_d12_ep0_complete(0) == USBS_CONTROL_RETURN_HANDLED)
	  d12_write_endp_buf(D12_BASE_ADDR, D12_TX_ENDP0, 0, 0);
	else
	  d12_stall_ctrl_endp(D12_BASE_ADDR, true);
      }
    }
    else {
      TRACE_D12("EP0: No Rx buffer. Discarding packet\n");
      d12_read_endp_buf(D12_BASE_ADDR, D12_RX_ENDP0, NULL);
    }
  }
}

// --------------------------------------------------------------------------
// Handler for when the device is put into or taken out of suspend mode.
// It updates the state variable in the control endpoint and calls the
// registered state change function, if any.

// TODO: Put the chip into low power mode??? Stop clocks, etc???

static void 
usbs_d12_suspend(bool suspended)
{
  int                     old_state = ep0.common.state;
  usbs_state_change       state_change;
  
  if (suspended) {
    ep0.common.state |= USBS_STATE_SUSPENDED;
    state_change = USBS_STATE_CHANGE_SUSPENDED;
  }
  else {
    ep0.common.state &= USBS_STATE_MASK;
    state_change = USBS_STATE_CHANGE_RESUMED;
  }
  
  if (ep0.common.state_change_fn) {
    (*ep0.common.state_change_fn)(&ep0.common, ep0.common.state_change_data,
				  state_change, old_state);
  }
}

// --------------------------------------------------------------------------
// Common Rx Endpoint 1 & 2
// --------------------------------------------------------------------------

#if defined(_RX_EP1) || defined(_RX_EP2)

static void usbs_d12_clear_rx_ep(rx_endpoint *ep)
{
  ep->common.buffer               = 0;
  ep->common.buffer_size          = 0;
  ep->common.complete_fn          = 0;
  ep->common.complete_data        = 0;
  
  ep->received                    = 0;
}

// --------------------------------------------------------------------------
// This is called when an rx operation is completed. It resets the endpoint
// vars and calls the registered completion function.
//

static void 
usbs_d12_ep_rx_complete(rx_endpoint *ep, int result)
{
  completion_fn fn = ep->common.complete_fn;
  void *data = ep->common.complete_data;
  
  usbs_d12_clear_rx_ep(ep);
  
  if (fn)
    (*fn)(data, result);
}

// --------------------------------------------------------------------------
// This routine is called when an rx buffer in the chip is full and ready to
// be read. If there's an endpoint buffer available and room to hold the data
// we read it in, otherwise we call the completion function, but leave the 
// data in the chip. The hardware will automatically NAK packages from the
// host until the app calls another start read to continue receiving data.
//
// CONTEXT:
//         Called from either the DSR or application thread, via start rx.
//         In either case, it's assumed that the chip is locked.
//              

static void 
usbs_d12_ep_rx(rx_endpoint *ep)
{
  int             n, ep_size, buf_remaining, endp = ep->endp;
  bool    done;
  
  // The main endp is double buffered and we need to be prepared
  // to read both simultaneously.
  ep_size = (endp == D12_MAIN_ENDP) ? (2 * D12_MAIN_ENDP_SIZE) 
    : RX_ENDP_SIZE[endp];
  
  buf_remaining = ep->common.buffer_size - ep->received;
  
  // ----- If no space left in buffer, call completion fn -----
  
  if (!ep->common.buffer || buf_remaining < ep_size) {
    int ret = ep->received;
    
    // See if caller requested a read smaller than the endp. Read &
    // throw away extra
    if (ep->common.buffer_size < ep_size) {
      byte tmp_buf[D12_MAX_PACKET_SIZE];
      
      if (endp == D12_MAIN_ENDP)
	n = d12_read_main_endp_buf(D12_BASE_ADDR, tmp_buf, &done);
      else
	n = d12_read_endp_buf(D12_BASE_ADDR, RX_ENDP_INDEX[endp], tmp_buf);
      
      if (n > ep->common.buffer_size) {
	n = ep->received = ep->common.buffer_size;
	ret = -ENOMEM;
	TRACE_D12("\tEP%d: *** Rx Buffer too small. Data Lost ***\n", endp);
      }
      else
	ret = ep->received = n;
      
      memcpy(ep->common.buffer, tmp_buf, n);
      buf_remaining = ep->common.buffer_size - n;
    }
    
    TRACE_D12("\tEP%d: Rx Complete. Buffer (nearly) full. [%d]\n", 
	      endp, buf_remaining);
    usbs_d12_ep_rx_complete(ep, ret);
    return;
  }
  
  // ----- Read the data from the chip -----
  
  if (endp == D12_MAIN_ENDP)
    n = d12_read_main_endp_buf(D12_BASE_ADDR, 
			       ep->common.buffer + ep->received, &done);
  else {
    n = d12_read_endp_buf(D12_BASE_ADDR, RX_ENDP_INDEX[endp],
			  ep->common.buffer + ep->received);
    done = (n < RX_ENDP_SIZE[endp]);
  }
  
  ep->received += n;
  buf_remaining = ep->common.buffer_size - ep->received;
  
  done = done || (buf_remaining < ep_size);
  
  TRACE_D12("EP%d: Received %d bytes.\n", endp, n);
  TRACE_BUF("\t", ep->common.buffer + ep->received-n, n);
  
  // ----- If we're done, complete the receive -----
  
  if (done) {
    TRACE_D12("\tEP%d Rx Complete (%d)  %p\n", endp, 
	      ep->received, ep->common.complete_fn);
    usbs_d12_ep_rx_complete(ep, ep->received);
  }
}

// --------------------------------------------------------------------------
// Stalls/unstalls the specified endpoint.

static void 
usbs_d12_stall_rx_ep(rx_endpoint *ep, cyg_bool halt)
{
  ep->common.halted = halt;
  d12_stall_endp(D12_BASE_ADDR, RX_ENDP_INDEX[ep->endp], halt);
}

// --------------------------------------------------------------------------
// Handler for an Rx endpoint full interrupt. It clears the interrupt on the
// D12 by reading the endpoint's status register then calls the routine to
// read the data into the buffer.
//
// Called from the DSR context only.
//

static void 
usbs_d12_ep_rx_intr(rx_endpoint *ep)
{
  d12_read_last_trans_status(D12_BASE_ADDR, RX_ENDP_INDEX[ep->endp]);
  usbs_d12_ep_rx(ep);
}

#endif

// --------------------------------------------------------------------------
// Common Tx Endpoint 1 & 2
// --------------------------------------------------------------------------

#if defined(_TX_EP1) || defined(_TX_EP2)

// Clears out the endpoint data structure before/after a tx is complete.

static void usbs_d12_clear_tx_ep(tx_endpoint *ep)
{
  ep->common.buffer = 0;
  ep->common.buffer_size = 0;
  ep->common.complete_fn = 0;
  ep->common.complete_data = 0;
  
  ep->transmitted = 0;
  ep->tx_empty = false;
}

// --------------------------------------------------------------------------
// This is called when a transmit is completed. It resets the endpoint vars
// and calls the registered completion function, if any.
//
// CONTEXT:
//         Called from either the DSR or the app thread that started tx. 

static void usbs_d12_ep_tx_complete(tx_endpoint *ep, int result)
{
  completion_fn fn = ep->common.complete_fn;
  void *data = ep->common.complete_data;
  
  usbs_d12_clear_tx_ep(ep);
  
  if (fn)
    (*fn)(data, result);
}

// --------------------------------------------------------------------------
// The routine writes data to the chip and updates the endpoint's counters. 
// It gets called at the start of a transfer operation to prime the device
// and then gets called each time the chip finishes sending a packet to the
// host and is ready for more data. If the amount of data remaining is 
// smaller than can fit in the chip's endpoint buffer, then this is the last
// packet to send, so we call the completion function.
//
// CONTEXT:
//        Called from either the DSR or the app thread that started the tx
//        In either case, it's assumed the chip is locked.

static void 
usbs_d12_ep_tx(tx_endpoint *ep)
{
  int n, nRemaining;
  
  // ----- Already done. Intermittent interrupt, so get out -----
  
  if (!ep->common.buffer)
    return;
  
  // ----- See how many bytes remaining in buffer -----
  
  nRemaining = ep->common.buffer_size - ep->transmitted;
  
  TRACE_D12("EP%d: Tx %p, %d Done, %d Remaining\n", ep->endp, 
	    ep->common.buffer, ep->transmitted, nRemaining);
  
  // ----- If prev packet was last, signal that we're done -----
  
  if (nRemaining == 0 && !ep->tx_empty) {
    TRACE_D12("\tEP%d: Tx complete (%d)  %p\n", ep->endp, 
	      ep->transmitted, ep->common.complete_fn);
    usbs_d12_ep_tx_complete(ep, ep->transmitted);
    return;
  }
  
  // ----- Write the next packet to chip -----
  
  if (nRemaining < TX_ENDP_SIZE[ep->endp]) {
    n = nRemaining;
    ep->tx_empty = false;
  }
  else
    n = TX_ENDP_SIZE[ep->endp];
  
  TRACE_D12("EP%d: Writing %d bytes. %s\n", ep->endp, 
	    n, (n == 0) ? "DONE" : "");
  TRACE_BUF("\t", ep->common.buffer + ep->transmitted, n);
  
  d12_write_endp_buf(D12_BASE_ADDR, TX_ENDP_INDEX[ep->endp], 
		     ep->common.buffer + ep->transmitted, (uint8) n);
  
  ep->transmitted += n;
  
  // ----- If empty packet, complete now -----
  
  if (n == 0) {
    TRACE_D12("\tEP%d: Tx complete (%d)  %p\n", ep->endp, 
	      ep->transmitted, ep->common.complete_fn);
    usbs_d12_ep_tx_complete(ep, ep->transmitted);
    return;
  }
}

// --------------------------------------------------------------------------
// Stalls/unstalls the specified tx endpoint.

static void 
usbs_d12_stall_tx_ep(tx_endpoint *ep, cyg_bool halt)
{
  ep->common.halted = halt;
  d12_stall_endp(D12_BASE_ADDR, TX_ENDP_INDEX[ep->endp], halt);
}

// --------------------------------------------------------------------------
// Handler for when the chip's tx RAM for an endoint has just been emptied 
// (sent to the host) and the chip is ready for more data.
// We read the endpoint's last trans status register to clear the interrupt
// on the D12, then call the tx function to send the next packet or 
// complete the transfer.

static void 
usbs_d12_ep_tx_intr(tx_endpoint *ep)
{
  d12_read_last_trans_status(D12_BASE_ADDR, TX_ENDP_INDEX[ep->endp]);
  usbs_d12_ep_tx(ep);
}

#endif // defined(_TX_EP1) || defined(_TX_EP2)

// --------------------------------------------------------------------------
// Application Program Interface (API)
// --------------------------------------------------------------------------

#if defined(_RX_EP1) || defined(_RX_EP2)
// Starts a receive operation on the specified endpoint. If the buffer size
// is zero the completion function is called immediately. The routine checks
// if tehre is data in the chip's endpoint buffer, and if so it will call
// ep_rx() to start reading the data out of the chip.
//
// If the endpoint is currently stalled, a read size of zero can be used to 
// block the calling thread until the stall is cleared. If the read size is
// non-zero and the endpoint is stalled the completion function is called
// immediately with an error result.

static void 
usbs_d12_api_start_rx_ep(usbs_rx_endpoint *ep)
{
  rx_endpoint *epx = (rx_endpoint *) ep;
  
  if (ep->halted) {
    if (ep->buffer_size != 0)
      usbs_d12_ep_rx_complete(epx, -EAGAIN);
  }
  else if (ep->buffer_size == 0) {
    usbs_d12_ep_rx_complete(epx, 0);
  }
  else {
    TRACE_D12("EP%d: Starting Rx, %p, %d\n", epx->endp, ep->buffer,
	      ep->buffer_size);
    usbs_d12_lock();
    
    epx->received = 0;
    if (d12_data_available(D12_BASE_ADDR, RX_ENDP_INDEX[epx->endp]))
      usbs_d12_ep_rx(epx);
    
    usbs_d12_unlock();
  }
}

// --------------------------------------------------------------------------
// Halts/unhalts one of the generic rx (OUT) endpoints.
//

static void usbs_d12_api_stall_rx_ep(usbs_rx_endpoint *ep, cyg_bool halt)
{
  usbs_d12_lock();
  usbs_d12_stall_rx_ep((rx_endpoint*) ep, halt);
  usbs_d12_unlock();
}

#endif // defined(_RX_EP1) || defined(_RX_EP2)

// --------------------------------------------------------------------------
// Tx API
// --------------------------------------------------------------------------

#if defined(_TX_EP1) || defined(_TX_EP2)

// This starts a transmit on one of the data endpoints. If the endpoint is
// stalled a buffer size of zero can be used to block until the stall is
// cleared. Any other size on a stalled endpoint will result in an error
// callback immediately. The first packet is sent to the chip immediately,
// in the application context. If the chip's buffer can contain the whole
// transfer, the completion function will be called immediately, again,
// still in the application context.
//
// If an empty packet is requested we send one from here and call the 
// completion function. This should not cause an intr on the D12.
//
// CONTEXT:
//        Called from an application thread

static void usbs_d12_api_start_tx_ep(usbs_tx_endpoint *ep)
{
  tx_endpoint *epx = (tx_endpoint*) ep;
  
  if (ep->halted) {
    if (ep->buffer_size != 0) 
      usbs_d12_ep_tx_complete(epx, -EAGAIN);
  }
  else if (ep->buffer_size == 0) {
    usbs_d12_lock();
    
    d12_write_endp_buf(D12_BASE_ADDR, TX_ENDP_INDEX[epx->endp], 0, 0);
    usbs_d12_ep_tx_complete(epx, 0);
    
    usbs_d12_unlock();
  }
  else {
    TRACE_D12("EP%d: Starting Tx, %p, %d\n", epx->endp, ep->buffer,
	      ep->buffer_size);
    usbs_d12_lock();
    
    epx->tx_empty = true;
    epx->transmitted = 0;
    usbs_d12_ep_tx(epx);
    
    usbs_d12_unlock();
  }
}

// --------------------------------------------------------------------------
// Halts/unhalts one of the generic endpoints.

static void 
usbs_d12_api_stall_tx_ep(usbs_tx_endpoint *ep, cyg_bool halt)
{
  usbs_d12_lock();
  usbs_d12_stall_tx_ep((tx_endpoint*) ep, halt);
  usbs_d12_unlock();
}

#endif // defined(_TX_ENDP1) || defined(_TX_EP2)

// --------------------------------------------------------------------------
// DSR
// --------------------------------------------------------------------------

// The DSR for the D12 chip. This is normally called in the DSR context when
// the D12 has raised its interrupt flag indicating that it needs to be 
// serviced. The interrupt register contains bit flags that are OR'ed togther
// indicating what items need to be serviced. There are flags for the 
// following:
//              - The endpoints (one bit for each)
//              - Bus Reset
//              - Suspend Change
//              - DMA (terminal count)
//
// Care must be taken in that the D12's interrupt output is level-sensitive
// (in that the interrupt sources are OR'ed together and not all cleared 
// atomically in a single operation). Platforms (such as the PC) may be 
// expecting edge-triggered interrupts, so we must work around that.
// So, we loop on the interrupt register. Even though, in each loop, we
// perform all of the required operations to clear the interrupts, a new
// one may have arrived before we finished clearing the previous ones.
// So we read the intr reg again. Once the intr reg gives a zero reading
// we know that the D12 has dropped its IRQ line.
//
// Note, if we're configured to use a thread, this routine is called from
// within a thread context (not a DSR context).
//

static void 
usbs_d12_dsr(cyg_vector_t vector, cyg_ucount32 count, 
                                                 cyg_addrword_t data)
{
  uint16  status;
  bool    suspended;
  
  CYG_ASSERT(vector == CYGNUM_DEVS_USB_D12_INT,
	     "DSR should only be invoked for D12 interrupts");
  
  while ((status = d12_read_intr_reg(D12_BASE_ADDR)) != 0) {
    TRACE_D12("Intr Status: 0x%04X\n", (unsigned) status);
    
    if (status & D12_INTR_BUS_RESET) {
      TRACE_D12("\n>>> Bus Reset <<<\n");
      usbs_d12_reset();
    }
    else {
      
      // ----- Suspend Change -----
      
      suspended = (bool) (ep0.common.state & USBS_STATE_SUSPENDED);
      
      if (status & D12_INTR_SUSPEND_CHANGE) {
	if (!suspended && (status & ~D12_INTR_SUSPEND_CHANGE) == 0)
	  usbs_d12_suspend(true);
      }
      else if (suspended)
	usbs_d12_suspend(false);
      
      // ----- Bulk Endpoints -----
      
#ifdef _TX_EP2
      if (status & D12_INTR_TX_ENDP2)
	usbs_d12_ep_tx_intr(&tx_ep2);
#endif
      
#ifdef _RX_EP2
      if (status & D12_INTR_RX_ENDP2)
	usbs_d12_ep_rx_intr(&rx_ep2);
#endif
      
      // ----- Interrupt Endpoints -----
      
#ifdef _TX_EP1
      if (status & D12_INTR_TX_ENDP1)
	usbs_d12_ep_tx_intr(&tx_ep1);
#endif
      
#ifdef _RX_EP1
      if (status & D12_INTR_RX_ENDP1)
	usbs_d12_ep_rx_intr(&rx_ep1);
#endif
      
      // ----- Control Endpoint -----
      
      if (status & D12_INTR_TX_CTRL_ENDP)
	usbs_d12_ep0_tx_intr();
      
      if (status & D12_INTR_RX_CTRL_ENDP)
	usbs_d12_ep0_rx_intr();
    }
  }
  
  cyg_drv_interrupt_unmask(vector);
}

// --------------------------------------------------------------------------
// Interrupt
// --------------------------------------------------------------------------

// Here, the ISR does nothing but schedule the DSR to run. The ISR's/DSR's
// are serialized. The CPU won't process another ISR until after the DSR
// completes.

static uint32 
usbs_d12_isr(cyg_vector_t vector, cyg_addrword_t data)
{
  CYG_ASSERT(CYGNUM_DEVS_USB_D12_INT == vector, 
	     "usbs_isr: Incorrect interrupt");
  
  // Prevent another interrupt until DSR completes
  cyg_drv_interrupt_mask(vector);
  cyg_drv_interrupt_acknowledge(vector);
  
  return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
}

// --------------------------------------------------------------------------
// Polling
// --------------------------------------------------------------------------

static void 
usbs_d12_poll(usbs_control_endpoint *endp)
{
  CYG_ASSERT(endp == &ep0.common, "usbs_poll: wrong endpoint");
  
  usbs_d12_lock();
  usbs_d12_dsr(CYGNUM_DEVS_USB_D12_INT, 0, 0);
  usbs_d12_unlock();
}

// --------------------------------------------------------------------------
// Thread Processing
// --------------------------------------------------------------------------

// The user can opt to configure the driver to service the D12 using a high
// priority thread. The thread's priority MUST be greater than the priority
// of any application thread making calls into the driver.
// When we use a thread, the DSR simply signals a semaphore tio wake the
// thread up. The thread, in turn, calls the the routine to service the D12,
// now in a thread context. This allows for greater debug options, including
// tracing.

#ifdef CYGPKG_DEVS_USB_D12_THREAD

static byte usbs_d12_thread_stack[CYGNUM_DEVS_USB_D12_THREAD_STACK_SIZE];
static cyg_thread   usbs_d12_thread;
static cyg_handle_t usbs_d12_thread_handle;
static cyg_sem_t    usbs_d12_sem;

static void 
usbs_d12_thread_dsr(cyg_vector_t vector, cyg_ucount32 count,
		    cyg_addrword_t data)
{
  cyg_semaphore_post(&usbs_d12_sem);
  
  CYG_UNUSED_PARAM(cyg_vector_t, vector);
  CYG_UNUSED_PARAM(cyg_ucount32, count);
  CYG_UNUSED_PARAM(cyg_addrword_t, data);
}


static void 
usbs_d12_thread_fn(cyg_addrword_t param)
{
  while (1) {
    cyg_semaphore_wait(&usbs_d12_sem);
    usbs_d12_poll(&ep0.common);
  }
  
  CYG_UNUSED_PARAM(cyg_addrword_t, param);
}


static void 
usbs_d12_thread_init(void)
{
  cyg_semaphore_init(&usbs_d12_sem, 0);
  
  cyg_thread_create(CYGNUM_DEVS_USB_D12_THREAD_PRIORITY,
		    &usbs_d12_thread_fn, 0, "D12 USB Driver Thread",
		    usbs_d12_thread_stack, 
		    CYGNUM_DEVS_USB_D12_THREAD_STACK_SIZE,
		    &usbs_d12_thread_handle, &usbs_d12_thread);
  cyg_thread_resume(usbs_d12_thread_handle);
}

#endif  // CYGPKG_DEVS_USB_D12_THREAD

// --------------------------------------------------------------------------
// Start/Reset
// --------------------------------------------------------------------------

// Chip initialization and handler for a USB Bus Reset. This gets called at
// system startup and after a USB Bus Reset. It puts the chip into the
// default state, with USB Address 0, connected to the bus (i.e. 
// "SoftConnect" asserted). Interrupts to the main endpoint  are turned on
// via the DMA register. 

static void 
usbs_d12_reset(void)
{
  int old_state = ep0.common.state;
  ep0.common.state = USBS_STATE_DEFAULT;
  
  if (ep0.common.state_change_fn) {
    (*ep0.common.state_change_fn)(&ep0.common, ep0.common.state_change_data,
				  USBS_STATE_CHANGE_RESET, old_state);
  }
  
  d12_set_addr_enable(D12_BASE_ADDR, 0, true);
  d12_set_endp_enable(D12_BASE_ADDR, true);
  d12_set_dma(D12_BASE_ADDR, D12_DMA_MAIN_ENDP_INTR_ENABLE);
  d12_set_mode(D12_BASE_ADDR, D12_MODE_CFG_DFLT | D12_MODE_CFG_SOFT_CONNECT,
	       D12_MODE_CLK_DFLT);
  
  // If any endpoints were going, signal the end of transfers
  
#if defined(_TX_EP2)
  usbs_d12_ep_tx_complete(&tx_ep2, -EPIPE);
#endif
  
#if defined(_RX_EP2)
  usbs_d12_ep_rx_complete(&rx_ep2, -EPIPE);
#endif
  
#if defined(_TX_EP1)
  usbs_d12_ep_tx_complete(&tx_ep1, -EPIPE);
#endif
  
#if defined(_RX_EP1)
  usbs_d12_ep_rx_complete(&rx_ep1, -EPIPE);
#endif
}

// --------------------------------------------------------------------------
// The start function is called indirectly by the application when 
// initialization is complete. By this time, the enumeration data has been
// assigned to the control endpoint and we're ready to connect to the host.
// Within the reset function the D12's SoftConnect line is asserted which
// allows the host (hub) to see us on the USB bus. If connected, the host 
// should start the enumeration process.
//

static void usbs_d12_start(usbs_control_endpoint *endpoint)
{
#if defined(_TRACE) && !defined(_TRACE_STDOUT)
  TRACE_OPEN(TRACE_SINK);
#endif
  
  CYG_ASSERT(endpoint == &ep0.common, "ep0 start: wrong endpoint");
  TRACE_D12("USBS D12: Starting.\n");
  
  d12_clear_all_intr(D12_BASE_ADDR);
  usbs_d12_reset();
}       

// --------------------------------------------------------------------------
// Initialization
// --------------------------------------------------------------------------

// This routine is called early in the program's startup, possibly before
// main (from within a C++ object initialization). We want to put this chip
// and driver in a neutral, but ready, state until the application gets 
// control, initializes itself and calls the usb start function.
//
// The D12 has a "Soft Connect" feature to tristate the USB bus, making it
// appear that the USB device is not connected to the bus. We initially
// keep seperated from the bus to allow for initialization.

void 
usbs_d12_init(void)
{
  cyg_DSR_t *pdsr;
  
  d12_set_mode(D12_BASE_ADDR, D12_MODE_CFG_DFLT & ~D12_MODE_CFG_SOFT_CONNECT, 
	       D12_MODE_CLK_DFLT);
  
  d12_set_addr_enable(D12_BASE_ADDR, 0, false);
  d12_set_endp_enable(D12_BASE_ADDR, false);
  
  // ----- Clear the endpoints -----
  
#if defined(_TX_EP2)
  usbs_d12_clear_tx_ep(&tx_ep2);
#endif
  
#if defined(_RX_EP2)
  usbs_d12_clear_rx_ep(&rx_ep2);
#endif
  
#if defined(_TX_EP1)
  usbs_d12_clear_tx_ep(&tx_ep1);
#endif
  
#if defined(_RX_EP1)
  usbs_d12_clear_rx_ep(&rx_ep1);
#endif
  
  // ----- Start the thread (if we're using it) -----
  
#ifdef CYGPKG_DEVS_USB_D12_THREAD
  usbs_d12_thread_init();
  pdsr = &usbs_d12_thread_dsr;
#else
  pdsr = &usbs_d12_dsr;
#endif
  
  // ----- Attach the ISR -----
  
  cyg_drv_interrupt_create(CYGNUM_DEVS_USB_D12_INT, 
			   0, 0, &usbs_d12_isr, pdsr,
			   &usbs_d12_intr_handle, &usbs_d12_intr_data);
  
  cyg_drv_interrupt_attach(usbs_d12_intr_handle);
  cyg_drv_interrupt_unmask(CYGNUM_DEVS_USB_D12_INT);
}

// ----------------------------------------------------------------------------
// Testing support.

usbs_testing_endpoint usbs_testing_endpoints[] = {
  {
  endpoint_type       : USB_ENDPOINT_DESCRIPTOR_ATTR_CONTROL, 
  endpoint_number     : 0,
  endpoint_direction  : USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN,
  endpoint            : (void*) &ep0.common,
#ifdef CYGVAR_DEVS_USB_D12_EP0_DEVTAB_ENTRY
  devtab_entry        : CYGDAT_DEVS_USB_D12_DEVTAB_BASENAME "0c",
#else        
  devtab_entry        : (const char*) 0,
#endif        
  min_size            : 1,
  max_size            : CYGNUM_DEVS_USB_D12_EP0_TXBUFSIZE,
  max_in_padding      : 0,
  alignment           : 0
  },
  
  /*
#ifdef _TX_EP1
  {   
  endpoint_type       : USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT,
  endpoint_number     : 1,
  endpoint_direction  : USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN,
  endpoint            : (void*) &tx_ep1.common,
# ifdef CYGVAR_DEVS_USB_D12_TX_EP2_DEVTAB_ENTRY
  devtab_entry        : CYGDAT_DEVS_USB_D12_DEVTAB_BASENAME "1w",
# else        
  devtab_entry        : (const char*) 0,
# endif        
  min_size            : 0,
  max_size            : 0x0FFFF,      // Driver limitation, only a single 
                                      // buffer descriptor is used
  max_in_padding      : 0,
  alignment           : HAL_DCACHE_LINE_SIZE
  },
#endif
  
#ifdef _RX_EP1
  {
  endpoint_type       : USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT,
  endpoint_number     : 1,
  endpoint_direction  : USB_ENDPOINT_DESCRIPTOR_ENDPOINT_OUT,
  endpoint            : (void*) &rx_ep1.common,
# ifdef CYGVAR_DEVS_USB_D12_RX_EP2_DEVTAB_ENTRY
  devtab_entry        : CYGDAT_DEVS_USB_D12_DEVTAB_BASENAME "1r",
# else        
  devtab_entry        : (const char*) 0,
# endif        
  min_size            : 1,
  max_size            : 0x0FFFF,      // Driver limitation
  max_in_padding      : 0,
  alignment           : HAL_DCACHE_LINE_SIZE
  },
#endif
  */
  
#ifdef _TX_EP2
  {
  endpoint_type       : USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
  endpoint_number     : 2,
  endpoint_direction  : USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN,
  endpoint            : (void*) &tx_ep2.common,
# ifdef CYGVAR_DEVS_USB_D12_TX_EP2_DEVTAB_ENTRY
  devtab_entry        : CYGDAT_DEVS_USB_D12_DEVTAB_BASENAME "2w",
# else        
  devtab_entry        : (const char*) 0,
# endif        
  min_size            : 0,
  max_size            : 0x1000,   // 4k for testing
  max_in_padding      : 0,
  alignment           : HAL_DCACHE_LINE_SIZE
  },
#endif
  
#ifdef _RX_EP2
  {
  endpoint_type       : USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
  endpoint_number     : 2,
  endpoint_direction  : USB_ENDPOINT_DESCRIPTOR_ENDPOINT_OUT,
  endpoint            : (void*) &rx_ep2.common,
# ifdef CYGVAR_DEVS_USB_D12_RX_EP2_DEVTAB_ENTRY
  devtab_entry        : CYGDAT_DEVS_USB_D12_DEVTAB_BASENAME "2r",
# else        
  devtab_entry        : (const char*) 0,
# endif        
  min_size            : 1,
  max_size            : 0x1000,           // 4k for testing
  max_in_padding      : 0,
  alignment           : HAL_DCACHE_LINE_SIZE
  },
#endif
  
  USBS_TESTING_ENDPOINTS_TERMINATOR
};

