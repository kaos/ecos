#ifndef CYGONCE_DEVS_SERIAL_FREESCALE_ESCI_H
#define CYGONCE_DEVS_SERIAL_FREESCALE_ESCI_H
//==========================================================================
//
//      ser_esci.h
//
//      Freescale eSCI Serial I/O definitions.
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) eCosCentric Ltd
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
// Author(s):   Ilija Koco <ilijak@siva.com.mk>
// Contributors:
// Date:        2006-04-20
// Purpose:     eSCI Serial I/O definitions.
// Description: 
//
//   
//####DESCRIPTIONEND####
//==========================================================================

#define CYGNUM_DEV_SER_FREESCALE_ESCI_SYSTEM_CLOCK      \
  (CYGNUM_HAL_ARM_MAC7100_CLOCK_SPEED/2)
#define FREESCALE_ESCI_BAUD(baud_rate)                          \
  ((CYGNUM_DEV_SER_FREESCALE_ESCI_SYSTEM_CLOCK)/(baud_rate*16))

#define FREESCALE_ESCI_A_BASE       (CYGADDR_IO_SERIAL_FREESCALE_ESCI_A_BASE)
#define FREESCALE_ESCI_B_BASE       (CYGADDR_IO_SERIAL_FREESCALE_ESCI_B_BASE)
#define FREESCALE_ESCI_C_BASE       (CYGADDR_IO_SERIAL_FREESCALE_ESCI_C_BASE)
#define FREESCALE_ESCI_D_BASE       (CYGADDR_IO_SERIAL_FREESCALE_ESCI_D_BASE)

#define FREESCALE_ESCI_A_I  0
#define FREESCALE_ESCI_B_I  1
#define FREESCALE_ESCI_C_I  2
#define FREESCALE_ESCI_D_I  3

#define FREESCALE_ESCI_BD(esci_base)                    \
  (esci_base + FREESCALE_ESCI_BD_OFFSET)      //short
#define FREESCALE_ESCI_CR12(esci_base)                  \
  (esci_base + FREESCALE_ESCI_CR12_OFFSET)    //short
#define FREESCALE_ESCI_CR34(esci_base)                  \
  (esci_base + FREESCALE_ESCI_CR34_OFFSET)    //short
#define FREESCALE_ESCI_CR1(esci_base)                   \
  (esci_base + FREESCALE_ESCI_CR1_OFFSET)     //char
#define FREESCALE_ESCI_CR2(esci_base)                   \
  (esci_base + FREESCALE_ESCI_CR2_OFFSET)     //char
#define FREESCALE_ESCI_CR3(esci_base)                   \
  (esci_base + FREESCALE_ESCI_CR3_OFFSET)     //char
#define FREESCALE_ESCI_CR4(esci_base)                   \
  (esci_base + FREESCALE_ESCI_CR4_OFFSET)     //short
#define FREESCALE_ESCI_LINCTRL(esci_base)               \
  (esci_base + FREESCALE_ESCI_LINCTRL_OFFSET) //short
#define FREESCALE_ESCI_LINCRCP(esci_base)               \
  (esci_base + FREESCALE_ESCI_LINCRCP_OFFSET) //short
#define FREESCALE_ESCI_SR(esci_base)                    \
  (esci_base + FREESCALE_ESCI_SR_OFFSET)      //short
#define FREESCALE_ESCI_DRL(esci_base)                   \
  (esci_base + FREESCALE_ESCI_DRL_OFFSET)     //char

#define FREESCALE_ESCI_BD_OFFSET      (0x0000)
#define FREESCALE_ESCI_CR12_OFFSET    (0x0002)
#define FREESCALE_ESCI_CR34_OFFSET    (0x0002)
#define FREESCALE_ESCI_CR1_OFFSET     (0x0002)
#define FREESCALE_ESCI_CR2_OFFSET     (0x0003)
#define FREESCALE_ESCI_CR3_OFFSET     (0x0004)
#define FREESCALE_ESCI_CR4_OFFSET     (0x0005)
#define FREESCALE_ESCI_DRL_OFFSET     (0x0007)
#define FREESCALE_ESCI_SR_OFFSET      (0x0008)
#define FREESCALE_ESCI_LINSTAT_OFFSET (0x000A)
#define FREESCALE_ESCI_LINCTRL_OFFSET (0x000C)
#define FREESCALE_ESCI_LINRX_OFFSET   (0x0010)
#define FREESCALE_ESCI_LINTX_OFFSET   (0x0014)
#define FREESCALE_ESCI_LINCRCP_OFFSET (0x0018)

#define FREESCALE_ESCI_CR12_LOOPS     (0x8000)
#define FREESCALE_ESCI_CR12_SCISDOZ   (0x4000)
#define FREESCALE_ESCI_CR12_RSRC      (0x2000)
#define FREESCALE_ESCI_CR12_M         (0x1000)
#define FREESCALE_ESCI_CR12_WAKE      (0x0800)
#define FREESCALE_ESCI_CR12_ILT       (0x0400)
#define FREESCALE_ESCI_CR12_PE        (0x0200)
#define FREESCALE_ESCI_CR12_PT        (0x0100)
#define FREESCALE_ESCI_CR12_TIE       (0x0080)
#define FREESCALE_ESCI_CR12_TCIE      (0x0040)
#define FREESCALE_ESCI_CR12_RIE       (0x0020)
#define FREESCALE_ESCI_CR12_ILIE      (0x0010)
#define FREESCALE_ESCI_CR12_TE        (0x0008)
#define FREESCALE_ESCI_CR12_RE        (0x0004)
#define FREESCALE_ESCI_CR12_RWU       (0x0002)
#define FREESCALE_ESCI_CR12_SBK       (0x0001)

#define FREESCALE_ESCI_CR3_MDIS       (0x80)
#define FREESCALE_ESCI_CR3_FBR        (0x40)
#define FREESCALE_ESCI_CR3_BSTP       (0x20)
#define FREESCALE_ESCI_CR3_IEBERR     (0x10)
#define FREESCALE_ESCI_CR3_RXDMA      (0x08)
#define FREESCALE_ESCI_CR3_TXDMA      (0x04)
#define FREESCALE_ESCI_CR3_BRK13      (0x02)
#define FREESCALE_ESCI_CR3_TXDIR      (0x01)

#define FREESCALE_ESCI_SR_TDRE        (0x8000)
#define FREESCALE_ESCI_SR_TC          (0x4000)
#define FREESCALE_ESCI_SR_RDRF        (0x2000)
#define FREESCALE_ESCI_SR_IDLE        (0x1000)
#define FREESCALE_ESCI_SR_OR          (0x0800)
#define FREESCALE_ESCI_SR_NF          (0x0400)
#define FREESCALE_ESCI_SR_FE          (0x0200)
#define FREESCALE_ESCI_SR_PF          (0x0100)
#define FREESCALE_ESCI_SR_BERR        (0x0010)
#define FREESCALE_ESCI_SR_RAF         (0x0001)

#endif // CYGONCE_DEVS_SERIAL_FREESCALE_ESCI_H
// EOF ser_esci.h
