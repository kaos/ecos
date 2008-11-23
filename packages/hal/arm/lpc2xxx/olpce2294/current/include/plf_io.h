#ifndef CYGONCE_HAL_PLF_IO_H
#define CYGONCE_HAL_PLF_IO_H
//=============================================================================
//
//      plf_io.h
//
//      Olimex LPC-E2294 board specific registers
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2008 eCosCentric Limited
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
// Author(s):     Sergei Gavrikov
// Contributors:  Sergei Gavrikov
// Date:          2008-08-31
// Purpose:       Olimex LPC-E2294 board specific registers
// Description:
// Usage:         #include <cyg/hal/plf_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================
// On-chip device base addresses

#ifndef __ASSEMBLER__
extern void hal_plf_hardware_init(void);
#define HAL_PLF_HARDWARE_INIT() \
	    hal_plf_hardware_init()

#define IO0PIN (*(volatile unsigned int *)0xE0028000)
#define IO0SET (*(volatile unsigned int *)0xE0028004)
#define IO0DIR (*(volatile unsigned int *)0xE0028008)
#define IO0CLR (*(volatile unsigned int *)0xE002800C)

#define IO1PIN (*(volatile unsigned int *)0xE0028010)
#define IO1SET (*(volatile unsigned int *)0xE0028014)
#define IO1DIR (*(volatile unsigned int *)0xE0028018)
#define IO1CLR (*(volatile unsigned int *)0xE002801C)

#define IO2PIN (*(volatile unsigned int *)0xE0028020)
#define IO2SET (*(volatile unsigned int *)0xE0028024)
#define IO2DIR (*(volatile unsigned int *)0xE0028028)
#define IO2CLR (*(volatile unsigned int *)0xE002802C)

#define IO3PIN (*(volatile unsigned int *)0xE0028030)
#define IO3SET (*(volatile unsigned int *)0xE0028034)
#define IO3DIR (*(volatile unsigned int *)0xE0028038)
#define IO3CLR (*(volatile unsigned int *)0xE002803C)

#define PINSEL0 (*(volatile unsigned int *)0xE002C00)
#define PINSEL1 (*(volatile unsigned int *)0xE002C04)

#endif // __ASSEMBLER__

//-----------------------------------------------------------------------------
// end of plf_io.h
#endif // CYGONCE_HAL_PLF_IO_H

