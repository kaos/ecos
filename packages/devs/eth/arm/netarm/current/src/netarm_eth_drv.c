//==========================================================================
//
//      netarm_eth_drv.c
//
//      NetSilion NET+ARM Ethernet Driver (DMA driven)
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2005 eCosCentric LTD
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           Harald Brandl (harald.brandl@fh-joanneum.at)
// Contributors:        Harald Brandl
// Date:                01.08.2004
// Purpose:             NET+ARM Ethernet Driver (DMA driven)
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/drv_api.h>
#include <cyg/io/eth/netdev.h>
#include <cyg/io/eth/eth_drv.h>

#include <pkgconf/devs_eth_arm_netarm.h>
#include <cyg/hal/hal_platform_ints.h>
#include "eth_regs.h"
#include "MII.h"
#include "netarm_eth_drv.h"
#include "eeprom.h"

#include <cyg/hal/hal_io.h>

// FIXME: The HAL should provide this
#define CYGNUM_HAL_INTERRUPT_DMA2               31
#define CYGNUM_HAL_INTERRUPT_DMA1               32

// FIXME: The HAL should provide this and it should handle more than
// RAM addresses
#define HAL_VIRT_TO_PHYS_ADDRESS( vaddr ) (unsigned char *)vaddr - 0x8000000

#define BufferSizeA 128
#define BufferSizeB 512
#define BufferSizeC 1524

#define NumA 16
#define NumB 8
#define NumC 8

#define EEPROM_MAC 0    // location of MAC address inside eeprom

static cyg_mutex_t Key_Mutex;
static private_data_t driver_private;

static volatile unsigned char RxBufferA[NumA][BufferSizeA];
static volatile unsigned char RxBufferB[NumB][BufferSizeB];
static volatile unsigned char RxBufferC[NumC][BufferSizeC];

static volatile unsigned char TxBuffer[1500];
static volatile unsigned char TxHeader[14];

static volatile BDP_t RxBDP_A[NumA];
static volatile BDP_t RxBDP_B[NumB];
static volatile BDP_t RxBDP_C[NumC];

static volatile BDP_t TxBDP[2];

// relocation pointer for data accessed by DMA to enable caching
static volatile unsigned char *pRxBufferA, *pRxBufferB, *pRxBufferC;
static volatile unsigned char *pTxHeader, *pTxBuffer;
static volatile BDP_t *pRxBDP_A, *pRxBDP_B, *pRxBDP_C;
static volatile BDP_t *pTxBDP;

ETH_DRV_SC(netarm_sc,
           (void *)&driver_private,     // driver specific data
           "eth0",                      // Name for this interface
           netarm_start,
           netarm_stop,
           netarm_control,
           netarm_can_send,
           netarm_send,
           netarm_recv,
           netarm_deliver,
           netarm_poll,
           netarm_int_vector);

NETDEVTAB_ENTRY(netarm_netdev,
                "ETH_DRV",
                netarm_init,
                &netarm_sc);

