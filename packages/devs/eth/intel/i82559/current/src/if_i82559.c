//==========================================================================
//
//      if_i82559.c
//
//	Intel 82559 ethernet driver
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//####BSDCOPYRIGHTBEGIN####
//
// -------------------------------------------
//
// Portions of this software may have been derived from OpenBSD or
// other sources, and are covered by the appropriate copyright
// disclaimers included herein.
//
// -------------------------------------------
//
//####BSDCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    hmt, gthomas
// Contributors: Ron Spence, Pacific Softworks, jskov
// Date:         2000-02-01
// Purpose:      
// Description:  hardware driver for 82559 Intel PRO/100+ ethernet
// Notes:        CU commands such as dump and config should, according
//               to the docs, set the CU active state while executing.
//               That does not seem to be the case though, and the
//               driver polls the completion bit in the packet status
//               word instead.
//
//               Platform code must provide this vector:
//               CYGNUM_DEVS_ETH_INTEL_I82559_MUX_INTERRUPT if it
//               requires the interrupts to be handled via demuxers.
//
//               Platform code can also provide this macro:
//               CYGPRI_DEVS_ETH_INTEL_I82559_INTERRUPT_ACK_LOOP(p_i82559)
//               to handle delaying for acks to register on the interrupt
//               controller as necessary on the EBSA.
//
//        FIXME: IN/OUT macros for accessing PCI IO space are probably
//               broken on proper BE systems. Tested on a board with
//               a Galileo which seems to do some weird stuff to 8/16
//               bit word addressing.
//        FIXME: replace -1/-2 return values with proper E-defines
//        FIXME: eth_set_mac_address and eth_set_promiscuous_mode
//               failures are not handled properly [try writing
//               endian swapped cmd - CU starts, but wedges]
//        FIXME: For 82557/8 compatibility the eth_set_config and
//               eth_set_promiscuous_mode functions probably need
//               some tweaking - config bits differ slightly but
//               crucially.
//        FIXME: EEPROM code not tested on a BE system.
//        FIXME: Apparently promiscuous mode cannot be disabled (on
//               BE systems?)
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/devs_eth_intel_i82559.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/drv_api.h>
#include <netdev.h>
#include <eth_drv.h>

#ifdef CYGPKG_NET
#include <pkgconf/net.h>
#include <net/if.h>  /* Needed for struct ifnet */
#else
#include <cyg/hal/hal_if.h>
#define diag_printf printf
#endif

#ifdef CYGPKG_IO_PCI
#include <cyg/io/pci.h>
// So we can check the validity of the PCI window against the MLTs opinion,
// and thereby what the malloc heap consumes willy-nilly:
#include CYGHWR_MEMORY_LAYOUT_H
#else
#error "Need PCI package here"
#endif

// Exported statistics and the like
#include <cyg/devs/eth/i82559_info.h>
#include <eth_drv_stats.h>
#include CYGDAT_DEVS_ETH_INTEL_I82559_INL

// ------------------------------------------------------------------------
// Platform specific

#define CYGHWR_INTEL_I82559_PCI_MEM_MAP_BASE (CYGARC_UNCACHED_ADDRESS(0x0ff00000))
#define CYGHWR_INTEL_I82559_PCI_MEM_MAP_SIZE 0x00100000

// ------------------------------------------------------------------------

#ifdef CYGDBG_DEVS_ETH_INTEL_I82559_CHATTER
#define notDEBUG_82559 // This one prints stuff as packets come and go
#define DEBUG          // Startup printing mainly
#define DEBUG_EE       // Some EEPROM specific retries &c
#endif

#define os_printf diag_printf
#define db_printf diag_printf

// ------------------------------------------------------------------------
// I/O access macros as inlines for type safety

#if (CYG_BYTEORDER == CYG_MSBFIRST)

#define HAL_CTOLE32(x)  ((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | (((x) >> 24) & 0xff))
#define HAL_LE32TOC(x)  ((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | (((x) >> 24) & 0xff))

#define HAL_CTOLE16(x)  ((((x) & 0xff) << 8) | (((x) & 0xff00) >> 8))
#define HAL_LE16TOC(x)  ((((x) & 0xff) << 8) | (((x) & 0xff00) >> 8))

static inline void OUTB(cyg_uint8 value, cyg_uint32 io_address)
{   *((volatile cyg_uint8 *)(io_address)) = value;    }

static inline void OUTW(cyg_uint16 value, cyg_uint32 io_address)
{   *((volatile cyg_uint16 *)(io_address)) = (((value & 0xff) << 8) | ((value & 0xff00) >> 8));   }

static inline void OUTL(cyg_uint32 value, cyg_uint32 io_address)
{   *((volatile cyg_uint32 *)io_address) = ((((value) & 0xff) << 24) | (((value) & 0xff00) << 8) | (((value) & 0xff0000) >> 8) | (((value) >> 24) & 0xff));   }

static inline cyg_uint8 INB(cyg_uint32 io_address)
{   return *((volatile cyg_uint8 *)(io_address));     }

static inline cyg_uint16 INW(cyg_uint32 io_address)
{   cyg_uint16 d;
    d = *((volatile cyg_uint16 *)(io_address));
    return (((d & 0xff) << 8) | ((d & 0xff00) >> 8));
}

static inline cyg_uint32 INL(cyg_uint32 io_address)
{   cyg_uint32 d;
    d = *((volatile cyg_uint32 *)io_address); 
    return ((((d) & 0xff) << 24) | (((d) & 0xff00) << 8) | (((d) & 0xff0000) >> 8) | (((d) >> 24) & 0xff));
}
#else
static inline void OUTB(cyg_uint8 value, cyg_uint32 io_address)
{   *((volatile cyg_uint8 *)io_address) = value;    }

static inline void OUTW(cyg_uint16 value, cyg_uint32 io_address)
{   *((volatile cyg_uint16 *)io_address) = value;   }

static inline void OUTL(cyg_uint32 value, cyg_uint32 io_address)
{   *((volatile cyg_uint32 *)io_address) = value;   }

static inline cyg_uint8 INB(cyg_uint32 io_address)
{   return *((volatile cyg_uint8 *)io_address);     }

static inline cyg_uint16 INW(cyg_uint32 io_address)
{   return *((volatile cyg_uint16 *)io_address);    }

static inline cyg_uint32 INL(cyg_uint32 io_address)
{   return *((volatile cyg_uint32 *)io_address);    }
#endif // byteorder

#define VIRT_TO_BUS( _x_ ) virt_to_bus((cyg_uint32)(_x_))
static inline cyg_uint32 virt_to_bus(cyg_uint32 p_memory)
{ return CYGARC_PHYSICAL_ADDRESS(p_memory);    }

#define BUS_TO_VIRT( _x_ ) bus_to_virt((cyg_uint32)(_x_))
static inline cyg_uint32 bus_to_virt(cyg_uint32 p_memory)
{ return CYGARC_UNCACHED_ADDRESS(p_memory); }


// ------------------------------------------------------------------------
//                                                                      
//                   82559 REGISTER OFFSETS (I/O SPACE)                 
//                                                                      
// ------------------------------------------------------------------------
#define SCBStatus       0               // Rx/Command Unit command and status.
#define SCBCmd          2               // Rx/Command Unit command and status.
#define SCBPointer      4               // General purpose pointer.
#define SCBPort         8               // Misc. commands and operands.
#define SCBflash        12              // Flash memory control.
#define SCBeeprom       14              // EEPROM memory control.
#define SCBCtrlMDI      16              // MDI interface control.
#define SCBEarlyRx      20              // Early receive byte count.
#define SCBGenControl   28              // 82559 General Control Register
#define SCBGenStatus    29              // 82559 General Status register


// ------------------------------------------------------------------------
//
//               82559 SCB STATUS WORD DEFNITIONS
//
// ------------------------------------------------------------------------
#define SCB_STATUS_CX   0x8000          // CU finished command (transmit)
#define SCB_STATUS_FR   0x4000          // frame received
#define SCB_STATUS_CNA  0x2000          // CU left active state
#define SCB_STATUS_RNR  0x1000          // receiver left ready state
#define SCB_STATUS_MDI  0x0800          // MDI read/write cycle done
#define SCB_STATUS_SWI  0x0400          // software generated interrupt
#define SCB_STATUS_FCP  0x0100          // flow control pause interrupt

#define SCB_INTACK_MASK 0xFD00          // all the above

#define SCB_INTACK_TX (SCB_STATUS_CX | SCB_STATUS_CNA)
#define SCB_INTACK_RX (SCB_STATUS_FR | SCB_STATUS_RNR)

// ------------------------------------------------------------------------
//
//               82559 PORT INTERFACE COMMANDS
//
// ------------------------------------------------------------------------
#define I82559_RESET            0x00000000 // software reset
#define I82559_SELFTEST         0x00000001 // 82559 selftest command
#define I82559_SELECTIVE_RESET  0x00000002
#define I82559_DUMP             0x00000003
#define I82559_DUMP_WAKEUP      0x00000007


// ------------------------------------------------------------------------
//
//                   82559 EEPROM INTERFACE
//
// ------------------------------------------------------------------------
//  EEPROM_Ctrl bits.
#define EE_SHIFT_CLK	0x01            // EEPROM shift clock.
#define EE_CS		0x02            // EEPROM chip select.
#define EE_DATA_WRITE	0x04            // EEPROM chip data in.
#define EE_DATA_READ	0x08            // EEPROM chip data out.
#define EE_ENB		(0x4800 | EE_CS)

// Delay between EEPROM clock transitions.
#define eeprom_delay(usec)		udelay(usec);

// The EEPROM commands include the always-set leading bit.
#define EE_WRITE_CMD(a)     (5 << (a))
#define EE_READ_CMD(a)	    (6 << (a))
#define EE_ERASE_CMD(a)	    (7 << (a))
#define EE_WRITE_EN_CMD(a)  (19 << ((a)-2))
#define EE_WRITE_DIS_CMD(a) (16 << ((a)-2))
#define EE_ERASE_ALL_CMD(a) (18 << ((a)-2))

#define EE_TOP_CMD_BIT(a)      ((a)+2) // Counts down to zero
#define EE_TOP_DATA_BIT        (15)    // Counts down to zero

#define EEPROM_ENABLE_DELAY (10) // Delay at chip select

#define EEPROM_SK_DELAY  (4) // Delay between clock edges *and* data
                             // read or transition; 3 of these per bit.
#define EEPROM_DONE_DELAY (100) // Delay when all done


// ------------------------------------------------------------------------
//
//               SYSTEM CONTROL BLOCK COMMANDS
//
// ------------------------------------------------------------------------
// CU COMMANDS
#define CU_NOP          0x0000
#define	CU_START        0x0010
#define	CU_RESUME       0x0020
#define	CU_STATSADDR    0x0040          // Load Dump Statistics ctrs addr
#define	CU_SHOWSTATS    0x0050          // Dump statistics counters.
#define	CU_ADDR_LOAD    0x0060          // Base address to add to CU commands
#define	CU_DUMPSTATS    0x0070          // Dump then reset stats counters.

// RUC COMMANDS
#define RUC_NOP         0x0000
#define	RUC_START       0x0001
#define	RUC_RESUME      0x0002
#define RUC_ABORT       0x0004
#define	RUC_ADDR_LOAD   0x0006          // (seems not to clear on acceptance)
#define RUC_RESUMENR    0x0007

#define CU_CMD_MASK     0x00f0
#define RU_CMD_MASK     0x0007

#define SCB_M	        0x0100          // 0 = enable interrupt, 1 = disable
#define SCB_SI          0x0200          // 1 - cause device to interrupt

#define CU_STATUS_MASK  0x00C0
#define RU_STATUS_MASK  0x003C

#define RU_STATUS_IDLE  (0<<2)
#define RU_STATUS_SUS   (1<<2)
#define RU_STATUS_NORES (2<<2)
#define RU_STATUS_READY (4<<2)
#define RU_STATUS_NO_RBDS_SUS   ((1<<2)|(8<<2))
#define RU_STATUS_NO_RBDS_NORES ((2<<2)|(8<<2))
#define RU_STATUS_NO_RBDS_READY ((4<<2)|(8<<2))

#define MAX_MEM_RESERVED_IOCTL 1000

// ------------------------------------------------------------------------
//
//               RECEIVE FRAME DESCRIPTORS
//
// ------------------------------------------------------------------------

// The status is split into two shorts to get atomic access to the EL bit;
// the upper word is not written by the device, so we can just hit it,
// leaving the lower word (which the device updates) alone.  Otherwise
// there's a race condition between software moving the end-of-list (EL)
// bit round and the device writing into the previous slot.

#if (CYG_BYTEORDER == CYG_MSBFIRST)

#define RFD_STATUS       0
#define RFD_STATUS_HI    2              // swapped
#define RFD_STATUS_LO    0              // swapped
#define RFD_LINK         4
#define RFD_RDB_ADDR     8
#define RFD_SIZE        14              // swapped
#define RFD_COUNT       12              // swapped
#define RFD_BUFFER      16
#define RFD_SIZEOF      16

// Note that status bits are endian converted - so we don't need to
// fiddle the byteorder when accessing the status word!
#define RFD_STATUS_EL   0x00000080      // 1=last RFD in RFA
#define RFD_STATUS_S    0x00000040      // 1=suspend RU after receiving frame
#define RFD_STATUS_H    0x00001000      // 1=RFD is a header RFD
#define RFD_STATUS_SF   0x00000800      // 0=simplified, 1=flexible mode
#define RFD_STATUS_C    0x00800000      // completion of received frame
#define RFD_STATUS_OK   0x00200000      // frame received with no errors

#define RFD_STATUS_HI_EL   0x0080       // 1=last RFD in RFA
#define RFD_STATUS_HI_S    0x0040       // 1=suspend RU after receiving frame
#define RFD_STATUS_HI_H    0x1000       // 1=RFD is a header RFD
#define RFD_STATUS_HI_SF   0x0800       // 0=simplified, 1=flexible mode

#define RFD_STATUS_LO_C    0x0080       // completion of received frame
#define RFD_STATUS_LO_OK   0x0020       // frame received with no errors


#define RFD_COUNT_MASK     0x3fff
#define RFD_COUNT_F        0x4000
#define RFD_COUNT_EOF      0x8000

#define RFD_RX_CRC          0x00080000  // crc error
#define RFD_RX_ALIGNMENT    0x00040000  // alignment error
#define RFD_RX_RESOURCE     0x00020000  // out of space, no resources
#define RFD_RX_DMA_OVER     0x00010000  // DMA overrun
#define RFD_RX_SHORT        0x80000000  // short frame error
#define RFD_RX_LENGTH       0x20000000  //
#define RFD_RX_ERROR        0x10000000  // receive error
#define RFD_RX_NO_ADR_MATCH 0x04000000  // no address match
#define RFD_RX_IA_MATCH     0x02000000  // individual address does not match
#define RFD_RX_TCO          0x01000000  // TCO indication

#else // Little-endian

#define RFD_STATUS       0
#define RFD_STATUS_HI    0
#define RFD_STATUS_LO    2
#define RFD_LINK         4
#define RFD_RDB_ADDR     8
#define RFD_SIZE        12
#define RFD_COUNT       14
#define RFD_BUFFER      16

