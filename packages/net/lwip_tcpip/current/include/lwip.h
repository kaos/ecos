#ifndef CYGONCE_LWIP_H
#define CYGONCE_LWIP_H
//=============================================================================
//
//      lwip.h
//
//      lwIP networking stack.
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008, 2009 Free Software Foundation
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Simon Kallweit
// Date:          2008-12-09
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

#include <lwip/opt.h>
#include <lwip/debug.h>
#include <lwip/stats.h>
#include <lwip/tcp.h>
#include "lwip/sys.h"
#include "lwip/api.h"

// Serial device id's
#define SIO_DEV_SLIPIF  0
#define SIO_DEV_PPPOS   1
    
#ifdef CYGFUN_LWIP_MODE_SIMPLE

externC void cyg_lwip_simple_init(void);
externC void cyg_lwip_simple_restart(void);
externC void cyg_lwip_simple_poll(void);

#endif // CYGFUN_LWIP_MODE_SIMPLE


#ifdef CYGFUN_LWIP_MODE_SEQUENTIAL

externC void cyg_lwip_sequential_init(void);

externC sys_thread_t cyg_lwip_thread_new(char *name,
                                         void (* thread)(void *arg), void *arg,
                                         void *stack, int stacksize, int prio);

#endif // CYGFUN_LWIP_MODE_SEQUENTIAL


#ifdef __cplusplus
}
#endif

//-----------------------------------------------------------------------------
#endif // CYGONCE_LWIP_H
