//==========================================================================
//
//      if_ebsa285.c
//
//	Ethernet drivers
//	Intel EBSA285 and PRO/100+ platform specific support
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
//####BSDCOPYRIGHTBEGIN####
//
// -------------------------------------------
//
// Portions of this software may have been derived from OpenBSD or other sources,
// and are covered by the appropriate copyright disclaimers included herein.
//
// -------------------------------------------
//
//####BSDCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    hmt, gthomas
// Contributors: Ron Spence, Pacific Softworks
// Date:         2000-02-01
// Purpose:      
// Description:  hardware driver for 82559 Intel PRO/100+ ethernet and
//               Intel StrongARM EBSA-285 development boards
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/drv_api.h>
#include <netdev.h>
#include <eth_drv.h>

#ifdef CYGPKG_IO_PCI
#include <cyg/io/pci.h>
#else
#error "Need PCI package here"
#endif

// ------------------------------------------------------------------------

#define nDEBUG_82559
#define DEBUG

#define nKEEP_STATISTICS
#define nDISPLAY_STATISTICS
#define nDISPLAY_82559_STATISTICS


#define os_printf diag_printf
#define db_printf diag_printf

// ------------------------------------------------------------------------
// I/O access macros as inlines for type safety

static inline void OUTB(cyg_uint8 value, cyg_uint32 io_address)
{   *((cyg_uint8 *)io_address) = value;    }

static inline void OUTW(cyg_uint16 value, cyg_uint32 io_address)
{   *((cyg_uint16 *)io_address) = value;   }

static inline void OUTL(cyg_uint32 value, cyg_uint32 io_address)
{   *((cyg_uint32 *)io_address) = value;   }

static inline cyg_uint8 INB(cyg_uint32 io_address)
{   return *((cyg_uint8 *)io_address);     }

static inline cyg_uint16 INW(cyg_uint32 io_address)
{   return *((cyg_uint16 *)io_address);    }

static inline cyg_uint32 INL(cyg_uint32 io_address)
{   return *((cyg_uint32 *)io_address);    }

#define VIRT_TO_BUS( _x_ ) virt_to_bus((cyg_uint32)(_x_))
static inline cyg_uint32 virt_to_bus(cyg_uint32 p_memory)
{    return (p_memory - CYGHWR_HAL_ARM_EBSA285_PCI_MEM_MAP_BASE);    }

#define BUS_TO_VIRT( _x_ ) bus_to_virt((cyg_uint32)(_x_))
static inline cyg_uint32 bus_to_virt(cyg_uint32 p_memory)
{    return (p_memory + CYGHWR_HAL_ARM_EBSA285_PCI_MEM_MAP_BASE);    }


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
//                   82559 GENERAL STATUS REGISTER
//
// ------------------------------------------------------------------------
#define GEN_STATUS_FDX          0x04    // 1 = full duplex, 0 = half
#define GEN_STATUS_100MBPS      0x02    // 1 = 100 Mbps, 0 = 10 Mbps
#define GEN_STATUS_LINK         0x01    // 1 = link up, 0 = link down


// ------------------------------------------------------------------------
//
//                   82559 EEPROM INTERFACE
//
// ------------------------------------------------------------------------
//  EEPROM_Ctrl bits.
#define EE_SHIFT_CLK	0x01            // EEPROM shift clock.
#define EE_CS		0x02            // EEPROM chip select.
#define EE_DATA_WRITE	0x04            // EEPROM chip data in.
#define EE_WRITE_0	0x01
#define EE_WRITE_1	0x05
#define EE_DATA_READ	0x08            // EEPROM chip data out.
#define EE_ENB		(0x4800 | EE_CS)

// Delay between EEPROM clock transitions.
#define eeprom_delay(usec)		udelay(usec);

// The EEPROM commands include the always-set leading bit.
// (and require the local variable "addr_len", usually 6 or 8)
#define EE_WRITE_CMD	(5 << addr_len)
#define EE_READ_CMD	(6 << addr_len)
#define EE_ERASE_CMD	(7 << addr_len)
#define EE_WRITE_EN_CMD (19 << (addr_len-2))


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
typedef struct rfd {
    volatile cyg_uint32 status;         // result of receive operation
    volatile cyg_uint32 link;           // offset from RU base to next RFD
    volatile cyg_uint32 rdb_address;    // pointer to Rx data buffer
    volatile cyg_uint32 count:14,       // number of bytes received +
        f:1,                            //   + EOF & F flags
        eof:1,
            size:16;                    // size of the data buffer
    volatile cyg_uint8 buffer[0];       // data buffer (simple mode)
} RFD;

#define RFD_STATUS_EL   0x80000000      // 1=last RFD in RFA
#define RFD_STATUS_S    0x40000000      // 1=suspend RU after receiving frame
#define RFD_STATUS_H    0x00100000      // 1=RFD is a header RFD
#define RFD_STATUS_SF   0x00080000      // 0=simplified, 1=flexible mode
#define RFD_STATUS_C    0x00008000      // completion of received frame
#define RFD_STATUS_OK   0x00002000      // frame received with no errors

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


typedef struct {
    volatile cyg_uint32 count:14,       // bytes used in buffer
        f:1,                            // buffer has been used (filled)
        eof:1;                          // last receive buffer in frame
    volatile cyg_uint32 next_rbd;       // next RBD (RU base relative)
    volatile cyg_uint32 buffer_address; // address of receive data buffer
    volatile cyg_uint32 size:15,        // size of the associated buffer
        el:1;                           // buffer of this RBD is last
} RBD;


// ------------------------------------------------------------------------
//
//               TRANSMIT FRAME DESCRIPTORS
//
// ------------------------------------------------------------------------
typedef struct {
    volatile cyg_uint32 status:16,      // result of transmit operation
        command:16;                     // transmit command
    volatile cyg_uint32 link;           // offset from RU base to next RFD
    volatile cyg_uint32 tbd_address;    // pointer to Rx data buffer
    volatile cyg_uint32 count:15,       // number of bytes in transmit buffer
        eof:1,
        tx_threshold:8,
        tbd_number:8;
    volatile cyg_uint8 buffer[0];       // data buffer (simple mode)
} TxCB;


#define TxCB_CMD_TRANSMIT   0x0004      // transmit command
#define TxCB_CMD_SF         0x0008      // 0=simplified, 1=flexible mode
#define TxCB_CMD_NC         0x0010      // 0=CRC insert by controller
#define TxCB_CMD_I          0x2000      // generate interrupt on completion
#define TxCB_CMD_S          0x4000      // suspend on completion
#define TxCB_CMD_EL         0x8000      // last command block in CBL


// ------------------------------------------------------------------------
//
//                   STRUCTURES ADDED FOR PROMISCUOUS MODE
//
// ------------------------------------------------------------------------
typedef struct {
    cyg_uint32 cb_status_word:13,
        cb_ok:1,
        cb_dc:1,
        cb_complete:1,
        cb_cmd:3,
        cb_cmd_word:10,
        cb_int:1,
        cb_suspend:1,
        cb_el:1;
    cyg_uint32 cb_link_offset;
} CB_STRUCT;


typedef struct {
    CB_STRUCT cb_entry;
    cyg_uint8 config_bytes[24];
} CONFIG_CMD_STRUCT;

// ------------------------------------------------------------------------
//
//                       STATISTICAL COUNTER STRUCTURE
//
// ------------------------------------------------------------------------
#ifdef KEEP_STATISTICS
typedef struct {
    cyg_uint32 tx_good;
    cyg_uint32 tx_max_collisions;
    cyg_uint32 tx_late_collisions;
    cyg_uint32 tx_underrun;
    cyg_uint32 tx_carrier_loss;
    cyg_uint32 tx_deferred;
    cyg_uint32 tx_single_collisions;
    cyg_uint32 tx_mult_collisions;
    cyg_uint32 tx_total_collisions;
    cyg_uint32 rx_good;
    cyg_uint32 rx_crc_errors;
    cyg_uint32 rx_align_errors;
    cyg_uint32 rx_resource_errors;
    cyg_uint32 rx_overrun_errors;
    cyg_uint32 rx_collisions;
    cyg_uint32 rx_short_frames;
    cyg_uint32 done;
} I82559_COUNTERS;