#define RFD_STATUS_EL   0x80000000      // 1=last RFD in RFA
#define RFD_STATUS_S    0x40000000      // 1=suspend RU after receiving frame
#define RFD_STATUS_H    0x00100000      // 1=RFD is a header RFD
#define RFD_STATUS_SF   0x00080000      // 0=simplified, 1=flexible mode
#define RFD_STATUS_C    0x00008000      // completion of received frame
#define RFD_STATUS_OK   0x00002000      // frame received with no errors

#define RFD_STATUS_HI_EL   0x8000       // 1=last RFD in RFA
#define RFD_STATUS_HI_S    0x4000       // 1=suspend RU after receiving frame
#define RFD_STATUS_HI_H    0x0010       // 1=RFD is a header RFD
#define RFD_STATUS_HI_SF   0x0008       // 0=simplified, 1=flexible mode

#define RFD_STATUS_LO_C    0x8000       // completion of received frame
#define RFD_STATUS_LO_OK   0x2000       // frame received with no errors

#define RFD_COUNT_MASK     0x3fff
#define RFD_COUNT_F        0x4000
#define RFD_COUNT_EOF      0x8000

#define RFD_RX_CRC          0x00000800  // crc error
#define RFD_RX_ALIGNMENT    0x00000400  // alignment error
#define RFD_RX_RESOURCE     0x00000200  // out of space, no resources
#define RFD_RX_DMA_OVER     0x00000100  // DMA overrun
#define RFD_RX_SHORT        0x00000080  // short frame error
#define RFD_RX_LENGTH       0x00000020  //
#define RFD_RX_ERROR        0x00000010  // receive error
#define RFD_RX_NO_ADR_MATCH 0x00000004  // no address match
#define RFD_RX_IA_MATCH     0x00000002  // individual address does not match
#define RFD_RX_TCO          0x00000001  // TCO indication

#endif // CYG_BYTEORDER

// ------------------------------------------------------------------------
//
//               TRANSMIT FRAME DESCRIPTORS
//
// ------------------------------------------------------------------------

#if (CYG_BYTEORDER == CYG_MSBFIRST)

#define TxCB_CMD             2          // swapped
#define TxCB_STATUS          0          // swapped
#define TxCB_LINK            4
#define TxCB_TBD_ADDR        8
#define TxCB_TX_THRESHOLD   14          // swapped
#define TxCB_TBD_NUMBER     15          // swapped
#define TxCB_COUNT          12          // swapped
#define TxCB_BUFFER         16
#define TxCB_SIZEOF         16

// Note that CMD/STATUS bits are endian converted - so we don't need
// to fiddle the byteorder when accessing the CMD/STATUS word!

#define TxCB_CMD_TRANSMIT   0x0400      // transmit command
#define TxCB_CMD_SF         0x0800      // 0=simplified, 1=flexible mode
#define TxCB_CMD_NC         0x0010      // 0=CRC insert by controller
#define TxCB_CMD_I          0x0020      // generate interrupt on completion
#define TxCB_CMD_S          0x0040      // suspend on completion
#define TxCB_CMD_EL         0x0080      // last command block in CBL

#define TxCB_COUNT_MASK     0x3fff
#define TxCB_COUNT_EOF      0x8000

#else // Little-endian layout

#define TxCB_CMD             0
#define TxCB_STATUS          2
#define TxCB_LINK            4
#define TxCB_TBD_ADDR        8
#define TxCB_TBD_NUMBER     12
#define TxCB_TX_THRESHOLD   13
#define TxCB_COUNT          14
#define TxCB_BUFFER         16

#define TxCB_COUNT_MASK     0x3fff
#define TxCB_COUNT_EOF      0x8000

#define TxCB_CMD_TRANSMIT   0x0004      // transmit command
#define TxCB_CMD_SF         0x0008      // 0=simplified, 1=flexible mode
#define TxCB_CMD_NC         0x0010      // 0=CRC insert by controller
#define TxCB_CMD_I          0x2000      // generate interrupt on completion
#define TxCB_CMD_S          0x4000      // suspend on completion
#define TxCB_CMD_EL         0x8000      // last command block in CBL

#endif // CYG_BYTEORDER

// ------------------------------------------------------------------------
//
//                   STRUCTURES ADDED FOR PROMISCUOUS MODE
//
// ------------------------------------------------------------------------

#if (CYG_BYTEORDER == CYG_MSBFIRST)

#define CFG_STATUS          0
#define CFG_CMD             2
#define CFG_CB_LINK_OFFSET  4
#define CFG_BYTES           8
#define CFG_SIZEOF          32

// Note CFG CMD and STATUS swapped, so no need for endian conversion
// in code.
#define CFG_CMD_EL         0x0080
#define CFG_CMD_SUSPEND    0x0040
#define CFG_CMD_INT        0x0020
#define CFG_CMD_IAS        0x0100       // individual address setup
#define CFG_CMD_CONFIGURE  0x0200       // configure

#define CFG_STATUS_C       0x0080
#define CFG_STATUS_OK      0x0020

#else // Little-endian 

#define CFG_CMD             0
#define CFG_STATUS          2
#define CFG_CB_LINK_OFFSET  4
#define CFG_BYTES           8
#define CFG_SIZEOF          32

#define CFG_CMD_EL         0x8000
#define CFG_CMD_SUSPEND    0x4000
#define CFG_CMD_INT        0x2000
#define CFG_CMD_IAS        0x0001       // individual address setup
#define CFG_CMD_CONFIGURE  0x0002       // configure

#define CFG_STATUS_C       0x8000
#define CFG_STATUS_OK      0x2000

#endif  // CYG_BYTEORDER


// ------------------------------------------------------------------------
//
//                       STATISTICAL COUNTER STRUCTURE
//
// ------------------------------------------------------------------------
#ifdef KEEP_STATISTICS
STATISTICS statistics[CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT];
I82559_COUNTERS i82559_counters[CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT];
#endif // KEEP_STATISTICS

// ------------------------------------------------------------------------
//
//                      DEVICES AND PACKET QUEUES
//
// ------------------------------------------------------------------------

#define MAX_RX_PACKET_SIZE  1536        // maximum Rx packet size
#define MAX_TX_PACKET_SIZE  1536        // maximum Tx packet size


// ------------------------------------------------------------------------
// Use arrays provided by platform header to verify pointers.
#ifdef CYGDBG_USE_ASSERTS
#define CHECK_NDP_SC_LINK()                                             \
    CYG_MACRO_START                                                     \
    int i, valid_netdev = 0, valid_sc = 0;                              \
    for(i = 0; i < CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT; i++) {       \
        if (i82559_netdev_array[i] == ndp) valid_netdev = 1;            \
        if (i82559_sc_array[i] == sc) valid_sc = 1;                     \
        if (valid_sc || valid_netdev) break;                            \
    }                                                                   \
    CYG_ASSERT( valid_netdev, "Bad ndp" );                              \
    CYG_ASSERT( valid_sc, "Bad sc" );                                   \
    CYG_ASSERT( (void *)p_i82559 == i82559_sc_array[i]->driver_private, \
                "sc pointer bad" );                                     \
    CYG_MACRO_END
#else
#define CHECK_NDP_SC_LINK()
#endif

#define IF_BAD_82559( _p_ )                                             \
if (({                                                                  \
    int i, valid_p = 0;                                                 \
    for(i = 0; i < CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT; i++) {       \
        if (i82559_priv_array[i] == (_p_)) {                            \
            valid_p = 1;                                                \
            break;                                                      \
        }                                                               \
    }                                                                   \
    CYG_ASSERT(valid_p, "Bad pointer-to-i82559");                       \
    (!valid_p);                                                         \
}))

// ------------------------------------------------------------------------
//
// Managing the memory that is windowed onto the PCI bus
//
// ------------------------------------------------------------------------

static cyg_uint32 i82559_heap_size;
static cyg_uint8 *i82559_heap_base;
static cyg_uint8 *i82559_heap_free;

static void *mem_reserved_ioctl = (void*)0;
// uncacheable memory reserved for ioctl calls

// ------------------------------------------------------------------------
//
//                       FUNCTION PROTOTYPES
//
// ------------------------------------------------------------------------

static int pci_init_find_82559s(void);

static void i82559_reset(struct i82559* p_i82559);
static int eth_set_mac_address(struct i82559* p_i82559, char *addr);

static void InitRxRing(struct i82559* p_i82559);
static void ResetRxRing(struct i82559* p_i82559);
static void InitTxRing(struct i82559* p_i82559);
static void ResetTxRing(struct i82559* p_i82559);

static int eth_set_config(struct i82559* p_i82559);

#ifdef CYGPKG_DEVS_ETH_INTEL_I82559_WRITE_EEPROM
static void program_eeprom(cyg_uint32 , cyg_uint32 , cyg_uint8 * );
#endif
#ifdef CYGPKG_NET
static int eth_set_promiscuous_mode(struct i82559* p_i82559);
#endif

// debugging/logging only:
void dump_txcb(TxCB* p_txcb);
void DisplayStatistics(void);
void update_statistics(struct i82559* p_i82559);
void dump_rfd(RFD* p_rfd, int anyway );
void dump_all_rfds( int intf );
void dump_packet(cyg_uint8 *p_buffer, int length);

// ------------------------------------------------------------------------
// utilities
// ------------------------------------------------------------------------

typedef enum {
    WAIT_RU,                            // wait before RU cmd
    WAIT_CU                             // wait before CU cmd
} cmd_wait_t;

static // inline
void 
wait_for_cmd_done(long scb_ioaddr, cmd_wait_t type)
{
    register int status;
    register int wait;

#ifdef nDEBUG_82559
    os_printf(">%s %04x\n", __FUNCTION__, INW(scb_ioaddr + SCBCmd));
#endif
    wait = 0x100000;
    do status = INW(scb_ioaddr + SCBCmd) ;
    while( (0 != ((CU_CMD_MASK | RU_CMD_MASK) & status)) && --wait >= 0);
#ifdef nDEBUG_82559
    os_printf(">%s %04x\n", __FUNCTION__, INW(scb_ioaddr + SCBCmd));
#endif
    CYG_ASSERT( wait > 0, "wait_for_cmd_done: cmd busy" );

    if (WAIT_CU == type) {
        // Also check CU is idle
#ifdef nDEBUG_82559
      os_printf(">%s %04x\n", __FUNCTION__, INW(scb_ioaddr + SCBStatus));
#endif
        wait = 0x100000;
        do status = INW(scb_ioaddr + SCBStatus) ;
        while( (status & CU_STATUS_MASK) && --wait >= 0);
#ifdef nDEBUG_82559
        os_printf("<%s %04x\n", __FUNCTION__, INW(scb_ioaddr + SCBStatus));
#endif
        CYG_ASSERT( wait > 0, "wait_for_cmd_done: CU busy" );
    } else if (WAIT_RU == type) {
        // Can't see any active state in the doc to check for 
    }
}

// Short circuit the drv_interrupt_XXX API once we are started:

static inline int 
Mask82559Interrupt(struct i82559* p_i82559)
{
    int old = 0;

//    if (query_enabled)
    old |= 1;
    cyg_drv_interrupt_mask(p_i82559->vector);
#ifdef CYGNUM_DEVS_ETH_INTEL_I82559_MUX_INTERRUPT
//    if (query_enabled)
    old |= 2;
    cyg_drv_interrupt_mask(CYGNUM_DEVS_ETH_INTEL_I82559_MUX_INTERRUPT);
#endif

    /* it may prove necessary to do something like this
    if ( mybits != (mybits & old) )
        /# then at least one of them was disabled, so
         # omit both from any restoration: #/
        old = 0;
    */

    return old;
}

static inline void
UnMask82559Interrupt(struct i82559* p_i82559, int old)
{
    // We must only unmask (enable) those which were unmasked before,
    // according to the bits in old.
    if (old & 1)
        cyg_drv_interrupt_unmask(p_i82559->vector);
#ifdef CYGNUM_DEVS_ETH_INTEL_I82559_MUX_INTERRUPT
    if (old & 2)
        cyg_drv_interrupt_mask(CYGNUM_DEVS_ETH_INTEL_I82559_MUX_INTERRUPT);
#endif
}

static void
Acknowledge82559Interrupt(struct i82559* p_i82559)
{
    cyg_drv_interrupt_acknowledge(p_i82559->vector);
#ifdef CYGNUM_DEVS_ETH_INTEL_I82559_MUX_INTERRUPT
    cyg_drv_interrupt_acknowledge(CYGNUM_DEVS_ETH_INTEL_I82559_MUX_INTERRUPT);
#endif

#ifdef CYGPRI_DEVS_ETH_INTEL_I82559_INTERRUPT_ACK_LOOP
    CYGPRI_DEVS_ETH_INTEL_I82559_INTERRUPT_ACK_LOOP(p_i82559);
#endif
}


externC void hal_delay_us(int);
static void
udelay(int delay)
{
    hal_delay_us(delay);
}

// ------------------------------------------------------------------------
// Memory management
//
// Simply carve off from the front of the PCI mapped window into real memory

static void*
pciwindow_mem_alloc(int size)
{
    void *p_memory;
    int _size = size;

    CYG_ASSERT(
        (CYGHWR_INTEL_I82559_PCI_MEM_MAP_BASE <= (int)i82559_heap_free)
        &&
        ((CYGHWR_INTEL_I82559_PCI_MEM_MAP_BASE + 
          CYGHWR_INTEL_I82559_PCI_MEM_MAP_SIZE) > (int)i82559_heap_free)
        &&
        (0 < i82559_heap_size)
        &&
        (CYGHWR_INTEL_I82559_PCI_MEM_MAP_SIZE >= i82559_heap_size)
        &&
        (CYGHWR_INTEL_I82559_PCI_MEM_MAP_BASE == (int)i82559_heap_base),
        "Heap variables corrupted" );

    p_memory = (void *)0;
    size = (size + 3) & ~3;
    if ( (i82559_heap_free+size) < (i82559_heap_base+i82559_heap_size) ) {
        cyg_uint32 *p;
        p_memory = (void *)i82559_heap_free;
        i82559_heap_free += size;
        for ( p = (cyg_uint32 *)p_memory; _size > 0; _size -= 4 )
            *p++ = 0;
    }

    return p_memory;
}