static void
fastcopy(void *buf, void *data, unsigned long len)
{
  asm volatile(
               
               "STMDB SP!, {R11};"
               
               "TST R1, #2;"                    // test if aligned
               "LDRNEH R3, [R1], #2;"
               "STRNEH R3, [R0], #2;"
               "SUBNE R2, R2, #2;"
               "TST R1, #1;"
               "LDRNEB R3, [R1], #1;"
               "STRNEB R3, [R0], #1;"
               "SUBNE R2, R2, #1;"
               
               ".START:"
               
               "CMP R2, #44;"
               "BLT .LASTBYTES;"
               "LDMIA R1!, {R3 - R12, R14};"
               "STMIA R0!, {R3 - R12, R14};"
               "SUB R2, R2, #44;"
               
               "CMP R2, #44;"
               "BLT .LASTBYTES;"
               "LDMIA R1!, {R3 - R12, R14};"
               "STMIA R0!, {R3 - R12, R14};"
               "SUB R2, R2, #44;"
               
               "CMP R2, #44;"
               "BLT .LASTBYTES;"
               "LDMIA R1!, {R3 - R12, R14};"
               "STMIA R0!, {R3 - R12, R14};"
               "SUB R2, R2, #44;"
               
               "CMP R2, #44;"
               "BLT .LASTBYTES;"
               "LDMIA R1!, {R3 - R12, R14};"
               "STMIA R0!, {R3 - R12, R14};"
               "SUB R2, R2, #44;"
               
               "CMP R2, #44;"
               "BLT .LASTBYTES;"
               "LDMIA R1!, {R3 - R12, R14};"
               "STMIA R0!, {R3 - R12, R14};"
               "SUB R2, R2, #44;"
               
               "CMP R2, #44;"
               "BLT .LASTBYTES;"
               "LDMIA R1!, {R3 - R12, R14};"
               "STMIA R0!, {R3 - R12, R14};"
               "SUB R2, R2, #44;"
               
               "CMP R2, #44;"
               "BLT .LASTBYTES;"
               "LDMIA R1!, {R3 - R12, R14};"
               "STMIA R0!, {R3 - R12, R14};"
               "SUB R2, R2, #44;"
               
               "CMP R2, #44;"
               "BLT .LASTBYTES;"
               "LDMIA R1!, {R3 - R12, R14};"
               "STMIA R0!, {R3 - R12, R14};"
               "SUB R2, R2, #44;"
               
               "CMP R2, #44;"
               "BLT .LASTBYTES;"
               "LDMIA R1!, {R3 - R12, R14};"
               "STMIA R0!, {R3 - R12, R14};"
               "SUB R2, R2, #44;"
               
               "CMP R2, #44;"
               "BLT .LASTBYTES;"
               "LDMIA R1!, {R3 - R12, R14};"
               "STMIA R0!, {R3 - R12, R14};"
               "SUB R2, R2, #44;"
               
               "BGE .START;"
               
               
               ".LASTBYTES:"
               
               "AND R14, R2, #0xfffffffc;"
               "LDR PC, [PC, R14];"
               "NOP;"
               
               ".SWITCH:"
               ".word   .CASE0;"
               ".word   .CASE1;"
               ".word   .CASE2;"
               ".word   .CASE3;"
               ".word   .CASE4;"
               ".word   .CASE5;"
               ".word   .CASE6;"
               ".word   .CASE7;"
               ".word   .CASE8;"
               ".word   .CASE9;"
               ".word   .CASE10;"
               
               ".CASE0:"
               "B .END;"
               
               ".CASE1:"
               "LDR R3, [R1]!;"
               "STR R3, [R0]!;"
               "B .END;"
               
               ".CASE2:"
               "LDMIA R1!, {R3, R4};"
               "STMIA R0!, {R3, R4};"
               "B .END;"
               
               ".CASE3:"
               "LDMIA R1!, {R3 - R5};"
               "STMIA R0!, {R3 - R5};"
               "B .END;"
               
               ".CASE4:"
               "LDMIA R1!, {R3 - R6};"
               "STMIA R0!, {R3 - R6};"
               "B .END;"
               
               ".CASE5:"
               "LDMIA R1!, {R3 - R7};"
               "STMIA R0!, {R3 - R7};"
               "B .END;"
               
               ".CASE6:"
               "LDMIA R1!, {R3 - R8};"
               "STMIA R0!, {R3 - R8};"
               "B .END;"
               
               ".CASE7:"
               "LDMIA R1!, {R3 - R9};"
               "STMIA R0!, {R3 - R9};"
               "B .END;"
               
               ".CASE8:"
               "LDMIA R1!, {R3 - R10};"
               "STMIA R0!, {R3 - R10};"
               "B .END;"
               
               ".CASE9:"
               "LDMIA R1!, {R3 - R11};"
               "STMIA R0!, {R3 - R11};"
               "B .END;"
               
               ".CASE10:"
               "LDMIA R1!, {R3 - R12};"
               "STMIA R0!, {R3 - R12};"
               
               ".END:"
               "TST R2, #2;"
               "LDRNEH R3, [R1], #2;"
               "STRNEH R3, [R0], #2;"
               "TST R2, #1;"
               "LDRNEB R3, [R1], #1;"
               "STRNEB R3, [R0], #1;"
               
               "LDMIA SP!, {R11};"
               
               :
               : "r" (buf), "r" (data), "r" (len)
               : "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r14"
               );
}

static bool
KeyBufferFull(private_data_t *pd)
{
  int tmp = 0;
  
  cyg_drv_mutex_lock(&Key_Mutex);
  
  if((pd->key_head + 1) % MaxKeys == pd->key_tail)
    {
      tmp = 1;
    }
  
  cyg_drv_mutex_unlock(&Key_Mutex);
  
  return tmp;
}

static void
AddKey(unsigned long key, private_data_t *pd)
{
  
  cyg_drv_mutex_lock(&Key_Mutex);
  
  pd->KeyBuffer[pd->key_head] = key;
  pd->key_head = (pd->key_head + 1) % MaxKeys;
  
  cyg_drv_mutex_unlock(&Key_Mutex);
}

