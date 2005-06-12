//=============================================================================
//
//      mod_regs_rtc.h
//
//      RTC (real time clock) Module register definitions
//
//=============================================================================
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jskov
// Contributors:jskov, Hajime Ishitani 
// Date:        2000-10-30
//              
//####DESCRIPTIONEND####
//
//=============================================================================

//--------------------------------------------------------------------------
// RealTime Clock

#define CYGARC_REG_RC64CNT              0xFFC80000		// 8bit
#define CYGARC_REG_RSECCNT              0xFFC80004		// 8bit
#define CYGARC_REG_RMINCNT              0xFFC80008		// 8bit
#define CYGARC_REG_RHRCNT               0xFFC8000C		// 8bit
#define CYGARC_REG_RWKCNT               0xFFC80010		// 8bit
#define CYGARC_REG_RDAYCNT              0xFFC80014		// 8bit
#define CYGARC_REG_RMONCNT              0xFFC80018		// 8bit
#define CYGARC_REG_RYRCNT               0xFFC8001C		// 16bit
#define CYGARC_REG_RSECAR               0xFFC80020		// 8bit
#define CYGARC_REG_RMINAR               0xFFC80024		// 8bit
#define CYGARC_REG_RHRAR                0xFFC80028		// 8bit
#define CYGARC_REG_RWKAR                0xFFC8002C		// 8bit
#define CYGARC_REG_RDAYAR               0xFFC80030		// 8bit
#define CYGARC_REG_RMONAR               0xFFC80034		// 8bit
#define CYGARC_REG_RCR1                 0xFFC80038		// 8bit
#define CYGARC_REG_RCR2                 0xFFC8003C		// 8bit
#define CYGARC_REG_RCR3                 0xFFC80050		// 8bit
#define CYGARC_REG_RYRAR                0xFFC80054		// 8bit

#define CYGARC_REG_RCR1_CF              0x80 // carry flag
#define CYGARC_REG_RCR1_CIE             0x10 // carry interrupt enable
#define CYGARC_REG_RCR1_AIE             0x08 // alarm interrupt enable
#define CYGARC_REG_RCR1_AF              0x01 // alarm flag

#define CYGARC_REG_RCR2_PEF             0x80 // periodic interrupt flag
#define CYGARC_REG_RCR2_PES2            0x40 // periodic interrupt setting
#define CYGARC_REG_RCR2_PES1            0x20
#define CYGARC_REG_RCR2_PES0            0x10
#define CYGARC_REG_RCR2_RTCEN           0x08 // RTC enable
#define CYGARC_REG_RCR2_ADJ             0x04 // second adjustment
#define CYGARC_REG_RCR2_RESET           0x02 // reset
#define CYGARC_REG_RCR2_START           0x01 // start

