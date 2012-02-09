//==========================================================================
//
//      devs/can/arm/lpc2xxx/current/src/can_lpc2xxx.c
//
//      CAN driver for LPC2xxx microcontrollers
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Uwe Kindler
// Contributors: Uwe Kindler
// Date:         2007-04-09
// Purpose:      Support LPC2xxx on-chip CAN moduls
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
#include <pkgconf/io.h>
#include <pkgconf/devs_can_lpc2xxx.h>

#include <cyg/infra/diag.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_io.h>

#include <cyg/hal/hal_diag.h>
#include <cyg/infra/cyg_ass.h>

#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/can.h>
#include <cyg/io/can_lpc2xxx_baudrates.h>




//===========================================================================
//                                DEFINES  
//===========================================================================
//
// Check if the macro HAL_LPC2XXX_GET_CAN_BR is provided
//
#ifndef HAL_LPC2XXX_GET_CAN_BR
#error "Macro HAL_LPC2XXX_GET_CAN_BR() missing"
#endif

//
// Support debug output if this option is enabled in CDL file
//
#ifdef CYGDBG_DEVS_CAN_LPC2XXX_DEBUG
#define LPC2XXX_DBG_PRINT diag_printf
#else
#define LPC2XXX_DBG_PRINT( fmt, ... )
#endif


//---------------------------------------------------------------------------
// we define our own set of register bits in order to be independent from
// platform specific names
//

//---------------------------------------------------------------------------
// Memory map of CAN block
//
#define CAN_ACCFILT_RAM_BASE  0xE0038000
#define CAN_ACCFILT_REG_BASE  0xE003C000
#define CAN_CENTRAL_REG_BASE  0xE0040000
#define CAN_CTRL_1_REG_BASE   0xE0044000
#define CAN_CTRL_2_REG_BASE   0xE0048000
#define CAN_CTRL_3_REG_BASE   0xE004C000
#define CAN_CTRL_4_REG_BASE   0xE0050000


//---------------------------------------------------------------------------
// CAN Acceptance Filter register layout
//
#define CAN_ACCFILT_AFMR            (CAN_ACCFILT_REG_BASE  + 0x0000)
#define CAN_ACCFILT_SFF_SA          (CAN_ACCFILT_REG_BASE  + 0x0004)
#define CAN_ACCFILT_SFF_GRP_SA      (CAN_ACCFILT_REG_BASE  + 0x0008)
#define CAN_ACCFILT_EFF_SA          (CAN_ACCFILT_REG_BASE  + 0x000C)
#define CAN_ACCFILT_EFF_GRP_SA      (CAN_ACCFILT_REG_BASE  + 0x0010)
#define CAN_ACCFILT_ENDOFTABLE      (CAN_ACCFILT_REG_BASE  + 0x0014)
#define CAN_ACCFILT_LUT_ERR_ADDR    (CAN_ACCFILT_REG_BASE  + 0x0018)
#define CAN_ACCFILT_LUT_ERR         (CAN_ACCFILT_REG_BASE  + 0x001C)

//---------------------------------------------------------------------------
// CAN_ACCFILT_AFMR Bits
//
#define AFMR_OFF       0x00000001 // 1 = Acceptance filter is not operational
#define AFMR_BYPASS    0x00000002 // 1 = all Rx messages are accepted on enabled CAN controllers.
#define AFMR_FULLCAN   0x00000004 // 1 = FullCAN mode
#define AFMR_ON        0x00000000 // Acceptance filter on
#define ACCFILT_RAM_SIZE 2048     // size of acceptance filter ram


//---------------------------------------------------------------------------
// Acceptance filter tool macros
//
#define ACCFILT_STD_ID_MASK                    0x7FF
#define ACCFILT_EXT_ID_MASK                    0x1FFFFFFF
#define ACCFILT_STD_DIS                        0x1000
#define ACCFILT_STD_CTRL_MASK                  0xE000
#define ACCFILT_EXT_CTRL_MASK                  0xE0000000
#define ACCFILT_STD_GET_CTRL(_entry_)          (((_entry_) >> 13) & 0x7)
#define ACCFILT_STD_GET_CTRL_LOWER(_entry_)    (((_entry_) >> 29) & 0x7)
#define ACCFILT_STD_GET_CTRL_UPPER(_entry_)    (((_entry_) >> 13) & 0x7)
#define ACCFILT_STD_GET_ID(_entry_)            ((_entry_) & ACCFILT_STD_ID_MASK)
#define ACCFILT_EXT_GET_ID(_entry_)            ((_entry_) & ACCFILT_EXT_ID_MASK)
#define ACCFILT_EXT_GET_CTRL(_entry_)          (((_entry_) >> 29) & 0x7)
#define ACCFILT_EXT_SET_CTRL(_entry_, _ctrl_)  ((_entry_ & 0xE0000000) | ((_ctrl_) << 29)) 


//---------------------------------------------------------------------------
// CAN Central CAN Registers register layout
//
#define CAN_CENTRAL_TXSR            (CAN_CENTRAL_REG_BASE  + 0x0000)
#define CAN_CENTRAL_RXSR            (CAN_CENTRAL_REG_BASE  + 0x0004)
#define CAN_CENTRAL_MSR             (CAN_CENTRAL_REG_BASE  + 0x0008)


//---------------------------------------------------------------------------
// CAN Controller register offsets
// Registers are offsets from base CAN module control register
//
#define CANREG_MOD   0x0000
#define CANREG_CMR   0x0004
#define CANREG_GSR   0x0008
#define CANREG_ICR   0x000C
#define CANREG_IER   0x0010
#define CANREG_BTR   0x0014
#define CANREG_EWL   0x0018
#define CANREG_SR    0x001C
#define CANREG_RFS   0x0020
#define CANREG_RID   0x0024
#define CANREG_RDA   0x0028
#define CANREG_RDB   0x002C
#define CANREG_TFI1  0x0030
#define CANREG_TID1  0x0034
#define CANREG_TDA1  0x0038
#define CANREG_TDB1  0x003C
#define CANREG_TFI2  0x0040
#define CANREG_TID2  0x0044
#define CANREG_TDA2  0x0048
#define CANREG_TDB2  0x004C
#define CANREG_TFI3  0x0050
#define CANREG_TID3  0x0054
#define CANREG_TDA3  0x0058
#define CANREG_TDB3  0x005C


//---------------------------------------------------------------------------
// CAN Controller register layout
//
#define CAN_CTRL_MOD(_extra_)   (CAN_CTRL_BASE(_extra_) + CANREG_MOD)
#define CAN_CTRL_CMR(_extra_)   (CAN_CTRL_BASE(_extra_) + CANREG_CMR)
#define CAN_CTRL_GSR(_extra_)   (CAN_CTRL_BASE(_extra_) + CANREG_GSR)
#define CAN_CTRL_ICR(_extra_)   (CAN_CTRL_BASE(_extra_) + CANREG_ICR)
#define CAN_CTRL_IER(_extra_)   (CAN_CTRL_BASE(_extra_) + CANREG_IER)
#define CAN_CTRL_BTR(_extra_)   (CAN_CTRL_BASE(_extra_) + CANREG_BTR)
#define CAN_CTRL_EWL(_extra_)   (CAN_CTRL_BASE(_extra_) + CANREG_EWL)
#define CAN_CTRL_SR(_extra_)    (CAN_CTRL_BASE(_extra_) + CANREG_SR)
#define CAN_CTRL_RFS(_extra_)   (CAN_CTRL_BASE(_extra_) + CANREG_RFS)
#define CAN_CTRL_RID(_extra_)   (CAN_CTRL_BASE(_extra_) + CANREG_RID)
#define CAN_CTRL_RDA(_extra_)   (CAN_CTRL_BASE(_extra_) + CANREG_RDA)
#define CAN_CTRL_RDB(_extra_)   (CAN_CTRL_BASE(_extra_) + CANREG_RDB)
#define CAN_CTRL_TFI1(_extra_)  (CAN_CTRL_BASE(_extra_) + CANREG_TFI1)
#define CAN_CTRL_TID1(_extra_)  (CAN_CTRL_BASE(_extra_) + CANREG_TID1)
#define CAN_CTRL_TDA1(_extra_)  (CAN_CTRL_BASE(_extra_) + CANREG_TDA1)
#define CAN_CTRL_TDB1(_extra_)  (CAN_CTRL_BASE(_extra_) + CANREG_TDB1)
#define CAN_CTRL_TFI2(_extra_)  (CAN_CTRL_BASE(_extra_) + CANREG_TFI2)
#define CAN_CTRL_TID2(_extra_)  (CAN_CTRL_BASE(_extra_) + CANREG_TID2)
#define CAN_CTRL_TDA2(_extra_)  (CAN_CTRL_BASE(_extra_) + CANREG_TDA2)
#define CAN_CTRL_TDB2(_extra_)  (CAN_CTRL_BASE(_extra_) + CANREG_TDB2)
#define CAN_CTRL_TFI3(_extra_)  (CAN_CTRL_BASE(_extra_) + CANREG_TFI3)
#define CAN_CTRL_TID3(_extra_)  (CAN_CTRL_BASE(_extra_) + CANREG_TID3)
#define CAN_CTRL_TDA3(_extra_)  (CAN_CTRL_BASE(_extra_) + CANREG_TDA3)
#define CAN_CTRL_TDB3(_extra_)  (CAN_CTRL_BASE(_extra_) + CANREG_TDB3)


//---------------------------------------------------------------------------
// CAN_CTRL_ICR register bits
//
#define ICR_RX                  0x00000001
#define ICR_TX1                 0x00000002
#define ICR_ERR_WARN            0x00000004
#define ICR_DATA_OVR            0x00000008
#define ICR_WAKE_UP             0x00000010
#define ICR_ERR_PASSIVE         0x00000020
#define ICR_ARBITR_LOST         0x00000040
#define ICR_BUS_ERR             0x00000080
#define ICR_ID_READY            0x00000100
#define ICR_TX2                 0x00000200
#define ICR_TX3                 0x00000400
#define ICR_LUT_ERR             0x00000800
#define ICR_GET_ERRBIT(_icr_)   (((_icr_) >> 16) & 0x1F)
#define ICR_ERR_DIRECTION       0x00200000
#define ICR_GET_ERRCODE(_icr_)  (((_icr_) >> 22) & 0x03)
#define ICR_GET_ALCBIT(_icr_)   (((_icr_) >> 24) & 0x1F)

#ifdef CYGOPT_DEVS_CAN_LPC2XXX_ALIE
#define CAN_ALL_ERR_INT (ICR_ERR_PASSIVE | ICR_ARBITR_LOST | ICR_BUS_ERR | ICR_ERR_WARN)
#else
#define CAN_ALL_ERR_INT (ICR_ERR_PASSIVE | ICR_BUS_ERR | ICR_ERR_WARN)
#endif
#define CAN_MISC_INT    (CAN_ALL_ERR_INT | ICR_WAKE_UP)


//---------------------------------------------------------------------------
// CAN_CTRL_ICR register bits
//
#define ICR_ERRCODE_BIT_ERR   0x00
#define ICR_ERRCODE_FORM_ERR  0x01
#define ICR_ERRCODE_STUFF_ERR 0x02
#define ICR_ERRCODE_OTHER_ERR 0x03