static unsigned
GetKey(private_data_t *pd)
{
  unsigned key = 0;
  
  cyg_drv_mutex_lock(&Key_Mutex);
  
  if(pd->key_tail != pd->key_head)
    {
      key = pd->KeyBuffer[pd->key_tail];
      pd->key_tail = (pd->key_tail + 1) % MaxKeys;
    }
  
  cyg_drv_mutex_unlock(&Key_Mutex);
  
  return key;
}

static cyg_uint32
dma_rx_isr(cyg_vector_t vector, cyg_addrword_t data)
{
  
  // block this interrupt until the dsr completes
  cyg_drv_interrupt_mask(vector);
  
  // tell ecos to allow further interrupt processing
  cyg_drv_interrupt_acknowledge(vector);
  
  return CYG_ISR_CALL_DSR;      // call the dsr
}

static void
dma_rx_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
  
  eth_drv_dsr( vector, count, data );
  
  DMA1A_SR = 0x80000000;                // acknowledge and mask interrupts
  DMA1B_SR = 0x80000000;
  DMA1C_SR = 0x80000000;
  
  cyg_drv_interrupt_unmask(vector);
}

static cyg_uint32
dma_tx_isr(cyg_vector_t vector, cyg_addrword_t data)
{
  
  // block this interrupt until the dsr completes
  cyg_drv_interrupt_mask(vector);
  
  // tell ecos to allow further interrupt processing
  cyg_drv_interrupt_acknowledge(vector);
  
  return CYG_ISR_CALL_DSR;      // invoke the dsr
}

static void
dma_tx_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
  
  eth_drv_dsr( vector, count, data );
  
  DMA2_SR |= 0x80000000; // acknowledge interrupt
  
  cyg_drv_interrupt_unmask(vector);
}

static void
SetupDMA(void)
{
  int i;
  
  pRxBufferA = (unsigned char *)HAL_VIRT_TO_PHYS_ADDRESS(RxBufferA);
  pRxBufferB = (unsigned char *)HAL_VIRT_TO_PHYS_ADDRESS(RxBufferB);
  pRxBufferC = (unsigned char *)HAL_VIRT_TO_PHYS_ADDRESS(RxBufferC);
  
  pTxBuffer = (unsigned char *)HAL_VIRT_TO_PHYS_ADDRESS(TxBuffer);
  pTxHeader = (unsigned char *)HAL_VIRT_TO_PHYS_ADDRESS(TxHeader);
  
  pRxBDP_A = (BDP_t *)HAL_VIRT_TO_PHYS_ADDRESS(RxBDP_A);
  pRxBDP_B = (BDP_t *)HAL_VIRT_TO_PHYS_ADDRESS(RxBDP_B);
  pRxBDP_C = (BDP_t *)HAL_VIRT_TO_PHYS_ADDRESS(RxBDP_C);
  
  pTxBDP = (BDP_t *)((unsigned char *)TxBDP);
  
  *SYSCON |= 0x40;              // reset DMA module
  
  for(i = 0; i < NumA; i++)
    {
      pRxBDP_A[i].lo = ((unsigned)(pRxBufferA+i*BufferSizeA)) & 0x3fffffff;
      pRxBDP_A[i].hi = BufferSizeA;
    }
  
  pRxBDP_A[i - 1].lo |= 0x80000000; // set W bit
  
  for(i = 0; i < NumB; i++)
    {
      pRxBDP_B[i].lo = ((unsigned)(pRxBufferB+i*BufferSizeB)) & 0x3fffffff;
      pRxBDP_B[i].hi = BufferSizeB;
    }
  
  pRxBDP_B[i - 1].lo |= 0x80000000;
  
  for(i = 0; i < NumC; i++)
    {
      pRxBDP_C[i].lo = ((unsigned)(pRxBufferC+i*BufferSizeC)) & 0x3fffffff;
      pRxBDP_C[i].hi = BufferSizeC;
    }
  
  pRxBDP_C[i - 1].lo |= 0x80000000;
  
  *SYSCON &= ~0x40;
  
  DMA1A_BDP = (unsigned)pRxBDP_A;
  DMA1A_CR = 0x82000000;        //burst transfer
  DMA1A_SR = 0xa00000;
  
  DMA1B_BDP = (unsigned)pRxBDP_B;
  DMA1B_CR = 0x82000000;        //burst transfer
  DMA1B_SR = 0xa00000;
  
  DMA1C_BDP = (unsigned)pRxBDP_C;
  DMA1C_CR = 0x82000000;        //burst transfer
  DMA1C_SR = 0xa00000;
  
  
  pTxBDP[0].lo = ((unsigned)pTxHeader) & 0x3fffffff;
  pTxBDP[1].lo = ((unsigned)pTxBuffer) & 0x3fffffff;
  pTxBDP[1].lo |= 0xa0000000;   // set W and L bit
  
  DMA2_BDP = (unsigned)pTxBDP;
  DMA2_CR = 0x86000000; //burst transfer
  DMA2_SR = 0x800000;
  
}

