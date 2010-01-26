//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008 Free Software Foundation
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

#ifndef __LWIP_SYS_ARCH_H__
#define __LWIP_SYS_ARCH_H__

#include <cyg/kernel/kapi.h>

#include <lwip/opt.h>

#if !NO_SYS

typedef cyg_sem_t *     sys_sem_t;
typedef cyg_handle_t    sys_mbox_t;
typedef cyg_handle_t    sys_thread_t;

#define SYS_SEM_NULL    ((sys_sem_t) NULL)
#define SYS_MBOX_NULL   ((sys_mbox_t) NULL)

#endif // !NO_SYS

#endif // __LWIP_SYS_ARCH_H__
