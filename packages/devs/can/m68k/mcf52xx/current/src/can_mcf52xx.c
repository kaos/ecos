//==========================================================================
//
//      devs/serial/m68k/flexcan/current/src/can_mcf_flexcan.c
//
//      CAN driver for Motorola coldfire processors
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Gary Thomas
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
// Author(s):    Uwe Kindler
// Contributors: Uwe Kindler
// Date:         2005-05-12
// Purpose:      support coldfire on-chip flexcan moduls
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================


//==========================================================================
//                              INCLUDES
//==========================================================================
#include <pkgconf/system.h>
#include <pkgconf/io_can.h>
#include <pkgconf/devs_can_mcf52xx_flexcan.h>

#include <cyg/infra/diag.h>

#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/can.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_io.h>


//===========================================================================
//                                DEFINES  
//===========================================================================

//
// we define our own ste of register bits in order to be independent from
// platform specific names
//

//---------------------------------------------------------------------------
// MCR regsiter bits
//
#define FLEXCAN_MCR_STOP                    (0x01 << 15)
#define FLEXCAN_MCR_FRZ                     (0x01 << 14)
#define FLEXCAN_MCR_HALT                    (0x01 << 12)
#define FLEXCAN_MCR_NOTRDY                  (0x01 << 11)
#define FLEXCAN_MCR_WAKEMSK                 (0x01 << 10)
#define FLEXCAN_MCR_SOFTRST                 (0x01 << 9)
#define FLEXCAN_MCR_FRZACK                  (0x01 << 8)
#define FLEXCAN_MCR_SUPV                    (0x01 << 7)
#define FLEXCAN_MCR_SELFWAKE                (0x01 << 6)
#define FLEXCAN_MCR_APS                     (0x01 << 5)
#define FLEXCAN_MCR_STOPACK                 (0x01 << 4)


//---------------------------------------------------------------------------
// CTRL0 register bits
//
#define FLEXCAN_CTRL0_BOFFMSK                   (0x01 << 7)
#define FLEXCAN_CTRL0_ERRMASK                   (0x01 << 6)
#define FLEXCAN_CTRL0_RXMODE                    (0x01 << 2)
#define FLEXCAN_CTRL0_RXMODE_0_DOMINANT         (0x00 << 2)
#define FLEXCAN_CTRL0_RXMODE_1_DOMINANT         (0x01 << 2)
#define FLEXCAN_CTRL0_TXMODE_MASK               (0x03 << 0)
#define FLEXCAN_CTRL0_TXMODE_SHIFT              0
#define FLEXCAN_CTRL0_TXMODE_FULL_0_DOMINANT    (0x00 << 0)
#define FLEXCAN_CTRL0_TXMODE_FULL_1_DOMINANT    (0x01 << 0)
#define FLEXCAN_CTRL0_TXMODE_OPEN_0_DOMINANT    (0x02 << 0)


//---------------------------------------------------------------------------
// CTRL1 register bits
//
#define FLEXCAN_CTRL1_SAMP                      (0x01 << 7)
#define FLEXCAN_CTRL1_TSYNC                     (0x01 << 5)
#define FLEXCAN_CTRL1_LBUF                      (0x01 << 4)
#define FLEXCAN_CTRL1_LOM                       (0x01 << 3)
#define FLEXCAN_CTRL1_PROPSEG_MASK              (0x07 << 0)
#define FLEXCAN_CTRL1_PROPSEG_SHIFT             0


//---------------------------------------------------------------------------
// CTRL2 register bits
//
#define FLEXCAN_CTRL2_RJW_MASK                  (0x03 << 6)
#define FLEXCAN_CTRL2_RJW_SHIFT                 6
#define FLEXCAN_CTRL2_PSEG1_MASK                (0x07 << 3)
#define FLEXCAN_CTRL2_PSEG1_SHIFT               3
#define FLEXCAN_CTRL2_PSEG2_MASK                (0x07 << 0)
#define FLEXCAN_CTRL2_PSEG2_SHIFT               0

//---------------------------------------------------------------------------
// ESTAT register bits
//
#define FLEXCAN_ESTAT_BITERR_MASK               (0x03 << 14)
#define FLEXCAN_ESTAT_BITERR_SHIFT              14
#define FLEXCAN_ESTAT_BITERR_NONE               (0x00 << 14)
#define FLEXCAN_ESTAT_BITERR_DOMINANT_RECESSIVE (0x01 << 14)
#define FLEXCAN_ESTAT_BITERR_RECESSIVE_DOMINANT (0x02 << 14)
#define FLEXCAN_ESTAT_ACKERR                    (0x01 << 13)
#define FLEXCAN_ESTAT_CRCERR                    (0x01 << 12)
#define FLEXCAN_ESTAT_FORMERR                   (0x01 << 11)
#define FLEXCAN_ESTAT_STUFFERR                  (0x01 << 10)
#define FLEXCAN_ESTAT_TXWARN                    (0x01 << 9)
#define FLEXCAN_ESTAT_RXWARN                    (0x01 << 8)
#define FLEXCAN_ESTAT_IDLE                      (0x01 << 7)
#define FLEXCAN_ESTAT_TX_RX                     (0x01 << 6)
#define FLEXCAN_ESTAT_FCS_MASK                  (0x03 << 4)
#define FLEXCAN_ESTAT_FCS_SHIFT                 4
#define FLEXCAN_ESTAT_FCS_ERROR_ACTIVE          (0x00 << 4)
#define FLEXCAN_ESTAT_FCS_ERROR_PASSIVE         (0x01 << 4)
#define FLEXCAN_ESTAT_BOFFINT                   (0x01 << 2)
#define FLEXCAN_ESTAT_ERRINT                    (0x01 << 1)
#define FLEXCAN_ESTAT_WAKEINT                   (0x01 << 0)

//
// For receive event calls we use these two identifiers for
// err and bus off events - message boxes use 0 - 15
//
#define FLEXCAN_ERR_EVENT                       16
#define FLEXCAN_BUSOFF_EVENT                    17


//---------------------------------------------------------------------------
// message buffer cfg bits
//
#define MBOX_RXCODE_NOT_ACTIVE 0x00
#define MBOX_RXCODE_BUSY       0x10
#define MBOX_RXCODE_EMPTY      0x40
#define MBOX_RXCODE_FULL       0x20
#define MBOX_RXCODE_OVERRUN    0x60

#define MBOX_TXCODE_NOT_READY  0x80
#define MBOX_TXCODE_TRANSMIT   0xC0
#define MBOX_TXCODE_RESPONSE   0xA0