static bool
netarm_init(struct cyg_netdevtab_entry *tab)
{
  struct eth_drv_sc *sc = (struct eth_drv_sc *)tab->device_instance;
  cyg_bool duplex;
  static cyg_interrupt dma_rx_int_object, dma_tx_int_object;
  static cyg_handle_t dma_rx_int_handle, dma_tx_int_handle;
  
#ifdef CYGSEM_DEVS_ETH_ARM_NETARM_ETH0_SET_ESA
  
  unsigned char esa[6] = CYGDAT_DEVS_ETH_ARM_NETARM_ETH0_ESA;
#else
  unsigned char esa[6];
  
  initI2C();
  eepromRead(0x50, EEPROM_MAC, esa, 6);
#endif
  
  // setup dma receiver
  SetupDMA();
  
  mii_reset();
  mii_negotiate();                      // initialize PHY
  duplex = mii_check_duplex();
  
  // Ethernet Controller Initializatition
  
  MACCR = (0x1c | (duplex << 1));       // auto CRC, late collision retry
  STLCR = 0x3;                          // insert MAC source address
                                        // into ethernet frame
  BtBIPGGapTimerR = 0x14;               // standard values
  NonBtBIPGGapTimerR  = ((0x9 << 7) | 0x11);    // standard values
  CollWinR = ((0x37 << 8) | 0xf);       // standard values
  SAFR = 0x1;                           // broadcast mode
  EthGenCR = (0x40400400 | (duplex << 16)); // dma mode, full duplex,
                                            // enable pNA mode(needed
                                            // for alignment)
  
  cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_DMA1,   // Interrupt Vector
                           0,                           // Interrupt Priority
                           (cyg_addrword_t)&netarm_sc,  // Reference
                                                        // to Driver
                                                        // Instance
                           dma_rx_isr,
                           dma_rx_dsr,
                           &dma_rx_int_handle,
                           &dma_rx_int_object);
  
  cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_DMA2,   // Interrupt Vector
                           0,                           // Interrupt Priority
                           (cyg_addrword_t)&netarm_sc,  // Reference
                                                        // to Driver
                                                        // Instance
                           dma_tx_isr,
                           dma_tx_dsr,
                           &dma_tx_int_handle,
                           &dma_tx_int_object);
  
  cyg_drv_interrupt_attach(dma_rx_int_handle);
  cyg_drv_interrupt_attach(dma_tx_int_handle);
  
  cyg_mutex_init(&Key_Mutex);
  
  sc->funs->eth_drv->init(sc, esa);
  
  return true;
}

static void
netarm_recv(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len)
{
  int i, len;
  private_data_t *pd = (private_data_t *)(sc->driver_private);
  unsigned char *data, *buf = pd->RxBuffer;
  
  for (i = 0;  i < sg_len;  i++) {
    data = (unsigned char *)(sg_list[i].buf);
    len = sg_list[i].len;
    if (len)
      {
        if(i == 1)
          {
            buf += 2;
          }
        
        fastcopy(data, buf, len);
        buf += len;
      }
  }
}

