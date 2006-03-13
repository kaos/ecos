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
#define FLEXCAN_WAKE_EVENT                      18

//
// Acceptance mask
//
#define FLEXCAN_ACCEPTANCE_MASK_RX_ALL          0x00       // receive all messages - mbox ID does not matter
#define FLEXCAN_ACCEPTANCE_MASK_RX_ID           0x1FFFFFFF // receive only messages where ID exactly matches mbox ID


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


//---------------------------------------------------------------------------
// flexcan message buffer configuration
//
#define FLEXCAN_MBOX_MIN      0
#define FLEXCAN_MBOX_MAX       15
#define FLEXCAN_MBOX_CNT      16
#define FLEXCAN_MBOX_TX       CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_DEFAULT_TX_MBOX
#define FLEXCAN_MBOX_RX_MIN   0
#define FLEXCAN_MBOX_RX_MAX   (FLEXCAN_MBOX_MAX - 1) // one msg box is tx
#define FLEXCAN_MBOX_RX_CNT   (FLEXCAN_MBOX_CNT - 1) // one msg box is tx


#define FLEXCAN_CTRLSTAT_NOT_READ 0 // indicates that control status register is not read


//===========================================================================
//                                 DATA TYPES
//===========================================================================
//
// Type of message buffer - required for function getevent in order to
// identify the type of message box that cause event
//
typedef enum
{
    MBOX_STATE_DISABLED,  // message box unused (free)
    MBOX_STATE_TX,        // TX message box
    MBOX_STATE_REMOTE_TX, // remote TX msaage box (data will be sent on reception of rtr frame) 
    MBOX_STATE_RX_ALL_STD, // RX message box for standard IDs
    MBOX_STATE_RX_ALL_EXT, // RX message box for standard IDs
    MBOX_STATE_RX_FILT     // RX message box for filter mboxes
} flexcan_mbox_state;


//
// configuration info for flexcan message buffer
//
typedef struct flexcan_mbox_info_st
{
    cyg_vector_t       isr_vec;           // isr vector
    int                isr_priority;      // isr priority
    cyg_interrupt      interrupt;         // stores interrupt data
    cyg_handle_t       interrupt_handle;  // stores interrupt number 
    cyg_uint8          num;               // number of message buffer
    bool               busy;              // if true, then transmission or reception is in progress
    flexcan_mbox_state state;             // message box state
    cyg_uint8          ctrlstat_shadow;   // shadow register of message box ctrlstat register
} flexcan_mbox_info; 

//
// Between ISR and DSR handling there is some kind of circular buffer.
// A DSR is only invoked if no other message box invoked a DSR before
// the DSR will read all available message buffers. This structure
// is for exchange of information between ISR and DSR
//
typedef struct st_rxmbox_circbuf
{
    cyg_uint8 idx_rd;       // the message box the DSR will read from
    cyg_uint8 idx_wr;       // the message box that will receive the next message
    cyg_uint8 count;        // the number of received message before DSR starts (number of ISR nesting)
} flexcan_rxmbox_circbuf;

//
// flexcan interrupt (busoff, err, wake) data - stores interrupt data for
// a non message box interrupt (bus off, err or wake interrupt)
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
    num          : (_mbox_no),                           \
    busy         : false                                 \
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
    cyg_uint8          *base;            // base address of flexcan modul
    cyg_vector_t        isr_vec_mbox0;   // vector number of ISR vector of first message box
    flexcan_mbox_info   mboxes[FLEXCAN_MBOX_CNT];// message boxes
    cyg_uint32          last_tx_id;      // last transmitted message identifier
    
    flexcan_int         boff_int;        // bus off interrupt data
    flexcan_int         err_int;         // error interrupt data
    flexcan_int         wake_int;        // wake interrupt data
    
    cyg_uint8           tx_all_mbox;     // number of message box for all transmit messages
    cyg_uint8           free_mboxes;     // number of free message boxes for msg filters and rtr buffers
    cyg_can_state       state;           // state of CAN controller
    
    flexcan_rxmbox_circbuf rxmbox_std_circbuf;
    flexcan_rxmbox_circbuf rxmbox_ext_circbuf;

    cyg_uint8           mboxes_std_cnt;   // contains number of standard message boxes available
    cyg_uint8           mboxes_ext_cnt;   // number of message boxes with ext id
    cyg_uint8           mboxes_rx_all_cnt;// number of all available mboxes

    bool                rx_all;           // true if reception of call can messages is active
    cyg_uint16          imask_shadow;    // interrupt mask shadow register
#ifdef CYGOPT_IO_CAN_TX_EVENT_SUPPORT 
    cyg_can_message     last_tx_msg;     // stores last transmitted message for TX events
#endif

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
                     _mbox0_isr_prio,                                   \
                     _mbox1_isr_prio,                                   \
                     _mbox2_isr_prio,                                   \
                     _mbox3_isr_prio,                                   \
                     _mbox4_isr_prio,                                   \
                     _mbox5_isr_prio,                                   \
                     _mbox6_isr_prio,                                   \
                     _mbox7_isr_prio,                                   \
                     _mbox8_isr_prio,                                   \
                     _mbox9_isr_prio,                                   \
                     _mbox10_isr_prio,                                  \
                     _mbox11_isr_prio,                                  \
                     _mbox12_isr_prio,                                  \
                     _mbox13_isr_prio,                                  \
                     _mbox14_isr_prio,                                  \
                     _mbox15_isr_prio,                                  \
                     _boff_isr_vec, _boff_isr_prio,                     \
                     _err_isr_vec, _err_isr_prio,                       \
                     _wake_isr_vec, _wake_isr_prio,                     \
                     _tx_all_mbox,                                      \
                     _std_mboxes, _ext_mboxes)                          \
flexcan_info _l = {                                                     \
    (void *)( _baseaddr),                                               \
    (_isr_vec_mbox0),                                                   \
    mboxes : {                                                          \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_mbox0_isr_prio), 0),          \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_mbox1_isr_prio), 1),          \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_mbox2_isr_prio), 2),          \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_mbox3_isr_prio), 3),          \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_mbox4_isr_prio), 4),          \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_mbox5_isr_prio), 5),          \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_mbox6_isr_prio), 6),          \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_mbox7_isr_prio), 7),          \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_mbox8_isr_prio), 8),          \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_mbox9_isr_prio), 9),          \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_mbox10_isr_prio),10),         \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_mbox11_isr_prio),11),         \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_mbox12_isr_prio),12),         \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_mbox13_isr_prio),13),         \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_mbox14_isr_prio),14),         \
    FLEXCAN_MBOX_INIT((_isr_vec_mbox0), (_mbox15_isr_prio),15),         \
    },                                                                  \
    last_tx_id      : 0xFFFFFFFF,                                       \
    boff_int        : FLEXCAN_INT_INIT(_boff_isr_vec, _boff_isr_prio),  \
    err_int         : FLEXCAN_INT_INIT(_err_isr_vec, _err_isr_prio),    \
    wake_int        : FLEXCAN_INT_INIT(_wake_isr_vec, _wake_isr_prio),  \
    tx_all_mbox     : _tx_all_mbox,                                     \
    free_mboxes      : ((_std_mboxes) + (_ext_mboxes)),                 \
    state            : CYGNUM_CAN_STATE_ACTIVE,                         \
    rx_all           : true,                                            \
    mboxes_std_cnt   : _std_mboxes,                                     \
    mboxes_ext_cnt   : _ext_mboxes,                                     \
    mboxes_rx_all_cnt : ((_std_mboxes) + (_ext_mboxes))                 \
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
// Define number of message boxes if they are not defined yet
//
#ifndef CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_STD_MBOXES
#define CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_STD_MBOXES 0
#endif
#ifndef CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_EXT_MBOXES
#define CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_EXT_MBOXES 0
#endif
#ifndef CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN1_STD_MBOXES
#define CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN1_STD_MBOXES 0
#endif
#ifndef CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN1_EXT_MBOXES
#define CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN1_EXT_MBOXES 0
#endif


#ifdef CYGPKG_DEVS_CAN_MCF52xx_FLEXCAN0
//
// FlexCAN channel initialisation for FlexCAN channel 0
//
FLEXCAN_INFO(flexcan_can0_info, 
             HAL_MCF52xx_MBAR + HAL_MCF52xx_FLEXCAN0_BASE,
             HAL_MCF52xx_FLEXCAN0_MBOX0_ISRVEC,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX0,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX1,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX2,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX3,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX4,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX5,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX6,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX7,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX8,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX9,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX10,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX11,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX12,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX13,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX14,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_MBOX15,
             HAL_MCF52xx_FLEXCAN0_BOFF_ISRVEC,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_BOFFINT,  
             HAL_MCF52xx_FLEXCAN0_ERR_ISRVEC,          
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_ERRINT,
             HAL_MCF52xx_FLEXCAN0_WAKE_ISRVEC,       
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN0_WAKEINT,
             CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_DEFAULT_TX_MBOX,
             CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_STD_MBOXES,
             CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN0_EXT_MBOXES);
