//==========================================================================
//
//      io/serial/common/tty.c
//
//      TTY (terminal-like interface) I/O driver
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   gthomas
// Contributors:  gthomas
// Date:        1999-02-04
// Purpose:     Device driver for tty I/O, layered on top of serial I/O
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/io.h>
#include <pkgconf/io_serial.h>
#ifdef CYGPKG_IO_SERIAL_TTY
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/ttyio.h>
#include <cyg/infra/diag.h>

static bool tty_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo tty_lookup(struct cyg_devtab_entry **tab, 
                               struct cyg_devtab_entry *sub_tab,
                               const char *name);
static Cyg_ErrNo tty_write(cyg_io_handle_t handle, const void *buf, cyg_uint32 *len);
static Cyg_ErrNo tty_read(cyg_io_handle_t handle, void *buf, cyg_uint32 *len);
static Cyg_ErrNo tty_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *buf, cyg_uint32 *len);
static Cyg_ErrNo tty_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buf, cyg_uint32 *len);

struct tty_private_info {
    cyg_tty_info_t     dev_info;
    cyg_io_handle_t dev_handle;
};

static DEVIO_TABLE(tty_devio,
                   tty_write,
                   tty_read,
                   tty_get_config,
                   tty_set_config
    );

#ifdef CYGPKG_IO_SERIAL_TTY_TTYDIAG
static struct tty_private_info tty_private_info_diag;
DEVTAB_ENTRY(tty_io_diag, 
//             "/dev/console",       
//             CYGDAT_IO_SERIAL_TTY_CONSOLE,   // Based on driver for this device
             "/dev/ttydiag",
             "/dev/haldiag",
             &tty_devio, 
             tty_init, 
             tty_lookup,      // Execute this when device is being looked up
             &tty_private_info_diag);
#endif

#ifdef CYGPKG_IO_SERIAL_TTY_TTY0
static struct tty_private_info tty_private_info0;
DEVTAB_ENTRY(tty_io0, 
             "/dev/tty0",
             CYGDAT_IO_SERIAL_TTY_TTY0_DEV,
             &tty_devio, 
             tty_init, 
             tty_lookup,      // Execute this when device is being looked up
             &tty_private_info0);
#endif

#ifdef CYGPKG_IO_SERIAL_TTY_TTY1
static struct tty_private_info tty_private_info1;
DEVTAB_ENTRY(tty_io1, 
             "/dev/tty1", 
             CYGDAT_IO_SERIAL_TTY_TTY1_DEV,
             &tty_devio, 
             tty_init, 
             tty_lookup,      // Execute this when device is being looked up
             &tty_private_info1);
#endif

#ifdef CYGPKG_IO_SERIAL_TTY_TTY2
static struct tty_private_info tty_private_info2;
DEVTAB_ENTRY(tty_io2, 
             "/dev/tty2", 
             CYGDAT_IO_SERIAL_TTY_TTY2_DEV,
             &tty_devio, 
             tty_init, 
             tty_lookup,      // Execute this when device is being looked up
             &tty_private_info2);
#endif

static bool 
tty_init(struct cyg_devtab_entry *tab)
{
    struct tty_private_info *priv = (struct tty_private_info *)tab->priv;
#ifdef CYGDBG_IO_INIT
    diag_printf("Init tty channel: %x\n", tab);
#endif
    priv->dev_info.tty_out_flags = CYG_TTY_OUT_FLAGS_DEFAULT;
    priv->dev_info.tty_in_flags = CYG_TTY_IN_FLAGS_DEFAULT;
    return true;
}

static Cyg_ErrNo 
tty_lookup(struct cyg_devtab_entry **tab, 
           struct cyg_devtab_entry *sub_tab,
           const char *name)
{
    cyg_io_handle_t chan = (cyg_io_handle_t)sub_tab;
    struct tty_private_info *priv = (struct tty_private_info *)(*tab)->priv;
#if 0
    cyg_int32 len;
#endif
    priv->dev_handle = chan;
#if 0
    len = sizeof(cyg_serial_info_t);
    // Initialize configuration
    cyg_io_get_config(chan, CYG_SERIAL_GET_CONFIG, 
                      (void *)&priv->dev_info.serial_config, &len);
#endif
    return ENOERR;
}

#define BUFSIZE 64