#define MBOX_DATA_FRAME        0x00 // data frame
#define MBOX_REMOTE_FRAME      0x01 // remote frame
#define MBOX_STD_ID            0x00 // standard identifier
#define MBOX_EXT_ID            0x01 // remote identifier
#define MBOX_TX                0x08 // tx message box
#define MBOX_RX                0x00 // rx messge box

#define MBOX_CFG_IDE           0x08
#define MBOX_CFG_RTR_EXT       0x01
#define MBOX_CFG_RTR_STD       0x10
#define MBOX_CFG_SSR           0x10
#define MBOX_CFG_DLC_MASK      0x0F
#define MBOX_CFG_STAT_MASK     0xF0


//===========================================================================
//                                 DATA TYPES
//===========================================================================
//
// configuration info for flexcan message buffer
//
typedef struct flexcan_mbox_info_st
{
    cyg_vector_t  isr_vec;
    int           isr_priority;
    cyg_interrupt interrupt;
    cyg_handle_t  interrupt_handle;   
    cyg_uint8     num;  
} flexcan_mbox_info; 

//
// flexcan interrupt (busoff, err, wake) data
//
typedef struct flexcan_int_st
{
    cyg_vector_t        isr_vec;  
    int                 isr_priority;
    cyg_interrupt       interrupt;
    cyg_handle_t        interrupt_handle;
} flexcan_int;

//
// flexcan message box initialisation
//
#define FLEXCAN_MBOX_INIT(_mbox0_vec, _prio, _mbox_no) { \
    isr_vec      : (_mbox0_vec) + (_mbox_no),            \
    isr_priority : (_prio),                              \
    num          : (_mbox_no)                            \
}

//
// Interrupt initialisation
//
#define FLEXCAN_INT_INIT(_vec, _prio) \
{                                     \
    isr_vec      : (_vec),            \
    isr_priority : (_prio)            \
}

//
// flexcan configuration 
//
typedef struct flexcan_info 
{
    cyg_uint8          *base;          // base address of flexcan modul
    cyg_vector_t        isr_vec_mbox0; // vector number of ISR vector of first message box
    flexcan_mbox_info   rx_mbox;       // rx message box interrupt
    flexcan_mbox_info   tx_mbox;       // tx message box interrupt
    cyg_uint32          last_tx_id;    // last transmitted message
    bool                tx_busy;       // indicates if transmit process ic currently running
    
    cyg_uint32          rxgmask;       // acceptance filter for message box 0 - 13
    cyg_uint32          rx14mask;      // acceptance filter for message box 14
    cyg_uint32          rx15mask;      // acceptance filter for message box 15
    
    flexcan_int         boff_int;      // bus off interrupt data
    flexcan_int         err_int;       // error interrupt data
 
    cyg_uint32          timeout_rd;
    cyg_uint32          timeout_wr;
    cyg_uint16          mbox_alloc_flags;// these bits are used to indicate which message buffers are already alloceted and which ones are free
#ifdef FLEXCAN_CAN_STATS
    cyg_uint32          isr_count;
    cyg_uint32          dsr_count;
    cyg_uint32          rx_bytes;
    cyg_uint32          tx_bytes;
    cyg_uint32          rx_errors;
#endif  
} flexcan_info;


//
// flexcan info initialisation
//
#define FLEXCAN_INFO(_l,                                                \
                     _baseaddr,                                         \
                     _isr_vec_mbox0,                                    \
                     _rx_mbox_no, _rx_isr_prio,                         \
                     _tx_mbox_no, _tx_isr_prio,                         \
                     _boff_isr_vec, _boff_isr_prio,                     \
                     _err_isr_vec, _err_isr_prio)                       \
flexcan_info _l = {                                                     \
    (void *)( _baseaddr),                                               \
    (_isr_vec_mbox0),                                                   \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_rx_isr_prio), (_rx_mbox_no)), \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_tx_isr_prio), (_tx_mbox_no)), \
    0xFFFFFFFF,                                                         \
    false,                                                              \
    rxgmask       : CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_RXMASK_GLOBAL,     \
    rx14mask      : CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_RXMASK_14,         \
    rx15mask      : CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_RXMASK_15,         \
    boff_int      : FLEXCAN_INT_INIT(_boff_isr_vec, _boff_isr_prio),    \
    err_int       : FLEXCAN_INT_INIT(_err_isr_vec, _err_isr_prio)       \
};


//===========================================================================
//                          GLOBAL DATA
//===========================================================================
//
// Note: two levels of macro are required to get proper expansion.
//
#define _FLEXCAN_MBOX_INTPRIO(n) CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX##n
#define FLEXCAN_MBOX_INTPRIO(n) _FLEXCAN_MBOX_INTPRIO(n)

//
// FlexCAN channel initialisation
//
FLEXCAN_INFO(flexcan_can0_info, 
             HAL_MCF52xx_MBAR + HAL_MCF52xx_FLEXCAN0_BASE,
             HAL_MCF52xx_FLEXCAN0_MBOX0_ISRVEC,
             CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_DEFAULT_RX_MBOX, 
             FLEXCAN_MBOX_INTPRIO(CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_DEFAULT_RX_MBOX),
             CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_DEFAULT_TX_MBOX, 
             FLEXCAN_MBOX_INTPRIO(CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_DEFAULT_TX_MBOX),
             HAL_MCF52xx_FLEXCAN0_BOFF_ISRVEC,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_BOFFINT,  
             HAL_MCF52xx_FLEXCAN0_ERR_ISRVEC,          
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_ERRINT);

//
// message box structure
//
typedef struct flexcan_mbox
{
    cyg_uint8  timestamp;
    cyg_uint8  ctrlstat;
    cyg_uint16 id_hi;
    cyg_uint16 id_lo;
    cyg_uint8  data[8];
    cyg_uint16 reserved;
} flexcan_mbox;

//
// flexcan register layout
//
typedef struct flexcan_regs
{
    cyg_uint16 CANMCR;       // 0x00
    cyg_uint16 reserved0[2]; // 0x02
    cyg_uint8  CANCTRL0;     // 0x06
    cyg_uint8  CANCTRL1;     // 0x07
    cyg_uint8  PRESDIV;      // 0x08
    cyg_uint8  CANCTRL2;     // 0x09
    cyg_uint16 TIMER;        // 0x0A
    cyg_uint16 reserved1[2]; // 0x0C
    cyg_uint16 RXGMASK_HI;   // 0x10
    cyg_uint16 RXGMASK_LO;   // 0x12
    cyg_uint16 RX14MASK_HI;  // 0x14
    cyg_uint16 RX14MASK_LO;  // 0x16
    cyg_uint16 RX15MASK_HI;  // 0x18
    cyg_uint16 RX15MASK_LO;  // 0x1A
    cyg_uint16 reserved2[2]; // 0x1C
    cyg_uint16 ESTAT;        // 0x20
    cyg_uint16 IMASK;        // 0x22
    cyg_uint16 IFLAG;        // 0x24
    cyg_uint8  RXERRCNT;     // 0x26
    cyg_uint8  TXERRCNT;     // 0x27
    cyg_uint16 reserved3[44];// 0x28
    flexcan_mbox mbox[16]; // 0x80
} flexcan_regs;


