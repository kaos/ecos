//==========================================================================
//
//      ks5000_ether.c
//
//      
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas, jskov
//               Grant Edwards <grante@visi.com>
// Date:         2001-07-31
// Purpose:      
// Description:  
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/devs_eth_arm_ks32c5000.h>
#if defined(CYGPKG_IO)
#include <pkgconf/io.h>
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#else
// need to prvide fake values for errno
#define EIO 1
#define EINVAL 2
#endif

#include <cyg/infra/diag.h>
#include <cyg/io/eth/netdev.h>
#include <cyg/io/eth/eth_drv.h>
#include <cyg/io/eth/eth_drv_stats.h>
#include <cyg/hal/hal_intr.h>

#if defined(CYGPKG_REDBOOT)
#include <pkgconf/redboot.h>
#endif

#if !defined(CYGPKG_NET)
#define cyg_drv_interrupt_unmask(v) /* noop */
#define cyg_drv_interrupt_mask(v)   /* noop */
#define cyg_drv_isr_lock()          /* noop */
#define cyg_drv_isr_unlock()        /* noop */
#define cyg_drv_mutex_init(m)       /* noop */
#define cyg_drv_mutex_lock(m)       /* noop */
#define cyg_drv_mutex_unlock(m)     /* noop */
#define cyg_drv_dsr_lock()          /* noop */
#define cyg_drv_dsr_unlock()        /* noop */
#endif

#define HavePHY 1
#define HavePHYinterrupt 0

#include "std.h"
#include "ks5000_regs.h"
#include "ks5000_ether.h"

#if HavePHY
#include "phy.h"
#endif

#if 1
#define debug_printf(args...) diag_printf(args)
#else
#define debug_printf(args...) /* noop */
#endif
#define Bit(n) (1<<(n))

// enable/disable software verification of rx CRC
// should be moved to user-controlled valud in CDL file

#if defined(CYG_HAL_CPUTYPE_KS32C5000A)
#define SoftwareCRC 1
#else
#define SoftwareCRC 0
#endif

#if HavePHY
// functions to read/write Phy chip registers via MII interface
// on 32c5000.  These need to be non-static since they're used
// by PHY-specific routines in a different file.
#define PHYREGWRITE	0x0400
#define MiiStart       0x0800

void MiiStationWrite(U32 RegAddr, U32 PhyAddr, U32 PhyWrData)
{
  STADATA = PhyWrData ;
  STACON = RegAddr | PhyAddr |  MiiStart | PHYREGWRITE ;
  while (STACON & MiiStart)  
    ;
  //debug_printf("PHY Wr %x:%02x := %04x\n",PhyAddr, RegAddr, PhyWrData) ;
}

U32 MiiStationRead(U32 RegAddr, U32 PhyAddr)
{
  U32	PhyRdData;
  STACON = RegAddr | PhyAddr |  MiiStart;
  while (STACON & MiiStart)
    ;
  PhyRdData = STADATA;
  //debug_printf("PHY Rd %x:%02x  %04x\n",PhyAddr,RegAddr,PhyRdData) ;
  return PhyRdData ;
}
#endif

// miscellaneous data structures

typedef  BYTE  ETH_ADDR[6] __attribute__((packed));

typedef  struct tagETH_HEADER 
{
  ETH_ADDR daddr __attribute__((packed));
  ETH_ADDR saddr __attribute__((packed));
  WORD type      __attribute__((packed));
} ETH_HEADER __attribute__((packed));

#define ETH_HEADER_SIZE   14

// Tx/Rx common descriptor structure
typedef  struct tagFRAME_DESCRIPTOR 
{
  LWORD FrameDataPtr;
  LWORD Reserved;   /*  cf: RX-reserved, TX-Reserved(25bits) + Control bits(7bits) */
  LWORD StatusAndFrameLength;
  struct tagFRAME_DESCRIPTOR   *NextFD;
} FRAME_DESCRIPTOR;

typedef struct
{
  U8 DestinationAddr[6];
  U8 SourceAddr[6];
  U8 LengthOrType[2];
  U8 LLCData[1506];
} MAC_FRAME;

#if defined(CYGPKG_NET)
static cyg_drv_mutex_t txMutex;
struct ether_drv_stats ifStats;
#endif

typedef struct
{
  LWORD BTxNLErr;
  LWORD BTxNOErr;
  LWORD BTxEmptyErr;
} BDMA_TX_ERR;

typedef struct
{
  LWORD BRxNLErr;
  LWORD BRxNOErr;
  LWORD BRxMSOErr;
  LWORD BRxEmptyErr;
  LWORD sBRxSEarly;
  LWORD noBufferAvail;
  LWORD queueOverflow;
  LWORD bad;
} BDMA_RX_ERR;


// interrupt entry counters
U32 ks5000_MAC_Rx_Cnt;
U32 ks5000_MAC_Tx_Cnt;
U32 ks5000_MAC_Phy_Cnt;
U32 ks5000_BDMA_Tx_Isr_Cnt;
U32 ks5000_BDMA_Tx_Dsr_Cnt;
U32 ks5000_BDMA_Rx_Isr_Cnt;
U32 ks5000_BDMA_Rx_Dsr_Cnt;


// packet and byte counters
static U32 MAC_Tx_Pkts;
static U32 MAC_Tx_Octets;
// static U32 BDMA_Rx_Pkts;
// static U32 BDMA_Rx_Octets;

// configuration values
static volatile U32 MACConfigVar;
static volatile U32 CAMConfigVar = CAMCON_COMP_EN | CAMCON_BROAD_ACC;
static volatile U32 MACTxConfigVar = 
  /* MACTXCON_EN_UNDER | */ 
  MACTXCON_EN_DEFER |
  MACTXCON_EN_NCARR | 
  MACTXCON_EN_EXCOLL |
  MACTXCON_EN_LATE_COLL | 
  MACTXCON_ENTX_PAR |
  MACTXCON_EN_COMP;
static volatile U32 MACRxConfigVar = 
  MACRXCON_RX_EN | 
  MACRXCON_EN_ALIGN |
  MACRXCON_EN_CRC_ERR | 
  MACRXCON_EN_OVER |
  MACRXCON_EN_LONG_ERR | 
  MACRXCON_EN_RX_PAR;