//---------------------------------------------------------------------------
// CAN_CTRL_RFS register bits
//
#define RFS_ACCFILT_INDEX_MASK       0x000003FF
#define RFS_RECEIVED_IN_BYPASS_MODE  0x00000400
#define RFS_DLC_MASK                 0x000F0000
#define RFS_RTR                      0x40000000
#define RFS_EXT                      0x80000000
#define RFS_GET_DLC(_regval_)        (((_regval_) >> 16) & 0xF)

//---------------------------------------------------------------------------
// CAN_CTRL_CMR register bits
//
#define CMR_TX_REQ          0x00000001
#define CMR_TX_ABORT        0x00000002
#define CMR_RX_RELEASE_BUF  0x00000004
#define CMR_CLEAR_DATA_OVR  0x00000008
#define CMR_SELF_RX_REQ     0x00000010
#define CMR_SEND_TX_BUF1    0x00000020
#define CMR_SEND_TX_BUF2    0x00000040
#define CMR_SEND_TX_BUF3    0x00000080


//---------------------------------------------------------------------------
// CAN_CTRL_TFI register bits
//
#define TFI_PRIO_MASK 0x000000FF
#define TFI_DLC_MASK  0x000F0000
#define TFI_DLC_RTR   0x40000000
#define TFI_DLC_EXT   0x80000000


//---------------------------------------------------------------------------
// CAN_CTRL_MOD register bits
//
#define CANMOD_OPERATIONAL    0x00000000
#define CANMOD_RESET          0x00000001
#define CANMOD_LISTEN_ONLY    0x00000002
#define CANMOD_SELF_TEST      0x00000004
#define CANMOD_TX_BUF_CFG     0x00000008
#define CANMOD_SLEEP          0x00000010
#define CANMOD_REV_POLARITY   0x00000020
#define CANMOD_TEST           0x00000040


//---------------------------------------------------------------------------
// CAN_CTRL_IER register bits
//
#define IER_RX                0x00000001
#define IER_TX1               0x00000002
#define IER_ERR_WARN          0x00000004
#define IER_DATA_OVR          0x00000008
#define IER_WAKE_UP           0x00000010
#define IER_ERR_PASSIVE       0x00000020
#define IER_ARBITR_LOST       0x00000040
#define IER_BUS_ERR           0x00000080
#define IER_ID_READY          0x00000100
#define IER_TX2               0x00000200
#define IER_TX3               0x00000400


//---------------------------------------------------------------------------
// CAN_CTRL_GSR register bits
//
#define GSR_RX_MSG_AVAILABLE  0x00000001
#define GSR_DATA_OVR          0x00000002
#define GSR_TX_NOT_PENDING    0x00000004
#define GSR_ALL_TX_COMPLETE   0x00000008
#define GSR_RECEIVING_ACTIVE  0x00000010
#define GSR_SENDING_ACTIVE    0x00000020
#define GSR_ERR               0x00000040
#define GSR_BUS_OFF           0x00000080
#define GSR_RXERR_CNT(_reg_)  (((_reg_) >> 16) & 0xFF)
#define GSR_TXERR_CNT(_reg_)  (((_reg_) >> 24) & 0xFF)


//---------------------------------------------------------------------------
// CAN_CTRL_SR register bits
//
#define SR_RX_MSG_AVAILABLE  0x01 
#define SR_DATA_OVR          0x02
#define SR_TX_BUF_WRITE_OK   0x04 // TBS1, TBS2, TBS3 (Bit 2, 10, 18)
#define SR_TX_COMPLETE       0x08 // TCS1, TCS2, TCS3 (Bit 3, 11, 19)
#define SR_RECEIVING_ACTIVE  0x10
#define SR_SENDING_ACTIVE    0x20 // TS1, TS2, TS3 (5, 13, 21)
#define SR_ERR               0x40
#define SR_BUS_OFF           0x80


//---------------------------------------------------------------------------
// Optimize for the case of a single CAN channel, while still allowing
// multiple channels.
//
#if CYGINT_IO_CAN_CHANNELS == 1
#define CAN_CTRL_BASE(_extra_)   CAN_CTRL_SINGLETON_BASE
#define CAN_ISRVEC(_extra_)      CAN_SINGLETON_ISRVEC
#define CAN_CHAN_NO(_extra_)     CAN_SINGLETON_CHAN_NO
#define CAN_DECLARE_INFO(_chan_)
#define CAN_DECLARE_CHAN(_data_)
#else
#define CAN_CTRL_BASE(_extra_)   ((_extra_)->base)
#define CAN_ISRVEC(_extra_)      ((_extra_)->isrvec)
#define CAN_CHAN_NO(_extra_)     ((_extra_)->chan_no)
#define CAN_DECLARE_INFO(_chan_) lpc2xxx_can_info_t *info = (lpc2xxx_can_info_t *)chan->dev_priv;
#define CAN_DECLARE_CHAN(_data_) can_channel  *chan = (can_channel *)data;
#endif // CYGINT_IO_CAN_CHANNELS == 1 


#ifdef CYGOPT_DEVS_CAN_LPC2XXX_CAN0_ACCFILT_STARTUP_CFG_RX_ALL
#define CAN0_FLAG_STARTUP_ACCFILT_SETUP INFO_FLAG_STARTUP_RX_ALL
#else
#define CAN0_FLAG_STARTUP_ACCFILT_SETUP 0x00
#endif

#ifdef CYGOPT_DEVS_CAN_LPC2XXX_CAN1_ACCFILT_STARTUP_CFG_RX_ALL
#define CAN1_FLAG_STARTUP_ACCFILT_SETUP INFO_FLAG_STARTUP_RX_ALL
#else
#define CAN1_FLAG_STARTUP_ACCFILT_SETUP 0x00
#endif

#ifdef CYGOPT_DEVS_CAN_LPC2XXX_CAN2_ACCFILT_STARTUP_CFG_RX_ALL
#define CAN2_FLAG_STARTUP_ACCFILT_SETUP INFO_FLAG_STARTUP_RX_ALL
#else
#define CAN2_FLAG_STARTUP_ACCFILT_SETUP 0x00
#endif

#ifdef CYGOPT_DEVS_CAN_LPC2XXX_CAN3_ACCFILT_STARTUP_CFG_RX_ALL
#define CAN3_FLAG_STARTUP_ACCFILT_SETUP INFO_FLAG_STARTUP_RX_ALL
#else
#define CAN3_FLAG_STARTUP_ACCFILT_SETUP 0x00
#endif


//===========================================================================
//                              DATA TYPES
//===========================================================================
//
// Structure stores LPC2xxx CAN channel related stuff
//

// If we use Self Reception Request command instead of the Transmission Request
// we must add last transmit message id in order to reject it in rx_ISR
// There are two last_tx_id because tx interrupt (and so transmission of next 
// message) happens before rx interrupt (which uses last_tx_id for rejecting)) 

// Format of last_tx_id:
//  (bits: 28:0-ID, 29-Validation, 30-RTR, 31-EXT)
//  if last_tx_id == 0xFFFFFFFF (Validation == 1) then last id is not valid
#ifdef CYGOPT_DEVS_CAN_LPC2XXX_USE_SELF_RECEPTION
 #define LPC2XXX_CAN_INFO_LAST_TX_ID_IDMASK   0x1FFFFFFF
 #define LPC2XXX_CAN_INFO_LAST_TX_ID_FLMASK   0xC0000000
 #define LPC2XXX_CAN_INFO_LAST_TX_ID_NOVALID  0xFFFFFFFF

 #define LPC2XXX_CAN_INFO_LAST_TX_ID_DECL     cyg_uint8    last_tx_index;                 \
                                              cyg_uint32   last_tx_id[2];
 #define LPC2XXX_CAN_INFO_LAST_TX_ID_INIT     last_tx_index : 0,                          \
                                              last_tx_id    : {LPC2XXX_CAN_INFO_LAST_TX_ID_NOVALID, LPC2XXX_CAN_INFO_LAST_TX_ID_NOVALID},
#else
 #define LPC2XXX_CAN_INFO_LAST_TX_ID_DECL
 #define LPC2XXX_CAN_INFO_LAST_TX_ID_INIT
#endif

typedef struct lpc2xxx_can_info_st
{
//
// Newer LPC2xxx variants like the LPC2468 do not support per channel 
// interrupts. They provide only one single interrupt vector for all
// CAN interrupts
//
#ifndef CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY
    cyg_interrupt      tx_interrupt;
    cyg_handle_t       tx_interrupt_handle;     
    cyg_uint8          tx_interrupt_priority;     
    cyg_interrupt      rx_interrupt;
    cyg_handle_t       rx_interrupt_handle; 
    cyg_uint8          rx_interrupt_priority;
#endif // CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY
    cyg_can_state      state;            // state of CAN controller 
    cyg_uint8          flags;            // flags indicating several states       
    LPC2XXX_CAN_INFO_LAST_TX_ID_DECL     // last transmitted messages ids   
#if CYGINT_IO_CAN_CHANNELS > 1
    cyg_uint32         base;             // Per-bus h/w details
#ifndef CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY
    cyg_uint8          isrvec;           // ISR vector (peripheral id)
#endif // CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY
    cyg_uint8          chan_no;          // number of CAN channel
#endif // CYGINT_IO_CAN_CHANNELS > 1
} lpc2xxx_can_info_t;


#define INFO_FLAG_RX_ALL           0x01 // this bit indicates that channel receives all CAN messages - no filtering active
#define INFO_FLAG_STARTUP_RX_ALL   0x02 // this bit indicates filter state at startup


//
// lpc2xxx info initialisation
//
#define LPC2XXX_CTRL_NOT_INITIALIZED 0xFF

#ifndef CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY
#if CYGINT_IO_CAN_CHANNELS > 1
#define LPC2XXX_CAN_INFO(_l, _base, _isrvec, _chan_no_, _tx_priority, _rx_priority, _flags) \
lpc2xxx_can_info_t _l  = {                                                                  \
    state             :  LPC2XXX_CTRL_NOT_INITIALIZED,                                      \
    base              : (_base),                                                            \
    isrvec            : (_isrvec),                                                          \
    chan_no           : (_chan_no_),                                                        \
    tx_interrupt_priority : (_tx_priority),                                                 \
    rx_interrupt_priority : (_rx_priority),                                                 \
    flags             : (_flags),                                                           \
    LPC2XXX_CAN_INFO_LAST_TX_ID_INIT                                                        \
};
#else // CYGINT_IO_CAN_CHANNELS == 1
#define LPC2XXX_CAN_INFO(_l, _tx_priority, _rx_priority, _flags) \
lpc2xxx_can_info_t _l = {                          \
    state      : CYGNUM_CAN_STATE_STOPPED,         \
    tx_interrupt_priority : (_tx_priority),        \
    rx_interrupt_priority : (_rx_priority),        \
    flags      : (_flags),                         \
    LPC2XXX_CAN_INFO_LAST_TX_ID_INIT               \
};
#endif // CYGINT_IO_CAN_CHANNELS == 1
#else // CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY
//
// Newer devices support only one global CAN interrupt. We do not need
// per channel interrupt data an ignore the values during initialisation
//
#if CYGINT_IO_CAN_CHANNELS > 1
#define LPC2XXX_CAN_INFO(_l, _base, _isrvec, _chan_no_, _tx_priority, _rx_priority, _flags) \
lpc2xxx_can_info_t _l  = {                                                       \
    state             :  LPC2XXX_CTRL_NOT_INITIALIZED,                           \
    base              : (_base),                                                 \
    chan_no           : (_chan_no_),                                             \
    flags             : (_flags),                                                \
    LPC2XXX_CAN_INFO_LAST_TX_ID_INIT                                             \
};
#else // CYGINT_IO_CAN_CHANNELS == 1
#define LPC2XXX_CAN_INFO(_l, _tx_priority, _rx_priority, _flags) \
lpc2xxx_can_info_t _l = {                          \
    state      : CYGNUM_CAN_STATE_STOPPED,         \
    flags      : (_flags),                         \
    LPC2XXX_CAN_INFO_LAST_TX_ID_INIT               \
};
#endif // CYGINT_IO_CAN_CHANNELS == 1