//===========================================================================
//                                 LOCAL DATA
//===========================================================================
static cyg_uint16 flexcan_baud_rates[] = {
    0,      // Unused
    10,     // 10 kbit/s
    20,     // 20
    50,     // 50
    100,    // 100
    125,    // 125
    250,    // 250
    500,    // 500
    800,    // 800
    1000,   // 1000 kbit/s
};


//===========================================================================
//                              PROTOTYPES
//===========================================================================
static bool        flexcan_init(struct cyg_devtab_entry* devtab_entry);
static Cyg_ErrNo   flexcan_lookup(struct cyg_devtab_entry** tab, struct cyg_devtab_entry* sub_tab, const char* name);
static Cyg_ErrNo   flexcan_set_config(can_channel *chan, cyg_uint32 key, const void* buf, cyg_uint32* len);
static bool        flexcan_putmsg(can_channel *priv, cyg_can_message *pmsg, void *pdata);
static bool        flexcan_getevent(can_channel *priv, cyg_can_event *pevent, void *pdata);
static void        flexcan_start_xmit(can_channel* chan);
static void        flexcan_stop_xmit(can_channel* chan);

//
// TX and RX ISRs and DSRs
//
static cyg_uint32  flexcan_mbox_rx_isr(cyg_vector_t, cyg_addrword_t);
static void        flexcan_mbox_rx_dsr(cyg_vector_t, cyg_ucount32, cyg_addrword_t);
static cyg_uint32  flexcan_mbox_tx_isr(cyg_vector_t, cyg_addrword_t);
static void        flexcan_mbox_tx_dsr(cyg_vector_t, cyg_ucount32, cyg_addrword_t);

//
// All other flexcan interrupt handlers
//
static cyg_uint32  flexcan_err_isr(cyg_vector_t, cyg_addrword_t);
static void        flexcan_err_dsr(cyg_vector_t, cyg_ucount32, cyg_addrword_t);
static cyg_uint32  flexcan_busoff_isr(cyg_vector_t, cyg_addrword_t);
static void        flexcan_busoff_dsr(cyg_vector_t, cyg_ucount32, cyg_addrword_t);


static bool flexcan_cfg_mbox_tx(flexcan_mbox *pmbox, cyg_can_message  *pmsg, bool rtr);
static void flexcan_cfg_mbox_rx(flexcan_mbox *pmbox, cyg_uint32 canid, cyg_uint8 ext);
static void flexcan_read_from_mbox(can_channel *chan, cyg_uint8 mbox, cyg_can_event *pevent, cyg_uint8 *ctrlstat);
static void flexcan_set_acceptance_mask(cyg_uint16 *rxmask_reg, cyg_uint32 mask, cyg_uint8 ext);
static void flexcan_start_chip(can_channel *chan);
static bool flexcan_set_baud(can_channel *chan, cyg_uint16 baudrate);
static bool flexcan_config(can_channel* chan, cyg_can_info_t* config, cyg_bool init);


CAN_LOWLEVEL_FUNS(flexcan_lowlevel_funs,
                  flexcan_putmsg,
                  flexcan_getevent,
                  flexcan_set_config,
                  flexcan_start_xmit,
                  flexcan_stop_xmit
     );


cyg_can_event    flexcan_can0_rxbuf[CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_QUEUESIZE_RX]; // buffer for 32 rx can events
cyg_can_message  flexcan_can0_txbuf[CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_QUEUESIZE_TX]; // buffer for 32 tx can messageds


CAN_CHANNEL_USING_INTERRUPTS(flexcan_can0_chan,
                             flexcan_lowlevel_funs,
                             flexcan_can0_info,
                             CYG_CAN_BAUD_RATE(CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_KBAUD),
                             flexcan_can0_txbuf, CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_QUEUESIZE_TX,
                             flexcan_can0_rxbuf, CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_QUEUESIZE_RX
    );


DEVTAB_ENTRY(flexcan_devtab, 
             CYGDAT_DEVS_CAN_MCF52xx_FLEXCAN0_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_can_devio, 
             flexcan_init, 
             flexcan_lookup,        // CAN driver may need initializing
             &flexcan_can0_chan
    );


//===========================================================================
//  Lookup the device and return its handle
//===========================================================================
static Cyg_ErrNo
flexcan_lookup(struct cyg_devtab_entry** tab, struct cyg_devtab_entry* sub_tab, const char* name)
{
    can_channel* chan    = (can_channel*) (*tab)->priv;
    (chan->callbacks->can_init)(chan);
    
    return ENOERR;
}


//===========================================================================
//  Set device configuration
//===========================================================================
static Cyg_ErrNo
flexcan_set_config(can_channel *chan, cyg_uint32 key, const void* buf, cyg_uint32* len)
{
    Cyg_ErrNo     res = ENOERR;
    
    switch(key) 
    {
        case CYG_IO_SET_CONFIG_CAN_INFO:
             {
                 cyg_can_info_t*  config = (cyg_can_info_t*) buf;
                 if (*len < sizeof(cyg_can_info_t)) 
                 {
                    return -EINVAL;
                 }
                 *len = sizeof(cyg_can_info_t);
                 if (!flexcan_config(chan, config, false)) 
                 {
                    return -EINVAL;
                 }
            }
            break;
            
        case CYG_IO_SET_CONFIG_CAN_RTR_BUF:
             { 
                 cyg_uint8          i;
                 cyg_uint16         alloc_mask = 0x0001;
                 flexcan_info      *info       = (flexcan_info *)chan->dev_priv;
                 flexcan_regs      *flexcan    = (flexcan_regs *)info->base;
                 cyg_can_rtr_buf_t *rtr_buf    = (cyg_can_rtr_buf_t*) buf;
                 
             
                 if (*len != sizeof(cyg_can_rtr_buf_t)) 
                 {
                    return -EINVAL;
                 }
                 *len = sizeof(cyg_can_rtr_buf_t);
                 
                 //
                 // If we need to create a new remote buffer then we check if there
                 // is one message box free and then setup this box for remote frame
                 // transmission
                 //
                 if (CYGNUM_CAN_RTR_BUF_INIT == rtr_buf->handle)
                 {
                    rtr_buf->handle = CYGNUM_CAN_RTR_BUF_NA;
                    for (i = 0; i < 14; ++i)
                    {
                        if (!(info->mbox_alloc_flags & alloc_mask))
                        {
                            rtr_buf->handle = i;
                            info->mbox_alloc_flags |= alloc_mask;
                            break;
                        }
                        alloc_mask <<= 1;
                     }
                 } // if (CYGNUM_CAN_RTR_BUF_INIT == rtr_buf->handle)
                 
                 //
                 // If we have a valid rtr buf handle then we can store data into 
                 // rtr message box
                 //
                 if ((rtr_buf->handle >= 0) && (rtr_buf->handle < 14))
                 {
                     flexcan_cfg_mbox_tx(&flexcan->mbox[rtr_buf->handle], &rtr_buf->msg, true);
                 }
                 else
                 {
                    return -EINVAL;
                 }
                 
             }
             break;
         
    } // switch (key)
    
    return res;
}