#endif // CYGPKG_DEVS_CAN_MCF52xx_FLEXCAN[set ::flexcan]

#ifdef CYGPKG_DEVS_CAN_MCF52xx_FLEXCAN1
//
// FlexCAN channel initialisation for FlexCAN channel 1
//
FLEXCAN_INFO(flexcan_can0_info,
             HAL_MCF52xx_MBAR + HAL_MCF52xx_FLEXCAN0_BASE,
             HAL_MCF52xx_FLEXCAN1_MBOX0_ISRVEC,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_MBOX0,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_MBOX1,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_MBOX2,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_MBOX3,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_MBOX4,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_MBOX5,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_MBOX6,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_MBOX7,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_MBOX8,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_MBOX9,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_MBOX10,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_MBOX11,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_MBOX12,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_MBOX13,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_MBOX14,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_MBOX15,
             HAL_MCF52xx_FLEXCAN1_BOFF_ISRVEC,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_BOFFINT,
             HAL_MCF52xx_FLEXCAN1_ERR_ISRVEC,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_ERRINT,
             HAL_MCF52xx_FLEXCAN1_WAKE_ISRVEC,
             CYGNUM_DEVS_CAN_MCF52xx_ISR_PRIORITY_FLEXCAN1_WAKEINT,
             CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN1_DEFAULT_TX_MBOX,
             CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN1_STD_MBOXES,
             CYGNUM_DEVS_CAN_MCF52xx_FLEXCAN1_EXT_MBOXES);
#endif // CYGPKG_DEVS_CAN_MCF52xx_FLEXCAN1

//
// message box structure for hardware access of message box
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
// flexcan register layout for hardware register access of FlexCAN modul
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
static Cyg_ErrNo   flexcan_get_config(can_channel *chan, cyg_uint32 key, const void* buf, cyg_uint32* len);
static bool        flexcan_putmsg(can_channel *priv, cyg_can_message *pmsg, void *pdata);
static bool        flexcan_getevent(can_channel *priv, cyg_can_event *pevent, void *pdata);
static void        flexcan_start_xmit(can_channel* chan);
static void        flexcan_stop_xmit(can_channel* chan);

//
// TX and RX ISRs and DSRs
//
#ifdef CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_STD_CAN_ID
static cyg_uint32  flexcan_mbox_rx_std_isr(cyg_vector_t, cyg_addrword_t);
static void        flexcan_mbox_rx_std_dsr(cyg_vector_t, cyg_ucount32, cyg_addrword_t);
#endif // #ifdef CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_STD_CAN_ID
#ifdef CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_EXT_CAN_ID
static cyg_uint32  flexcan_mbox_rx_ext_isr(cyg_vector_t, cyg_addrword_t);
static void        flexcan_mbox_rx_ext_dsr(cyg_vector_t, cyg_ucount32, cyg_addrword_t);
#endif // CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_STD_EXT_ID
static cyg_uint32  flexcan_mbox_rx_filt_isr(cyg_vector_t, cyg_addrword_t);
static cyg_uint32  flexcan_mbox_tx_isr(cyg_vector_t, cyg_addrword_t);
static void        flexcan_mbox_tx_dsr(cyg_vector_t, cyg_ucount32, cyg_addrword_t);
static void        flexcan_mbox_rx_filt_dsr(cyg_vector_t, cyg_ucount32, cyg_addrword_t );

//
// All other flexcan interrupt handlers
//
static cyg_uint32  flexcan_err_isr(cyg_vector_t, cyg_addrword_t);
static void        flexcan_err_dsr(cyg_vector_t, cyg_ucount32, cyg_addrword_t);
static cyg_uint32  flexcan_busoff_isr(cyg_vector_t, cyg_addrword_t);
static void        flexcan_busoff_dsr(cyg_vector_t, cyg_ucount32, cyg_addrword_t);
static cyg_uint32  flexcan_wake_isr(cyg_vector_t, cyg_addrword_t);
static void        flexcan_wake_dsr(cyg_vector_t, cyg_ucount32, cyg_addrword_t);

//
// Flexcan utility functions
//
static bool flexcan_cfg_mbox_tx(flexcan_mbox *pmbox, cyg_can_message  *pmsg, bool rtr);
static void flexcan_cfg_mbox_rx(flexcan_mbox *pmbox, cyg_can_message  *pmsg, bool enable);
static void flexcan_read_from_mbox(can_channel *chan, cyg_uint8 mbox, cyg_can_event *pevent, cyg_uint8 *ctrlstat);
static void flexcan_set_acceptance_mask(cyg_uint16 *rxmask_reg, cyg_uint32 mask, cyg_can_id_type ext);
static void flexcan_start_chip(can_channel *chan);
static void flexcan_enter_standby(can_channel *chan, bool selfwake);
static void flexcan_stop_chip(can_channel *chan);
static void flexcan_leave_standby(can_channel *chan);
static bool flexcan_set_baud(can_channel *chan, cyg_uint16 baudrate);
static bool flexcan_config(can_channel* chan, cyg_can_info_t* config, cyg_bool init);
static cyg_int8 flexcan_alloc_mbox(flexcan_info *info);
static void flexcan_disable_mbox(can_channel *chan, cyg_uint32 mbox_id);
static void flexcan_setup_rxmbox(can_channel *chan, cyg_uint32 mbox_id, cyg_ISR_t *isr, cyg_can_message *pmsg, bool enable, bool int_enable);
static void flexcan_setup_txmbox(can_channel *chan, cyg_uint32 mbox_id, cyg_can_message *pmsg);
static void flexcan_setup_rtrmbox(can_channel *chan, cyg_uint32 mbox_id, cyg_can_message *pmsg);
static void flexcan_mboxint_enable(flexcan_info *info, cyg_uint32 mbox_id);
static void flexcan_mboxint_disable(flexcan_info *info, cyg_uint32 mbox_id);
static void flexcan_config_rx_all(can_channel *chan);
static void flexcan_enable_rxmbox(can_channel *chan, cyg_uint32 mbox_id);