typedef struct {
    cyg_uint32 interrupts;
    cyg_uint32 rx_count;
    cyg_uint32 rx_deliver;
    cyg_uint32 rx_resource;
    cyg_uint32 rx_restart;
    cyg_uint32 tx_count;
    cyg_uint32 tx_complete;
    cyg_uint32 tx_dropped;
} STATISTICS;


STATISTICS statistics[2];
I82559_COUNTERS i82559_counters[2];

#endif // KEEP_STATISTICS

// ------------------------------------------------------------------------

#define MAX_RX_PACKET_SIZE  1536        // maximum Rx packet size
#define MAX_TX_PACKET_SIZE  1536        // maximum Tx packet size

// The system seems to work OK with as few as 8 of RX and TX descriptors.
// It limps very painfully with only 4.
// Performance is better with more than 8.
// But the size of non-cached (so useless for anything else)
// memory window is 1Mb, so we might as well use it all.
//
// 128 for these uses the whole 1Mb, near enough.

#ifndef MAX_RX_DESCRIPTORS
#define MAX_RX_DESCRIPTORS	128     // number of Rx descriptors
#endif
#ifndef MAX_TX_DESCRIPTORS
#define MAX_TX_DESCRIPTORS	128     // number of Tx descriptors
#endif



typedef struct i82559 {
    cyg_uint8                           // (split up for atomic byte access)
        found:1,                        // was hardware discovered?
        mac_addr_ok:1,                  // can we bring up?
        active:1,                       // has this if been brung up?
        spare1:5; 
    cyg_uint8
        spare2:8; 
    cyg_uint8
        tx_in_progress:1,               // transmit in progress flag
        tx_queue_full:1,                // all Tx descriptors used flag
        spare3:6; 
    cyg_uint8  index;                   // 0 or 1 or whatever
    cyg_uint32 devid;                   // PCI device id
    cyg_uint32 memory_address;          // PCI memory address
    cyg_uint32 io_address;              // memory mapped I/O address
    cyg_uint8  mac_address[6];          // mac (hardware) address
    void *ndp;                          // Network Device Pointer

    int next_rx_descriptor;             // descriptor index for RFDs
    RFD *rx_ring[MAX_RX_DESCRIPTORS];   // location of Rx descriptors

    int tx_descriptor_add;              // descriptor index for additions
    int tx_descriptor_active;           // descriptor index for active tx
    int tx_descriptor_remove;           // descriptor index for remove

    TxCB *tx_ring[MAX_TX_DESCRIPTORS];  // location of Tx descriptors
    unsigned long tx_keys[MAX_TX_DESCRIPTORS];
                                        // keys for tx q management

    // Interrupt handling stuff
    cyg_vector_t    vector;             // interrupt vector
    cyg_handle_t    interrupt_handle;   // handle for int.handler
    cyg_interrupt   interrupt_object;

#ifdef KEEP_STATISTICS
    void *p_statistics;                 // pointer to statistical counters
#endif

} I82559;


// This is encapsulated here so that a change to > 2 interfaces can
// easily be accommodated.

#define IF_BAD_82559( _p_ )                                     \
    CYG_ASSERT( (&i82559[0] == (_p_)) || (&i82559[1] == (_p_)), \
                "Bad pointer-to-i82559" );                      \
    if ( (&i82559[0] != (_p_)) && (&i82559[1] != (_p_)) )

// ------------------------------------------------------------------------
// Instantiate the interfaces that we have:

#define MAX_82559 2                     // number of interfaces

I82559 i82559[MAX_82559];               // i82559 device info. structure

// eth0

ETH_DRV_SC(ebsa285_sc0,
           &i82559[0],                  // Driver specific data
           "eth0",                      // Name for this interface
           i82559_start,
           i82559_stop,
           i82559_ioctl,
           i82559_can_send,
           i82559_send,
           i82559_recv);

NETDEVTAB_ENTRY(ebsa285_netdev0, 
                "ebsa285-0", 
                ebsa285_i82559_init, 
                &ebsa285_sc0);

#if 1

// eth1

ETH_DRV_SC(ebsa285_sc1,
           &i82559[1],                  // Driver specific data
           "eth1",                      // Name for this interface
           i82559_start,
           i82559_stop,
           i82559_ioctl,
           i82559_can_send,
           i82559_send,
           i82559_recv);

NETDEVTAB_ENTRY(ebsa285_netdev1, 
                "ebsa285-1", 
                ebsa285_i82559_init, 
                &ebsa285_sc1);

#else
int ebsa285_netdev1 = -1; // for asserts about valid addresses
int ebsa285_sc1 = -1;
#endif // eth1 is included

// This is in a macro so that if more devices arrive it can easily be changed
#define CHECK_NDP_SC_LINK() CYG_MACRO_START                                \
    CYG_ASSERT( ((void *)ndp == (void *)&ebsa285_netdev0) ||               \
                ((void *)ndp == (void *)&ebsa285_netdev1), "Bad ndp" );    \
    CYG_ASSERT( ((void *)sc == (void *)&ebsa285_sc0) ||                    \
                ((void *)sc == (void *)&ebsa285_sc1), "Bad sc" );          \
    CYG_ASSERT( (void *)p_i82559 == sc->driver_private, "sc pointer bad" );\
CYG_MACRO_END

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

static void InitRxRing(struct i82559* p_i82559);
static void ResetRxRing(struct i82559* p_i82559);
static void InitTxRing(struct i82559* p_i82559);
static void ResetTxRing(struct i82559* p_i82559);

static int write_enable_eeprom(long ,  int );
static void program_eeprom(cyg_uint32 , cyg_uint32 , cyg_uint8 * );

static int eth_set_promiscuous_mode(struct i82559* p_i82559);

// debugging/logging only:
void dump_txcb(TxCB *p_txcb);
void DisplayStatistics(void);
void dump_rfd(RFD *p_rfd, int anyway );
void dump_all_rfds( int intf );
void dump_packet(cyg_uint8 *p_buffer, int length);

// ------------------------------------------------------------------------
// utilities
// ------------------------------------------------------------------------

static inline void wait_for_cmd_done(long scb_ioaddr)
{
    register int CSRstatus;
    register int wait = 0x100000;
    do CSRstatus = INB(scb_ioaddr + SCBCmd) ;
    while( CSRstatus && --wait >= 0);
    CYG_ASSERT( wait > 0, "wait_for_cmd_done" );
}

static inline void Mask82559Interrupt(struct i82559* p_i82559)
{
    cyg_drv_interrupt_mask(p_i82559->vector);
    cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_PCI_IRQ);
}

static inline void UnMask82559Interrupt(struct i82559* p_i82559)
{
    cyg_drv_interrupt_unmask(p_i82559->vector);
    cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_PCI_IRQ);
}

#ifdef CYGDBG_USE_ASSERTS // an indication of a debug build
static int acknowledge82559interrupt_compensating = 0;
#endif

static void Acknowledge82559Interrupt(struct i82559* p_i82559)
{
    int sources, mask;
    cyg_uint32 ioaddr;
    cyg_uint16 status;
    int loops = 64;

    cyg_drv_interrupt_acknowledge(p_i82559->vector);
    cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_PCI_IRQ);

    // It appears that some time can be taken before the interrupt source
    // *really* quietens down... this is ugly, but effective.
    // Without it, we get "Spurious Interrupt!" failures.
    ioaddr = p_i82559->io_address; // get I/O address for 82559
    mask = (1 << p_i82559->vector); // Do not include the MUX vector or we
    sources = *SA110_IRQCONT_IRQSTATUS; //...get hung on the other 82559
    status = INW(ioaddr + SCBStatus);
    while ( ((0 != (sources & mask)) || (0 != (status & SCB_INTACK_MASK)))
            && --loops >= 0) {
        OUTW( status & SCB_INTACK_MASK, ioaddr + SCBStatus);
        cyg_drv_interrupt_acknowledge(p_i82559->vector);
        cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_PCI_IRQ);