//
// The following defines are only dummies required for proper 
// initialisation of can channel data structures
//
#define CYGNUM_HAL_INTERRUPT_CAN1_TX
#define CYGNUM_DEVS_CAN_LPC2XXX_CAN0_TX_INT_PRIORITY
#define CYGNUM_DEVS_CAN_LPC2XXX_CAN0_RX_INT_PRIORITY
#define CYGNUM_HAL_INTERRUPT_CAN2_TX
#define CYGNUM_DEVS_CAN_LPC2XXX_CAN1_TX_INT_PRIORITY
#define CYGNUM_DEVS_CAN_LPC2XXX_CAN1_RX_INT_PRIORITY
#define CYGNUM_HAL_INTERRUPT_CAN3_TX
#define CYGNUM_DEVS_CAN_LPC2XXX_CAN2_TX_INT_PRIORITY
#define CYGNUM_DEVS_CAN_LPC2XXX_CAN2_RX_INT_PRIORITY
#define CYGNUM_HAL_INTERRUPT_CAN4_TX
#define CYGNUM_DEVS_CAN_LPC2XXX_CAN3_TX_INT_PRIORITY
#define CYGNUM_DEVS_CAN_LPC2XXX_CAN3_RX_INT_PRIORITY
#endif // CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY


//
// Acceptance filter data
//
typedef struct lpc2xxx_global_can_info_st
{
    cyg_interrupt       interrupt;          // common CAN interrupt
    cyg_handle_t        interrupt_handle;   // common CAN interrupt handle 
    cyg_uint16          free_filters;       // number of free message filter
#if CYGINT_IO_CAN_CHANNELS > 1              // optimize for single channel
    cyg_uint8           init_cnt;           // counts number of initialized channels
    can_channel*        active_channels[5]; // stores pointers to active channels - the last entry is just a delimiter
#else // CYGINT_IO_CAN_CHANNELS > 1
    can_channel*        active_channels[1]; // optimize for one single channel
#endif // CYGINT_IO_CAN_CHANNELS > 1
} lpc2xxx_global_can_info_t;


#if CYGINT_IO_CAN_CHANNELS > 1
#define LPC2XXX_GET_CAN_CHANNEL(_can_info_, _chan_no_) ((can_channel*)(_can_info_).active_channels[_chan_no_])
#else
#define LPC2XXX_GET_CAN_CHANNEL(_can_info_, _chan_no_) ((can_channel*)(_can_info_).active_channels[0])
#endif

//
// The number of available message filters depends on the size of the
// acceptance filter RAM and on the size of one entry. The size of
// one entry is 4 byte (standard ID only 2 byte, extended groups 8 byte)
//
#define ACCFILT_COMMON_ENTRY_SIZE 4
#define LPC2XXX_CAN_MSG_FILTERS_MAX (ACCFILT_RAM_SIZE / ACCFILT_COMMON_ENTRY_SIZE)
lpc2xxx_global_can_info_t lpc2xxx_global_can_info =
{
    .free_filters     = LPC2XXX_CAN_MSG_FILTERS_MAX,
#if CYGINT_IO_CAN_CHANNELS > 1 // optimize for single channel
    .init_cnt         = 0,
    .active_channels  = {0, 0, 0, 0, 0},
#endif // #if CYGINT_IO_CAN_CHANNELS > 1
};



//
// Data type for access of single bytes/words of an dword value
//
typedef union lsc_buf_u
{
    cyg_uint8  bytes[4];
    struct 
    {
        cyg_uint16 low;
        cyg_uint16 high;
    } words;
    
    struct
    {
        cyg_uint16 upper; // uppper column of acceptance filter ram
        cyg_uint16 lower; // lower column of acceptance filter ram
    } column;
    
    cyg_uint32 dword;
} lsc_buf_t;


//===========================================================================
//                          GLOBAL DATA
//===========================================================================
#if CYGINT_IO_CAN_CHANNELS > 1
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN0
LPC2XXX_CAN_INFO(lpc2xxx_can0_info,
                 CAN_CTRL_1_REG_BASE,
                 CYGNUM_HAL_INTERRUPT_CAN1_TX,
                 0,
                 CYGNUM_DEVS_CAN_LPC2XXX_CAN0_TX_INT_PRIORITY,
                 CYGNUM_DEVS_CAN_LPC2XXX_CAN0_RX_INT_PRIORITY,
                 CAN0_FLAG_STARTUP_ACCFILT_SETUP);
#endif

#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN1
LPC2XXX_CAN_INFO(lpc2xxx_can1_info, 
                 CAN_CTRL_2_REG_BASE, 
                 CYGNUM_HAL_INTERRUPT_CAN2_TX,
                 1,
                 CYGNUM_DEVS_CAN_LPC2XXX_CAN1_TX_INT_PRIORITY,
                 CYGNUM_DEVS_CAN_LPC2XXX_CAN1_RX_INT_PRIORITY,
                 CAN1_FLAG_STARTUP_ACCFILT_SETUP);
#endif

#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN2
LPC2XXX_CAN_INFO(lpc2xxx_can2_info, 
                 CAN_CTRL_3_REG_BASE, 
                 CYGNUM_HAL_INTERRUPT_CAN3_TX,
                 2,
                 CYGNUM_DEVS_CAN_LPC2XXX_CAN2_TX_INT_PRIORITY,
                 CYGNUM_DEVS_CAN_LPC2XXX_CAN2_RX_INT_PRIORITY,
                 CAN2_FLAG_STARTUP_ACCFILT_SETUP);
#endif

#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN3
LPC2XXX_CAN_INFO(lpc2xxx_can3_info, 
                 CAN_CTRL_4_REG_BASE, 
                 CYGNUM_HAL_INTERRUPT_CAN4_TX,
                 3,
                 CYGNUM_DEVS_CAN_LPC2XXX_CAN3_TX_INT_PRIORITY,
                 CYGNUM_DEVS_CAN_LPC2XXX_CAN3_RX_INT_PRIORITY,
                 CAN3_FLAG_STARTUP_ACCFILT_SETUP);
#endif
#else // CYGINT_IO_CAN_CHANNELS == 1
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN0
LPC2XXX_CAN_INFO(lpc2xxx_can0_info, 
                 CYGNUM_DEVS_CAN_LPC2XXX_CAN0_TX_INT_PRIORITY, 
                 CYGNUM_DEVS_CAN_LPC2XXX_CAN0_RX_INT_PRIORITY, 
                 CAN0_FLAG_STARTUP_ACCFILT_SETUP);
#define CAN_CTRL_SINGLETON_BASE   CAN_CTRL_1_REG_BASE
#define CAN_SINGLETON_ISRVEC      CYGNUM_HAL_INTERRUPT_CAN1_TX
#define CAN_SINGLETON_CHAN_NO     0
#endif

#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN1
LPC2XXX_CAN_INFO(lpc2xxx_can1_info,
                 CYGNUM_DEVS_CAN_LPC2XXX_CAN1_TX_INT_PRIORITY,
                 CYGNUM_DEVS_CAN_LPC2XXX_CAN1_RX_INT_PRIORITY,
                 CAN1_FLAG_STARTUP_ACCFILT_SETUP);
#define CAN_CTRL_SINGLETON_BASE   CAN_CTRL_2_REG_BASE
#define CAN_SINGLETON_ISRVEC      CYGNUM_HAL_INTERRUPT_CAN2_TX
#define CAN_SINGLETON_CHAN_NO     1
#endif

#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN2
LPC2XXX_CAN_INFO(lpc2xxx_can2_info,
                 CYGNUM_DEVS_CAN_LPC2XXX_CAN2_TX_INT_PRIORITY,
                 CYGNUM_DEVS_CAN_LPC2XXX_CAN2_RX_INT_PRIORITY,
                 CAN2_FLAG_STARTUP_ACCFILT_SETUP);
#define CAN_CTRL_SINGLETON_BASE   CAN_CTRL_3_REG_BASE
#define CAN_SINGLETON_ISRVEC      CYGNUM_HAL_INTERRUPT_CAN3_TX
#define CAN_SINGLETON_CHAN_NO     2
#endif

#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN3
LPC2XXX_CAN_INFO(lpc2xxx_can3_info,
                 CYGNUM_DEVS_CAN_LPC2XXX_CAN3_TX_INT_PRIORITY,
                 CYGNUM_DEVS_CAN_LPC2XXX_CAN3_RX_INT_PRIORITY,
                 CAN3_FLAG_STARTUP_ACCFILT_SETUP);
#define CAN_CTRL_SINGLETON_BASE   CAN_CTRL_4_REG_BASE
#define CAN_SINGLETON_ISRVEC      CYGNUM_HAL_INTERRUPT_CAN4_TX
#define CAN_SINGLETON_CHAN_NO     3
#endif

#endif // #if CYGINT_IO_CAN_CHANNELS > 1


//===========================================================================
//                              PROTOTYPES
//===========================================================================

//--------------------------------------------------------------------------
// Device driver interface functions
//
static bool        lpc2xxx_can_init(struct cyg_devtab_entry* devtab_entry);
static Cyg_ErrNo   lpc2xxx_can_lookup(struct cyg_devtab_entry** tab, struct cyg_devtab_entry* sub_tab, const char* name);
static Cyg_ErrNo   lpc2xxx_can_set_config(can_channel *chan, cyg_uint32 key, const void* buf, cyg_uint32* len);
static Cyg_ErrNo   lpc2xxx_can_get_config(can_channel *chan, cyg_uint32 key, const void* buf, cyg_uint32* len);
static bool        lpc2xxx_can_putmsg(can_channel *priv, CYG_CAN_MSG_T *pmsg, void *pdata);
static bool        lpc2xxx_can_getevent(can_channel *priv, CYG_CAN_EVENT_T *pevent, void *pdata);
static void        lpc2xxx_can_start_xmit(can_channel* chan);
static void        lpc2xxx_can_stop_xmit(can_channel* chan);


