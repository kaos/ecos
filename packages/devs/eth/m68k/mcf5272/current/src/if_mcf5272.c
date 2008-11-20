//==========================================================================
//
//      if_mcfxxxx.c
//
//	    Ethernet driver for Freescale MCFxxxx coldfires
//
//==========================================================================
//###ECOSGPLCOPYRIGHTBEGIN####
//-------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003,2004,2006,2007,2008 Free Software Foundation, Inc.
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
// Author(s):    bartv
// Contributors:
// Date:         2003-06-04
// Description:  hardware driver for MCFxxxx ethernet devices.
//
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/system.h>
#include CYGBLD_HAL_PLATFORM_H
#include <pkgconf/devs_eth_mcfxxxx.h>
#include <pkgconf/io_eth_drivers.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_if.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/drv_api.h>
#include <cyg/io/eth/netdev.h>
#include <cyg/io/eth/eth_drv.h>
#include <string.h>
#ifdef CYGPKG_NET
#include <pkgconf/net.h>
#include <net/if.h>  /* Needed for struct ifnet */
#endif
#include <cyg/io/eth/eth_drv_stats.h>

// ----------------------------------------------------------------------------
// Support for multiple devices - still incomplete. 
# define EIR(_eth_)             (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_EIR)
# define EIMR(_eth_)            (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_EIMR)
# define RDAR(_eth_)            (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_RDAR)
# define TDAR(_eth_)            (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_TDAR)
# define ECR(_eth_)             (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_ECR)
# define MMFR(_eth_)            (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_MMFR)
# define MSCR(_eth_)            (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_MSCR)
# define MIBC(_eth_)            (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_MIBC)
# define RCR(_eth_)             (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_RCR)
# define TCR(_eth_)             (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_TCR)
# define PALR(_eth_)            (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_PALR)
# define PAUR(_eth_)            (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_PAUR)
# define OPD(_eth_)             (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_OPD)
# define IAUR(_eth_)            (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_IAUR)
# define IALR(_eth_)            (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_IALR)
# define GAUR(_eth_)            (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_GAUR)
# define GALR(_eth_)            (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_GALR)
# define TFWR(_eth_)            (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_TFWR)
# define FRBR(_eth_)            (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_FRBR)
# define FRSR(_eth_)            (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_FRSR)
# define ERDSR(_eth_)           (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_ERDSR)
# define ETDSR(_eth_)           (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_ETDSR)
# define EMRBR(_eth_)           (HAL_MCFxxxx_ETH0_BASE + HAL_MCFxxxx_ETHx_EMRBR)

// ----------------------------------------------------------------------------
// Debug support, in the form of diagnostics and a simple trace
// buffer. Also make it easy to enable stats without messing about
// with the configuration.

#define DIAG_LEVEL     0
#define DIAG_BUFFERED  0
#define DIAG_WRAP      1
#ifndef CYGFUN_DEVS_ETH_MCFxxxx_STATISTICS
//# define CYGFUN_DEVS_ETH_MCFxxxx_STATISTICS 1
#endif

#if (DIAG_LEVEL <= 0)

# define DIAG(_level_, _fmt_, ...)                  \
    CYG_MACRO_START                                 \
    CYG_MACRO_END

# define DIAGPKT(_level_, _msg_, _pkt_, _len_)      \
    CYG_MACRO_START                                 \
    CYG_MACRO_END

#elif (! DIAG_BUFFERED)