#ifdef CYGDBG_USE_ASSERTS
        acknowledge82559interrupt_compensating++; // verify this is executed
#endif
        sources = *SA110_IRQCONT_IRQSTATUS;
        status = INW(ioaddr + SCBStatus);
    }
    CYG_ASSERT( loops >= 0, "Acknowledge82559Interrupt" );
}


static void udelay(int delay)
{
  int i;
  // the loop is going to take 3 ticks.  At 228 MHz, to give uS, multiply
  // by 228/3 = 76 near enough.  No volatile is needed on i; gcc recognizes
  // delay loops and does NOT elide them.
  for ( i = 76 * delay; i ; i--)
    ;
}

// ------------------------------------------------------------------------
// Memory management
//
// Simply carve off from the front of the PCI mapped window into real memory

static void *pciwindow_mem_alloc(int size)
{
    void *p_memory;

    CYG_ASSERT(
        (CYGHWR_HAL_ARM_EBSA285_PCI_MEM_MAP_BASE <= (int)i82559_heap_free)
        &&
        ((CYGHWR_HAL_ARM_EBSA285_PCI_MEM_MAP_BASE + 
          CYGHWR_HAL_ARM_EBSA285_PCI_MEM_MAP_SIZE) > (int)i82559_heap_free)
        &&
        (0 < i82559_heap_size)
        &&
        (CYGHWR_HAL_ARM_EBSA285_PCI_MEM_MAP_SIZE >= i82559_heap_size)
        &&
        (CYGHWR_HAL_ARM_EBSA285_PCI_MEM_MAP_BASE == (int)i82559_heap_base),
        "Heap variables corrupted" );

    p_memory = (void *)0;
    size = (size + 3) & ~3;
    if ( (i82559_heap_free+size) < (i82559_heap_base+i82559_heap_size) ) {
        cyg_uint32 *p;
        p_memory = (void *)i82559_heap_free;
        i82559_heap_free += size;
        for ( p = (cyg_uint32 *)p_memory; size > 0; size -= 4 )
            *p++ = 0;
    }

    return p_memory;
}



