//==========================================================================
//
//      mmc_spi.c
//
//      Provide a disk device driver for MMC cards over SPI
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2004, 2006 eCosCentric Limited
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
//
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author:       bartv
// Date:         2004-04-25
//
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/devs_disk_mmc.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_if.h>             // delays
#include <cyg/hal/hal_intr.h>
#include <string.h>
#include <errno.h>
#include <cyg/io/io.h>
#include <cyg/io/spi.h>
#include <cyg/io/devtab.h>
#include <cyg/io/disk.h>
#include <cyg/io/mmc_protocol.h>

// Communication parameters. First some debug support
#define DEBUG   0
#if DEBUG > 0
# define DEBUG1(format, ...)    diag_printf(format, ## __VA_ARGS__)
#else
# define DEBUG1(format, ...)
#endif
#if DEBUG > 1
# define DEBUG2(format, ...)    diag_printf(format, ## __VA_ARGS__)
#else
# define DEBUG2(format, ...)
#endif

// Should the SPI operations run in polled or interrupt-driven mode?
// The default value is determined by CDL, but can be overridden at
// run-time if necessary. For example if configured for
// interrupt-driven I/O then it will be impossible to perform disk
// operations during system initialization, e.g. from a static
// constructor, unless this flag is changed for the duration.
#ifdef CYGIMP_DEVS_DISK_MMC_SPI_POLLED
cyg_bool    cyg_mmc_spi_polled = true;
#else
cyg_bool    cyg_mmc_spi_polled = false;
#endif

// Should write operations be allowed to complete in the background,
// or must the operation complete in the foreground. The latter
// requires polling for potentially a long time, up to some 100's of
// milliseconds, but the former appears unreliable if there are other
// devices on the SPI bus. In theory the MMC card should detect that
// the chip select line is dropped and tristate the output line, but
// in practice this does not always happen.
#undef MMC_SPI_BACKGROUND_WRITES

// The size of each disk block
#define MMC_SPI_BLOCK_SIZE      512
// The number of retries during a mount operation when switching to
// IDLE mode.
#define MMC_SPI_GO_IDLE_RETRIES 16
// The number of retries during a mount operation when switching from
// idle to operational
#define MMC_SPI_OP_COND_RETRIES 128
// The number of retries when waiting for a response to any command
#define MMC_SPI_COMMAND_RETRIES 32
// Retries when waiting for a data response token during a read
#define MMC_SPI_READ_DATA_TOKEN_RETRIES 32768
// Retries during a write while waiting for completion
#define MMC_SPI_WRITE_BUSY_RETRIES 32768

// ----------------------------------------------------------------------------
// SPI-specific parts of the MMC protocol.
//
// The main response byte. 0 indicates success, other bits
// indicate various error conditions.
#define MMC_REPLY_SUCCESS                   0x00
#define MMC_REPLY_PARAMETER_ERROR           (0x01 << 6)
#define MMC_REPLY_ADDRESS_ERROR             (0x01 << 5)
#define MMC_REPLY_ERASE_SEQUENCE_ERROR      (0x01 << 4)
#define MMC_REPLY_COM_CRC_ERROR             (0x01 << 3)
#define MMC_REPLY_ILLEGAL_COMMAND           (0x01 << 2)
#define MMC_REPLY_ERASE_RESET               (0x01 << 1)
#define MMC_REPLY_IN_IDLE_STATE             (0x01 << 0)

// A send-status command generates a second response byte
#define MMC_REPLY2_OUT_OF_RANGE             (0x01 << 7)
#define MMC_REPLY2_ERASE_PARAM              (0x01 << 6)
#define MMC_REPLY2_WP_VIOLATION             (0x01 << 5)
#define MMC_REPLY2_CARD_ECC_FAILED          (0x01 << 4)
#define MMC_REPLY2_CC_ERROR                 (0x01 << 3)
#define MMC_REPLY2_ERROR                    (0x01 << 2)
#define MMC_REPLY2_WP_ERASE_SKIP            (0x01 << 1)
// Alias for the above
#define MMC_REPLY2_LOCK_UNLOCK_FAILED       (0x01 << 1)
#define MMC_REPLY2_CARD_LOCKED              (0x01 << 0)

// The data error token byte which may get sent if a read
// operation fails. The top 3 bits will be 0. A successful
// response will have these bits 1.
#define MMC_DATA_TOKEN_SUCCESS                  (0x00FE)
#define MMC_DATA_ERROR_TOKEN_CARD_LOCKED        (0x01 << 4)
#define MMC_DATA_ERROR_TOKEN_OUT_OF_RANGE       (0x01 << 3)
#define MMC_DATA_ERROR_TOKEN_CARD_ECC_FAILED    (0x01 << 2)
#define MMC_DATA_ERROR_TOKEN_CC_ERROR           (0x01 << 1)
#define MMC_DATA_ERROR_TOKEN_ERROR              (0x01 << 0)

// ----------------------------------------------------------------------------
// Structures and statics.
//
// There should be an SPI device cyg_spi_mmc_dev0, probably provided by
// the HAL, possibly by the application. Because of the latter we cannot
// assume the variable will be defined in a header.
extern cyg_spi_device   cyg_spi_mmc_dev0;

// When retrieving data it is necessary to send an 0xff byte stream,
// which the card will not confuse with further commands. The largest
// transfer is 512 bytes, too large a buffer to place on the stack.
static cyg_uint8        mmc_spi_ff_data[512];
#define MMC_SPI_INIT_FF_DATA()              \
    CYG_MACRO_START                         \
    memset(mmc_spi_ff_data, 0x00FF, 512);   \
    CYG_MACRO_END

// Details of a specific MMC card
typedef struct cyg_mmc_spi_disk_info_t {
    cyg_spi_device*     mmc_spi_dev;
    cyg_uint32          mmc_saved_baudrate;
    cyg_uint32          mmc_block_count;
#ifdef MMC_SPI_BACKGROUND_WRITES    
    cyg_bool            mmc_writing;
#endif    
    cyg_bool            mmc_read_only;
    cyg_bool            mmc_connected;
    cyg_uint32          mmc_heads_per_cylinder;
    cyg_uint32          mmc_sectors_per_head;
    cyg_uint32          mmc_read_block_length;
    cyg_uint32          mmc_write_block_length;
    mmc_cid_register    mmc_id;
} cyg_mmc_spi_disk_info_t;

// There is no need for a hardware-specific disk controller structure.
// The closest equivalent is probably an SPI bus, i.e. if there were
// MMC connectors attached to different SPI buses then these would
// have separate controllers with independent locking. However that
// can be handled without a cyg_mmc_spi_controller_info_t structure.

// ----------------------------------------------------------------------------
// The low-level MMC operations

// After power-on an MMC card is in idle state and needs at least 74
// clock cycles before any communication. These might be supplied
// courtesy of another SPI device, but no guarantees, so generate some
// ticks.
static void
mmc_spi_send_init(cyg_mmc_spi_disk_info_t* disk)
{
    cyg_spi_bus     *bus;
    cyg_spi_device  *dev;

    DEBUG2("mmc_spi_send_init(): dev pointer 0x%p, %d\n", disk->mmc_spi_dev, cyg_mmc_spi_polled );
    dev = disk->mmc_spi_dev;
    bus = dev->spi_bus;
    DEBUG2("                   : begin pointer %p\n", bus->spi_transaction_begin );
    cyg_spi_tick(disk->mmc_spi_dev, cyg_mmc_spi_polled, 10);
}