// ------------------------------------------------------------------------
//
//                       GET EEPROM SIZE
//
// ------------------------------------------------------------------------
static int
get_eeprom_size(long ioaddr)
{
    unsigned short retval = 0;
    int ee_addr = ioaddr + SCBeeprom;
    int i, addrbits;

    // Should already be not-selected, but anyway:
    OUTW(EE_ENB & ~EE_CS, ee_addr);
    eeprom_delay(EEPROM_ENABLE_DELAY);
    OUTW(EE_ENB, ee_addr);
    eeprom_delay(EEPROM_ENABLE_DELAY);
    
    // Shift the read command bits out.
    for (i = 2; i >= 0; i--) {
        short dataval = (6 & (1 << i)) ? EE_DATA_WRITE : 0;
        OUTW(EE_ENB | dataval               , ee_addr);
        eeprom_delay(EEPROM_SK_DELAY);
        OUTW(EE_ENB | dataval | EE_SHIFT_CLK, ee_addr);
        eeprom_delay(EEPROM_SK_DELAY);
        OUTW(EE_ENB | dataval               , ee_addr);
        eeprom_delay(EEPROM_SK_DELAY);
    }
    // Now clock out address zero, looking for the dummy 0 data bit
    for ( i = 1; i <= 12; i++ ) {
        OUTW(EE_ENB               , ee_addr);
        eeprom_delay(EEPROM_SK_DELAY);
        OUTW(EE_ENB | EE_SHIFT_CLK, ee_addr);
        eeprom_delay(EEPROM_SK_DELAY);
        OUTW(EE_ENB               , ee_addr);
        eeprom_delay(EEPROM_SK_DELAY);
        retval = INW(ee_addr) & EE_DATA_READ;
        if ( 0 == retval )
            break; // The dummy zero est arrive'
    }

#ifdef DEBUG_EE
    os_printf( "eeprom data bits %d (ioaddr %x)\n", i, ee_addr );
#endif
    
    if ( 6 != i && 8 != i && 1 != i) {
#ifdef DEBUG_EE
        os_printf( "*****EEPROM data bits not 6, 8 or 1*****\n" );
#endif
        i = 6;
    }
    addrbits = i;

    // clear the dataval, leave the clock low to read in the data regardless
    OUTW(EE_ENB, ee_addr);
    eeprom_delay(1);
    
    retval = INW(ee_addr);
    if ( (EE_DATA_READ & retval) != 0 ) {
#ifdef DEBUG_EE
        os_printf( "Size EEPROM: Dummy data bit not 0, reg %x\n" , retval );
#endif
    }
    eeprom_delay(1);
    
    for (i = EE_TOP_DATA_BIT; i >= 0; i--) {
        OUTW(EE_ENB | EE_SHIFT_CLK, ee_addr);
        eeprom_delay(EEPROM_SK_DELAY);
        retval = INW(ee_addr);
        eeprom_delay(EEPROM_SK_DELAY);
        OUTW(EE_ENB, ee_addr);
        eeprom_delay(EEPROM_SK_DELAY);
    }
    
    // Terminate the EEPROM access.
    OUTW(EE_ENB & ~EE_CS, ee_addr);
    eeprom_delay(EEPROM_DONE_DELAY);
    
    return addrbits;
}


