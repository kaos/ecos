#ifndef CYGONCE_CAN_LPC2XXX_BAUDRATES_H
#define CYGONCE_CAN_LPC2XXX_BAUDRATES_H
//==========================================================================
//
//      devs/can/arm/lpc2xxx/current/include/can_lpc2xxx_baudrates.h
//
//      Precalculated values for bit timing register
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Gary Thomas
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
// Author(s):    Uwe Kindler
// Contributors: Uwe Kindler
// Date:         2007-07-01
// Purpose:      Precalculated bit timing values for various baudrates
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================


//
// Peripheral clock speed
//
#define CYGNUM_CAN_LPC2XXX_VPB_CLK (CYGNUM_HAL_ARM_LPC2XXX_CLOCK_SPEED / CYGNUM_HAL_ARM_LPC2XXX_VPBDIV)

//
// Macro for creation of CAN_BR value for baudrate tbl
//
#define CAN_BR_TBL_ENTRY(_brp_, _tseg1_, _tseg2_, _sjw_, _sam_) \
   ((_sam_ << 23) | (_tseg2_ << 20) | (_tseg1_ << 16) | (_sjw_ << 14) | (_brp_))


//==========================================================================
//                             BAUDRATES
//==========================================================================
#if CYGNUM_CAN_LPC2XXX_VPB_CLK == 60000000
//
// Table with register values for baudrates at peripheral clock of 60 MHz
//
static const cyg_uint32 lpc2xxx_br_tbl[] =
{
    CAN_BR_TBL_ENTRY(300, 15, 2, 0, 1), // 10  kbaud
    CAN_BR_TBL_ENTRY(150, 15, 2, 0, 1), // 20  kbaud
    CAN_BR_TBL_ENTRY(59,  15, 2, 0, 1), // 50  kbaud
    CAN_BR_TBL_ENTRY(39,  11, 1, 0, 1), // 100 kbaud
    CAN_BR_TBL_ENTRY(29,  12, 1, 0, 1), // 125 kbaud
    CAN_BR_TBL_ENTRY(14,  12, 1, 0, 1), // 250 kbaud
    CAN_BR_TBL_ENTRY( 7,  11, 1, 0, 0), // 500 kbaud
    CAN_BR_TBL_ENTRY( 4,  11, 1, 0, 0), // 800 kbaud
    CAN_BR_TBL_ENTRY( 3,  11, 1, 0, 0), // 1000 kbaud
    CAN_BR_TBL_ENTRY( 0,   0, 0, 0, 0), // Autobaud  - not supported
};
#define HAL_LPC2XXX_BAUD_TBL_DEFINED 1
#endif // CYGNUM_CAN_LPC2XXX_VPB_CLK == 60000000

#if CYGNUM_CAN_LPC2XXX_VPB_CLK == 30000000
//
// Table with register values for baudrates at peripheral clock of 30 MHz
//
static const cyg_uint32 lpc2xxx_br_tbl[] =
{
    CAN_BR_TBL_ENTRY( 0,  0, 0, 0, 0), // 10  kbaud - not supported
    CAN_BR_TBL_ENTRY( 0,  0, 0, 0, 0), // 20  kbaud - not supported
    CAN_BR_TBL_ENTRY(59, 15, 2, 0, 1), // 50  kbaud
    CAN_BR_TBL_ENTRY(39, 11, 1, 0, 1), // 100 kbaud
    CAN_BR_TBL_ENTRY(29, 12, 1, 0, 1), // 125 kbaud
    CAN_BR_TBL_ENTRY(14, 12, 1, 0, 1), // 250 kbaud
    CAN_BR_TBL_ENTRY( 7, 11, 1, 0, 0), // 500 kbaud
    CAN_BR_TBL_ENTRY( 4, 11, 1, 0, 0), // 800 kbaud
    CAN_BR_TBL_ENTRY( 3, 11, 1, 0, 0), // 1000 kbaud
    CAN_BR_TBL_ENTRY( 0,  0, 0, 0, 0), // Autobaud  - not supported
};
#define HAL_LPC2XXX_BAUD_TBL_DEFINED 1
#endif // CYGNUM_CAN_LPC2XXX_VPB_CLK == 30000000

#if CYGNUM_CAN_LPC2XXX_VPB_CLK == 15000000
//
// Table with register values for baudrates at peripheral clock of 15 MHz
//
static const cyg_uint32 lpc2xxx_br_tbl[] =
{
    CAN_BR_TBL_ENTRY(59, 15, 7, 0, 1), // 10  kbaud
    CAN_BR_TBL_ENTRY(49, 11, 1, 0, 1), // 20  kbaud
    CAN_BR_TBL_ENTRY(19, 11, 1, 0, 1), // 50  kbaud
    CAN_BR_TBL_ENTRY( 9, 11, 1, 0, 1), // 100 kbaud
    CAN_BR_TBL_ENTRY( 7, 11, 1, 0, 1), // 125 kbaud
    CAN_BR_TBL_ENTRY( 3, 11, 1, 0, 1), // 250 kbaud
    CAN_BR_TBL_ENTRY( 1, 11, 1, 0, 0), // 500 kbaud
    CAN_BR_TBL_ENTRY( 0,  0, 0, 0, 0), // 800 kbaud - not supported
    CAN_BR_TBL_ENTRY( 0, 11, 1, 0, 0), // 1000 kbaud
    CAN_BR_TBL_ENTRY( 0,  0, 0, 0, 0), // Autobaud  - not supported
};
#define HAL_LPC2XXX_BAUD_TBL_DEFINED 1
#endif // CYGNUM_CAN_LPC2XXX_VPB_CLK == 15000000

