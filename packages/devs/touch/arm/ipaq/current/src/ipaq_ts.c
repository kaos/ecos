//==========================================================================
//
//      ipaq_ts.c
//
//      Touchscreen driver for the Compaq iPAQ
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
// Copyright (C) 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
//-------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2001-03-05
// Purpose:      
// Description:  Touchscreen driver for Compaq IPAQ
//
//####DESCRIPTIONEND####
//
//==========================================================================


#include <pkgconf/devs_touch_ipaq.h>

#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_sa11x0.h>
#include <cyg/hal/ipaq.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>

#include <cyg/fileio/fileio.h>  // For select() functionality
static cyg_selinfo      ts_select_info; 
static cyg_bool         ts_select_active;

#include <cyg/io/devtab.h>
#include <cyg/hal/atmel_support.h>

// Functions in this module

static Cyg_ErrNo ts_read(cyg_io_handle_t handle, 
                         void *buffer, 
                         cyg_uint32 *len);
static cyg_bool  ts_select(cyg_io_handle_t handle, 
                           cyg_uint32 which, 
                           cyg_addrword_t info);
static Cyg_ErrNo ts_set_config(cyg_io_handle_t handle, 
                               cyg_uint32 key, 
                               const void *buffer, 
                               cyg_uint32 *len);
static Cyg_ErrNo ts_get_config(cyg_io_handle_t handle, 
                               cyg_uint32 key, 
                               void *buffer, 
                               cyg_uint32 *len);
static bool      ts_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo ts_lookup(struct cyg_devtab_entry **tab, 
                           struct cyg_devtab_entry *st, 
                           const char *name);

CHAR_DEVIO_TABLE(ipaq_ts_handlers,
                 NULL,                                   // Unsupported write() function
                 ts_read,
                 ts_select,
                 ts_get_config,
                 ts_set_config);

CHAR_DEVTAB_ENTRY(ipaq_ts_device,
                  CYGDAT_DEVS_TOUCH_IPAQ_NAME,
                  NULL,                                   // Base device name
                  &ipaq_ts_handlers,
                  ts_init,
                  ts_lookup,
                  NULL);                                  // Private data pointer

struct _event {
    short button_state;
    short xPos, yPos;
    short _unused;
};
#define MAX_EVENTS CYGNUM_DEVS_TOUCH_IPAQ_EVENT_BUFFER_SIZE
static int   num_events;
static int   _event_put, _event_get;
static bool  pen_down = false;
static struct _event _events[MAX_EVENTS];

static bool _is_open = false;
#ifdef DEBUG_RAW_EVENTS
static unsigned char _ts_buf[512];
static int _ts_buf_ptr = 0;
#endif

//
// Note: this routine is called from the Atmel processing DSR
//
static void
ts_handler(atmel_pkt *pkt)
{
    unsigned char *dp = pkt->data;
    static short lastX, lastY;
    short x, y;
    struct _event *ev;

#ifdef DEBUG_RAW_EVENTS
    memcpy(&_ts_buf[_ts_buf_ptr], pkt->data, 8);
    _ts_buf_ptr += 8;
    if (_ts_buf_ptr == 512) {
        diag_printf("TS handler\n");
        diag_dump_buf(_ts_buf, 512);
        _ts_buf_ptr = 0;
    }
#endif
    // Try and interpret the mouse data
    if ((dp[0] & 0x0F) == 0) {
        // This is a pen up event
        x = lastX;
        y = lastY;
        pen_down = false;
    } else {
        // Some sort of event with the pen down        
        x = lastX = (dp[1] << 8) | dp[2];
        y = lastY = (dp[3] << 8) | dp[4];
        pen_down = true;
    }
    if (num_events < MAX_EVENTS) {
        num_events++;
        ev = &_events[_event_put++];
        if (_event_put == MAX_EVENTS) {
            _event_put = 0;
        }
        ev->button_state = pen_down ? 0x04 : 0x00;
        ev->xPos = x;
        ev->yPos = y;
        if (ts_select_active) {
            ts_select_active = false;
            cyg_selwakeup(&ts_select_info);
        }
    }
}

typedef struct {
    short min;
    short max;
    short span;
} bounds;

static bounds xBounds = {1024, 0, 1024};
static bounds yBounds = {1024, 0, 1024};

static Cyg_ErrNo 
ts_read(cyg_io_handle_t handle, 
        void *buffer, 
        cyg_uint32 *len)
{
    struct _event *ev;
    int tot = *len;
    unsigned char *bp = (unsigned char *)buffer;

    cyg_scheduler_lock();  // Prevent interaction with DSR code
    while (tot >= sizeof(struct _event)) {
        if (num_events > 0) {
            ev = &_events[_event_get++];
            if (_event_get == MAX_EVENTS) {
                _event_get = 0;
            }
            // Self calibrate
            if (ev->xPos > xBounds.max) xBounds.max = ev->xPos;
            if (ev->xPos < xBounds.min) xBounds.min = ev->xPos;
            if (ev->yPos > yBounds.max) yBounds.max = ev->yPos;
            if (ev->yPos < yBounds.min) yBounds.min = ev->yPos;
            if ((xBounds.span = xBounds.max - xBounds.min) <= 1) {
                xBounds.span = 1;
            }
            if ((yBounds.span = yBounds.max - yBounds.min) <= 1) {
                yBounds.span = 1;
            }
            // Scale values - done here so these potentially lengthy
            // operations take place outside of interrupt processing
            ev->xPos = ((xBounds.max - ev->xPos) * 320) / xBounds.span;
            ev->yPos = ((yBounds.max - ev->yPos) * 240) / yBounds.span;
            memcpy(bp, ev, sizeof(*ev));
            bp += sizeof(*ev);
            tot -= sizeof(*ev);
            num_events--;
        } else {
            break;  // No more events
        }
    }
    cyg_scheduler_unlock(); // Allow DSRs again
    *len -= tot;
    return ENOERR;
}

static cyg_bool  
ts_select(cyg_io_handle_t handle, 
          cyg_uint32 which, 
          cyg_addrword_t info)
{
    if (which == CYG_FREAD) {
        cyg_scheduler_lock();  // Prevent interaction with DSR code
        if (num_events > 0) {
            cyg_scheduler_unlock();  // Reallow interaction with DSR code
            return true;
        }        
        if (!ts_select_active) {
            ts_select_active = true;
            cyg_selrecord(info, &ts_select_info);
        }
        cyg_scheduler_unlock();  // Reallow interaction with DSR code
    }
    return false;
}

static Cyg_ErrNo 
ts_set_config(cyg_io_handle_t handle, 
              cyg_uint32 key, 
              const void *buffer, 
              cyg_uint32 *len)
{
    return EINVAL;
}

static Cyg_ErrNo 
ts_get_config(cyg_io_handle_t handle, 
              cyg_uint32 key, 
              void *buffer, 
              cyg_uint32 *len)
{
    return EINVAL;
}

static bool      
ts_init(struct cyg_devtab_entry *tab)
{
    cyg_selinit(&ts_select_info);
    return true;
}

static Cyg_ErrNo 
ts_lookup(struct cyg_devtab_entry **tab, 
          struct cyg_devtab_entry *st, 
          const char *name)
{
    if (!_is_open) {
        _is_open = true;
        atmel_register(ATMEL_CMD_TOUCH, ts_handler);
        atmel_interrupt_mode(true);
    }
    return ENOERR;
}
