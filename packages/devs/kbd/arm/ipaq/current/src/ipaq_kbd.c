//==========================================================================
//
//      ipaq_kbd.c
//
//      Keypad driver for the Compaq iPAQ
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
// Date:         2001-03-15
// Purpose:      
// Description:  Keypad driver for Compaq IPAQ
//
//####DESCRIPTIONEND####
//
//==========================================================================


#include <pkgconf/devs_kbd_ipaq.h>

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
static cyg_selinfo      kbd_select_info; 
static cyg_bool         kbd_select_active;

#include <cyg/io/devtab.h>
#include <cyg/hal/atmel_support.h>

// Functions in this module

static Cyg_ErrNo kbd_read(cyg_io_handle_t handle, 
                          void *buffer, 
                          cyg_uint32 *len);
static cyg_bool  kbd_select(cyg_io_handle_t handle, 
                            cyg_uint32 which, 
                            cyg_addrword_t info);
static Cyg_ErrNo kbd_set_config(cyg_io_handle_t handle, 
                                cyg_uint32 key, 
                                const void *buffer, 
                                cyg_uint32 *len);
static Cyg_ErrNo kbd_get_config(cyg_io_handle_t handle, 
                                cyg_uint32 key, 
                                void *buffer, 
                                cyg_uint32 *len);
static bool      kbd_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo kbd_lookup(struct cyg_devtab_entry **tab, 
                            struct cyg_devtab_entry *st, 
                            const char *name);

CHAR_DEVIO_TABLE(ipaq_kbd_handlers,
                 NULL,                                   // Unsupported write() function
                 kbd_read,
                 kbd_select,
                 kbd_get_config,
                 kbd_set_config);

CHAR_DEVTAB_ENTRY(ipaq_kbd_device,
                  CYGDAT_DEVS_KBD_IPAQ_NAME,
                  NULL,                                   // Base device name
                  &ipaq_kbd_handlers,
                  kbd_init,
                  kbd_lookup,
                  NULL);                                  // Private data pointer

#define MAX_EVENTS CYGNUM_DEVS_KBD_IPAQ_EVENT_BUFFER_SIZE
static int   num_events;
static int   _event_put, _event_get;
static unsigned char _events[MAX_EVENTS];

static bool _is_open = false;

//
// Note: this routine is called from the Atmel processing DSR
//
static void
kbd_handler(atmel_pkt *pkt)
{
    unsigned char *dp = pkt->data;
    unsigned char *ev;

//    diag_printf("KBD = %x\n", dp[1]);
    if (num_events < MAX_EVENTS) {
        num_events++;
        ev = &_events[_event_put++];
        if (_event_put == MAX_EVENTS) {
            _event_put = 0;
        }
        *ev = dp[1];
        if (kbd_select_active) {
            kbd_select_active = false;
            cyg_selwakeup(&kbd_select_info);
        }
    }
}

static Cyg_ErrNo 
kbd_read(cyg_io_handle_t handle, 
         void *buffer, 
         cyg_uint32 *len)
{
    unsigned char *ev;
    int tot = *len;
    unsigned char *bp = (unsigned char *)buffer;

    cyg_scheduler_lock();  // Prevent interaction with DSR code
    while (tot >= sizeof(*ev)) {
        if (num_events > 0) {
            ev = &_events[_event_get++];
            if (_event_get == MAX_EVENTS) {
                _event_get = 0;
            }
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
kbd_select(cyg_io_handle_t handle, 
           cyg_uint32 which, 
           cyg_addrword_t info)
{
    if (which == CYG_FREAD) {
        cyg_scheduler_lock();  // Prevent interaction with DSR code
        if (num_events > 0) {
            cyg_scheduler_unlock();  // Reallow interaction with DSR code
            return true;
        }        
        if (!kbd_select_active) {
            kbd_select_active = true;
            cyg_selrecord(info, &kbd_select_info);
        }
        cyg_scheduler_unlock();  // Reallow interaction with DSR code
    }
    return false;
}

static Cyg_ErrNo 
kbd_set_config(cyg_io_handle_t handle, 
               cyg_uint32 key, 
               const void *buffer, 
               cyg_uint32 *len)
{
    return EINVAL;
}

static Cyg_ErrNo 
kbd_get_config(cyg_io_handle_t handle, 
               cyg_uint32 key, 
               void *buffer, 
               cyg_uint32 *len)
{
    return EINVAL;
}

static bool      
kbd_init(struct cyg_devtab_entry *tab)
{
    cyg_selinit(&kbd_select_info);
    return true;
}

static Cyg_ErrNo 
kbd_lookup(struct cyg_devtab_entry **tab, 
           struct cyg_devtab_entry *st, 
           const char *name)
{
    if (!_is_open) {
        _is_open = true;
        atmel_register(ATMEL_CMD_KEYBD, kbd_handler);
        atmel_interrupt_mode(true);
    }
    return ENOERR;
}