// ------------------------------------------------------------------------
//
//                       READ EEPROM
//
// ------------------------------------------------------------------------
static int
read_eeprom(long ioaddr, int location, int addr_len)
{
    unsigned short retval = 0;
    int ee_addr = ioaddr + SCBeeprom;
    int read_cmd = location | EE_READ_CMD(addr_len);
    int i, tries = 10;

 try_again:
    // Should already be not-selected, but anyway:
    OUTW(EE_ENB & ~EE_CS, ee_addr);
    eeprom_delay(EEPROM_ENABLE_DELAY);
    OUTW(EE_ENB, ee_addr);
    eeprom_delay(EEPROM_ENABLE_DELAY);
    
    // Shift the read command bits out, changing only one bit per time.
    for (i = EE_TOP_CMD_BIT(addr_len); i >= 0; i--) {
        short dataval = (read_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
        OUTW(EE_ENB | dataval               , ee_addr);
        eeprom_delay(EEPROM_SK_DELAY);
        OUTW(EE_ENB | dataval | EE_SHIFT_CLK, ee_addr);
        eeprom_delay(EEPROM_SK_DELAY);
        OUTW(EE_ENB | dataval               , ee_addr);
        eeprom_delay(EEPROM_SK_DELAY);
    }

    // clear the dataval, leave the clock low
    OUTW(EE_ENB, ee_addr);
    eeprom_delay(1);
    
    retval = INW(ee_addr);
    // This should show a zero in the data read bit to confirm that the
    // address transfer is compelete.  If not, go to the start and try
    // again!
    if ( (0 != (retval & EE_DATA_READ)) && (tries-- > 0) ) {
    // Terminate the EEPROM access.
        OUTW(EE_ENB & ~EE_CS, ee_addr);
        eeprom_delay(EEPROM_DONE_DELAY);
#ifdef DEBUG_EE
        os_printf( "Warning: Retrying EEPROM read word %d, address %x, try %d\n",
                   location,  ee_addr, tries+1 );
#endif
        goto try_again;
    }

    // This fires with one device on one of the customer boards!
    // (but is OK on all other h/w.  Worrying huh.)
    if ( (EE_DATA_READ & retval) != 0 ) {
#ifdef DEBUG_EE
        os_printf( "Read EEPROM: Dummy data bit not 0, reg %x\n" , retval );
#endif
    }
    eeprom_delay(1);
    retval = 0;

    for (i = EE_TOP_DATA_BIT; i >= 0; i--) {
        OUTW(EE_ENB | EE_SHIFT_CLK, ee_addr);
        eeprom_delay(EEPROM_SK_DELAY);
        retval = (retval << 1) | ((INW(ee_addr) & EE_DATA_READ) ? 1 : 0);
        eeprom_delay(EEPROM_SK_DELAY);
        OUTW(EE_ENB, ee_addr);
        eeprom_delay(EEPROM_SK_DELAY);
    }
    
    // Terminate the EEPROM access.
    OUTW(EE_ENB & ~EE_CS, ee_addr);
    eeprom_delay(EEPROM_DONE_DELAY);
    
    return retval;
}

// ------------------------------------------------------------------------
//
//                NETWORK INTERFACE INITIALIZATION
//
//  Function : Init82559
//
//  Description :
//       This routine resets, configures, and initializes the chip.
//       It also clears the ethernet statistics structure, and selects
//       which statistics are supported by this driver.
//
// ------------------------------------------------------------------------
static bool
i82559_init(struct cyg_netdevtab_entry * ndp)
{
    static int initialized = 0; // only probe PCI et al *once*

    struct eth_drv_sc *sc;
    cyg_uint32 selftest;
    volatile cyg_uint32 *p_selftest;
    cyg_uint32 ioaddr;
    int count;
    int ints;
    struct i82559 *p_i82559;
    cyg_uint8 mac_address[ETHER_ADDR_LEN];

#ifdef DEBUG
    db_printf("intel_i82559_init\n");
#endif

    sc = (struct eth_drv_sc *)(ndp->device_instance);
    p_i82559 = (struct i82559 *)(sc->driver_private);

    IF_BAD_82559( p_i82559 ) {
#ifdef DEBUG
        os_printf( "Bad device private pointer %x\n", sc->driver_private );
#endif
        return 0;
    }

    CHECK_NDP_SC_LINK();

    if ( 0 == initialized++ ) {
        // then this is the first time ever:
        if ( ! pci_init_find_82559s() ) {
#ifdef DEBUG
            os_printf( "pci_init_find_82559s failed" );
#endif
            return 0;
        }
    }

    // If this device is not present, exit
    if (0 == p_i82559->found)
        return 0;

    ioaddr = p_i82559->io_address; // get I/O address for 82559

#ifdef DEBUG
    os_printf("Init82559 %d @ %x\n82559 Self Test\n",
              p_i82559->index, (int)ndp);
#endif

    ints = Mask82559Interrupt(p_i82559);

    // Reset device
    i82559_reset(p_i82559);

    // Perform a system self-test. (get enough mem to round address)
    if ( (selftest = (cyg_uint32)pciwindow_mem_alloc(32) ) == 0)
        return (0);
    p_selftest = (cyg_uint32 *) ((selftest + 15) & ~0xf);
    p_selftest[0] = p_selftest[1] = -1;

    OUTL( (VIRT_TO_BUS(p_selftest)) | I82559_SELFTEST, ioaddr + SCBPort);
    count = 0x7FFFF;                // Timeout for self-test.
    do {
        udelay(10);
    } while ( (p_selftest[1] == -1)  &&  (--count >= 0) );

    // Reset device again after selftest
    i82559_reset(p_i82559);

    Acknowledge82559Interrupt(p_i82559);
    UnMask82559Interrupt(p_i82559, ints );
    
    if (count < 0) {
        // Test timed out.
#ifdef DEBUG
        os_printf("Self test failed\n");
#endif
        return (0);
    }
#ifdef DEBUG
    os_printf("  General self-test: %s.\n"
              "  Serial sub-system self-test: %s.\n"
              "  Internal registers self-test: %s.\n"
              "  ROM checksum self-test: %s (%08X).\n",
              HAL_LE32TOC(p_selftest[1]) & 0x1000 ? "failed" : "passed",
              HAL_LE32TOC(p_selftest[1]) & 0x0020 ? "failed" : "passed",
              HAL_LE32TOC(p_selftest[1]) & 0x0008 ? "failed" : "passed",
              HAL_LE32TOC(p_selftest[1]) & 0x0004 ? "failed" : "passed",
              HAL_LE32TOC(p_selftest[0]));
#endif

    // FIXME: free self-test memory?

    if (p_i82559->hardwired_esa) {
        // Hardwire the address without consulting the EEPROM.
        // When this flag is set, the p_i82559 will already contain
        // the ESA. Copy it to a mac_address for call to set_mac_addr
        mac_address[0] = p_i82559->mac_address[0];
        mac_address[1] = p_i82559->mac_address[1];
        mac_address[2] = p_i82559->mac_address[2];
        mac_address[3] = p_i82559->mac_address[3];
        mac_address[4] = p_i82559->mac_address[4];
        mac_address[5] = p_i82559->mac_address[5];

        eth_set_mac_address(p_i82559, mac_address);
    } else {
        int addr_length, i;
        cyg_uint16 checksum;

        // read eeprom and get 82559's mac address
        addr_length = get_eeprom_size(ioaddr);
        // (this is the length of the *EEPROM*s address, not MAC address)

        // If length is 1, it _probably_ means there's no EEPROM
        // present.  Couldn't find an explicit mention of this in the
        // docs, but length=1 appears to be the behaviour in that case.
        if (1 == addr_length) {
#ifdef DEBUG_EE
            os_printf("Error: No EEPROM present for device %d\n", 
                      p_i82559->index);
#endif
        } else {
            for (checksum = 0, i = 0, count = 0; count < 64; count++) {
                cyg_uint16 value;
                // read word from eeprom
                value = read_eeprom(ioaddr, count, addr_length);
#ifdef DEBUG_EE
                // os_printf( "%2d: %04x\n", count, value );
#endif
                checksum += value;
                if (count < 3) {
                    mac_address[i++] = value & 0xFF;
                    mac_address[i++] = (value >> 8) & 0xFF;
                }
            }

            // If the EEPROM checksum is wrong, the MAC address read
            // from the EEPROM is probably wrong as well. In that
            // case, we don't set mac_addr_ok, but continue the
            // initialization. If then somebody calls i82559_start
            // without calling eth_set_mac_address() first, we refuse
            // to bring up the interface, because running with an
            // invalid MAC address is not a very brilliant idea.
        
            if ((checksum & 0xFFFF) != 0xBABA)  {
                // selftest verified checksum, verify again
#ifdef DEBUG_EE
                os_printf("Warning: Invalid EEPROM checksum %04X for device %d\n",
                          checksum, p_i82559->index);
#endif
            } else {
                p_i82559->mac_addr_ok = 1;
#ifdef DEBUG_EE
                os_printf("Valid EEPROM checksum\n");
#endif
            }
        }

        // record the MAC address in the device structure
        p_i82559->mac_address[0] = mac_address[0];
        p_i82559->mac_address[1] = mac_address[1];
        p_i82559->mac_address[2] = mac_address[2];
        p_i82559->mac_address[3] = mac_address[3];
        p_i82559->mac_address[4] = mac_address[4];
        p_i82559->mac_address[5] = mac_address[5];
    }

#ifdef DEBUG
    os_printf("MAC Address = %02X %02X %02X %02X %02X %02X\n",
              p_i82559->mac_address[0], p_i82559->mac_address[1],
              p_i82559->mac_address[2], p_i82559->mac_address[3],
              p_i82559->mac_address[4], p_i82559->mac_address[5]);
#endif
    
    // and record the net dev pointer
    p_i82559->ndp = (void *)ndp;
    
    InitRxRing(p_i82559);
    InitTxRing(p_i82559);

    // Initialize upper level driver
    if ( p_i82559->mac_addr_ok )
        (sc->funs->eth_drv->init)(sc, &(p_i82559->mac_address[0]) );
    else
        (sc->funs->eth_drv->init)(sc, NULL );

    return (1);
}

// ------------------------------------------------------------------------
//
//  Function : i82559_start
//
// ------------------------------------------------------------------------
static void 
i82559_start( struct eth_drv_sc *sc, unsigned char *enaddr, int flags )
{
    struct i82559 *p_i82559;
    cyg_uint32 ioaddr;
    int count;
    cyg_uint16 status;
#ifdef KEEP_STATISTICS
    void *p_statistics;
#endif
#ifdef CYGPKG_NET
    struct ifnet *ifp = &sc->sc_arpcom.ac_if;
#endif

    p_i82559 = (struct i82559 *)sc->driver_private;
    ioaddr = p_i82559->io_address; // get 82559's I/O address
    
    IF_BAD_82559( p_i82559 ) {
#ifdef DEBUG
        os_printf( "i82559_start: Bad device pointer %x\n", p_i82559 );
#endif
        return;
    }

    if ( ! p_i82559->mac_addr_ok ) {
#ifdef DEBUG
        os_printf("i82559_start %d: invalid MAC address, "
                  "can't bring up interface\n",
                  p_i82559->index );
#endif
        return;
    }

    if ( p_i82559->active )
        i82559_stop( sc );

#ifdef KEEP_STATISTICS
#ifdef CYGDBG_DEVS_ETH_INTEL_I82559_KEEP_STATISTICS
    p_i82559->p_statistics =
        p_statistics = pciwindow_mem_alloc(sizeof(I82559_COUNTERS));
    memset(p_statistics, 0xFFFFFFFF, sizeof(I82559_COUNTERS));
    // set statistics dump address
    wait_for_cmd_done(ioaddr, WAIT_CU);
    OUTL(VIRT_TO_BUS(p_statistics), ioaddr + SCBPointer);
    OUTW(SCB_M | CU_STATSADDR, ioaddr + SCBCmd);
    // Start dump command
    wait_for_cmd_done(ioaddr, WAIT_CU);
    OUTW(SCB_M | CU_DUMPSTATS, ioaddr + SCBCmd); // start register dump
    // ...and wait for it to complete operation
    count = 1000;
    do {
      HAL_READ_UINT16((cyg_uint8*)p_statistics + CFG_STATUS, status);
      udelay(1);
    } while (0 == (status & CFG_STATUS_C) && (count-- > 0));
#ifdef CYGDBG_USE_ASSERTS
    {
      HAL_READ_UINT16((cyg_uint8*)p_statistics + CFG_STATUS, status);
      CYG_ASSERT((CFG_STATUS_C | CFG_STATUS_OK)
                 == (status & (CFG_STATUS_C | CFG_STATUS_OK)),
                 "Dump stat command incomplete/failedd");
    }
#endif // CYGDBG_USE_ASSERTS
#endif
#endif

    // Enable device
    p_i82559->active = 1;
    eth_set_config(p_i82559);

#ifdef CYGPKG_NET
    if (( 0
#ifdef ETH_DRV_FLAGS_PROMISC_MODE
          != (flags & ETH_DRV_FLAGS_PROMISC_MODE)
#endif
        ) || (ifp->if_flags & IFF_PROMISC)
        ) {
        eth_set_promiscuous_mode(p_i82559);
    }
#endif
#ifdef DEBUG
    {
        int status = i82559_status( sc );
        os_printf("i82559_start %d flg %x Link = %s, %s Mbps, %s Duplex\n",
                  p_i82559->index,
                  *(int *)p_i82559,
                  status & GEN_STATUS_LINK ? "Up" : "Down",
                  status & GEN_STATUS_100MBPS ?  "100" : "10",
                  status & GEN_STATUS_FDX ? "Full" : "Half");
    }
#endif

    // Load pointer to Rx Ring and enable receiver
    wait_for_cmd_done(ioaddr, WAIT_RU);
    OUTL(VIRT_TO_BUS(p_i82559->rx_ring[0]), ioaddr + SCBPointer);
    OUTW(RUC_START, ioaddr + SCBCmd);
   
}

// ------------------------------------------------------------------------
//
//  Function : i82559_status
//
// ------------------------------------------------------------------------
int
i82559_status( struct eth_drv_sc *sc )
{
    int status;
    struct i82559 *p_i82559;
    cyg_uint32 ioaddr;
    p_i82559 = (struct i82559 *)sc->driver_private;
    
    IF_BAD_82559( p_i82559 ) {
#ifdef DEBUG
        os_printf( "i82559_status: Bad device pointer %x\n", p_i82559 );
#endif
        return 0;
    }

    ioaddr = p_i82559->io_address; // get 82559's I/O address

    status = INB(ioaddr + SCBGenStatus);

    return status;
}

// ------------------------------------------------------------------------
//
//  Function : BringDown82559
//
// ------------------------------------------------------------------------

static void
i82559_stop( struct eth_drv_sc *sc )
{
    struct i82559 *p_i82559;

    p_i82559 = (struct i82559 *)sc->driver_private;

    IF_BAD_82559( p_i82559 ) {
#ifdef DEBUG
        os_printf( "i82559_stop: Bad device pointer %x\n", p_i82559 );
#endif
        return;
    }
   
#ifdef DEBUG
    os_printf("i82559_stop %d flg %x\n", p_i82559->index, *(int *)p_i82559 );
#endif

    p_i82559->active = 0;               // stop people tormenting it
    i82559_reset(p_i82559);             // that should stop it

    ResetRxRing( p_i82559 );
    ResetTxRing( p_i82559 );
}


// ------------------------------------------------------------------------
//
//  Function : InitRxRing
//
// ------------------------------------------------------------------------
static void
InitRxRing(struct i82559* p_i82559)
{
    int i;
    RFD *rfd;
    RFD *p_rfd = 0;
#ifdef DEBUG_82559
    os_printf("InitRxRing %d\n", p_i82559->index);
#endif
    for ( i = 0; i < MAX_RX_DESCRIPTORS; i++ ) {
        rfd = (RFD *)pciwindow_mem_alloc(RFD_SIZEOF + MAX_RX_PACKET_SIZE);
        p_i82559->rx_ring[i] = rfd;
        if ( i )
            HAL_WRITE_UINT32(p_rfd+RFD_LINK, HAL_CTOLE32(VIRT_TO_BUS(rfd)));
        p_rfd = (RFD *)rfd;
    }
    // link last RFD to first:
    HAL_WRITE_UINT32(p_rfd+RFD_LINK,
                     HAL_CTOLE32(VIRT_TO_BUS(p_i82559->rx_ring[0])));

    ResetRxRing( p_i82559 );
}

// ------------------------------------------------------------------------
//
//  Function : ResetRxRing
//
// ------------------------------------------------------------------------
static void
ResetRxRing(struct i82559* p_i82559)
{
    RFD *p_rfd;
    int i;
#ifdef DEBUG_82559
    os_printf("ResetRxRing %d\n", p_i82559->index);
#endif
    for ( i = 0; i < MAX_RX_DESCRIPTORS; i++ ) {
        p_rfd = p_i82559->rx_ring[i];
        CYG_ASSERT( (cyg_uint8 *)p_rfd >= i82559_heap_base, "rfd under" );
        CYG_ASSERT( (cyg_uint8 *)p_rfd <  i82559_heap_free, "rfd over" );
#ifdef CYGDBG_USE_ASSERTS
        {
          RFD *p_rfd2;
          cyg_uint32 link;
          p_rfd2 = p_i82559->rx_ring[ ( i ? (i-1) : (MAX_RX_DESCRIPTORS-1) ) ];
          HAL_READ_UINT32(p_rfd2 + RFD_LINK, link);
          CYG_ASSERT( HAL_LE32TOC(link) == VIRT_TO_BUS(p_rfd), 
                      "rfd linked list broken" );
        }
#endif
        HAL_WRITE_UINT32(p_rfd + RFD_STATUS, /*0*/RFD_STATUS_S);
        HAL_WRITE_UINT16(p_rfd + RFD_COUNT, 0);
        HAL_WRITE_UINT32(p_rfd + RFD_RDB_ADDR, HAL_CTOLE32(0xFFFFFFFF));
        HAL_WRITE_UINT16(p_rfd + RFD_SIZE, HAL_CTOLE16(MAX_RX_PACKET_SIZE));
    }
    p_i82559->next_rx_descriptor = 0;
    // And set an end-of-list marker in the previous one.
    HAL_WRITE_UINT32(p_rfd + RFD_STATUS, RFD_STATUS_EL);
}

// ------------------------------------------------------------------------
//
//  Function : PacketRxReady        (Called from delivery thread)
//
// ------------------------------------------------------------------------
static void
PacketRxReady(struct i82559* p_i82559)
{
    RFD *p_rfd;
    int next_descriptor;
    int length, ints;
    struct cyg_netdevtab_entry *ndp;
    struct eth_drv_sc *sc;
    cyg_uint32 ioaddr;
    cyg_uint16 status;

    ndp = (struct cyg_netdevtab_entry *)(p_i82559->ndp);
    sc = (struct eth_drv_sc *)(ndp->device_instance);

    CHECK_NDP_SC_LINK();

    ioaddr = p_i82559->io_address;

    next_descriptor = p_i82559->next_rx_descriptor;
    p_rfd = p_i82559->rx_ring[next_descriptor];

    CYG_ASSERT( (cyg_uint8 *)p_rfd >= i82559_heap_base, "rfd under" );
    CYG_ASSERT( (cyg_uint8 *)p_rfd <  i82559_heap_free, "rfd over" );

    while ( 1 ) {
        cyg_uint32 rxstatus;
        cyg_uint16 rxstatus_hi;
        HAL_READ_UINT32(p_rfd + RFD_STATUS, rxstatus);
        if (0 == (rxstatus & RFD_STATUS_C))
            break;

        HAL_READ_UINT16(p_rfd + RFD_STATUS_HI, rxstatus_hi);
        rxstatus_hi |= RFD_STATUS_HI_EL;
        HAL_WRITE_UINT16(p_rfd + RFD_STATUS_HI, rxstatus_hi);

        HAL_READ_UINT16(p_rfd + RFD_COUNT, length);
        length = HAL_LE16TOC(length);
        length &= RFD_COUNT_MASK;

#ifdef DEBUG_82559
        os_printf( "Device %d (eth%d), rx descriptor %d:\n", 
                   p_i82559->index, p_i82559->index, next_descriptor );
//        dump_rfd( p_rfd, 1 );
#endif

        p_i82559->next_rx_descriptor = next_descriptor;
        // Check for bogusly short packets; can happen in promisc mode:
        // Asserted against and checked by upper layer driver.
#ifdef CYGPKG_NET
        if ( length > sizeof( struct ether_header ) )
            // then it is acceptable; offer the data to the network stack
#endif
        (sc->funs->eth_drv->recv)( sc, length );

        HAL_WRITE_UINT16(p_rfd + RFD_COUNT, 0);
        HAL_WRITE_UINT16(p_rfd + RFD_STATUS_LO, 0);

        // The just-emptied slot is now ready for re-use and already marked EL;
        // we can now remove the EL marker from the previous one.
        if ( 0 == next_descriptor )
            p_rfd = p_i82559->rx_ring[ MAX_RX_DESCRIPTORS-1 ];
        else
            p_rfd = p_i82559->rx_ring[ next_descriptor-1 ];
        // The previous one: check it *was* marked before clearing.
        HAL_READ_UINT16(p_rfd + RFD_STATUS_HI, rxstatus_hi);
        CYG_ASSERT( rxstatus_hi & RFD_STATUS_HI_EL, "No prev EL" );
        // that word is not written by the device.
        HAL_WRITE_UINT16(p_rfd + RFD_STATUS_HI, 0);

#ifdef KEEP_STATISTICS
        statistics[p_i82559->index].rx_deliver++;
#endif
        if (++next_descriptor >= MAX_RX_DESCRIPTORS)
            next_descriptor = 0;
        p_rfd = p_i82559->rx_ring[next_descriptor];

        CYG_ASSERT( (cyg_uint8 *)p_rfd >= i82559_heap_base, "rfd under" );
        CYG_ASSERT( (cyg_uint8 *)p_rfd <  i82559_heap_free, "rfd over" );

    }

    // See if the RU has gone idle (usually because of out of resource
    // condition) and restart it if needs be.
    ints = Mask82559Interrupt(p_i82559);
    status = INW(ioaddr + SCBStatus);
    if ( RU_STATUS_READY != (status & RU_STATUS_MASK) ) {
        // Acknowledge the RX INT sources
        OUTW( SCB_INTACK_RX, ioaddr + SCBStatus);
        // (see pages 6-10 & 6-90)

#ifdef KEEP_STATISTICS
        statistics[p_i82559->index].rx_restart++;
#endif
        // There's an end-of-list marker out there somewhere...
        // So mop it up; it takes a little time but this is infrequent.
        ResetRxRing( p_i82559 );  
        next_descriptor = 0;        // re-initialize next desc.
        // load pointer to Rx Ring
        wait_for_cmd_done(ioaddr, WAIT_RU);
        OUTL(VIRT_TO_BUS(p_i82559->rx_ring[0]),
             ioaddr + SCBPointer);
        OUTW(RUC_START, ioaddr + SCBCmd);
        Acknowledge82559Interrupt(p_i82559);
    }
    UnMask82559Interrupt(p_i82559, ints);

    p_i82559->next_rx_descriptor = next_descriptor;
}

// and the callback function

static void 
i82559_recv( struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len )
{
    struct i82559 *p_i82559;
    RFD *p_rfd;
    int next_descriptor;
    int total_len;
    struct eth_drv_sg *last_sg;
    volatile cyg_uint8 *from_p;
    cyg_uint32 rxstatus;

    p_i82559 = (struct i82559 *)sc->driver_private;
    
    IF_BAD_82559( p_i82559 ) {
#ifdef DEBUG
        os_printf( "i82559_recv: Bad device pointer %x\n", p_i82559 );
#endif
        return;
    }

    next_descriptor = p_i82559->next_rx_descriptor;
    p_rfd = p_i82559->rx_ring[next_descriptor];
    
    CYG_ASSERT( (cyg_uint8 *)p_rfd >= i82559_heap_base, "rfd under" );
    CYG_ASSERT( (cyg_uint8 *)p_rfd <  i82559_heap_free, "rfd over" );

    HAL_READ_UINT32(p_rfd + RFD_STATUS, rxstatus);
    CYG_ASSERT( rxstatus & RFD_STATUS_C, "No complete frame" );
    CYG_ASSERT( rxstatus & RFD_STATUS_EL, "No marked frame" );
    CYG_ASSERT( rxstatus & RFD_STATUS_C, "No complete frame 2" );
    CYG_ASSERT( rxstatus & RFD_STATUS_EL, "No marked frame 2" );
    
    if ( 0 == (rxstatus & RFD_STATUS_C) )
        return;

    HAL_READ_UINT16(p_rfd + RFD_COUNT, total_len);
    total_len = HAL_LE16TOC(total_len);
    total_len &= RFD_COUNT_MASK;
    
#ifdef DEBUG_82559
    os_printf("Rx %d %x (status %x): %d sg's, %d bytes\n",
              p_i82559->index, (int)p_i82559, 
              HAL_LE32TOC(rxstatus), sg_len, total_len);
#endif

    // Copy the data to the network stack
    from_p = p_rfd + RFD_BUFFER;

    // check we have memory to copy into; we would be called even if
    // caller was out of memory in order to maintain our state.
    if ( 0 == sg_len || 0 == sg_list )
        return; // caller was out of mbufs

    CYG_ASSERT( 0 < sg_len, "sg_len underflow" );
    CYG_ASSERT( MAX_ETH_DRV_SG >= sg_len, "sg_len overflow" );

    for ( last_sg = &sg_list[sg_len]; sg_list < last_sg; sg_list++ ) {
        cyg_uint8 *to_p;
        int l;
            
        to_p = (cyg_uint8 *)(sg_list->buf);
        l = sg_list->len;

        CYG_ASSERT( 0 <= l, "sg length -ve" );

        if ( 0 >= l || 0 == to_p )
            return; // caller was out of mbufs

        if ( l > total_len )
            l = total_len;

        memcpy( to_p, (unsigned char *)from_p, l );
        from_p += l;
        total_len -= l;
    }

    CYG_ASSERT( 0 == total_len, "total_len mismatch in rx" );
    CYG_ASSERT( last_sg == sg_list, "sg count mismatch in rx" );
    CYG_ASSERT( p_rfd + RFD_BUFFER < from_p, "from_p wild in rx" );
    CYG_ASSERT( p_rfd + RFD_BUFFER + MAX_RX_PACKET_SIZE >= from_p,
                "from_p overflow in rx" );
}


// ------------------------------------------------------------------------
//
//  Function : InitTxRing
//
// ------------------------------------------------------------------------
static void
InitTxRing(struct i82559* p_i82559)
{
    int i;
    cyg_uint32 ioaddr;

#ifdef DEBUG_82559
    os_printf("InitTxRing %d\n", p_i82559->index);
#endif
    ioaddr = p_i82559->io_address;
    for ( i = 0; i < MAX_TX_DESCRIPTORS; i++) {
        p_i82559->tx_ring[i] = (TxCB *)pciwindow_mem_alloc(
            TxCB_SIZEOF + MAX_TX_PACKET_SIZE);
    }

    ResetTxRing(p_i82559);
}

// ------------------------------------------------------------------------
//
//  Function : ResetTxRing
//
// ------------------------------------------------------------------------
static void
ResetTxRing(struct i82559* p_i82559)
{
    int i;
    cyg_uint32 ioaddr;

#ifdef DEBUG_82559
    os_printf("ResetTxRing %d\n", p_i82559->index);
#endif
    ioaddr = p_i82559->io_address;
    p_i82559->tx_descriptor_add =
        p_i82559->tx_descriptor_active = 
        p_i82559->tx_descriptor_remove = 0;
    p_i82559->tx_in_progress =
        p_i82559->tx_queue_full = 0;

    for ( i = 0; i < MAX_TX_DESCRIPTORS; i++) {
        TxCB *p_txcb = p_i82559->tx_ring[i];
        CYG_ASSERT( (cyg_uint8 *)p_txcb >= i82559_heap_base, "txcb under" );
        CYG_ASSERT( (cyg_uint8 *)p_txcb <  i82559_heap_free, "txcb over" );

        HAL_WRITE_UINT16(p_txcb + TxCB_STATUS, 0);
        HAL_WRITE_UINT16(p_txcb + TxCB_CMD, 0);
        HAL_WRITE_UINT32(p_txcb + TxCB_LINK,
                         HAL_CTOLE32(VIRT_TO_BUS((cyg_uint32)p_txcb)));
        HAL_WRITE_UINT32(p_txcb + TxCB_TBD_ADDR, HAL_CTOLE32(0xFFFFFFFF));
        HAL_WRITE_UINT8(p_txcb + TxCB_TBD_NUMBER, 0);
        HAL_WRITE_UINT8(p_txcb + TxCB_TX_THRESHOLD, 16);
        HAL_WRITE_UINT16(p_txcb + TxCB_COUNT,
                         HAL_CTOLE16(TxCB_COUNT_EOF | 0));
        p_i82559->tx_keys[i] = 0;
    }
}

// ------------------------------------------------------------------------
//
//  Function : TxMachine          (Called from FG & ISR)
//
// This steps the Tx Machine onto the next record if necessary - allowing
// for missed interrupts, and so on.
// ------------------------------------------------------------------------

static void
TxMachine(struct i82559* p_i82559)
{
    int tx_descriptor_active;
    cyg_uint32 ioaddr;

    tx_descriptor_active = p_i82559->tx_descriptor_active;
    ioaddr = p_i82559->io_address;  
    
    // See if the CU is idle when we think it isn't; this is the only place
    // tx_descriptor_active is advanced. (Also recovers from a dropped intr)
    if ( p_i82559->tx_in_progress ) {
        cyg_uint16 status;
        status = INW(ioaddr + SCBStatus);
        if ( 0 == (status & CU_STATUS_MASK) ) {
            // It is idle.  So ack the TX interrupts
            OUTW( SCB_INTACK_TX, ioaddr + SCBStatus);
            // (see pages 6-10 & 6-90)

            // and step on to the next queued tx.
            p_i82559->tx_in_progress = 0;
            if ( ++tx_descriptor_active >= MAX_TX_DESCRIPTORS )
                tx_descriptor_active = 0;
            p_i82559->tx_descriptor_active = tx_descriptor_active;
        }
    }

    // is the CU idle, and there a next tx to set going?
    if ( ( ! p_i82559->tx_in_progress )
         && p_i82559->tx_descriptor_add != tx_descriptor_active ) {
        TxCB *p_txcb = p_i82559->tx_ring[tx_descriptor_active];
        cyg_uint16 status;
        status = INW(ioaddr + SCBStatus);
        CYG_ASSERT( ( 0 == (status & CU_STATUS_MASK)), "CU not idle");
        CYG_ASSERT( (cyg_uint8 *)p_txcb >= i82559_heap_base, "txcb under" );
        CYG_ASSERT( (cyg_uint8 *)p_txcb <  i82559_heap_free, "txcb over" );
#ifdef DEBUG_82559
        {
            unsigned long key = p_i82559->tx_keys[ tx_descriptor_active ];
            os_printf("Tx %d %x: Starting Engines: KEY %x TxCB %x\n",
                      p_i82559->index, (int)p_i82559, key, p_txcb);
        }
#endif

        // start Tx operation
        wait_for_cmd_done(ioaddr, WAIT_CU);
        OUTL(VIRT_TO_BUS(p_txcb), ioaddr + SCBPointer);
        OUTW(CU_START, ioaddr + SCBCmd);
        p_i82559->tx_in_progress = 1;
    }
}

// ------------------------------------------------------------------------
//
//  Function : TxDone          (Called from delivery thread)
//
// This returns Tx's from the Tx Machine to the stack (ie. reports
// completion) - allowing for missed interrupts, and so on.
// ------------------------------------------------------------------------

static void
TxDone(struct i82559* p_i82559)
{
    struct cyg_netdevtab_entry *ndp;
    struct eth_drv_sc *sc;
    int tx_descriptor_remove = p_i82559->tx_descriptor_remove;

    ndp = (struct cyg_netdevtab_entry *)(p_i82559->ndp);
    sc = (struct eth_drv_sc *)(ndp->device_instance);

    CHECK_NDP_SC_LINK();
    
    // "Done" txen are from here to active, OR 
    // the remove one if the queue is full AND its status is nonzero:
    while (  (tx_descriptor_remove != p_i82559->tx_descriptor_active) ||
             ( p_i82559->tx_queue_full) ) {

        cyg_uint16 txstatus;
        TxCB *p_txcb = p_i82559->tx_ring[ tx_descriptor_remove ];
        unsigned long key = p_i82559->tx_keys[ tx_descriptor_remove ];

        HAL_READ_UINT16(p_txcb + TxCB_STATUS, txstatus);
        if (0 == txstatus)
            break;

#ifdef DEBUG_82559
        os_printf("TxDone %d %x: KEY %x TxCB %x\n",
                  p_i82559->index, (int)p_i82559, key, p_txcb );
#endif
        (sc->funs->eth_drv->tx_done)( sc, key, 1 /* status */ );
        
        if ( ++tx_descriptor_remove >= MAX_TX_DESCRIPTORS )
            tx_descriptor_remove = 0;
        p_i82559->tx_descriptor_remove = tx_descriptor_remove;
        p_i82559->tx_queue_full = 0;
    }
}


// ------------------------------------------------------------------------
//
//  Function : i82559_can_send
//
// ------------------------------------------------------------------------

static int 
i82559_can_send(struct eth_drv_sc *sc)
{
    struct i82559 *p_i82559;
    int ints;

    p_i82559 = (struct i82559 *)sc->driver_private;

    IF_BAD_82559( p_i82559 ) {
#ifdef DEBUG
        os_printf( "i82559_send: Bad device pointer %x\n", p_i82559 );
#endif
        return 0;
    }
    
    // Advance TxMachine atomically
    ints = Mask82559Interrupt(p_i82559);
    TxMachine(p_i82559);
    Acknowledge82559Interrupt(p_i82559); // This can eat an Rx interrupt, so
    PacketRxReady(p_i82559);
    UnMask82559Interrupt(p_i82559,ints);

    return ! p_i82559->tx_queue_full;
}

// ------------------------------------------------------------------------
//
//  Function : i82559_send
//
// ------------------------------------------------------------------------

static void 
i82559_send(struct eth_drv_sc *sc,
            struct eth_drv_sg *sg_list, int sg_len, int total_len,
            unsigned long key)
{
    struct i82559 *p_i82559;
    int tx_descriptor_add, ints;
    TxCB *p_txcb;
    cyg_uint32 ioaddr;

    p_i82559 = (struct i82559 *)sc->driver_private;

    IF_BAD_82559( p_i82559 ) {
#ifdef DEBUG
        os_printf( "i82559_send: Bad device pointer %x\n", p_i82559 );
#endif
        return;
    }

#ifdef DEBUG_82559
    os_printf("Tx %d %x: %d sg's, %d bytes, KEY %x\n",
              p_i82559->index, (int)p_i82559, sg_len, total_len, key );
#endif

    if ( ! p_i82559->active )
        return;                         // device inactive, no return
#ifdef KEEP_STATISTICS
    statistics[p_i82559->index].tx_count++;
#endif
    ioaddr = p_i82559->io_address;      // get device I/O address

    if ( p_i82559->tx_queue_full ) {
#ifdef KEEP_STATISTICS
        statistics[p_i82559->index].tx_dropped++;
#endif
#ifdef DEBUG_82559
        os_printf( "i82559_send: Queue full, device %x, key %x\n",
                   p_i82559, key );
#endif
    }
    else {
        struct eth_drv_sg *last_sg;
        volatile cyg_uint8 *to_p;

        tx_descriptor_add = p_i82559->tx_descriptor_add;

        p_i82559->tx_keys[tx_descriptor_add] = key;

        p_txcb = p_i82559->tx_ring[tx_descriptor_add];

        CYG_ASSERT( (cyg_uint8 *)p_txcb >= i82559_heap_base, "txcb under" );
        CYG_ASSERT( (cyg_uint8 *)p_txcb <  i82559_heap_free, "txcb over" );

        HAL_WRITE_UINT16(p_txcb + TxCB_STATUS, 0);
        HAL_WRITE_UINT16(p_txcb + TxCB_CMD,
                         (TxCB_CMD_TRANSMIT | TxCB_CMD_S
                          | TxCB_CMD_I | TxCB_CMD_EL));
        HAL_WRITE_UINT32(p_txcb + TxCB_LINK, 
                         HAL_CTOLE32(VIRT_TO_BUS((cyg_uint32)p_txcb)));
        HAL_WRITE_UINT32(p_txcb + TxCB_TBD_ADDR,
                         HAL_CTOLE32(0xFFFFFFFF));
        HAL_WRITE_UINT8(p_txcb + TxCB_TBD_NUMBER, 0);
        HAL_WRITE_UINT8(p_txcb + TxCB_TX_THRESHOLD, 16);
        HAL_WRITE_UINT16(p_txcb + TxCB_COUNT,
                         HAL_CTOLE16(TxCB_COUNT_EOF | total_len));

        // Copy from the sglist into the txcb
        to_p = p_txcb + TxCB_BUFFER;

        CYG_ASSERT( 0 < sg_len, "sg_len underflow" );
        CYG_ASSERT( MAX_ETH_DRV_SG >= sg_len, "sg_len overflow" );

        for ( last_sg = &sg_list[sg_len]; sg_list < last_sg; sg_list++ ) {
            cyg_uint8 *from_p;
            int l;
            
            from_p = (cyg_uint8 *)(sg_list->buf);
            l = sg_list->len;

            if ( l > total_len )
                l = total_len;

            memcpy( (unsigned char *)to_p, from_p, l );
            to_p += l;
            total_len -= l;

            if ( 0 > total_len ) 
                break; // Should exit via sg_last normally
        }

        CYG_ASSERT( 0 == total_len, "length mismatch in tx" );
        CYG_ASSERT( last_sg == sg_list, "sg count mismatch in tx" );
        CYG_ASSERT( p_txcb + TxCB_BUFFER < to_p, "to_p wild in tx" );
        CYG_ASSERT( p_txcb + TxCB_BUFFER + MAX_TX_PACKET_SIZE >= to_p,
                    "to_p overflow in tx" );
  
        // Next descriptor
        if ( ++tx_descriptor_add >= MAX_TX_DESCRIPTORS)
            tx_descriptor_add = 0;
        p_i82559->tx_descriptor_add = tx_descriptor_add;

        // From this instant, interrupts can advance the world and start,
        // even complete, this tx request...

        if ( p_i82559->tx_descriptor_remove == tx_descriptor_add )
            p_i82559->tx_queue_full = 1;
    }

    // Try advancing the Tx Machine regardless

    // no more interrupts until started
    ints = Mask82559Interrupt(p_i82559);

    // Check that either:
    //     tx is already active, there is other stuff queued,
    // OR  this tx just added is the current active one
    // OR  this tx just added is already complete
    CYG_ASSERT(
        // The machine is busy:
        (p_i82559->tx_in_progress == 1) ||
        // or: The machine is idle and this just added is the next one
        (((p_i82559->tx_descriptor_add-1) == p_i82559->tx_descriptor_active)
         || ((0 == p_i82559->tx_descriptor_add) &&
             ((MAX_TX_DESCRIPTORS-1) == p_i82559->tx_descriptor_active))) ||
        // or: This tx is already complete
        (p_i82559->tx_descriptor_add == p_i82559->tx_descriptor_active),
                "Active/add mismatch" );

    // Advance TxMachine atomically
    TxMachine(p_i82559);
    Acknowledge82559Interrupt(p_i82559); // This can eat an Rx interrupt, so
    PacketRxReady(p_i82559);
    UnMask82559Interrupt(p_i82559, ints);
}

// ------------------------------------------------------------------------
//
//  Function : i82559_reset
//
// ------------------------------------------------------------------------
static void
i82559_reset(struct i82559* p_i82559)
{
    cyg_uint32 ioaddr = p_i82559->io_address;

    // First do soft reset. This must be done before the hard reset,
    // otherwise we may create havoc on the PCI bus.
    // Wait 20 uSecs afterwards for chip to settle.
    OUTL(I82559_SELECTIVE_RESET, ioaddr + SCBPort);
    udelay(20);
  
    // Do hard reset now that the controller is off the PCI bus.
    // Wait 20 uSecs afterwards for chip to settle.
    OUTL(I82559_RESET, ioaddr + SCBPort);
    udelay(20);

    // Set the base addresses
    wait_for_cmd_done(ioaddr, WAIT_RU);
    OUTL(0, ioaddr + SCBPointer);
    OUTW(SCB_M | RUC_ADDR_LOAD, ioaddr + SCBCmd);

    wait_for_cmd_done(ioaddr, WAIT_CU);
    OUTL(0, ioaddr + SCBPointer);
    OUTW(SCB_M | CU_ADDR_LOAD, ioaddr + SCBCmd);
}

// ------------------------------------------------------------------------
//
//                       INTERRUPT HANDLERS
//
// ------------------------------------------------------------------------

static cyg_uint32
eth_isr(cyg_vector_t vector, cyg_addrword_t data)
{
    struct i82559* p_i82559 = (struct i82559 *)data;
    cyg_uint16 status;
    cyg_uint32 ioaddr;

    IF_BAD_82559( p_i82559 ) {
#ifdef DEBUG
        os_printf( "i82559_isr: Bad device pointer %x\n", p_i82559 );
#endif
        return 0;
    }

    ioaddr = p_i82559->io_address;
    status = INW(ioaddr + SCBStatus);

    // Acknowledge all INT sources that were active
    OUTW( status & SCB_INTACK_MASK, ioaddr + SCBStatus);
    // (see pages 6-10 & 6-90)

#ifdef KEEP_STATISTICS
    statistics[p_i82559->index].interrupts++;

    // receiver left ready state ?
    if ( status & SCB_STATUS_RNR )
        statistics[p_i82559->index].rx_resource++;

    // frame receive interrupt ?
    if ( status & SCB_STATUS_FR )
        statistics[p_i82559->index].rx_count++;

    // transmit interrupt ?
    if ( status & SCB_STATUS_CX )
        statistics[p_i82559->index].tx_complete++;
#endif

    // Advance the Tx Machine regardless
    TxMachine(p_i82559);

    // it should have settled down now...
    Acknowledge82559Interrupt(p_i82559);

    return CYG_ISR_CALL_DSR;        // schedule DSR
}


// ------------------------------------------------------------------------
#ifdef CYGNUM_DEVS_ETH_INTEL_I82559_MUX_INTERRUPT
static cyg_uint32
eth_mux_isr(cyg_vector_t vector, cyg_addrword_t data)
{
    int i;
    struct i82559* p_i82559;

    for (i = 0; i < CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT; i++) {
        p_i82559 = i82559_priv_array[i];
        if ( p_i82559->active )
            (void)eth_isr( vector, (cyg_addrword_t)p_i82559 );
    }

    return CYG_ISR_CALL_DSR;
}
#endif

// ------------------------------------------------------------------------

static void
eth_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    struct i82559* p_i82559 = (struct i82559 *)data;
    struct cyg_netdevtab_entry *ndp =
        (struct cyg_netdevtab_entry *)(p_i82559->ndp);
    struct eth_drv_sc *sc = (struct eth_drv_sc *)(ndp->device_instance);

    // but here, it must be a *sc:
    eth_drv_dsr( vector, count, (cyg_addrword_t)sc );
}