//--------------------------------------------------------------------------
// ISRs and DSRs
//
#ifndef CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY
static cyg_uint32 lpc2xxx_can_tx_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       lpc2xxx_can_tx_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
static cyg_uint32 lpc2xxx_can_rx_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       lpc2xxx_can_rx_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);
#endif // CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY
static cyg_uint32 lpc2xxx_can_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       lpc2xxx_can_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);


//--------------------------------------------------------------------------
// Private utility functions
//
static bool lpc2xxx_can_config_channel(can_channel* chan, cyg_can_info_t* config, cyg_bool init);
static bool lpc2xxx_can_set_baud(can_channel *chan, cyg_can_baud_rate_t *baudrate);
static Cyg_ErrNo lpc2xxx_enter_lowpower_mode(can_channel *chan);
static void lpc2xxx_start_module(can_channel *chan);
static cyg_can_state lpc2xxx_get_state(lpc2xxx_can_info_t *info);
static void lpc2xxx_set_state(lpc2xxx_can_info_t *info, cyg_can_state state);


//--------------------------------------------------------------------------
// Message box configuration
//
static void lpc2xxx_can_config_rx_all(can_channel *chan);
#ifdef CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
static void lpc2xxx_can_config_rx_none(can_channel *chan);
static bool lpc2xxx_can_add_rx_filter(lpc2xxx_can_info_t *info, cyg_can_filter *filter);
#endif // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG


#include "can_accfilt_lpc2xxx.c"

//===========================================================================
//                   GENERIC CAN IO DATA INITIALISATION
//===========================================================================
CAN_LOWLEVEL_FUNS(lpc2xxx_can_lowlevel_funs,
                  lpc2xxx_can_putmsg,
                  lpc2xxx_can_getevent,
                  lpc2xxx_can_get_config,
                  lpc2xxx_can_set_config,
                  lpc2xxx_can_start_xmit,
                  lpc2xxx_can_stop_xmit
     );


//---------------------------------------------------------------------------
// CAN channel 0
//
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN0
CYG_CAN_EVENT_T  lpc2xxx_can0_rxbuf[CYGNUM_DEVS_CAN_LPC2XXX_CAN0_QUEUESIZE_RX]; // buffer for RX can events
CYG_CAN_MSG_T    lpc2xxx_can0_txbuf[CYGNUM_DEVS_CAN_LPC2XXX_CAN0_QUEUESIZE_TX]; // buffer for TX can messages


CAN_CHANNEL_USING_INTERRUPTS(lpc2xxx_can0_chan,
                             lpc2xxx_can_lowlevel_funs,
                             lpc2xxx_can0_info,
                             CYG_CAN_BAUD_RATE(CYGNUM_DEVS_CAN_LPC2XXX_CAN0_KBAUD),
                             lpc2xxx_can0_txbuf, CYGNUM_DEVS_CAN_LPC2XXX_CAN0_QUEUESIZE_TX,
                             lpc2xxx_can0_rxbuf, CYGNUM_DEVS_CAN_LPC2XXX_CAN0_QUEUESIZE_RX
    );


DEVTAB_ENTRY(lpc2xxx_can0_devtab, 
             CYGPKG_DEVS_CAN_LPC2XXX_CAN0_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_can_devio, 
             lpc2xxx_can_init, 
             lpc2xxx_can_lookup,    // CAN driver may need initializing
             &lpc2xxx_can0_chan
    );
#endif // CYGPKG_DEVS_CAN_LPC2XXX_CAN0


//---------------------------------------------------------------------------
// CAN channel 1
//
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN1
CYG_CAN_EVENT_T  lpc2xxx_can1_rxbuf[CYGNUM_DEVS_CAN_LPC2XXX_CAN1_QUEUESIZE_RX]; // buffer for RX can events
CYG_CAN_MSG_T    lpc2xxx_can1_txbuf[CYGNUM_DEVS_CAN_LPC2XXX_CAN1_QUEUESIZE_TX]; // buffer for TX can messages


CAN_CHANNEL_USING_INTERRUPTS(lpc2xxx_can1_chan,
                             lpc2xxx_can_lowlevel_funs,
                             lpc2xxx_can1_info,
                             CYG_CAN_BAUD_RATE(CYGNUM_DEVS_CAN_LPC2XXX_CAN1_KBAUD),
                             lpc2xxx_can1_txbuf, CYGNUM_DEVS_CAN_LPC2XXX_CAN1_QUEUESIZE_TX,
                             lpc2xxx_can1_rxbuf, CYGNUM_DEVS_CAN_LPC2XXX_CAN1_QUEUESIZE_RX
    );


DEVTAB_ENTRY(lpc2xxx_can1_devtab, 
             CYGPKG_DEVS_CAN_LPC2XXX_CAN1_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_can_devio, 
             lpc2xxx_can_init, 
             lpc2xxx_can_lookup,    // CAN driver may need initializing
             &lpc2xxx_can1_chan
    );
#endif // CYGPKG_DEVS_CAN_LPC2XXX_CAN1


//---------------------------------------------------------------------------
// CAN channel 2
//
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN2
CYG_CAN_EVENT_T  lpc2xxx_can2_rxbuf[CYGNUM_DEVS_CAN_LPC2XXX_CAN2_QUEUESIZE_RX]; // buffer for RX can events
CYG_CAN_MSG_T    lpc2xxx_can2_txbuf[CYGNUM_DEVS_CAN_LPC2XXX_CAN2_QUEUESIZE_TX]; // buffer for TX can messages


CAN_CHANNEL_USING_INTERRUPTS(lpc2xxx_can2_chan,
                             lpc2xxx_can_lowlevel_funs,
                             lpc2xxx_can2_info,
                             CYG_CAN_BAUD_RATE(CYGNUM_DEVS_CAN_LPC2XXX_CAN2_KBAUD),
                             lpc2xxx_can2_txbuf, CYGNUM_DEVS_CAN_LPC2XXX_CAN2_QUEUESIZE_TX,
                             lpc2xxx_can2_rxbuf, CYGNUM_DEVS_CAN_LPC2XXX_CAN2_QUEUESIZE_RX
    );


DEVTAB_ENTRY(lpc2xxx_can2_devtab, 
             CYGPKG_DEVS_CAN_LPC2XXX_CAN2_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_can_devio, 
             lpc2xxx_can_init, 
             lpc2xxx_can_lookup,    // CAN driver may need initializing
             &lpc2xxx_can2_chan
    );
#endif // CYGPKG_DEVS_CAN_LPC2XXX_CAN2


//---------------------------------------------------------------------------
// CAN channel 3
//
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN3
CYG_CAN_EVENT_T  lpc2xxx_can3_rxbuf[CYGNUM_DEVS_CAN_LPC2XXX_CAN3_QUEUESIZE_RX]; // buffer for RX can events
CYG_CAN_MSG_T    lpc2xxx_can3_txbuf[CYGNUM_DEVS_CAN_LPC2XXX_CAN3_QUEUESIZE_TX]; // buffer for TX can messages


CAN_CHANNEL_USING_INTERRUPTS(lpc2xxx_can3_chan,
                             lpc2xxx_can_lowlevel_funs,
                             lpc2xxx_can3_info,
                             CYG_CAN_BAUD_RATE(CYGNUM_DEVS_CAN_LPC2XXX_CAN3_KBAUD),
                             lpc2xxx_can3_txbuf, CYGNUM_DEVS_CAN_LPC2XXX_CAN3_QUEUESIZE_TX,
                             lpc2xxx_can3_rxbuf, CYGNUM_DEVS_CAN_LPC2XXX_CAN3_QUEUESIZE_RX
    );


DEVTAB_ENTRY(lpc2xxx_can3_devtab, 
             CYGPKG_DEVS_CAN_LPC2XXX_CAN3_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_can_devio, 
             lpc2xxx_can_init, 
             lpc2xxx_can_lookup,    // CAN driver may need initializing
             &lpc2xxx_can3_chan
    );
#endif // CYGPKG_DEVS_CAN_LPC2XXX_CAN3


//===========================================================================
//                            IMPLEMENTATION
//===========================================================================



//===========================================================================
/// First initialisation and reset of CAN modul.
//===========================================================================
static bool lpc2xxx_can_init(struct cyg_devtab_entry* devtab_entry)
{
    can_channel          *chan    = (can_channel*)devtab_entry->priv;
    bool                  res;

#ifdef CYGDBG_IO_INIT
    diag_printf("LPC2XXX CAN init\n");
#endif  

    //
    // Newer LPC2xxx variants do not support individual interrupt
    // sources for CAN on chip peripherals
    //
#ifndef CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY  
    lpc2xxx_can_info_t   *info    = (lpc2xxx_can_info_t *)chan->dev_priv;  
    //
    // Create TX interrupt
    //
    cyg_drv_interrupt_create(CAN_ISRVEC(info),
                             info->tx_interrupt_priority,
                             (cyg_addrword_t)chan,     // Data item passed to interrupt handler
                             lpc2xxx_can_tx_ISR,
                             lpc2xxx_can_tx_DSR,
                             &info->tx_interrupt_handle,
                             &info->tx_interrupt);
    cyg_drv_interrupt_attach(info->tx_interrupt_handle);
    cyg_drv_interrupt_unmask(CAN_ISRVEC(info));
    
    //
    // Create RX interrupt
    //
    cyg_drv_interrupt_create(CAN_ISRVEC(info) + 6,
                             info->rx_interrupt_priority,
                             (cyg_addrword_t)chan,     // Data item passed to interrupt handler
                             lpc2xxx_can_rx_ISR,
                             lpc2xxx_can_rx_DSR,
                             &info->rx_interrupt_handle,
                             &info->rx_interrupt);
    cyg_drv_interrupt_attach(info->rx_interrupt_handle);
    cyg_drv_interrupt_unmask(CAN_ISRVEC(info) + 6);
#endif // CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY
    
    //
    // Now create and enable global CAN interrupt. This interrupt is
    // global for all channels and so we need to call it only one times -
    // when the first channel is initialized
    //
#if CYGINT_IO_CAN_CHANNELS > 1
    if (!lpc2xxx_global_can_info.init_cnt)
#endif // #if CYGINT_IO_CAN_CHANNELS > 1
    {
        //
        // Create err interrupt
        //
        cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_CAN,
#ifndef CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY
                                 CYGNUM_DEVS_CAN_LPC2XXX_ERR_INT_PRIORITY,
#else // CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY
                                 CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY,
#endif // CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY
                                 0,                        // Data item passed to interrupt handler
                                 lpc2xxx_can_ISR,
                                 lpc2xxx_can_DSR,
                                 &lpc2xxx_global_can_info.interrupt_handle,
                                 &lpc2xxx_global_can_info.interrupt);
        cyg_drv_interrupt_attach(lpc2xxx_global_can_info.interrupt_handle);
        cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_CAN);     
    }   
   
    res = lpc2xxx_can_config_channel(chan, &chan->config, true);
#if CYGINT_IO_CAN_CHANNELS > 1
    lpc2xxx_global_can_info.active_channels[lpc2xxx_global_can_info.init_cnt++] = chan;
#else // CYGINT_IO_CAN_CHANNELS > 1
    lpc2xxx_global_can_info.active_channels[0] = chan;
#endif
    return res; 
}