// Send the first part of a command sequence. This consists of the
// command itself, an argument, a CRC, and then waiting for a
// reply byte from the card.
static cyg_uint32
mmc_spi_send_command_start(cyg_mmc_spi_disk_info_t* disk, cyg_uint32 command, cyg_uint32 arg)
{
    cyg_spi_device* dev = disk->mmc_spi_dev;
    cyg_uint8       request[7];
    cyg_uint8       response[7];
    cyg_uint8       reply;
    int             i;

#ifdef MMC_SPI_BACKGROUND_WRITES    
    // If the last operation was a block write, those can take a while
    // to complete. Rather than wait at the end of the write(), do so
    // at the beginning of the next operation i.e. here. This also
    // allows the chip select to be dropped while the write comples,
    // so communication is possible with other devices. The polling is
    // done as a sequence of transactions rather than in a single
    // transaction, again to let other threads in to communicate with
    // other devices.
    //
    // The card will send a stream of 0x00 bytes while the write
    // completes. Some cards have been observed to send a byte 0x03 at
    // the end, Either way, when the card sends a byte 0xff it should
    // be ready for the next command.
    if (disk->mmc_writing) {
        DEBUG2("mmc_spi_send_command_start(): polling for completion of previous write\n");
        disk->mmc_writing   = 0;
        response[0]    = 0x00;
        for (i = 0; (i < MMC_SPI_WRITE_BUSY_RETRIES) && (0x00FF != response[0]); i++) {
            cyg_spi_transfer(dev, cyg_mmc_spi_polled, 1, mmc_spi_ff_data, response);
        }
    }
#endif    
    
    request[0]  = command | 0x0040;
    request[1]  = (arg >> 24) & 0x00FF;
    request[2]  = (arg >> 16) & 0x00FF;
    request[3]  = (arg >>  8) & 0x00FF;
    request[4]  = arg         & 0x00FF;
    // A CRC is needed for the go-idle-state command, because that
    // command switches the device from MMC to SPI mode. That CRC is
    // well-known. Once in SPI mode the card will not use CRCs by
    // default.
    request[5]  = (command == 0x00) ? 0x0095 : 0x00ff;
    // There will need to be at least one extra byte transfer to get
    // the card's response, so send that straightaway. Extra
    // outgoing data like this should be 0xff so that the card
    // does not confuse it with a new incoming command.
    request[6]  = 0x00ff;

    // Lock the SPI bus. It remains locked until a subsequent call to
    // mmc_spi_end_command().
    cyg_spi_transaction_begin(dev);
    
    // Transfer the whole command, and try to read the response back
    // immediately.
    cyg_spi_transaction_transfer(dev, cyg_mmc_spi_polled, 7, request, response, 0);
    DEBUG2("Sent command %02x %d: reply bytes %02x %02x %02x %02x %02x %02x %02x\n", command, arg, \
                response[0], response[1], response[2], response[3], response[4], response[5], response[6]);
    
    // The response will be a single byte with the top bit clear.
    // The remaining bits are error/status flags. If the command
    // involves an additional response then that will be handled
    // by the calling code.
    reply       = response[6];
    for (i = 0; (i < MMC_SPI_COMMAND_RETRIES) && (0 != (reply & 0x0080)); i++) {
        cyg_spi_transaction_transfer(dev, cyg_mmc_spi_polled, 1, mmc_spi_ff_data, response, 0);
        reply = response[0];
        DEBUG2("  loop %d, additional reply %02x\n", i, reply);
    }

    // Leave the interpretation of the reply code to the caller
    return (cyg_uint32) reply;
}

// At the end of each command the card needs eight clocks to finish
// its processing. A tick() call takes care of that, and will have
// the side effect of dropping the chip select. Ending the transaction
// unlocks the bus for other SPI I/O operations
static void
mmc_spi_end_command(cyg_mmc_spi_disk_info_t* disk)
{
    cyg_spi_device* dev = disk->mmc_spi_dev;
    cyg_spi_transaction_tick(dev, cyg_mmc_spi_polled, 1);
    cyg_spi_transaction_end(dev);
}

// A utility combination of the above two for simple commands which do
// not involve any other data.
static cyg_uint32
mmc_spi_send_command(cyg_mmc_spi_disk_info_t* disk, cyg_uint32 command, cyg_uint32 arg)
{
    cyg_uint32  reply;
    reply = mmc_spi_send_command_start(disk, command, arg);
    mmc_spi_end_command(disk);
    return reply;
}

// The card will return a data block when reading a disk block, or
// for certain other commands like reading card registers. Each
// data block consists of:
//   1) some number of padding bytes 0xff while the card is still
//      processing the command and preparing the data
//   2) a data token byte, usually 0xFE for success
//   3) n bytes of data
//   4) two bytes of crc, which can be ignored.
//
// The data token byte is the only indication of success or failure,
// so that gets returned.
//
// When mounting certain types of card an extra delay may be needed
// before reading the first data block. This is handled by the
// extra_delay argument.
static cyg_uint32
mmc_spi_read_data(cyg_mmc_spi_disk_info_t* disk, cyg_uint8* buf, cyg_uint32 count, cyg_bool extra_delay)
{
    cyg_spi_device* dev = disk->mmc_spi_dev;
    int             i;
    cyg_uint8       response[2];
    cyg_uint32      retries;

    if (extra_delay) {
        retries = MMC_SPI_READ_DATA_TOKEN_RETRIES * 100;
    } else {
        retries = MMC_SPI_READ_DATA_TOKEN_RETRIES;
    }

    response[0] = 0x00FF;
    for (i = 0; (i < retries) && (0x00FF == response[0]); i++) {
        cyg_spi_transaction_transfer(dev, cyg_mmc_spi_polled, 1, mmc_spi_ff_data, response, 0);
    }
    
    if (MMC_DATA_TOKEN_SUCCESS != response[0]) {
        DEBUG1("mmc_spi_read_data(): got error response %02x after %d iterations\n", response[0], i);
        return response[0];
    }

    // Now for the actual data. There is no way of detecting a failure from
    // this point on.
    cyg_spi_transaction_transfer(dev, cyg_mmc_spi_polled, count, mmc_spi_ff_data, buf, 0);
    // And the CRC, which can be ignored
    cyg_spi_transaction_transfer(dev, cyg_mmc_spi_polled, 2, mmc_spi_ff_data, response, 0);
    DEBUG2("mmc_spi_read_data(): got data and CRC %02x %02x\n", response[0], response[1]);
    
    return MMC_DATA_TOKEN_SUCCESS;
}

// Read one of the card registers, e.g. CSD or CID
static Cyg_ErrNo
mmc_spi_read_register(cyg_mmc_spi_disk_info_t* disk, cyg_uint32 command, cyg_uint8* buf, cyg_uint32 count)
{
    cyg_uint32      reply;
    
    reply = mmc_spi_send_command_start(disk, command, 0);
    if (MMC_REPLY_SUCCESS != reply) {
        DEBUG1("mmc_spi_read_register(): unexpected response to command %02x, reply code %02x\n", command, reply);
        mmc_spi_end_command(disk);
        return (0x00FF == reply) ? -ENODEV : -EIO;
    }
    reply = mmc_spi_read_data(disk, buf, count, false);
    mmc_spi_end_command(disk);
    if (MMC_DATA_TOKEN_SUCCESS != reply) {
        DEBUG1("mmc_spi_read_register(): unexpected response to command %02x, expected 0x00FE data token, got %02x\n", command, reply);
        return -EIO;
    }
    return ENOERR;
}