// ------------------------------------------------------------------------
// This is called from the function below (used to be uni-DSR)
void
i82559_deliver(struct eth_drv_sc *sc)
{
    struct i82559* p_i82559 = (struct i82559 *)(sc->driver_private);

    // First pass any rx data up the stack
    PacketRxReady(p_i82559);

    // Then scan for completed Txen and inform the stack
    TxDone(p_i82559);
}


// ------------------------------------------------------------------------

#ifdef CYGNUM_DEVS_ETH_INTEL_I82559_MUX_INTERRUPT
static void
mux_deliver(struct eth_drv_sc *sc)
{
    int i;

    // Since this must mux all devices, the incoming arg is ignored.
    for (i = 0; i < CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT; i++) {
        sc = i82559_sc_array[i];
        if ( p_i82559->active )
            i82559_deliver( sc );
    }
}
#endif

// ------------------------------------------------------------------------
// Device table entry to operate the chip in a polled mode.
// Only diddle the interface we were asked to!

void
i82559_poll(struct eth_drv_sc *sc)
{
    struct i82559 *p_i82559;
    int ints;
    p_i82559 = (struct i82559 *)sc->driver_private;
    
    IF_BAD_82559( p_i82559 ) {
#ifdef DEBUG
        os_printf( "i82559_poll: Bad device pointer %x\n", p_i82559 );
#endif
        return;
    }

    // Do these atomically
    ints = Mask82559Interrupt(p_i82559);

    // As it happens, this driver always requests the DSR to be called:
    (void)eth_isr( p_i82559->vector, (cyg_addrword_t)p_i82559 );

    // (no harm in calling this ints-off also, when polled)
    i82559_deliver( sc );

    Acknowledge82559Interrupt(p_i82559);
    UnMask82559Interrupt(p_i82559, ints);
}

