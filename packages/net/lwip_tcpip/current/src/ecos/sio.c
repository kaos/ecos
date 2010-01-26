//==========================================================================
//
//      sio.c
//
//      Serial operations for SLIP, PPP etc.
//
//==========================================================================
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Simon Kallweit
// Contributors:
// Date:         2008-12-01
// Purpose:
// Description:  Serial operations for SLIP, PPP etc.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/net_lwip.h>

#include "lwip.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/sio.h"

#include <cyg/error/codes.h>
#include <cyg/io/io.h>
#include <cyg/io/serialio.h>
#include <cyg/io/config_keys.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>


/*
 * Opens a serial device for communication.
 */
sio_fd_t sio_open(u8_t devnum)
{
    Cyg_ErrNo ret;
    cyg_io_handle_t handle;
    char *dev;
    
#ifdef CYGDBG_LWIP_DEBUG_SIO
    diag_printf("sio_open(devnum=%d)\n", devnum);
#endif

    switch (devnum) {
#ifdef CYGPKG_LWIP_SLIP
    case SIO_DEV_SLIPIF:
        dev = CYGDAT_LWIP_SLIPIF_DEV;
        break;
#endif
#ifdef CYGFUN_LWIP_PPPOS_SUPPORT
    case SIO_DEV_PPPOS:
        dev = CYGDAT_LWIP_PPPOS_DEV;
        break;
#endif
    default:
        // Unknown serial io device
        return NULL;
        break;
    }

    ret = cyg_io_lookup(dev, &handle);
    if (ret != ENOERR)
        return NULL;

    return handle;
}

/*
 * Sends a single character to the serial device.
 */
void sio_send(u8_t c, sio_fd_t fd)
{
    cyg_uint32 len = 1;
    
#ifdef CYGDBG_LWIP_DEBUG_SIO
    diag_printf("sio_send(c=0x%02x,fd=%p)\n", c, fd);
#endif
    
	cyg_io_write((cyg_io_handle_t) fd, &c, &len);
}

/*
 * Receives a single character from the serial device.
 */
u8_t sio_recv(sio_fd_t fd)
{
    cyg_uint32 len = 1;
    char c;
    
#ifdef CYGDBG_LWIP_DEBUG_SIO
    diag_printf("sio_recv(fd=%p)\n", fd);
#endif
    
	cyg_io_read((cyg_io_handle_t) fd, &c, &len);
	
#ifdef CYGDBG_LWIP_DEBUG_SIO
    diag_printf("sio_recv: %02X\n", (cyg_uint8) c);
#endif
	
	return c;
}

/*
 * Reads from the serial device.
 */
u32_t sio_read(sio_fd_t fd, u8_t *data, u32_t len)
{
    Cyg_ErrNo ret;
    
#ifdef CYGDBG_LWIP_DEBUG_SIO
    diag_printf("sio_read(fd=%p,data=%p,len=%lu:)\n", fd, data, len);
#endif
    
    ret = cyg_io_read((cyg_io_handle_t) fd, data, (cyg_uint32 *) &len);
    if (ret != ENOERR)
        return 0;

#ifdef CYGDBG_LWIP_DEBUG_SIO
    diag_printf("sio_read: ");
    diag_dump_buf(data, len);
#endif

    return len;
}

/*
 * Tries to read from the serial device. Same as sio_read but returns
 * immediately if no data is available and never blocks.
 */
u32_t sio_tryread(sio_fd_t fd, u8_t *data, u32_t len)
{
    Cyg_ErrNo ret;
    cyg_serial_buf_info_t info;
    cyg_uint32 l;
    
#ifdef CYGDBG_LWIP_DEBUG_SIO
    diag_printf("sio_tryread(fd=%p,data=%p,len=%lu:)\n", fd, data, len);
#endif

    // Check how many bytes there are to read
    l = sizeof(info);
    ret = cyg_io_get_config((cyg_io_handle_t) fd, CYG_IO_GET_CONFIG_SERIAL_BUFFER_INFO, &info, &l);
    if (ret != ENOERR)
        return 0;
    l = info.rx_count;
    if (l < 1)
        return 0;
    if (l > len)
        l = len;

    ret = cyg_io_read((cyg_io_handle_t) fd, data, &l);
    if (ret != ENOERR)
        return 0;
    
#ifdef CYGDBG_LWIP_DEBUG_SIO
   diag_printf("sio_tryread: ");
   diag_dump_buf(data, len);
#endif
   
   return l;
}

/*
 * Writes to the serial device.
 */
u32_t sio_write(sio_fd_t fd, u8_t *data, u32_t len)
{
    Cyg_ErrNo ret;
    cyg_uint32 count = 0;
    cyg_uint32 chunk;
    
#ifdef CYGDBG_LWIP_DEBUG_SIO
    diag_printf("sio_write(fd=%p,data=%p,len=%lu:)\n", fd, data, len);
    diag_printf("sio_write: ");
    diag_dump_buf(data, len);
#endif

    while (count < len) {
        chunk = len - count;
        ret = cyg_io_write((cyg_io_handle_t) fd, data, &chunk);
        if (ret != ENOERR)
            break;
        data += chunk;
        count += chunk;
    }
    
	return count;
}

/*
 * Aborts a blocking sio_read() call.
 */
void sio_read_abort(sio_fd_t fd)
{
    cyg_uint32 l = 0;
    
#ifdef CYGDBG_LWIP_DEBUG_SIO
    diag_printf("sio_read_abort(fd=%p)\n", fd);
#endif

   cyg_io_set_config((cyg_io_handle_t) fd, CYG_IO_GET_CONFIG_SERIAL_ABORT, NULL, &l);
}
