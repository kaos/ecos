#ifndef CYGONCE_IPAQ_H
#define CYGONCE_IPAQ_H

//=============================================================================
//
//      ipaq.h
//
//      Platform specific support (register layout, etc)
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
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas, richard.panton@3glab.com
// Date:         2001-02-24
// Purpose:      Intel SA1110/iPAQ platform specific support routines
// Description: 
// Usage:        #include <cyg/hal/ipaq.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================


#ifndef __ASSEMBLER__
//
// Extended GPIO
// Note: This register is write-only.  Thus a shadow copy is provided so that
// it may be safely updated/shared by multiple threads.
//
extern unsigned long _ipaq_EGPIO;  // Shadow copy

extern unsigned short _ipaq_LCD_params[(4*2)+1];  // Various LCD parameters

extern void ipaq_EGPIO(unsigned long mask, unsigned long value);

#endif

// 
// Signal assertion levels
//
#define SA1110_LOGIC_ONE(m)  (m & 0xFFFF)
#define SA1110_LOGIC_ZERO(m) (m & 0x0000)

//
// iPAQ Extended GPIO definitions
//
#define SA1110_EGPIO			REG16_PTR(0x49000000)
#define SA1110_EIO_MIN			0x0080

#define SA1110_EIO_VPP			0x0001
# define SA1110_EIO_VPP_OFF			SA1110_LOGIC_ZERO(SA1110_EIO_VPP)
# define SA1110_EIO_VPP_ON			SA1110_LOGIC_ONE(SA1110_EIO_VPP)
#define SA1110_EIO_CF_RESET		0x0002
# define SA1110_EIO_CF_RESET_DISABLE		SA1110_LOGIC_ZERO(SA1110_EIO_CF_RESET)
# define SA1110_EIO_CF_RESET_ENABLE		SA1110_LOGIC_ONE(SA1110_EIO_CF_RESET)
#define SA1110_EIO_OPT_RESET		0x0004
# define SA1110_EIO_OPT_RESET_DISABLE		SA1110_LOGIC_ZERO(SA1110_EIO_OPT_RESET)
# define SA1110_EIO_OPT_RESET_ENABLE		SA1110_LOGIC_ONE(SA1110_EIO_OPT_RESET)
#define SA1110_EIO_CODEC_RESET		0x0008	// Active LOW
# define SA1110_EIO_CODEC_RESET_DISABLE		SA1110_LOGIC_ONE(SA1110_EIO_CODEC_RESET)
# define SA1110_EIO_CODEC_RESET_ENABLE		SA1110_LOGIC_ZERO(SA1110_EIO_CODEC_RESET)
#define SA1110_EIO_OPT_PWR		0x0010
# define SA1110_EIO_OPT_PWR_OFF			SA1110_LOGIC_ZERO(SA1110_EIO_OPT_PWR)
# define SA1110_EIO_OPT_PWR_ON			SA1110_LOGIC_ONE(SA1110_EIO_OPT_PWR)
#define SA1110_EIO_OPT			0x0020
# define SA1110_EIO_OPT_OFF			SA1110_LOGIC_ZERO(SA1110_EIO_OPT)
# define SA1110_EIO_OPT_ON			SA1110_LOGIC_ONE(SA1110_EIO_OPT)
#define SA1110_EIO_LCD_3V3		0x0040
# define SA1110_EIO_LCD_3V3_OFF			SA1110_LOGIC_ZERO(SA1110_EIO_LCD_3V3)
# define SA1110_EIO_LCD_3V3_ON			SA1110_LOGIC_ONE(SA1110_EIO_LCD_3V3)
#define SA1110_EIO_RS232		0x0080
# define SA1110_EIO_RS232_OFF			SA1110_LOGIC_ZERO(SA1110_EIO_RS232)
# define SA1110_EIO_RS232_ON			SA1110_LOGIC_ONE(SA1110_EIO_RS232)
#define SA1110_EIO_LCD_CTRL		0x0100
# define SA1110_EIO_LCD_CTRL_OFF		SA1110_LOGIC_ZERO(SA1110_EIO_LCD_CTRL)
# define SA1110_EIO_LCD_CTRL_ON			SA1110_LOGIC_ONE(SA1110_EIO_LCD_CTRL)
#define SA1110_EIO_IR			0x0200
# define SA1110_EIO_IR_OFF			SA1110_LOGIC_ZERO(SA1110_EIO_IR)
# define SA1110_EIO_IR_ON			SA1110_LOGIC_ONE(SA1110_EIO_IR)
#define SA1110_EIO_AMP			0x0400
# define SA1110_EIO_AMP_OFF			SA1110_LOGIC_ZERO(SA1110_EIO_AMP)
# define SA1110_EIO_AMP_ON			SA1110_LOGIC_ONE(SA1110_EIO_AMP)
#define SA1110_EIO_AUDIO		0x0800
# define SA1110_EIO_AUDIO_OFF			SA1110_LOGIC_ZERO(SA1110_EIO_AUDIO)
# define SA1110_EIO_AUDIO_ON			SA1110_LOGIC_ONE(SA1110_EIO_AUDIO)
#define SA1110_EIO_MUTE			0x1000
# define SA1110_EIO_MUTE_OFF			SA1110_LOGIC_ZERO(SA1110_EIO_MUTE)
# define SA1110_EIO_MUTE_ON			SA1110_LOGIC_ONE(SA1110_EIO_MUTE)
#define SA1110_EIO_IR_FSEL		0x2000
# define SA1110_EIO_SIR				SA1110_LOGIC_ZERO(SA1110_EIO_IR_FSEL)
# define SA1110_EIO_FIR				SA1110_LOGIC_ONE(SA1110_EIO_IR_FSEL)
#define SA1110_EIO_LCD_5V		0x4000
# define SA1110_EIO_LCD_5V_OFF			SA1110_LOGIC_ZERO(SA1110_EIO_LCD_5V)
# define SA1110_EIO_LCD_5V_ON			SA1110_LOGIC_ONE(SA1110_EIO_LCD_5V)
#define SA1110_EIO_LCD_VDD		0x8000
# define SA1110_EIO_LCD_VDD_OFF			SA1110_LOGIC_ZERO(SA1110_EIO_LCD_VDD)
# define SA1110_EIO_LCD_VDD_ON			SA1110_LOGIC_ONE(SA1110_EIO_LCD_VDD)