// ------------------------------------------------------------------------
// Determine interrupt vector used by a device - for attaching GDB stubs
// packet handler.
int
i82559_int_vector(struct eth_drv_sc *sc)
{
    struct i82559 *p_i82559;
    p_i82559 = (struct i82559 *)sc->driver_private;
    return (p_i82559->vector);
}

#if 0
int
i82559_int_op( struct eth_drv_sc *sc, int mask)
{
    struct i82559 *p_i82559;
    p_i82559 = (struct i82559 *)sc->driver_private;

    if ( 1 == mask )
        return Mask82559Interrupt( p_i82559 );

    if ( 0 == mask )
        UnMask82559Interrupt( p_i82559, 0x0fffffff ); // enable all

    return 0;
}
#endif
    

// ------------------------------------------------------------------------
//
//  Function : pci_init_find_82559s
//
// This is called exactly once at the start of time to:
//  o scan the PCI bus for objects
//  o record them in the device table
//  o acquire all the info needed for the driver to access them
//  o instantiate interrupts for them
//  o attach those interrupts appropriately
// ------------------------------------------------------------------------
static int
pci_init_find_82559s( void )
{
    cyg_pci_device_id devid;
    cyg_pci_device dev_info;
    cyg_uint16 cmd;
    int device_index;
    int found_devices = 0;

#ifdef DEBUG
    db_printf("pci_init_find_82559s()\n");
#endif

    // allocate memory to be used in ioctls later
    if (mem_reserved_ioctl != (void*)0) {
#ifdef DEBUG
        db_printf("pci_init_find_82559s() called > once\n");
#endif
        return 0;
    }

    CYG_ASSERT( CYGARC_UNCACHED_ADDRESS((CYG_ADDRWORD)CYGMEM_SECTION_pci_window) ==
                CYGHWR_INTEL_I82559_PCI_MEM_MAP_BASE,
      "PCI window configured does not match PCI memory section base" );
    CYG_ASSERT( CYGMEM_SECTION_pci_window_SIZE ==
                CYGHWR_INTEL_I82559_PCI_MEM_MAP_SIZE,
        "PCI window configured does not match PCI memory section size" );

    if ( CYGARC_UNCACHED_ADDRESS((CYG_ADDRWORD)CYGMEM_SECTION_pci_window) !=
         CYGHWR_INTEL_I82559_PCI_MEM_MAP_BASE
         ||
         CYGMEM_SECTION_pci_window_SIZE !=
         CYGHWR_INTEL_I82559_PCI_MEM_MAP_SIZE ) {
#ifdef DEBUG
        db_printf("pci_init_find_82559s(): PCI window misconfigured\n");
#endif
        return 0;
    }

    // First initialize the heap in PCI window'd memory
    i82559_heap_size = CYGHWR_INTEL_I82559_PCI_MEM_MAP_SIZE;
    i82559_heap_base = (cyg_uint8 *)CYGHWR_INTEL_I82559_PCI_MEM_MAP_BASE;
    i82559_heap_free = i82559_heap_base;

    mem_reserved_ioctl = pciwindow_mem_alloc(MAX_MEM_RESERVED_IOCTL);     

    cyg_pci_init();
#ifdef DEBUG
    db_printf("Finished cyg_pci_init();\n");
#endif

    devid = CYG_PCI_NULL_DEVID;

    for (device_index = 0; 
         device_index < CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT;
         device_index++) {
        struct i82559 *p_i82559 = i82559_priv_array[device_index];

        p_i82559->index = device_index;

        // Intel 82559 and 82557 are virtually identical,
        // with different dev codes
        if (cyg_pci_find_device(0x8086, 0x1030, &devid) ||
            cyg_pci_find_device(0x8086, 0x1209, &devid) ||
            cyg_pci_find_device(0x8086, 0x1229, &devid) ) {
#ifdef DEBUG
            db_printf("eth%d = 82559\n", device_index);
#endif
            cyg_pci_get_device_info(devid, &dev_info);

            if (cyg_pci_translate_interrupt(&dev_info, &p_i82559->vector)) {
#ifdef DEBUG
                db_printf(" Wired to HAL vector %d\n", p_i82559->vector);
#endif
                cyg_drv_interrupt_create(
                    p_i82559->vector,
                    0,                  // Priority - unused
                    (CYG_ADDRWORD)p_i82559, // Data item passed to ISR & DSR
                    eth_isr,            // ISR
                    eth_dsr,            // DSR
                    &p_i82559->interrupt_handle, // handle to intr obj
                    &p_i82559->interrupt_object ); // space for int obj

                cyg_drv_interrupt_attach(p_i82559->interrupt_handle);

                // Don't unmask the interrupt yet, that could get us into a
                // race.
#ifdef CYGNUM_DEVS_ETH_INTEL_I82559_MUX_INTERRUPT
                // ALSO attach it to MUX interrupt for multiplexed
                // interrupts.  This is for certain boards where the
                // PCI backplane is wired "straight through" instead of
                // with a rotation of interrupt lines in the different
                // slots.
                {
                    static cyg_handle_t mux_interrupt_handle = 0;
                    static cyg_interrupt mux_interrupt_object;

                    if ( ! mux_interrupt_handle ) {
#ifdef DEBUG
                        db_printf(" Also attaching to HAL vector %d\n", 
                                  CYGNUM_DEVS_ETH_INTEL_I82559_MUX_INTERRUPT);
#endif
                        cyg_drv_interrupt_create(
                            CYGNUM_DEVS_ETH_INTEL_I82559_MUX_INTERRUPT,
                            0,              // Priority - unused
                            (CYG_ADDRWORD)p_i82559,// Data item passed to ISR and DSR
                            eth_mux_isr,    // ISR
                            eth_dsr,        // DSR
                            &mux_interrupt_handle,
                            &mux_interrupt_object );
                        
                        cyg_drv_interrupt_attach(mux_interrupt_handle);
                    }
                }
#endif // CYGNUM_DEVS_ETH_INTEL_I82559_MUX_INTERRUPT
            }
            else {
                p_i82559->vector=0;
#ifdef DEBUG
                db_printf(" Does not generate interrupts.\n");
#endif
            }

            if (cyg_pci_configure_device(&dev_info)) {
#ifdef DEBUG
                int i;
                db_printf("Found device on bus %d, devfn 0x%02x:\n",
                          CYG_PCI_DEV_GET_BUS(devid),
                          CYG_PCI_DEV_GET_DEVFN(devid));

                if (dev_info.command & CYG_PCI_CFG_COMMAND_ACTIVE) {
                    db_printf(" Note that board is active. Probed"
                              " sizes and CPU addresses invalid!\n");
                }
                db_printf(" Vendor    0x%04x", dev_info.vendor);
                db_printf("\n Device    0x%04x", dev_info.device);
                db_printf("\n Command   0x%04x, Status 0x%04x\n",
                          dev_info.command, dev_info.status);
                
                db_printf(" Class/Rev 0x%08x", dev_info.class_rev);
                db_printf("\n Header 0x%02x\n", dev_info.header_type);

                db_printf(" SubVendor 0x%04x, Sub ID 0x%04x\n",
                          dev_info.header.normal.sub_vendor, 
                          dev_info.header.normal.sub_id);

                for(i = 0; i < CYG_PCI_MAX_BAR; i++) {
                    db_printf(" BAR[%d]    0x%08x /", i, dev_info.base_address[i]);
                    db_printf(" probed size 0x%08x / CPU addr 0x%08x\n",
                              dev_info.base_size[i], dev_info.base_map[i]);
                }
                db_printf(" eth%d configured\n", device_index);
#endif
                found_devices++;
                p_i82559->found = 1;
                p_i82559->active = 0;
                p_i82559->devid = devid;
                p_i82559->memory_address = dev_info.base_map[0];
                p_i82559->io_address = dev_info.base_map[1];
#ifdef DEBUG
                db_printf(" memory address = 0x%08x\n", dev_info.base_map[0]);
                db_printf(" I/O address = 0x%08x\n", dev_info.base_map[1]);
#endif

                // Don't use cyg_pci_set_device_info since it clears
                // some of the fields we want to print out below.
                cyg_pci_read_config_uint16(dev_info.devid,
                                           CYG_PCI_CFG_COMMAND, &cmd);
                cmd |= (CYG_PCI_CFG_COMMAND_IO         // enable I/O space
                        | CYG_PCI_CFG_COMMAND_MEMORY   // enable memory space
                        | CYG_PCI_CFG_COMMAND_MASTER); // enable bus master
                cyg_pci_write_config_uint16(dev_info.devid,
                                            CYG_PCI_CFG_COMMAND, cmd);

                // Now the PCI part of the device is configured, reset
                // it. This should make it safe to enable the
                // interrupt
                i82559_reset(p_i82559);

                if (p_i82559->vector != 0) {
                    cyg_drv_interrupt_acknowledge(p_i82559->vector);
                    cyg_drv_interrupt_unmask(p_i82559->vector);
                }
#ifdef DEBUG
                db_printf(" **** Device enabled for I/O and Memory "
                          "and Bus Master\n");
#endif
            }
            else {
                p_i82559->found = 0;
                p_i82559->active = 0;
#ifdef DEBUG
                db_printf("Failed to configure device %d\n",device_index);
#endif
            }
        }
        else {
            p_i82559->found = 0;
            p_i82559->active = 0;
#ifdef DEBUG
            db_printf("eth%d not found\n", device_index);
#endif
        }
    }

    if (0 == found_devices)
        return 0;

#ifdef CYGNUM_DEVS_ETH_INTEL_I82559_MUX_INTERRUPT
    // Now enable the mux shared interrupt if it is in use
    if (mux_interrupt_handle) {
        cyg_drv_interrupt_acknowledge(CYGNUM_DEVS_ETH_INTEL_I82559_MUX_INTERRUPT);
        cyg_drv_interrupt_unmask(CYGNUM_DEVS_ETH_INTEL_I82559_MUX_INTERRUPT);
    }
#endif

    // Now a delay to ensure the hardware has "come up" before you try to
    // use it.  Yes, really, the full 2 seconds.  It's only really
    // necessary if DEBUG is off - otherwise all that printout wastes
    // enough time.  No kidding.
    udelay( 2000000 );
    return 1;
}

// This function is called to enable the device by setting the necessary
// minimum of the configuration.
static int
eth_set_config(struct i82559* p_i82559)
{
    cyg_uint32  ioaddr;
    volatile CFG *ccs;
    volatile cyg_uint8* config_bytes;
    cyg_uint16 status;
    int count;

    IF_BAD_82559( p_i82559 ) {
#ifdef DEBUG
        os_printf( "eth_set_config: Bad device pointer %x\n", p_i82559 );
#endif
        return -1;
    }

    ioaddr = p_i82559->io_address;

    // Check the malloc we did earlier worked
    ccs = (CFG *)mem_reserved_ioctl;
    if (ccs == (void*)0) 
        return 2; // Failed

    // Prepare header
    HAL_WRITE_UINT16(ccs + CFG_CMD, 
                     (CFG_CMD_EL | CFG_CMD_SUSPEND | CFG_CMD_CONFIGURE));
    HAL_WRITE_UINT16(ccs + CFG_STATUS, 0);
    HAL_WRITE_UINT32(ccs + CFG_CB_LINK_OFFSET, 
                     HAL_CTOLE32(VIRT_TO_BUS((cyg_uint32)ccs)));

    // Default values from the Intel Manual
    config_bytes = ccs + CFG_BYTES;
    config_bytes[0]=0x9;
    config_bytes[1]=0xc;
    config_bytes[2]=0x0;
    config_bytes[3]=0x0;
    config_bytes[4]=0x0;
    config_bytes[5]=0x0;
    config_bytes[6]=0x32;
    config_bytes[7]=0x1;
    // This is the bit we go through all the trouble for. Must be set
    // before the device activates.
    config_bytes[8]=0x1;

    // Let chip read configuration
    wait_for_cmd_done(ioaddr, WAIT_CU);
    OUTL(VIRT_TO_BUS(ccs), ioaddr + SCBPointer);
    OUTW(SCB_M | CU_START, ioaddr + SCBCmd);

    // ...and wait for it to complete operation
    count = 1000;
    do {
      HAL_READ_UINT16(ccs + CFG_STATUS, status);
      udelay(1);
    } while (0 == (status & CFG_STATUS_C) && (count-- > 0));

    // Check status
    if ((status & (CFG_STATUS_C | CFG_STATUS_OK)) 
        != (CFG_STATUS_C | CFG_STATUS_OK)) {
        // Failed!
#ifdef DEBUG_82559
        os_printf("%s:%d Config update failed\n", __FUNCTION__, __LINE__);
#endif
        return 1;
    }

    return 0;
}

#ifdef CYGPKG_NET
// ------------------------------------------------------------------------
//
//  Function : eth_set_promiscuous_mode
//
//  Return : 0 = It worked.
//           non0 = It failed.
// ------------------------------------------------------------------------