// Reading a disk block is just a combination of the above utilities.
// This code is also responsible for translating error codes, since
// higher-level code does not get to see the initial response vs. the
// data token byte.
static Cyg_ErrNo
mmc_spi_read_disk_block(cyg_mmc_spi_disk_info_t* disk, cyg_uint8* buf, cyg_uint32 block, cyg_bool extra_delay)
{
    cyg_uint32 reply;
    
    // First the command itself.
    DEBUG2("mmc_spi_read_disk_block(%d): sending command\n", block);
    reply = mmc_spi_send_command_start(disk, MMC_REQUEST_READ_SINGLE_BLOCK, block * MMC_SPI_BLOCK_SIZE);
    if (MMC_REPLY_SUCCESS != reply) {
        DEBUG1("mmc_spi_read_disk_block(%d): unexpected response to READ_SINGLE_BLOCK command, code %02x\n", block, reply);
        mmc_spi_end_command(disk);
        // A byte 0xFF indicates the card has been removed.
        if (0x00FF == reply) {
            return -ENODEV;
        }
        // Parameter or address error should not occur, higher-level
        // code should have checked the block to ensure that it is
        // in range.
        if (0 != (reply & (MMC_REPLY_PARAMETER_ERROR | MMC_REPLY_ADDRESS_ERROR))) {
            return -EINVAL;
        }
        // The disk should not be in idle state or in an erase sequence. The
        // command is definitely legal and CRCs should be disabled. So everything
        // else is an I/O error.
        return -EIO;
    }
    
    // Now read back the data block. That code can be shared with other read
    // operations, e.g. for retrieving registers.
    DEBUG2("mmc_spi_read_disk_block(%d): reading data token/data/crc\n", block);
    reply = mmc_spi_read_data(disk, buf, MMC_SPI_BLOCK_SIZE, extra_delay);
    mmc_spi_end_command(disk);
    if (MMC_DATA_TOKEN_SUCCESS != reply) {
        DEBUG1("mmc_spi_read_disk_block(%d): failed to retrieve data, error token %02x\n", block, reply);
        
        // Possibilities are password-locked, range error, ECC failure
        // if the raw data is corrupt, CC error for an internal card
        // error, or some other error. A byte 0xFF indicates the card
        // has been removed.
        if (0x00FF == reply) {
            return -ENODEV;
        } else if (0 != (MMC_DATA_ERROR_TOKEN_CARD_LOCKED & reply)) {
            // This should have been caught by a mount operation.
            return -EPERM;
        } else if (0 != (MMC_DATA_ERROR_TOKEN_OUT_OF_RANGE & reply)) {
            return -EINVAL;
        } else {
            return -EIO;
        }
    }
    return ENOERR;
}

// Writing a block involves a bit more work. Some of this could be
// moved into a utility routine if necessary, shared with code for
// e.g. updating the CSD register, but for now that other functionality
// is not needed.
static Cyg_ErrNo
mmc_spi_write_disk_block(cyg_mmc_spi_disk_info_t* disk, const cyg_uint8* buf, cyg_uint32 block)
{
    cyg_spi_device* dev = disk->mmc_spi_dev;
    cyg_uint32      reply;
    cyg_uint8       extra[4];
    int             i;
   
    // First, send the command itself and get the initial response
    DEBUG2("mmc_spi_write_disk_block(), sending command\n");
    reply = mmc_spi_send_command_start(disk, MMC_REQUEST_WRITE_BLOCK, block * MMC_SPI_BLOCK_SIZE);
    if (MMC_REPLY_SUCCESS != reply) {
        DEBUG1("mmc_spi_write_disk_block(): unexpected response to WRITE_BLOCK command, code %02x\n", reply);
        mmc_spi_end_command(disk);
        if (0x00FF == reply) {
            return -ENODEV;
        }
        // Parameter or address error should not occur, higher-level
        // code should have checked the block to ensure that it is
        // in range.
        if (0 != (reply & (MMC_REPLY_PARAMETER_ERROR | MMC_REPLY_ADDRESS_ERROR))) {
            return -EINVAL;
        }
        // The disk should not be in idle state or in an erase sequence. The
        // command is definitely legal and CRCs should be disabled. So everything
        // else is an I/O error.
        return -EIO;
    }

    // The card is now expecting a data block. This consists of a single byte
    // 0x00FE, then the data itself, and a dummy CRC. The reply from the card
    // does not contain any useful information.
    DEBUG2("mmc_spi_write_disk_block(): sending data token/data/crc\n");
    extra[0]    = 0x00FE;
    cyg_spi_transaction_transfer(dev, cyg_mmc_spi_polled, 1, extra, (cyg_uint8*)0, 0);
    cyg_spi_transaction_transfer(dev, cyg_mmc_spi_polled, MMC_SPI_BLOCK_SIZE, buf, (cyg_uint8*)0, 0);
    cyg_spi_transaction_transfer(dev, cyg_mmc_spi_polled, 2, mmc_spi_ff_data, (cyg_uint8*)0, 0);

    // The card should respond immediately with a data response token.
    cyg_spi_transaction_transfer(dev, cyg_mmc_spi_polled, 1, mmc_spi_ff_data, extra, 0);
    DEBUG2("mmc_spi_write_disk_block(): got data response token %02x\n", extra[0]);

    // The bottom five bits contain the response. 00101 indicates success,
    // anything else is a CRC error. Everything else will have been checked
    // before the data got transferred.
    if (0x05 != (extra[0] & 0x1F)) {
        DEBUG1("mmc_spi_write_disk_block(): invalid data response token %02x\n", extra[0]);
        mmc_spi_end_command(disk);
        if (0x00FF == extra[0]) {
            return -ENODEV;
        }
        return -EIO;
    }

#ifdef MMC_SPI_BACKGROUND_WRITES    
    // Mark the card as writing. The next operation will poll for completion.
    disk->mmc_writing   = true;
#else
    // The card is now busy doing the write and will output a stream of 0's
    // while busy. The timeout should really be calculated using the CSD
    // register settings.
    //
    // It should be legal to drop the chip select here, i.e. to end
    // the current transaction and start a new one for each poll
    // operation. That would allow other SPI devices to be accessed.
    // However it appears that this does not work with all MMC cards.
    extra[0]    = 0x00;
    for (i = 0; (i < MMC_SPI_WRITE_BUSY_RETRIES) && (0x00 == extra[0]); i++) {
        cyg_spi_transaction_transfer(dev, cyg_mmc_spi_polled, 1, mmc_spi_ff_data, extra, 0);
        DEBUG2("mmc_spi_write_disk_block(), polling for ! busy, got response %02x\n", extra[0]);
    }
#endif
    
    // Assume that the loop did in fact terminate.
    mmc_spi_end_command(disk);
    return ENOERR;
}