static volatile U32 BDMATxConfigVar =  
  BDMATXCON_MSL111 |
  BDMATXCON_STP_SKP | 
  3;   /* burst size - 1 */

#define EtherFramePadding 2

#if EtherFramePadding == 0
#define BDMARXCON_ALIGN BDMARXCON_WA00
#elif EtherFramePadding == 1
#define BDMARXCON_ALIGN BDMARXCON_WA01
#elif EtherFramePadding == 2
#define BDMARXCON_ALIGN BDMARXCON_WA10
#elif EtherFramePadding == 3
#define BDMARXCON_ALIGN BDMARXCON_WA11
#else
#error "EtherFramePadding must be 0,1,2 or 3"
#endif

static volatile U32 BDMARxConfigVar = 
  BDMARXCON_DIE | 
  BDMARXCON_EN |
  BDMARXCON_BIG | 
  BDMARXCON_MA_INC |
  BDMARXCON_NOIE | 
  BDMARXCON_ALIGN |
  BDMARXCON_STP_SKP | 
  15;  /* burst size - 1 */


/* Global variables For BDMA Error Report */

static BDMA_TX_ERR BDMATxErrCnt = {0,0,0};
static BDMA_RX_ERR BDMARxErrCnt = {0,0,0,0,0};


#if SoftwareCRC
static U32 crc32( unsigned char *blk_adr, unsigned blk_len);
#endif
static void Init_TxFrameDescriptorArray(void);
static void Init_RxFrameDescriptorArray(void);

// number of ethernet buffers should be enough to keep both rx
// and tx queues full plus some extras for in-process packets

#if defined(CYGPKG_REDBOOT)
#define NUM_ETH_BUFFERS 10
#define MAX_RX_FRAME_DESCRIPTORS        4     // Max number of Rx Frame Descriptors
#define MAX_TX_FRAME_DESCRIPTORS  	4     // Max number of Tx Frame Descriptors
#else
#define NUM_ETH_BUFFERS 80
#define MAX_RX_FRAME_DESCRIPTORS        32     // Max number of Rx Frame Descriptors
#define MAX_TX_FRAME_DESCRIPTORS  	32     // Max number of Tx Frame Descriptors
#endif

static FRAME_DESCRIPTOR _rxFrameDescrArray[MAX_RX_FRAME_DESCRIPTORS] __attribute__((aligned(16)));
static FRAME_DESCRIPTOR _txFrameDescrArray[MAX_TX_FRAME_DESCRIPTORS] __attribute__((aligned(16)));

/* define aliases that will set the no-cache bit */
#define rxFrameDescrArray ((FRAME_DESCRIPTOR*)(((unsigned)_rxFrameDescrArray)|0x4000000))
#define txFrameDescrArray ((FRAME_DESCRIPTOR*)(((unsigned)_txFrameDescrArray)|0x4000000))

static FRAME_DESCRIPTOR *rxReadPointer;
static FRAME_DESCRIPTOR *txDonePointer;
static FRAME_DESCRIPTOR *txWritePointer;

static cyg_drv_mutex_t oldRxMutex;
static cyg_drv_cond_t  oldRxCond;


static bool configDone;

/*----------------------------------------------------------------------
 * Data structures used to manage ethernet buffers
 */
#define MAX_ETH_FRAME_SIZE 1520
  
typedef struct tEthBufferTag
{
  unsigned char data[MAX_ETH_FRAME_SIZE+8];
  unsigned length;
  unsigned userData;
  struct tEthBufferTag *next;
  struct tEthBufferTag *prev;
}tEthBuffer;


typedef struct 
{
  tEthBuffer *head;
  tEthBuffer *tail;
}tEthBufQueue;

#define EmptyQueue {NULL,NULL}

static void ethBufQueueClear(tEthBufQueue *q)
{
  q->head = NULL;
  q->tail = NULL;
}

static tEthBuffer *ethBufQueueGet(tEthBufQueue *q)
{
  tEthBuffer *r;
  
  r = q->head;
  
  if (r)
    q->head = r->next;
  
  return r;
}

static void ethBufQueuePut(tEthBufQueue *q, tEthBuffer *b)
{
  b->next = NULL;
  
  if (!q->head)
    {
      q->head = b;
      q->tail = b;
    }
  else
    {
      q->tail->next = b;
      q->tail = b;
    }
}

#if 0
// not used at the moment
static bool ethBufQueueEmpty(tEthBufQueue *q)
{
  return q->head != NULL;
}
#endif

/*----------------------------------------------------------------------
 * Free pool and routines to manipulate it.
 */

static tEthBuffer __bufferPool[NUM_ETH_BUFFERS] __attribute__((aligned(16)));
#define bufferPool ((tEthBuffer*)((unsigned)__bufferPool|0x4000000))

static tEthBufQueue freeList;
static int freeCount;

// do not call from ISR routine
static void freeBuffer(tEthBuffer *b)
{
  cyg_drv_isr_lock();
  ++freeCount;
  ethBufQueuePut(&freeList,b);
  cyg_drv_isr_unlock();
}

static int allocFail;

void bufferListError(void)
{
  while (1)
    ;
}

// do not call from ISR routine
static tEthBuffer *allocBuffer(void)
{
  tEthBuffer *r;
  cyg_drv_isr_lock();
  r = ethBufQueueGet(&freeList);
  cyg_drv_isr_unlock();
  if (r)
    --freeCount;
  else
    {
      ++allocFail;
      if (freeCount)
        bufferListError();
    }
  return r;
}

// call only from ISR routine or init
static void isrFreeBuffer(tEthBuffer *b)
{
  ++freeCount;
  ethBufQueuePut(&freeList,b);
}

#if 0
// not used at the moment

// call only from ISR routine or init
static tEthBuffer *isrAllocBuffer(void)
{
  tEthBuffer *r;
  r = ethBufQueueGet(&freeList);
  if (r)
    --freeCount;
  else
    {
      ++allocFail;
      if (freeCount)
        bufferListError();
    }
  return r;
}
#endif

static void initFreeList(void)
{
  int i;
  ethBufQueueClear(&freeList);
  freeCount = 0;
  for (i=0; i<NUM_ETH_BUFFERS; ++i)
    isrFreeBuffer(bufferPool+i);
}


//----------------------------------------------------------------------
// queue a buffer for transmit
//
// returns true if buffer was queued.