// ------------------------------------------------------------------------
//
//                       READ EEPROM
//
// ------------------------------------------------------------------------
static int read_eeprom(long ioaddr, int location, int addr_len)
{
    unsigned short retval = 0;
    int ee_addr = ioaddr + SCBeeprom;
    int read_cmd = location | EE_READ_CMD;
    int i;
    
    OUTW(EE_ENB & ~EE_CS, ee_addr);
    eeprom_delay(1);
    OUTW(EE_ENB, ee_addr);
    eeprom_delay(1);
    
    // Shift the read command bits out.
    for (i = 12; i >= 0; i--) {
        short dataval = (read_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
        OUTW(EE_ENB | dataval, ee_addr);
        eeprom_delay(1);
        OUTW(EE_ENB | dataval | EE_SHIFT_CLK, ee_addr);
        eeprom_delay(1);
    }
    OUTW(EE_ENB, ee_addr);
    eeprom_delay(1);
    
    for (i = 15; i >= 0; i--) {
        OUTW(EE_ENB | EE_SHIFT_CLK, ee_addr);
        eeprom_delay(1);
        retval = (retval << 1) | ((INW(ee_addr) & EE_DATA_READ) ? 1 : 0);
        OUTW(EE_ENB, ee_addr);
        eeprom_delay(1);
    }
    
    // Terminate the EEPROM access.
    OUTW(EE_ENB & ~EE_CS, ee_addr);
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
ebsa285_i82559_init(struct cyg_netdevtab_entry * ndp)
{
    static int initialized = 0; // only probe PCI et al *once*

    struct eth_drv_sc *sc;
    cyg_uint32 selftest;
    volatile cyg_uint32 *p_selftest;
    cyg_uint32 ioaddr;
    cyg_uint16 checksum;
    int count;
    int i;
    int addr_length;
    cyg_uint8 mac_address[6];
    struct i82559 *p_i82559;

#ifdef DEBUG
    db_printf("ebsa285_i82559_init\n");
#endif

    sc = (struct eth_drv_sc *)(ndp->device_instance);
    p_i82559 = (struct i82559 *)(sc->driver_private);

    IF_BAD_82559( p_i82559 ) {
        os_printf( "Bad device private pointer %x\n", sc->driver_private );
        return 0;
    }

    CHECK_NDP_SC_LINK();

    if ( 0 == initialized++ ) {
        // then this is the first time ever:
        if ( ! pci_init_find_82559s() ) {
            os_printf( "pci_init_find_82559s failed" );
            return 0;
        }
    }

    if ( ! p_i82559->found ) // no device on PCI bus
        return (0);

    ioaddr = p_i82559->io_address; // get I/O address for 82559

#ifdef DEBUG
    os_printf("Init82559 %d @ %x\n82559 Self Test\n",
              p_i82559->index, (int)ndp);
#endif

    Mask82559Interrupt(p_i82559);

    wait_for_cmd_done(ioaddr); // make sure no command operating

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

    Acknowledge82559Interrupt(p_i82559);
    UnMask82559Interrupt(p_i82559);
    
    if (count < 0) {
        // Test timed out.
        os_printf("Self test failed\n");
        return (0);
    }
#ifdef DEBUG
    os_printf("  General self-test: %s.\n"
              "  Serial sub-system self-test: %s.\n"
              "  Internal registers self-test: %s.\n"
              "  ROM checksum self-test: %s (%08X).\n",
              p_selftest[1] & 0x1000 ? "failed" : "passed",
              p_selftest[1] & 0x0020 ? "failed" : "passed",
              p_selftest[1] & 0x0008 ? "failed" : "passed",
              p_selftest[1] & 0x0004 ? "failed" : "passed",
              p_selftest[0]);
#endif

    // read eeprom and get 82559's mac address
    addr_length = (read_eeprom(ioaddr, 0, 6) == 0xffff) ? 8 : 6;
    // (this is the length of the *EEPROM*s address, not MAC address)

    for (checksum = 0, i = 0, count = 0; count < 64; count++) {
        cyg_uint16 value;
        // read word from eeprom
        value = read_eeprom(ioaddr, count, addr_length);
        checksum += value;
        if (count < 3) {
            mac_address[i++] = value & 0xFF;
            mac_address[i++] = (value >> 8) & 0xFF;
        }
    }

    // If the EEPROM checksum is wrong, the MAC address read from the
    // EEPROM is probably wrong as well. In that case, we don't set
    // mac_addr_ok, but continue the initialization. If then somebody calls
    // i82559_start without calling eth_set_mac_address() first, we refuse
    // to bring up the interface, because running with an invalid MAC
    // address is not a very brilliant idea.

    if ((checksum & 0xFFFF) != 0xBABA)  {
        // selftest verified checksum, verify again
        os_printf( "Warning: Invalid EEPROM checksum %04X for device %d\n",
                   checksum, p_i82559->index);
    } else {
        p_i82559->mac_addr_ok = 1;
    }
#ifdef DEBUG
    os_printf("Valid EEPROM checksum\n");
    os_printf("MAC Address = %02X %02X %02X %02X %02X %02X\n",
              mac_address[0], mac_address[1], mac_address[2], mac_address[3],
              mac_address[4], mac_address[5]);
#endif

    // record the MAC address in the device structure
    p_i82559->mac_address[0] = mac_address[0];
    p_i82559->mac_address[1] = mac_address[1];
    p_i82559->mac_address[2] = mac_address[2];
    p_i82559->mac_address[3] = mac_address[3];
    p_i82559->mac_address[4] = mac_address[4];
    p_i82559->mac_address[5] = mac_address[5];
    
    // and record the net dev pointer
    p_i82559->ndp = (void *)ndp;
    
    InitRxRing(p_i82559);
    InitTxRing(p_i82559);

    // Initialize upper level driver
    if ( p_i82559->mac_addr_ok )
        eth_drv_init(sc, &(p_i82559->mac_address[0]) );
    else
        eth_drv_init(sc, 0 );

    return (1);
}



// ------------------------------------------------------------------------
//
//  Function : i82559_start
//
// ------------------------------------------------------------------------
static void i82559_start( struct eth_drv_sc *sc,
                          unsigned char *enaddr, int flags )
{
    struct i82559 *p_i82559;
    cyg_uint32 ioaddr;
#ifdef KEEP_STATISTICS
#ifdef DISPLAY_82559_STATISTICS
    void *p_statistics;
#endif
#endif

    p_i82559 = (struct i82559 *)sc->driver_private;
    
    IF_BAD_82559( p_i82559 ) {
        os_printf( "i82559_start: Bad device pointer %x\n", p_i82559 );
        return;
    }

    if ( ! p_i82559->mac_addr_ok ) {
        os_printf("i82559_start %d: invalid MAC address, "
                  "can't bring up interface\n",
                  p_i82559->index );
        return;
    }

    if ( p_i82559->active )
        i82559_stop( sc );

    ioaddr = p_i82559->io_address; // get 82559's I/O address

#ifdef KEEP_STATISTICS
#ifdef DISPLAY_82559_STATISTICS
    p_i82559->p_statistics =
        p_statistics = pciwindow_mem_alloc(sizeof(I82559_COUNTERS));
    memset(p_statistics, 0xFFFFFFFF, sizeof(I82559_COUNTERS));
    wait_for_cmd_done(ioaddr); // make sure no command operating
                                        // set statistics dump address
    OUTL(VIRT_TO_BUS(p_statistics), ioaddr + SCBPointer);
    OUTW(SCB_M | CU_STATSADDR, ioaddr + SCBCmd);

    wait_for_cmd_done(ioaddr); // make sure no command operating
    OUTW(SCB_M | CU_DUMPSTATS, ioaddr + SCBCmd); // start register dump
#endif
#endif

    // Set the base address
    wait_for_cmd_done(ioaddr);
    OUTL(0, ioaddr + SCBPointer);       // load ru base address = 0
    OUTW(SCB_M | RUC_ADDR_LOAD, ioaddr + SCBCmd);
    udelay( 1000 );                     // load pointer to Rx Ring
    OUTL(VIRT_TO_BUS(p_i82559->rx_ring[0]), ioaddr + SCBPointer);
    OUTW(RUC_START, ioaddr + SCBCmd);
    
    p_i82559->active = 1;

    if ( 0
#ifdef ETH_DRV_FLAGS_PROMISC_MODE
         != (flags & ETH_DRV_FLAGS_PROMISC_MODE)
#endif
        ) {
        eth_set_promiscuous_mode(p_i82559);
    }
#ifdef DEBUG
    {
        int status;
        status = INB(ioaddr + SCBGenStatus);
        os_printf("i82559_start %d flg %x Link = %s, %s Mbps, %s Duplex\n",
                  p_i82559->index,
                  *(int *)p_i82559,
                  status & GEN_STATUS_LINK ? "Up" : "Down",
                  status & GEN_STATUS_100MBPS ?  "100" : "10",
                  status & GEN_STATUS_FDX ? "Full" : "Half");
    }
#endif
}


// ------------------------------------------------------------------------
//
//  Function : BringDown82559
//
// ------------------------------------------------------------------------

static void i82559_stop( struct eth_drv_sc *sc )
{
    struct i82559 *p_i82559;

    p_i82559 = (struct i82559 *)sc->driver_private;

    IF_BAD_82559( p_i82559 ) {
        os_printf( "i82559_stop: Bad device pointer %x\n", p_i82559 );
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
static void InitRxRing(struct i82559* p_i82559)
{
    int i;
    RFD *rfd;
    RFD *p_rfd = 0;
#ifdef DEBUG_82559
    os_printf("InitRxRing %d\n", p_i82559->index);
#endif
    for ( i = 0; i < MAX_RX_DESCRIPTORS; i++ ) {
        rfd = (RFD *)pciwindow_mem_alloc(sizeof(RFD) + MAX_RX_PACKET_SIZE);
        p_i82559->rx_ring[i] = rfd;
        if ( i )
            p_rfd->link = VIRT_TO_BUS(rfd);
        p_rfd = (RFD *)rfd;
    }
    // link last RFD to first:
    p_rfd->link = VIRT_TO_BUS(p_i82559->rx_ring[0]);

    ResetRxRing( p_i82559 );
}

// ------------------------------------------------------------------------
//
//  Function : ResetRxRing
//
// ------------------------------------------------------------------------
static void ResetRxRing(struct i82559* p_i82559)
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
        CYG_ASSERT( p_i82559->rx_ring[
                 ( i ? (i-1) : (MAX_RX_DESCRIPTORS-1) )
            ]->link == VIRT_TO_BUS(p_rfd), "rfd linked list broken" );
        p_rfd->status = 0;
        p_rfd->count = 0;
        p_rfd->f = 0;
        p_rfd->eof = 0;
        p_rfd->rdb_address = 0xFFFFFFFF;
        p_rfd->size = MAX_RX_PACKET_SIZE;
    }
    p_i82559->next_rx_descriptor = 0;
}

// ------------------------------------------------------------------------
//
//  Function : PacketRxReady        (Called from DSR)
//
// ------------------------------------------------------------------------
static void PacketRxReady(struct i82559* p_i82559)
{
    RFD *p_rfd;
    int next_descriptor;
    int length;
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

    while ( p_rfd->status & RFD_STATUS_C ) {
        p_rfd->status |= RFD_STATUS_EL;
        length = p_rfd->count;

#ifdef DEBUG_82559
        os_printf( "Device %d (eth%d), rx descriptor %d:\n", 
                   p_i82559->index, p_i82559->index, next_descriptor );
//        dump_rfd( p_rfd, 1 );
#endif

        // Offer the data to the network stack
        p_i82559->next_rx_descriptor = next_descriptor;
        eth_drv_recv( sc, length );

        p_rfd->count = 0;
        p_rfd->f = 0;
        p_rfd->eof = 0;
        p_rfd->status = 0;
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
    Mask82559Interrupt(p_i82559);
    status = INW(ioaddr + SCBStatus);
    if ( RU_STATUS_READY != (status & RU_STATUS_MASK) ) {
        // Acknowledge the RX INT sources
        OUTW( SCB_INTACK_RX, ioaddr + SCBStatus);
        // (see pages 6-10 & 6-90)

#ifdef KEEP_STATISTICS
        statistics[p_i82559->index].rx_restart++;
#endif
        next_descriptor = 0;        // re-initialize next desc.
        // wait for SCB command complete
        wait_for_cmd_done(ioaddr);
        // load pointer to Rx Ring
        OUTL(VIRT_TO_BUS(p_i82559->rx_ring[0]),
             ioaddr + SCBPointer);
        OUTW(RUC_START, ioaddr + SCBCmd);
        Acknowledge82559Interrupt(p_i82559);
    }
    UnMask82559Interrupt(p_i82559);

    p_i82559->next_rx_descriptor = next_descriptor;
}

// and the callback function

static void i82559_recv( struct eth_drv_sc *sc,
                         struct eth_drv_sg *sg_list, int sg_len )
{
    struct i82559 *p_i82559;
    RFD *p_rfd;
    int next_descriptor;
    int total_len;
    struct eth_drv_sg *last_sg;
    volatile cyg_uint8 *from_p;

    p_i82559 = (struct i82559 *)sc->driver_private;
    
    IF_BAD_82559( p_i82559 ) {
        os_printf( "i82559_recv: Bad device pointer %x\n", p_i82559 );
        return;
    }

    next_descriptor = p_i82559->next_rx_descriptor;
    p_rfd = p_i82559->rx_ring[next_descriptor];
    
    CYG_ASSERT( (cyg_uint8 *)p_rfd >= i82559_heap_base, "rfd under" );
    CYG_ASSERT( (cyg_uint8 *)p_rfd <  i82559_heap_free, "rfd over" );

    CYG_ASSERT( p_rfd->status & RFD_STATUS_C, "No complete frame" );
    CYG_ASSERT( p_rfd->status & RFD_STATUS_EL, "No marked frame" );
    
    if ( 0 == (p_rfd->status & RFD_STATUS_C) )
        return;
        
    total_len = p_rfd->count;
    
#ifdef DEBUG_82559
    os_printf("Rx %d %x (status %x): %d sg's, %d bytes\n",
              p_i82559->index, (int)p_i82559, p_rfd->status, sg_len, total_len);
#endif

    // Copy the data to the network stack
    from_p = &p_rfd->buffer[0];

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

        if ( 0 == l || 0 == to_p )
            return; // caller was out of mbufs

        if ( l > total_len )
            l = total_len;

        memcpy( to_p, (unsigned char *)from_p, l );
        from_p += l;
        total_len -= l;
    }

    CYG_ASSERT( 0 == total_len, "total_len mismatch in rx" );
    CYG_ASSERT( last_sg == sg_list, "sg count mismatch in rx" );
    CYG_ASSERT( &p_rfd->buffer[0] < from_p, "from_p wild in rx" );
    CYG_ASSERT( &p_rfd->buffer[0] + MAX_RX_PACKET_SIZE >= from_p,
                "from_p overflow in rx" );
}    


// ------------------------------------------------------------------------
//
//  Function : InitTxRing
//
// ------------------------------------------------------------------------
static void InitTxRing(struct i82559* p_i82559)
{
    int i;
    cyg_uint32 ioaddr;

#ifdef DEBUG_82559
    os_printf("InitTxRing %d\n", p_i82559->index);
#endif
    ioaddr = p_i82559->io_address;
    for ( i = 0; i < MAX_TX_DESCRIPTORS; i++) {
        p_i82559->tx_ring[i] = (TxCB *)pciwindow_mem_alloc(
            sizeof(TxCB) + MAX_TX_PACKET_SIZE);
    }

    ResetTxRing(p_i82559);
}

// ------------------------------------------------------------------------
//
//  Function : ResetTxRing
//
// ------------------------------------------------------------------------
static void ResetTxRing(struct i82559* p_i82559)
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

        p_txcb->status = 0;
        p_txcb->command = 0;
        p_txcb->link = VIRT_TO_BUS((cyg_uint32)p_txcb);
        p_txcb->tbd_address = 0xFFFFFFFF;
        p_txcb->tbd_number = 0;
        p_txcb->tx_threshold = 16;
        p_txcb->eof = 1;
        p_txcb->count = 0;
        p_i82559->tx_keys[i] = 0;
    }
    
    wait_for_cmd_done(ioaddr);
    OUTL(0, ioaddr + SCBPointer);
    OUTW(SCB_M | CU_ADDR_LOAD, ioaddr + SCBCmd);
}