// MMC sockets will default to a slow clockrate. During a successful mount
// the SPI device settings will be changed to the fastest supported by the
// card, as per the CSD register. This will need to be undone during an
// unmount, or if the final stages of a mount are unsuccessful.
static void
mmc_spi_restore_baud(cyg_mmc_spi_disk_info_t* disk)
{
    cyg_uint32  len = sizeof(cyg_uint32);
    (void) cyg_spi_set_config(disk->mmc_spi_dev, CYG_IO_SET_CONFIG_SPI_CLOCKRATE, (void*) &(disk->mmc_saved_baudrate), &len);
}

// check_for_disk() tries to communicate with an MMC card that is not
// currently mounted. It performs the appropriate initialization so
// that read and write operations are possible, checks the disk format,
// distinguishes between read-only and read-write cards, calculates the
// card size, stores the unique id, etc.
//
// The main error conditions are ENODEV (no card), EIO (card not
// responding sensibly to requests), ENOTDIR (wrong format), or EPERM
// (card is password-locked).
static Cyg_ErrNo
mmc_spi_check_for_disk(cyg_mmc_spi_disk_info_t* disk)
{
    cyg_spi_device*     dev = disk->mmc_spi_dev;
    int                 i;
    cyg_uint32          reply;
    Cyg_ErrNo           code;
    mmc_csd_register    csd;

#ifdef MMC_SPI_BACKGROUND_WRITES    
    // If we have unmounted a disk and are remounting it, assume that
    // any writes have completed.
    disk->mmc_writing   = false;
#endif    
    reply               = 0x00ff;
    
    for (i = 0; (i < MMC_SPI_GO_IDLE_RETRIES) && (0x01 != reply); i++) {
        // Allow platform HALs to provide additional initialization,
        // if the hardware needs it.
#ifdef HAL_MMC_SPI_INIT
        HAL_MMC_SPI_INIT(dev, reply);
        if (! reply) {
            return -ENODEV;
        }
#endif
        // MMC cards generic initialization. The card may have just
        // been plugged in so there is no guarantee that any previous
        // init() calls or other traffic will have affected this card.
        mmc_spi_send_init(disk);
    
        // Now set the card to idle state. This involves the GO_IDLE_STATE
        // command which will be accepted irrespective of whether the card is
        // currently in MMC or SPI mode, and will leave the card in SPI mode.
        reply = mmc_spi_send_command(disk, MMC_REQUEST_GO_IDLE_STATE, 0);

        // The card should reply with 0x01. FF suggests that there is
        // no card. Any other response indicates some synchronization
        // problem. For example the card might still be responding to
        // some request from a previous session which aborted at an
        // inconvenient moment. Some dummy traffic is generated in the
        // hope that this gets things back in sync.
        if (0x01 != reply) {
            DEBUG1("mmc_spi_check_for_disk(): loop %d, card did not enter idle state, code %02x\n", i, reply);
            if (0x0ff != reply) {
                cyg_spi_transfer(dev, cyg_mmc_spi_polled, 128, mmc_spi_ff_data, (cyg_uint8*) 0);
            }
        }
    }
    if (0x0ff == reply) {
        DEBUG1("mmc_spi_check_for_disk(): unable to get a response from the MMC card: code %02x\n", reply);
        // A working card should be returning some data
        return -ENODEV;
    }
    if (0x01 != reply) {
        DEBUG1("mmc_spi_check_for_disk(): card did not enter idle state, code %02x\n", reply);
        return -EIO;
    }
    
    // Next, wait for the card to initialize. This involves repeatedly
    // trying the SEND_OP_COND command until we get a reply that is
    // not idle
    reply = 0x00ff;
    for (i = 0; (i < MMC_SPI_OP_COND_RETRIES) && ((0x00ff == reply) || (0 != (MMC_REPLY_IN_IDLE_STATE & reply))); i++) {
#ifdef CYGPKG_DEVS_DISK_MMC_SPI_IDLE_RETRIES_WAIT
        CYGACC_CALL_IF_DELAY_US(CYGPKG_DEVS_DISK_MMC_SPI_IDLE_RETRIES_WAIT);
#endif
        reply = mmc_spi_send_command(disk, MMC_REQUEST_SEND_OP_COND, 0);
    }
    if (MMC_REPLY_SUCCESS != reply) {
        DEBUG1("mmc_spi_check_for_disk(): card has not entered operational state: reply code %02x\n", reply);
        return (0x00FF == reply) ? -ENODEV : -EIO;
    }

    // The card has now generated sufficient responses that we don't need to
    // worry about a missing card anymore.
    // Get hold of the card's unique ID and store it, to allow disk changes
    // to be detected.
    code = mmc_spi_read_register(disk, MMC_REQUEST_SEND_CID, (cyg_uint8*) &(disk->mmc_id), 16);
    if (code) {
        mmc_spi_end_command(disk);
        return code;
    }
    DEBUG2("CID data: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",               \
           disk->mmc_id.cid_data[ 0], disk->mmc_id.cid_data[ 1], disk->mmc_id.cid_data[ 2], disk->mmc_id.cid_data[ 3],  \
           disk->mmc_id.cid_data[ 4], disk->mmc_id.cid_data[ 5], disk->mmc_id.cid_data[ 6], disk->mmc_id.cid_data[ 7],  \
           disk->mmc_id.cid_data[ 8], disk->mmc_id.cid_data[ 9], disk->mmc_id.cid_data[10], disk->mmc_id.cid_data[11],  \
           disk->mmc_id.cid_data[12], disk->mmc_id.cid_data[13], disk->mmc_id.cid_data[14], disk->mmc_id.cid_data[15]);
#if DEBUG > 0
    DEBUG1("CID data: register\n");
    DEBUG1("        : Manufacturer ID       : MID = 0x%02x\n", MMC_CID_REGISTER_MID(&(disk->mmc_id)) & 0xff);
    DEBUG1("        : OEM/Application ID    : OID = 0x%04x\n", MMC_CID_REGISTER_OID(&(disk->mmc_id)) & 0xffff);
    DEBUG1("        : Product name          : PNM = 0x%02x%02x%02x%02x%02x%02x\n",
                                                               MMC_CID_REGISTER_PNM(&(disk->mmc_id))[0] & 0xff,
                                                               MMC_CID_REGISTER_PNM(&(disk->mmc_id))[1] & 0xff,
                                                               MMC_CID_REGISTER_PNM(&(disk->mmc_id))[2] & 0xff,
                                                               MMC_CID_REGISTER_PNM(&(disk->mmc_id))[3] & 0xff,
                                                               MMC_CID_REGISTER_PNM(&(disk->mmc_id))[4] & 0xff,
                                                               MMC_CID_REGISTER_PNM(&(disk->mmc_id))[5] & 0xff);
    DEBUG1("        : Product revision      : PRV = 0x%02x\n", MMC_CID_REGISTER_PRV(&(disk->mmc_id)) & 0xff);
    DEBUG1("        : Product serial number : PSN = 0x%08x\n", MMC_CID_REGISTER_PSN(&(disk->mmc_id)) & 0xffffffff);
    DEBUG1("        : Manufacturing date    : MDT = 0x%02x\n", MMC_CID_REGISTER_MDT(&(disk->mmc_id)) & 0xff);
    DEBUG1("        : 7-bit CRC checksum    : CRC = 0x%02x\n", MMC_CID_REGISTER_CRC(&(disk->mmc_id)) & 0xff);
#endif

    // And retrieve the card's configuration data.
    code = mmc_spi_read_register(disk, MMC_REQUEST_SEND_CSD, (cyg_uint8*) &csd, 16);
    if (code) {
        mmc_spi_end_command(disk);
        return code;
    }
    DEBUG2("CSD data: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",   \
           csd.csd_data[ 0], csd.csd_data[ 1], csd.csd_data[ 2], csd.csd_data[3],                           \
           csd.csd_data[ 4], csd.csd_data[ 5], csd.csd_data[ 6], csd.csd_data[7],                           \
           csd.csd_data[ 8], csd.csd_data[ 9], csd.csd_data[10], csd.csd_data[11],                          \
           csd.csd_data[12], csd.csd_data[13], csd.csd_data[14], csd.csd_data[15]);
    
    // Optionally dump the whole CSD register. This takes a lot of
    // code but gives a lot of info about the card. If the info looks
    // correct then we really are interacting properly with an MMC card.
#if DEBUG > 0
    DEBUG1("CSD data: structure 0x%02x, version 0x%02x\n", MMC_CSD_REGISTER_CSD_STRUCTURE(&csd), MMC_CSD_REGISTER_SPEC_VERS(&csd));
    if (0 != MMC_CSD_REGISTER_FILE_FORMAT_GROUP(&csd)) {
        DEBUG1("        : Reserved (unknown), FILE_FORMAT_GROUP %d, FILE_FORMAT %d\n", \
                    MMC_CSD_REGISTER_FILE_FORMAT_GROUP(&csd), MMC_CSD_REGISTER_FILE_FORMAT(&csd));
    } else if (0 == MMC_CSD_REGISTER_FILE_FORMAT(&csd)) {
        DEBUG1("        : Partioned disk, FILE_FORMAT_GROUP 0, FILE_FORMAT 0\n");
    } else if (1 == MMC_CSD_REGISTER_FILE_FORMAT(&csd)) {
        DEBUG1("        : FAT disk, FILE_FORMAT_GROUP 0, FILE_FORMAT 1\n");
    } else if (2 == MMC_CSD_REGISTER_FILE_FORMAT(&csd)) {
        DEBUG1("        : Universal File format, FILE_FORMAT_GROUP 0, FILE_FORMAT 2\n");
    } else {
        DEBUG1("        : Others/Unknown disk, FILE_FORMAT_GROUP 0, FILE_FORMAT 3\n");
    }
    {
        static const cyg_uint32 mantissa_speeds_x10[16]   = { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };
        static const cyg_uint32 exponent_speeds_div10[8]  = { 10000, 100000, 1000000, 10000000, 0, 0, 0, 0 };
        cyg_uint32 speed = mantissa_speeds_x10[MMC_CSD_REGISTER_TRAN_SPEED_MANTISSA(&csd)] *
            exponent_speeds_div10[MMC_CSD_REGISTER_TRAN_SPEED_EXPONENT(&csd)];
        speed /= 1000;
        DEBUG1("        : TRAN_SPEED %d %d -> %d kbit/s\n", \
               MMC_CSD_REGISTER_TRAN_SPEED_MANTISSA(&csd), MMC_CSD_REGISTER_TRAN_SPEED_EXPONENT(&csd), speed);
    }        
    DEBUG1("        : READ_BL_LEN block length 2^%d (%d)\n", MMC_CSD_REGISTER_READ_BL_LEN(&csd), \
                0x01 << MMC_CSD_REGISTER_READ_BL_LEN(&csd));
    DEBUG1("        : C_SIZE %d, C_SIZE_MULT %d\n", MMC_CSD_REGISTER_C_SIZE(&csd), MMC_CSD_REGISTER_C_SIZE_MULT(&csd));
    {
        cyg_uint32 block_len = 0x01 << MMC_CSD_REGISTER_READ_BL_LEN(&csd);
        cyg_uint32 mult      = 0x01 << (MMC_CSD_REGISTER_C_SIZE_MULT(&csd) + 2);
        cyg_uint32 size      = block_len * mult * (MMC_CSD_REGISTER_C_SIZE(&csd) + 1);
        cyg_uint32 sizeK     = (cyg_uint32) (size / 1024);
        cyg_uint32 sizeM     =  sizeK / 1024;
        sizeK  -= (sizeM * 1024);
        DEBUG1("        : total card size %dM%dK\n", sizeM, sizeK);
    }
    DEBUG1("        : WR_BL_LEN block length 2^%d (%d)\n", \
           MMC_CSD_REGISTER_WRITE_BL_LEN(&csd), 0x01 << MMC_CSD_REGISTER_WRITE_BL_LEN(&csd));
    {
        static cyg_uint32 taac_mantissa_speeds_x10[16]   = { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };
        static cyg_uint32 taac_exponent_speeds_div10[8]  = { 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000 };
        cyg_uint32 taac_speed = taac_mantissa_speeds_x10[MMC_CSD_REGISTER_TAAC_MANTISSA(&csd)] *
            taac_exponent_speeds_div10[MMC_CSD_REGISTER_TAAC_EXPONENT(&csd)];
        taac_speed /= 100;
        DEBUG1("        : asynchronous read access time TAAC %d %d -> %d ns\n", \
               MMC_CSD_REGISTER_TAAC_MANTISSA(&csd), MMC_CSD_REGISTER_TAAC_EXPONENT(&csd), taac_speed);
    }
    DEBUG1("        : synchronous read access time NSAC %d * 100 cycles\n", \
           MMC_CSD_REGISTER_NSAC(&csd));
    DEBUG1("        : typical write program time %d * read time\n", MMC_CSD_REGISTER_R2W_FACTOR(&csd));
    DEBUG1("        : CCC command classes 0x%04x\n", MMC_CSD_REGISTER_CCC(&csd));
    DEBUG1("        : READ_BL_PARTIAL %d, WRITE_BLK_MISALIGN %d, READ_BLK_MISALIGN %d, DSR_IMP %d\n",   \
           MMC_CSD_REGISTER_READ_BL_PARTIAL(&csd), MMC_CSD_REGISTER_WRITE_BLK_MISALIGN(&csd),           \
           MMC_CSD_REGISTER_READ_BLK_MISALIGN(&csd), MMC_CSD_REGISTER_DSR_IMP(&csd));
    DEBUG1("        : WR_BL_PARTIAL %d\n", MMC_CSD_REGISTER_WR_BL_PARTIAL(&csd));
    {
        static cyg_uint8    min_currents[8] = { 1, 1, 5, 10, 25, 35, 60, 100 };
        static cyg_uint8    max_currents[8] = { 1, 5, 10, 25, 35, 45, 80, 200 };
        DEBUG1("        : read current min %dmA, max %dmA\n",               \
                    min_currents[MMC_CSD_REGISTER_VDD_R_CURR_MIN(&csd)],    \
                    max_currents[MMC_CSD_REGISTER_VDD_R_CURR_MAX(&csd)]);
        DEBUG1("        : write current min %dmA, max %dmA\n",              \
                    min_currents[MMC_CSD_REGISTER_VDD_W_CURR_MIN(&csd)],    \
                    max_currents[MMC_CSD_REGISTER_VDD_W_CURR_MAX(&csd)]);
    }
    DEBUG1("        : erase sector size %d, erase group size %d\n", \
           MMC_CSD_REGISTER_SECTOR_SIZE(&csd) + 1, MMC_CSD_REGISTER_ERASE_GRP_SIZE(&csd) + 1);
    DEBUG1("        : write group enable %d, write group size %d\n", \
           MMC_CSD_REGISTER_WR_GRP_ENABLE(&csd), MMC_CSD_REGISTER_WR_GRP_SIZE(&csd) + 1);
    DEBUG1("        : copy bit %d\n", MMC_CSD_REGISTER_COPY(&csd));
    DEBUG1("        : permanent write protect %d, temporary write protect %d\n", \
           MMC_CSD_REGISTER_PERM_WRITE_PROTECT(&csd), MMC_CSD_REGISTER_TMP_WRITE_PROTECT(&csd));
    DEBUG1("        : ecc %d, default ecc %d\n", MMC_CSD_REGISTER_ECC(&csd), MMC_CSD_REGISTER_DEFAULT_ECC(&csd));
    DEBUG1("        : crc 0x%08x\n", MMC_CSD_REGISTER_CRC(&csd));
#endif                

    // There is information available about the file format, e.g.
    // partitioned vs. simple FAT. With the current version of the
    // generic disk code this needs to be known statically, via
    // the mbr field of the disk channel structure. If the card
    // is inappropriately formatted, reject the mount request.
    if ((0 != MMC_CSD_REGISTER_FILE_FORMAT_GROUP(&csd)) ||
        (0 != MMC_CSD_REGISTER_FILE_FORMAT(&csd))) {
        return -ENOTDIR;
    }

    // Look for a write-protect bit (permanent or temporary), and set
    // the disk as read-only or read-write as appropriate. The
    // temporary write-protect could be cleared by rewriting the CSD
    // register (including recalculating the CRC) but the effort
    // involves does not seem worth-while.
    if ((0 != MMC_CSD_REGISTER_PERM_WRITE_PROTECT(&csd)) || (0 != MMC_CSD_REGISTER_TMP_WRITE_PROTECT(&csd))) {
        disk->mmc_read_only   = true;
    } else {
        disk->mmc_read_only   = false;
    }
    DEBUG1("Disk read-only flag %d\n", disk->mmc_read_only);
    
    // Calculate the disk size, primarily for assertion purposes.
    // By design MMC cards are limited to 4GB, which still doesn't
    // quite fit into 32 bits.
    disk->mmc_block_count = (((cyg_uint64)(0x01 << MMC_CSD_REGISTER_READ_BL_LEN(&csd))) *
                             ((cyg_uint64)(0x01 << (MMC_CSD_REGISTER_C_SIZE_MULT(&csd) + 2))) *
                             ((cyg_uint64)(MMC_CSD_REGISTER_C_SIZE(&csd) + 1))) / (cyg_uint64)MMC_SPI_BLOCK_SIZE;
    DEBUG1("Disk blockcount %d (0x%08x)\n", disk->mmc_block_count, disk->mmc_block_count);
    
    // Assume for now that the block length is 512 bytes. This is
    // probably a safe assumption since we have just got the card
    // initialized out of idle state. If it ever proves to be a problem
    // the SET_BLOCK_LEN command can be used.
    // Nevertheless store the underlying block sizes
    disk->mmc_read_block_length  = 0x01 << MMC_CSD_REGISTER_READ_BL_LEN(&csd);
    disk->mmc_write_block_length = 0x01 << MMC_CSD_REGISTER_WRITE_BL_LEN(&csd);

    // The CSD contains the maximum supported transfer speed. Adjust
    // the SPI device to match, saving the old value for an unmount
    // operation. It is assumed that the SPI bus driver will munge
    // the supplied speed to something appropriate.
    {
        static const cyg_uint32 mantissa_speeds_x10[16]   = { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };
        static const cyg_uint32 exponent_speeds_div10[8]  = { 10000, 100000, 1000000, 10000000, 0, 0, 0, 0 };
        cyg_uint32 speed, len;
        
        len = sizeof(cyg_uint32);
        if (cyg_spi_get_config(dev, CYG_IO_GET_CONFIG_SPI_CLOCKRATE, (void*) &disk->mmc_saved_baudrate, &len)) {
            DEBUG1("Failed to retrieve current SPI device clockrate\n");
            return -EIO;
        }
        speed = mantissa_speeds_x10[MMC_CSD_REGISTER_TRAN_SPEED_MANTISSA(&csd)] * exponent_speeds_div10[MMC_CSD_REGISTER_TRAN_SPEED_EXPONENT(&csd)];
        if (speed > disk->mmc_saved_baudrate) {
            DEBUG1("Old SPI speed %d, switching to %d\n", disk->mmc_saved_baudrate, speed);
            cyg_spi_set_config(dev, CYG_IO_SET_CONFIG_SPI_CLOCKRATE, (void*) &speed, &len);
        } else {
            DEBUG1("Old SPI speed %d already greater than max speed %d, leaving it alone\n",
                   disk->mmc_saved_baudrate, speed);
        }
    }

    // Read the partition table off the card. This is a way of
    // checking that the card is not password-locked. It also
    // provides information about the "disk geometry" which is
    // needed by higher-level code.
    // FIXME: the higher-level code should be made to use LBA
    // addressing instead.
    {
        cyg_uint8   data[MMC_SPI_BLOCK_SIZE];
        cyg_uint8*  partition;
        cyg_uint32  lba_first, lba_size, lba_end, head, cylinder, sector;
        
        code = mmc_spi_read_disk_block(disk, data, 0, true);
        if (code) {
            mmc_spi_restore_baud(disk);
            return code;
        }
#if DEBUG > 1
        {
            cyg_uint8 *ptr_data;

            DEBUG2("MBR dump\n");
            for (i = 0; i < MMC_SPI_BLOCK_SIZE; i += 16) {
                ptr_data = &data[i];
                DEBUG2(" %04x: %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x\n",
                    i,
                    ptr_data[ 0], ptr_data[ 1], ptr_data[ 2], ptr_data[ 3],
                    ptr_data[ 4], ptr_data[ 5], ptr_data[ 6], ptr_data[ 7],
                    ptr_data[ 8], ptr_data[ 9], ptr_data[10], ptr_data[11],
                    ptr_data[12], ptr_data[13], ptr_data[14], ptr_data[15]);
            }
        }
#endif
#if DEBUG > 0
        DEBUG1("Read block 0 (partition table)\n");
        DEBUG1("Signature 0x%02x 0x%02x, should be 0x55 0xaa\n", data[0x1fe], data[0x1ff]);
        // There should be four 16-byte partition table entries at offsets
        // 0x1be, 0x1ce, 0x1de and 0x1ee. The numbers are stored little-endian
        for (i = 0; i < 4; i++) {
            partition = &(data[0x1be + (0x10 * i)]);
            DEBUG1("Partition %d: boot %02x, first sector %02x %02x %02x, file system %02x, last sector %02x %02x %02x\n", i,   \
                   partition[0], partition[1], partition[2], partition[3], partition[4], \
                   partition[5], partition[6], partition[7]);
            DEBUG1("           : first sector (linear) %02x %02x %02x %02x, sector count %02x %02x %02x %02x\n", \
                   partition[11], partition[10], partition[9], partition[8], \
                   partition[15], partition[14], partition[13], partition[12]);
        }
#endif        
        if ((0x0055 != data[0x1fe]) || (0x00aa != data[0x1ff])) {
            mmc_spi_restore_baud(disk);
            return -ENOTDIR;
        }
        partition   = &(data[0x1be]);
        lba_first   = (partition[11] << 24) | (partition[10] << 16) | (partition[9] << 8) | partition[8];
        lba_size    = (partition[15] << 24) | (partition[14] << 16) | (partition[13] << 8) | partition[12];
        lba_end     = lba_first + lba_size - 1;

        // First sector in c/h/s format
        cylinder    = ((partition[2] & 0xC0) << 2) | partition[3];
        head        = partition[1];
        sector      = partition[2] & 0x3F;

        // lba_start == (((cylinder * Nh) + head) * Ns) + sector - 1, where (Nh == heads/cylinder) and (Ns == sectors/head)
        // Strictly speaking we should be solving some simultaneous
        // equations here for lba_start/lba_end, but that gets messy.
        // The first partition is at the start of the card so cylinder will be 0,
        // and we can ignore Nh.
        CYG_ASSERT(0 == cylinder, "Driver assumption - partition 0 is at start of card\n");
        CYG_ASSERT(0 != head,     "Driver assumption - partition table is sensible\n");
        disk->mmc_sectors_per_head = ((lba_first + 1) - sector) / head;

        // Now for lba_end.
        cylinder    = ((partition[6] & 0xC0) << 2) | partition[7];
        head        = partition[5];
        sector      = partition[6] & 0x3F;
        disk->mmc_heads_per_cylinder = ((((lba_end + 1) - sector) / disk->mmc_sectors_per_head) - head) / cylinder;
    }
    
    return ENOERR;
}