static int
eth_set_promiscuous_mode(struct i82559* p_i82559)
{
    cyg_uint32  ioaddr;
    volatile CFG *ccs;
    volatile cyg_uint8* config_bytes;
    cyg_uint16 status;
    int count;

    IF_BAD_82559( p_i82559 ) {
#ifdef DEBUG
        os_printf( "eth_set_promiscuos_mode: Bad device pointer %x\n",
                   p_i82559 );
#endif
        return -1;
    }

    ioaddr = p_i82559->io_address;  
  
    // Check the malloc we did earlier worked
    ccs = (CFG *)mem_reserved_ioctl;
    if (ccs == (void*)0) 
        return 2; // Failed

    // Prepare header
    HAL_WRITE_UINT16(ccs + CFG_CMD, 
                     (CFG_CMD_EL | CFG_CMD_SUSPEND | CFG_CMD_CONFIGURE));
    HAL_WRITE_UINT16(ccs + CFG_STATUS, 0);
    HAL_WRITE_UINT32(ccs + CFG_CB_LINK_OFFSET, 
                     HAL_CTOLE32(VIRT_TO_BUS((cyg_uint32)ccs)));

    // Default values from the Intel Manual
    config_bytes = ccs + CFG_BYTES;
    config_bytes[0]=0x13;
    config_bytes[1]=0x8;
    config_bytes[2]=0x0;
    config_bytes[3]=0x0;
    config_bytes[4]=0x0;
    config_bytes[5]=0x0;
    config_bytes[6]=0xb2; // (promisc ? 0x80 : 0) | 0x32 for small stats,
    config_bytes[7]=0x1;  // \      ditto         | 0x12 for stats with PAUSE stats
    config_bytes[8]=0x0;  //  \     ditto         | 0x16 for PAUSE + TCO stats
    config_bytes[9]=0x0;
    config_bytes[10]=0x28;
    config_bytes[11]=0x0;
    config_bytes[12]=0x60;
    config_bytes[13]=0x0;          // arp
    config_bytes[14]=0x0;          // arp
                   
    config_bytes[15]=0x81;         // promiscuous mode set
                                   // \ or 0x80 for normal mode.
    config_bytes[16]=0x0;
    config_bytes[17]=0x40;
    config_bytes[18]=0x72;         // Keep the Padding Enable bit

    // Let chip read configuration
    wait_for_cmd_done(ioaddr, WAIT_CU);
    OUTL(VIRT_TO_BUS(ccs), ioaddr + SCBPointer);
    OUTW(SCB_M | CU_START, ioaddr + SCBCmd);

    // ...and wait for it to complete operation
    count = 1000;
    do {
      HAL_READ_UINT16(ccs + CFG_STATUS, status);
      udelay(1);
    } while (0 == (status & CFG_STATUS_C) && (count-- > 0));

    // Check status
    if ((status & (CFG_STATUS_C | CFG_STATUS_OK)) 
        != (CFG_STATUS_C | CFG_STATUS_OK)) {
        // Failed!
#ifdef DEBUG_82559
        os_printf("%s:%d Config update failed\n", __FUNCTION__, __LINE__);
#endif
        return 1;
    }
    return 0;
}
#endif

// ------------------------------------------------------------------------
// We use this as a templete when writing a new MAC address into the
// eeproms. The MAC address in the first few bytes is over written
// with the correct MAC address and then the whole lot is programmed
// into the serial EEPROM. The checksum is calculated on the fly and
// sent instead of the last two bytes.
// The values are copied from the Intel EtherPro10/100+ &c devices
// in the EBSA boards.

#ifdef CYGPKG_DEVS_ETH_INTEL_I82559_WRITE_EEPROM

#define ee00 0x00, 0x00 // shorthand

static char eeprom_burn[126] = { 
/* halfword addresses! */
/*  0: */  0x00, 0x90,   0x27, 0x8c,       0x57, 0x82,   0x03, 0x02,
/*  4: */     ee00   ,   0x01, 0x02,       0x01, 0x47,      ee00   ,
/*  8: */  0x13, 0x72,   0x06, 0x83,       0xa2, 0x40,   0x0c, 0x00,
/*  C: */  0x86, 0x80,      ee00   ,          ee00   ,      ee00   ,
/* 10: */     ee00   ,      ee00   ,          ee00   ,      ee00   ,
/* 14: */     ee00   ,      ee00   ,          ee00   ,      ee00   ,
/* 18: */     ee00   ,      ee00   ,          ee00   ,      ee00   ,
/* 1C: */     ee00   ,      ee00   ,          ee00   ,      ee00   ,
/* 20: */     ee00   ,      ee00   ,          ee00   ,      ee00   ,
/* 24: */     ee00   ,      ee00   ,          ee00   ,      ee00   ,
/* 28: */     ee00   ,      ee00   ,          ee00   ,      ee00   ,
/* 2C: */     ee00   ,      ee00   ,          ee00   ,      ee00   ,
/* 30: */  0x28, 0x01,      ee00   ,          ee00   ,      ee00   ,
/* 34: */     ee00   ,      ee00   ,          ee00   ,      ee00   ,
/* 38: */     ee00   ,      ee00   ,          ee00   ,      ee00   ,
/* 3C: */     ee00   ,      ee00   ,          ee00   
};
#undef ee00

#endif