static void
netarm_deliver(struct eth_drv_sc *sc)
{
  static int a = 0, b = 0, c = 0;
  unsigned long key;
  private_data_t *pd = (private_data_t *)(sc->driver_private);
  
  while((key = GetKey(pd)))
    {
      sc->funs->eth_drv->tx_done(sc, key, 0);
    }
  
  while(pRxBDP_A[a].hi & 0x8000)
    {
      pd->RxBuffer = (unsigned char *)(pRxBDP_A[a].lo & 0x1FFFFFFF);
      HAL_REORDER_BARRIER();
      sc->funs->eth_drv->recv(sc, pRxBDP_A[a].hi & 0x7FFF);
      HAL_REORDER_BARRIER();
      pRxBDP_A[a].hi = BufferSizeA;
      HAL_REORDER_BARRIER();
      a = (a + 1) % NumA;
    }
  
  while(pRxBDP_B[b].hi & 0x8000)
    {
      pd->RxBuffer = (unsigned char *)(pRxBDP_B[b].lo & 0x1FFFFFFF);
      HAL_REORDER_BARRIER();
      sc->funs->eth_drv->recv(sc, pRxBDP_B[b].hi & 0x7FFF);
      HAL_REORDER_BARRIER();
      pRxBDP_B[b].hi = BufferSizeB;
      HAL_REORDER_BARRIER();
      b = (b + 1) % NumB;
    }
  
  while(pRxBDP_C[c].hi & 0x8000)
    {
      pd->RxBuffer = (unsigned char *)(pRxBDP_C[c].lo & 0x1FFFFFFF);
      HAL_REORDER_BARRIER();
      sc->funs->eth_drv->recv(sc, pRxBDP_C[c].hi & 0x7FFF);
      HAL_REORDER_BARRIER();
      pRxBDP_C[c].hi = BufferSizeC;
      HAL_REORDER_BARRIER();
      c = (c + 1) % NumC;
    }
  
  if((DMA1A_SR & 0x20000000) || (DMA1B_SR & 0x20000000) || (DMA1C_SR & 0x20000000))
    {
      EthGenCR &= ~0xc0000000;          // reset Rx FIFO
      EthGenCR |= 0xc0000000;
    }
  
  DMA1A_SR = 0x20a00000;
  DMA1B_SR = 0x20a00000;
  DMA1C_SR = 0x20a00000;
}

static int
netarm_can_send(struct eth_drv_sc *sc)
{
  private_data_t *pd = (private_data_t *)(sc->driver_private);
  
  if((pTxBDP[0].hi & 0x8000) || (pTxBDP[1].hi & 0x8000) || KeyBufferFull(pd))
    return 0;
  
  return 1;
}

static void
netarm_send(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list,
                        int sg_len, int total_len, unsigned long key)
{
  private_data_t *pd = (private_data_t *)(sc->driver_private);
  int i, len;
  unsigned char *data;
  volatile char *buf = pTxBuffer;
  
  AddKey(key, pd);
  
  // Put data into buffer
  for (i = 0;  i < sg_len;  i++) {
    data = (unsigned char *)sg_list[i].buf;
    len = sg_list[i].len;
    
    if(i == 0)
      {
        if((unsigned long)data & 0x3)
          {
            memcpy((unsigned char*)pTxHeader, data, 0xe);
          }
        else
          {
            fastcopy((unsigned char *)pTxHeader, data, 0xe);
          }
        
        len -= 0xe;
        data += 0xe;
      }
    
    if (len)
      {
        fastcopy((unsigned char *)buf, data, len);
        buf += len;
      }
  }
  
  cyg_drv_dsr_lock();
  pTxBDP[0].hi = 0x800e;
  HAL_REORDER_BARRIER();
  pTxBDP[1].hi = (total_len - 0xe) | 0x8000;
  HAL_REORDER_BARRIER();
  DMA2_SR |= 0xf0000000;
  cyg_drv_dsr_unlock();
}

static void
netarm_start(struct eth_drv_sc *sc, unsigned char *enaddr, int flags)
{
  SetMAC(enaddr);                       // set MAC address
  HAL_REORDER_BARRIER();
  EthGenCR |= 0x80800000;       // enable Rx und Tx FIFO
  HAL_REORDER_BARRIER();
  cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_DMA1);
  cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_DMA2);
}

static void
netarm_stop(struct eth_drv_sc *sc)
{
  EthGenCR &= 0x7f7fffff;
  HAL_REORDER_BARRIER();
  cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_DMA1);
  cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_DMA2);
}

static int
netarm_control(struct eth_drv_sc *sc, unsigned long key, void *data, int len)
{
  switch (key)
    {
    case ETH_DRV_SET_MAC_ADDRESS:
      return 0;
      break;
    case ETH_DRV_SET_MC_LIST:
    case ETH_DRV_SET_MC_ALL:
      return 0;
    default:
      return 1;
      break;
    }
}

static void
netarm_poll(struct eth_drv_sc *sc)
{
}

static int
netarm_int_vector(struct eth_drv_sc *sc)
{
  return CYGNUM_HAL_INTERRUPT_DMA1;
}

static void
SetMAC(unsigned char *esa)
{
  SAR1 = (esa[1] << 8) | esa[0];        // set MAC address
  SAR2 = (esa[3] << 8) | esa[2];
  SAR3 = (esa[5] << 8) | esa[4];
}