// Check that the current card is the one that was previously
// accessed. This may fail if the card has been removed and the
// slot is empty, or if the card has been removed and a different
// one inserted. It may pass incorrectly if a card is removed,
// modified elsewhere, and reinserted without eCos noticing.
// There is no way around that without some way of detecting
// disk removal in hardware.
//
// Re-reading the cid may actually be overkill. If a new card
// has been plugged in then it will not have been initialized so
// it will respond with 0xff anyway. It is very unlikely that
// an init sequence will have happened by accident.
static cyg_bool
mmc_spi_disk_changed(cyg_mmc_spi_disk_info_t* disk)
{
    mmc_cid_register    cid;
    Cyg_ErrNo           code;

    code = mmc_spi_read_register(disk, MMC_REQUEST_SEND_CID, (cyg_uint8*) &cid, 16);
    if (-ENODEV == code) {
        return true;
    }

    if (0 != memcmp(&cid, &(disk->mmc_id), sizeof(mmc_cid_register))) {
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------

// No hardware initialization is performed here. Even if a card is
// currently plugged in it may get removed before it gets mounted, so
// there is no point looking at the card here. It is still necessary
// to invoke the callback init function so that higher-level code gets
// a chance to do its bit.
static cyg_bool
mmc_spi_disk_init(struct cyg_devtab_entry* tab)
{
    disk_channel*   chan    = (disk_channel*) tab->priv;
    MMC_SPI_INIT_FF_DATA();
    return (*chan->callbacks->disk_init)(tab);
}

// lookup() is called during a mount() operation, so this is the right
// place to check whether or not there is a card.

static char*
mmc_spi_disk_lookup_itoa(cyg_uint32 num, char* where)
{
    if (0 == num) {
        *where++ = '0';
    } else {
        char local[10];  // 2^32 just fits into 10 places
        int  index = 9;
        while (num > 0) {
            local[index--] = (num % 10) + '0';
            num /= 10;
        }
        for (index += 1; index < 10; index++) {
            *where++ = local[index];
        }
    }
    return where;
}

static Cyg_ErrNo
mmc_spi_disk_lookup(struct cyg_devtab_entry** tab, struct cyg_devtab_entry *sub_tab, const char* name)
{
    disk_channel*               chan    = (disk_channel*) (*tab)->priv;
    cyg_mmc_spi_disk_info_t*    disk    = (cyg_mmc_spi_disk_info_t*) chan->dev_priv;
    Cyg_ErrNo                   result;

    DEBUG2("mmc_spi_disk_lookup(): target name=%s\n", name );
    DEBUG2("                     : device name=%s dep_name=%s\n", tab[0]->name, tab[0]->dep_name );
    DEBUG2("                     : sub    name=%s dep_name=%s\n", sub_tab->name, sub_tab->dep_name );

    if (disk->mmc_connected) {
        // There was a card plugged in last time we looked. Is it still there?
        if (mmc_spi_disk_changed(disk)) {
            // The old card is gone. Either there is no card plugged in, or
            // it has been replaced with a different one. If the latter the
            // existing mounts must be removed before anything sensible
            // can be done.
            disk->mmc_connected = false;
            (*chan->callbacks->disk_disconnected)(chan);
            if (0 != chan->info->mounts) {
                return -ENODEV;
            }
        }
    }

    if ((0 != chan->info->mounts) && !disk->mmc_connected) {
        // There are still mount points to an old card. We cannot accept
        // new mount requests until those have been cleaned out.
        return -ENODEV;
    }

    if (!disk->mmc_connected) {
        cyg_disk_identify_t ident;
        cyg_uint32          id_data;
        char*               where;
        int                 i;
        
        // The world is consistent and the higher-level code does not
        // know anything about the current card, if any. Is there a
        // card?
        result = mmc_spi_check_for_disk(disk);
        if (ENOERR != result) {
            return result;
        }
        // A card has been found. Tell the higher-level code about it.
        // This requires an identify structure, although it is not
        // entirely clear what purpose that serves.
        disk->mmc_connected = true;
        // Serial number, up to 20 characters; The CID register contains
        // various fields which can be used for this.
        where   = &(ident.serial[0]);
        id_data = disk->mmc_id.cid_data[0];   // 1-byte manufacturer id -> 3 chars, 17 left
        where   = mmc_spi_disk_lookup_itoa(id_data, where);
        id_data = (disk->mmc_id.cid_data[1] << 8) + disk->mmc_id.cid_data[2]; // 2-byte OEM ID, 5 chars, 12 left
        where   = mmc_spi_disk_lookup_itoa(id_data, where);
        id_data = (disk->mmc_id.cid_data[10] << 24) + (disk->mmc_id.cid_data[11] << 16) +
            (disk->mmc_id.cid_data[12] << 8) + disk->mmc_id.cid_data[13];
        where   = mmc_spi_disk_lookup_itoa(id_data, where); // 4-byte OEM ID, 10 chars, 2 left
        // And terminate the string with a couple of places to spare.
        *where = '\0';

        // Firmware revision number. There is a one-byte product
        // revision number in the CID, BCD-encoded
        id_data = disk->mmc_id.cid_data[9] >> 4;
        if (id_data <= 9) {
            ident.firmware_rev[0] = id_data + '0';
        } else {
            ident.firmware_rev[0] = id_data - 10 + 'A';
        }
        id_data = disk->mmc_id.cid_data[9] & 0x0F;
        if (id_data <= 9) {
            ident.firmware_rev[1] = id_data + '0';
        } else {
            ident.firmware_rev[1] = id_data - 10 + 'A';
        }
        ident.firmware_rev[2]   = '\0';
        
        // Model number. There is a six-byte product name in the CID.
        for (i = 0; i < 6; i++) {
            if ((disk->mmc_id.cid_data[i + 3] >= 0x20) && (disk->mmc_id.cid_data[i+3] <= 0x7E)) {
                ident.model_num[i] = disk->mmc_id.cid_data[i + 3];
            } else {
                break;
            }
        }
        ident.model_num[i] = '\0';

        // We don't have no cylinders, heads, or sectors, but
        // higher-level code may interpret partition data using C/H/S
        // addressing rather than LBA. Hence values for some of these
        // settings were calculated above.
        ident.cylinders_num     = 1;
        ident.heads_num         = disk->mmc_heads_per_cylinder;
        ident.sectors_num       = disk->mmc_sectors_per_head;
        ident.lba_sectors_num   = disk->mmc_block_count;
        ident.phys_block_size   = disk->mmc_write_block_length/512;
        ident.max_transfer      = disk->mmc_write_block_length;

        DEBUG1("Calling disk_connected(): serial %s, firmware %s, model %s, heads %d, sectors %d, lba_sectors_num %d, phys_block_size %d\n", \
               ident.serial, ident.firmware_rev, ident.model_num, ident.heads_num, ident.sectors_num,
               ident.lba_sectors_num, ident.phys_block_size);
        (*chan->callbacks->disk_connected)(*tab, &ident);

        // We now have a valid card and higher-level code knows about it. Fall through.
    }

    // And leave it to higher-level code to finish the lookup, taking
    // into accounts partitions etc.
    return (*chan->callbacks->disk_lookup)(tab, sub_tab, name);
}

static Cyg_ErrNo
mmc_spi_disk_read(disk_channel* chan, void* buf_arg, cyg_uint32 blocks, cyg_uint32 first_block)
{
    cyg_mmc_spi_disk_info_t*    disk    = (cyg_mmc_spi_disk_info_t*) chan->dev_priv;
    cyg_uint32                  i;
    cyg_uint8*                  buf = (cyg_uint8*) buf_arg;
    Cyg_ErrNo                   code = ENOERR;
    
    DEBUG1("mmc_spi_disk_read(): first block %d, buf %p, len %lu blocks (%lu bytes)\n",
           first_block, buf, (unsigned long)blocks, (unsigned long)blocks*512);

    if (! disk->mmc_connected) {
        return -ENODEV;
    }
    if ((first_block + blocks) >= disk->mmc_block_count) {
        return -EINVAL;
    }

    for (i = 0; (i < blocks) && (ENOERR == code); i++) {
        code = mmc_spi_read_disk_block(disk, buf, first_block + i, false);
        buf += MMC_SPI_BLOCK_SIZE;
    }
    return code;
}

static Cyg_ErrNo
mmc_spi_disk_write(disk_channel* chan, const void* buf_arg, cyg_uint32 blocks, cyg_uint32 first_block)
{
    cyg_mmc_spi_disk_info_t*    disk    = (cyg_mmc_spi_disk_info_t*) chan->dev_priv;
    cyg_uint32                  i;
    const cyg_uint8*            buf = (cyg_uint8*) buf_arg;
    Cyg_ErrNo                   code = ENOERR;

    DEBUG1("mmc_spi_disk_write(): first block %d, buf %p, len %lu blocks (%lu bytes)\n",
           first_block, buf, (unsigned long)blocks, (unsigned long)blocks*512);

    if (! disk->mmc_connected) {
        return -ENODEV;
    }
    if (disk->mmc_read_only) {
        return -EROFS;
    }
    if ((first_block + blocks) >= disk->mmc_block_count) {
        return -EINVAL;
    }

    for (i = 0; (i < blocks) && (ENOERR == code); i++) {
        code = mmc_spi_write_disk_block(disk, buf, first_block + i);
        buf += MMC_SPI_BLOCK_SIZE;
    }
    return code;
}

// get_config() and set_config(). There are no supported get_config() operations
// at this time.
static Cyg_ErrNo
mmc_spi_disk_get_config(disk_channel* chan, cyg_uint32 key, const void* buf, cyg_uint32* len)
{
    CYG_UNUSED_PARAM(disk_channel*, chan);
    CYG_UNUSED_PARAM(cyg_uint32, key);
    CYG_UNUSED_PARAM(const void*, buf);
    CYG_UNUSED_PARAM(cyg_uint32*, len);
    
    return -EINVAL;
}

static Cyg_ErrNo
mmc_spi_disk_set_config(disk_channel* chan, cyg_uint32 key, const void* buf, cyg_uint32* len)
{
    Cyg_ErrNo                   result  = ENOERR;
    cyg_mmc_spi_disk_info_t*    disk    = (cyg_mmc_spi_disk_info_t*) chan->dev_priv;

    switch(key) {
      case CYG_IO_SET_CONFIG_DISK_MOUNT:
        // There will have been a successful lookup(), so there's
        // little point in checking the disk again.
        break;

      case CYG_IO_SET_CONFIG_DISK_UMOUNT:
        if (0 == chan->info->mounts) {
            // If this is the last unmount of the card, mark it as
            // disconnected. If the user then removes the card and
            // plugs in a new one everything works cleanly. Also
            // reset the SPI device's clockrate.
            disk->mmc_connected = false;
            mmc_spi_restore_baud(disk);
            result = (chan->callbacks->disk_disconnected)(chan);
        }
        break;
    }

    return result;
}

// ----------------------------------------------------------------------------
// And finally the data structures that define this disk. Some of this
// should be moved into an exported header file so that applications can
// define additional disks.
//
// It is not obvious why there are quite so many structures. Apart
// from the devtab entries there are no tables involved, so there is
// no need to keep everything the same size. The cyg_disk_info_t could
// be the common part of a h/w info_t. The channel structure is
// redundant and its fields could be merged into the cyg_disk_info_t
// structure. That would leave a devtab entry, a disk info structure
// (h/w specific but with a common base), and a disk controller
// structure (ditto).

DISK_FUNS(cyg_mmc_spi_disk_funs,
          mmc_spi_disk_read,
          mmc_spi_disk_write,
          mmc_spi_disk_get_config,
          mmc_spi_disk_set_config
          );

static cyg_mmc_spi_disk_info_t cyg_mmc_spi_disk0_hwinfo = {
    .mmc_spi_dev        = &cyg_spi_mmc_dev0,
#ifdef MMC_SPI_BACKGROUND_WRITES    
    .mmc_writing        = 0,
#endif    
    .mmc_connected      = 0
};

// No h/w controller structure is needed, but the address of the
// second argument is taken anyway.
DISK_CONTROLLER(cyg_mmc_spi_disk_controller_0, cyg_mmc_spi_disk0_hwinfo);

DISK_CHANNEL(cyg_mmc_spi_disk0_channel,
             cyg_mmc_spi_disk_funs,
             cyg_mmc_spi_disk0_hwinfo,
             cyg_mmc_spi_disk_controller_0,
             true,                            /* MBR support */
             1                                /* Number of partitions supported */
             );
             
BLOCK_DEVTAB_ENTRY(cyg_mmc_spi_disk0_devtab_entry,
                   CYGDAT_DEVS_DISK_MMC_SPI_DISK0_NAME,
                   0,
                   &cyg_io_disk_devio,
                   &mmc_spi_disk_init,
                   &mmc_spi_disk_lookup,
                   &cyg_mmc_spi_disk0_channel);

// EOF mmc_spi.c
