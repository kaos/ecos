//==========================================================================
//
//      lpc2xxx_wallclock.cxx
//
//      Wallclock implementation for LPC2xxx
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Gary Thomas
// Copyright (C) 2004 eCosCentric Ltd
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
// Author(s):     Hans Rosenfeld <rosenfeld@grumpf.hope-2000.org>
// Contributors:  
// Date:          2007-06-19
// Purpose:       Wallclock driver for LPC2xxx
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/wallclock.h>
#include <pkgconf/devices_wallclock_arm_lpc2xxx.h>

#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/infra/cyg_type.h>

#include <cyg/io/wallclock.hxx>
#include <cyg/io/wallclock/wallclock.inl>

/*
 * I don't like to write LOTS OF CAPITALIZED TEXT.
 * This code is intended for LPC2xxx processors _only_, so there is nothing
 * wrong with accessing this device directly without using the HAL macros.
 */

struct time {
  volatile cyg_uint32 sec;
  volatile cyg_uint32 min;
  volatile cyg_uint32 hour;
  volatile cyg_uint32 dom;
  volatile cyg_uint32 dow;
  volatile cyg_uint32 doy;
  volatile cyg_uint32 month;
  volatile cyg_uint32 year;
};

struct rtcdev {
  volatile cyg_uint32 ilr;
  volatile cyg_uint32 ctc;
  volatile cyg_uint32 ccr;
  volatile cyg_uint32 ciir;
  volatile cyg_uint32 amr;
  volatile cyg_uint32 ctime[3];
  struct time time;
  cyg_uint32 dummy[8];
  struct time alarm;
  volatile cyg_uint32 preint;
  volatile cyg_uint32 prefrac;
};

static struct rtcdev * const rtc = 
  (struct rtcdev *) CYGARC_HAL_LPC2XXX_REG_RTC_BASE;

void
Cyg_WallClock::init_hw_seconds(void)
{
  /* halt clock, disable interrupts, disable alarm */
  rtc->ccr  = 0x2;
  rtc->ciir = 0x0;
  rtc->amr  = 0xf;
  
  /* initialize prescaler */
  rtc->preint  = CYGNUM_HAL_ARM_LPC2XXX_RTCDEV_PREINT;
  rtc->prefrac = CYGNUM_HAL_ARM_LPC2XXX_RTCDEV_PREFRAC;
  
#ifndef CYGSEM_WALLCLOCK_SET_GET_MODE
  /* reset time to the Unix Epoch */
  rtc->time.year  = 1970;
  rtc->time.month = 1;
  rtc->time.dom   = 1;
  rtc->time.doy   = 1;
  rtc->time.dow   = 4;
  rtc->time.hour  = 0;
  rtc->time.min   = 0;
  rtc->time.sec   = 0;
#endif
  
  /* reset alarm */
  rtc->alarm.year = rtc->alarm.month = rtc->alarm.dom = rtc->alarm.doy =
    rtc->alarm.dow = rtc->alarm.hour = rtc->alarm.min = 
    rtc->alarm.sec = 0;
  
  /* start clock */
  rtc->ccr = 0x1;
}

cyg_uint32
Cyg_WallClock::get_hw_seconds(void)
{
  return _simple_mktime(rtc->time.year,
                        rtc->time.month,
                        rtc->time.dom,
                        rtc->time.hour,
                        rtc->time.min,
                        rtc->time.sec);
}

#ifdef CYGSEM_WALLCLOCK_SET_GET_MODE
void
Cyg_WallClock::set_hw_seconds(cyg_uint32 secs)
{
  cyg_uint32 year, month, dom, hour, min, sec;
  
  /* halt clock, reset counter */
  rtc->ccr = 0x2;
  
  /* set time */
  _simple_mkdate(secs, &year, &month, &dom, &hour, &min, &sec);
  rtc->time.year  = year;
  rtc->time.month = month;
  rtc->time.dom   = dom;
  rtc->time.hour  = hour;
  rtc->time.min   = min;
  rtc->time.sec   = sec;
  
  /* restart clock */
  rtc->ccr = 0x1;
}
#endif