//===========================================================================
//  Read one event from can hardware
//===========================================================================
static bool flexcan_getevent(can_channel *chan, cyg_can_event *pevent, void *pdata)
{
    flexcan_info *info           = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan        = (flexcan_regs *)info->base;
    cyg_uint8     mbox_ctrlstat;
    bool          res            = true;
    cyg_uint8     event_id       = *((cyg_uint8 *)pdata);
    cyg_uint16    estat;
        
    //
    // if event_id is 0 - 15 the we have a message box event - if is
    //
    if (event_id < FLEXCAN_ERR_EVENT)
    {   
        //
        // read data from message box - during processing of this function
        // the message box is locked and cannot receive further messages
        //
        flexcan_read_from_mbox(chan, event_id, pevent, &mbox_ctrlstat); 

#ifdef CYGOPT_IO_CAN_TX_EVENT_SUPPORT    
        if (pevent->msg.id == info->last_tx_id)
        {
            pevent->flags = CYGNUM_CAN_EVENT_TX;
        }
        else
        {
            pevent->flags = CYGNUM_CAN_EVENT_RX;
        } 
#else // !CYGOPT_IO_CAN_TX_EVENT_SUPPORT
        //
        // If tx events are not supported and we received a self transmitted frame
        // then this is not really an rx event and we return false. We rely on the
        // fact here that two devices in network do not send the same identifier
        //
        if (pevent->msg.id == info->last_tx_id)    {   
            info->last_tx_id = 0xFFFFFFFF; // set last received ID to an invalid value
            res = false; 
        }
    
        pevent->flags = CYGNUM_CAN_EVENT_RX;
#endif
    
        //
        // check if an overun occured in this message box
        //
        if ((mbox_ctrlstat & MBOX_RXCODE_OVERRUN) == MBOX_RXCODE_OVERRUN)
        {
            pevent->flags |= CYGNUM_CAN_EVENT_OVERRUN_RX;
        }
    }
    else // (event_id >= FLEXCAN_ERR_EVENT)
    {
        //
        // We have an status event - check if it is an bus off interrupt or an
        // error interrupt and provide error information to upper layer
        //
        HAL_READ_UINT16(&flexcan->ESTAT, estat);
        pevent->msg.data[0] = estat & 0xFF;
        pevent->msg.data[1] = (estat >> 8) & 0xFF; 
        HAL_READ_UINT8(&flexcan->RXERRCNT, pevent->msg.data[2]);
        HAL_READ_UINT8(&flexcan->TXERRCNT, pevent->msg.data[3]);
        switch (event_id)
        {
            case FLEXCAN_ERR_EVENT :
                 //
                 // indicate error passive event and provide content of estat register
                 // for a more precise error information
                 //
                 if (estat & FLEXCAN_ESTAT_FCS_ERROR_PASSIVE)
                 {
                    pevent->flags = CYGNUM_CAN_EVENT_ERR_PASSIVE;
                 }
                 //
                 // If we are not in error passive state then we check if the
                 // error counters reached the warning level
                 //
                 else
                 {               
                     //
                     // indicate tx error counter warning level reached
                     //
                     if (estat & FLEXCAN_ESTAT_TXWARN)
                     {
                         pevent->flags |= CYGNUM_CAN_EVENT_WARNING_TX;
                     }
                 
                     //
                     // indicate rx error counter warning level reached
                     //
                     if (estat & FLEXCAN_ESTAT_RXWARN)
                     {
                         pevent->flags |= CYGNUM_CAN_EVENT_WARNING_RX;
                     }
                 }
                 break;
                 
            case FLEXCAN_BUSOFF_EVENT:
                 pevent->flags = CYGNUM_CAN_EVENT_BUS_OFF;
                 break;
        } // switch (event_id)
    }

    return res;
}


//===========================================================================
// Send one CAN message to CAN hardware
//===========================================================================
static bool flexcan_putmsg(can_channel *chan, cyg_can_message *pmsg, void *pdata)
{
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    cyg_uint8        mbox = *((cyg_uint8 *)pdata);
    cyg_uint16       iflag;
   
    HAL_READ_UINT16(&flexcan->IFLAG, iflag);
    
    //
    // check if device is busy sending a message
    //
    if (info->tx_busy)
    {
        //
        // if devise is busy and the interrupt flag is set, then we know
        // that device is not busy any longer - if more message boxes are
        // used for transmitting then tx_busy should be part of a message box
        // structure to keep track of the state of different message boxes
        //
        if (iflag & (0x0001 << mbox))
        {
            HAL_WRITE_UINT16(&flexcan->IFLAG, (0x0001 << mbox));
        }
        else
        {
            return false;
        }
    }
    
    info->tx_busy    = true;     // mark transmitter as busy
    info->last_tx_id = pmsg->id; // store message in order to identify self recieved frames
    flexcan_cfg_mbox_tx(&flexcan->mbox[mbox], pmsg, false);
    
    return true;
}


//===========================================================================
// Flexcan start xmit
//===========================================================================
static void flexcan_start_xmit(can_channel* chan)
{
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    cyg_uint16    imask;
    CYG_INTERRUPT_STATE     saved_state;

    HAL_DISABLE_INTERRUPTS(saved_state);
    
    //
    // Now enable message box 15 interrupts
    //
    HAL_READ_UINT16(&flexcan->IMASK, imask);
    HAL_WRITE_UINT16(&flexcan->IMASK, imask | (0x0001 << info->tx_mbox.num));
    
    //
    // kick transmitter
    //
    chan->callbacks->xmt_msg(chan, &info->tx_mbox.num);  // Kick transmitter (if necessary)
    
    HAL_RESTORE_INTERRUPTS(saved_state);
}