static int ks32c5000_eth_buffer_send(tEthBuffer *buf)
{
#if defined(CYGPKG_NET)  
  while (!configDone)
    cyg_thread_delay(10);
#endif

  //diag_printf("Phy Status = %x\n",PhyStatus());
  
  if (txWritePointer->FrameDataPtr & FRM_OWNERSHIP_BDMA)
    {
      // queue is full!  make sure transmit is running
      BDMATXCON |= BDMATXCON_EN;
      MACTXCON |= MACTXCON_TX_EN;
      return 0;
    }
  
  cyg_drv_mutex_lock(&txMutex);

  // free old buffer if we need to
  
  cyg_drv_isr_lock();
  if (txWritePointer->FrameDataPtr)
    {
      freeBuffer((tEthBuffer*)txWritePointer->FrameDataPtr);
      txWritePointer->FrameDataPtr = 0;
    }
  cyg_drv_isr_unlock();

  MAC_Tx_Pkts += 1;
  MAC_Tx_Octets += buf->length;

  // fill in the packet descriptor

  txWritePointer->Reserved = (TXFDCON_PADDING_MODE | TXFDCON_CRC_MODE |
                    TXFDCON_SRC_ADDR_INC | TXFDCON_BIG_ENDIAN |
                    TXFDCON_WIDGET_ALIGN00 | TXFDCON_MAC_TX_INT_EN);
  
  txWritePointer->StatusAndFrameLength = buf->length;
  txWritePointer->FrameDataPtr = ((unsigned)buf | FRM_OWNERSHIP_BDMA);
  
  txWritePointer = txWritePointer->NextFD;
  
  cyg_drv_mutex_unlock(&txMutex);
    
  // start transmit

#if defined(CYGPKG_NET)  
  ++ifStats.tx_count;
#endif
  
  BDMATXCON |= BDMATXCON_EN;
  MACTXCON |= MACTXCON_TX_EN;
  return 1;
}


//======================================================================
// check to see if there's a frame waiting

static int rx_frame_avail(void)
{
  if (rxReadPointer->FrameDataPtr & FRM_OWNERSHIP_BDMA)
    {
      // queue is empty -- make sure Rx is running
      if (!(BDMARXCON & BDMARXCON_EN))
          {
            ++BDMARxErrCnt.queueOverflow;
            BDMARXCON |= BDMARXCON_EN;
          }
      return 0;
    }
  else
    return 1;
}


//======================================================================
// de-queue a receive buffer
static tEthBuffer *ks32c5000_eth_get_recv_buffer(void)
{
  unsigned RxStatusAndLength;
  tEthBuffer *RxBufPtr;
  tEthBuffer *emptyBuf;
#if SoftwareCRC  
  unsigned crc, crclen;
  int crcOK;
#else
# define crcOK 1
#endif

  while (1)
    {
      if (rxReadPointer->FrameDataPtr & FRM_OWNERSHIP_BDMA)
        {
          // queue is empty -- make sure Rx is running
          if (!(BDMARXCON & BDMARXCON_EN))
            {
              ++BDMARxErrCnt.queueOverflow;
              BDMARXCON |= BDMARXCON_EN;
            }
          return NULL;
        }
  
      RxBufPtr = (tEthBuffer*)rxReadPointer->FrameDataPtr;
      RxStatusAndLength = rxReadPointer->StatusAndFrameLength;
      
      // counting on short-circuit && evaluation below to only
      // allocate a fresh buffer if rx packet is good!!

#if defined (CYGPKG_NET)      
      ++ifStats.rx_count;
#endif      
      
#if SoftwareCRC
      crclen = (RxStatusAndLength & 0xffff) - 4;
      crc = crc32(RxBufPtr->data+2,crclen);
      crcOK = ((U08)(crc>>0) == RxBufPtr->data[2+crclen+0] &&
               (U08)(crc>>8) == RxBufPtr->data[2+crclen+1] &&
               (U08)(crc>>16) == RxBufPtr->data[2+crclen+2] &&
               (U08)(crc>>24) == RxBufPtr->data[2+crclen+3]);
#endif
      if ((RxStatusAndLength & (RXFDSTAT_GOOD<<16)) 
          && crcOK
          && (emptyBuf = allocBuffer()))
        {
          // good packet and we've got a fresh buffer to take
          // it's place in the receive queue
          rxReadPointer->FrameDataPtr = (unsigned)emptyBuf | FRM_OWNERSHIP_BDMA;
          rxReadPointer = rxReadPointer->NextFD;
          RxBufPtr->length = RxStatusAndLength & 0xffff;
#if defined(CYGPKG_NET)
          ++ifStats.rx_deliver;
#endif          
          return RxBufPtr;
        }
      else
        {
          // bad packet or out of buffers.  either way we
          // ignore this packet, and reuse the buffer
#if defined(CYGPKG_NET)
          if (RxStatusAndLength & (RXFDSTAT_GOOD<<16) && crcOK)
            ++ifStats.rx_resource;
          else
            ++ifStats.rx_crc_errors;
#endif          
          rxReadPointer->FrameDataPtr |= FRM_OWNERSHIP_BDMA;
          rxReadPointer = rxReadPointer->NextFD;
        }
    }
}