#  define DIAG(_level_, _fmt_, ...)                                             \
    CYG_MACRO_START                                                             \
    if ((_level_) <= DIAG_LEVEL) {                                              \
        diag_printf("mcfxxxx_eth %s : " _fmt_ "\n", __func__, ## __VA_ARGS__);  \
    }                                                                           \
    CYG_MACRO_END

# define DIAGPKT(_level_, _msg_, _pkt_, _len_)                                  \
    CYG_MACRO_START                                                             \
    if ((_level_) <= DIAG_LEVEL) {                                              \
        diag_printf("mcfxxxx_eth %s : pkt %p, len %d\n", __func__,              \
                    (cyg_uint8*)_pkt_, _len_);                                  \
    }                                                                           \
    CYG_MACRO_END

#else
// Trace buffer size. This has to be kept smallish or RedBoot will overrun
// its RAM allocation. With 24 bytes per entry, 300 entries require a bit
// under 8K.
# define DIAG_BUFFER_SIZE      300
typedef struct mcfxxxx_eth_trace {
    const char*     fn;
    const char*     msg;
    cyg_uint32      len;
    cyg_uint8       packet[14];
} mcfxxxx_eth_trace_entry;
static mcfxxxx_eth_trace_entry  mcfxxxx_eth_trace_data[DIAG_BUFFER_SIZE];
static int                      mcfxxxx_eth_trace_next      = DIAG_BUFFER_SIZE - 1;
static cyg_bool                 mcfxxxx_eth_trace_wrapped   = false;

static void
mcfxxxx_eth_trace(const char* fn, const char* msg, cyg_uint8* packet, cyg_uint32 len)
{
    mcfxxxx_eth_trace_entry* entry = &(mcfxxxx_eth_trace_data[mcfxxxx_eth_trace_next]);

# ifdef DIAG_WRAP
    if (0 == mcfxxxx_eth_trace_next) {
        mcfxxxx_eth_trace_next = DIAG_BUFFER_SIZE - 1;
        mcfxxxx_eth_trace_wrapped = true;
    } else {
        mcfxxxx_eth_trace_next -= 1;
    }
# else
    if (mcfxxxx_eth_trace_next < 0) {
        return;
    }
    mcfxxxx_eth_trace_next -= 1;
# endif

    entry->fn   = fn;
    entry->msg  = msg;
    entry->len  = len;
    if ((cyg_uint8*)0 == packet) {
        memset(entry->packet, 0, 14);
    } else {
        memcpy(entry->packet, packet, 14);
    }
}

#  define DIAG(_level_, _fmt_, ...)                                         \
    CYG_MACRO_START                                                         \
    if ((_level_) <= DIAG_LEVEL) {                                          \
        mcfxxxx_eth_trace(__func__, _fmt_, (cyg_uint8*)0, 0);               \
    }                                                                       \
    CYG_MACRO_END

# define DIAGPKT(_level_, _msg_, _pkt_, _len_)                              \
    CYG_MACRO_START                                                         \
    if ((_level_) <= DIAG_LEVEL) {                                          \
        mcfxxxx_eth_trace(__func__, _msg_, (cyg_uint8*)_pkt_, _len_);       \
    }                                                                       \
    CYG_MACRO_END

#endif

#if (DIAG_LEVEL < 3)
# define WRITE32(_addr_, _val_)         HAL_WRITE_UINT32(_addr_, _val_)
# define READ32( _addr_, _var_)         HAL_READ_UINT32(_addr_, _var_)
#else

# define WRITE32(_addr_, _val_)                                             \
    CYG_MACRO_START                                                         \
    DIAG(DIAG_LEVEL, "WRITE %s %08x <= 0x%08x", # _addr_, _addr_, _val_) ;  \
    HAL_WRITE_UINT32(_addr_, _val_);                                        \
    CYG_MACRO_END

#define READ32(_addr_, _var_)                                               \
    CYG_MACRO_START                                                         \
    HAL_READ_UINT32(_addr_, _var_);                                         \
    DIAG(DIAG_LEVEL, "READ  %s %08x == 0x%08x", # _addr_, _addr_, _var_) ;  \
    CYG_MACRO_END

#endif

#if 0
// A function that gets placed in RAM. This can be called by flash-resident
// code, e.g. RedBoot, when I need a breakpoint on a specific condition.
static void mcfxxxx_eth_ramfn(void) __attribute__((section (".2ram.mcfxxxx_eth_ramfn")));
static int  mcfxxxx_eth_ramfn_calls;
static void
mcfxxxx_eth_ramfn(void)
{
    mcfxxxx_eth_ramfn_calls += 1;
}
#endif

// ----------------------------------------------------------------------------
#ifdef CYGFUN_DEVS_ETH_MCFxxxx_STATISTICS
# define INCR_STAT(_a_)                 \
    CYG_MACRO_START                     \
    _a_ += 1;                           \
    CYG_MACRO_END
#else
# define INCR_STAT(_a_)
#endif

// ----------------------------------------------------------------------------
// Most existing MCFxxxx chips have a single on-chip ethernet device,
// so all device-specific data could be held in statics. However, in
// case this driver ever gets re-used for a chip with multiple
// ethernet devices that data is held in a driver-specific structure.
//
// The driver currently supports n rx buffers, configurable, each of
// maximum size. Incoming frames are then copied to higher-level's code
// sg lists.
//
// For tx the current code supports a single outgoing transmission at a
// time. This does limit outgoing bandwidth a bit, but only a bit, and
// saves memory and complexity.
//
// There are actually two implementations of the TX code. The MCF5272
// has no alignment restrictions for the data being transmitted, so it
// is possible to avoid a copy and put the higher-level sg_list entries
// directly into the device's ring buffer. The MCF5282 does have
// alignment restrictions and the sg_list entries may not be suitably
// aligned, so a copy operation is necessary.
// NOTE: a possible optimization is to detect aligned vs. misaligned
// data and only copy the bits that need aligning. This would significantly
// complicate the code and it is not clear that data would be aligned
// sufficiently often to make it worthwhile, at least not without work
// inside the TCP/IP stack.
//
// With only one outgoing transmission at a time a full ring buffer seems
// overkill. However it appears that the hardware can get confused if the
// ring buffer consists of only a single entry, so the code has to
// implement a full ring buffer anyway

#if defined(HAL_DCACHE_LINE_SIZE) && (HAL_DCACHE_LINE_SIZE != 16)
# error Driver code needs adjusting for cache line sizes other than 16 bytes.
#endif

#if defined(CYGPKG_HAL_M68K_MCF5272)

# undef TX_NEEDS_ALIGNMENT
# define TX_BUFFER_SIZE 0

// Always use an even number of buffer descriptors, to preserve
// alignment to a 16-byte boundary. We need one more entry in the
// ring buffer than SG_LIST_SIZE.
# if (0 == (CYGNUM_IO_ETH_DRIVERS_SG_LIST_SIZE & 0x01))
#  define TX_BUFFER_DESCRIPTOR_COUNT   (CYGNUM_IO_ETH_DRIVERS_SG_LIST_SIZE + 2)
# else
#  define TX_BUFFER_DESCRIPTOR_COUNT   (CYGNUM_IO_ETH_DRIVERS_SG_LIST_SIZE + 1)
# endif

// This #elif should probably become a #else
#elif defined(CYGPKG_HAL_M68K_MCF5282) || defined(CYGPKG_HAL_M68K_MCF532x)

# define TX_NEEDS_ALIGNMENT             1
# define TX_BUFFER_SIZE                 1520
# define TX_BUFFER_DESCRIPTOR_COUNT     2

#else

# error Current processor unsupported

#endif

#if (0 == (CYGNUM_DEVS_ETH_MCFxxxx_RXBUFFERS & 0x01)) || (0 == HAL_DCACHE_SIZE)
# define RX_BUFFER_DESCRIPTOR_PADDING   0
#else
# define RX_BUFFER_DESCRIPTOR_PADDING   16
#endif

typedef struct mcfxxxx_eth {
    
    // There are two separate interrupt vectors to worry about, RXF
    // and TXF. Other interrupt sources such as error conditions are
    // handled by the main tx/rx code, not by separate interrupt
    // handlers.
    cyg_handle_t    interrupt_handle_rx;
    cyg_interrupt   interrupt_data_rx;
    cyg_handle_t    interrupt_handle_tx;
    cyg_interrupt   interrupt_data_tx;

    // Pointers to the buffer descriptors. These index the data[] array
    // below.
    hal_mcfxxxx_eth_buffer_descriptor*  txbds;
    hal_mcfxxxx_eth_buffer_descriptor*  rxbds;
    
#ifdef CYGFUN_DEVS_ETH_MCFxxxx_STATISTICS
    cyg_uint32      interrupts;
    cyg_uint32      tx_count;
    cyg_uint32      tx_good;
    cyg_uint32      tx_max_collisions;
    cyg_uint32      tx_late_collisions;
    cyg_uint32      tx_underrun;
    cyg_uint32      tx_carrier_loss;
    cyg_uint32      tx_deferred;
    cyg_uint32      tx_single_collisions;
    cyg_uint32      tx_mult_collisions;
    cyg_uint32      tx_total_collisions;
    
    cyg_uint32      rx_count;
    cyg_uint32      rx_good;
    cyg_uint32      rx_crc_errors;
    cyg_uint32      rx_align_errors;
    cyg_uint32      rx_overrun_errors;
    cyg_uint32      rx_short_frames;
    cyg_uint32      rx_too_long_frames;
#endif

    unsigned long   tx_key;

    cyg_uint8       tx_index;
    cyg_uint8       started;
    cyg_uint8       tx_can_send;
    cyg_uint8       tx_done;
    cyg_uint8       rx_rdy;
    cyg_uint8       rx_next_buffer;
    cyg_uint16      rx_len;
    cyg_uint8       mac[6];

    // We need:
    // 1) padding to get the data aligned to a 16-byte boundary. A
    //    16 byte alignment for the rx and tx buffer descriptors is
    //    recommended, and should also help with flushing/invalidating
    //    cache lines when necessary.
    // 2) n 1520 byte rx buffers. 1520 is a multiple of 16 so maintains
    //    alignment.
    // 3) possibly a 1520 byte tx buffer, as per TX_ALIGNED_BUF_SIZE.
    // 4) some number of tx buffer descriptors, as per TX_BUFFER_COUNT.
    //    This number will be even, maintaining alignment to 16 bytes.
    // 5) some number of rx buffer descriptors. The number of RX buffers
    //    need not be even since each buffer is large.
    // 6) padding for the RXBUFFERS to ensure that cache lines are not
    //    shared between buffer descriptors and other data.
    cyg_uint8   data[15 +
                     (1520 * CYGNUM_DEVS_ETH_MCFxxxx_RXBUFFERS) +
                     TX_BUFFER_SIZE +
                     (TX_BUFFER_DESCRIPTOR_COUNT * sizeof(hal_mcfxxxx_eth_buffer_descriptor)) +
                     (CYGNUM_DEVS_ETH_MCFxxxx_RXBUFFERS * sizeof(hal_mcfxxxx_eth_buffer_descriptor)) +
                     RX_BUFFER_DESCRIPTOR_PADDING
        ];
} mcfxxxx_eth;

// This structure is held in bss so all stats etc. are automatically zeroed.
static mcfxxxx_eth  mcfxxxx_eth_info;

ETH_DRV_SC(mcfxxxx_eth_sc0,
           (void*) &mcfxxxx_eth_info,
           CYGDAT_DEVS_ETH_MCFxxxx_NAME,
           mcfxxxx_eth_start,
           mcfxxxx_eth_stop,
           mcfxxxx_eth_ioctl,
           mcfxxxx_eth_can_send,
           mcfxxxx_eth_send,
           mcfxxxx_eth_recv,
           mcfxxxx_eth_deliver,
           mcfxxxx_eth_poll,
           mcfxxxx_eth_int_vector
    );

NETDEVTAB_ENTRY(mcfxxxx_eth_netdev0,
                "mcfxxxx-" CYGDAT_DEVS_ETH_MCFxxxx_NAME,
                mcfxxxx_eth_init,
                &mcfxxxx_eth_sc0);

static cyg_uint32   mcfxxxx_eth_isr_rx(cyg_vector_t, cyg_addrword_t);
static void         mcfxxxx_eth_dsr_rx(cyg_vector_t, cyg_ucount32, cyg_addrword_t);
static cyg_uint32   mcfxxxx_eth_isr_tx(cyg_vector_t, cyg_addrword_t);
static void         mcfxxxx_eth_dsr_tx(cyg_vector_t, cyg_ucount32, cyg_addrword_t);

// ----------------------------------------------------------------------------
// The MAC address. Usually this will be supplied by the platform HAL, but some
// platforms may not have appropriate hardware. In that case the address will
// normally come from RedBoot's config info, with a fallback configurable
// value.
#ifdef CYGINT_DEVS_ETH_MCFxxxx_PLATFORM_MAC

# if !defined(HAL_MCFxxxx_ETH_GET_MAC_ADDRESS) && !defined(HAL_MCFxxxx_ETH_GET_MAC_ADDRESS)
#  error Platform HAL should have provided a MAC address macro
# endif

#else

static cyg_uint8    mcfxxxx_eth_default_mac[6]  = { CYGDAT_DEVS_ETH_MCFxxxx_PLATFORM_MAC } ;

# ifdef CYGPKG_REDBOOT
#  include <pkgconf/redboot.h>
#  ifdef CYGSEM_REDBOOT_FLASH_CONFIG
#   include <redboot.h>
#   include <flash_config.h>
RedBoot_config_option("Network hardware address [MAC]",
                      mcfxxxx_mac,
                      ALWAYS_ENABLED, true,
                      CONFIG_ESA, 0
    );
#  endif
# endif

static void
mcfxxxx_eth_get_config_mac_address(cyg_uint8* mac)
{
    int mac_ok  = 0;
#ifdef CYGPKG_REDBOOT
# ifdef CYGSEM_REDBOOT_FLASH_CONFIG
    mac_ok  = flash_get_config("mcfxxxx_mac", mac, CONFIG_ESA);
# endif    
#else
    // Not in RedBoot. Do we have virtual vectors?
# ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    mac_ok  = CYGACC_CALL_IF_FLASH_CFG_OP(CYGNUM_CALL_IF_FLASH_CFG_GET, "mcfxxxx_mac", mac, CYGNUM_FLASH_CFG_TYPE_CONFIG_ESA);
# endif    
#endif
    if (! mac_ok) {
        memcpy(mac, mcfxxxx_eth_default_mac, 6);
    }
}

#endif // CYGINT_DEVS_ETH_MCFxxxx_PLATFORM_MAC

// ----------------------------------------------------------------------------

static bool
mcfxxxx_eth_init(struct cyg_netdevtab_entry* ndp)
{
    struct eth_drv_sc*      sc  = (struct eth_drv_sc*)  ndp->device_instance;
    struct  mcfxxxx_eth*    eth = (struct mcfxxxx_eth*) sc->driver_private;
    CYG_ADDRWORD            ptr;
    cyg_uint8*              rxbuffers;
    cyg_uint8*              txbuffer;
    cyg_uint32              i;

    DIAG(1, "entry");

#ifdef HAL_MCFxxxx_ETH_DETECT_DEVICE
    if (! HAL_MCFxxxx_ETH_DETECT_DEVICE(HAL_MCFxxxx_ETH0_BASE) ) {
        DIAG(1, "device disabled by HAL");
        return 0;
    }
#endif
    
#ifdef CYGSEM_HAL_USE_ROM_MONITOR
    // If we are debugging over RedBoot, it is possible that there is an
    // outgoing packet still queued up. Give it a chance to get out, a
    // couple of milliseconds should suffice.
    for (i = 0; i < 20; i++) {
        cyg_uint32              x_des_active;
        READ32(TDAR(eth), x_des_active);
        if (0 == (x_des_active & HAL_MCFxxxx_ETHx_TDAR_X_DES_ACTIVE)) {
            break;
        }
        HAL_DELAY_US(100);
    }
#endif

    // Set the whole ethernet device to disabled. Then the other registers
    // can be manipulated safely. The device gets activated in the start
    // function. The reset also sets all registers to their default values.
    WRITE32(ECR(eth), HAL_MCFxxxx_ETHx_ECR_RESET);

    eth->started        = 0;
    // While the reset is going on, sort out the buffers and buffer descriptors.
    // These should be aligned to a 16-byte boundary.
    ptr          = (CYG_ADDRWORD) eth->data;
    ptr          = (ptr + 15) & ~15;
    // First the rx buffers, n * 1520 bytes.
    rxbuffers    = (cyg_uint8*) ptr;
    ptr         += (CYGNUM_DEVS_ETH_MCFxxxx_RXBUFFERS * 1520);
    // We are still aligned to a 16-byte boundary. Now for the tx buffer if necessary.
    txbuffer     = (cyg_uint8*) ptr;
    ptr         += TX_BUFFER_SIZE;
    // Still aligned. The tx buffer descriptors. This will be an even number
    // so alignment is maintained.
    eth->txbds   = (hal_mcfxxxx_eth_buffer_descriptor*) ptr;
    ptr         += (TX_BUFFER_DESCRIPTOR_COUNT * sizeof(hal_mcfxxxx_eth_buffer_descriptor));
    // And finally the rx buffer descriptors.
    eth->rxbds   = (hal_mcfxxxx_eth_buffer_descriptor*) ptr;

    // We can fill in the buffer fields in the various rx buffer descriptors.
    // The length and flag fields are handled by _start().
    for (i = 0; i < CYGNUM_DEVS_ETH_MCFxxxx_RXBUFFERS; i++) {
        eth->rxbds[i].ethbd_buffer   = rxbuffers;
        rxbuffers                   += 1520;
    }

    // Ditto for the tx buffers, if tx involves copying into an
    // aligned buffer. Only one entry in the ring buffer will be used
    // at a time, so the two buffer descriptors can share the same
    // buffer.
#ifdef TX_NEEDS_ALIGNMENT
    eth->txbds[0].ethbd_buffer  = txbuffer;
    eth->txbds[1].ethbd_buffer  = txbuffer;
#endif
    
    // rx_next_buffer tracks the next rx buffer descriptor which the
    // hardware may process.
    eth->rx_next_buffer     = 0;

    // Determine the MAC address. This has to be done by the platform HAL if possible
#ifdef CYGINT_DEVS_ETH_MCFxxxx_PLATFORM_MAC
    HAL_MCFxxxx_ETH_GET_MAC_ADDRESS(eth->mac);
#else
    mcfxxxx_eth_get_config_mac_address(eth->mac);
#endif
    
    // The interrupt handlers can also be installed here. All
    // interrupts are masked after the reset so nothing will
    // actually happen. Separate ISR's and DSR's are used for
    // the various interrupt sources.
    cyg_drv_interrupt_create(CYGNUM_HAL_ISR_ERX,
                             CYGNUM_DEVS_ETH_MCFxxxx_ISR_RX_PRIORITY,
                             (CYG_ADDRWORD) sc,
                             &mcfxxxx_eth_isr_rx,
                             &mcfxxxx_eth_dsr_rx,
                             &(eth->interrupt_handle_rx),
                             &(eth->interrupt_data_rx));
    cyg_drv_interrupt_attach(eth->interrupt_handle_rx);
    cyg_drv_interrupt_unmask(CYGNUM_HAL_ISR_ERX);
    
    cyg_drv_interrupt_create(CYGNUM_HAL_ISR_ETX,
                             CYGNUM_DEVS_ETH_MCFxxxx_ISR_TX_PRIORITY,
                             (CYG_ADDRWORD) sc,
                             &mcfxxxx_eth_isr_tx,
                             &mcfxxxx_eth_dsr_tx,
                             &(eth->interrupt_handle_tx),
                             &(eth->interrupt_data_tx));
    cyg_drv_interrupt_attach(eth->interrupt_handle_tx);
    cyg_drv_interrupt_unmask(CYGNUM_HAL_ISR_ETX);

    // An EBERR bus error will halt the ethernet device and raise an
    // interrupt. Arguably this should be handled here, but the assumption
    // is that the interrupt will never be raised because the driver will
    // never be given a bogus pointer.
    
    // Now the rest of the hardware can be initialized. First control
    // the GPIO pins, or whatever else needs doing on a per-processor
    // basis. For now the PHY is enabled unconditionally.
    // NOTE: much of this initialization code should be moved to a
    // separate reset function which can be called during start-up or
    // after an internal bus error.
#ifdef HAL_MCFxxxx_ETH0_PROC_INIT
    HAL_MCFxxxx_ETH0_PROC_INIT(1);
#endif    
#ifdef HAL_MCFxxxx_ETH0_PLATFORM_INIT
    HAL_MCFxxxx_ETH0_PLATFORM_INIT();
#endif    

    // Now for the ethernet device itself. The order used here is as
    // per the MCF5282 User's Manual.
    //
    // EIMR:     We want interrupts for complete frame transfers only
    WRITE32(EIMR(eth), HAL_MCFxxxx_ETHx_EIMR_TXF  | HAL_MCFxxxx_ETHx_EIMR_RXF);

    // EIR:     Cleared by the reset, but just in case.
    WRITE32(EIR(eth), 0xFFF80000);
    // TFWR:     Leave to its default value
#if defined(HAL_MCFxxxx_ETHx_IAUR)
    // IAUR & IALR.     The MCF5282 also has a unicast hash table, allowing the
    //                  device to accept frames sent to several different addresses.
    //                  This is not supported, only an exact match is used.
    WRITE32(IAUR(eth), 0x0);
    WRITE32(IALR(eth), 0x0);
#endif
    // Multicast hash table (GAUR & GALR). All 1's should be equivalent
    // to multi-all. All 0's should be equivalent to no-multi.
    WRITE32(GAUR(eth), 0x0);
    WRITE32(GALR(eth),  0x0);
    // MAC address (PAUR & PALR)
    i = (eth->mac[4] << 24) | (eth->mac[5] << 16);
    WRITE32(PAUR(eth), i);
    i = (eth->mac[0] << 24) | (eth->mac[1] << 16) | (eth->mac[2] << 8) | (eth->mac[3] << 0);
    WRITE32(PALR(eth), i);
#if defined(HAL_MCFxxxx_ETHx_OPD)
    // OPD: not actually used, but reset it to its default anyway
    WRITE32(OPD(eth), 0x00010000);
#endif
    // RCR:     Default to half-duplex mode. Promiscuous mode may get
    //          set by start().
    //          MII mode should be platform-specific, 7-wire mode is possible.
    //          On an mcf5282 the register also contains the max frame length
#if defined(HAL_MCFxxxx_ETHx_RCR_MAX_FL_VALUE)
    WRITE32(RCR(eth), HAL_MCFxxxx_ETHx_RCR_MAX_FL_VALUE | HAL_MCFxxxx_ETHx_RCR_MII_MODE | HAL_MCFxxxx_ETHx_RCR_DRT);
#else 
    WRITE32(RCR(eth), HAL_MCFxxxx_ETHx_RCR_MII_MODE | HAL_MCFxxxx_ETHx_RCR_DRT);
#endif    
    // TCR:     default to half-duplex
    WRITE32(TCR(eth), 0);

    // MSCR/MII_SPEED.
    // MDC_FREQUENCY == HZ / (4 * reg value)
    // MDC_FREQUENCY should be <= 2.5MHz
    WRITE32(MSCR(eth), ((CYGHWR_HAL_SYSTEM_CLOCK_HZ - 1) / (4 * 2500000)) + 1);
    
    // FRSR:       Leave to its default value
    // EMRBR:   The receiver buffer size is 1520 (must be a multiple of 16 and > frame size)
    WRITE32(EMRBR(eth), 1520);
    // The buffer descriptor rings, ERDSR & ETDSR
    WRITE32(ERDSR(eth), (cyg_uint32)eth->rxbds);
    WRITE32(ETDSR(eth), (cyg_uint32)eth->txbds);

    // Other registers:
    // IVEC: readonly
    // RDAR:        Set by start() and by the receive code
    // TDAR:        Set by start() and by the transmit code
    // MMFR:        There is no need to access the phy
    // R_BOUND:     Read-only
    // MFLR:        The default length is already 1518 bytes, so leave it.
    //              (This register does not exist on the 5282)
    
    // The ethernet device gets enabled in _start()
    
    DIAG(1, "calling higher-level");
    
    // Initialize the upper level driver
    (sc->funs->eth_drv->init)(sc, eth->mac);

    DIAG(1, "done");
    
    return 1;
}

// ----------------------------------------------------------------------------

// The driver only supports one transmit at a time, and a global is used
// to keep track of whether or not a transmit is in progress.
static int
mcfxxxx_eth_can_send(struct eth_drv_sc* sc)
{
    struct  mcfxxxx_eth*    eth = (struct mcfxxxx_eth*) sc->driver_private;
    if (eth->tx_can_send) {
        DIAG(1, "yes");
    } else {
        DIAG(3, "no");
    }
    return eth->tx_can_send;
}

// There are two implementations of the transmit code, one for the case
// where the data needs to be copied into an aligned buffer, the other
// for when the data can be used in place.

static void
mcfxxxx_eth_send(struct eth_drv_sc* sc,
                 struct eth_drv_sg* sg_list, int sg_len, int total_len,
                 unsigned long key)
{
    struct  mcfxxxx_eth*    eth     = (struct mcfxxxx_eth*) sc->driver_private;
    cyg_uint32              index   = eth->tx_index;
    int                     i;

    DIAG(1, " total_len %d", total_len);
    eth->tx_can_send    = 0;
    eth->tx_key         = key;

#ifdef TX_NEEDS_ALIGNMENT
    {
        // The aligned buffer is permanently held in the buffer descriptor.
        cyg_uint8*   buf = eth->txbds[index].ethbd_buffer;
        for (i = 0; i < sg_len; i++) {
            memcpy(buf, (cyg_uint8*) sg_list[i].buf, sg_list[i].len);
            buf += sg_list[i].len;
        }
        eth->txbds[index].ethbd_length  = total_len;
        eth->txbds[index].ethbd_flags  |= HAL_MCFxxxx_ETHx_TXBD_R + HAL_MCFxxxx_ETHx_TXBD_L + HAL_MCFxxxx_ETHx_TXBD_TC;
        DIAGPKT(1, "start", buf, total_len);
        
# ifdef HAL_DCACHE_STORE
        // Make sure the data has been written to memory.
        HAL_DCACHE_STORE(eth->txbds[index].ethbd_buffer, total_len);
# endif
    }
#else
    {
        cyg_uint32 last_index = index;
        for (i = 0; i < sg_len; i++) {
            eth->txbds[index].ethbd_flags   &= ~(HAL_MCFxxxx_ETHx_TXBD_L | HAL_MCFxxxx_ETHx_TXBD_TC);
            // The R bit can be set immediately, rather than in the
            // usual reverse order to prevent an underrun. This is
            // because a new send is only initiated when the transmit
            // engine is idle.
            eth->txbds[index].ethbd_flags   |= HAL_MCFxxxx_ETHx_TXBD_R;
            eth->txbds[index].ethbd_length   = sg_list[i].len;
            eth->txbds[index].ethbd_buffer   = (cyg_uint8*) sg_list[i].buf;
# ifdef HAL_DCACHE_STORE
            HAL_DCACHE_STORE(eth->txbds[index].ethbd_buffer, sg_list[i].len);
# endif            
            last_index  = index;
            if (index == (TX_BUFFER_DESCRIPTOR_COUNT - 1)) {
                index = 0;
            } else {
                index += 1;
            }
        }
        eth->txbds[last_index].ethbd_flags |= HAL_MCFxxxx_ETHx_TXBD_L | HAL_MCFxxxx_ETHx_TXBD_TC;

        eth->tx_index  = last_index;
        DIAGPKT(1, "start", sg_list[0].buf, total_len);
    }
#endif
    
    // Make sure all the buffer descriptors have been written to memory.
    // No need to invalidate the cache entries, the driver does not look
    // for any bits changed by the hardware except on the xxxx which
    // does not have a data cache at all.
#ifdef HAL_DCACHE_STORE    
    HAL_DCACHE_STORE(&(eth->txbds[0]), TX_BUFFER_DESCRIPTOR_COUNT * sizeof(hal_mcfxxxx_eth_buffer_descriptor));
#endif    
    WRITE32(TDAR(eth), HAL_MCFxxxx_ETHx_TDAR_X_DES_ACTIVE);
}

// The TX interrupt should only trigger when a whole frame has been
// transmitted. There is no need to worry about nested tx interrupts,
// at most one transmit can be in progress at any one time. Just
// clear the interrupt pending bit and leave the rest to the DSR.
static cyg_uint32
mcfxxxx_eth_isr_tx(cyg_vector_t vector, cyg_addrword_t data)
{
    HAL_WRITE_UINT32(EIR(eth), HAL_MCFxxxx_ETHx_EIR_TXF | HAL_MCFxxxx_ETHx_EIR_TXB);
    return CYG_ISR_CALL_DSR;
}

static void
mcfxxxx_eth_dsr_tx(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    struct eth_drv_sc*  sc      = (struct eth_drv_sc*) data;
    mcfxxxx_eth*        eth     = (mcfxxxx_eth*)(sc->driver_private);

    INCR_STAT(eth->interrupts);
#ifdef CYGFUN_DEVS_ETH_MCFxxxx_STATISTICS
    
# if defined(CYGPKG_HAL_M68K_MCF5272)
    {
        cyg_uint16          flags;
        // The MCF5272 has all appropriate error flags in the tx buffer descriptor.
        flags = eth->txbds[eth->tx_index].ethbd_flags & (HAL_MCFxxxx_ETHx_TXBD_DEF | HAL_MCFxxxx_ETHx_TXBD_LC | HAL_MCFxxxx_ETHx_TXBD_RL |
                                                         HAL_MCFxxxx_ETHx_TXBD_RC_MASK | HAL_MCFxxxx_ETHx_TXBD_UN | HAL_MCFxxxx_ETHx_TXBD_CSL);
        if (0 == flags) {
            INCR_STAT(eth->tx_good);
        } else {
            if (flags & HAL_MCFxxxx_ETHx_TXBD_RL) {
                eth->tx_max_collisions    = 16;
                INCR_STAT(eth->tx_total_collisions);
            } else {
                int collisions = (flags & HAL_MCFxxxx_ETHx_TXBD_RC_MASK) >> HAL_MCFxxxx_ETHx_TXBD_RC_SHIFT;
                if (collisions > 0) {
                    INCR_STAT(eth->tx_total_collisions);
                    if (collisions > eth->tx_max_collisions) {
                        eth->tx_max_collisions = collisions;
                    }
                    if (1 == collisions) {
                        INCR_STAT(eth->tx_single_collisions);
                    } else {
                        INCR_STAT(eth->tx_mult_collisions);
                    }
                }
            }
            if (flags & HAL_MCFxxxx_ETHx_TXBD_LC) {
                INCR_STAT(eth->tx_late_collisions);
            }
            if (flags & HAL_MCFxxxx_ETHx_TXBD_UN) {
                INCR_STAT(eth->tx_underrun);
            }
            if (flags & HAL_MCFxxxx_ETHx_TXBD_CSL) {
                INCR_STAT(eth->tx_carrier_loss);
            }
            if (flags & HAL_MCFxxxx_ETHx_TXBD_DEF) {
                INCR_STAT(eth->tx_deferred);
            }
        }
    }
# else
    {
        // Assume MCF5282-compatible.
        // There are no error bits in the tx buffer descriptor. Instead it is
        // possible to check the interrupt status register, look for error
        // flags, and clear them.
        cyg_uint32  flags;
        HAL_READ_UINT32(EIR(eth), flags);
        flags &= (HAL_MCFxxxx_ETHx_EIR_LC | HAL_MCFxxxx_ETHx_EIR_RL | HAL_MCFxxxx_ETHx_EIR_UN);
        if (0 == flags) {
            INCR_STAT(eth->tx_good);
        } else {
            if (flags & HAL_MCFxxxx_ETHx_EIR_LC) {
                INCR_STAT(eth->tx_late_collisions);
            }
            if (flags & HAL_MCFxxxx_ETHx_EIR_RL) {
                INCR_STAT(eth->tx_total_collisions);
                INCR_STAT(eth->tx_mult_collisions);
            }
            if (flags & HAL_MCFxxxx_ETHx_EIR_UN) {
                INCR_STAT(eth->tx_underrun);
            }
            // Clear these interrupt flags
            HAL_WRITE_UINT32(EIR(eth), flags);
        }
    }
# endif
#endif
    
    eth->tx_done    = 1;
    eth->tx_index   = eth->tx_index + 1;
    if (TX_BUFFER_DESCRIPTOR_COUNT == eth->tx_index) {
        eth->tx_index = 0;
    }
    // There is no need to worry about cleaning up the buffer descriptors.
    // The hardware will have cleared the R bit, which is the only one that
    // must be cleared.
    DIAG(1, "packet sent");
    eth_drv_dsr(vector, count, data);
}

// ----------------------------------------------------------------------------

// A whole ethernet frame has been received and reported to higher-level
// code, which has allocated an mbuf. The frame is identified by the
// rx_next_buffer field of the mcfxxxx_eth structure, which gets set in
// deliver(). The data must be copied into the mbuf, and then the
// hardware needs to be told that this buffer is available again.
// The hardware stores a 4-byte checksum in the receive buffer
// which must be discarded.
static void
mcfxxxx_eth_recv(struct eth_drv_sc* sc, struct eth_drv_sg* sg_list, int sg_len)
{
    mcfxxxx_eth*    eth     = (mcfxxxx_eth*) sc->driver_private;
    int             index   = eth->rx_next_buffer;
    cyg_uint8*      buf     = eth->rxbds[index].ethbd_buffer;
    int             len     = eth->rx_len - 4;
    int             i;

    DIAGPKT(1, "packet received", buf, len);
    
    for (i = 0; (i < sg_len) && (len > 0); i++) {
        if (0 == sg_list[i].buf) {
            break;
        } else if (len <= sg_list[i].len) {
            memcpy((void*) sg_list[i].buf, buf, len);
            break;
        } else {
            memcpy((void*) sg_list[i].buf, buf, sg_list[i].len);
            buf += sg_list[i].len;
            len -= sg_list[i].len;
        }
    }

    // Once the data has been copied out of the RX buffer, clear the
    // cache lines ready for the next packet. Use an invalidate if
    // available, otherwise a flush.
#if defined(HAL_DCACHE_INVALIDATE)
    HAL_DCACHE_INVALIDATE(eth->rxbds[index].ethbd_buffer, eth->rx_len);
#elif defined(HAL_DCACHE_FLUSH)
    HAL_DCACHE_FLUSH(eth->rxbds[index].ethbd_buffer, eth->rx_len);
#endif
}

// The RX interrupt triggers whenever a whole frame has been received (or
// when an error has occurred). The hardware may be busy receiving into the
// next buffer.
//
// Because of the ring buffer several packets may arrive before the cpu gets
// around to processing them. It is possible to save a little bit of load
// by masking the rxf interrupt inside the ethernet device, then doing the
// acknowledge and an unmask inside the deliver() code.
static cyg_uint32
mcfxxxx_eth_isr_rx(cyg_vector_t vector, cyg_addrword_t data)
{
    WRITE32(EIMR(eth), HAL_MCFxxxx_ETHx_EIMR_TXF);
    return CYG_ISR_CALL_DSR;
}

static void
mcfxxxx_eth_dsr_rx(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    struct eth_drv_sc*  sc  = (struct eth_drv_sc*) data;
    mcfxxxx_eth*        eth = (mcfxxxx_eth*)(sc->driver_private);

    INCR_STAT(eth->interrupts);
    eth->rx_rdy = 1;
    eth_drv_dsr(vector, count, data);
}

// ----------------------------------------------------------------------------
// deliver() is usually called from thread context, after the DSR has woken
// up the packet handling thread. It needs to report completed transmits so
// that higher-level code can release the mbuf, and report all received
// frames.

static void
mcfxxxx_eth_deliver(struct eth_drv_sc* sc)
{
    mcfxxxx_eth*    eth = (mcfxxxx_eth*) sc->driver_private;

    DIAG(1, "entry");
    
    if (eth->tx_done) {
        DIAG(1, "tx_done");
        INCR_STAT(eth->tx_count);
        eth->tx_done        = 0;
        eth->tx_can_send    = 1;
        (*sc->funs->eth_drv->tx_done)(sc, eth->tx_key, 1);
    }
    
    if (eth->rx_rdy) {
        int bd_emptied  = 0;
        int frame_found = 0;

        DIAG(1, "rx_rdy");
        eth->rx_rdy = 0;

        // Acknowledge any packets that have been received and will be processed
        // by the loop below. There is a small possibility of a packet arriving
        // in the middle of this loop, causing an rxf interrupt to become pending
        // even though the packet will get processed by the loop. This is harmless.
        // The rxb interrupt is cleared as well because that comes for free, making
        // debugging slightly easier.
        WRITE32(EIR(eth), HAL_MCFxxxx_ETHx_EIR_RXF | HAL_MCFxxxx_ETHx_EIR_RXB);

        DIAG(1, "updated eir");
        // Loop while there are non-empty packets. This has to be done with a bit
        // of care. Packets should be processed in the right order where possible,
        // which means keeping track of the last packet received. However it is
        // necessary to check all the buffers - for some reason the hardware does
        // not always put the next frame in the expected buffer.
        //
        // This code interacts with any data cache in unpleasant ways. A buffer
        // descriptor is only 8 bytes, i.e. half a cache line, and since
        // typically the system is configured with several rx buffers some of
        // these may get updated by the hardware at any time. The only safe
        // way to handle this is to disable the data cache when accessing the
        // rx buffer descriptors and thus make sure that there are never any
        // modified cachelines for the tx buffer descriptors.
        do {
            int         current;
            int         i;

            current     = eth->rx_next_buffer;
            frame_found = 0;
            
            for (i = 0; i < CYGNUM_DEVS_ETH_MCFxxxx_RXBUFFERS; i++) {
                // Note: these absolutely must be held in registers. The package's
                // CDL imposes constraints to ensure the driver is built with
                // sufficient optimization.
                register cyg_uint16 flags;
                register cyg_uint16 len;
                register hal_mcfxxxx_eth_buffer_descriptor* rxbd = &(eth->rxbds[current]);
#if defined(HAL_DCACHE_SIZE) && (HAL_DCACHE_SIZE > 0)
                register int cache_enabled;
                HAL_DCACHE_IS_ENABLED(cache_enabled);
                if (cache_enabled) {
                    register int ints_enabled;
                    HAL_DISABLE_INTERRUPTS(ints_enabled);
                    HAL_DCACHE_DISABLE();
                    flags   = rxbd->ethbd_flags;
                    len     = rxbd->ethbd_length;
                    if (! (flags & HAL_MCFxxxx_ETHx_RXBD_E)) {
                        rxbd->ethbd_flags  = (flags & HAL_MCFxxxx_ETHx_RXBD_W) | HAL_MCFxxxx_ETHx_RXBD_E;
                        rxbd->ethbd_length = 1518;
                    }
                    HAL_DCACHE_ENABLE();
                    HAL_RESTORE_INTERRUPTS(ints_enabled);
                } else 
#endif
                {
                    flags   = rxbd->ethbd_flags;
                    len     = rxbd->ethbd_length;
                    if (! (flags & HAL_MCFxxxx_ETHx_RXBD_E)) {
                        rxbd->ethbd_flags  = (flags & HAL_MCFxxxx_ETHx_RXBD_W) | HAL_MCFxxxx_ETHx_RXBD_E;
                        rxbd->ethbd_length = 1518;
                    }
                }

                if (flags & HAL_MCFxxxx_ETHx_RXBD_E) {
                    current = (current == (CYGNUM_DEVS_ETH_MCFxxxx_RXBUFFERS - 1)) ? 0 : (current + 1);
                    continue;
                }

                // Full frames should fill one buffer. However if an over-sized frame
                // is received then the first 1520 bytes will go into one buffer, the
                // remaining into the next. The LG flag will get set only in the last
                // buffer descriptor. Just mark the first buffer as empty after the
                // if.
                if (flags & HAL_MCFxxxx_ETHx_RXBD_L) {
                    // We have the last frame in a packet.
#ifdef CYGFUN_DEVS_ETH_MCFxxxx_STATISTICS
                    INCR_STAT(eth->rx_count);
                    if (flags & HAL_MCFxxxx_ETHx_RXBD_OV) {
                        // An overrun does not invalidate the current frame
                        INCR_STAT(eth->rx_overrun_errors);
                    }
#endif

                    if (flags & (HAL_MCFxxxx_ETHx_RXBD_LG | HAL_MCFxxxx_ETHx_RXBD_NO | HAL_MCFxxxx_ETHx_RXBD_CR | HAL_MCFxxxx_ETHx_RXBD_TR)) {
                        // The packet is invalid and should be discarded.
#ifdef CYGFUN_DEVS_ETH_MCFxxxx_STATISTICS
                        if (flags & (HAL_MCFxxxx_ETHx_RXBD_LG | HAL_MCFxxxx_ETHx_RXBD_TR)) {
                            INCR_STAT(eth->rx_too_long_frames);
                        }
                        if (flags & HAL_MCFxxxx_ETHx_RXBD_NO) {
                            INCR_STAT(eth->rx_align_errors);
                        }
                        if (flags & HAL_MCFxxxx_ETHx_RXBD_CR) {
                            INCR_STAT(eth->rx_crc_errors);
                        }
#endif
                    } else {
                        // A valid packet
                        INCR_STAT(eth->rx_good);
                        eth->rx_len         = len;
                        eth->rx_next_buffer = current;
                        (*sc->funs->eth_drv->recv)(sc, len - 4);
                    }
                }

                // Move on to the next buffer descriptor.
                frame_found                 = 1;
                bd_emptied                  = 1;
                eth->rx_next_buffer         = (current == (CYGNUM_DEVS_ETH_MCFxxxx_RXBUFFERS - 1)) ? 0 : (current + 1);
                break;
            }
        } while(frame_found);

        // If at least one buffer was marked as now empty, inform the hardware.
        if (bd_emptied) {
            WRITE32(RDAR(eth), HAL_MCFxxxx_ETHx_RDAR_R_DES_ACTIVE);
        }
    }

    // And make sure rx interrupts are unmasked again.
    WRITE32(EIMR(eth), HAL_MCFxxxx_ETHx_EIMR_TXF  | HAL_MCFxxxx_ETHx_EIMR_RXF);
}

// ----------------------------------------------------------------------------

// Polling does not need any special action, just check for pending interrupts
// and act as if an interrupt had occurred.
static void
mcfxxxx_eth_poll(struct eth_drv_sc* sc)
{
    struct  mcfxxxx_eth*    eth = (struct mcfxxxx_eth*) sc->driver_private;
    cyg_uint32              eir;

    DIAG(4, "entry");
    
    READ32(EIR(eth), eir);
    
    if (eir & HAL_MCFxxxx_ETHx_EIR_TXF) {
        WRITE32(EIR(eth), HAL_MCFxxxx_ETHx_EIR_TXF | HAL_MCFxxxx_ETHx_EIR_TXB);
        eth->tx_done    = 1;
        eth->tx_index   = eth->tx_index + 1;
        if (TX_BUFFER_DESCRIPTOR_COUNT == eth->tx_index) {
            eth->tx_index = 0;
        }
    }
    if (eir & HAL_MCFxxxx_ETHx_EIR_RXF) {
        eth->rx_rdy     = 1;
    }
    if (eth->tx_done || eth->rx_rdy) {
        mcfxxxx_eth_deliver(sc);
    }
}

// intvector() is used by RedBoot/stubs to ensure ctrl-C will get through
// if ethernet is used for the debug channel.
static int
mcfxxxx_eth_int_vector(struct eth_drv_sc* sc)
{
    return CYGNUM_HAL_ISR_ERX;
}

// ----------------------------------------------------------------------------

// SET_MAC_ADDRESS is straightforward, it just requires updating two registers.
// SET_MC_ALL and SET_MC_LIST can be implemented by setting all bits in the
// hash table. That should cause the hardware to match all multicast packets.

static int
mcfxxxx_eth_ioctl(struct eth_drv_sc* sc, unsigned long key, void* data, int data_length)
{
    mcfxxxx_eth*    eth = (mcfxxxx_eth*) sc->driver_private;

    DIAG(1, "entry");
    
    switch(key) {
      case ETH_DRV_SET_MAC_ADDRESS:
        {
            cyg_uint32  reg;
            memcpy(eth->mac, data, 6);
            reg = (eth->mac[4] << 24) | (eth->mac[5] << 16);
            WRITE32(PAUR(eth), reg);
            reg = (eth->mac[0] << 24) | (eth->mac[1] << 16) | (eth->mac[2] << 8) | (eth->mac[3] << 0);
            WRITE32(PALR(eth), reg);
            return 0;
        }

      case ETH_DRV_SET_MC_ALL:
      case ETH_DRV_SET_MC_LIST:
        {
            WRITE32(GAUR(eth), 0xFFFFFFFF);
            WRITE32(GALR(eth),  0xFFFFFFFF);
            return 0;
        }
#if defined(CYGFUN_DEVS_ETH_MCFxxxx_STATISTICS) && defined(ETH_DRV_GET_IF_STATS_UD)
      case ETH_DRV_GET_IF_STATS_UD:
      case ETH_DRV_GET_IF_STATS:
        {
            struct ether_drv_stats* stats   = (struct ether_drv_stats*) data;
            cyg_uint32              tcr;
            
            strcpy(stats->description, "Freescale MCFxxxx ethernet device");
            stats->snmp_chipset[0]      = '\0';
            READ32(TCR(eth), tcr);
            if (0 == (tcr & HAL_MCFxxxx_ETHx_TCR_FDEN)) {
                stats->duplex               = 2;                // simplex
            } else {
                stats->duplex               = 3;                // full duplex
            }
            stats->operational          = eth->started ? 3 : 2;
            stats->speed                = 10 * 1000000;     // assume 10MHz. This would require querying the phy
            stats->supports_dot3        = 1;
            stats->tx_queue_len         = 1;                // fixed by the driver design
            stats->tx_dropped           = 0;                // can_send() is robust
            
            stats->interrupts           = eth->interrupts;

            stats->tx_count             = eth->tx_count;
            stats->tx_good              = eth->tx_good;
            stats->tx_max_collisions    = eth->tx_max_collisions;
            stats->tx_late_collisions   = eth->tx_late_collisions;
            stats->tx_underrun          = eth->tx_underrun;
            stats->tx_carrier_loss      = eth->tx_carrier_loss;
            stats->tx_deferred          = eth->tx_deferred;
            stats->tx_single_collisions = eth->tx_single_collisions;
            stats->tx_mult_collisions   = eth->tx_mult_collisions;
            stats->tx_total_collisions  = eth->tx_total_collisions;
            
            stats->rx_count             = eth->rx_count;
            stats->rx_good              = eth->rx_good;
            stats->rx_crc_errors        = eth->rx_crc_errors;
            stats->rx_align_errors      = eth->rx_align_errors;
            stats->rx_overrun_errors    = eth->rx_overrun_errors;
            stats->rx_short_frames      = eth->rx_short_frames;
            stats->rx_too_long_frames   = eth->rx_too_long_frames;
            
            return 0;
        }
#endif
        
      default:
        return 1;
    }
}

// ----------------------------------------------------------------------------

// Starting involves setting up initial empty buffer descriptors, then
// enabling the ethernet device. Optionally promiscuous mode needs to be
// set as well.

#if 0
static int
mcfxxxx_eth_read_phy(int addr, int reg)
{
    cyg_uint32  events;
    int         ints_state;
    int         i;
    int         result  = -1;
    
    HAL_DISABLE_INTERRUPTS(ints_state);
    WRITE32(MMFR(eth),
                     HAL_MCFxxxx_ETHx_MMFR_ST_VALUE |
                     HAL_MCFxxxx_ETHx_MMFR_OP_READ |
                     (addr << HAL_MCFxxxx_ETHx_MMFR_PA_SHIFT) |
                     (reg << HAL_MCFxxxx_ETHx_MMFR_RA_SHIFT) |
                     HAL_MCFxxxx_ETHx_MMFR_TA_VALUE |
                     (0x00 << HAL_MCFxxxx_ETHx_MMFR_DATA_SHIFT));

    // When the phy has responded the MII interrupt bit should be set.
    for (i = 0; i < 10; i++) {
        // 16 bits * both ways @ 2.5MHz, should just take a few microseconds.
        // This happens in a loop, just in case.
        HAL_DELAY_US(10);
        READ32(EIR(eth), events);
        if (events & HAL_MCFxxxx_ETHx_EIR_MII) {
            WRITE32(EIR(eth), HAL_MCFxxxx_ETHx_EIR_MII);
            READ32(MMFR(eth), result);
            result  &= 0x0000FFFF;
            break;
        }
    }
    
    HAL_RESTORE_INTERRUPTS(ints_state);
    return result;
}
#endif

static void
mcfxxxx_eth_start(struct eth_drv_sc* sc, unsigned char* enaddr, int flags)
{
    mcfxxxx_eth*    eth = (mcfxxxx_eth*) sc->driver_private;
    cyg_uint32      rcr;
    int             i;

    if (eth->started) {
        return;
    }

    DIAG(1, "entry");
    
    eth->tx_can_send    = 1;
    eth->tx_done        = 0;
    eth->rx_rdy         = 0;

    // Clear all tx buffer descriptors, setting the WRAP bit on the last one.
    for (i = 0; i < TX_BUFFER_DESCRIPTOR_COUNT; i++) {
        eth->txbds[i].ethbd_flags     = 0;
    }
    eth->txbds[TX_BUFFER_DESCRIPTOR_COUNT - 1].ethbd_flags    = HAL_MCFxxxx_ETHx_TXBD_W;
    eth->tx_index = 0;

    // Set all rx buffer descriptors to empty.
    for (i = 0; i < CYGNUM_DEVS_ETH_MCFxxxx_RXBUFFERS; i++) {
        eth->rxbds[i].ethbd_flags   = HAL_MCFxxxx_ETHx_RXBD_E;
        eth->rxbds[i].ethbd_length  = 1518;
    }
    eth->rxbds[CYGNUM_DEVS_ETH_MCFxxxx_RXBUFFERS - 1].ethbd_flags = HAL_MCFxxxx_ETHx_RXBD_E | HAL_MCFxxxx_ETHx_RXBD_W;

    // None of the rx buffers should be in the cache so no need to invalidate.
    // The rxbds must be written to memory. The txbds will be taken care of
    // by the transmit code when there are packets to transmit.
#ifdef HAL_DCACHE_STORE
    HAL_DCACHE_STORE(&(eth->rxbds[0]), CYGNUM_DEVS_ETH_MCFxxxx_RXBUFFERS * sizeof(hal_mcfxxxx_eth_buffer_descriptor));
#endif
    
#if 0
    // Interact with the phy to see if the RCR DRT and X_CTRNL FDEN
    // bits should be set. Default to half-duplex.
    // NOTE: this should be conditional on the presence of a phy, and the
    // platform should be able to override the default phy settings. It
    // is also necessary to search the phy address space.
    {
        int     addr    = -1;
        int     i;
        int     reg;

        // Find the phy's address. If the phy is not present the id register
        // will read as 0x00FFFF.
        for (i = 0; i < 32; i++) {
            reg = mcfxxxx_eth_read_phy(i, 2);
            if ((-1 != reg) && (0x0000FFFF != reg)) {
                addr = i;
                break;
            }
        }
        
        if (-1 != addr) {
            // Read the status register, especially the link bit
            reg = mcfxxxx_eth_read_phy(addr, 1);
            if (reg & 0x04) {
                // The link is up. What was negotiated with the partner?
                reg = mcfxxxx_eth_read_phy(addr, 5);
                if (reg & 0x0140) {
                    // Full duplex, either 100 or 10. Clear the RCR DRT bit, set
                    // the TCR FDEN bit
                    cyg_uint32  rcr;
                    READ32(RCR(eth), rcr);
                    WRITE32(RCR(eth), rcr & ~ HAL_MCFxxxx_ETHx_RCR_DRT);
                    WRITE32(TCR(eth), HAL_MCFxxxx_ETHx_TCR_FDEN);
                }
            }
        }
    }
#endif

    // The RCR register can have additional bits set on some processors,
    // e.g. on the mcf5282 it contains the maximum frame length. Change it only
    // by read/modify/write.
    READ32(RCR(eth), rcr);
#ifdef CYGPKG_NET    
    if (flags & IFF_PROMISC) {
        rcr |= HAL_MCFxxxx_ETHx_RCR_PROM;
    } else {
        rcr &= ~HAL_MCFxxxx_ETHx_RCR_PROM;
    }
#else
    rcr &= ~HAL_MCFxxxx_ETHx_RCR_PROM;
#endif
    WRITE32(RCR(eth), rcr);

    // The hardware is now ready
    // The documentation says that setting R_DES_ACTIVE could also be done before asserting
    // ETHER_EN, but that appears incorrect: you'll never get any packets.
    WRITE32(ECR(eth),  HAL_MCFxxxx_ETHx_ECR_ETHER_EN);
    WRITE32(RDAR(eth), HAL_MCFxxxx_ETHx_RDAR_R_DES_ACTIVE);
    WRITE32(TDAR(eth), HAL_MCFxxxx_ETHx_TDAR_X_DES_ACTIVE);
    
    eth->started = 1;

    DIAG(1, "done");
}

// Stopping can be done simply by disabling the device, which causes
// all transmits and receives to be aborted. There should be no further
// interrupts.
static void
mcfxxxx_eth_stop(struct eth_drv_sc* sc)
{
    mcfxxxx_eth*    eth = (mcfxxxx_eth*) sc->driver_private;

    DIAG(1, "entry");
    
    eth->started    = 0;
    WRITE32(ECR(eth), 0);
    eth->tx_done    = 0;
    eth->rx_rdy     = 0;
    if (! eth->tx_can_send) {
        (*sc->funs->eth_drv->tx_done)(sc, eth->tx_key, 1);
        eth->tx_can_send    = 1;
    }

    DIAG(1, "done");
}