// ------------------------------------------------------------------------
//
//  Function : TxMachine          (Called from FG & ISR)
//
// This steps the Tx Machine onto the next record if necessary - allowing
// for missed interrupts, and so on.
// ------------------------------------------------------------------------

static void TxMachine(struct i82559* p_i82559)
{
    int tx_descriptor_active;
    cyg_uint32 ioaddr;

    tx_descriptor_active = p_i82559->tx_descriptor_active;
    ioaddr = p_i82559->io_address;  
    
    // See if the CU is idle when we think it isn't:
    // (Recovers from a dropped interrupt)
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
        TxCB *p_txcb;
        p_txcb = p_i82559->tx_ring[tx_descriptor_active];
        CYG_ASSERT( (cyg_uint8 *)p_txcb >= i82559_heap_base, "txcb under" );
        CYG_ASSERT( (cyg_uint8 *)p_txcb <  i82559_heap_free, "txcb over" );
#ifdef DEBUG_82559
        os_printf("Tx %d %x: Starting Engines, KEY %x\n",
                  p_i82559->index, (int)p_i82559, key );
#endif
        // make sure no command operating
        wait_for_cmd_done(ioaddr); 
        // start Tx operation
        OUTL(VIRT_TO_BUS(p_txcb), ioaddr + SCBPointer);
        OUTW(CU_START, ioaddr + SCBCmd);
        p_i82559->tx_in_progress = 1;
    }
}

// ------------------------------------------------------------------------
//
//  Function : TxDone          (Called from DSR)
//
// This returns Tx's from the Tx Machine to the stack (ie. reports
// completion) - allowing for missed interrupts, and so on.
// ------------------------------------------------------------------------

static void TxDone(struct i82559* p_i82559)
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
             ( p_i82559->tx_queue_full &&
              (0 != p_i82559->tx_ring[ tx_descriptor_remove ]->status) ) ) {
        unsigned long key = p_i82559->tx_keys[ tx_descriptor_remove ];
#ifdef DEBUG_82559
        os_printf("TxDone %d %x: KEY %x\n",
                  p_i82559->index, (int)p_i82559, key );
#endif
        eth_drv_tx_done( sc, key, 1 /* status */ );
        
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
//    return 1;

    struct i82559 *p_i82559;

    p_i82559 = (struct i82559 *)sc->driver_private;

    IF_BAD_82559( p_i82559 ) {
        os_printf( "i82559_send: Bad device pointer %x\n", p_i82559 );
        return 0;
    }
    
    // Advance TxMachine atomically
    Mask82559Interrupt(p_i82559);
    TxMachine(p_i82559);
    Acknowledge82559Interrupt(p_i82559);
    UnMask82559Interrupt(p_i82559);

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
    int tx_descriptor_add;
    TxCB *p_txcb;
    cyg_uint32 ioaddr;

    p_i82559 = (struct i82559 *)sc->driver_private;

    IF_BAD_82559( p_i82559 ) {
        os_printf( "i82559_send: Bad device pointer %x\n", p_i82559 );
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
        os_printf( "i82559_send: Queue full, device %x, key %x\n",
                   p_i82559, key );
    }
    else {
        struct eth_drv_sg *last_sg;
        volatile cyg_uint8 *to_p;

        tx_descriptor_add = p_i82559->tx_descriptor_add;

        p_i82559->tx_keys[tx_descriptor_add] = key;

        p_txcb = p_i82559->tx_ring[tx_descriptor_add];

        CYG_ASSERT( (cyg_uint8 *)p_txcb >= i82559_heap_base, "txcb under" );
        CYG_ASSERT( (cyg_uint8 *)p_txcb <  i82559_heap_free, "txcb over" );

        p_txcb->status = 0;
        p_txcb->command = TxCB_CMD_TRANSMIT | TxCB_CMD_S
                                | TxCB_CMD_I | TxCB_CMD_EL;
        p_txcb->link = VIRT_TO_BUS((cyg_uint32)p_txcb);
        p_txcb->tbd_address = 0xFFFFFFFF;
        p_txcb->tbd_number = 0;
        p_txcb->tx_threshold = 16;
        p_txcb->eof = 1;
        p_txcb->count = total_len;

        // Copy from the sglist into the txcb
        to_p = &p_txcb->buffer[0];

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
        CYG_ASSERT( &p_txcb->buffer[0] < to_p, "to_p wild in tx" );
        CYG_ASSERT( &p_txcb->buffer[0] + MAX_TX_PACKET_SIZE >= to_p,
                    "to_p overflow in tx" );
  
        // Next descriptor
        if ( ++tx_descriptor_add >= MAX_TX_DESCRIPTORS)
            tx_descriptor_add = 0;
        p_i82559->tx_descriptor_add = tx_descriptor_add;

        if ( p_i82559->tx_descriptor_remove == tx_descriptor_add )
            p_i82559->tx_queue_full = 1;
    }

    // Try advancing the Tx Machine regardless

    // no more interrupts until started
    Mask82559Interrupt(p_i82559);

    // Check that either:
    //     tx is already active, there is other stuff queued,
    // OR  this tx just added is the current active one.
    CYG_ASSERT( (p_i82559->tx_in_progress == 1) ||
       ((p_i82559->tx_descriptor_add-1) == p_i82559->tx_descriptor_active)
    || ((0 == p_i82559->tx_descriptor_add) &&
        ((MAX_TX_DESCRIPTORS-1) == p_i82559->tx_descriptor_active)),
                "Active/add mismatch" );

    // Advance TxMachine atomically
    TxMachine(p_i82559);
    Acknowledge82559Interrupt(p_i82559);
    UnMask82559Interrupt(p_i82559);
}