// ------------------------------------------------------------------------
//
//  Function : eth_set_mac_address
//
//  Return : 0 = It worked.
//           non0 = It failed.
// ------------------------------------------------------------------------
static int
eth_set_mac_address(struct i82559* p_i82559, char *addr)
{
    cyg_uint32  ioaddr;
    cyg_uint16 status;
    volatile CFG *ccs;
    volatile cyg_uint8* config_bytes;
    int count;

    IF_BAD_82559( p_i82559 ) {
#ifdef DEBUG
        os_printf( "eth_set_mac_address : Bad device pointer %x\n",
                   p_i82559 );
#endif
        return -1;
    }

    ioaddr = p_i82559->io_address;      
    
    ccs = (CFG *)mem_reserved_ioctl;
    if (ccs == (void*)0)
        return 2;

    HAL_WRITE_UINT16(ccs + CFG_CMD, 
                     (CFG_CMD_EL | CFG_CMD_SUSPEND | CFG_CMD_IAS));
    HAL_WRITE_UINT16(ccs + CFG_STATUS, 0);
    HAL_WRITE_UINT32(ccs + CFG_CB_LINK_OFFSET, 
                     HAL_CTOLE32(VIRT_TO_BUS((cyg_uint32)ccs)));

    config_bytes = ccs + CFG_BYTES;
    memcpy((char *)(config_bytes),addr,6);
    config_bytes[6]=0x0;
    config_bytes[7]=0x0;

    // Let chip read new ESA
    wait_for_cmd_done(ioaddr, WAIT_CU);
    OUTL(VIRT_TO_BUS(ccs), ioaddr + SCBPointer); 
    OUTW(SCB_M | CU_START, ioaddr + SCBCmd);    

    // ...and wait for it to complete operation
    count = 1000;
    do {
      HAL_READ_UINT16(ccs + CFG_STATUS, status);
      udelay(1);
    } while (0 == (status & CFG_STATUS_C) && (count-- > 0));

    // Check status
    HAL_READ_UINT16(ccs + CFG_STATUS, status);
    if ((status & (CFG_STATUS_C | CFG_STATUS_OK)) 
        != (CFG_STATUS_C | CFG_STATUS_OK)) {
#ifdef DEBUG_82559
        os_printf("%s:%d ESA update failed\n", __FUNCTION__, __LINE__);
#endif
        return 3;
    }

#ifdef CYGPKG_DEVS_ETH_INTEL_I82559_WRITE_EEPROM
    {
        int checksum, i, count;
        // (this is the length of the *EEPROM*s address, not MAC address)
        int addr_length;

        addr_length = get_eeprom_size( ioaddr );

        // now set this address in the device eeprom ....
        (void)memcpy(eeprom_burn,addr,6);

        // No idea what these were for...
        // eeprom_burn[20] &= 0xfe;   
        // eeprom_burn[20] |= p_i82559->index;   
        
        program_eeprom( ioaddr, addr_length, eeprom_burn );
   
        // update 82559 driver data structure ...
        udelay( 100000 );

        // by reading EEPROM to get the mac address back
        for (checksum = 0, i = 0, count = 0; count < 64; count++) {
            cyg_uint16 value;
            // read word from eeprom
            value = read_eeprom(ioaddr, count, addr_length);
            checksum += value;
            if (count < 3) {
                p_i82559->mac_address[i++] = value & 0xFF;
                p_i82559->mac_address[i++] = (value >> 8) & 0xFF;
            }
        }
    
#ifdef DEBUG
        os_printf("MAC Address = %02X %02X %02X %02X %02X %02X\n",
                  p_i82559->mac_address[0], p_i82559->mac_address[1],
                  p_i82559->mac_address[2], p_i82559->mac_address[3],
                  p_i82559->mac_address[4], p_i82559->mac_address[5]);
#endif

        p_i82559->mac_addr_ok = 1;

        for ( i = 0, count = 0; i < 6; i++ )
            if ( p_i82559->mac_address[i] != addr[i] )
                count++;

        if ( count ) {
#ifdef DEBUG
            os_printf( "Warning: MAC Address read back wrong!  %d bytes differ.\n",
                       count );
#endif
            p_i82559->mac_addr_ok = 0;
        }

        // If the EEPROM checksum is wrong, the MAC address read from
        // the EEPROM is probably wrong as well. In that case, we
        // don't set mac_addr_ok.
        if ((checksum & 0xFFFF) != 0xBABA)  {
#ifdef DEBUG
            os_printf( "Warning: Invalid EEPROM checksum %04X for device %d\n",
                       checksum, p_i82559->index);
#endif
            p_i82559->mac_addr_ok = 0;
        }

#else // CYGPKG_DEVS_ETH_INTEL_I82559_WRITE_EEPROM

    // record the MAC address in the device structure
    p_i82559->mac_address[0] = addr[0];
    p_i82559->mac_address[1] = addr[1];
    p_i82559->mac_address[2] = addr[2];
    p_i82559->mac_address[3] = addr[3];
    p_i82559->mac_address[4] = addr[4];
    p_i82559->mac_address[5] = addr[5];
    p_i82559->mac_addr_ok = 1;

#endif // ! CYGPKG_DEVS_ETH_INTEL_I82559_WRITE_EEPROM

    return p_i82559->mac_addr_ok ? 0 : 1;
}

#ifdef CYGPKG_DEVS_ETH_INTEL_I82559_WRITE_EEPROM
// ------------------------------------------------------------------------
static void
write_eeprom(long ioaddr, int location, int addr_len, unsigned short value)
{
    int ee_addr = ioaddr + SCBeeprom;
    int write_cmd = location | EE_WRITE_CMD(addr_len); 
    int i;
    
    OUTW(EE_ENB & ~EE_CS, ee_addr);
    eeprom_delay( 100 );
    OUTW(EE_ENB, ee_addr);
    eeprom_delay( 100 );

//    os_printf("\n write_eeprom : write_cmd : %x",write_cmd);  
//    os_printf("\n addr_len : %x  value : %x ",addr_len,value);  

    /* Shift the write command bits out. */
    for (i = (addr_len+2); i >= 0; i--) {
        short dataval = (write_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
        OUTW(EE_ENB | dataval, ee_addr);
        eeprom_delay(100);
        OUTW(EE_ENB | dataval | EE_SHIFT_CLK, ee_addr);
        eeprom_delay(150);
    }
    OUTW(EE_ENB, ee_addr);
        
    for (i = 15; i >= 0; i--) {
        short dataval = (value & (1 << i)) ? EE_DATA_WRITE : 0;
        OUTW(EE_ENB | dataval, ee_addr);
        eeprom_delay(100);
        OUTW(EE_ENB | dataval | EE_SHIFT_CLK, ee_addr);
        eeprom_delay(150);
    }

    /* Terminate the EEPROM access. */
    OUTW(EE_ENB & ~EE_CS, ee_addr);
    eeprom_delay(150000); // let the write take effect
}

// ------------------------------------------------------------------------
static int
write_enable_eeprom(long ioaddr,  int addr_len)
{
    int ee_addr = ioaddr + SCBeeprom;
    int write_en_cmd = EE_WRITE_EN_CMD(addr_len); 
    int i;

    OUTW(EE_ENB & ~EE_CS, ee_addr);
    OUTW(EE_ENB, ee_addr);

#ifdef DEBUG_82559
    os_printf("write_en_cmd : %x",write_en_cmd);
#endif

    // Shift the wr/er enable command bits out.
    for (i = (addr_len+2); i >= 0; i--) {
	short dataval = (write_en_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
	OUTW(EE_ENB | dataval, ee_addr);
	eeprom_delay(100);
	OUTW(EE_ENB | dataval | EE_SHIFT_CLK, ee_addr);
	eeprom_delay(150);
    }

    // Terminate the EEPROM access.
    OUTW(EE_ENB & ~EE_CS, ee_addr);
    eeprom_delay(EEPROM_DONE_DELAY);
}


// ------------------------------------------------------------------------
static void
program_eeprom(cyg_uint32 ioaddr, cyg_uint32 eeprom_size, cyg_uint8 *data)
{
  cyg_uint32 i;
  cyg_uint16 checksum = 0;
  cyg_uint16 value;

  // First enable erase/write operations on the eeprom.
  // This is done through the EWEN instruction.
  write_enable_eeprom( ioaddr, eeprom_size );

  for (i=0 ; i< 63 ; i++) {
    value = ((unsigned short *)data)[i];
    checksum += value;
#ifdef DEBUG_82559
    os_printf("\n i : %x ... value to be written : %x",i,value);
#endif
    write_eeprom( ioaddr, i, eeprom_size, value);
#ifdef DEBUG_82559
    os_printf("\n val read : %x ",read_eeprom(ioaddr,i,eeprom_size));
#endif
  }
  value = 0xBABA - checksum;
#ifdef DEBUG_82559
  os_printf("\n i : %x ... checksum adjustment val to be written : %x",i,value);
#endif
  write_eeprom( ioaddr, i, eeprom_size, value );
}

// ------------------------------------------------------------------------
#endif // ! CYGPKG_DEVS_ETH_INTEL_I82559_WRITE_EEPROM


// ------------------------------------------------------------------------
//
//  Function : eth_get_mac_address
//
// ------------------------------------------------------------------------
#ifdef ETH_DRV_GET_MAC_ADDRESS
static int
eth_get_mac_address(struct i82559* p_i82559, char *addr)
{
    IF_BAD_82559( p_i82559 ) {
#ifdef DEBUG
        os_printf( "eth_get_mac_address : Bad device pointer %x\n",
                   p_i82559 );
#endif
        return -1;
    }

    memcpy( addr, (char *)(&p_i82559->mac_address[0]), 6 );
    return 0;
}
#endif
// ------------------------------------------------------------------------
//
//  Function : i82559_ioctl
//
// ------------------------------------------------------------------------
static int
i82559_ioctl(struct eth_drv_sc *sc, unsigned long key,
             void *data, int data_length)
{
    struct i82559 *p_i82559;

    p_i82559 = (struct i82559 *)sc->driver_private;

    IF_BAD_82559( p_i82559 ) {
#ifdef DEBUG
        os_printf( "i82559_ioctl/control: Bad device pointer %x\n", p_i82559 );
#endif
        return -1;
    }

#ifdef DEBUG
    db_printf( "i82559_ioctl: device eth%d at %x; key is 0x%x, data at %x[%d]\n",
               p_i82559->index, p_i82559, key, data, data_length );
#endif

    switch ( key ) {

#ifdef ETH_DRV_SET_MAC_ADDRESS
    case ETH_DRV_SET_MAC_ADDRESS:
        if ( 6 != data_length )
            return -2;
        return eth_set_mac_address( p_i82559, data );
#endif

#ifdef ETH_DRV_GET_MAC_ADDRESS
    case ETH_DRV_GET_MAC_ADDRESS:
        return eth_get_mac_address( p_i82559, data );
#endif

#ifdef ETH_DRV_GET_IF_STATS_UD
    case ETH_DRV_GET_IF_STATS_UD: // UD == UPDATE
        ETH_STATS_INIT( sc );    // so UPDATE the statistics structure
#endif
        // drop through
#ifdef ETH_DRV_GET_IF_STATS
    case ETH_DRV_GET_IF_STATS:
#endif
#if defined(ETH_DRV_GET_IF_STATS) || defined (ETH_DRV_GET_IF_STATS_UD)
    {
        struct ether_drv_stats *p = (struct ether_drv_stats *)data;
        int i;
        static unsigned char my_chipset[]
            = { ETH_DEV_DOT3STATSETHERCHIPSET };

        strcpy( p->description, CYGDAT_DEVS_ETH_DESCRIPTION );
        CYG_ASSERT( 48 > strlen(p->description), "Description too long" );

        for ( i = 0; i < SNMP_CHIPSET_LEN; i++ )
            if ( 0 == (p->snmp_chipset[i] = my_chipset[i]) )
                break;

        i = i82559_status( sc );

        if ( !( i & GEN_STATUS_LINK) ) {
            p->operational = 2;         // LINK DOWN
            p->duplex = 1;              // UNKNOWN
            p->speed = 0;
        }
        else {
            p->operational = 3;            // LINK UP
            p->duplex = (i & GEN_STATUS_FDX) ? 3 : 2; // 2 = SIMPLEX, 3 = DUPLEX
            p->speed = ((i & GEN_STATUS_100MBPS) ? 100 : 10) * 1000000;
        }

#ifdef KEEP_STATISTICS
        {
            I82559_COUNTERS *pc = &i82559_counters[ p_i82559->index ];
            STATISTICS      *ps = &statistics[      p_i82559->index ];

            // Admit to it...
            p->supports_dot3        = true;

            // Those commented out are not available on this chip.

            p->tx_good              = pc->tx_good             ;
            p->tx_max_collisions    = pc->tx_max_collisions   ;
            p->tx_late_collisions   = pc->tx_late_collisions  ;
            p->tx_underrun          = pc->tx_underrun         ;
            p->tx_carrier_loss      = pc->tx_carrier_loss     ;
            p->tx_deferred          = pc->tx_deferred         ;
            //p->tx_sqetesterrors   = pc->tx_sqetesterrors    ;
            p->tx_single_collisions = pc->tx_single_collisions;
            p->tx_mult_collisions   = pc->tx_mult_collisions  ;
            p->tx_total_collisions  = pc->tx_total_collisions ;
            p->rx_good              = pc->rx_good             ;
            p->rx_crc_errors        = pc->rx_crc_errors       ;
            p->rx_align_errors      = pc->rx_align_errors     ;
            p->rx_resource_errors   = pc->rx_resource_errors  ;
            p->rx_overrun_errors    = pc->rx_overrun_errors   ;
            p->rx_collisions        = pc->rx_collisions       ;
            p->rx_short_frames      = pc->rx_short_frames     ;
            //p->rx_too_long_frames = pc->rx_too_long_frames  ;
            //p->rx_symbol_errors   = pc->rx_symbol_errors    ;
        
            p->interrupts           = ps->interrupts          ;
            p->rx_count             = ps->rx_count            ;
            p->rx_deliver           = ps->rx_deliver          ;
            p->rx_resource          = ps->rx_resource         ;
            p->rx_restart           = ps->rx_restart          ;
            p->tx_count             = ps->tx_count            ;
            p->tx_complete          = ps->tx_complete         ;
            p->tx_dropped           = ps->tx_dropped          ;
        }
#endif // KEEP_STATISTICS

        p->tx_queue_len = MAX_TX_DESCRIPTORS;

        return 0; // OK
    }
#endif

    default:
        break;
    }
    return -1;
}

// ------------------------------------------------------------------------
//
// Statistics update...
//
// ------------------------------------------------------------------------

#ifdef KEEP_STATISTICS
#ifdef CYGDBG_DEVS_ETH_INTEL_I82559_KEEP_82559_STATISTICS
void
update_statistics(struct i82559* p_i82559)
{
    I82559_COUNTERS *p_statistics;
    cyg_uint32 *p_counter;
    cyg_uint32 *p_register;
    int reg_count, ints;
    
    ints = Mask82559Interrupt(p_i82559);

    // This points to the sthared memory stats area/command block
    p_statistics = (I82559_COUNTERS *)(p_i82559->p_statistics);

    if ( (p_statistics->done & 0xFFFF) == 0xA007 ) {
        p_counter = (cyg_uint32 *)&i82559_counters[ p_i82559->index ];
        p_register = (cyg_uint32 *)p_statistics;
        for ( reg_count = 0;
              reg_count < sizeof( I82559_COUNTERS ) / sizeof( cyg_uint32 ) - 1;
              reg_count++ ) {
            *p_counter += *p_register;
            p_counter++;
            p_register++;
        }
        p_statistics->done = 0;
        // start register dump
        wait_for_cmd_done(p_i82559->io_address, WAIT_CU);
        OUTW(CU_DUMPSTATS, p_i82559->io_address + SCBCmd);
    }
    Acknowledge82559Interrupt(p_i82559); // This can eat an Rx interrupt, so
    PacketRxReady(p_i82559);

    UnMask82559Interrupt(p_i82559, ints);
}
#endif
#endif // KEEP_STATISTICS

// ------------------------------------------------------------------------
//
//
//           CODE FOR DEBUGGING PURPOSES ONLY
//
//
// ------------------------------------------------------------------------
void
dump_txcb(TxCB *p_txcb)
{
    cyg_uint16 tmp8;
    cyg_uint16 tmp16;
    cyg_uint32 tmp32;

    os_printf("TxCB @ %x\n", (int)p_txcb);
    HAL_READ_UINT16(p_txcb + TxCB_STATUS, tmp16);
    os_printf("status = %04X ", HAL_LE16TOC(tmp16));
    HAL_READ_UINT16(p_txcb + TxCB_CMD, tmp16);
    os_printf("command = %04X ", HAL_LE16TOC(tmp16));
    HAL_READ_UINT32(p_txcb + TxCB_LINK, tmp32);
    os_printf("link = %08X ", HAL_LE32TOC(tmp32));
    HAL_READ_UINT32(p_txcb + TxCB_TBD_ADDR, tmp32);
    os_printf("tbd = %08X ", HAL_LE32TOC(tmp32));
    HAL_READ_UINT16(p_txcb + TxCB_COUNT, tmp16);
    tmp16 = HAL_LE16TOC(tmp16);
    os_printf("count = %d ", (tmp16 & TxCB_COUNT_MASK));
    os_printf("eof = %x ", (tmp16 & TxCB_COUNT_EOF) ? 1 : 0);
    HAL_READ_UINT8(p_txcb + TxCB_TX_THRESHOLD, tmp8);
    os_printf("threshold = %d ", tmp8);
    HAL_READ_UINT8(p_txcb + TxCB_TBD_NUMBER, tmp8);
    os_printf("tbd number = %d\n", tmp8);
}

// This is intended to be the body of a THREAD that prints stuff every 10
// seconds or so:
#ifdef KEEP_STATISTICS
#ifdef DISPLAY_STATISTICS
void
DisplayStatistics(void)
{
    int i;
    I82559_COUNTERS *p_statistics;
    cyg_uint32 *p_counter;
    cyg_uint32 *p_register;
    int reg_count;
    int status;
    
    while ( 1 ) {
#ifdef DISPLAY_82559_STATISTICS
        for ( i = 0; i < 2; i ++ ) {
            p_statistics = (I82559_COUNTERS *)i82559[i].p_statistics;
            if ( (p_statistics->done & 0xFFFF) == 0xA007 ) {
                p_counter = (cyg_uint32 *)&i82559_counters[i];
                p_register = (cyg_uint32 *)&p_statistics->tx_good;
                for ( reg_count = 20; reg_count != 0; reg_count--) {
                    *p_counter += *p_register;
                    p_counter++;
                    p_register++;
                }
                p_statistics->done = 0;
                // start register dump
            	wait_for_cmd_done(i82559[i].io_address, WAIT_CU);
                OUTW(CU_DUMPSTATS, i82559[i].io_address + SCBCmd);
            }
        }
#endif
        os_printf("\nRx\nPackets = %d  %d\n",
        statistics[0].rx_count, statistics[1].rx_count);
        os_printf("Deliver   %d  %d\n",
        statistics[0].rx_deliver, statistics[1].rx_deliver);
        os_printf("Resource  %d  %d\n",
        statistics[0].rx_resource, statistics[1].rx_resource);
        os_printf("Restart   %d  %d\n",
        statistics[0].rx_restart, statistics[1].rx_restart);

#ifdef DISPLAY_82559_STATISTICS
        os_printf("Count     %d  %d\n",
        i82559_counters[0].rx_good, i82559_counters[1].rx_good);
        os_printf("CRC       %d  %d\n",
        i82559_counters[0].rx_crc_errors, i82559_counters[1].rx_crc_errors);
        os_printf("Align     %d  %d\n",
        i82559_counters[0].rx_align_errors, i82559_counters[1].rx_align_errors);
        os_printf("Resource  %d  %d\n",
        i82559_counters[0].rx_resource_errors, i82559_counters[1].rx_resource_errors);
        os_printf("Overrun   %d  %d\n",
        i82559_counters[0].rx_overrun_errors, i82559_counters[1].rx_overrun_errors);
        os_printf("Collision %d  %d\n",
        i82559_counters[0].rx_collisions, i82559_counters[1].rx_collisions);
        os_printf("Short     %d  %d\n",
        i82559_counters[0].rx_short_frames, i82559_counters[1].rx_short_frames);
#endif
        os_printf("\nTx\nPackets = %d  %d\n",
        statistics[0].tx_count, statistics[1].tx_count);
        os_printf("Complete  %d  %d\n",
        statistics[0].tx_complete, statistics[1].tx_complete);
        os_printf("Dropped   %d  %d\n",
        statistics[0].tx_dropped, statistics[1].tx_dropped);
        os_printf("Count     %d  %d\n",
        i82559_counters[0].tx_good, i82559_counters[1].tx_good);
#ifdef DISPLAY_82559_STATISTICS
        os_printf("Collision %d  %d\n",
        i82559_counters[0].tx_max_collisions,i82559_counters[1].tx_max_collisions);
        os_printf("Late Col. %d  %d\n",
        i82559_counters[0].tx_late_collisions,i82559_counters[1].tx_late_collisions);
        os_printf("Underrun  %d  %d\n",
        i82559_counters[0].tx_underrun,i82559_counters[1].tx_underrun);
        os_printf("Carrier   %d  %d\n",
        i82559_counters[0].tx_carrier_loss,i82559_counters[1].tx_carrier_loss);
        os_printf("Deferred  %d  %d\n",
        i82559_counters[0].tx_deferred, i82559_counters[1].tx_deferred);
        os_printf("1 Col     %d  %d\n",
        i82559_counters[0].tx_single_collisions, i82559_counters[0].tx_single_collisions);
        os_printf("Mult. Col %d  %d\n",
        i82559_counters[0].tx_mult_collisions, i82559_counters[0].tx_mult_collisions);
        os_printf("Total Col %d  %d\n",
        i82559_counters[0].tx_total_collisions, i82559_counters[0].tx_total_collisions);
#endif
        status = INB(i82559[0].io_address + SCBGenStatus);
        os_printf("Interface 0 Link = %s, %s Mbps, %s Duplex\n",
            status & GEN_STATUS_LINK ? "Up" : "Down",
            status & GEN_STATUS_100MBPS ?  "100" : "10",
            status & GEN_STATUS_FDX ? "Full" : "Half");

        status = INB(i82559[1].io_address + SCBGenStatus);
        os_printf("Interface 1 Link = %s, %s Mbps, %s Duplex\n",
            status & GEN_STATUS_LINK ? "Up" : "Down",
            status & GEN_STATUS_100MBPS ?  "100" : "10",
            status & GEN_STATUS_FDX ? "Full" : "Half");

        cyg_thread_delay(1000);
    }
}
#endif // DISPLAY_STATISTICS
#endif // KEEP_STATISTICS

void
dump_rfd(RFD *p_rfd, int anyway )
{
    cyg_uint32 rxstatus, tmp32;
    cyg_uint16 tmp16;

    HAL_READ_UINT32(p_rfd + RFD_STATUS, rxstatus);
    if ( (0 != rxstatus) || anyway ) {
        os_printf("RFD @ %x = ", (int)p_rfd);
        os_printf("status = %x ", HAL_LE32TOC(rxstatus));
        HAL_READ_UINT32(p_rfd + RFD_LINK, tmp32);
        os_printf("link = %x ", HAL_LE32TOC(tmp32));
//      HAL_READ_UINT32(p_rfd + RFD_RDB_ADDR, tmp32);
//      os_printf("rdb_address = %x ", HAL_LE32TOC(rdb_address));
        HAL_READ_UINT16(p_rfd + RFD_COUNT, tmp16);
        tmp16 = HAL_LE16TOC(tmp16);
        os_printf("count = %x ", (tmp16 & RFD_COUNT_MASK));
        os_printf("f = %x ", (tmp16 & RFD_COUNT_F) ? 1 : 0);
        os_printf("eof = %x ", (tmp16 & RFD_COUNT_EOF) ? 1 : 0);
        HAL_READ_UINT16(p_rfd + RFD_SIZE, tmp16);
        os_printf("size = %x\n", HAL_LE16TOC(tmp16));
        // FIXME: HAL_BE16TOC surely? This is the network package
        // header, right?
        os_printf("[%04x %04x %04x] ",
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 0)),
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 2)),
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 4)));
        os_printf("[%04x %04x %04x] %04x : ",          
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 6)),
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 8)),
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 10)),
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 12)));
        os_printf("(%04x %04x %04x %04x) ",            
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 14)),
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 16)),
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 18)),
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 20)) );
        os_printf("[%04x %04x %04x] ",                 
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 22)),
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 24)),
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 26)) );
        os_printf("%d.%d.%d.%d ",
                  *((cyg_uint8  *)(p_rfd + RFD_BUFFER + 28)),
                  *((cyg_uint8  *)(p_rfd + RFD_BUFFER + 29)),
                  *((cyg_uint8  *)(p_rfd + RFD_BUFFER + 30)),
                  *((cyg_uint8  *)(p_rfd + RFD_BUFFER + 31)) );
        os_printf("[%04x %04x %04x] ",                 
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 32)),
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 34)),
                  *((cyg_uint16 *)(p_rfd + RFD_BUFFER + 36)) );
        os_printf("%d.%d.%d.%d ...\n",
                  *((cyg_uint8  *)(p_rfd + RFD_BUFFER + 38)),
                  *((cyg_uint8  *)(p_rfd + RFD_BUFFER + 39)),
                  *((cyg_uint8  *)(p_rfd + RFD_BUFFER + 40)),
                  *((cyg_uint8  *)(p_rfd + RFD_BUFFER + 41)) );
    }
}

void
dump_all_rfds( int intf )
{
    struct i82559* p_i82559 = i82559_priv_array[intf];
    int i, j;
    j = p_i82559->next_rx_descriptor;
    os_printf("rx descriptors for interface %d (eth%d):\n", intf, intf );
    for ( i = 0; i < MAX_RX_DESCRIPTORS; i++ )
        dump_rfd( p_i82559->rx_ring[i], (i > (j-3) && (i <= j)) );
    os_printf("next rx descriptor = %x\n\n", j);
}


void
dump_packet(cyg_uint8 *p_buffer, int length)
{
    int count;

    count = 0;
    while ( length > 0 ) {
        if ( count == 0 )
            os_printf("\n");
        count = (count + 1) & 0x0F;
        os_printf("%02X ", *p_buffer++);
        length--;
    }
    os_printf("\n");
}

// ------------------------------------------------------------------------

// EOF if_i82559.c