static Cyg_ErrNo 
tty_write(cyg_io_handle_t handle, const void *_buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t *t = (cyg_devtab_entry_t *)handle;
    struct tty_private_info *priv = (struct tty_private_info *)t->priv;
    cyg_io_handle_t chan = (cyg_io_handle_t)priv->dev_handle;
    cyg_int32 size, bytes_successful, actually_written;
    cyg_uint8 xbuf[BUFSIZE];
    cyg_uint8 c;
    cyg_uint8 *buf = (cyg_uint8 *)_buf;
    Cyg_ErrNo res = -EBADF;
    // assert(chan)
    size = 0;
    bytes_successful = 0;
    actually_written = 0;
    while (bytes_successful++ < *len) {
        xbuf[size++] = (c = *buf++);
        if ((c == '\n') &&
            (priv->dev_info.tty_out_flags & CYG_TTY_OUT_FLAGS_CRLF)) {
            xbuf[size++] = '\r';
        }
        // Always leave room for possible CR/LF expansion
        if ((size == (BUFSIZE-1)) ||
            (bytes_successful == *len)) {
            res = cyg_io_write(chan, xbuf, &size);
            if (res != ENOERR) {
                *len = actually_written;
                return res;
            }
            actually_written += size;
            size = 0;
        }
    }
    *len = actually_written;
    return res;
}

static Cyg_ErrNo 
tty_read(cyg_io_handle_t handle, void *_buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t *t = (cyg_devtab_entry_t *)handle;
    struct tty_private_info *priv = (struct tty_private_info *)t->priv;
    cyg_io_handle_t chan = (cyg_io_handle_t)priv->dev_handle;
    cyg_uint32 clen;
    cyg_int32 size;
    Cyg_ErrNo res;
    cyg_uint8 c;
    cyg_uint8 *buf = (cyg_uint8 *)_buf;
    // assert(chan)
    size = 0;
    while (size < *len) {
        clen = 1;
        res = cyg_io_read(chan, &c, &clen);
        if (res != ENOERR) {
            *len = size;
            return res;
        }
        buf[size++] = c;
        if ((priv->dev_info.tty_in_flags & CYG_TTY_IN_FLAGS_BINARY) == 0) {
            if ((c == '\b') || (c == 0x7F)) {
                size -= 2;  // erase one character + 'backspace' char
                if (size < 0) {
                    size = 0;
                } else if (priv->dev_info.tty_in_flags & CYG_TTY_IN_FLAGS_ECHO) {
                    clen = 3;
                    cyg_io_write(chan, "\b \b", &clen);
                }
            } else if ((c == '\n') || (c == '\r')) {
                clen = 2;
                if (priv->dev_info.tty_in_flags & CYG_TTY_IN_FLAGS_ECHO) {
                    cyg_io_write(chan, "\n\r", &clen);
                }
                if (priv->dev_info.tty_in_flags & CYG_TTY_IN_FLAGS_CRLF) {
                    c = '\n';  // Map CR -> LF
                }
                buf[size-1] = c;
                break;
            } else {
                if (priv->dev_info.tty_in_flags & CYG_TTY_IN_FLAGS_ECHO) {
                    clen = 1;
                    cyg_io_write(chan, &c, &clen);
                }
            }
        }
    }
    *len = size;
    return ENOERR;
}

static Cyg_ErrNo 
tty_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t *t = (cyg_devtab_entry_t *)handle;
    struct tty_private_info *priv = (struct tty_private_info *)t->priv;
    cyg_io_handle_t chan = (cyg_io_handle_t)priv->dev_handle;
    Cyg_ErrNo res = ENOERR;
#if 0
    cyg_int32 current_len;
#endif
    // assert(chan)
    switch (key) {
    case CYG_IO_GET_CONFIG_TTY_INFO:
        if (*len < sizeof(cyg_tty_info_t)) {
            return -EINVAL;
        }
#if 0
        current_len = sizeof(cyg_serial_info_t);
        res = cyg_io_get_config(chan, CYG_SERIAL_GET_CONFIG, 
                                (void *)&priv->dev_info.serial_config, &current_len);
        if (res != ENOERR) {
            return res;
        }
#endif
        *(cyg_tty_info_t *)buf = priv->dev_info;
        *len = sizeof(cyg_tty_info_t);
        break;
    default:  // Assume this is a 'serial' driver control
        res = cyg_io_get_config(chan, key, buf, len);
    }
    return res;
}

static Cyg_ErrNo 
tty_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t *t = (cyg_devtab_entry_t *)handle;
    struct tty_private_info *priv = (struct tty_private_info *)t->priv;
    cyg_io_handle_t chan = (cyg_io_handle_t)priv->dev_handle;
#if 0
    cyg_int32 current_len;
#endif
    Cyg_ErrNo res = ENOERR;
    // assert(chan)
    switch (key) {
    case CYG_IO_SET_CONFIG_TTY_INFO:
        if (*len != sizeof(cyg_tty_info_t)) {
            return -EINVAL;
        }
        priv->dev_info = *(cyg_tty_info_t *)buf;
        break;
    default: // Pass on to serial driver
        res = cyg_io_set_config(chan, key, buf, len);
    }
    return res;
}
#endif // CYGPKG_IO_SERIAL_TTY