//======================================================================
static int EthInit(U08* mac_address)
{
  if (mac_address)
    debug_printf("EthInit(%02x:%02x:%02x:%02x:%02x:%02x)\n",
                mac_address[0],mac_address[1],mac_address[2],
                mac_address[3],mac_address[4],mac_address[5]);
  else
    debug_printf("EthInit(NULL)\n");

#if HavePHY  
  PhyReset();
#endif
  
  /* Set the initial condition of the BDMA. */
  BDMARXCON = BDMARXCON_RESET;
  BDMATXCON = BDMATXCON_RESET;
  BDMARXLSZ = MAX_ETH_FRAME_SIZE;

  BDMARXPTR = (U32)rxReadPointer;
  BDMATXPTR = (U32)txWritePointer;
  
  MACCON = MACON_SW_RESET;
  MACCON = MACConfigVar;
	 
  CAMCON = CAMConfigVar;
  
  // set up our MAC address

  if (mac_address)
    {
      *((volatile U32*)CAM_BaseAddr) = 
        (mac_address[0]<<24) |
        (mac_address[1]<<16) |
        (mac_address[2]<< 8) |
        (mac_address[3]<< 0);
      *((volatile U16*)(CAM_BaseAddr+4)) =
        (mac_address[4]<< 8) |
        (mac_address[5]<< 0);
    }
  
  // CAM Enable
  CAMEN = 0x0001;
  
  // update the Configuration of BDMA and MAC to begin Rx/Tx

  BDMARXCON = BDMARxConfigVar;
  MACRXCON = MACRxConfigVar;

  BDMATXCON = BDMATxConfigVar;
  MACTXCON =  MACTxConfigVar;

  diag_printf("ks32C5000 eth: %02x:%02x:%02x:%02x:%02x:%02x  ",
              *((volatile unsigned char*)CAM_BaseAddr+0),
              *((volatile unsigned char*)CAM_BaseAddr+1),
              *((volatile unsigned char*)CAM_BaseAddr+2),
              *((volatile unsigned char*)CAM_BaseAddr+3),
              *((volatile unsigned char*)CAM_BaseAddr+4),
              *((volatile unsigned char*)CAM_BaseAddr+5));

#if SoftwareCRC
  diag_printf("Software CRC\n");
#else
  diag_printf("Hardware CRC\n");
#endif
  
  return 0;
}

//======================================================================
static void Init_TxFrameDescriptorArray(void)
{
  FRAME_DESCRIPTOR *pFrameDescriptor;
  int i;
  
  // Each Frame Descriptor's frame data pointer points is NULL
  // if not in use, otherwise points to an ethBuffer

  pFrameDescriptor = txFrameDescrArray;
  
  for(i=0; i < MAX_TX_FRAME_DESCRIPTORS; i++) 
    {
      pFrameDescriptor->FrameDataPtr = 0;
      pFrameDescriptor->Reserved = 0;
      pFrameDescriptor->StatusAndFrameLength = 0;
      pFrameDescriptor->NextFD = pFrameDescriptor+1;
      pFrameDescriptor++;
    }

  // fix up the last pointer to loop back to the first

  txFrameDescrArray[MAX_TX_FRAME_DESCRIPTORS-1].NextFD = txFrameDescrArray;
  
  txDonePointer = txWritePointer = txFrameDescrArray;
  
  return;
}


//======================================================================
static void Init_RxFrameDescriptorArray(void)
{
  FRAME_DESCRIPTOR *pFrameDescriptor;
  int i;
  
  // Each Frame Descriptor's frame data pointer points to
  // an ethBuffer struct

  pFrameDescriptor = rxFrameDescrArray;
  
  for(i=0; i < MAX_RX_FRAME_DESCRIPTORS; i++) 
    {
      pFrameDescriptor->FrameDataPtr = ((unsigned)allocBuffer() | FRM_OWNERSHIP_BDMA);
      pFrameDescriptor->Reserved = 0;
      pFrameDescriptor->StatusAndFrameLength = 0;
      pFrameDescriptor->NextFD = pFrameDescriptor+1;
      pFrameDescriptor++;
    }
  
  // fix up the last pointer to loop back to the first

  rxFrameDescrArray[MAX_RX_FRAME_DESCRIPTORS-1].NextFD = rxFrameDescrArray;
  
  rxReadPointer = rxFrameDescrArray;
  
  return;
}

#if HavePHY

#if HavePHYinterrupt
static unsigned linkStatus;

static cyg_uint32 MAC_Phy_isr(cyg_vector_t vector, cyg_addrword_t data)
{
  cyg_drv_interrupt_acknowledge(vector);
  PhyInterruptAck();
  ++ks5000_MAC_Phy_Cnt;
  linkStatus = PhyStatus();
  if (linkStatus & PhyStatus_FullDuplex)  
    MACConfigVar |= (1<<3);
  else
    MACConfigVar &= ~(1<<3);
  
#if defined(CYGPKG_NET)      
  if (linkStatus & PhyStatus_FullDuplex)
    ifStats.duplex = 3;
  else
    ifStats.duplex = 2;
  
  if (linkStatus & PhyStatus_LinkUp)
    ifStats.operational = 3;
  else
    ifStats.operational = 2;

  if (linkStatus & PhyStatus_100Mb)
    ifStats.speed = 100000000;
  else
    ifStats.speed = 10000000;
#endif  
  
  MACCON = MACConfigVar;
  return CYG_ISR_HANDLED;
}
#endif
#endif

static void ks32c5000_handle_tx_complete(void)
{

  // record status and then free any buffers we're done with
  
  while (txDonePointer->FrameDataPtr && !(txDonePointer->FrameDataPtr & FRM_OWNERSHIP_BDMA))
    {
#if defined(CYGPKG_NET)
      U32 txStatus;
        
      txStatus = txDonePointer->StatusAndFrameLength>>16;
    
      ++ks5000_MAC_Tx_Cnt;
      ++ifStats.interrupts;
  
      if (txStatus & MACTXSTAT_COMP)
        ++ifStats.tx_complete;
      
      if (txStatus & (MACTXSTAT_EX_COLL | MACTXSTAT_DEFFERED |
                            MACTXSTAT_UNDER | MACTXSTAT_DEFER |
                            MACTXSTAT_NCARR | MACTXSTAT_SIG_QUAL |
                            MACTXSTAT_LATE_COLL | MACTXSTAT_PAR |
                            MACTXSTAT_PAUSED | MACTXSTAT_HALTED)) 
        {
          // transmit failed, log errors
          if (txStatus & MACTXSTAT_EX_COLL)
            ++ifStats.tx_max_collisions;
          if (txStatus & MACTXSTAT_DEFFERED)
            ++ifStats.tx_deferred;
          if (txStatus & MACTXSTAT_UNDER)
            ++ifStats.tx_underrun;
          if (txStatus & MACTXSTAT_DEFER)
            ;
          if (txStatus & MACTXSTAT_NCARR)
            ++ifStats.tx_carrier_loss;
          if (txStatus & MACTXSTAT_SIG_QUAL)
            ++ifStats.tx_sqetesterrors;
          if (txStatus & MACTXSTAT_LATE_COLL)
            ++ifStats.tx_late_collisions;
          if (txStatus & MACTXSTAT_PAR)
            ;
          if (txStatus & MACTXSTAT_PAUSED)
            ;
          if (txStatus & MACTXSTAT_HALTED)
            ;
        }
      else
        {
          // transmit OK
          int collisionCnt = txStatus & 0x0f;
          ++ifStats.tx_good;
          if (collisionCnt)
            {
              if (collisionCnt == 1)
                ++ifStats.tx_single_collisions;
              else
                ++ifStats.tx_mult_collisions;
              ifStats.tx_total_collisions += collisionCnt;
            }
        }
#endif      
      isrFreeBuffer((tEthBuffer*)txDonePointer->FrameDataPtr);
      txDonePointer->FrameDataPtr = 0;
      txDonePointer = txDonePointer->NextFD;
    }
}