#if CYGNUM_CAN_LPC2XXX_VPB_CLK == 48000000
//
// Table with register values for baudrates at peripheral clock of 48 MHz
//
static const cyg_uint32 lpc2xxx_br_tbl[] =
{
    CAN_BR_TBL_ENTRY( 0,  0, 0, 0, 0), // 10  kbaud - not supported
    CAN_BR_TBL_ENTRY( 0,  0, 0, 0, 0), // 20  kbaud - not supported
    CAN_BR_TBL_ENTRY(59, 12, 1, 0, 1), // 50  kbaud
    CAN_BR_TBL_ENTRY(29, 12, 1, 0, 1), // 100 kbaud
    CAN_BR_TBL_ENTRY(23, 12, 1, 0, 1), // 125 kbaud
    CAN_BR_TBL_ENTRY(11, 12, 1, 0, 1), // 250 kbaud
    CAN_BR_TBL_ENTRY( 5, 12, 1, 0, 0), // 500 kbaud
    CAN_BR_TBL_ENTRY( 3, 11, 1, 0, 0), // 800 kbaud
    CAN_BR_TBL_ENTRY( 2, 12, 1, 0, 0), // 1000 kbaud
    CAN_BR_TBL_ENTRY( 0,  0, 0, 0, 0), // Autobaud  - not supported
};
#define HAL_LPC2XXX_BAUD_TBL_DEFINED 1
#endif // CYGNUM_CAN_LPC2XXX_VPB_CLK == 48000000

#if CYGNUM_CAN_LPC2XXX_VPB_CLK == 24000000
//
// Table with register values for baudrates at peripheral clock of 24 MHz
//
static const cyg_uint32 lpc2xxx_br_tbl[] =
{
    CAN_BR_TBL_ENTRY( 0,  0, 0, 0, 0), // 10  kbaud - not supported
    CAN_BR_TBL_ENTRY(59, 15, 2, 0, 1), // 20  kbaud
    CAN_BR_TBL_ENTRY(29, 12, 1, 0, 1), // 50  kbaud
    CAN_BR_TBL_ENTRY(14, 12, 1, 0, 1), // 100 kbaud
    CAN_BR_TBL_ENTRY(11, 12, 1, 0, 1), // 125 kbaud
    CAN_BR_TBL_ENTRY( 5, 12, 1, 0, 1), // 250 kbaud
    CAN_BR_TBL_ENTRY( 2, 12, 1, 0, 0), // 500 kbaud
    CAN_BR_TBL_ENTRY( 1, 11, 1, 0, 0), // 800 kbaud
    CAN_BR_TBL_ENTRY( 1,  5, 0, 0, 0), // 1000 kbaud
    CAN_BR_TBL_ENTRY( 0,  0, 0, 0, 0), // Autobaud  - not supported
};
#define HAL_LPC2XXX_BAUD_TBL_DEFINED 1
#endif // CYGNUM_CAN_LPC2XXX_VPB_CLK == 24000000

#if CYGNUM_CAN_LPC2XXX_VPB_CLK == 12000000
//
// Table with register values for baudrates at peripheral clock of 12 MHz
//
static const cyg_uint32 lpc2xxx_br_tbl[] =
{
    CAN_BR_TBL_ENTRY(59, 15, 2, 0, 1), // 10  kbaud - not supported
    CAN_BR_TBL_ENTRY(39, 11, 1, 0, 1), // 20  kbaud
    CAN_BR_TBL_ENTRY(14, 12, 1, 0, 1), // 50  kbaud
    CAN_BR_TBL_ENTRY( 7, 11, 1, 0, 1), // 100 kbaud
    CAN_BR_TBL_ENTRY( 5, 12, 1, 0, 1), // 125 kbaud
    CAN_BR_TBL_ENTRY( 2, 12, 1, 0, 1), // 250 kbaud
    CAN_BR_TBL_ENTRY( 2, 05, 0, 0, 0), // 500 kbaud
    CAN_BR_TBL_ENTRY( 0, 11, 1, 0, 0), // 800 kbaud
    CAN_BR_TBL_ENTRY( 0,  9, 0, 0, 0), // 1000 kbaud
    CAN_BR_TBL_ENTRY( 0,  0, 0, 0, 0), // Autobaud  - not supported
};
#define HAL_LPC2XXX_BAUD_TBL_DEFINED 1
#endif // CYGNUM_CAN_LPC2XXX_VPB_CLK == 12000000


//==========================================================================
//                          BIT TIMING MACRO
//==========================================================================
//
// Macro fills baudrate register value depending on selected baudrate
// For several LPC2XXX peripheral clock speeds we provide a pre calculated
// baudrate table. If the board uses another clock speed, then the platform 
// HAL needs to provide an own HAL_LPC2XXX_GET_CAN_BR() macro that returns 
// valid baudrate register values or it needs to patch this file with
// an additional table for the desired clock speed
//
//
// If a certain baudrate is not supported, then this macro shall return
// 0 as the baudrate register value
//
#ifdef HAL_LPC2XXX_BAUD_TBL_DEFINED 
#define HAL_LPC2XXX_GET_CAN_BR(_baudrate_, _br_)                 \
CYG_MACRO_START                                                  \
    _br_ = lpc2xxx_br_tbl[(_baudrate_) - CYGNUM_CAN_KBAUD_10];   \
CYG_MACRO_END
#endif // HAL_LPC2XXX_BAUD_TBL_DEFINED 

//-------------------------------------------------------------------------
#endif // #ifndef CYGONCE_CAN_LPC2XXX_BAUDRATES_H