//===========================================================================
// Flexcan start xmit
//===========================================================================
static void flexcan_stop_xmit(can_channel* chan)
{
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    cyg_uint16    imask;
    CYG_INTERRUPT_STATE     saved_state;

    HAL_DISABLE_INTERRUPTS(saved_state);
    
    //
    // Now disable message box 15 interrupts
    //
    HAL_READ_UINT16(&flexcan->IMASK, imask);
    HAL_WRITE_UINT16(&flexcan->IMASK, imask & ~(0x0001 << info->tx_mbox.num));

    HAL_RESTORE_INTERRUPTS(saved_state);
}


//===========================================================================
// Configure flexcan channel
//===========================================================================
static bool flexcan_config(can_channel* chan, cyg_can_info_t* config, cyg_bool init)
{
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    cyg_uint16    tmp16;
    cyg_uint8     tmp8;
    cyg_uint8     i;
    
    if (init)
    {
#if defined(CYGPKG_DEVS_CAN_MCF52xx_FLEXCAN0) && defined(HAL_MCF52xx_FLEXCAN0_PROC_INIT)
        if (info == &flexcan_can0_info) {
            HAL_MCF52xx_FLEXCAN0_PROC_INIT();
        }
#endif
        
        //
        // Issue a reset in order to go into halt mode. The reset will set the
        // the halt bit in mcr
        //
        HAL_READ_UINT16(&flexcan->CANMCR, tmp16);
        tmp16 &= ~FLEXCAN_MCR_FRZ;
        HAL_WRITE_UINT16(&flexcan->CANMCR, tmp16);
        tmp16 |= FLEXCAN_MCR_SOFTRST;
        HAL_WRITE_UINT16(&flexcan->CANMCR, tmp16);
        HAL_DELAY_US(10);
    
        //
        // Check reset status
        //
        HAL_READ_UINT16(&flexcan->CANMCR, tmp16);
        if (tmp16 & FLEXCAN_MCR_SOFTRST)
        {
            return false;
        }
    
        //
        // Initialize the transmit and receive pin modes
        //
        HAL_WRITE_UINT8(&flexcan->CANCTRL0 , (FLEXCAN_CTRL0_TXMODE_FULL_0_DOMINANT  
                                            | FLEXCAN_CTRL0_RXMODE_0_DOMINANT)
                                            & ~FLEXCAN_CTRL0_BOFFMSK
                                            & ~FLEXCAN_CTRL0_ERRMASK);
                                            
        //
        // setup message box acceptance filter
        //
        flexcan_set_acceptance_mask(&flexcan->RXGMASK_HI,  info->rxgmask, 0);
        flexcan_set_acceptance_mask(&flexcan->RX14MASK_HI, info->rx14mask, 0);
        flexcan_set_acceptance_mask(&flexcan->RX15MASK_HI, info->rx15mask, 0);                                          
    } // if (init)
    
    //
    // stop chip
    //
    HAL_READ_UINT16(&flexcan->CANMCR, tmp16);
    HAL_WRITE_UINT16(&flexcan->CANMCR, tmp16 | FLEXCAN_MCR_HALT);  
    
    //
    // deactivate all message buffers - this is mandatory for configuration
    // of message buffers
    //
    for (i = 0; i < 16; ++i)
    {
        HAL_WRITE_UINT16(&flexcan->mbox[i], MBOX_RXCODE_NOT_ACTIVE);
    }
    //
    // mask all interrupts
    //
    HAL_WRITE_UINT16(&flexcan->IMASK, 0x0000);
    HAL_READ_UINT8(&flexcan->CANCTRL0, tmp8);
    HAL_WRITE_UINT8(&flexcan->CANCTRL0, tmp8 & ~(FLEXCAN_CTRL0_BOFFMSK | FLEXCAN_CTRL0_ERRMASK));
                      
    flexcan_set_baud(chan, config->baud);
    
    //
    // setup bus arbitration mode - the LBUF bit defines the 
    // transmit-first scheme 0 = message buffer with lowest ID
    // 1 = message buffer with lowest number. We use lowest ID here
    //
    HAL_READ_UINT8(&flexcan->CANCTRL1, tmp8);
    HAL_WRITE_UINT8(&flexcan->CANCTRL1, (tmp8 & ~FLEXCAN_CTRL1_LBUF));
        
    info->mbox_alloc_flags = 0; // no buffers used yet
    
    //
    // Message box 14 is our receiv message box. We configure it for
    // reception of any message
    //
    flexcan_cfg_mbox_rx(&flexcan->mbox[info->rx_mbox.num], 0x100, 0);
    flexcan_set_acceptance_mask(&flexcan->RX14MASK_HI, 0, 0);
    info->mbox_alloc_flags |= (0x0001 << info->rx_mbox.num); // mark rx mbox flag as used
    info->mbox_alloc_flags |= (0x0001 << info->tx_mbox.num); // mark tx mbox flag as used
    
    //
    // enable the rx interrupt for mbox 0 (tx interrupt are enabled in start xmit)
    // bus off interrupt and error interrupt
    //
    HAL_WRITE_UINT16(&flexcan->IMASK, (0x0001 << info->rx_mbox.num));
    HAL_READ_UINT8(&flexcan->CANCTRL0, tmp8);
    HAL_WRITE_UINT8(&flexcan->CANCTRL0, tmp8 | (FLEXCAN_CTRL0_BOFFMSK | FLEXCAN_CTRL0_ERRMASK));
    
    //
    // now we can start the chip
    //
    flexcan_start_chip(chan);
    
    //
    // store new config values
    //
    if (config != &chan->config) 
    {
        chan->config = *config;
    }
    
    return true;
}

//===========================================================================
//                           CAN INIT
//
/// First initialisation and reset of CAN modul.
//===========================================================================
static bool flexcan_init(struct cyg_devtab_entry* devtab_entry)
{
    can_channel  *chan    = (can_channel*)devtab_entry->priv;
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;

    if (!flexcan_config(chan, &chan->config, true))
    {
        return false;
    }
    
    //
    // prepare message box interrupt for message box 0 - the rx message box
    //
    cyg_drv_interrupt_create(info->rx_mbox.isr_vec,
                             info->rx_mbox.isr_priority,
                             (cyg_addrword_t) chan,
                             &flexcan_mbox_rx_isr,
                             &flexcan_mbox_rx_dsr,
                             &(info->rx_mbox.interrupt_handle),
                             &(info->rx_mbox.interrupt));
    cyg_drv_interrupt_attach(info->rx_mbox.interrupt_handle);
    cyg_drv_interrupt_unmask(info->rx_mbox.isr_vec);
    
    //
    // prepare message box interrupt for message box 15 - the tx message box
    //
    cyg_drv_interrupt_create(info->tx_mbox.isr_vec,
                             info->tx_mbox.isr_priority,
                             (cyg_addrword_t) chan,
                             &flexcan_mbox_tx_isr,
                             &flexcan_mbox_tx_dsr,
                             &(info->tx_mbox.interrupt_handle),
                             &(info->tx_mbox.interrupt));
    cyg_drv_interrupt_attach(info->tx_mbox.interrupt_handle);
    cyg_drv_interrupt_unmask(info->tx_mbox.isr_vec);
    
    //
    // prepare error interrupt
    //
    cyg_drv_interrupt_create(info->err_int.isr_vec,
                             info->err_int.isr_priority,
                             (cyg_addrword_t) chan,
                             &flexcan_err_isr,
                             &flexcan_err_dsr,
                             &(info->err_int.interrupt_handle),
                             &(info->err_int.interrupt));
    cyg_drv_interrupt_attach(info->err_int.interrupt_handle);
    cyg_drv_interrupt_unmask(info->err_int.isr_vec);
    
    //
    // prepare busoff interrupt
    //
    cyg_drv_interrupt_create(info->boff_int.isr_vec,
                             info->boff_int.isr_priority,
                             (cyg_addrword_t) chan,
                             &flexcan_busoff_isr,
                             &flexcan_busoff_dsr,
                             &(info->boff_int.interrupt_handle),
                             &(info->boff_int.interrupt));
    cyg_drv_interrupt_attach(info->boff_int.interrupt_handle);
    cyg_drv_interrupt_unmask(info->boff_int.isr_vec);
        
    return true;
}


