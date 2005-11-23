#ifndef CYGONCE_DEVS_ETH_ARM_NETARM_ETH_REGS_H
#define CYGONCE_DEVS_ETH_ARM_NETARM_ETH_REGS_H

// ====================================================================
//
//      eth_regs.h
//
//      Address mappings for ethernet and DMA controller registers
//
// ====================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2005 eCosCentric Ltd.
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
//####ECOSGPLCOPYRIGHTEND####
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           Harald Brandl (harald.brandl@fh-joanneum.at)
// Contributors:        Harald Brandl
// Date:                01.08.2004
// Purpose:             Ethernet and DMA controller registers
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_io.h>


#ifndef HAL_OR_UINT32
#define HAL_OR_UINT32( _register_, _value_ ) \
        CYG_MACRO_START                      \
        cyg_uint32 _i_;                      \
        HAL_READ_UINT32( _register_, _i_ );  \
        _i_ |= _value_;                        \
        HAL_WRITE_UINT32( _register_, _i_ ); \
        CYG_MACRO_END
#endif

#ifndef HAL_AND_UINT32
#define HAL_AND_UINT32( _register_, _value_ ) \
        CYG_MACRO_START                      \
        cyg_uint32 _i_;                      \
        HAL_READ_UINT32( _register_, _i_ );  \
        _i_ &= _value_;                        \
        HAL_WRITE_UINT32( _register_, _i_ ); \
        CYG_MACRO_END
#endif

// Ethernet

#define EthGenCR 			 (unsigned *)0xff800000
#define EthGenSR 			 (unsigned *)0xff800004
#define EthFIFODR			 (unsigned *)0xff800008
#define EthFIFODRkickoff	 (unsigned *)0xff80000c
#define EthTxSR 			 (unsigned *)0xff800010
#define EthRxSR 			 (unsigned *)0xff800014
#define MACCR 				 (unsigned *)0xff800400
#define MACTR 				 (unsigned *)0xff800404
#define PCSCR 				 (unsigned *)0xff800408
#define PCSTR 				 (unsigned *)0xff80040c
#define STLCR 				 (unsigned *)0xff800410
#define STLTR 				 (unsigned *)0xff800414
#define BtBIPGGapTimerR		 (unsigned *)0xff800440
#define NonBtBIPGGapTimerR	 (unsigned *)0xff800444
#define CollWinR 			 (unsigned *)0xff800448
#define TxPNCR 				 (unsigned *)0xff800460
#define TxBCR 				 (unsigned *)0xff800464
#define ReTxBCR 			 (unsigned *)0xff800468
#define TxRNG 				 (unsigned *)0xff80046c
#define TxMRN 				 (unsigned *)0xff800470
#define TxCDec 				 (unsigned *)0xff800474
#define TOTxC 				 (unsigned *)0xff800478
#define RxBC 				 (unsigned *)0xff800480
#define RxCDec 				 (unsigned *)0xff800484
#define TORxC 				 (unsigned *)0xff800488
#define LnFC 				 (unsigned *)0xff8004c0
#define JC10M 				 (unsigned *)0xff800500
#define LoCC10M 			 (unsigned *)0xff800504
#define MIICR 				 (unsigned *)0xff800540
#define MIIAR 				 (unsigned *)0xff800544
#define MIIWDR 				 (unsigned *)0xff800548
#define MIIRDR 				 (unsigned *)0xff80054c
#define MIIIR 				 (unsigned *)0xff800550
#define CRCEC 				 (unsigned *)0xff800580
#define AEC 				 (unsigned *)0xff800584
#define CEC 				 (unsigned *)0xff800588
#define LFC 				 (unsigned *)0xff80058c
#define SFC 				 (unsigned *)0xff800590
#define LCC 				 (unsigned *)0xff800594
#define EDC 				 (unsigned *)0xff800598
#define MCC 				 (unsigned *)0xff80059c
#define SAFR 				 (unsigned *)0xff8005c0
#define SAR1 				 (unsigned *)0xff8005c4
#define SAR2 				 (unsigned *)0xff8005c8
#define SAR3 				 (unsigned *)0xff8005cc
#define SAMHT1 				 (unsigned *)0xff8005d0
#define SAMHT2 				 (unsigned *)0xff8005d4
#define SAMHT3 				 (unsigned *)0xff8005d8
#define SAMHT4 				 (unsigned *)0xff8005dc

// DMA

#define DMA1A_BDP	(unsigned *)0xff900000
#define DMA1A_CR	(unsigned *)0xff900010
#define DMA1A_SR	(unsigned *)0xff900014
#define DMA1B_BDP	(unsigned *)0xff900020
#define DMA1B_CR	(unsigned *)0xff900030
#define DMA1B_SR	(unsigned *)0xff900034
#define DMA1C_BDP	(unsigned *)0xff900040
#define DMA1C_CR	(unsigned *)0xff900050
#define DMA1C_SR	(unsigned *)0xff900054
#define DMA1D_BDP	(unsigned *)0xff900060
#define DMA1D_CR	(unsigned *)0xff900070
#define DMA1D_SR	(unsigned *)0xff900074
#define DMA2_BDP	(unsigned *)0xff900080
#define DMA2_CR		(unsigned *)0xff900090
#define DMA2_SR		(unsigned *)0xff900094

#endif	// CYGONCE_DEVS_ETH_ARM_NETARM_ETH_REGS_H
