//==========================================================================
//
//      loop_can.c
//
//      Loopback CAN device driver
//
//==========================================================================
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
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Uwe Kindler
// Contributors: Uwe Kindler
// Date:        2005-07-10
// Purpose:     Loopback CAN device driver
// Description: This device driver implements a pair of CAN lines that are
//              connected back-to-back. Data output to one will appear as
//              input on the other. This process in part driven by an alarm
//              object which provides a degree of separation between the two
//              channels.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/io_can.h>
#include <pkgconf/io_can_loop.h>
#include <cyg/hal/hal_io.h>

#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/can.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/kernel/kapi.h>

#ifdef CYGPKG_IO_CAN_LOOP

//-------------------------------------------------------------------------

extern void diag_printf(const char *fmt, ...);

//-------------------------------------------------------------------------
// Forward definitions

static bool loop_can_init(struct cyg_devtab_entry *devtab_entry);
static bool loop_can_putmsg(can_channel *priv, cyg_can_message *pmsg, void *pdata);
static Cyg_ErrNo loop_can_lookup(struct cyg_devtab_entry **tab, 
                                   struct cyg_devtab_entry *sub_tab,
                                   const char *name);
static bool loop_can_getevent(can_channel *priv, cyg_can_event *pevent, void *pdata);
static Cyg_ErrNo loop_can_set_config(can_channel *chan, cyg_uint32 key,
                                     const void *xbuf, cyg_uint32 *len);
static void loop_can_start_xmit(can_channel *chan);
static void loop_can_stop_xmit(can_channel *chan);

static void alarm_handler(cyg_handle_t alarm, cyg_addrword_t data);


//-------------------------------------------------------------------------
// Alarm object for feeding data back into CAN channels

static cyg_alarm alarm_obj;

static cyg_handle_t alarm_handle;

//-------------------------------------------------------------------------
// Transfer FIFOs

#define FIFO_SIZE 16

struct fifo
{
    cyg_bool                tx_enable;
    volatile int            head;
    volatile int            tail;
    volatile int            num;
    volatile cyg_can_event  buf[FIFO_SIZE+1];
};

static struct fifo fifo0 = { false, 0, 0, 0 };   // from CAN0 to CAN1
static struct fifo fifo1 = { false, 0, 0, 0 };   // from CAN1 to CAN0

//-------------------------------------------------------------------------

#define BUFSIZE 128

//-------------------------------------------------------------------------
// Info for each serial device controlled

typedef struct loop_can_info {
    struct fifo         *write_fifo;
    struct fifo         *read_fifo;
} loop_can_info;

//-------------------------------------------------------------------------
// Callback functions exported by this driver   
CAN_LOWLEVEL_FUNS(loop_can_lowlevel_funs,
                  loop_can_putmsg,
                  loop_can_getevent,
                  loop_can_set_config,
                  loop_can_start_xmit,
                  loop_can_stop_xmit
     );

//-------------------------------------------------------------------------
// Hardware info for each serial line

#ifdef CYGPKG_IO_CAN_LOOP_CAN0
static loop_can_info loop_can_info0 = {
    &fifo0,
    &fifo1
};

static cyg_can_message  loop_can_txbuf0[CYGNUM_DEVS_CAN_LOOP_CAN0_QUEUESIZE_TX];
static cyg_can_event    loop_can_rxbuf0[CYGNUM_DEVS_CAN_LOOP_CAN0_QUEUESIZE_RX];
#endif // CYGPKG_IO_SERIAL_LOOP_SERIAL0

#ifdef CYGPKG_IO_CAN_LOOP_CAN1
static loop_can_info loop_can_info1 = {
    &fifo1,
    &fifo0
};

static cyg_can_message  loop_can_txbuf1[CYGNUM_DEVS_CAN_LOOP_CAN1_QUEUESIZE_TX];
static cyg_can_event    loop_can_rxbuf1[CYGNUM_DEVS_CAN_LOOP_CAN1_QUEUESIZE_RX];
#endif // CYGPKG_IO_SERIAL_LOOP_SERIAL1



//-------------------------------------------------------------------------
// Channel descriptions:
//
#ifdef CYGPKG_IO_CAN_LOOP_CAN0
CAN_CHANNEL_USING_INTERRUPTS(loop_can0_chan,
                             loop_can_lowlevel_funs,
                             loop_can_info0,
                             CYG_CAN_BAUD_RATE(CYGNUM_DEVS_CAN_LOOP_CAN0_KBAUD),
                             loop_can_txbuf0, CYGNUM_DEVS_CAN_LOOP_CAN0_QUEUESIZE_TX,
                             loop_can_rxbuf0, CYGNUM_DEVS_CAN_LOOP_CAN0_QUEUESIZE_RX
    );
#endif // CYGPKG_IO_CAN_LOOP_CAN1
    
#ifdef CYGPKG_IO_CAN_LOOP_CAN1
CAN_CHANNEL_USING_INTERRUPTS(loop_can1_chan,
                             loop_can_lowlevel_funs,
                             loop_can_info1,
                             CYG_CAN_BAUD_RATE(CYGNUM_DEVS_CAN_LOOP_CAN1_KBAUD),
                             loop_can_txbuf1, CYGNUM_DEVS_CAN_LOOP_CAN1_QUEUESIZE_TX,
                             loop_can_rxbuf1, CYGNUM_DEVS_CAN_LOOP_CAN1_QUEUESIZE_RX
    );
#endif // CYGPKG_IO_CAN_LOOP_CAN1
 
 
  
//-------------------------------------------------------------------------
// And finally, the device table entries:
//
#ifdef CYGPKG_IO_CAN_LOOP_CAN0
DEVTAB_ENTRY(loop_can_io0, 
             CYGDAT_DEVS_CAN_LOOP_CAN0_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_can_devio, 
             loop_can_init, 
             loop_can_lookup,        // CAN driver may need initializing
             &loop_can0_chan
    );