//===========================================================================
// Flexcan error interrupt handler
//===========================================================================
static cyg_uint32  flexcan_err_isr(cyg_vector_t vec, cyg_addrword_t data)
{
    can_channel  *chan    = (can_channel *)data;
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    cyg_uint8     ctrl0;
    cyg_uint16    estat;
    
    //
    // first we disable error interrupts - DSR will reenable it later
    //
    HAL_READ_UINT8(&flexcan->CANCTRL0, ctrl0);
    HAL_WRITE_UINT8(&flexcan->CANCTRL0, ctrl0 & ~FLEXCAN_CTRL0_ERRMASK);
    
    //
    // for clearing the interrupt we first read the flag register as 1
    // and then write it as 1 (and not as zero like the manual stated)
    // we clear only the flag of this interrupt and leave all other
    // message box interrupts untouched
    //
    HAL_READ_UINT16(&flexcan->ESTAT, estat);
    HAL_WRITE_UINT16(&flexcan->ESTAT, FLEXCAN_ESTAT_ERRINT); 
    
    //
    // On the mcf5272 there is no need to acknowledge internal
    // interrupts, only external ones.
    // cyg_drv_interrupt_acknowledge(vec);
    //
    return CYG_ISR_CALL_DSR;
}


//===========================================================================
// DSR for all interrupts that are no message box interrupts
//===========================================================================
static void flexcan_err_dsr(cyg_vector_t vec, cyg_ucount32 count, cyg_addrword_t data)
{
    can_channel  *chan     = (can_channel *)data;
    flexcan_info *info     = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan  = (flexcan_regs *)info->base;
    cyg_uint8     ctrl0;
    cyg_uint8     event_id = FLEXCAN_ERR_EVENT;

    //
    // signal CAN event to generic IO CAN driver - it will do any further 
    // processing
    //            
    chan->callbacks->rcv_event(chan, &event_id);
    
    //
    // reenable bus off interrupts
    //
    HAL_READ_UINT8(&flexcan->CANCTRL0, ctrl0);
    HAL_WRITE_UINT8(&flexcan->CANCTRL0, ctrl0 | FLEXCAN_CTRL0_ERRMASK);
}


//===========================================================================
// Bus off interrupt handler
//===========================================================================
static cyg_uint32  flexcan_busoff_isr(cyg_vector_t vec, cyg_addrword_t data)
{
    can_channel  *chan    = (can_channel *)data;
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    cyg_uint8     ctrl0;
    cyg_uint16    estat;
    
    //
    // first we disable bus off interrupts - DSR will reenable it later
    //
    HAL_READ_UINT8(&flexcan->CANCTRL0, ctrl0);
    HAL_WRITE_UINT8(&flexcan->CANCTRL0, ctrl0 & ~FLEXCAN_CTRL0_BOFFMSK);
    
    //
    // for clearing the interrupt we first read the flag register as 1
    // and then write it as 1 (and not as zero like the manual stated)
    // we clear only the flag of this interrupt and leave all other
    // message box interrupts untouched
    //
    HAL_READ_UINT16(&flexcan->ESTAT, estat);
    HAL_WRITE_UINT16(&flexcan->ESTAT, FLEXCAN_ESTAT_BOFFINT);  
    
    //
    // On the mcf5272 there is no need to acknowledge internal
    // interrupts, only external ones.
    // cyg_drv_interrupt_acknowledge(vec);
    //
    return CYG_ISR_CALL_DSR;
}


//===========================================================================
// DSR for all interrupts that are no message box interrupts
//===========================================================================
static void flexcan_busoff_dsr(cyg_vector_t vec, cyg_ucount32 count, cyg_addrword_t data)
{
    can_channel  *chan     = (can_channel *)data;
    flexcan_info *info     = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan  = (flexcan_regs *)info->base;
    cyg_uint8     ctrl0;
    cyg_uint8     event_id = FLEXCAN_BUSOFF_EVENT;

    //
    // signal CAN event to generic IO CAN driver - it will do any further 
    // processing
    //            
    chan->callbacks->rcv_event(chan, &event_id);
    
    //
    // reenable bus off interrupts
    //
    HAL_READ_UINT8(&flexcan->CANCTRL0, ctrl0);
    HAL_WRITE_UINT8(&flexcan->CANCTRL0, ctrl0 | FLEXCAN_CTRL0_BOFFMSK);  
}


//===========================================================================
// Flexcan message box isr
//===========================================================================
static cyg_uint32 flexcan_mbox_rx_isr(cyg_vector_t vec, cyg_addrword_t data)
{
    can_channel  *chan    = (can_channel *)data;
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    cyg_uint16    iflag;
    cyg_uint16    imask;
    
    //
    // number of message box can be calculated from vector that cause
    // interrupt - we pass this message box number as additional data to the
    // callback because it is required in the receive event function later
    //
    cyg_uint8 mbox = vec - info->isr_vec_mbox0;    
    
    //
    // first we disable interrupts of this message box - the DSR will
    // reenable it later
    //
    HAL_READ_UINT16(&flexcan->IMASK, imask);
    HAL_WRITE_UINT16(&flexcan->IMASK, imask & ~(0x0001 << mbox));
    
    //
    // for clearing the interrupt we first read the flag register as 1
    // and then write it as 1 (and not as zero like the manual stated)
    // we clear only the flag of this interrupt and leave all other
    // message box interrupts untouched
    //
    HAL_READ_UINT16(&flexcan->IFLAG, iflag);
    HAL_WRITE_UINT16(&flexcan->IFLAG, (0x0001 << mbox));    
    
    //
    // On the mcf5272 there is no need to acknowledge internal
    // interrupts, only external ones.
    // cyg_drv_interrupt_acknowledge(vec);
    //
    return CYG_ISR_CALL_DSR;
}