//======================================================================
static cyg_uint32 MAC_Tx_isr(cyg_vector_t vector, cyg_addrword_t data)
{
  cyg_drv_interrupt_acknowledge(vector);
  ks32c5000_handle_tx_complete();
  return CYG_ISR_HANDLED;
}

static unsigned accumulatedMaxRxStatus=0;

//======================================================================
static cyg_uint32 MAC_Rx_isr(cyg_vector_t vector, cyg_addrword_t data)
{
  U32 IntMACRxStatus;
  
  cyg_drv_interrupt_acknowledge(vector);
  
  IntMACRxStatus = MACRXSTAT;
  MACRXSTAT = IntMACRxStatus;

  accumulatedMaxRxStatus |= IntMACRxStatus;
  
  ++ks5000_MAC_Rx_Cnt;

#if defined(CYGPKG_NET)
  
  ++ifStats.interrupts;

  if (IntMACRxStatus & MACRXSTAT_GOOD) 
    {
      ++ifStats.rx_good;
      
      if (IntMACRxStatus & MACRXSTAT_CTL_RECD)
        ;  // we don't do anything with control packets
      
      return CYG_ISR_HANDLED;
    }

  if (IntMACRxStatus & (MACRXSTAT_ALLIGN_ERR | MACRXSTAT_CRC_ERR |
                        MACRXSTAT_OVERFLOW | MACRXSTAT_LONG_ERR |
                        MACRXSTAT_PAR | MACRXSTAT_HALTED) ) 
    {
      if (IntMACRxStatus & MACRXSTAT_ALLIGN_ERR)
        ++ifStats.rx_align_errors;
      if (IntMACRxStatus & MACRXSTAT_CRC_ERR)
        ++ifStats.rx_crc_errors;
      if (IntMACRxStatus & MACRXSTAT_OVERFLOW)
        ++ifStats.rx_overrun_errors;
      if (IntMACRxStatus & MACRXSTAT_LONG_ERR)
        ++ifStats.rx_too_long_frames;
      if (IntMACRxStatus & MACRXSTAT_PAR)
        ++ifStats.rx_symbol_errors;
      if (IntMACRxStatus & MACRXSTAT_HALTED)
        ;
    }
#endif  
  return CYG_ISR_HANDLED;
}



//======================================================================
// This interrupt only happens when errors occur
static cyg_uint32 BDMA_Tx_isr(cyg_vector_t vector, cyg_addrword_t data)
{
  U32 IntBDMATxStatus;

  cyg_drv_interrupt_acknowledge(vector);
  
  IntBDMATxStatus = BDMASTAT;
  BDMASTAT = IntBDMATxStatus;

  ++ks5000_BDMA_Tx_Isr_Cnt;
#if defined(CYGPKG_NET)  
  ++ifStats.interrupts;
#endif  
  if (IntBDMATxStatus & BDMASTAT_TX_CCP) 
    {
      debug_printf("+-- Control Packet Transfered : %x\r",ERMPZCNT);
      debug_printf("    Tx Control Frame Status : %x\r",ETXSTAT);
    }

  if (IntBDMATxStatus & (BDMASTAT_TX_NL|BDMASTAT_TX_NO|BDMASTAT_TX_EMPTY) )
    {
      if (IntBDMATxStatus & BDMASTAT_TX_NL)
        BDMATxErrCnt.BTxNLErr++;
      if (IntBDMATxStatus & BDMASTAT_TX_NO)
        BDMATxErrCnt.BTxNOErr++;
      if (IntBDMATxStatus & BDMASTAT_TX_EMPTY)
        BDMATxErrCnt.BTxEmptyErr++;
    }
  
  // free any buffers we're done with

  while (txDonePointer->FrameDataPtr && !(txDonePointer->FrameDataPtr & FRM_OWNERSHIP_BDMA))
    {
      freeBuffer((tEthBuffer*)txDonePointer->FrameDataPtr);
      txDonePointer->FrameDataPtr = 0;
      txDonePointer = txDonePointer->NextFD;
    }

  // don't call tx dsr for now -- it has nothing to do

  return CYG_ISR_HANDLED;
}


static void BDMA_Tx_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
  ++ks5000_BDMA_Tx_Dsr_Cnt;
}



//======================================================================
static cyg_uint32 BDMA_Rx_isr(cyg_vector_t vector, cyg_addrword_t data)
{
  U32 IntBDMARxStatus;
  
  cyg_drv_interrupt_acknowledge(vector);
  
  IntBDMARxStatus = BDMASTAT;
  BDMASTAT = IntBDMARxStatus;

  ++ks5000_BDMA_Rx_Isr_Cnt;
#if defined(CYGPKG_NET)  
  ++ifStats.interrupts;
#endif
  if (IntBDMARxStatus & (BDMASTAT_RX_NL  | BDMASTAT_RX_NO |
                         BDMASTAT_RX_MSO | BDMASTAT_RX_EMPTY |
                         BDMASTAT_RX_SEARLY) )
    {
//      printf("RxIsr %u\r\n", (unsigned)cyg_current_time());
      if (IntBDMARxStatus & BDMASTAT_RX_NL)
        BDMARxErrCnt.BRxNLErr++;
      if (IntBDMARxStatus & BDMASTAT_RX_NO)
        BDMARxErrCnt.BRxNOErr++;
      if (IntBDMARxStatus & BDMASTAT_RX_MSO)
        BDMARxErrCnt.BRxMSOErr++;
      if (IntBDMARxStatus & BDMASTAT_RX_EMPTY)
        BDMARxErrCnt.BRxEmptyErr++;
      if (IntBDMARxStatus & BDMASTAT_RX_SEARLY)
        BDMARxErrCnt.sBRxSEarly++;
    }
  
  return CYG_ISR_HANDLED|CYG_ISR_CALL_DSR;
}