//===========================================================================
// Configure can channel
//===========================================================================
static bool lpc2xxx_can_config_channel(can_channel* chan, cyg_can_info_t* config, cyg_bool init)
{
    CAN_DECLARE_INFO(chan);
    bool       res = true;
    
    if (init)
    {
        //
        // In case platform needs extra initialization (i.e. setup of
        // CAN transceivers) it should implement this macro
        //
#ifdef CYGPRI_IO_CAN_LPC2XXX_PLF_INIT_HOOK
        CYGPRI_IO_CAN_LPC2XXX_PLF_INIT_HOOK(chan, config);
#endif
        
        HAL_WRITE_UINT32(CAN_ACCFILT_AFMR, AFMR_OFF);       // Acceptance Filter Mode Register = off
        HAL_WRITE_UINT32(CAN_CTRL_MOD(info), CANMOD_RESET); // Go into reset mode
        HAL_WRITE_UINT32(CAN_CTRL_IER(info), 0);            // disable all interrupts
        HAL_WRITE_UINT32(CAN_CTRL_GSR(info), 0);            // Clear Status register - clears error counters  
        
        //
        // Perform platform/variant specific initialisation here. 
        // The variant/ platform should setup the pin configuration to support 
        // CAN here
        //
        HAL_LPC2XXX_INIT_CAN(CAN_CHAN_NO(info)); 
         
        //
        // If this is the first channel to initialize then we reset the CAN 
        // registers and setup the CAN I/O pins
        //
#if CYGINT_IO_CAN_CHANNELS > 1
        if (!lpc2xxx_global_can_info.init_cnt)
#endif // #if CYGINT_IO_CAN_CHANNELS > 1
        {
            lpc2xxx_can_accfilt_reset();           
        }   
    } // if (init)
    
    res = lpc2xxx_can_set_baud(chan, &config->baud);           // set baudrate
    // $$$$ enable receive interrupt?
    HAL_WRITE_UINT32(CAN_CTRL_MOD(info), CANMOD_OPERATIONAL);  // enter normal operating mode
            
    //
    // store new config values
    //
    if (config != &chan->config) 
    {
        chan->config = *config;
    }   
    
    return res;
}


//===========================================================================
// Set baudrate of certain can channel
//===========================================================================
static bool lpc2xxx_can_set_baud(can_channel *chan, cyg_can_baud_rate_t *baudrate)
{
    bool                  res = true;
    cyg_uint32            canbtr;
    cyg_uint32            canmod;
    CAN_DECLARE_INFO(chan);
    
    //
    // Get bit timings from HAL because bit timings depend on sysclock
    // If the macro fills the canbtr value with 0 then the baudrate
    // is not supported and the function returns false
    //
    HAL_LPC2XXX_GET_CAN_BR(*baudrate, canbtr);   
    if (0 == canbtr)
    {
        return false;
    }
    
    //
    // Any modificatons to the baudrate register must be done while CAN
    // module is in reset mode. So we first set the CAN module in reset
    // mode, then we set baudrate and then we restore content of CANMOD 
    // register
    //
    HAL_READ_UINT32(CAN_CTRL_MOD(info), canmod);        // backup canmod register
    HAL_WRITE_UINT32(CAN_CTRL_MOD(info), CANMOD_RESET); // Go into reset mode
    HAL_WRITE_UINT32(CAN_CTRL_BTR(info), canbtr);       // write baudrate value
    HAL_WRITE_UINT32(CAN_CTRL_MOD(info), canmod);       // restore previous value
       
    return res;
}


//===========================================================================
//  Lookup the device and return its handle
//===========================================================================
static Cyg_ErrNo lpc2xxx_can_lookup(struct cyg_devtab_entry** tab, struct cyg_devtab_entry* sub_tab, const char* name)
{
    can_channel* chan    = (can_channel*) (*tab)->priv;
    lpc2xxx_can_info_t *info = (lpc2xxx_can_info_t *)chan->dev_priv;
    cyg_uint32   regval;

    chan->callbacks->can_init(chan); 
    
    //
    // If runtime acceptance filter configuration is supported then we only
    // configure RX ALL if the user selected the RX ALL setup in config utility
    //
#ifdef CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
    if (info->flags & INFO_FLAG_STARTUP_RX_ALL)
#endif // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
    {
       lpc2xxx_can_config_rx_all(chan); 
    }
    
    HAL_WRITE_UINT32(CAN_ACCFILT_AFMR, AFMR_ON);       // Activate acceptance filter
    HAL_READ_UINT32(CAN_CTRL_IER(info), regval);
    regval = regval | IER_RX | CAN_MISC_INT;           // enable all interrupts     
    HAL_WRITE_UINT32(CAN_CTRL_IER(info), regval);  
      
    return ENOERR;
}


#ifdef CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
//===========================================================================
// Setup LPC2XXX CAN module in a state where all message boxes are disabled
// After this call it is possible to add single message buffers and filters
//===========================================================================
static void lpc2xxx_can_config_rx_none(can_channel *chan)
{
    lpc2xxx_can_info_t *info = (lpc2xxx_can_info_t *)chan->dev_priv;
      
    //
    // Remove all acceptance filters
    // $$$$ maybe we should also abort any pending transfers and
    // disable receive interrupts ?
    //
    lpc2xxx_can_accfilt_remove_all_ctrl_entries(info);
    info->flags  &= ~INFO_FLAG_RX_ALL;
#ifdef CYGDBG_DEVS_CAN_LPC2XXX_DEBUG
    lpc2xxx_can_accfilt_dbg_dump();
#endif
}


//===========================================================================
// Add one single message filter to acceptance filter
//===========================================================================
static bool lpc2xxx_can_add_rx_filter(lpc2xxx_can_info_t *info, cyg_can_filter *filter)
{
    bool res;
    
    res = lpc2xxx_can_accfilt_add(info, filter->msg.id, 0, filter->msg.ext); 
    if (!res)
    {
        filter->handle = CYGNUM_CAN_MSGBUF_NA;
    }
#ifdef CYGDBG_DEVS_CAN_LPC2XXX_DEBUG
    lpc2xxx_can_accfilt_dbg_dump();
#endif    
    return res;
}
#endif // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG



#ifdef CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
//===========================================================================
// Configure message buffers
//===========================================================================
static Cyg_ErrNo lpc2xxx_can_config_msgbuf(can_channel *chan, const void* buf, cyg_uint32* len)
{
    Cyg_ErrNo             res  = ENOERR;
    lpc2xxx_can_info_t   *info = (lpc2xxx_can_info_t *)chan->dev_priv;   
    cyg_can_msgbuf_cfg   *msg_buf = (cyg_can_msgbuf_cfg *)buf;

    if (*len != sizeof(cyg_can_msgbuf_cfg))
    {
        return -EINVAL;
    }

    switch (msg_buf->cfg_id)
    {
        //
        // clear all message filters and remote buffers - prepare for message buffer
        // configuration
        //
        case CYGNUM_CAN_MSGBUF_RESET_ALL :
             {
                 lpc2xxx_can_config_rx_none(chan);
             }
             break;

        //
        // setup driver for reception of all standard and extended messages
        //
        case CYGNUM_CAN_MSGBUF_RX_FILTER_ALL :
             {
                 if (!(info->flags & INFO_FLAG_RX_ALL)) // if rx_all is enabled we do not need to do anything
                 {
                    lpc2xxx_can_config_rx_all(chan);  // setup RX all state
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
                 // if the acceptance filter is configured to receive all messages then 
                 // it is not allowed to add single message filters because then more 
                 // than one acceptance filter would receive the same CAN id
                 //
                 if (info->flags & INFO_FLAG_RX_ALL)
                 {
                    return -EPERM;
                 }
                 
                 //
                 // try to allocate a free acceptance filter entry - if we have a free one
                 // then we can prepare the acceptance filter table for reception of
                 // this message
                 //
                 if (!lpc2xxx_can_add_rx_filter(info, filter))
                 {
                     return -EPERM;
                 }
             }
             break; //CYGNUM_CAN_MSGBUF_RX_FILTER_ADD
             

#ifdef CYGOPT_IO_CAN_REMOTE_BUF
        //
        // Try to add a new RTR response message buffer for automatic
        // transmission of data frame on reception of a remote frame
        //
        case CYGNUM_CAN_MSGBUF_REMOTE_BUF_ADD :
             {
                 // $$$$ TODO implement remote response buffers in software
                 return -ENOSUPP;
             }
             break;
                     
        //
        // write data into remote response buffer
        //
        case CYGNUM_CAN_MSGBUF_REMOTE_BUF_WRITE :
             {
                 // $$$$ TODO implement remote response buffers in software
                 return -ENOSUPP;
             }
             break;
#endif // #ifdef CYGOPT_IO_CAN_REMOTE_BUF
        default:
            return -EINVAL;
    } // switch (buf->cfg_id)
    
    return res;
}
#endif // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG


//===========================================================================
// Read state of CAN controller
// The CAN state variable for each channel is modified by DSR so if we 
// read the state we need to lock DSRs to protect the data access
//===========================================================================
static cyg_can_state lpc2xxx_get_state(lpc2xxx_can_info_t *info)
{
    cyg_can_state result;
    
    cyg_drv_dsr_lock();
    result = info->state;
    cyg_drv_dsr_unlock();
    
    return result;
}


//===========================================================================
// Set state of CAN controller
// The CAN state variable for each channel is modified by DSR so if we 
// write the state we need to lock DSRs to protect the data access
//===========================================================================
static void lpc2xxx_set_state(lpc2xxx_can_info_t *info, cyg_can_state state)
{   
    cyg_drv_dsr_lock();
    info->state = state;
    cyg_drv_dsr_unlock();
}


//===========================================================================
// Enter low power mode
//===========================================================================
static Cyg_ErrNo lpc2xxx_enter_lowpower_mode(can_channel *chan)
{
    cyg_uint32          regval;
    lpc2xxx_can_info_t *info = (lpc2xxx_can_info_t *)chan->dev_priv;
    
    //
    // Before we enter low power mode, we have to enable wake up interrupt
    // Normally this interrupt is always enabled so we do not need to do
    // anything here
    //
    HAL_READ_UINT32(CAN_CTRL_MOD(info), regval); 
    
    //
    // Software can only set SM when RM in the CAN Mode register is 0
    //
    if (regval & CANMOD_RESET)
    {
        return -EPERM;
    }
    
    //regval &= CANMOD_SLEEP;
    lpc2xxx_set_state(info, CYGNUM_CAN_STATE_STANDBY);
    HAL_WRITE_UINT32(CAN_CTRL_MOD(info), CANMOD_SLEEP); 
    return ENOERR;
}


//===========================================================================
// Start CAN module - set CANMOD operational and enable all interrupts
//===========================================================================
static void lpc2xxx_start_module(can_channel *chan)
{
    cyg_uint32          regval;
    lpc2xxx_can_info_t *info = (lpc2xxx_can_info_t *)chan->dev_priv;
    
    HAL_WRITE_UINT32(CAN_CTRL_MOD(info), CANMOD_OPERATIONAL);  
    //
    // The interrupt enable register is also modified by ISR and DSR so
    // we need to protect acces here
    //
    cyg_drv_isr_lock();
    HAL_READ_UINT32(CAN_CTRL_IER(info), regval);
    regval = regval | IER_RX | CAN_MISC_INT;           // enable all interrupts     
    HAL_WRITE_UINT32(CAN_CTRL_IER(info), regval); 
    info->state = CYGNUM_CAN_STATE_ACTIVE; 
    cyg_drv_isr_unlock();
}


//===========================================================================
// Enter reset mode
//===========================================================================
static void lpc2xxx_enter_reset_mode(can_channel *chan)
{
    lpc2xxx_can_info_t *info = (lpc2xxx_can_info_t *)chan->dev_priv;  
    
    info->state = CYGNUM_CAN_STATE_STOPPED;
    HAL_WRITE_UINT32(CAN_CTRL_MOD(info), CANMOD_RESET);
}


#ifdef CYGOPT_DEVS_CAN_RANGE_FILTERING_CFG_KEYS
//===========================================================================
// Add message filter group
//===========================================================================
static Cyg_ErrNo lpc2xxx_can_config_accfilt_group(can_channel *chan, const void* buf, cyg_uint32* len)
{
    bool                     res;
    cyg_can_filter_range_cfg *filter_grp = (cyg_can_filter_range_cfg *)buf;
    lpc2xxx_can_info_t      *info = (lpc2xxx_can_info_t *)chan->dev_priv; 
    
    
    if (*len != sizeof(cyg_can_filter_range_cfg))
    {
        return -EINVAL;
    }
    
    if (filter_grp->lower_id_bound >= filter_grp->upper_id_bound)
    {
        return -EINVAL;
    }
    
    //
    // if the acceptance filter is configured to receive all messages then 
    // it is not allowed to add single message filter groups because then more 
    // than one acceptance filter would receive the same CAN id
    //
    if (info->flags & INFO_FLAG_RX_ALL)
    {
        return -EPERM;
    }
    
    res = lpc2xxx_can_accfilt_add(info, 
                                  filter_grp->lower_id_bound, 
                                  filter_grp->upper_id_bound, 
                                  filter_grp->ext);
    
#ifdef CYGDBG_DEVS_CAN_LPC2XXX_DEBUG
    lpc2xxx_can_accfilt_dbg_dump();
#endif        
    return res ? ENOERR : -EPERM;
}
#endif // CYGOPT_DEVS_CAN_RANGE_FILTERING_CFG_KEYS


//===========================================================================
// Change device configuration
//===========================================================================
static Cyg_ErrNo lpc2xxx_can_set_config(can_channel *chan, cyg_uint32 key, const void* buf, cyg_uint32* len)
{
    Cyg_ErrNo  res = ENOERR;
    
    switch (key)
    {   
        //
        // Setup a new CAN configuration. This will i.e. setup a new baud rate
        //
        case CYG_IO_SET_CONFIG_CAN_INFO:
             {
                 cyg_can_info_t*  config = (cyg_can_info_t*) buf;
                 if (*len < sizeof(cyg_can_info_t))
                 {
                     return -EINVAL;
                 }
                 *len = sizeof(cyg_can_info_t);
                 if (!lpc2xxx_can_config_channel(chan, config, false))
                 {
                     return -EINVAL;
                 }
             }
             break;

#ifdef CYGOPT_IO_CAN_RUNTIME_MBOX_CFG            
        //
        // configure message buffers
        //
        case CYG_IO_SET_CONFIG_CAN_MSGBUF :
             {               
                res = lpc2xxx_can_config_msgbuf(chan, buf, len);
             }
             break;
#endif // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
         
#ifdef CYGOPT_DEVS_CAN_RANGE_FILTERING_CFG_KEYS
        //
        // Add message filter group to acceptance filter
        //
        case CYG_IO_SET_CONFIG_CAN_RANGE_FILTER :
             {
                 return lpc2xxx_can_config_accfilt_group(chan, buf, len);
             }
             break;
#endif // CYGOPT_DEVS_CAN_RANGE_FILTERING_CFG_KEYS
                        
        //
        // Change CAN state of CAN module
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
                // decide what to do according to mode
                //
                switch (*can_mode)
                {
                    //
                    // The controller does not support a stopped and standby state so we
                    // simply enter the low power state here. This state is also safe for
                    // message buffer configuration
                    //
                    case CYGNUM_CAN_MODE_STOP :    lpc2xxx_enter_reset_mode(chan);    break; 
                    case CYGNUM_CAN_MODE_START :   lpc2xxx_start_module(chan);        break;                       
                    case CYGNUM_CAN_MODE_STANDBY : lpc2xxx_enter_lowpower_mode(chan); break;
                    case CYGNUM_CAN_MODE_CONFIG :  lpc2xxx_enter_reset_mode(chan);    break;
                    case CYGNUM_CAN_MODE_LISTEN_ONLY_ENTER: return -EINVAL;
                    case CYGNUM_CAN_MODE_LISTEN_ONLY_EXIT: return -EINVAL;
                }
             }
             break; // case CYG_IO_SET_CONFIG_CAN_MODE :
        //
        // Unknown config key - indicate this by returning -EINVAL
        //
        default:
                    return -EINVAL;
    } // switch (key)
    
    return res;
}


//===========================================================================
// Query device configuration
//===========================================================================
static Cyg_ErrNo lpc2xxx_can_get_config(can_channel *chan, cyg_uint32 key, const void* buf, cyg_uint32* len)
{
    Cyg_ErrNo            res  = ENOERR;
    lpc2xxx_can_info_t  *info = (lpc2xxx_can_info_t *)chan->dev_priv;
    
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
                *can_state = lpc2xxx_get_state(info);
             }
             break;

#ifdef CYGOPT_IO_CAN_RUNTIME_MBOX_CFG       
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
                 cyg_uint32 end_of_table;
                *len = sizeof(cyg_can_msgbuf_info);
              
                 HAL_READ_UINT32(CAN_ACCFILT_ENDOFTABLE, end_of_table);
                 mbox_info->count = LPC2XXX_CAN_MSG_FILTERS_MAX;
                 mbox_info->free  = (ACCFILT_RAM_SIZE - end_of_table) / ACCFILT_COMMON_ENTRY_SIZE;
             }
             break;
#endif // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG

        
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
             }
             break;
             
        default :
            res = -EINVAL;
    }// switch(key)
    
    return res;
}


//===========================================================================
// Send single message
//===========================================================================
static bool lpc2xxx_can_putmsg(can_channel *chan, CYG_CAN_MSG_T *pmsg, void *pdata)
{
    cyg_uint32 regval;
#ifdef CYGOPT_DEVS_CAN_LPC2XXX_USE_SELF_RECEPTION
    lpc2xxx_can_info_t  *info = (lpc2xxx_can_info_t *) chan->dev_priv;
#else
    CAN_DECLARE_INFO(info);
#endif
    
    //
    // We use only one single transmit buffer of the three available buffers
    // We use buffer 1 (buffer 2 and 3 are unused)
    //
    // The errata sheet tells the following about the transmit buffers:
    // Problem: The Triple Transmit Buffer function cannot be used.
    // Work-around: Use any one Transmit buffer only (Use either Transmit Buffer 1, 
    // Transmit Buffer 2 or Transmit Buffer 3 exclusively). The buffer you decided 
    // to use should be loaded only when there is no pending transmission.
    //
    HAL_READ_UINT32(CAN_CTRL_SR(info), regval);
    if (!(regval & SR_TX_BUF_WRITE_OK))
    {
        return false;    
    }
    
    regval = pmsg->dlc << 16;
    if (pmsg->rtr)
    {
        regval |= TFI_DLC_RTR;
    }
 
#ifdef CYGOPT_IO_CAN_EXT_CAN_ID
    if (pmsg->ext)
    {
        regval |= TFI_DLC_EXT;
    }
#endif // #define CYGOPT_IO_CAN_EXT_CAN_ID
    HAL_WRITE_UINT32(CAN_CTRL_TFI1(info), regval);                       // write DLC
    HAL_WRITE_UINT32(CAN_CTRL_TID1(info), pmsg->id);                     // write ID
    HAL_WRITE_UINT32(CAN_CTRL_TDA1(info), pmsg->data.dwords[0]);         // write first 4 data bytes
    HAL_WRITE_UINT32(CAN_CTRL_TDB1(info), pmsg->data.dwords[1]);         // write second 4 data bytes
    
    //
    // Request transmission of message
    // The errata sheet tells the following about tx request:
    // Introduction: The CAN module can lose arbitration to another CAN node during an 
    // attempt to transmit a CAN message. The message of the CAN node the arbitration was 
    // lost to is supposed to be received correctly by the CAN module.
    // Problem: Messages might not be received correctly if during a CAN Transmission the 
    // CAN bus arbitration is lost to another CAN node.
    // Work-around: Use the Self Reception Request command instead of the Transmission 
    // Request command. However, it has to be taken into account that now all transmitted
    // messages may be received if not prevented by appropriate Acceptance Filter settings. 
    // (Don't set up Acceptance Filter Message Identifiers for the messages you are
    // transmitting yourself.)
    //
#ifdef CYGOPT_DEVS_CAN_LPC2XXX_USE_SELF_RECEPTION
    // Calc last_tx_id
    regval = pmsg->id | (regval & LPC2XXX_CAN_INFO_LAST_TX_ID_FLMASK);
    
    // Save last message id to next last_tx_id
    info->last_tx_index = info->last_tx_index == 0 ? 1 : 0;
    info->last_tx_id[info->last_tx_index] = regval;
    
    // Write self transmission request
    HAL_WRITE_UINT32(CAN_CTRL_CMR(info), CMR_SELF_RX_REQ | CMR_SEND_TX_BUF1);
#else
    // Write transmission request
    HAL_WRITE_UINT32(CAN_CTRL_CMR(info), CMR_TX_REQ | CMR_SEND_TX_BUF1);
#endif
   
    return true;
}