//===========================================================================
// Flexcan message box dsr
//===========================================================================
static void flexcan_mbox_rx_dsr(cyg_vector_t vec, cyg_ucount32 count, cyg_addrword_t data)
{
    can_channel  *chan    = (can_channel *)data;
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    cyg_uint16    imask;
    
    //
    // number of message box can be calculated from vector that caused
    // interrupt - we pass this message box number as additional data to the
    // callback
    //
    cyg_uint8 mbox = vec - info->isr_vec_mbox0;    
    
    //
    // signal CAN event to generic IO CAN driver - it will do any further 
    // processing
    //            
    chan->callbacks->rcv_event(chan, &mbox);
    
    //
    // reenable interrupts for the message box that caused the DSR to run
    //
    HAL_READ_UINT16(&flexcan->IMASK, imask);
    HAL_WRITE_UINT16(&flexcan->IMASK, imask | (0x0001 << mbox)); 
}

//===========================================================================
// Flexcan message box isr
//===========================================================================
static cyg_uint32 flexcan_mbox_tx_isr(cyg_vector_t vec, cyg_addrword_t data)
{
    can_channel  *chan    = (can_channel *)data;
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    cyg_uint16    iflag;
    cyg_uint16    imask;
    
    // number of message box can be calculated from vector that cause
    // interrupt - we pass this message box number as additional data to the
    // callback
    //
    cyg_uint8 mbox = vec - info->isr_vec_mbox0;
    
    //
    // the first thing we do here is to disable this message box. we do this
    // because if we just sent a remote transmission request with this message
    // box then the FLEXCAN modul will automatically set up this message box
    // for reception of the requested data frame. But we won't receive messages
    // with the TX message box and deactivate it here immediatelly.
    // 
    HAL_WRITE_UINT8(&flexcan->mbox[mbox].ctrlstat, MBOX_TXCODE_NOT_READY);   
    
    //
    // first we disable interrupts of this message box - the DSR will
    // reenable it later
    //
    HAL_READ_UINT16(&flexcan->IMASK, imask);
    HAL_WRITE_UINT16(&flexcan->IMASK, imask & ~(0x0001 << mbox));
    
    //
    // for clearing the interrupt we first read the flag register as 1
    // and then write it as 1 (and not as zero like the manual stated)
    // we clear only the flag of this interrupt and leave all other
    // message box interrupts untouched
    //
    HAL_READ_UINT16(&flexcan->IFLAG, iflag);
    HAL_WRITE_UINT16(&flexcan->IFLAG, (0x0001 << mbox)); 
    info->tx_busy = false;   
    
    //
    // On the mcf5272 there is no need to acknowledge internal
    // interrupts, only external ones.
    // cyg_drv_interrupt_acknowledge(vec);
    //
    return CYG_ISR_CALL_DSR;
}


//===========================================================================
// Flexcan message box dsr
//===========================================================================
static void flexcan_mbox_tx_dsr(cyg_vector_t vec, cyg_ucount32 count, cyg_addrword_t data)
{
    can_channel  *chan    = (can_channel *)data;
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    cyg_uint16    imask;
    
    //
    // number of message box can be calculated from vector that caused
    // interrupt - we pass this message box number as additional data to the
    // callback
    //
    cyg_uint8 mbox = vec - info->isr_vec_mbox0;    
    
    //
    // send next message 
    //            
    chan->callbacks->xmt_msg(chan, &mbox);
    
    //
    // reenable interrupts for the message box that caused the DSR to run
    //
    HAL_READ_UINT16(&flexcan->IMASK, imask);
    HAL_WRITE_UINT16(&flexcan->IMASK, imask | (0x0001 << mbox));
}


//===========================================================================
//                     START FLEXCAN MODUL
//===========================================================================
static void flexcan_start_chip(can_channel *chan)
{
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    
    cyg_uint16 tmp16;
    HAL_READ_UINT16(&flexcan->CANMCR, tmp16);    
    HAL_WRITE_UINT16(&flexcan->CANMCR, tmp16 
                 & ~(FLEXCAN_MCR_FRZ | FLEXCAN_MCR_HALT));
}


//===========================================================================
//                      SET ACCEPTANCE MASK
//===========================================================================
static void flexcan_set_acceptance_mask(cyg_uint16 *rxmask_reg, cyg_uint32 mask, cyg_uint8 ext)
{
    cyg_uint16 id;
    //
    // 32 bit access to RXMASK filters is broken so we use  16 Bit
    // access here
    //
    if (ext != 0)
    {
        id  = ((mask >> 13) & 0xFFE0);       // set mask bits 18 - 28
        id |= ((mask >> 15) & 0x7);          // set mask bits 15 -17
        HAL_WRITE_UINT16(&rxmask_reg[0], id);
        
        id = (mask << 1) & 0xFFFE;
        HAL_WRITE_UINT16(&rxmask_reg[1], id);
    }
    else
    {
        id = ((mask << 5) & 0xFFE0); 
        HAL_WRITE_UINT16(&rxmask_reg[0], id);
        HAL_WRITE_UINT16(&rxmask_reg[1], 0xFFFF);  
    }
    
}



//===========================================================================
//                 CONFIGURE MESSAGE BOX FOR TRANSMISSION
//===========================================================================
static bool flexcan_cfg_mbox_tx(flexcan_mbox     *pmbox,
                                cyg_can_message  *pmsg,
                                bool              rtr)
{
    cyg_uint16 id;
  
    HAL_WRITE_UINT8(&pmbox->ctrlstat, MBOX_TXCODE_NOT_READY); 
    
    if (pmsg->ext != 0)
    { 
        id  = ((pmsg->id >> 13) & 0xFFE0);   // setup id bits 18 - 28
        id |= (MBOX_CFG_IDE | MBOX_CFG_SSR); // set SSR and IDE bit to 1
        id |= ((pmsg->id >> 15) & 0x7);      // set id bits 15 - 17
        HAL_WRITE_UINT16(&pmbox->id_hi, id);
        
        id = ((pmsg->id << 1) & 0xFFFE);
        
        if (pmsg->rtr)
        {
            id |= MBOX_CFG_RTR_EXT;
        }   
        
        HAL_WRITE_UINT16(&pmbox->id_lo, id);
    }   
    else
    {
        id = ((pmsg->id << 5) & 0xFFE0);
        if (pmsg->rtr)
        {
            id |= MBOX_CFG_RTR_STD;           // set rtr bit for standard ID
        }
        HAL_WRITE_UINT16(&pmbox->id_hi, id);
        HAL_WRITE_UINT16(&pmbox->id_lo, 0);
    }
    
    pmsg->dlc %= 9; // limit data length to 8 bytes
    
    //
    // Now copy data bytes into buffer and start transmission
    //
    HAL_WRITE_UINT8_VECTOR(&pmbox->data, pmsg->data, pmsg->dlc, 1); 
    
    if (rtr)
    {
        HAL_WRITE_UINT8(&pmbox->ctrlstat, MBOX_TXCODE_RESPONSE | pmsg->dlc);
    }
    else
    {
        HAL_WRITE_UINT8(&pmbox->ctrlstat, MBOX_TXCODE_TRANSMIT | pmsg->dlc);
    }
    
    return true;
}