//
// Special purpose GPIO interrupt mappings
//
#define SA1110_CF_IRQ               CYGNUM_HAL_INTERRUPT_GPIO21
#define SA1110_CF_DETECT            CYGNUM_HAL_INTERRUPT_GPIO17

//
// GPIO layout
//
#define SA1110_GPIO_CF_DETECT       0x00020000     // 0 = Compact Flash detect
#define SA1110_GPIO_CF_PRESENT      SA1110_LOGIC_ZERO(SA1110_GPIO_CF_DETECT)
#define SA1110_GPIO_CF_ABSENT       SA1110_LOGIC_ONE(SA1110_GPIO_CF_DETECT)

//
// LCD Controller
//
#define SA1110_LCCR0                SA11X0_REGISTER(0x30100000)
#define SA1110_LCSR                 SA11X0_REGISTER(0x30100004)
#define SA1110_DBAR1                SA11X0_REGISTER(0x30100010)
#define SA1110_DCAR1                SA11X0_REGISTER(0x30100014)
#define SA1110_DBAR2                SA11X0_REGISTER(0x30100018)
#define SA1110_DCAR2                SA11X0_REGISTER(0x3010001C)
#define SA1110_LCCR1                SA11X0_REGISTER(0x30100020)
#define SA1110_LCCR2                SA11X0_REGISTER(0x30100024)
#define SA1110_LCCR3                SA11X0_REGISTER(0x30100028)

/* end of ipaq.h                                                          */
#endif /* CYGONCE_IPAQ_H */