#endif // CYGPKG_IO_CAN_LOOP_CAN0

#ifdef CYGPKG_IO_CAN_LOOP_CAN1
DEVTAB_ENTRY(loop_can_io1, 
             CYGDAT_DEVS_CAN_LOOP_CAN1_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_can_devio, 
             loop_can_init, 
             loop_can_lookup,        // CAN driver may need initializing
             &loop_can1_chan
    );
#endif // CYGPKG_IO_CAN_LOOP_CAN1

//-------------------------------------------------------------------------

static bool
loop_can_config_channel(can_channel *chan, cyg_can_info_t *new_config, bool init)
{
    if (new_config != &chan->config) {
        chan->config = *new_config;
    }
    return true;
}

//-------------------------------------------------------------------------
// Function to initialize the device.  Called at bootstrap time.

bool loop_can_init(struct cyg_devtab_entry *tab)
{
    can_channel *chan = (can_channel *)tab->priv;

    (chan->callbacks->can_init)(chan);
    
    // Set up alarm for feeding data back into channels

    cyg_alarm_create( cyg_real_time_clock(),
                      alarm_handler,
                      0,
                      &alarm_handle,
                      &alarm_obj);

    cyg_alarm_initialize( alarm_handle, 1, 1 );
    
    loop_can_config_channel(chan, &chan->config, true);
    
    return true;
}

//-------------------------------------------------------------------------
// This routine is called when the device is "looked" up (i.e. attached)

static Cyg_ErrNo 
loop_can_lookup(struct cyg_devtab_entry **tab, 
                struct cyg_devtab_entry *sub_tab,
                const char *name)
{
    can_channel *chan = (can_channel *)(*tab)->priv;
    (chan->callbacks->can_init)(chan);
    return ENOERR;
}

//-------------------------------------------------------------------------
// Return 'true' if message is sent to device

bool
loop_can_putmsg(can_channel *chan, cyg_can_message *pmsg, void *pdata)
{
    loop_can_info *loop_chan = (loop_can_info *)chan->dev_priv;

    struct fifo *fwr = loop_chan->write_fifo;
    struct fifo *frd = loop_chan->read_fifo;

    if( fwr->num == FIFO_SIZE )
    {
        return false;
    }

    fwr->buf[fwr->tail].msg  = *pmsg;
    fwr->buf[fwr->tail].flags = CYGNUM_CAN_EVENT_RX;
    fwr->num++;
    fwr->tail = (fwr->tail + 1) % FIFO_SIZE;
    
#ifdef CYGOPT_IO_CAN_TX_EVENT_SUPPORT 
    //
    // if TX events are supported we insert a TX event into read fifo
    //
    if( frd->num < FIFO_SIZE )
    {
        frd->buf[frd->tail].msg  = *pmsg;
        frd->buf[frd->tail].flags = CYGNUM_CAN_EVENT_TX;
        frd->num++;
        frd->tail = (frd->tail + 1) % FIFO_SIZE;
    }   
#endif
    
    return true;
}

//-------------------------------------------------------------------------

bool loop_can_getevent(can_channel *chan, cyg_can_event *pevent, void *pdata)
{
    loop_can_info *loop_chan = (loop_can_info *)chan->dev_priv;

    struct fifo *frd = loop_chan->read_fifo;
    
    while( frd->num == 0 )
    {
        continue;
    }

    *pevent = frd->buf[frd->head];
    
    
#ifdef CYGOPT_IO_CAN_SUPPORT_TIMESTAMP 
    //
    // if timestamps are supported then we store a actual timestamp into
    // CAN event
    // 
    pevent->timestamp = cyg_current_time();
#endif // CYGOPT_IO_CAN_SUPPORT_TIMESTAMP

    frd->num--;
    frd->head = (frd->head + 1) % FIFO_SIZE;
    
    return true;
}

//-------------------------------------------------------------------------

static Cyg_ErrNo
loop_can_set_config(can_channel *chan, cyg_uint32 key,
                    const void *xbuf, cyg_uint32 *len)
{
    return ENOERR;
}


//-------------------------------------------------------------------------
// Enable the transmitter on the device

static void
loop_can_start_xmit(can_channel *chan)
{  
    loop_can_info *loop_chan = (loop_can_info *)chan->dev_priv;

    loop_chan->write_fifo->tx_enable = true;
    
    (chan->callbacks->xmt_msg)(chan, 0);  
}

//-------------------------------------------------------------------------
// Disable the transmitter on the device

static void 
loop_can_stop_xmit(can_channel *chan)
{ 
    loop_can_info *loop_chan = (loop_can_info *)chan->dev_priv;

    loop_chan->write_fifo->tx_enable = false;  
}

//-------------------------------------------------------------------------

static void alarm_handler(cyg_handle_t alarm, cyg_addrword_t data)
{
    can_channel *chan0 = &loop_can0_chan;
    can_channel *chan1 = &loop_can1_chan;


    while(fifo0.num )
    {
        (chan1->callbacks->rcv_event)(chan1, 0);
        
        if(fifo0.tx_enable )
        {
            (chan0->callbacks->xmt_msg)(chan0, 0); 
        }       
    }

    while(fifo1.num )
    {
        (chan0->callbacks->rcv_event)(chan0, 0);
        
        if(fifo1.tx_enable )
        {
            (chan1->callbacks->xmt_msg)(chan1, 0); 
        }        
    } // while( fifo1.num )
}
    

#endif // CYGPKG_IO_CAN_LOOP

//-------------------------------------------------------------------------
// EOF loop_can.c