// ------------------------------------------------------------------------
//
//  Function : i82559_reset
//
// ------------------------------------------------------------------------
static void i82559_reset(struct i82559* p_i82559)
{
    cyg_uint32 ioaddr;
    int count;

    ioaddr = p_i82559->io_address;
    // make sure no command operating
    wait_for_cmd_done(ioaddr);   
 
    OUTL(I82559_SELECTIVE_RESET, ioaddr + SCBPort);
  
    for (count = 10 ; count-- ; ) {
        udelay(1000);
    }

    OUTL(I82559_RESET, ioaddr + SCBPort);
    
    for (count = 10 ; count-- ; ) {
      udelay(1000);
    }
}


// ------------------------------------------------------------------------
//
//                       INTERRUPT HANDLERS
//
// ------------------------------------------------------------------------

static cyg_uint32 eth_isr(cyg_vector_t vector, cyg_addrword_t data)
{
    struct i82559* p_i82559 = (struct i82559 *)data;
    cyg_uint16 status;
    cyg_uint32 ioaddr;

    IF_BAD_82559( p_i82559 ) {
        os_printf( "i82559_isr: Bad device pointer %x\n", p_i82559 );
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
static int mux_device_index = 0;

static cyg_uint32 eth_mux_isr(cyg_vector_t vector, cyg_addrword_t data)
{
    int device_index = mux_device_index;
    struct i82559* p_i82559;

    mux_device_index ^= 1; // look at the other one first next time.

    do {
        p_i82559 = &i82559[device_index];
        if ( p_i82559->active )
            (void)eth_isr( vector, (cyg_addrword_t)p_i82559 );
        device_index ^= 1;
    } while ( device_index == mux_device_index );

    return CYG_ISR_CALL_DSR;
}

// ------------------------------------------------------------------------

void eth_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    struct i82559* p_i82559 = (struct i82559 *)data;

    // First pass any rx data up the stack
    PacketRxReady(p_i82559);

    // Then scan for completed Txen and inform the stack
    TxDone(p_i82559);
}


// ------------------------------------------------------------------------
void eth_mux_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    int device_index = mux_device_index;
    struct i82559* p_i82559;

    mux_device_index ^= 1; // look at the other one first next time.
                           // (non-atomicity wrt IRQ does not matter)
    do {
        p_i82559 = &i82559[device_index];
        if ( p_i82559->active )
            eth_dsr( vector, count, (cyg_addrword_t)p_i82559 );
        device_index ^= 1;
    } while ( device_index == mux_device_index );
}

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

    // MUX interrupt - special case when 2 cards share one intr.
    static cyg_handle_t mux_interrupt_handle = 0;
    static cyg_interrupt mux_interrupt_object;

#ifdef DEBUG
    db_printf("pci_init_find_82559s()\n");
#endif

    // allocate memory to be used in ioctls later
    if (mem_reserved_ioctl != (void*)0) {
        db_printf("pci_init_find_82559s() called > once\n");
        return 0;
    }

    // First initialize the heap in PCI window'd memory
    i82559_heap_size = CYGHWR_HAL_ARM_EBSA285_PCI_MEM_MAP_SIZE;
    i82559_heap_base = (cyg_uint8 *)CYGHWR_HAL_ARM_EBSA285_PCI_MEM_MAP_BASE;
    i82559_heap_free = i82559_heap_base;

    mem_reserved_ioctl = pciwindow_mem_alloc(MAX_MEM_RESERVED_IOCTL);     

    cyg_pci_init();
#ifdef DEBUG
    db_printf("Finished cyg_pci_init();\n");
#endif
    devid = CYG_PCI_NULL_DEVID;

    for (device_index = 0; device_index < MAX_82559; device_index++) {
        struct i82559 *p_i82559 = &i82559[device_index];
        p_i82559->index = device_index;

        if (cyg_pci_find_device(0x8086, 0x1229, &devid) ) {
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

                // ALSO attach it to interrupt #18 for multiplexed
                // interrupts.  This is for certain boards where the
                // PCI backplane is wired "straight through" instead of
                // with a rotation of interrupt lines in the different
                // slots.
                if ( ! mux_interrupt_handle ) {
#ifdef DEBUG
                    db_printf(" Also attaching to HAL vector %d\n", 
                              CYGNUM_HAL_INTERRUPT_PCI_IRQ);
#endif
                    cyg_drv_interrupt_create(
                        CYGNUM_HAL_INTERRUPT_PCI_IRQ,
                        0,              // Priority - unused
                        0,              // Data item passed to ISR (not used)
                        eth_mux_isr,    // ISR
                        eth_mux_dsr,    // DSR
                        &mux_interrupt_handle,
                        &mux_interrupt_object );
                    
                    cyg_drv_interrupt_attach(mux_interrupt_handle);
                }
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
                cyg_pci_read_config_uint16(dev_info.devid, CYG_PCI_CFG_COMMAND, &cmd);
                cmd |= CYG_PCI_CFG_COMMAND_IO // enable I/O space
                    | CYG_PCI_CFG_COMMAND_MEMORY // enable memory space
                    | CYG_PCI_CFG_COMMAND_MASTER; // enable bus master
                cyg_pci_write_config_uint16(dev_info.devid, CYG_PCI_CFG_COMMAND, cmd);

                // Now the PCI part of the device is configured, reset it. This 
                // should make it safe to enable the interrupt
                i82559_reset(p_i82559);

                if (p_i82559->vector != 0) {
                    cyg_interrupt_acknowledge(p_i82559->vector);
                    cyg_drv_interrupt_unmask(p_i82559->vector);
                }
#ifdef DEBUG
                db_printf(" **** Device enabled for I/O and Memory and Bus Master\n");
#endif
            }
            else {
                p_i82559->found = 0;
                p_i82559->active = 0;
                db_printf("Failed to configure device %d\n",device_index);
            }
        }
        else {
            p_i82559->found = 0;
            p_i82559->active = 0;
            db_printf("eth%d not found\n", device_index);
        }
    }

    // Now enable the mux shared interrupt if it is in use
    if (mux_interrupt_handle) {
        cyg_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_PCI_IRQ);
        cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_PCI_IRQ);
    }

    return 1;
}


// ------------------------------------------------------------------------
// We use this as a templete when writing a new MAC address into the
// eeproms. The MAC address in the first few bytes is over written
// with the correct MAC address and then the whole lot is programmed
// into the serial EEPROM. The checksum is calculated on the fly and
// sent instead of the last two bytes.

static char eeprom_burn[126] = { 
  0x00, 0x90, 0x27, 0x8c, 0x57, 0x82, 0x03, 0x02, 0x00, 0x00, 0x01,
  0x02, 0x01, 0x47, 0x00, 0x00, 0x13, 0x72, 0x06, 0x83, 0xa2, 0x40,
  0x0c, 0x00, 0x86, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00 
};