static void eth_handle_recv_buffer(tEthBuffer*);

static cyg_handle_t  bdmaRxIntrHandle;
static cyg_handle_t  bdmaTxIntrHandle;
static cyg_handle_t  macRxIntrHandle;
static cyg_handle_t  macTxIntrHandle;

static cyg_interrupt  bdmaRxIntrObject;
static cyg_interrupt  bdmaTxIntrObject;
static cyg_interrupt  macRxIntrObject;
static cyg_interrupt  macTxIntrObject;

static int ethernetRunning;

#if HavePHYinterrupt
static cyg_handle_t  macPhyIntrHandle;
static cyg_interrupt  macPhyIntrObject;
#endif

static void ks32c5000_eth_deliver(struct eth_drv_sc *sc)
{
  unsigned short p;
  tEthBuffer *rxBuffer;
  extern void cyg_interrupt_post_dsr(CYG_ADDRWORD intr_obj);
  
  ++ks5000_BDMA_Rx_Dsr_Cnt;
  
  while (1)
    {
      if (!rx_frame_avail())
        {
          // no more frames
          return;
        }
      
      if (!(rxBuffer=ks32c5000_eth_get_recv_buffer()))
        {
          // no buffers available
          return;
        }
      
      p = *((unsigned short*)(rxBuffer->data+EtherFramePadding+ETH_HEADER_SIZE-2));
      
      if (ethernetRunning)
        eth_handle_recv_buffer(rxBuffer);
      else
        freeBuffer(rxBuffer);
    }
}


static void installInterrupts(void)
{
  extern struct eth_drv_sc ks32c5000_sc;
  bool firstTime=true;

  debug_printf("ks5000_ether: installInterrupts()\n");
  
  if (!firstTime)
    return;
  firstTime = false;
  
  initFreeList();
  Init_RxFrameDescriptorArray();
  Init_TxFrameDescriptorArray();
  
  BDMARXPTR = (U32)rxReadPointer;
  BDMATXPTR = (U32)txWritePointer;
  
  cyg_drv_mutex_init(&txMutex);
  cyg_drv_mutex_init(&oldRxMutex);
  cyg_drv_cond_init(&oldRxCond,&oldRxMutex);

  cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_EtherBDMARx,0,(unsigned)&ks32c5000_sc,BDMA_Rx_isr,eth_drv_dsr,&bdmaRxIntrHandle,&bdmaRxIntrObject);
  cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_EtherBDMATx,0,0,BDMA_Tx_isr,BDMA_Tx_dsr,&bdmaTxIntrHandle,&bdmaTxIntrObject);
  cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_EtherMacRx,0,0,MAC_Rx_isr,NULL,&macRxIntrHandle,&macRxIntrObject);
  cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_EtherMacTx,0,0,MAC_Tx_isr,NULL,&macTxIntrHandle,&macTxIntrObject);
#if HavePHYinterrupt
  cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_Ext0,0,0,MAC_Phy_isr,NULL,&macPhyIntrHandle,&macPhyIntrObject);
  cyg_drv_interrupt_attach(macPhyIntrHandle);
#endif  
  
  cyg_drv_interrupt_attach(bdmaRxIntrHandle);
  cyg_drv_interrupt_attach(bdmaTxIntrHandle);
  cyg_drv_interrupt_attach(macRxIntrHandle);
  cyg_drv_interrupt_attach(macTxIntrHandle);
  
  cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_Ext0);
  cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_Ext0);
}

//======================================================================
// Driver code that interfaces to the TCP/IP stack via the common
// Ethernet interface.

    
// don't have any private data, but if we did, this is where it would go
typedef struct
{
  int j;
}ks32c5000_priv_data_t;

ks32c5000_priv_data_t ks32c5000_priv_data;    

#define eth_drv_tx_done(sc,key,retval) (sc)->funs->eth_drv->tx_done(sc,key,retval)
#define eth_drv_init(sc,enaddr)  ((sc)->funs->eth_drv->init)(sc, myMacAddr)
#define eth_drv_recv(sc,len)  ((sc)->funs->eth_drv->recv)(sc, len)

static unsigned char myMacAddr[6] = { CYGPKG_DEVS_ETH_ARM_KS32C5000_MACADDR };

static bool ks32c5000_eth_init(struct cyg_netdevtab_entry *tab)
{
  struct eth_drv_sc *sc = (struct eth_drv_sc *)tab->device_instance;
  *(unsigned*)0x7ff5000 |= Bit(1)+Bit(3)+Bit(5);  // enable debug output bits
  // memcpy(myMacAddr,(unsigned char*)CAM_BaseAddr,6);
  debug_printf("ks32c5000_eth_init()\n");
  debug_printf("  MAC address %02x:%02x:%02x:%02x:%02x:%02x\n",myMacAddr[0],myMacAddr[1],myMacAddr[2],myMacAddr[3],myMacAddr[4],myMacAddr[5]);
#if defined(CYGPKG_NET)  
  ifStats.duplex = 1;      //unknown
  ifStats.operational = 1; //unknown
  ifStats.tx_queue_len = MAX_TX_FRAME_DESCRIPTORS;
#endif  
  installInterrupts();
  EthInit(myMacAddr);
  cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EtherBDMARx);
  cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EtherBDMATx);
  cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EtherMacRx);
  cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EtherMacTx);
  configDone = 1;
  ethernetRunning = 1;
  eth_drv_init(sc, myMacAddr);
  return true;
}

static void ks32c5000_eth_start(struct eth_drv_sc *sc, unsigned char *enaddr, int flags)
{
  debug_printf("ks32c5000_eth_start()\n");
  if (!ethernetRunning)
    {
      cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_EtherBDMARx);
      cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_EtherBDMATx);
      cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_EtherMacRx);
      cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_EtherMacTx);
      EthInit(enaddr);
      cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EtherBDMARx);
      cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EtherBDMATx);
      cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EtherMacRx);
      cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EtherMacTx);
      ethernetRunning = 1;
    }
}

static void ks32c5000_eth_stop(struct eth_drv_sc *sc)
{
  debug_printf("ks32c5000_eth_stop()\n");
  ethernetRunning = 0;
}
 