//===========================================================================
//              CONFIGURE MESSAGE BOX FOR RECEPTION OF FRAMES
//===========================================================================
static void flexcan_cfg_mbox_rx(flexcan_mbox *pmbox,
                                cyg_uint32    canid,
                                cyg_uint8     ext)
{
    cyg_uint16 id;
    
    HAL_WRITE_UINT8(&pmbox->ctrlstat, MBOX_RXCODE_NOT_ACTIVE); 
    
    if (ext != 0)
    { 
        id  = ((canid >> 13) & 0xFFE0);      // setup id bits 18 - 28
        id |= (MBOX_CFG_IDE | MBOX_CFG_SSR); // set SSR and IDE bit to 1
        id |= ((canid >> 15) & 0x7);         // set id bits 15 - 17
        HAL_WRITE_UINT16(&pmbox->id_hi, id);  // write ID high
        
        id = ((canid << 1) & 0xFFFE);
        
        HAL_WRITE_UINT16(&pmbox->id_lo, id);// write ID low
    }   
    else
    {
        id = ((canid << 5) & 0xFFE0);

        HAL_WRITE_UINT16(&pmbox->id_hi, id);
        HAL_WRITE_UINT16(&pmbox->id_lo, 0);
    }
    
    HAL_WRITE_UINT8(&pmbox->ctrlstat, MBOX_RXCODE_EMPTY);
}


//===========================================================================
//                       READ DATA FROM MESSAGE BOX
//==========================================================================
static void flexcan_read_from_mbox(can_channel  *chan,
                            cyg_uint8            mbox,
                            cyg_can_event       *pevent,
                            cyg_uint8           *ctrlstat)
{
    flexcan_info    *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs    *flexcan = (flexcan_regs *)info->base;
    flexcan_mbox    *pmbox   = &flexcan->mbox[mbox];
    cyg_can_message *pmsg    = &pevent->msg;
    cyg_uint16       id;
    cyg_uint8        i;
    
    HAL_READ_UINT8(&pmbox->ctrlstat, *ctrlstat); // this read will lock the mbox
    
    //
    // If message buffer is busy then it is now beeing filled with a new message
    // This condition will be cleared within 20 cycles - wi simply do a 20 us
    // delay here, that should be enougth
    //
    if (*ctrlstat & MBOX_RXCODE_BUSY)
    {
        HAL_DELAY_US(20);
    }
    
    pmsg->dlc = (*ctrlstat & MBOX_CFG_DLC_MASK); // store received data len
    pmsg->rtr = 0;
    
    HAL_READ_UINT16(&pmbox->id_hi, id);         // read ID high
    
    if (id & MBOX_CFG_IDE)
    {
        pmsg->ext = 1;
        pmsg->id  = (id & 0xFFE0) << 13;
        
        HAL_READ_UINT16(&pmbox->id_lo, id);
        pmsg->id |= (id & 0xFFFE) >> 1;
    }
    else
    {
        pmsg->ext = 0;
        pmsg->id  = (id & 0xFFE0) >> 5;
    }
    
    //
    // now finally copy data
    //
    HAL_READ_UINT8_VECTOR(&pmbox->data, pmsg->data, pmsg->dlc, 1);
       
    //
    // now zero out the remaining bytes in can message in order
    // to deliver a defined state
    //
    for (i = pmsg->dlc; i < 8; ++i)
    {
        pmsg->data[i] = 0;
    }
  
    //
    // now mark this mbox as empty and read the free running timer
    // to unlock this mbox
    //
    HAL_WRITE_UINT8(&pmbox->ctrlstat, MBOX_RXCODE_EMPTY);
    HAL_READ_UINT16(&flexcan->TIMER, id);
#ifdef CYGOPT_IO_CAN_SUPPORT_TIMESTAMP    
    pevent->timestamp = id;
#endif
}


//===========================================================================
//                           INIT CAN BAUD RATE
//===========================================================================
static bool flexcan_set_baud(can_channel *chan, cyg_uint16 baud)
{
    flexcan_info             *info = (flexcan_info *)chan->dev_priv;
    flexcan_regs             *flexcan = (flexcan_regs *)info->base;
    cyg_uint16                mcr_bck;
    cyg_uint8                 tmp8;
    cyg_uint8                 presdiv;
    cyg_uint8                 propseg;
    cyg_uint8                 pseg1_2;
    cyg_uint16                baudrate = flexcan_baud_rates[baud];
    
    //
    // Get bit timings from HAL because bit timings depend on sysclock
    //
    HAL_MCF52xx_FLEXCAN_GET_BIT_TIMINGS(&baudrate, &presdiv, &propseg, &pseg1_2);
    
    //
    // return false if baudrate is not supported
    //
    if (0 == baudrate)
    {
        return false;
    }
    
    //
    // For setting the bit timings we have to stop the flexcan modul
    //
    HAL_READ_UINT16(&flexcan->CANMCR, mcr_bck);
    HAL_WRITE_UINT16(&flexcan->CANMCR, mcr_bck | FLEXCAN_MCR_HALT);
    
    //
    // now we setup bit timings
    //
    HAL_WRITE_UINT8(&flexcan->PRESDIV, presdiv);
    HAL_READ_UINT8(&flexcan->CANCTRL1, tmp8);
    HAL_WRITE_UINT8(&flexcan->CANCTRL1, (tmp8 & 0xF8) | propseg); 
    HAL_READ_UINT8(&flexcan->CANCTRL2, tmp8);
    HAL_WRITE_UINT8(&flexcan->CANCTRL2, (tmp8 & 0xC0) | pseg1_2); 
    
    //
    // Now restore the previous state - if the modul was started then
    // it will no be started again, if it was stopped, then it remains stopped
    //
    HAL_WRITE_UINT16(&flexcan->CANMCR, mcr_bck);
    
    return true;
}

//---------------------------------------------------------------------------
// end of can_mcf_flexcan.c