// ------------------------------------------------------------------------
//
//  Function : eth_set_promiscuous_mode
//
//  Return : 0 = It worked.
//           non0 = It failed.
// ------------------------------------------------------------------------

static int eth_set_promiscuous_mode(struct i82559* p_i82559)
{
    cyg_uint32  ioaddr;
    volatile CONFIG_CMD_STRUCT *ccs;

    IF_BAD_82559( p_i82559 ) {
        os_printf( "eth_set_promiscuos_mode: Bad device pointer %x\n",
                   p_i82559 );
        return -1;
    }

    ioaddr = p_i82559->io_address;  
    wait_for_cmd_done(ioaddr); 
    // load cu base address = 0 */ 
    OUTL(0, ioaddr + SCBPointer);         
    // 32 bit linear addressing used
                                        
    OUTW(SCB_M | CU_ADDR_LOAD, ioaddr + SCBCmd);
    // wait for SCB command complete
    wait_for_cmd_done(ioaddr);   
  
    ccs = (CONFIG_CMD_STRUCT *)mem_reserved_ioctl;
  
    // Check the malloc we did earlier worked
    if (ccs == (void*)0) 
        return 2; // Failed
  
    ccs->cb_entry.cb_cmd=0x2;
    ccs->cb_entry.cb_cmd_word=0x0;
    ccs->cb_entry.cb_status_word=0x0;
    ccs->cb_entry.cb_int=0;
    ccs->cb_entry.cb_suspend=1;
    ccs->cb_entry.cb_el=1;
    ccs->cb_entry.cb_complete=0;
    ccs->cb_entry.cb_link_offset=VIRT_TO_BUS((cyg_uint32)&ccs);
    
    // Default values from the Intel Manual
    ccs->config_bytes[0]=0x13;
    ccs->config_bytes[1]=0x8;
    ccs->config_bytes[2]=0x0;
    ccs->config_bytes[3]=0x0;
    ccs->config_bytes[4]=0x0;
    ccs->config_bytes[5]=0x0;
    ccs->config_bytes[6]=0xb2;
    ccs->config_bytes[7]=0x0;
    ccs->config_bytes[8]=0x0;
    ccs->config_bytes[9]=0x0;
    ccs->config_bytes[10]=0x28;
    ccs->config_bytes[11]=0x0;
    ccs->config_bytes[12]=0x60;
    ccs->config_bytes[13]=0x0;            // arp
    ccs->config_bytes[14]=0x0;            // arp
    
    ccs->config_bytes[15]=0x81;           // promiscuous mode set
    
    ccs->config_bytes[16]=0x0;
    ccs->config_bytes[17]=0x40;
    ccs->config_bytes[18]=0x70;
    
    // wait for SCB command complete
    wait_for_cmd_done(ioaddr);   
    
    OUTL(VIRT_TO_BUS(ccs), ioaddr + SCBPointer); 
    OUTW(SCB_M | CU_START, ioaddr + SCBCmd);    
  
    // now check for result ...
    wait_for_cmd_done(ioaddr);   
  
    if ( (!ccs->cb_entry.cb_ok) || (!ccs->cb_entry.cb_complete) )
        return 1; // Failed
    return 0; // OK
}

// ------------------------------------------------------------------------
//
//  Function : eth_set_mac_address
//
//  Return : 0 = It worked.
//           non0 = It failed.
// ------------------------------------------------------------------------
static int eth_set_mac_address(struct i82559* p_i82559, char *addr)
{
    int addr_length;
  
    cyg_uint32  ioaddr;
    volatile CONFIG_CMD_STRUCT *ccs;
  
    IF_BAD_82559( p_i82559 ) {
        os_printf( "eth_set_mac_address : Bad device pointer %x\n",
                   p_i82559 );
        return -1;
    }

    ioaddr = p_i82559->io_address;      
    
    wait_for_cmd_done(ioaddr); 
    // load cu base address = 0 */ 
    OUTL(0, ioaddr + SCBPointer);       
    // 32 bit linear addressing used
                                        
    OUTW(SCB_M | CU_ADDR_LOAD, ioaddr + SCBCmd);
    // wait for SCB command complete
    wait_for_cmd_done(ioaddr); 
    
    ccs = (CONFIG_CMD_STRUCT *)mem_reserved_ioctl;
    if (ccs == (void*)0)
        return 2;

    ccs->cb_entry.cb_cmd=0x1;
    ccs->cb_entry.cb_cmd_word=0x0;
    ccs->cb_entry.cb_status_word=0x0;
    ccs->cb_entry.cb_int=0;
    ccs->cb_entry.cb_suspend=1;
    ccs->cb_entry.cb_el=1;
  
    memcpy((char *)(ccs->config_bytes),addr,6);

    ccs->config_bytes[6]=0x0;
    ccs->config_bytes[7]=0x0;
  
    ioaddr = p_i82559->io_address;  
  
    OUTL(VIRT_TO_BUS(ccs), ioaddr + SCBPointer); 
    OUTW(SCB_M | CU_START, ioaddr + SCBCmd);    
    // Next delay seems to be required, otherwise,
    // cb_ok/cb_complete won't be set later.

    udelay(100);
    wait_for_cmd_done(ioaddr);   
  
    // now check for result ...
    if ( (!ccs->cb_entry.cb_ok) || (!ccs->cb_entry.cb_complete) )
        return 3;
  
    // now set this address in the device eeprom ....
    (void)memcpy(eeprom_burn,addr,6);
    eeprom_burn[20] &= 0xfe;   
    eeprom_burn[20] |= p_i82559->index;   

    addr_length=6; // eeprom address word length in bits ...
    program_eeprom(ioaddr,addr_length,eeprom_burn);
   
    // now update netword stack ...
    // FIXME - no feedback to stack when MAC changes

    // update 82559 driver data structure ...
    (void)memcpy((char *)(p_i82559->mac_address),addr,6);
    p_i82559->mac_addr_ok = 1;
    return 0;
}