//===========================================================================
// Read event from device driver
//===========================================================================
static bool lpc2xxx_can_getevent(can_channel *chan, CYG_CAN_EVENT_T *pevent, void *pdata)
{
    lpc2xxx_can_info_t   *info       = (lpc2xxx_can_info_t *)chan->dev_priv;
    bool                  res        = true;
    cyg_uint32            regval;
    cyg_uint32            event      = *((cyg_uint32*)pdata);
    lsc_buf_t             data;
      
    //
    // Handle RX event
    //
    if (event & ICR_RX)
    {
        cyg_uint32            id;
        
        pevent->flags |= CYGNUM_CAN_EVENT_RX;
        HAL_READ_UINT32(CAN_CTRL_RFS(info), regval); 
        HAL_READ_UINT32(CAN_CTRL_RID(info), id);

#ifdef CYGOPT_IO_CAN_EXT_CAN_ID
        if (regval & RFS_EXT)
        {
            pevent->msg.ext = CYGNUM_CAN_ID_EXT;
            pevent->msg.id = id & 0x1FFFFFFF;
        }
        else
#endif // #define CYGOPT_IO_CAN_EXT_CAN_ID
        {
#ifdef CYGOPT_IO_CAN_STD_CAN_ID
            pevent->msg.ext = CYGNUM_CAN_ID_STD;
            pevent->msg.id = id & 0x7FF;
#endif // #ifdef CYGOPT_IO_CAN_STD_CAN_ID
        } // if (regval & RFS_EXT)
        
        if (regval & RFS_RTR)
        {
            pevent->msg.rtr = CYGNUM_CAN_FRAME_RTR;
        }
        else
        {
            pevent->msg.rtr = CYGNUM_CAN_FRAME_DATA;  
            HAL_READ_UINT32(CAN_CTRL_RDA(info), pevent->msg.data.dwords[0]);
            HAL_READ_UINT32(CAN_CTRL_RDB(info), pevent->msg.data.dwords[1]);
        } //if (regval & RFS_RTR)
        pevent->msg.dlc = RFS_GET_DLC(regval);
        //
        // Release the message buffer. Now this buffer can receive the next message
        //
        HAL_WRITE_UINT32(CAN_CTRL_CMR(info), CMR_RX_RELEASE_BUF);
    
        //
        // Now check if an data overrun occurred - a message was lost
        // because the preceding message to this CAN controller was not read 
        // and released quickly enough. After reading the status we clear 
        // the overrun bit
        // 
        HAL_READ_UINT32(CAN_CTRL_GSR(info), regval);
        if (regval & GSR_DATA_OVR)
        {
            pevent->flags |= CYGNUM_CAN_EVENT_OVERRUN_RX_HW;
            HAL_WRITE_UINT32(CAN_CTRL_CMR(info), CMR_CLEAR_DATA_OVR);
        }
    }
    
    //
    // Handle TX events
    //
#ifndef CYGOPT_IO_CAN_TX_EVENT_SUPPORT 
    if (event & ICR_TX1)
    {
        pevent->flags |= CYGNUM_CAN_EVENT_TX;
    }
#endif
    
    //
    // Handle all other events
    //
    if (event & (CAN_MISC_INT | ICR_LUT_ERR))
    {
        HAL_READ_UINT32(CAN_CTRL_GSR(info), data.dword);
        
        //
        // 1: Error Warning Interrupt -- this bit is set on every change (set or clear) of the Error
        // Status or Bus Status bit in CANSR, if the EIE bit in CAN is 1 at the time of the
        // change.
        //
        if (event & ICR_ERR_WARN)
        {
            //
            // If one of the warning counters is above 96 then the controller is in bus warning
            // state. If both counters are below 96 the this interrupt indicates that the
            // controller has left the bus warning state and is error active again
            //
            if (data.bytes[2] >= 96)
            {
                pevent->flags |= CYGNUM_CAN_EVENT_WARNING_RX;  
                info->state = CYGNUM_CAN_STATE_BUS_WARN;
            }
            else if (data.bytes[3] >= 96)
            {
                pevent->flags |= CYGNUM_CAN_EVENT_WARNING_TX;
                info->state = CYGNUM_CAN_STATE_BUS_WARN;
            }
            else
            {
                info->state = CYGNUM_CAN_STATE_ACTIVE;
            }
            LPC2XXX_DBG_PRINT("ICR_ERR_WARN (%p)\n", (void*) chan);
        }
        
        //
        // 1: Wake-Up Interrupt: this bit is set if the CAN controller is sleeping and bus activity
        // is detected, if the WUIE bit in CANIE is 1.
        //
        if (event & ICR_WAKE_UP)
        {
            pevent->flags |= CYGNUM_CAN_EVENT_LEAVING_STANDBY; 
            info->state = CYGNUM_CAN_STATE_ACTIVE;
            LPC2XXX_DBG_PRINT("ICR_WAKE_UP (%p)\n", (void*) chan);
        }
        
        //
        // Error Passive Interrupt -- this bit is set if the EPIE bit in CANIE is 1, and the CAN
        // controller switches between Error Passive and Error Active mode in either
        // direction. We have to check if the ERR bit is set in global status register.
        // If it is set, then it is a switch to error passive else it is a switch to
        // error active state
        //
        if (event & ICR_ERR_PASSIVE)
        {
            if (data.dword & GSR_ERR)
            {
                pevent->flags |= CYGNUM_CAN_EVENT_ERR_PASSIVE;    
                info->state = CYGNUM_CAN_STATE_ERR_PASSIVE;
            }
            else
            {
                info->state = CYGNUM_CAN_STATE_ACTIVE;    
            }
            LPC2XXX_DBG_PRINT("ICR_ERR_PASSIVE (%p)\n", (void*) chan);
        }

#ifdef CYGOPT_DEVS_CAN_LPC2XXX_ALIE       
        //
        // Arbitration Lost Interrupt -- this bit is set if the ALIE bit in CANIE is 1, and the
        // CAN controller loses arbitration while attempting to transmit.
        //
        if (event & ICR_ARBITR_LOST)
        {
            pevent->flags |= CYGNUM_CAN_EVENT_ARBITRATION_LOST;   
            LPC2XXX_DBG_PRINT("ICR_ARBITR_LOST (%p)\n", (void*) chan);
        }
#endif // CYGOPT_DEVS_CAN_LPC2XXX_ALIE
        
        //
        // 1: Bus Error Interrupt -- this bit is set if the BEIE bit in CANIE is 1, and the CAN
        // controller detects an error on the bus.
        //
        if (event & ICR_BUS_ERR)
        {
            pevent->flags |= CYGNUM_CAN_EVENT_BUS_OFF; 
            LPC2XXX_DBG_PRINT("ICR_BUS_ERR (%p)\n", (void*) chan);
        }
        
#ifdef CYGOPT_DEVS_CAN_LPC2XXX_LUT_ERR_SUPP
        //
        // LUT error interrupt -- this bit is set if bit 0 in LUTerr is 1 and LUTerrAd
        // points to entry in filter table for this CAN controller
        //
        if(event & ICR_LUT_ERR)
        {
            pevent->flags |= CYGNUM_CAN_EVENT_FILTER_ERR;
            LPC2XXX_DBG_PRINT("ICR_LUT_ERR (%p)\n", (void*) chan);
        }
#endif // CYGOPT_DEVS_CAN_LPC2XXX_LUT_ERR_SUPP

    } // if (event & (CAN_MISC_INT | ICR_LUT_ERR))
          
    return res;
}


//===========================================================================
// Kick transmitter
//===========================================================================
static void lpc2xxx_can_start_xmit(can_channel* chan)
{
    cyg_uint32 regval;
    CAN_DECLARE_INFO(chan);
    
    LPC2XXX_DBG_PRINT("start_xmit (%p)\n", (void*) chan);

    cyg_drv_dsr_lock();
    HAL_READ_UINT32(CAN_CTRL_IER(info), regval);
    regval |= IER_TX1;                           // enable tx interrupt for tx buf 1
    HAL_WRITE_UINT32(CAN_CTRL_IER(info), regval); 
    cyg_drv_dsr_unlock();
    
    //
    // kick transmitter
    //
    chan->callbacks->xmt_msg(chan, 0);  // Kick transmitter (if necessary)
}


//===========================================================================
// Stop transmitter
//===========================================================================
static void lpc2xxx_can_stop_xmit(can_channel* chan)
{
    cyg_uint32 regval; 
    CAN_DECLARE_INFO(chan);
    
    LPC2XXX_DBG_PRINT("stop_xmit (%p)\n", (void*) chan);
     
    cyg_drv_dsr_lock();
    HAL_READ_UINT32(CAN_CTRL_IER(info), regval);
    regval &= ~IER_TX1;                           // disable tx interrupt for tx buf 1
    HAL_WRITE_UINT32(CAN_CTRL_IER(info), regval); 
    cyg_drv_dsr_unlock();
}


#ifndef CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY
//===========================================================================
// Low level transmit interrupt handler
//===========================================================================
static cyg_uint32 lpc2xxx_can_tx_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    //
    // Now read input capture register - this clears all interrupt bits in this
    // register and also acknowledges the interrupt - any further processing is done
    // by the DSR
    //
    cyg_drv_interrupt_mask(vector);
    cyg_drv_interrupt_acknowledge(vector);
    LPC2XXX_DBG_PRINT("tx_ISR (%p)\n", (void*) data);
    return CYG_ISR_CALL_DSR;
}


//===========================================================================
// High level transmit interrupt handler
//===========================================================================
static void lpc2xxx_can_tx_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    can_channel              *chan = (can_channel *)data;
    cyg_uint32                regval;
    lpc2xxx_can_info_t       *info = (lpc2xxx_can_info_t *)chan->dev_priv;
    
    //
    // First read the ICR register to acknowledge all interrupts and
    // get all captured interrupts
    //
    HAL_READ_UINT32(CAN_CTRL_ICR(info), regval);
    
    //
    // If TX events are supported then only call the rcv_event() callback
    // if any other event occurred - pass the event field to the getevent function
    // to indicate the events
    //
#ifndef CYGOPT_IO_CAN_TX_EVENT_SUPPORT
    if (regval & ~ICR_TX1) 
#endif
    {
        chan->callbacks->rcv_event(chan, &regval); 
    }
    
    //
    // Now transmit next message and reenable interrupts
    //
    chan->callbacks->xmt_msg(chan, 0); // send next message 
    LPC2XXX_DBG_PRINT("tx_DSR (%p)\n", (void*) data);
    cyg_drv_interrupt_unmask(vector);    
}


//===========================================================================
// Low level receive interrupt handler
//===========================================================================
static cyg_uint32 lpc2xxx_can_rx_ISR(cyg_vector_t vector, cyg_addrword_t data)
{   
#ifdef CYGOPT_DEVS_CAN_LPC2XXX_USE_SELF_RECEPTION
    cyg_uint32   regval;
    can_channel* chan = (can_channel*)data;
    lpc2xxx_can_info_t  *info = (lpc2xxx_can_info_t *) chan->dev_priv;
    cyg_uint32 id;
    cyg_uint32 index;
    
    // We have to reject self tx message, so read message id
    HAL_READ_UINT32(CAN_CTRL_RID(info), id);
    HAL_READ_UINT32(CAN_CTRL_RFS(info), regval);
    id |= (regval & LPC2XXX_CAN_INFO_LAST_TX_ID_FLMASK);
    
    // Test message id
    for(index = 0; index < 2; index++)
    {
        if(id == info->last_tx_id[index])
        {
            // Clear last_tx_id
            info->last_tx_id[index] = LPC2XXX_CAN_INFO_LAST_TX_ID_NOVALID;
            
            // Clear receive buffer
            HAL_WRITE_UINT32(CAN_CTRL_CMR(info), CMR_RX_RELEASE_BUF);
            
            // Acknowledge a vector
            cyg_drv_interrupt_acknowledge(vector);
            
            // Exit without calling DSR
            LPC2XXX_DBG_PRINT("self_rx_ISR (%p)\n", (void*) data);
            return CYG_ISR_HANDLED;
        }
    }
#endif
        
    //
    // The ISR only disables and acknowledges the RX interrupt
    // any further processing is done by DSR. We also need to mask the
    // global CAN status interrupt here because the interrupt flag
    // in ICR is not cleared yet and may still cause a status 
    // interrupt
    //
    cyg_drv_interrupt_mask(vector);
    cyg_drv_interrupt_acknowledge(vector);
    LPC2XXX_DBG_PRINT("rx_ISR (%p)\n", (void*) data);

    return CYG_ISR_CALL_DSR;
}