static int ks32c5000_eth_control(struct eth_drv_sc *sc, 
                          unsigned long cmd, 
                          void *data, 
                          int len)
{
  switch (cmd)
    {
#if defined(CYGPKG_NET)      
     case ETH_DRV_GET_IF_STATS_UD:
     case ETH_DRV_GET_IF_STATS:
        {
          struct ether_drv_stats *p = (struct ether_drv_stats*)data;
          *p = ifStats;
          strncpy(p->description,"description goes here",sizeof p->description);
          strncpy(p->snmp_chipset,"chipset name",sizeof p->snmp_chipset);
          return 0;
        }
#endif      
     default:
      return -1;
    }
}

static int ks32c5000_eth_can_send_count=0;
static int ks32c5000_eth_can_send_count_OK=0;

// In case there are multiple Tx frames waiting, we should
// return how many empty Tx spots we have.  For now we just
// return 0 or 1.
static int ks32c5000_eth_can_send(struct eth_drv_sc *sc)
{
  FRAME_DESCRIPTOR *TxFp, *StartFp;
  
  // find the next unused spot in the queue

  ++ks32c5000_eth_can_send_count;
  
  StartFp = TxFp = (FRAME_DESCRIPTOR*)BDMATXPTR;
  
  while (TxFp->FrameDataPtr & FRM_OWNERSHIP_BDMA)
    {
      TxFp = TxFp->NextFD;
      if (TxFp == StartFp)
        return 0;
    }
  ++ks32c5000_eth_can_send_count_OK;
  return 1;
}

static int ks5000_eth_send_count=0;

static void ks32c5000_eth_send(struct eth_drv_sc *sc,
                               struct eth_drv_sg *sg_list,
                               int sg_len,
                               int total_len,
                               unsigned long key)
{
  unsigned char *dest;
  unsigned        len;
  tEthBuffer *buf;

  if (total_len >= MAX_ETH_FRAME_SIZE)
    {
      eth_drv_tx_done(sc,key,-EINVAL);
      return;
    }

  ++ks5000_eth_send_count;

  // allocate buffer

  buf = allocBuffer();
  if (!buf)
    {
      // out of buffers
      eth_drv_tx_done(sc,key,-EIO);
      return;
    }

  // copy data from scatter/gather list into BDMA data buffer

  len = 0;
  dest = buf->data;
  
  while (sg_len)
    {
      memcpy(dest,(unsigned char*)sg_list->buf,sg_list->len);
      len += sg_list->len;
      dest += sg_list->len;
      ++sg_list;
      --sg_len;
    }
  
  buf->length = len;
  
  // tell upper layer that we're done with this sglist

  eth_drv_tx_done(sc,key,0);
  
  // queue packet for transmit
  
  while(!ks32c5000_eth_buffer_send(buf))
    {
#if defined(CYGPKG_KERNEL)
      // wait a tick and try again.
      cyg_thread_delay(1);
#else
      // toss it.
      freeBuffer(buf);
      break;
#endif      
    }
  
}


static int ks5000_eth_rcv_count=0;
static tEthBuffer *tcpIpRxBuffer;

// called from DSR
static void eth_handle_recv_buffer(tEthBuffer* rxBuffer)
{
  extern struct eth_drv_sc ks32c5000_sc;
  tcpIpRxBuffer = rxBuffer;
  eth_drv_recv(&ks32c5000_sc,tcpIpRxBuffer->length-4);  // discard 32-bit CRC
}

static void ks32c5000_eth_recv(struct eth_drv_sc *sc,
                        struct eth_drv_sg *sg_list,
                        int sg_len)
{
  unsigned char *source;

  ++ks5000_eth_rcv_count;
  
  if (!tcpIpRxBuffer)
      return;  // no packet waiting, shouldn't be here!

  // copy data from eth buffer into scatter/gather list

  source = tcpIpRxBuffer->data + EtherFramePadding;
  
  while (sg_len)
    {
	  if (sg_list->buf)
		memcpy((unsigned char*)sg_list->buf,source,sg_list->len);
      source += sg_list->len;
      ++sg_list;
      --sg_len;
    }

  freeBuffer(tcpIpRxBuffer);
  tcpIpRxBuffer = NULL;
  return;
}

// routine called to handle ethernet controller in polled mode
static void ks32c5000_eth_poll(struct eth_drv_sc *sc)
{
  ks32c5000_eth_deliver(sc);  // handle rx frames
  ks32c5000_handle_tx_complete();
}

static int ks32c5000_eth_int_vector(struct eth_drv_sc *sc)
{
  return CYGNUM_HAL_INTERRUPT_EtherBDMARx;
}


ETH_DRV_SC(ks32c5000_sc,
           &ks32c5000_priv_data, // Driver specific data
           "eth0",                // Name for this interface
           ks32c5000_eth_start,
           ks32c5000_eth_stop,
           ks32c5000_eth_control,
           ks32c5000_eth_can_send,
           ks32c5000_eth_send,
           ks32c5000_eth_recv,
           ks32c5000_eth_deliver,
           ks32c5000_eth_poll,
           ks32c5000_eth_int_vector
           );

NETDEVTAB_ENTRY(ks32c5000_netdev, 
                "ks32c5000", 
                ks32c5000_eth_init, 
                &ks32c5000_sc);


#if SoftwareCRC