// ------------------------------------------------------------------------
//
//  Function : write_enable_eeprom
//
// ------------------------------------------------------------------------
static int write_enable_eeprom(long ioaddr,  int addr_len)
{
    int ee_addr = ioaddr + SCBeeprom;
    int write_en_cmd = EE_WRITE_EN_CMD; 
    int i;
  
    OUTW(EE_ENB & ~EE_CS, ee_addr);
    eeprom_delay(1);
    OUTW(EE_ENB, ee_addr);
    eeprom_delay(1);

  
    // Shift the wr/er enable command bits out.
    for (i = (addr_len+2); i >= 0; i--) {
        short dataval = (write_en_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
        OUTW(EE_ENB | dataval, ee_addr);
        eeprom_delay(1);
        OUTW(EE_ENB | dataval | EE_SHIFT_CLK, ee_addr);
        eeprom_delay(1);
    }

    // Terminate the EEPROM access.
    OUTW(EE_ENB & ~EE_CS, ee_addr);
    eeprom_delay(1);
    return 0;
}

// ------------------------------------------------------------------------
//
//  Function : write_eeprom
//
//  Return : 0 = It worked.
//           1 = It failed.
// ------------------------------------------------------------------------
static int write_eeprom(long ioaddr, int location,
                 int addr_len, unsigned short value)
{
    unsigned short retval = 0;
    int ee_addr = ioaddr + SCBeeprom;
    int write_cmd = location | EE_WRITE_CMD; 
    int i;
  
    OUTW(EE_ENB & ~EE_CS, ee_addr);
    eeprom_delay(1);
    OUTW(EE_ENB, ee_addr);
    eeprom_delay(1);

    // Shift the write command bits out.
    for (i = (addr_len+2); i >= 0; i--) {
        short dataval = (write_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
        OUTW(EE_ENB | dataval, ee_addr);
        eeprom_delay(1);
        OUTW(EE_ENB | dataval | EE_SHIFT_CLK, ee_addr);
        eeprom_delay(1);
    }
    OUTW(EE_ENB, ee_addr);
    eeprom_delay(1);
    
    for (i = 15; i >= 0; i--) {
        short dataval = (value & (1 << i)) ? EE_DATA_WRITE : 0;
        OUTW(EE_ENB | dataval, ee_addr);
        eeprom_delay(1);
        OUTW(EE_ENB | dataval | EE_SHIFT_CLK, ee_addr);
        eeprom_delay(1);
    }

    // Terminate the EEPROM access.
    OUTW(EE_ENB & ~EE_CS, ee_addr);
    eeprom_delay(10000);
    return retval;
}

// ------------------------------------------------------------------------
//
//  Function : Program EEPROM
//  Parameters : ioaddr
//               addr_length in bits (8 or 6)
//               char[128] data to program
//  Return : void
// ------------------------------------------------------------------------
static void program_eeprom(cyg_uint32 ioaddr,
                    cyg_uint32 eeprom_size,
                    cyg_uint8 * data)
{
    cyg_uint32 i;
    cyg_uint16 checksum = 0;
    cyg_uint16 value;
    cyg_uint16 * cyg_uint16_data = (cyg_uint16 *) data;

    // first enable erase/write operations on the eeprom ...
    // this is done through the EWEN instruction ...

    write_enable_eeprom(ioaddr,eeprom_size);
  
    // Now write the bytes, except the checksum
    for (i=0 ; i< 63 ; i++) {
        value = ((unsigned short*)cyg_uint16_data)[i];
        checksum += value;
        write_eeprom(ioaddr,i,eeprom_size,value);
    }
  
    // Work out the checksum and program it
    value = 0xBABA - checksum;
    write_eeprom(ioaddr,i,eeprom_size,value);
}

// ------------------------------------------------------------------------
//
//  Function : eth_get_mac_address
//
// ------------------------------------------------------------------------
#ifdef ETH_DRV_GET_MAC_ADDRESS
static int eth_get_mac_address(struct i82559* p_i82559, char *addr)
{
    IF_BAD_82559( p_i82559 ) {
        os_printf( "eth_get_mac_address : Bad device pointer %x\n",
                   p_i82559 );
        return -1;
    }

    memcpy( addr, (char *)(&p_i82559->mac_address[0]), 6 );
    return (0);
}
#endif
// ------------------------------------------------------------------------
//
//  Function : i82559_ioctl
//
// ------------------------------------------------------------------------
static int i82559_ioctl(struct eth_drv_sc *sc, unsigned long key,
                        void *data, int data_length)
{
    struct i82559 *p_i82559;

    p_i82559 = (struct i82559 *)sc->driver_private;

    IF_BAD_82559( p_i82559 ) {
        os_printf( "i82559_ioctl/control: Bad device pointer %x\n", p_i82559 );
        return -1;
    }

    switch ( key ) {

#ifdef ETH_DRV_SET_MAC_ADDRESS
    case ETH_DRV_SET_MAC_ADDRESS:
        if ( 6 != data_length )
            return -2;
        eth_set_mac_address( p_i82559, data );
        return 0;
#endif

#ifdef ETH_DRV_GET_MAC_ADDRESS
    case ETH_DRV_GET_MAC_ADDRESS:
        eth_get_mac_address( p_i82559, data );
        return 0;
#endif

    default:
        break;
    }
    return -1;
}

// ------------------------------------------------------------------------
//
//
//           CODE FOR DEBUGGING PURPOSES ONLY
//
//
// ------------------------------------------------------------------------
void dump_txcb(TxCB *p_txcb)
{
    os_printf("TxCB @ %x\n", (int)p_txcb);
    os_printf("status = %04X ", p_txcb->status);
    os_printf("command = %04X ", p_txcb->command);
    os_printf("link = %08X ", p_txcb->link);
    os_printf("tbd = %08X ", p_txcb->tbd_address);
    os_printf("count = %d ", p_txcb->count);
    os_printf("eof = %x ", p_txcb->eof);
    os_printf("threshold = %d ", p_txcb->tx_threshold);
    os_printf("tbd number = %d\n", p_txcb->tbd_number);
}


// This is intended to be the body of a THREAD that prints stuff every 10
// seconds or so:
#ifdef KEEP_STATISTICS
#ifdef DISPLAY_STATISTICS
void DisplayStatistics(void)
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
                // make sure no command operating
            	wait_for_cmd_done(i82559[i].io_address);
                // start register dump
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

void dump_rfd(RFD *p_rfd, int anyway )
{
    if ( (0 != p_rfd->status) || anyway ) {
        os_printf("RFD @ %x = ", (int)p_rfd);
        os_printf("status = %x ", p_rfd->status);
        os_printf("link = %x ", p_rfd->link);
//        os_printf("rdb_address = %x ", p_rfd->rdb_address);
        os_printf("count = %x ", p_rfd->count);
        os_printf("f = %x ", p_rfd->f);
        os_printf("eof = %x ", p_rfd->eof);
        os_printf("size = %x\n", p_rfd->size);
        os_printf("[%04x %04x %04x] ",
                  *((cyg_uint16 *)(&(p_rfd->buffer[0]))),
                  *((cyg_uint16 *)(&(p_rfd->buffer[2]))),
                  *((cyg_uint16 *)(&(p_rfd->buffer[4]))) );
        os_printf("[%04x %04x %04x] %04x : ",          
                  *((cyg_uint16 *)(&(p_rfd->buffer[6]))),
                  *((cyg_uint16 *)(&(p_rfd->buffer[8]))),
                  *((cyg_uint16 *)(&(p_rfd->buffer[10]))),
                  *((cyg_uint16 *)(&(p_rfd->buffer[12]))) );
        os_printf("(%04x %04x %04x %04x) ",            
                  *((cyg_uint16 *)(&(p_rfd->buffer[14]))),
                  *((cyg_uint16 *)(&(p_rfd->buffer[16]))),
                  *((cyg_uint16 *)(&(p_rfd->buffer[18]))),
                  *((cyg_uint16 *)(&(p_rfd->buffer[20]))) );
        os_printf("[%04x %04x %04x] ",                 
                  *((cyg_uint16 *)(&(p_rfd->buffer[22]))),
                  *((cyg_uint16 *)(&(p_rfd->buffer[24]))),
                  *((cyg_uint16 *)(&(p_rfd->buffer[26]))) );
        os_printf("%d.%d.%d.%d ",
                  *((cyg_uint8  *)(&(p_rfd->buffer[28]))),
                  *((cyg_uint8  *)(&(p_rfd->buffer[29]))),
                  *((cyg_uint8  *)(&(p_rfd->buffer[30]))),
                  *((cyg_uint8  *)(&(p_rfd->buffer[31]))) );
        os_printf("[%04x %04x %04x] ",                 
                  *((cyg_uint16 *)(&(p_rfd->buffer[32]))),
                  *((cyg_uint16 *)(&(p_rfd->buffer[34]))),
                  *((cyg_uint16 *)(&(p_rfd->buffer[36]))) );
        os_printf("%d.%d.%d.%d ...\n",
                  *((cyg_uint8  *)(&(p_rfd->buffer[38]))),
                  *((cyg_uint8  *)(&(p_rfd->buffer[39]))),
                  *((cyg_uint8  *)(&(p_rfd->buffer[40]))),
                  *((cyg_uint8  *)(&(p_rfd->buffer[41]))) );
    }
}

void dump_all_rfds( int intf )
{
    struct i82559* p_i82559 = &i82559[intf];
    int i, j;
    j = p_i82559->next_rx_descriptor;
    os_printf("rx descriptors for interface %d (eth%d):\n", intf, intf );
    for ( i = 0; i < MAX_RX_DESCRIPTORS; i++ )
        dump_rfd( p_i82559->rx_ring[i], (i > (j-3) && (i <= j)) );
    os_printf("next rx descriptor = %x\n\n", j);
}


void dump_packet(cyg_uint8 *p_buffer, int length)
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

// EOF if_ebsa285.c