//===========================================================================
// High level receive interrupt handler
//===========================================================================
static void lpc2xxx_can_rx_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    can_channel  *chan = (can_channel *)data;
    cyg_uint32    icr = ICR_RX;
      
    //
    // Read the event, the receive buffer will be released by the
    // get_event() function
    //
    chan->callbacks->rcv_event(chan, &icr); 
    LPC2XXX_DBG_PRINT("rx_DSR (%p)\n", (void*) data);
    cyg_drv_interrupt_unmask(vector);
}



//===========================================================================
// status ISR handler
//===========================================================================
static cyg_uint32 lpc2xxx_can_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    //
    // Acknowledge and disable the interrupt - any further processing is
    // done by the DSR
    //    
    cyg_drv_interrupt_mask(vector);    
    cyg_drv_interrupt_acknowledge(vector);
    LPC2XXX_DBG_PRINT("err_ISR\n");
    return CYG_ISR_CALL_DSR;    
}


//===========================================================================
// status ISR handler
//===========================================================================
static void lpc2xxx_can_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{     
#ifdef CYGOPT_DEVS_CAN_LPC2XXX_LUT_ERR_SUPP
    // If we use acceptance filter we can get LUT error
    cyg_uint32 luterr;
    cyg_uint8  luterr_chan0 = 0; // Init to avoid warnings
    cyg_uint8  luterr_chan1 = 0; // Init to avoid warnings
    
    // Read LUT error flag
    HAL_READ_UINT32(CAN_ACCFILT_LUT_ERR, luterr);
    
    if (luterr & 1)
    {
        cyg_uint32 lutaddr;
        cyg_uint32 eff_sa;
        lsc_buf_t  errentry;
        
        // Read address of failed entry (it clears interrupt flag)
        HAL_READ_UINT32(CAN_ACCFILT_LUT_ERR_ADDR, lutaddr);
        
        // Read address of extended id individual table
        HAL_READ_UINT32(CAN_ACCFILT_EFF_SA, eff_sa); 
        
        // Read error entry
        HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + lutaddr, errentry.dword);
        
        // If err entry from standard id tables then read two
        // controllers numbers
        if(lutaddr < eff_sa)
        {
            // Calc CAN controllers numbers
            luterr_chan0 = (cyg_uint8) ACCFILT_STD_GET_CTRL_UPPER(errentry.dword);
            
            if(errentry.column.lower & ACCFILT_STD_DIS)
            {
            	luterr_chan1 = luterr_chan0;
            }
            else 
            {
            	luterr_chan1 = (cyg_uint8) ACCFILT_STD_GET_CTRL_LOWER(errentry.dword);
            }
        }
        else
        {
            // Calc CAN controller number 
            luterr_chan0 = luterr_chan1 = (cyg_uint8) ACCFILT_EXT_GET_CTRL(errentry.dword);
        }
    }
#endif // CYGOPT_DEVS_CAN_LPC2XXX_LUT_ERR_SUPP

    //
    // Loop through all channels - we need to do this only if we have more
    // than one channel so we can optimize here for single channel
    //
#if CYGINT_IO_CAN_CHANNELS > 1 
    cyg_uint8 i = 0;     
    while (lpc2xxx_global_can_info.active_channels[i])
#endif
    {
        cyg_uint32 regval;
        can_channel *chan = LPC2XXX_GET_CAN_CHANNEL(lpc2xxx_global_can_info, i++);
        CAN_DECLARE_INFO(chan);

        HAL_READ_UINT32(CAN_CTRL_ICR(info), regval);      
#ifdef CYGOPT_DEVS_CAN_LPC2XXX_LUT_ERR_SUPP
        // Set ICR_LUT_ERR flag only for controller which cause LUT error
        if ((luterr & 1) && ((luterr_chan0 == i) || (luterr_chan1 == i)))
        {
            regval |= ICR_LUT_ERR;
        } 
#endif // CYGOPT_DEVS_CAN_LPC2XXX_LUT_ERR_SUPP
        regval &= CAN_MISC_INT; // don't care about RX and TX events here
        if (regval)
        {
            chan->callbacks->rcv_event(chan, &regval);
        }
    } // while (lpc2xxx_global_can_info.active_channels[i])
    
    LPC2XXX_DBG_PRINT("err_DSR\n");
    cyg_drv_interrupt_unmask(vector);
}
#else // #ifndef CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY


//===========================================================================
// Global CAN interrupt handler
//===========================================================================
static cyg_uint32 lpc2xxx_can_ISR(cyg_vector_t vector, cyg_addrword_t data)
{   
    //
    // Disable interrupts, the DSR will enable it as soon as it processed
    // the current interrupt
    //
    cyg_drv_interrupt_mask(vector);    
    cyg_drv_interrupt_acknowledge(vector);
    LPC2XXX_DBG_PRINT("CAN_ISR\n");
    return CYG_ISR_CALL_DSR;   
}

//===========================================================================
// Global CAN DSR
//===========================================================================
static void lpc2xxx_can_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{ 
#ifdef CYGOPT_DEVS_CAN_LPC2XXX_LUT_ERR_SUPP
    // If we use acceptance filter we can get LUT error
    cyg_uint32 luterr;
    cyg_uint8  luterr_chan0 = 0xFF; // Init to avoid warnings
    cyg_uint8  luterr_chan1 = 0xFF; // Init to avoid warnings
    
    // Read LUT error flag
    HAL_READ_UINT32(CAN_ACCFILT_LUT_ERR, luterr);
    
    if (luterr & 1)
    {
        cyg_uint32 lutaddr;
        cyg_uint32 eff_sa;
        lsc_buf_t  errentry;
        
        // Read address of failed entry (it clears interrupt flag)
        HAL_READ_UINT32(CAN_ACCFILT_LUT_ERR_ADDR, lutaddr);
        
        // Read address of extended id individual table
        HAL_READ_UINT32(CAN_ACCFILT_EFF_SA, eff_sa); 
        
        // Read error entry
        HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + lutaddr, errentry.dword);
        
        // If errentry from standard id tables then read two controllers numbers
        if(lutaddr < eff_sa)
        {
            // Calc CAN controllers numbers
            luterr_chan0 = (cyg_uint8) ACCFILT_STD_GET_CTRL_UPPER(errentry.dword);
            
            if(errentry.column.lower & ACCFILT_STD_DIS)
            {
            	luterr_chan1 = luterr_chan0;
            }
            else 
            {
            	luterr_chan1 = (cyg_uint8) ACCFILT_STD_GET_CTRL_LOWER(errentry.dword);
            }
        }
        else
        {
            // Calc CAN controller number 
            luterr_chan0 = luterr_chan1 = (cyg_uint8) ACCFILT_EXT_GET_CTRL(errentry.dword);
        }
    }
#endif // CYGOPT_DEVS_CAN_LPC2XXX_LUT_ERR_SUPP
    
    //
    // Walk through list of active CAN channels and process interrupts
    // of all channels - we need to loop only if we have more than one CAN channel so
    // we can optimize for single CAN channel here
    //
#if CYGINT_IO_CAN_CHANNELS > 1
    cyg_uint8 i = 0; 
    while (lpc2xxx_global_can_info.active_channels[i])
#endif // CYGINT_IO_CAN_CHANNELS > 1
    {
        cyg_uint32   icr;
        can_channel *chan = LPC2XXX_GET_CAN_CHANNEL(lpc2xxx_global_can_info, i++);
        CAN_DECLARE_INFO(chan);
        
        HAL_READ_UINT32(CAN_CTRL_ICR(info), icr);      // this read clears ICR     
#ifdef CYGOPT_DEVS_CAN_LPC2XXX_LUT_ERR_SUPP
        // Set ICR_LUT_ERR flag only for controller which cause LUT error
        if ((luterr_chan0 == i) || (luterr_chan1 == i))
        {
            icr |= ICR_LUT_ERR;
        }
#endif // CYGOPT_DEVS_CAN_LPC2XXX_LUT_ERR_SUPP
        //
        // If TX events are supported then we simply call the rcv_event()
        // callback to store the event. If TX events are not supported then
        // we only call the rcv_event() function if any other interrupt than
        // the TX interrupt was captured
        //
#ifndef CYGOPT_IO_CAN_TX_EVENT_SUPPORT  
        if (icr & ~ICR_TX1)
#endif // CYGOPT_IO_CAN_TX_EVENT_SUPPORT 
        {
            chan->callbacks->rcv_event(chan, &icr); 
        }   
        //
        // If this was an TX interrupt then transmit next message now
        //
        if (icr & ICR_TX1)
        { 
            chan->callbacks->xmt_msg(chan, 0); // send next message 
        }
    } // while (lpc2xxx_global_can_info.active_channels[i])   
    LPC2XXX_DBG_PRINT("CAN_DSR\n"); 
    cyg_drv_interrupt_unmask(vector);
}
#endif // #ifndef CYGNUM_DEVS_CAN_LPC2XXX_INT_PRIORITY


#ifdef CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
//===========================================================================
// Configure message boxes for reception of any CAN message
//===========================================================================
static void lpc2xxx_can_config_rx_all(can_channel *chan)
{   
    lpc2xxx_can_info_t *info = (lpc2xxx_can_info_t *)chan->dev_priv;
    //
    // First clear all acceptance filter entries and then insert the
    // two RX all groups
    //
    lpc2xxx_can_config_rx_none(chan); 
#ifdef CYGOPT_IO_CAN_STD_CAN_ID
    lpc2xxx_can_accfilt_add(info, 0x000, 0x7FF, CYGNUM_CAN_ID_STD);
#endif //  CYGOPT_IO_CAN_STD_CAN_ID
#ifdef CYGOPT_IO_CAN_EXT_CAN_ID
    lpc2xxx_can_accfilt_add(info, 0x000, 0x1FFFFFFF, CYGNUM_CAN_ID_EXT);
#endif // CYGOPT_IO_CAN_EXT_CAN_ID   
  
    info->flags  |= INFO_FLAG_RX_ALL;
#ifdef CYGDBG_DEVS_CAN_LPC2XXX_DEBUG
    lpc2xxx_can_accfilt_dbg_dump();
#endif // CYGDBG_DEVS_CAN_LPC2XXX_DEBUG
}
#endif // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG


#ifndef CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
//===========================================================================
// Configure message boxes for reception of any CAN message
//===========================================================================
static void lpc2xxx_can_config_rx_all(can_channel *chan)
{   
    lpc2xxx_can_info_t *info = (lpc2xxx_can_info_t *)chan->dev_priv;

    lpc2xxx_can_accfilt_simple_rx_all();
    info->flags  |= INFO_FLAG_RX_ALL;

#ifdef CYGDBG_DEVS_CAN_LPC2XXX_DEBUG
    lpc2xxx_can_accfilt_dbg_dump();
#endif // CYGDBG_DEVS_CAN_LPC2XXX_DEBUG
}
#endif // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG


//---------------------------------------------------------------------------
// EOF can_lpc2xxx.c