static U32 crctable[256] =
{
0x00000000L, 0x77073096L, 0xEE0E612CL, 0x990951BAL,
0x076DC419L, 0x706AF48FL, 0xE963A535L, 0x9E6495A3L,
0x0EDB8832L, 0x79DCB8A4L, 0xE0D5E91EL, 0x97D2D988L,
0x09B64C2BL, 0x7EB17CBDL, 0xE7B82D07L, 0x90BF1D91L,
0x1DB71064L, 0x6AB020F2L, 0xF3B97148L, 0x84BE41DEL,
0x1ADAD47DL, 0x6DDDE4EBL, 0xF4D4B551L, 0x83D385C7L,
0x136C9856L, 0x646BA8C0L, 0xFD62F97AL, 0x8A65C9ECL,
0x14015C4FL, 0x63066CD9L, 0xFA0F3D63L, 0x8D080DF5L,
0x3B6E20C8L, 0x4C69105EL, 0xD56041E4L, 0xA2677172L,
0x3C03E4D1L, 0x4B04D447L, 0xD20D85FDL, 0xA50AB56BL,
0x35B5A8FAL, 0x42B2986CL, 0xDBBBC9D6L, 0xACBCF940L,
0x32D86CE3L, 0x45DF5C75L, 0xDCD60DCFL, 0xABD13D59L,
0x26D930ACL, 0x51DE003AL, 0xC8D75180L, 0xBFD06116L,
0x21B4F4B5L, 0x56B3C423L, 0xCFBA9599L, 0xB8BDA50FL,
0x2802B89EL, 0x5F058808L, 0xC60CD9B2L, 0xB10BE924L,
0x2F6F7C87L, 0x58684C11L, 0xC1611DABL, 0xB6662D3DL,
0x76DC4190L, 0x01DB7106L, 0x98D220BCL, 0xEFD5102AL,
0x71B18589L, 0x06B6B51FL, 0x9FBFE4A5L, 0xE8B8D433L,
0x7807C9A2L, 0x0F00F934L, 0x9609A88EL, 0xE10E9818L,
0x7F6A0DBBL, 0x086D3D2DL, 0x91646C97L, 0xE6635C01L,
0x6B6B51F4L, 0x1C6C6162L, 0x856530D8L, 0xF262004EL,
0x6C0695EDL, 0x1B01A57BL, 0x8208F4C1L, 0xF50FC457L,
0x65B0D9C6L, 0x12B7E950L, 0x8BBEB8EAL, 0xFCB9887CL,
0x62DD1DDFL, 0x15DA2D49L, 0x8CD37CF3L, 0xFBD44C65L,
0x4DB26158L, 0x3AB551CEL, 0xA3BC0074L, 0xD4BB30E2L,
0x4ADFA541L, 0x3DD895D7L, 0xA4D1C46DL, 0xD3D6F4FBL,
0x4369E96AL, 0x346ED9FCL, 0xAD678846L, 0xDA60B8D0L,
0x44042D73L, 0x33031DE5L, 0xAA0A4C5FL, 0xDD0D7CC9L,
0x5005713CL, 0x270241AAL, 0xBE0B1010L, 0xC90C2086L,
0x5768B525L, 0x206F85B3L, 0xB966D409L, 0xCE61E49FL,
0x5EDEF90EL, 0x29D9C998L, 0xB0D09822L, 0xC7D7A8B4L,
0x59B33D17L, 0x2EB40D81L, 0xB7BD5C3BL, 0xC0BA6CADL,
0xEDB88320L, 0x9ABFB3B6L, 0x03B6E20CL, 0x74B1D29AL,
0xEAD54739L, 0x9DD277AFL, 0x04DB2615L, 0x73DC1683L,
0xE3630B12L, 0x94643B84L, 0x0D6D6A3EL, 0x7A6A5AA8L,
0xE40ECF0BL, 0x9309FF9DL, 0x0A00AE27L, 0x7D079EB1L,
0xF00F9344L, 0x8708A3D2L, 0x1E01F268L, 0x6906C2FEL,
0xF762575DL, 0x806567CBL, 0x196C3671L, 0x6E6B06E7L,
0xFED41B76L, 0x89D32BE0L, 0x10DA7A5AL, 0x67DD4ACCL,
0xF9B9DF6FL, 0x8EBEEFF9L, 0x17B7BE43L, 0x60B08ED5L,
0xD6D6A3E8L, 0xA1D1937EL, 0x38D8C2C4L, 0x4FDFF252L,
0xD1BB67F1L, 0xA6BC5767L, 0x3FB506DDL, 0x48B2364BL,
0xD80D2BDAL, 0xAF0A1B4CL, 0x36034AF6L, 0x41047A60L,
0xDF60EFC3L, 0xA867DF55L, 0x316E8EEFL, 0x4669BE79L,
0xCB61B38CL, 0xBC66831AL, 0x256FD2A0L, 0x5268E236L,
0xCC0C7795L, 0xBB0B4703L, 0x220216B9L, 0x5505262FL,
0xC5BA3BBEL, 0xB2BD0B28L, 0x2BB45A92L, 0x5CB36A04L,
0xC2D7FFA7L, 0xB5D0CF31L, 0x2CD99E8BL, 0x5BDEAE1DL,
0x9B64C2B0L, 0xEC63F226L, 0x756AA39CL, 0x026D930AL,
0x9C0906A9L, 0xEB0E363FL, 0x72076785L, 0x05005713L,
0x95BF4A82L, 0xE2B87A14L, 0x7BB12BAEL, 0x0CB61B38L,
0x92D28E9BL, 0xE5D5BE0DL, 0x7CDCEFB7L, 0x0BDBDF21L,
0x86D3D2D4L, 0xF1D4E242L, 0x68DDB3F8L, 0x1FDA836EL,
0x81BE16CDL, 0xF6B9265BL, 0x6FB077E1L, 0x18B74777L,
0x88085AE6L, 0xFF0F6A70L, 0x66063BCAL, 0x11010B5CL,
0x8F659EFFL, 0xF862AE69L, 0x616BFFD3L, 0x166CCF45L,
0xA00AE278L, 0xD70DD2EEL, 0x4E048354L, 0x3903B3C2L,
0xA7672661L, 0xD06016F7L, 0x4969474DL, 0x3E6E77DBL,
0xAED16A4AL, 0xD9D65ADCL, 0x40DF0B66L, 0x37D83BF0L,
0xA9BCAE53L, 0xDEBB9EC5L, 0x47B2CF7FL, 0x30B5FFE9L,
0xBDBDF21CL, 0xCABAC28AL, 0x53B39330L, 0x24B4A3A6L,
0xBAD03605L, 0xCDD70693L, 0x54DE5729L, 0x23D967BFL,
0xB3667A2EL, 0xC4614AB8L, 0x5D681B02L, 0x2A6F2B94L,
0xB40BBE37L, 0xC30C8EA1L, 0x5A05DF1BL, 0x2D02EF8DL
};


static U32 crc32( unsigned char *blk_adr, unsigned blk_len)
{
  U32 crc = 0xffffffff;
  
  while (blk_len--)
    crc = crctable[(crc ^ *blk_adr++) & 0xffL] ^ (crc >> 8);
  
  return crc ^ 0xffffffff;
}

#endif




 