CAN_LOWLEVEL_FUNS(flexcan_lowlevel_funs,
                  flexcan_putmsg,
                  flexcan_getevent,
                  flexcan_get_config,
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
// Enable hardware message box for reception
//===========================================================================
static __inline__  void flexcan_hwmbox_enable_rx(flexcan_regs *flexcan, cyg_uint32 mbox_id)
{
    HAL_WRITE_UINT8(&(flexcan->mbox[mbox_id].ctrlstat), MBOX_RXCODE_EMPTY);
}


//===========================================================================
// Disable hardware message box
//===========================================================================
static __inline__  void flexcan_hwmbox_disable(flexcan_regs *flexcan, cyg_uint32 mbox_id)
{
    HAL_WRITE_UINT8(&(flexcan->mbox[mbox_id].ctrlstat), MBOX_RXCODE_NOT_ACTIVE);
}


//===========================================================================
// lock mbox by reading control status register
//===========================================================================
static __inline__  void flexcan_hwmbox_lock(flexcan_regs *flexcan, cyg_uint32 mbox_id, cyg_uint8 *pctrlstat)
{
    HAL_READ_UINT8(&(flexcan->mbox[mbox_id].ctrlstat), *pctrlstat); // this read will lock the mbox
}


//===========================================================================
//  Enable message box interrupt for one message box
//===========================================================================
static void flexcan_mboxint_enable(flexcan_info *info, cyg_uint32 mbox_id)
{
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    
    info->imask_shadow |= (0x0001 << mbox_id);
    HAL_WRITE_UINT16(&flexcan->IMASK, info->imask_shadow);
}


//===========================================================================
//  Disable message box interrupt for one message box
//===========================================================================
static void flexcan_mboxint_disable(flexcan_info *info, cyg_uint32 mbox_id)
{
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    
    info->imask_shadow &= ~(0x0001 << mbox_id);
    HAL_WRITE_UINT16(&flexcan->IMASK, info->imask_shadow);
}


//===========================================================================
// Allocate message box
// Try to find a free message box and return its ID
//===========================================================================
static cyg_int8 flexcan_alloc_mbox(flexcan_info *info)
{
    cyg_uint8     i;
    cyg_int8      res = CYGNUM_CAN_MSGBUF_NA;
    
    if (info->free_mboxes)
    {  
        for (i = (FLEXCAN_MBOX_RX_CNT - info->free_mboxes); i <= FLEXCAN_MBOX_RX_MAX; ++i)
        {
            if (MBOX_STATE_DISABLED == info->mboxes[i].state)
            {
                info->free_mboxes--;
                res = i;
                break;
            }               
        }
    } // if (info->free_mboxes)
    
    return res;
}


//===========================================================================
// Enable a previously configured rx mbox - a mbox ready to recive
//===========================================================================
static void flexcan_enable_rxmbox(can_channel     *chan,
                                  cyg_uint32       mbox_id)
{
    flexcan_info *info = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;

    flexcan_hwmbox_enable_rx(flexcan, mbox_id);
    flexcan_mboxint_enable(info, mbox_id);
}

//===========================================================================
// Prepare message buffer filter
// Setup a RX message box for reception of a certain CAN identifier but do
// not enable it
//===========================================================================
static void flexcan_setup_rxmbox(can_channel     *chan, 
                                 cyg_uint32       mbox_id,
                                 cyg_ISR_t       *isr,
                                 cyg_can_message *pmsg,
                                 bool             enable,
                                 bool             int_enable)
{
    flexcan_info      *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs      *flexcan = (flexcan_regs *)info->base;
    flexcan_mbox_info *pmbox;
    cyg_DSR_t         *dsr_func = &flexcan_mbox_rx_filt_dsr;
    
    //
    // Set state of message buffer accoring to ISR function that
    // will be registered
    //
#ifdef CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_STD_CAN_ID
    if (*isr == flexcan_mbox_rx_std_isr)
    {
        //
        // If we have only one single RX all message box then we use
        // the filter ISR instead of RX all standard ISR because it
        // is better suited for a single RX mbox
    //
        if (info->mboxes_std_cnt > 1)
        {
            info->mboxes[mbox_id].state = MBOX_STATE_RX_ALL_STD;
            dsr_func = &flexcan_mbox_rx_std_dsr;
        }
        else
        {
            info->mboxes[mbox_id].state = MBOX_STATE_RX_FILT;
            isr = flexcan_mbox_rx_filt_isr;
        }
    }
    else
#endif // CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_STD_CAN_ID
#ifdef CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_EXT_CAN_ID
    if (*isr == flexcan_mbox_rx_ext_isr)
    {
        //
        // If we have only one single RX all message box then we use
        // the filter ISR instead of RX all standard ISR because it
        // is better suited for a single RX mbox
        //
        if (info->mboxes_ext_cnt > 1)
        {
            info->mboxes[mbox_id].state = MBOX_STATE_RX_ALL_EXT;
            dsr_func = &flexcan_mbox_rx_ext_dsr;
        }
        else
        {
            info->mboxes[mbox_id].state = MBOX_STATE_RX_FILT;
            isr = flexcan_mbox_rx_filt_isr;
        }
    }
    else
#endif // CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_EXT_CAN_ID
    if (*isr == flexcan_mbox_rx_filt_isr)
    {
        info->mboxes[mbox_id].state = MBOX_STATE_RX_FILT;
    }
    else
    {
        CYG_ASSERT(0, "Invalid ISR function pointer");
    }

    pmbox = &info->mboxes[mbox_id];
    flexcan_cfg_mbox_rx(&flexcan->mbox[mbox_id], pmsg, enable);

    cyg_drv_interrupt_create(pmbox->isr_vec,
                             pmbox->isr_priority,
                             (cyg_addrword_t) chan,
                             isr,
                             dsr_func,
                             &(pmbox->interrupt_handle),
                             &(pmbox->interrupt));
    cyg_drv_interrupt_attach(pmbox->interrupt_handle);
    cyg_drv_interrupt_unmask(pmbox->isr_vec);
    
    //
    // now enable interrupt for this message box - but only if we
    // really should do it
    //
    if (int_enable)
    {
    flexcan_mboxint_enable(info, mbox_id);
    }
}


//===========================================================================
// Disable a message box - after this call a message box is available
// again for message filters or remote buffers
//===========================================================================
static void flexcan_disable_mbox(can_channel *chan, cyg_uint32 mbox_id)
{
    flexcan_info      *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs      *flexcan = (flexcan_regs *)info->base;
    flexcan_mbox_info *pmbox;
    
    //
    // if message box is already disabled we do not need to disable it
    // here
    //
    if (MBOX_STATE_DISABLED == info->mboxes[mbox_id].state)
    {
        return;
    }
    
    HAL_WRITE_UINT8(&flexcan->mbox[mbox_id].ctrlstat, MBOX_RXCODE_NOT_ACTIVE);
    info->mboxes[mbox_id].state = MBOX_STATE_DISABLED;
    pmbox = &info->mboxes[mbox_id];
    
    //
    // now disable interrupts for this message box and free all
    // interrupt resources
    //
    flexcan_mboxint_disable(info, mbox_id);
    cyg_drv_interrupt_mask(pmbox->isr_vec);
    cyg_drv_interrupt_detach(pmbox->interrupt_handle);
    cyg_drv_interrupt_delete(pmbox->interrupt_handle);
    
    info->free_mboxes++;
}


//===========================================================================
// Setup a transmit message box
//===========================================================================
static void flexcan_setup_txmbox(can_channel      *chan,
                                 cyg_uint32        mbox_id,
                                 cyg_can_message  *pmsg)
{
    flexcan_info      *info   = (flexcan_info *)chan->dev_priv;
    flexcan_regs      *flexcan = (flexcan_regs *)info->base;
    flexcan_mbox_info *pmbox;
    
    info->mboxes[mbox_id].state = MBOX_STATE_TX;
    pmbox = &info->mboxes[mbox_id];
    flexcan_cfg_mbox_tx(&flexcan->mbox[mbox_id], pmsg, false);

    //
    // prepare message box interrupt for message box 
    //
    cyg_drv_interrupt_create(pmbox->isr_vec,
                             pmbox->isr_priority,
                             (cyg_addrword_t) chan,
                             &flexcan_mbox_tx_isr,
                             &flexcan_mbox_tx_dsr,
                             &(pmbox->interrupt_handle),
                             &(pmbox->interrupt));
    cyg_drv_interrupt_attach(pmbox->interrupt_handle);
    cyg_drv_interrupt_unmask(pmbox->isr_vec);
    
    //
    // now enable interrupt for this message box
    //
    flexcan_mboxint_enable(info, mbox_id);
}


//===========================================================================
// Setup a RTR response message box
//===========================================================================
static void flexcan_setup_rtrmbox(can_channel      *chan,
                                  cyg_uint32        mbox_id,
                                  cyg_can_message  *pmsg)
{
    flexcan_info      *info   = (flexcan_info *)chan->dev_priv;
    flexcan_regs      *flexcan = (flexcan_regs *)info->base;
    
    info->mboxes[mbox_id].state = MBOX_STATE_REMOTE_TX;
    flexcan_cfg_mbox_tx(&flexcan->mbox[mbox_id], pmsg, true);
}


//===========================================================================
// Setup the list of message boxes ready to receive a message
//===========================================================================
static void flexcan_setup_rxmbox_circbuf(flexcan_rxmbox_circbuf *pbuf)
{
    pbuf->count  = 0;
    pbuf->idx_rd = 0;
    pbuf->idx_wr = 0;
}


//===========================================================================
// Setup flexCAN modul for reception of any kind of message
//===========================================================================
static void flexcan_config_rx_all(can_channel *chan)
{
    flexcan_info *info       = (flexcan_info *)chan->dev_priv;
    flexcan_regs       *flexcan = (flexcan_regs *)info->base;
    cyg_int8           i;
    
        //
    // setup all available message boxes for reception of of messages
    // All standard and extended message buffers will be configured
        //
    for (i = 0; i < info->mboxes_rx_all_cnt; ++i)
             {
        cyg_can_message filter_param;
        filter_param.id  = 0;

        //
        // configure message buffers for standard frames
        //
#ifdef CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_STD_CAN_ID
        if (i < info->mboxes_std_cnt)
                 {
            filter_param.ext = CYGNUM_CAN_ID_STD;
            flexcan_setup_rxmbox(chan, i, &flexcan_mbox_rx_std_isr, &filter_param, false, true);
                 }
#endif // CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_STD_CAN_ID

       //
       // configure message buffers for extended frames
       //
#ifdef CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_EXT_CAN_ID
        else
                 {
            filter_param.ext = CYGNUM_CAN_ID_EXT;
            flexcan_setup_rxmbox(chan, i, &flexcan_mbox_rx_ext_isr, &filter_param, false, true);
                 }
#endif // CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_STD_CAN_ID
            }
       
        //
    // We need to receive all available CAN messages so we have to set the acceptance filter
    // properly
    //
    flexcan_set_acceptance_mask(&flexcan->RXGMASK_HI,  FLEXCAN_ACCEPTANCE_MASK_RX_ALL,  CYGNUM_CAN_ID_EXT);
    flexcan_set_acceptance_mask(&flexcan->RX14MASK_HI, FLEXCAN_ACCEPTANCE_MASK_RX_ALL, CYGNUM_CAN_ID_EXT);
    info->free_mboxes = FLEXCAN_MBOX_RX_CNT - info->mboxes_rx_all_cnt;
    info->rx_all = true;

    //
    // now finally setup the first active message boxes and enable ist
        //     
#ifdef CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_STD_CAN_ID
    if (info->mboxes_std_cnt)
             { 
        flexcan_setup_rxmbox_circbuf(&info->rxmbox_std_circbuf);
        flexcan_enable_rxmbox(chan, 0);
    }
#endif // CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_STD_CAN_ID
                             
#ifdef CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_EXT_CAN_ID
    if (info->mboxes_ext_cnt)
                 {
        flexcan_setup_rxmbox_circbuf(&info->rxmbox_ext_circbuf);
        flexcan_enable_rxmbox(chan, info->mboxes_std_cnt);
                 }
#endif // CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_STD_CAN_ID

}

//===========================================================================
// Setup Flex CAN moduls in a state where all message boxes are disabled
// After this call single message filters and buffers can be added
//===========================================================================
static void flexcan_config_rx_none(can_channel *chan)
{
    flexcan_info       *info = (flexcan_info *)chan->dev_priv;
    flexcan_regs       *flexcan = (flexcan_regs *)info->base;
    cyg_int8           i;
                 
                 //
    // setup all RX messages moxes into a disabled state
                 //
    for (i = 0; i < FLEXCAN_MBOX_RX_CNT; ++i)
                 {
        flexcan_disable_mbox(chan, i);
    }
                     
    //
    // If we want to receive only certain CAN identiffiers then the ID does matter and
    // we have to setup the acceptance mask properly
    //
    flexcan_set_acceptance_mask(&flexcan->RXGMASK_HI,  FLEXCAN_ACCEPTANCE_MASK_RX_ID,  CYGNUM_CAN_ID_EXT);
    flexcan_set_acceptance_mask(&flexcan->RX14MASK_HI, FLEXCAN_ACCEPTANCE_MASK_RX_ID,  CYGNUM_CAN_ID_EXT);
    info->free_mboxes = FLEXCAN_MBOX_RX_CNT;
    info->rx_all = false;
}


//===========================================================================
// Configure message buffers
//===========================================================================
static Cyg_ErrNo flexcan_set_config_msgbuf(can_channel *chan, cyg_can_msgbuf_cfg *buf)
{
    Cyg_ErrNo     res = ENOERR;
    flexcan_info *info = (flexcan_info *)chan->dev_priv;

    switch (buf->cfg_id)
                     {
                         //
        // clear all message filters and remote buffers - prepare for message buffer
        // configuration
                         //
        case CYGNUM_CAN_MSGBUF_RESET_ALL :
                     {
                 flexcan_config_rx_none(chan);
                     }
            break;

                     //
        // setup FlexCAN modul for reception of all standard and extended messages
                     //
        case CYGNUM_CAN_MSGBUF_RX_FILTER_ALL :
                     {
                if (!info->rx_all) // if rx_all is enabled we do not need to do anything
                     {
                    flexcan_config_rx_none(chan); // set into default state
                    flexcan_config_rx_all(chan);  // setup RX all state
                     }
             }
             break;
        
        //
        // add single message filter, message with filter ID will be received
        //     
        case CYGNUM_CAN_MSGBUF_RX_FILTER_ADD :
             {
                 cyg_can_filter *filter   = (cyg_can_filter*) buf;
                 
                 //
                 // if FlexCAN is configured to receive all messages then it is not
                 // allowed to add single message filters because then more than
                 // one message buffer would receive the same CAN id
                 //
                 if (info->rx_all)
                 {
                    return -EPERM;
                 }
                 
                 //
                 // try to allocate a free message box - if we have a free one
                 // then we can prepare the message box for reception of the
                 // desired message id
                 //
                 filter->handle = flexcan_alloc_mbox(info);
                 if (filter->handle > CYGNUM_CAN_MSGBUF_NA)
                 {
                     flexcan_setup_rxmbox(chan, filter->handle, &flexcan_mbox_rx_filt_isr, &filter->msg, true, true);
                 }
             }
             break; //CYGNUM_CAN_MSGBUF_RX_FILTER_ADD

                     //
        // Try to add a new RTR response message buffer for automatic transmisson
        // of data frame on reception of a remote frame
                     //
        case CYGNUM_CAN_MSGBUF_REMOTE_BUF_ADD :
                     {
                 cyg_can_remote_buf *rtr_buf    = (cyg_can_remote_buf*) buf;
                 rtr_buf->handle = flexcan_alloc_mbox(info);
                     
                 if (rtr_buf->handle > CYGNUM_CAN_MSGBUF_NA)
                     {
                     //
                     // if we have a free message buffer then we setup this buffer
                     // for remote frame reception
                     //
                     flexcan_setup_rtrmbox(chan, rtr_buf->handle, &rtr_buf->msg);
                 }
                     }
             break;
                     
                     //
        // write data into remote response buffer
        //
        case CYGNUM_CAN_MSGBUF_REMOTE_BUF_WRITE :
             {
                 cyg_can_remote_buf *rtr_buf    = (cyg_can_remote_buf*) buf;

                 //
                 // If we have a valid rtr buf handle then we can store data into
                 // rtr message box
                     // 
                 if ((rtr_buf->handle >= 0) && (rtr_buf->handle <= FLEXCAN_MBOX_RX_MAX))
                 {
                     flexcan_regs *flexcan = (flexcan_regs *)info->base;
                     flexcan_cfg_mbox_tx(&flexcan->mbox[rtr_buf->handle], &rtr_buf->msg, true);
                 }
                 else
                 {
                    return -EINVAL;
                 }    
             }
             break;
    } // switch (buf->cfg_id)
        
     return res;
}

//===========================================================================
// Set device configuration
//===========================================================================
static Cyg_ErrNo
flexcan_set_config(can_channel *chan, cyg_uint32 key, const void* buf, cyg_uint32* len)
{
    Cyg_ErrNo     res = ENOERR;

    switch(key)
             {
                 //
        //Setup a new CAN configuration. This will i.e. setup a new baud rate
                 //
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
                 
                 //
        // configure message buffers
                 //
        case CYG_IO_SET_CONFIG_CAN_MSGBUF :
             {
                cyg_can_msgbuf_cfg *msg_buf = (cyg_can_msgbuf_cfg *) buf;

                if (*len != sizeof(cyg_can_msgbuf_cfg))
                {
                    return -EINVAL;
                }

                flexcan_set_config_msgbuf(chan, msg_buf);
             }
             break;
        
        //
        // Change CAN state of FlexCAN modul. This function will set the FlexCAN
        // modul into STOPPED, ACTIVE or STANDBY state
        //    
        case CYG_IO_SET_CONFIG_CAN_MODE :
             {
                cyg_can_mode   *can_mode  = (cyg_can_mode*) buf;
                
                if (*len != sizeof(cyg_can_mode)) 
                {
                    return -EINVAL;
                }
                *len = sizeof(cyg_can_mode);
                
                //
                // decide what to do acording to mode
                //
                switch (*can_mode)
                {
                    case CYGNUM_CAN_MODE_STOP :      // stop FlexCANm modul
                         flexcan_stop_chip(chan);
                         break;
                         
                    case CYGNUM_CAN_MODE_START :     // start FlexCAN modul
                         flexcan_leave_standby(chan);
                         break;
                          
                    case CYGNUM_CAN_MODE_STANDBY :   // set FlexCAN modul into standby state
                         flexcan_enter_standby(chan, true);
                         break;
                }
             }
             break; // case CYG_IO_SET_CONFIG_CAN_MODE :         
    } // switch (key)
    
    return res;
}


//===========================================================================
//  Query device configuration
//===========================================================================
static Cyg_ErrNo
flexcan_get_config(can_channel *chan, cyg_uint32 key, const void* buf, cyg_uint32* len)
{
    Cyg_ErrNo     res  = ENOERR;
    flexcan_info *info = (flexcan_info *)chan->dev_priv;
    
    switch(key)
    {
        //
        // query state of CAN controller
        //
        case CYG_IO_GET_CONFIG_CAN_STATE :
             {
                cyg_can_state *can_state  = (cyg_can_state*) buf;
                
                if (*len != sizeof(cyg_can_state)) 
                {
                    return -EINVAL;
                }
                *len = sizeof(cyg_can_state);
                *can_state = info->state;
             }
             break;
        
        //
        // Query message box information - returns available and free message
        // boxes
        //     
        case CYG_IO_GET_CONFIG_CAN_MSGBUF_INFO :
             {
                 cyg_can_msgbuf_info *mbox_info  = (cyg_can_msgbuf_info*) buf;
                
                 if (*len != sizeof(cyg_can_msgbuf_info)) 
                 {
                     return -EINVAL;
                 }
                *len = sizeof(cyg_can_msgbuf_info);
                
                 mbox_info->count = FLEXCAN_MBOX_RX_CNT;
                 mbox_info->free  = info->free_mboxes;
             }
             break;
        
        //
        // Query hardware description of FlexCAN device driver
        //     
        case CYG_IO_GET_CONFIG_CAN_HDI :
             {
                cyg_can_hdi *hdi = (cyg_can_hdi *)buf;
                //
                // comes from high level driver so we do not need to
                // check buffer size here
                //             
                hdi->support_flags = CYGNUM_CAN_HDI_FRAMETYPE_EXT_ACTIVE
                                   | CYGNUM_CAN_HDI_FULLCAN;
#ifdef CYGOPT_IO_CAN_SUPPORT_TIMESTAMP 
                hdi->support_flags |= CYGNUM_CAN_HDI_TIMESTAMP;
#endif
             }
             break;
             
        default :
            res = -EINVAL;
    }// switch(key)
    
    return res;
} 


//===========================================================================
//  Check if we received self transmitted frame
//===========================================================================
static bool flexcan_is_no_self_tx(cyg_can_event *pevent, flexcan_info *info, flexcan_mbox_info *pmbox)
{
    //
    // If we received a self transmitted frame
    // then this is not really an rx event and we return false. We rely on the
    // fact here that two devices in network do not send the same identifier
    //
    if (pevent->msg.id == info->last_tx_id)
    {
        info->last_tx_id = 0xFFFFFFFF; // set last received ID to an invalid value
        return false;
    }
    else
    {
        pevent->flags |= CYGNUM_CAN_EVENT_RX;

        //
        // check if an overun occured in this message box
        //
        if ((pmbox->ctrlstat_shadow & MBOX_RXCODE_OVERRUN) == MBOX_RXCODE_OVERRUN)
        {
            pevent->flags |= CYGNUM_CAN_EVENT_OVERRUN_RX;
        }

        return true;
    }
}


//===========================================================================
//  Read one event from can hardware - called from high level I/O CAN driver
//===========================================================================
static bool flexcan_getevent(can_channel *chan, cyg_can_event *pevent, void *pdata)
{
    flexcan_info *info           = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan        = (flexcan_regs *)info->base;
    bool          res            = true;
    cyg_uint16    estat;
    cyg_uint8     event_id       = *((cyg_uint8 *)pdata);
     
    //   
    // if event_id is 0 - 15 the we have a message box event
    //
    if (event_id < FLEXCAN_ERR_EVENT)
    {   
        flexcan_mbox_info *pmbox_info;
        pmbox_info = &info->mboxes[event_id];
        
        //
        // Deceide what to do according to type of message box that caused this event
        //
        switch (pmbox_info->state)
        {
            //
            // If we have an RX event then we need to read the received data from
            // message box that caused this event and fill it into message queue of
            // high level I/O CAN driver. We could handle this stuff in a function
            // because it is the same like MBOX_STATE_RX_ALL_EXT but speed is more
            // important here than codesize
            //
            case MBOX_STATE_RX_ALL_STD:
            case MBOX_STATE_RX_ALL_EXT:
            case MBOX_STATE_RX_FILT:
            {
                 //
                 // read data from message box - during processing of this function
                 // the message box is locked and cannot receive further messages
                 //
                 flexcan_read_from_mbox(chan, event_id, pevent, &(pmbox_info->ctrlstat_shadow));
                 res = flexcan_is_no_self_tx(pevent, info, pmbox_info);
                 }
                 break;           

#ifdef CYGOPT_IO_CAN_TX_EVENT_SUPPORT
            //
            // If a TX message box cause the event then we store the last transmitted
            // message into the receive message queue
            //
            case MBOX_STATE_TX:  
                 pevent->flags = CYGNUM_CAN_EVENT_TX;
                 pevent->msg = info->last_tx_msg;
                 break;
#endif // CYGOPT_IO_CAN_TX_EVENT_SUPPORT
                            
            case MBOX_STATE_REMOTE_TX:
                 break;
                 
            default:
                 res = false;
        } // switch (pmbox->state)
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
                 
            case FLEXCAN_WAKE_EVENT:
                 pevent->flags = CYGNUM_CAN_EVENT_LEAVING_STANDBY;
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
    flexcan_info      *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs      *flexcan = (flexcan_regs *)info->base;
    cyg_uint8          mbox    = *((cyg_uint8 *)pdata);
    flexcan_mbox_info *pmbox   = &info->mboxes[mbox];
    cyg_uint16         iflag;
   
    HAL_READ_UINT16(&flexcan->IFLAG, iflag);
    
    //
    // check if device is busy sending a message
    //
    if (pmbox->busy)
    {
        //
        // if device is busy and the interrupt flag is set, then we know
        // that device is not busy any longer
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
    
    pmbox->busy    = true;        // mark transmitter as busy
    info->last_tx_id  = pmsg->id; // store message in order to identify self recieved frames 

#ifdef CYGOPT_IO_CAN_TX_EVENT_SUPPORT 
    info->last_tx_msg = *pmsg;    // store the transmitted message for TX events
#endif  
  
    flexcan_cfg_mbox_tx(&flexcan->mbox[mbox], pmsg, false); // send message
    
    return true;
}


//===========================================================================
// Flexcan start xmit - If the chip is in standby mode then a call to this
// function will cause the FlexCAN modul to leave the standby mode. So
// the output queue should be empty before entering stadby mode
//===========================================================================
static void flexcan_start_xmit(can_channel* chan)
{
    flexcan_info         *info    = (flexcan_info *)chan->dev_priv;
    CYG_INTERRUPT_STATE   saved_state;

    HAL_DISABLE_INTERRUPTS(saved_state);
    
    //
    // if we are in standby state the we leave standby state now. This is
    // the reason that the user should wait until the TX queue is empty before 
    // entering standby mode- or he should drain or flush the TX queue
    //
    if (CYGNUM_CAN_STATE_STANDBY == info->state)
    {
        flexcan_leave_standby(chan);
    }
    
    //
    // Now enable message box 15 interrupts
    //
    flexcan_mboxint_enable(info, info->tx_all_mbox);
    
    //
    // kick transmitter
    //
    chan->callbacks->xmt_msg(chan, &info->tx_all_mbox);  // Kick transmitter (if necessary)
    
    HAL_RESTORE_INTERRUPTS(saved_state);
}


//===========================================================================
// Flexcan stop transmission
//===========================================================================
static void flexcan_stop_xmit(can_channel* chan)
{
    flexcan_info         *info    = (flexcan_info *)chan->dev_priv;
    CYG_INTERRUPT_STATE  saved_state;

    HAL_DISABLE_INTERRUPTS(saved_state);
    
    //
    // Now disable message box 15 interrupts
    //
    flexcan_mboxint_disable(info, info->tx_all_mbox);

    HAL_RESTORE_INTERRUPTS(saved_state);
}


//===========================================================================
// Configure flexcan channel
//===========================================================================
static bool flexcan_config(can_channel* chan, cyg_can_info_t* config, cyg_bool init)
{
    flexcan_info    *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs    *flexcan = (flexcan_regs *)info->base;
    cyg_uint16       tmp16;
    cyg_uint8        tmp8;
    cyg_uint8        i;
    
    //
    // the first thing we need to do is to stop the chip
    //
    flexcan_stop_chip(chan);  
    
    //
    // if this is the first initialisation of the FlexCAN modul we need to execute
    // some extra steps here
    //
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
        // deactivate all message buffers - this is mandatory for configuration
        // of message buffers
        //
        for (i = FLEXCAN_MBOX_MIN; i <= FLEXCAN_MBOX_MAX; ++i)
        {
            HAL_WRITE_UINT16(&flexcan->mbox[i], MBOX_RXCODE_NOT_ACTIVE);
        }   
        
        //
        // mask all interrupts
        //
        info->imask_shadow = 0x0000;
        HAL_WRITE_UINT16(&flexcan->IMASK, info->imask_shadow);
        HAL_READ_UINT8(&flexcan->CANCTRL0, tmp8);
        HAL_WRITE_UINT8(&flexcan->CANCTRL0, tmp8 & ~(FLEXCAN_CTRL0_BOFFMSK | FLEXCAN_CTRL0_ERRMASK));    
        
        //
        // setup bus arbitration mode - the LBUF bit defines the 
        // transmit-first scheme 0 = message buffer with lowest ID
        // 1 = message buffer with lowest number. We use lowest ID here
        //
        HAL_READ_UINT8(&flexcan->CANCTRL1, tmp8);
        HAL_WRITE_UINT8(&flexcan->CANCTRL1, (tmp8 & ~FLEXCAN_CTRL1_LBUF));    
        
        //
        // setup all rx message buffers
        //
        flexcan_config_rx_all(chan);
    
        //
        // bus off interrupt and error interrupt
        //
        HAL_READ_UINT8(&flexcan->CANCTRL0, tmp8);
        HAL_WRITE_UINT8(&flexcan->CANCTRL0, tmp8 | (FLEXCAN_CTRL0_BOFFMSK | FLEXCAN_CTRL0_ERRMASK));                             
    } // if (init)
                      
    flexcan_set_baud(chan, config->baud); // setup baud rate
    
    //
    // store new config values
    //
    if (config != &chan->config) 
    {
        chan->config = *config;
    }
    flexcan_start_chip(chan);             // now we can start the chip again
    
    return true;
}

//===========================================================================
/// First initialisation and reset of CAN modul.
//===========================================================================
static bool flexcan_init(struct cyg_devtab_entry* devtab_entry)
{
    can_channel       *chan    = (can_channel*)devtab_entry->priv;
    flexcan_info      *info    = (flexcan_info *)chan->dev_priv;
    flexcan_mbox_info *pmbox;

    if (!flexcan_config(chan, &chan->config, true))
    {
        return false;
    }
       
    //
    // prepare message box interrupt for message box 15 - the tx message box
    //
    pmbox = &info->mboxes[info->tx_all_mbox];
    cyg_drv_interrupt_create(pmbox->isr_vec,
                             pmbox->isr_priority,
                             (cyg_addrword_t) chan,
                             &flexcan_mbox_tx_isr,
                             &flexcan_mbox_tx_dsr,
                             &(pmbox->interrupt_handle),
                             &(pmbox->interrupt));
    cyg_drv_interrupt_attach(pmbox->interrupt_handle);
    cyg_drv_interrupt_unmask(pmbox->isr_vec);
    
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
    
    //
    // prepare wake interrupt
    //
    cyg_drv_interrupt_create(info->wake_int.isr_vec,
                             info->wake_int.isr_priority,
                             (cyg_addrword_t) chan,
                             &flexcan_wake_isr,
                             &flexcan_wake_dsr,
                             &(info->wake_int.interrupt_handle),
                             &(info->wake_int.interrupt));
    cyg_drv_interrupt_attach(info->wake_int.interrupt_handle);
    cyg_drv_interrupt_unmask(info->wake_int.isr_vec);
        
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
// Bus off interrupt handler
//===========================================================================
static cyg_uint32  flexcan_wake_isr(cyg_vector_t vec, cyg_addrword_t data)
{
    can_channel  *chan    = (can_channel *)data;
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    cyg_uint16    estat;
    
    //
    // first we disable wake interrupts - we set the mcr register to
    // zero in order to bring it back to normal state
    //
    HAL_WRITE_UINT16(&flexcan->CANMCR, 0);
    
    //
    // for clearing the interrupt we first read the flag register as 1
    // and then write it as 1 (and not as zero like the manual stated)
    // we clear only the flag of this interrupt and leave all other
    // message box interrupts untouched
    //
    HAL_READ_UINT16(&flexcan->ESTAT, estat);
    HAL_WRITE_UINT16(&flexcan->ESTAT, FLEXCAN_ESTAT_WAKEINT);
       
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
static void flexcan_wake_dsr(cyg_vector_t vec, cyg_ucount32 count, cyg_addrword_t data)
{
    can_channel  *chan     = (can_channel *)data;
    flexcan_info *info     = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan  = (flexcan_regs *)info->base;
    cyg_uint8     event_id = FLEXCAN_WAKE_EVENT;
    cyg_uint8     ctrl0;

    //
    // signal CAN event to generic IO CAN driver - it will do any further 
    // processing we will enable all other interrupts after the call to 
    // rcv_event because the user should receive this event as the first
    // event after FlexCAN leaves standby.
    //            
    chan->callbacks->rcv_event(chan, &event_id);
    
    //
    // for standby we disabled all interrut source so we have to enable
    // it here again for normal operation
    //
    HAL_WRITE_UINT16(&flexcan->IMASK, info->imask_shadow);
    HAL_READ_UINT8(&flexcan->CANCTRL0, ctrl0);
    HAL_WRITE_UINT8(&flexcan->CANCTRL0, ctrl0 |(FLEXCAN_CTRL0_BOFFMSK | FLEXCAN_CTRL0_ERRMASK));
}


//===========================================================================
// Flexcan message box isr for rx messages if message filtering is
// enabled
//===========================================================================
static cyg_uint32 flexcan_mbox_rx_filt_isr(cyg_vector_t vec, cyg_addrword_t data)
{
    can_channel  *chan    = (can_channel *)data;
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    cyg_uint16    iflag;
    
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
    flexcan_mboxint_disable(info, mbox);
    info->mboxes[mbox].ctrlstat_shadow = FLEXCAN_CTRLSTAT_NOT_READ;
    
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


#ifdef CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_EXT_CAN_ID
//===========================================================================
// Flexcan message box isr for extended identifiers if reception of all
// available messages is enabled
//===========================================================================
static cyg_uint32 flexcan_mbox_rx_ext_isr(cyg_vector_t vec, cyg_addrword_t data)
{
    can_channel            *chan    = (can_channel *)data;
    flexcan_info           *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs           *flexcan = (flexcan_regs *)info->base;
    cyg_uint16              iflag;
    flexcan_rxmbox_circbuf *prx_mbox_list = &(info->rxmbox_ext_circbuf);

    //
    // number of message box can be calculated from vector that caused
    // interrupt - we pass this message box number as additional data to the
    // callback because it is required in the receive event function later
    //
    cyg_uint8 mbox = vec - info->isr_vec_mbox0;
    if (++prx_mbox_list->count < info->mboxes_ext_cnt)
    {
        prx_mbox_list->idx_wr = (prx_mbox_list->idx_wr + 1) % info->mboxes_ext_cnt;
        flexcan_hwmbox_enable_rx(flexcan, prx_mbox_list->idx_wr + info->mboxes_std_cnt);
        flexcan_hwmbox_lock(flexcan, mbox, &(info->mboxes[mbox].ctrlstat_shadow));
        flexcan_hwmbox_disable(flexcan, mbox);  // now disable this message box - it is already locked
        //
        // first we disable interrupts of this message box - the DSR will
        // reenable it later
        //
        flexcan_mboxint_disable(info, mbox);
    }
    else
    {
        prx_mbox_list->count = info->mboxes_ext_cnt;
        info->mboxes[mbox].ctrlstat_shadow = FLEXCAN_CTRLSTAT_NOT_READ;
    }

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
    // cyg_drv_interrupt_acknowledge(vec); If counter of mbox list is > 1
    // then we know that there is already a DSR running and we do not
    // need to invoke one
    //
    if (prx_mbox_list->count > 1)
    {
        return CYG_ISR_HANDLED;
    }
    else
    {
        return CYG_ISR_CALL_DSR;
    }
}


//===========================================================================
// FlexCAN message box DSR for extended CAN frames
//===========================================================================
static void flexcan_mbox_rx_ext_dsr(cyg_vector_t vec, cyg_ucount32 count, cyg_addrword_t data)
{
    can_channel  *          chan    = (can_channel *)data;
    flexcan_info           *info    = (flexcan_info *)chan->dev_priv;
    flexcan_rxmbox_circbuf *prx_mbox_list = &(info->rxmbox_ext_circbuf);
    cyg_uint8               mbox;
    cyg_uint8               mbox_cnt;

    //
    // we do not process the message box we received as event_id
    // we take the message boxes from the ring buffer
    //
    do
    {
        cyg_drv_isr_lock();
        mbox_cnt = --prx_mbox_list->count;
        cyg_drv_isr_unlock();

        mbox = prx_mbox_list->idx_rd + info->mboxes_std_cnt;
        prx_mbox_list->idx_rd = (prx_mbox_list->idx_rd + 1) % info->mboxes_ext_cnt;
        chan->callbacks->rcv_event(chan, &mbox);
        flexcan_mboxint_enable(info, mbox);

        //
        // if the last message box is enabled, then we have to enable
        // another one now because the last message box is filled already
        //
        if (mbox_cnt == (info->mboxes_ext_cnt - 1))
        {
            cyg_uint8     active_mbox;
            cyg_uint8     next_mbox;
            flexcan_regs *flexcan = (flexcan_regs *)info->base;

            cyg_drv_isr_lock();
            active_mbox = prx_mbox_list->idx_wr;
            next_mbox = prx_mbox_list->idx_wr = (prx_mbox_list->idx_wr + 1) % info->mboxes_ext_cnt;
            cyg_drv_isr_unlock();

            active_mbox += info->mboxes_std_cnt;
            next_mbox += info->mboxes_std_cnt;
            flexcan_hwmbox_lock(flexcan, active_mbox, &(info->mboxes[active_mbox].ctrlstat_shadow));
            flexcan_hwmbox_disable(flexcan, active_mbox);  // now disable this message box - it is already locked
            flexcan_hwmbox_enable_rx(flexcan, next_mbox);
        }
    }
    while (mbox_cnt);
}

#endif // #ifdef CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_EXT_CAN_ID


#ifdef CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_STD_CAN_ID
//===========================================================================
// Flexcan message box isr for standard identifiers if reception of all
// available messages is enabled
//===========================================================================
static cyg_uint32 flexcan_mbox_rx_std_isr(cyg_vector_t vec, cyg_addrword_t data)
{
    can_channel            *chan    = (can_channel *)data;
    flexcan_info           *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs           *flexcan = (flexcan_regs *)info->base;
    cyg_uint16              iflag;
    flexcan_rxmbox_circbuf *prx_mbox_list = &(info->rxmbox_std_circbuf);

    //
    // number of message box can be calculated from vector that caused
    // interrupt - we pass this message box number as additional data to the
    // callback because it is required in the receive event function later
    //
    cyg_uint8 mbox = vec - info->isr_vec_mbox0;
    if (++prx_mbox_list->count < info->mboxes_std_cnt)
    {
        prx_mbox_list->idx_wr = (prx_mbox_list->idx_wr + 1) % info->mboxes_std_cnt;
        flexcan_hwmbox_enable_rx(flexcan, prx_mbox_list->idx_wr);
        flexcan_hwmbox_lock(flexcan, mbox, &(info->mboxes[mbox].ctrlstat_shadow));
        flexcan_hwmbox_disable(flexcan, mbox);  // now disable this message box - it is already locked
        //
        // first we disable interrupts of this message box - the DSR will
        // reenable it later
        //
        flexcan_mboxint_disable(info, mbox);
    }
    else
    {
        prx_mbox_list->count = info->mboxes_std_cnt;
        info->mboxes[mbox].ctrlstat_shadow = FLEXCAN_CTRLSTAT_NOT_READ;
    }

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
    // cyg_drv_interrupt_acknowledge(vec); If counter of mbox list is > 1
    // then we know that there is already a DSR running and we do not
    // need to invoke one
    //
    if (prx_mbox_list->count > 1)
    {
        return CYG_ISR_HANDLED;
    }
    else
    {
        return CYG_ISR_CALL_DSR;
    }
}


//===========================================================================
// Flexcan message box dsr for standard CAN frames
//===========================================================================
static void flexcan_mbox_rx_std_dsr(cyg_vector_t vec, cyg_ucount32 count, cyg_addrword_t data)
{
    can_channel  *          chan    = (can_channel *)data;
    flexcan_info           *info    = (flexcan_info *)chan->dev_priv;
    flexcan_rxmbox_circbuf *prx_mbox_list = &(info->rxmbox_std_circbuf);
    cyg_uint8               mbox;
    cyg_uint8               mbox_cnt;

    //
    // we do not process the message box we received as event_id
    // we take the message boxes from the ring buffer
    //
    do
    {
        cyg_drv_isr_lock();
        mbox_cnt = --prx_mbox_list->count;
        cyg_drv_isr_unlock();

        mbox = prx_mbox_list->idx_rd;
        prx_mbox_list->idx_rd = (prx_mbox_list->idx_rd + 1) % info->mboxes_std_cnt;
        chan->callbacks->rcv_event(chan, &mbox);
        flexcan_mboxint_enable(info, mbox);

        //
        // if the last message box is enabled, then we have to enable
        // another one now because the last message box is filled already
        //
        if (mbox_cnt == (info->mboxes_std_cnt - 1))
        {
            cyg_uint8     active_mbox;
            cyg_uint8     next_mbox;
            flexcan_regs *flexcan = (flexcan_regs *)info->base;

            cyg_drv_isr_lock();
            active_mbox = prx_mbox_list->idx_wr;
            next_mbox = prx_mbox_list->idx_wr = (prx_mbox_list->idx_wr + 1) % info->mboxes_ext_cnt;
            cyg_drv_isr_unlock();

            flexcan_hwmbox_lock(flexcan, active_mbox, &(info->mboxes[active_mbox].ctrlstat_shadow));
            flexcan_hwmbox_disable(flexcan, active_mbox);  // now disable this message box - it is already locked
            flexcan_hwmbox_enable_rx(flexcan, next_mbox);
        }
    }
    while (mbox_cnt);
}
#endif // CYGINT_DEVS_CAN_MCF52xx_FLEXCAN_SUPP_STD_CAN_ID


//===========================================================================
// FlexCAN DSR for message filters
//===========================================================================
static void flexcan_mbox_rx_filt_dsr(cyg_vector_t vec, cyg_ucount32 count, cyg_addrword_t data)
{
    can_channel  *chan    = (can_channel *)data;
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    
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
    flexcan_mboxint_enable(info, mbox);
}

//===========================================================================
// Flexcan message box isr
//===========================================================================
static cyg_uint32 flexcan_mbox_tx_isr(cyg_vector_t vec, cyg_addrword_t data)
{
    can_channel       *chan    = (can_channel *)data;
    flexcan_info      *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs      *flexcan = (flexcan_regs *)info->base;
    cyg_uint16         iflag;
    
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
    flexcan_mboxint_disable(info, mbox);
    
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
static void flexcan_mbox_tx_dsr(cyg_vector_t vec, cyg_ucount32 count, cyg_addrword_t data)
{
    can_channel       *chan    = (can_channel *)data;
    flexcan_info      *info    = (flexcan_info *)chan->dev_priv;
    flexcan_mbox_info *pmbox;
    
    //
    // number of message box can be calculated from vector that caused
    // interrupt - we pass this message box number as additional data to the
    // callback
    //
    cyg_uint8 mbox = vec - info->isr_vec_mbox0; 
    pmbox = &info->mboxes[mbox];
    
#ifdef CYGOPT_IO_CAN_TX_EVENT_SUPPORT 
    //
    // signal CAN TX event to generic IO CAN driver - it will do any further 
    // processing
    //            
    chan->callbacks->rcv_event(chan, &mbox);
#endif
    
    pmbox->busy = false;
       
    //
    // send next message 
    //            
    chan->callbacks->xmt_msg(chan, &mbox);
    
    //
    // reenable interrupts for the message box that caused the DSR to run
    //
    flexcan_mboxint_enable(info, mbox);
}


//===========================================================================
// Start FlexCAN modul
//===========================================================================
static void flexcan_start_chip(can_channel *chan)
{
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    
    cyg_uint16 tmp16;
    
    info->state = CYGNUM_CAN_STATE_ACTIVE;
    HAL_READ_UINT16(&flexcan->CANMCR, tmp16);    
    HAL_WRITE_UINT16(&flexcan->CANMCR, tmp16 
                 & ~(FLEXCAN_MCR_FRZ | FLEXCAN_MCR_HALT));
}


//===========================================================================
// Stop FlexCAN modul
//===========================================================================
static void flexcan_stop_chip(can_channel *chan)
{
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    cyg_uint16    tmp16;
    
    info->state = CYGNUM_CAN_STATE_STOPPED;
    HAL_READ_UINT16(&flexcan->CANMCR, tmp16);    
    HAL_WRITE_UINT16(&flexcan->CANMCR, tmp16 | FLEXCAN_MCR_HALT);
}


//===========================================================================
// Set FlexCAN modul into standby mode
// If the flag selfwake is active then the FlexCAN modul will be set into
// standby mode with selwake. This means the FlexCAN modul will leave
// standby as soon as a message box will receive a message
//===========================================================================
static void flexcan_enter_standby(can_channel *chan, bool selfwake)
{
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    cyg_uint16    tmp16;
    cyg_uint8     tmp8;
    cyg_uint8     i;
    
    //
    // The CPU should disable all interrupts in the FlexCAN before entering low-power
    // stop mode. Otherwise it may be interrupted while in STOP mode upon a non
    // wake-up condition; If desired, the WAKEMASK bit should be set to enable the
    // WAKEINT.
    //
    HAL_READ_UINT8(&flexcan->CANCTRL0, tmp8);
    HAL_WRITE_UINT8(&flexcan->CANCTRL0, tmp8 & ~(FLEXCAN_CTRL0_BOFFMSK | FLEXCAN_CTRL0_ERRMASK));
    
    //
    // We disable all message box interrupts. The WAKE DSR will reenable it later
    // after processing the WAKE event. This ensures that the wake event will be the
    // first event if a message arrives
    //
    HAL_WRITE_UINT16(&flexcan->IMASK, 0); 
    
    HAL_READ_UINT16(&flexcan->CANMCR, tmp16);  
    
    tmp16 |= FLEXCAN_MCR_STOP;
    
    //
    // if we should go to standby then we activate the SELWAKE bit so that a received
    // frame will bring us back to live
    //
    if (selfwake)
    {
        tmp16 |= FLEXCAN_MCR_SELFWAKE;
    }
      
    HAL_WRITE_UINT16(&flexcan->CANMCR, tmp16);
    
    //
    // we have to poll the STOPACK bit in order to determine if chip
    // has entered stop mode. We poll 10 times - se we spent a maximum
    // of 2 ms here
    //
    for (i = 0; i < 10; ++i)
    {
        HAL_READ_UINT16(&flexcan->CANMCR, tmp16);
        
        if (tmp16 & FLEXCAN_MCR_STOPACK)
        {
            info->state = CYGNUM_CAN_STATE_STANDBY;
            break;
        }
        HAL_DELAY_US(200);
    }
    
    //
    // if we are not in low power stop mode then we have to reenable interrupts
    //
    if (10 == i)
    {
        HAL_READ_UINT8(&flexcan->CANCTRL0, tmp8);
        HAL_WRITE_UINT8(&flexcan->CANCTRL0, tmp8 | (FLEXCAN_CTRL0_BOFFMSK | FLEXCAN_CTRL0_ERRMASK));
    }
    else
    {
        //
        // if we go into standby then we activate the wake interrupt so we will receive
        // a wake interrupt if we leave standby and can reenable interrups
        //
        if (selfwake)
        {
            HAL_READ_UINT16(&flexcan->CANMCR, tmp16);  
            HAL_WRITE_UINT16(&flexcan->CANMCR, tmp16 | FLEXCAN_MCR_WAKEMSK);  
        }
    }
}


//===========================================================================
// Leave standby mode
//===========================================================================
static void flexcan_leave_standby(can_channel *chan)
{
    flexcan_info *info    = (flexcan_info *)chan->dev_priv;
    flexcan_regs *flexcan = (flexcan_regs *)info->base;
    cyg_uint16    tmp16;
    cyg_uint8     i;
    
    HAL_READ_UINT16(&flexcan->CANMCR, tmp16);  
    HAL_WRITE_UINT16(&flexcan->CANMCR, tmp16 & ~(FLEXCAN_MCR_STOP | FLEXCAN_MCR_SELFWAKE));
    
    //
    // we have to poll the STOPACK bit in order to determine if chip
    // has leaved stop mode. We poll 10 times - se we spent a maximum
    // of 2 ms here
    //
    for (i = 0; i < 10; ++i)
    {
        HAL_READ_UINT16(&flexcan->CANMCR, tmp16);
        
        if (!(tmp16 & FLEXCAN_MCR_STOPACK))
        {
            HAL_WRITE_UINT16(&flexcan->IMASK, info->imask_shadow);
            info->state = CYGNUM_CAN_STATE_ACTIVE;
            break;    
        }
        HAL_DELAY_US(200);
    } // for (i = 0; i < 10; ++i)
}


//===========================================================================
// Set acceptance mask for message buffer
//===========================================================================
static void flexcan_set_acceptance_mask(cyg_uint16 *rxmask_reg, cyg_uint32 mask, cyg_can_id_type ext)
{
    cyg_uint16 id;
    //
    // 32 bit access to RXMASK filters is broken so we use  16 Bit
    // access here
    //
    if (CYGNUM_CAN_ID_EXT == ext)
    {
        id  = ((mask >> 13) & 0xFFE0);       // set mask bits 18 - 28
        id |= ((mask >> 15) & 0x7);          // set mask bits 15 -17
        HAL_WRITE_UINT16(&rxmask_reg[0], id);
        
        id = (mask << 1) & 0xFFFE;
        HAL_WRITE_UINT16(&rxmask_reg[1], id);
    }
    else // (CYGNUM_CAN_ID_STD == ext)
    {
        id = ((mask << 5) & 0xFFE0); 
        HAL_WRITE_UINT16(&rxmask_reg[0], id);
        HAL_WRITE_UINT16(&rxmask_reg[1], 0xFFFF);  
    }
    
}



//===========================================================================
// Configure message box for transmission
//===========================================================================
static bool flexcan_cfg_mbox_tx(flexcan_mbox     *pmbox,
                                cyg_can_message  *pmsg,
                                bool              rtr)
{
    cyg_uint16 id;
  
    HAL_WRITE_UINT8(&pmbox->ctrlstat, MBOX_TXCODE_NOT_READY); 
    
    if (CYGNUM_CAN_ID_EXT == pmsg->ext)
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
// Configure message box for reception of a certain CAN identifier
//===========================================================================
static void flexcan_cfg_mbox_rx(flexcan_mbox     *pmbox,
                                cyg_can_message  *pmsg,
                                bool              enable)
{
    cyg_uint16 id;
    
    HAL_WRITE_UINT8(&pmbox->ctrlstat, MBOX_RXCODE_NOT_ACTIVE); 
    
    if (CYGNUM_CAN_ID_EXT == pmsg->ext)
    { 
        id  = ((pmsg->id >> 13) & 0xFFE0);      // setup id bits 18 - 28
        id |= (MBOX_CFG_IDE | MBOX_CFG_SSR);    // set SSR and IDE bit to 1
        id |= ((pmsg->id >> 15) & 0x7);         // set id bits 15 - 17
        HAL_WRITE_UINT16(&pmbox->id_hi, id);    // write ID high
        
        id = ((pmsg->id << 1) & 0xFFFE);
        
        HAL_WRITE_UINT16(&pmbox->id_lo, id);// write ID low
    }   
    else
    {
        id = ((pmsg->id << 5) & 0xFFE0);

        HAL_WRITE_UINT16(&pmbox->id_hi, id);
        HAL_WRITE_UINT16(&pmbox->id_lo, 0);
    }
    
    if (enable)
    {
    HAL_WRITE_UINT8(&pmbox->ctrlstat, MBOX_RXCODE_EMPTY);
    }
}


//===========================================================================
// Read date from a message box
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
    bool             enable_mbox = false;
    
    //
    // If controlstat was not read, then read it now
    //
    if (FLEXCAN_CTRLSTAT_NOT_READ == *ctrlstat)
    {
    HAL_READ_UINT8(&pmbox->ctrlstat, *ctrlstat); // this read will lock the mbox
        enable_mbox = true;
    }
    
    //
    // If message buffer is busy then it is now beeing filled with a new message
    // This condition will be cleared within 20 cycles - we simply do a 20 µs
    // delay here, that should be enougth
    //
    if (*ctrlstat & MBOX_RXCODE_BUSY)
    {
        HAL_DELAY_US(20);
    }
    
    pmsg->dlc = (*ctrlstat & MBOX_CFG_DLC_MASK); // store received data len
    
    HAL_READ_UINT16(&pmbox->id_hi, id);          // read ID high
    
    if (id & MBOX_CFG_IDE)
    {
        pmsg->ext = CYGNUM_CAN_ID_EXT;
        pmsg->id  = (id & 0xFFE0) << 13;
        pmsg->id |= (id & 0x07) << 15;
        
        HAL_READ_UINT16(&pmbox->id_lo, id);
        pmsg->id |= (id & 0xFFFE) >> 1;
        
        if (id & MBOX_CFG_RTR_EXT)
        {
            pmsg->rtr = CYGNUM_CAN_FRAME_RTR;
        }
        else
        {
            pmsg->rtr = CYGNUM_CAN_FRAME_DATA;
        }
    }
    else
    {
        pmsg->ext = CYGNUM_CAN_ID_STD;
        pmsg->id  = (id & 0xFFE0) >> 5;
        
        if (id & MBOX_CFG_RTR_STD)
        {
            pmsg->rtr = CYGNUM_CAN_FRAME_RTR;
        }
        else
        {
            pmsg->rtr = CYGNUM_CAN_FRAME_DATA;
        }
    }
    
    //
    // now finally copy data
    //
    HAL_READ_UINT8_VECTOR(&pmbox->data, pmsg->data, pmsg->dlc, 1);
       
    //
    // now mark this mbox as empty and read the free running timer
    // to unlock this mbox
    //
    if (enable_mbox)
    {
    HAL_WRITE_UINT8(&pmbox->ctrlstat, MBOX_RXCODE_EMPTY);
    HAL_READ_UINT16(&flexcan->TIMER, id);
    }
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

