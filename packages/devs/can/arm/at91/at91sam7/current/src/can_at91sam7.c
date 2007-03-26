//==========================================================================
//
//      devs/can/arm/at91sam7x/current/src/can_at91sam7x.c
//
//      CAN driver for Atmel AT91SAM7X microcontrollers
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
// Date:         2007-01-06
// Purpose:      Support at91sam7 on-chip CAN moduls
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
#include <pkgconf/devs_can_at91sam7.h>

#include <cyg/infra/diag.h>

#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/can.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_io.h>

#include <cyg/hal/hal_diag.h>
#include <cyg/infra/cyg_ass.h>


//===========================================================================
//                                DEFINES  
//===========================================================================

//
// Support debug output if this option is enabled in CDL file
//
#ifdef CYGDBG_DEVS_CAN_AT91SAM7_DEBUG
#define AT91SAM7_DBG_PRINT diag_printf
#else
#define AT91SAM7_DBG_PRINT( fmt, ... )
#endif


//
// we define our own set of register bits in order to be independent from
// platform specific names
//

//---------------------------------------------------------------------------
// Mailbox bits
//
#define BIT_MB0            (0x01 << 0)
#define BIT_MB1            (0x01 << 1)
#define BIT_MB2            (0x01 << 2)
#define BIT_MB3            (0x01 << 3)
#define BIT_MB4            (0x01 << 4)
#define BIT_MB5            (0x01 << 5)
#define BIT_MB6            (0x01 << 6)
#define BIT_MB7            (0x01 << 7)


//---------------------------------------------------------------------------
// CAN Mode Register bits (CAN_MR)
//
#define MR_CAN_ENABLE      (0x01 << 0)
#define MR_LOW_POWER       (0x01 << 1)
#define MR_AUTOBAUD        (0x01 << 2)
#define MR_OVERLOAD        (0x01 << 3)
#define MR_TIMESTAMP_EOF   (0x01 << 4)
#define MR_TIME_TRIG       (0x01 << 5)
#define MR_TIMER_FREEZE    (0x01 << 6)
#define MR_DISABLE_REPEAT  (0x01 << 7)


//---------------------------------------------------------------------------
// CAN Interrupt Enable/Disable, Mask and Status Register bits (CAN_IER, CAN_IDR, CAN_IMR)
//
#define INT_ERR_ACTIVE     (0x01 << 16)
#define INT_WARN           (0x01 << 17)
#define INT_ERR_PASSIVE    (0x01 << 18)
#define INT_BUS_OFF        (0x01 << 19)
#define INT_SLEEP          (0x01 << 20)
#define INT_WAKEUP         (0x01 << 21)
#define INT_TMR_OVF        (0x01 << 22)
#define INT_TIMESTAMP      (0x01 << 23)
#define INT_CRC_ERR        (0x01 << 24)
#define INT_STUFF_ERR      (0x01 << 25)
#define INT_ACKN_ERR       (0x01 << 26)
#define INT_FORM_ERR       (0x01 << 27)
#define INT_BIT_ERR        (0x01 << 28)
#define INT_MB              0xFF        // message box intterupt (mbox 1 - 8)
#define INT_MB_RX           0x7F        // rx message box interrupts
#define INT_MB_TX           0x80        // tx message box interrupts

//
// We do not enable INT_WARN by default because this flug is buggy and causes interrupts
// event if no counter reached warning level.
//
#define INT_ALL_ERR        (INT_CRC_ERR | INT_STUFF_ERR | INT_ACKN_ERR | INT_FORM_ERR | INT_BIT_ERR)
#define INT_DEFAULT        (INT_ERR_PASSIVE | INT_BUS_OFF | INT_SLEEP | INT_WAKEUP | INT_ALL_ERR)


//
// these bits are only in status register (CAN_SR)
//
#define SR_RX_BUSY         (0x01 << 29)
#define SR_TX_BUSY         (0x01 << 30)
#define SR_OVL_BUSY        (0x01 << 31)


//---------------------------------------------------------------------------
// CAN Baudrate Register (CAN_BR)
//
#define BR_PHASE2_BITMASK  0x00000007
#define BR_PHASE1_BITMASK  0x00000070
#define BR_PROPAG_BITMASK  0x00000700
#define BR_SJW_BITMASK     0x00003000
#define BR_BRP_BITMASK     0x007F0000
#define BR_SMP_BITMASK     0x01000000


//---------------------------------------------------------------------------
// CAN Error Counter Register (CAN_ECR)
//
#define ECR_GET_TEC(_ecr_) (((_ecr_) >> 16) & 0xFF)
#define ECR_GET_REC(_ecr_) ((_ecr_) & 0xFF)


//---------------------------------------------------------------------------
// CAN Transfer Command Resgister (CAN_TCR)
//
#define TCR_TMR_RESET      0x80000000


//---------------------------------------------------------------------------
// CAN Message Mode Register (CAN_MMRx)
//
#define MMR_TIMEMARK_BITMASK 0x0000FFFF
#define MMR_PRIOR_BITMASK    0x000F0000

#define MMR_MB_SHIFTER       24
#define MMR_MB_TYPE_BITMASK  (0x07 << MMR_MB_SHIFTER) // mask the mot bits
#define MMR_MB_TYPE_DISABLED (0x00 << MMR_MB_SHIFTER) // message box disabled
#define MMR_MB_TYPE_RX       (0x01 << MMR_MB_SHIFTER) // rx message box
#define MMR_MB_TYPE_RX_OVW   (0x02 << MMR_MB_SHIFTER) // rx message box with overwrite
#define MMR_MB_TYPE_TX       (0x03 << MMR_MB_SHIFTER) // tx message box
#define MMR_MB_TYPE_CONSUME  (0x04 << MMR_MB_SHIFTER) // consumer - receives RTR and sends its content
#define MMR_MB_TYPE_PRODUCE  (0x05 << MMR_MB_SHIFTER) // producer - sends a RTR and waits for answer
#define MMR_MB_GET_TYPE(_mb_) ((_mb_) &  MMR_MB_TYPE_BITMASK)

//---------------------------------------------------------------------------
// CAN Message Acceptance Mask/ID Register (CAN_MAMx, CAN_MIDx)
//
#define MID_MIDvB_BITMASK    0x0003FFFF
#define MID_MIDvA_BITMASK    0x1FFC0000
#define MID_MIDE             0x20000000
#define MID_MIDvA_SHIFTER    18
#define MID_SET_STD(_id_)    (((_id_) << MID_MIDvA_SHIFTER) & MID_MIDvA_BITMASK)
#define MID_SET_EXT(_id_)    ((_id_) | MID_MIDE)
#define MAM_SET_STD          ((((0x7FF << MID_MIDvA_SHIFTER) & MID_MIDvA_BITMASK) | MID_MIDE))
#define MAM_SET_EXT          0xFFFFFFFF
#define MID_GET_STD(_mid_)   (((_mid_) >> MID_MIDvA_SHIFTER) &  CYG_CAN_STD_ID_MASK)
#define MID_GET_EXT(_mid_)   ((_mid_) & CYG_CAN_EXT_ID_MASK)


//---------------------------------------------------------------------------
// CAN Message Status Register (CAN_MSRx)
//
#define MSR_TIMESTAMP      0x0000FFFF
#define MSR_DLC            0x000F0000
#define MSR_RTR            0x00100000
#define MSR_MSG_ABORT      0x00400000
#define MSR_RDY            0x00800000
#define MSR_MSG_IGNORED    0x01000000
#define MSR_DLC_SHIFTER    16
#define MSR_DLC_GET(_msr_) (((_msr_) >> 16) & 0x0F)

//---------------------------------------------------------------------------
// CAN Message Control Register (CAN_MCRx)
//
#define MCR_DLC          0x000F0000 // MDLC
#define MCR_RTR          0x00100000 // MRTR
#define MCR_MSG_ABORT    0x00400000 // MACR
#define MCR_TRANSFER_CMD 0x00800000 // MTCR
#define MCR_DLC_SHIFTER 16
#define MCR_DLC_CREATE(_len_) ((_len_) << MCR_DLC_SHIFTER)

//---------------------------------------------------------------------------
// CAN Module Register Layout
//
#define CANREG_MR         0x0000
#define CANREG_IER        0x0004
#define CANREG_IDR        0x0008
#define CANREG_IMR        0x000C
#define CANREG_SR         0x0010
#define CANREG_BR         0x0014
#define CANREG_TIM        0x0018
#define CANREG_TIMESTAMP  0x001C
#define CANREG_ECR        0x0020
#define CANREG_TCR        0x0024
#define CANREG_ACR        0x0028

#define CANREG_MB_BASE    0x0200

//
// Register layout of message box relativ to base register of a certain
// message box
//
#define CANREG_MMR     0x0000
#define CANREG_MAM     0x0004
#define CANREG_MID     0x0008
#define CANREG_MFID    0x000C
#define CANREG_MSR     0x0010
#define CANREG_MDL     0x0014
#define CANREG_MDH     0x0018
#define CANREG_MCR     0x001C


#define AT91SAM7_CAN_PERIPHERAL_ID 15
#define CAN_MBOX_MIN                0
#define CAN_MBOX_MAX                7
#define CAN_MBOX_CNT                8
#define CAN_MBOX_RX_MIN             0
#define CAN_MBOX_RX_MAX             (CAN_MBOX_MAX - 1) // one message box is tx
#define CAN_MBOX_RX_CNT             (CAN_MBOX_CNT - 1) // one message box is tx 

#define CAN_MR(_extra_)         (CAN_BASE(_extra_) + CANREG_MR)
#define CAN_IER(_extra_)        (CAN_BASE(_extra_) + CANREG_IER)
#define CAN_IDR(_extra_)        (CAN_BASE(_extra_) + CANREG_IDR)
#define CAN_IMR(_etxra_)        (CAN_BASE(_extra_) + CANREG_IMR)
#define CAN_SR(_etxra_)         (CAN_BASE(_extra_) + CANREG_SR)
#define CAN_BR(_etxra_)         (CAN_BASE(_extra_) + CANREG_BR)
#define CAN_TIM(_etxra_)        (CAN_BASE(_extra_) + CANREG_TIM)
#define CAN_TIMESTAMP(_etxra_)  (CAN_BASE(_extra_) + CANREG_TIMESTAMP)
#define CAN_ECR(_etxra_)        (CAN_BASE(_extra_) + CANREG_ECR)
#define CAN_TCR(_etxra_)        (CAN_BASE(_extra_) + CANREG_TCR)
#define CAN_ACR(_etxra_)        (CAN_BASE(_extra_) + CANREG_ACR) 

//
// Message box registers
//
#define CAN_MB_BASE(_extra_)       (CAN_BASE(_extra_) + CANREG_MB_BASE)
#define CAN_MB_MMR(_extra_, _mb_)  (CAN_MB_BASE(_extra_) + 0x0020 * (_mb_) + CANREG_MMR)
#define CAN_MB_MAM(_extra_, _mb_)  (CAN_MB_BASE(_extra_) + 0x0020 * (_mb_) + CANREG_MAM)
#define CAN_MB_MID(_extra_, _mb_)  (CAN_MB_BASE(_extra_) + 0x0020 * (_mb_) + CANREG_MID)
#define CAN_MB_MFID(_extra_, _mb_) (CAN_MB_BASE(_extra_) + 0x0020 * (_mb_) + CANREG_MFID)
#define CAN_MB_MSR(_extra_, _mb_)  (CAN_MB_BASE(_extra_) + 0x0020 * (_mb_) + CANREG_MSR)
#define CAN_MB_MDL(_extra_, _mb_)  (CAN_MB_BASE(_extra_) + 0x0020 * (_mb_) + CANREG_MDL)
#define CAN_MB_MDH(_extra_, _mb_)  (CAN_MB_BASE(_extra_) + 0x0020 * (_mb_) + CANREG_MDH)
#define CAN_MB_MCR(_extra_, _mb_)  (CAN_MB_BASE(_extra_) + 0x0020 * (_mb_) + CANREG_MCR)


//---------------------------------------------------------------------------
// Optimize for the case of a single CAN channel, while still allowing
// multiple channels. At the moment only AT91SAM7 controllers with one
// CAN channel are known.
//
#if CYGINT_DEVS_CAN_AT91SAM7_CAN_CHANNELS == 1

#define CAN_PID(_extra_)            AT91SAM7_CAN_PERIPHERAL_ID
#define CAN_ISRVEC(_extra_)         CAN_PID(_extra_)
#define CAN_ISRPRIO(_extra_)        CYGNUM_DEVS_CAN_AT91SAM7_CAN0_ISR_PRIORITY
#define CAN_BASE(_extra_)           AT91_CAN
#define CAN_DECLARE_INFO(_chan_)
#define CAN_MBOX_TX(_extra_)        CYGNUM_DEVS_CAN_AT91SAM7_CAN0_DEFAULT_TX_MBOX
#define CAN_MBOX_STD_CNT(_extra_)   CYGNUM_DEVS_CAN_AT91SAM7_CAN0_STD_MBOXES
#define CAN_MBOX_EXT_CNT(_extra_)   CYGNUM_DEVS_CAN_AT91SAM7_CAN0_EXT_MBOXES
#define CAN_MBOX_RX_ALL_CNT(_extra) (CAN_MBOX_STD_CNT(_extra_) + CAN_MBOX_EXT_CNT(_extra_))

#ifndef CYGNUM_DEVS_CAN_AT91SAM7_CAN0_STD_MBOXES
#define CYGNUM_DEVS_CAN_AT91SAM7_CAN0_STD_MBOXES 0
#endif

#ifndef CYGNUM_DEVS_CAN_AT91SAM7_CAN0_EXT_MBOXES
#define CYGNUM_DEVS_CAN_AT91SAM7_CAN0_EXT_MBOXES 0
#endif

#else  // #if CYGINT_DEVS_CAN_AT91SAM7_CAN_CHANNELS == 1

#define CAN_PID(_extra_)            ((_extra_)->isrvec)
#define CAN_ISRVEC(_extra_)         ((_extra_)->isrvec)
#define CAN_ISRPRIO(_extra_)        ((_extra_)->isrprio)
#define CAN_BASE(_extra_)           ((_extra_)->base)
#define CAN_DECLARE_INFO(_chan_)    at91sam7_can_info_t *info = (at91sam7_can_info_t *)chan->dev_priv;
#define CAN_MBOX_TX(_extra_)        7 // normally it is always the last mailbox
#define CAN_MBOX_STD_CNT(_extra_)   ((_extra_)->mboxes_std_cnt)
#define CAN_MBOX_EXT_CNT(_extra_)   ((_extra_)->mboxes_ext_cnt)
#define CAN_MBOX_RX_ALL_CNT(_extra) ((_extra_)->mboxes_rx_all_cnt)

#endif // #if CYGINT_DEVS_CAN_AT91SAM7_CAN_CHANNELS == 1


//===========================================================================
//                              DATA TYPES
//===========================================================================
typedef struct at91sam7_can_info_t
{
    cyg_interrupt      interrupt;
    cyg_handle_t       interrupt_handle;
    cyg_uint32         stat;             // buffers status register value between ISR and DSR
    cyg_uint8          free_mboxes;      // number of free message boxes for msg filters and rtr buffers
    bool               rx_all;           // true if reception of call can messages is active
    cyg_can_state      state;            // state of CAN controller      

#if CYGINT_DEVS_CAN_AT91SAM7_CAN_CHANNELS > 1
    cyg_uint32         base;             // Per-bus h/w details
    cyg_uint8          isrpri;           // ISR priority
    cyg_uint8          isrvec;           // ISR vector (peripheral id)
    cyg_uint8          mboxes_std_cnt;   // contains number of standard message boxes available
    cyg_uint8          mboxes_ext_cnt;   // number of message boxes with ext id
    cyg_uint8          mboxes_rx_all_cnt;// number of all available mboxes
#endif
} at91sam7_can_info_t;


//
// at91sam7 info initialisation
//
#if CYGINT_DEVS_CAN_AT91SAM7_CAN_CHANNELS > 1
#define AT91SAM7_CAN_INFO(_l, _base, _isrpri, _isrvec, _std_mboxes, _ext_mboxes) \
at91sam7_can_info_t _l {                                                         \
    state             : CYGNUM_CAN_STATE_STOPPED,                                \
    base              : (_base),                                                 \
    isrpri            : (_isrpri),                                               \
    isrvec            : (_isrvec),                                               \
    mboxes_std_cnt    : (_std_mboxes),                                           \
    mboxes_ext_cnt    : (_ext_mboxes),                                           \
    mboxes_rx_all_cnt : ((_std_mboxes) + (_ext_mboxes)),                         \
};
#else
#define AT91SAM7_CAN_INFO(_l)              \
at91sam7_can_info_t _l = {                 \
    state      : CYGNUM_CAN_STATE_STOPPED, \
};
#endif


//===========================================================================
//                          GLOBAL DATA
//===========================================================================
#if CYGINT_DEVS_CAN_AT91SAM7_CAN_CHANNELS > 1
//
// ToDo - Initialisation of individual CAN channels if more than one channel
// is supported
//
#else // CYGINT_DEVS_CAN_AT91SAM7_CAN_CHANNELS == 1
//
// Only one single CAN channel supported by SAM7 chip
//
AT91SAM7_CAN_INFO(at91sam7_can0_info);
#endif


//===========================================================================
//                          LOCAL DATA
//===========================================================================
//
// Macro for creation of CAN_BR value for baudrate tbl
//

#define CAN_BR_TBL_ENTRY(_brp_, _propag_, _phase1_, _phase2_, _sjw_) \
   ((_brp_ << 16) | (_propag_ << 8) | (_phase2_) | (_phase1_ << 4) | (_sjw_ << 12))

//
// Table with register values for baudrates at main clock of 48 MHz
//
static const cyg_uint32 at91sam7_br_tbl[] =
{
    CAN_BR_TBL_ENTRY(0xef, 0x07, 0x07, 0x02, 0), // 10  kbaud
    CAN_BR_TBL_ENTRY(0x95, 0x04, 0x07, 0x01, 0), // 20  kbaud
    CAN_BR_TBL_ENTRY(0x3b, 0x04, 0x07, 0x01, 0), // 50  kbaud
    CAN_BR_TBL_ENTRY(0x1d, 0x04, 0x07, 0x01, 0), // 100 kbaud
    CAN_BR_TBL_ENTRY(0x17, 0x04, 0x07, 0x01, 0), // 125 kbaud
    CAN_BR_TBL_ENTRY(0x0b, 0x04, 0x07, 0x01, 0), // 250 kbaud
    CAN_BR_TBL_ENTRY(0x05, 0x04, 0x07, 0x01, 0), // 500 kbaud
    CAN_BR_TBL_ENTRY(0x03, 0x03, 0x07, 0x01, 0), // 800 kbaud
    CAN_BR_TBL_ENTRY(0x02, 0x04, 0x07, 0x01, 0), // 1000 kbaud
    CAN_BR_TBL_ENTRY(0x00, 0x00, 0x00, 0x00, 0), // Autobaud
};

//
// Macro fills baudrate register value depending on selected baudrate
// For a standard AT91 clock speed of 48 MHz we provide a pre calculated
// baudrate table. If the board uses another clock speed, then the platform 
// HAL needs to provide an own HAL_AT91SAM7_GET_CAN_BR() macro that returns 
// valid baudrate register values
//
#ifdef CYGNUM_HAL_ARM_AT91_CLOCK_SPEED_48000000
#define HAL_AT91SAM7_GET_CAN_BR(_baudrate_, _br_)                \
CYG_MACRO_START                                                  \
    _br_ = at91sam7_br_tbl[(_baudrate_) - CYGNUM_CAN_KBAUD_10];  \
CYG_MACRO_END
#endif


//===========================================================================
//                              PROTOTYPES
//===========================================================================

//--------------------------------------------------------------------------
// Device driver interface functions
//
static bool        at91sam7_can_init(struct cyg_devtab_entry* devtab_entry);
static Cyg_ErrNo   at91sam7_can_lookup(struct cyg_devtab_entry** tab, struct cyg_devtab_entry* sub_tab, const char* name);
static Cyg_ErrNo   at91sam7_can_set_config(can_channel *chan, cyg_uint32 key, const void* buf, cyg_uint32* len);
static Cyg_ErrNo   at91sam7_can_get_config(can_channel *chan, cyg_uint32 key, const void* buf, cyg_uint32* len);
static bool        at91sam7_can_putmsg(can_channel *priv, CYG_CAN_MSG_T *pmsg, void *pdata);
static bool        at91sam7_can_getevent(can_channel *priv, CYG_CAN_EVENT_T *pevent, void *pdata);
static void        at91sam7_can_start_xmit(can_channel* chan);
static void        at91sam7_can_stop_xmit(can_channel* chan);


//--------------------------------------------------------------------------
// ISRs and DSRs
//
static cyg_uint32 at91sam7_can_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void       at91sam7_can_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);


//--------------------------------------------------------------------------
// Private utility functions
//
static bool at91sam7_can_config_channel(can_channel* chan, cyg_can_info_t* config, cyg_bool init);
static bool at91sam7_can_set_baud(can_channel *chan, cyg_can_baud_rate_t *baudrate);
static void at91sam7_can_mbox_config_rx_all(can_channel *chan);
static void at91sam7_can_setup_mbox(can_channel *chan,    // channel 
                                    cyg_uint8    mbox,    // message box number (0 -7)
                                    cyg_uint32   mid,     // message identifier
                                    cyg_uint32   mam,     // acceptance mask for this message box
                                    cyg_uint32   rxtype); // RX or RX with overwrite are valid values
static void at91sam7_enter_lowpower_mode(can_channel *chan);
static void at91sam7_start_module(can_channel *chan);
static cyg_can_state at91sam7_get_state(at91sam7_can_info_t *info);

#ifdef CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
static void at91sam7_can_config_rx_none(can_channel *chan);
static Cyg_ErrNo at91sam7_can_set_config_msgbuf(can_channel *chan, cyg_can_msgbuf_cfg *buf);
#endif


                                       

//===========================================================================
//                   GENERIC CAN IO DATA INITIALISATION
//===========================================================================
CAN_LOWLEVEL_FUNS(at91sam7_can_lowlevel_funs,
                  at91sam7_can_putmsg,
                  at91sam7_can_getevent,
                  at91sam7_can_get_config,
                  at91sam7_can_set_config,
                  at91sam7_can_start_xmit,
                  at91sam7_can_stop_xmit
     );


CYG_CAN_EVENT_T  at91sam7_can0_rxbuf[CYGNUM_DEVS_CAN_AT91SAM7_CAN0_QUEUESIZE_RX]; // buffer for RX can events
CYG_CAN_MSG_T    at91sam7_can0_txbuf[CYGNUM_DEVS_CAN_AT91SAM7_CAN0_QUEUESIZE_TX]; // buffer for TX can messages


CAN_CHANNEL_USING_INTERRUPTS(at91sam7_can0_chan,
                             at91sam7_can_lowlevel_funs,
                             at91sam7_can0_info,
                             CYG_CAN_BAUD_RATE(CYGNUM_DEVS_CAN_AT91SAM7_CAN0_KBAUD),
                             at91sam7_can0_txbuf, CYGNUM_DEVS_CAN_AT91SAM7_CAN0_QUEUESIZE_TX,
                             at91sam7_can0_rxbuf, CYGNUM_DEVS_CAN_AT91SAM7_CAN0_QUEUESIZE_RX
    );


DEVTAB_ENTRY(at91sam7_can_devtab, 
             CYGPKG_DEVS_CAN_AT91SAM7_CAN0_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_can_devio, 
             at91sam7_can_init, 
             at91sam7_can_lookup,  // CAN driver may need initializing
             &at91sam7_can0_chan
    );


//===========================================================================
//                            IMPLEMENTATION
//===========================================================================



//===========================================================================
/// First initialisation and reset of CAN modul.
//===========================================================================
static bool at91sam7_can_init(struct cyg_devtab_entry* devtab_entry)
{
    can_channel          *chan    = (can_channel*)devtab_entry->priv;
    at91sam7_can_info_t *info    = (at91sam7_can_info_t *)chan->dev_priv;

#ifdef CYGDBG_IO_INIT
    diag_printf("AT91 CAN init\n");
#endif   
    cyg_drv_interrupt_create(CAN_ISRVEC(info),
                             CAN_ISRPRIO(info),        // Priority
                             (cyg_addrword_t)chan,     // Data item passed to interrupt handler
                             at91sam7_can_ISR,
                             at91sam7_can_DSR,
                             &info->interrupt_handle,
                             &info->interrupt);
    cyg_drv_interrupt_attach(info->interrupt_handle);
    cyg_drv_interrupt_unmask(CAN_ISRVEC(info));
     
    return at91sam7_can_config_channel(chan, &chan->config, true);
}


//===========================================================================
//  Lookup the device and return its handle
//===========================================================================
static Cyg_ErrNo at91sam7_can_lookup(struct cyg_devtab_entry** tab, struct cyg_devtab_entry* sub_tab, const char* name)
{
    can_channel* chan    = (can_channel*) (*tab)->priv;
    CAN_DECLARE_INFO(chan);

    chan->callbacks->can_init(chan); 
    HAL_WRITE_UINT32(CAN_IER(info), INT_DEFAULT);                  // enable wakeup and error interrupts
    HAL_WRITE_UINT32(AT91_PMC+AT91_PMC_PCER, 1 << CAN_PID(info));  // Enable the peripheral clock to the device      
     
    //
    // It is important to setup the message buffer configuration after enabling the 
    // peripheral clock. This is nowhere documented in the at91sam7 hardware manual.
    // If the message buffer configuration is set before the peripheral clock is
    // enabled, then message buffers that receive extended frames might not work
    // properly
    //
    at91sam7_can_mbox_config_rx_all(chan); 
      
    return ENOERR;
}


#ifdef CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
//===========================================================================
// Setup AT91SAM7 CAN module in a state where all message boxes are disabled
// After this callit is possible to add single message buffers and filters
//===========================================================================
static void at91sam7_can_config_rx_none(can_channel *chan)
{
    at91sam7_can_info_t *info = (at91sam7_can_info_t *)chan->dev_priv;
    cyg_uint8 i;
    
    //
    // setup all RX messages moxes into a disabled state and disable all
    // interrupts - maybe we have to abort pending transfers before $$$$
    //
    HAL_WRITE_UINT32(CAN_IDR(info), INT_MB_RX);
    for (i = 0; i < CAN_MBOX_RX_CNT; ++i)
    {
        HAL_WRITE_UINT32(CAN_MB_MMR(info, i), MMR_MB_TYPE_DISABLED); // first disable message box
    }
    
    info->free_mboxes = CAN_MBOX_RX_CNT;
    info->rx_all = false;
}


//===========================================================================
// Add single message filter - setupm message box and enable interrupt
//===========================================================================
static void at91sam7_can_add_rx_filter(can_channel *chan, cyg_uint8 mbox, cyg_can_message *msg)
{   
    CAN_DECLARE_INFO(chan);
    
    if (msg->ext)
    {
        at91sam7_can_setup_mbox(chan, mbox, MID_SET_EXT(msg->id), MAM_SET_EXT, MMR_MB_TYPE_RX); 
    }
    else
    {
        at91sam7_can_setup_mbox(chan, mbox, MID_SET_STD(msg->id), MAM_SET_STD, MMR_MB_TYPE_RX);    
    } 
    HAL_WRITE_UINT32(CAN_IER(info), 0x01 << mbox);   
}
#endif // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG


#ifdef CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
//===========================================================================
// Allocate message box
// Try to find a free message box and return its ID
//===========================================================================
static cyg_int8 at91sam7_can_alloc_mbox(at91sam7_can_info_t *info)
{
    cyg_uint8     i;
    cyg_int8      res = CYGNUM_CAN_MSGBUF_NA;
    
    if (info->free_mboxes)
    {  
        for (i = (CAN_MBOX_RX_CNT - info->free_mboxes); i <= CAN_MBOX_RX_MAX; ++i)
        {
            cyg_uint32 mmr;
            HAL_READ_UINT32(CAN_MB_MMR(info, i), mmr);
            if ((mmr & MMR_MB_TYPE_BITMASK) == MMR_MB_TYPE_DISABLED)
            {
                info->free_mboxes--;
                res = i;
                break;
            }             
        }
    } // if (info->free_mboxes)
    
    return res;
}
#endif // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG


#ifdef CYGOPT_IO_CAN_REMOTE_BUF
//===========================================================================
// Setup a RTR response message box
//===========================================================================
static bool at91sam7_can_setup_rtrmbox(can_channel      *chan,
                                       cyg_uint32        mbox,
                                       cyg_can_message  *pmsg,
                                       bool              init)
{
    CAN_DECLARE_INFO(chan);
    cyg_uint32 mcr;

    //
    // To prevent concurrent access with the internal CAN core, the application
    // must disable the mailbox before writing to CAN_MIDx registers - so we
    // do this here
    //
    if (init)
    {
        if (pmsg->ext)
        {
            at91sam7_can_setup_mbox(chan, mbox, MID_SET_EXT(pmsg->id), MAM_SET_EXT, MMR_MB_TYPE_PRODUCE); 
        }
        else
        {
            at91sam7_can_setup_mbox(chan, mbox, MID_SET_STD(pmsg->id), MAM_SET_STD, MMR_MB_TYPE_PRODUCE);    
        }   
        HAL_WRITE_UINT32(CAN_IER(info), 0x01 << mbox); // enable interrupt
    }
    else
    {
        cyg_uint32 msr;
        //
        // Check if this message box is ready for transmission or if it still transmits
        // a message - we read the MSR register to check the ready flag
        //
        HAL_READ_UINT32(CAN_MB_MSR(info, mbox), msr);    
        if (!(msr & MSR_RDY))
        {
            AT91SAM7_DBG_PRINT("(RTR) !MSR_RDY\n");
            return false;
        }
    }
    
    HAL_WRITE_UINT32(CAN_MB_MDL(info, mbox), pmsg->data.dwords[0]); // set data
    HAL_WRITE_UINT32(CAN_MB_MDH(info, mbox), pmsg->data.dwords[1]); // set data
    mcr = (pmsg->dlc << MCR_DLC_SHIFTER) | MCR_TRANSFER_CMD;        // set data lengt and transfer request
    HAL_WRITE_UINT32(CAN_MB_MCR(info, mbox), mcr);                  // transfer request    
    return true;
}
#endif // CYGOPT_IO_CAN_REMOTE_BUF


#ifdef CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
//===========================================================================
// Configure message buffers
//===========================================================================
static Cyg_ErrNo at91sam7_can_set_config_msgbuf(can_channel *chan, cyg_can_msgbuf_cfg *buf)
{
    Cyg_ErrNo             res  = ENOERR;
    at91sam7_can_info_t *info = (at91sam7_can_info_t *)chan->dev_priv;

    switch (buf->cfg_id)
    {
        //
        // clear all message filters and remote buffers - prepare for message buffer
        // configuration
        //
        case CYGNUM_CAN_MSGBUF_RESET_ALL :
             {
                 at91sam7_can_config_rx_none(chan);
             }
             break;

        //
        // setup AT91SAM7 CAN module for reception of all standard and extended messages
        //
        case CYGNUM_CAN_MSGBUF_RX_FILTER_ALL :
             {
                 if (!info->rx_all) // if rx_all is enabled we do not need to do anything
                 {
                    at91sam7_can_mbox_config_rx_all(chan);  // setup RX all state
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
                 // if AT91SAM7 CAN module is configured to receive all messages then 
                 // it is not allowed to add single message filters because then more 
                 // than one message buffer would receive the same CAN id
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
                 filter->handle = at91sam7_can_alloc_mbox(info);
                 if (filter->handle > CYGNUM_CAN_MSGBUF_NA)
                 {
                     at91sam7_can_add_rx_filter(chan, filter->handle, &filter->msg);
                 }
             }
             break; //CYGNUM_CAN_MSGBUF_RX_FILTER_ADD


#ifdef CYGOPT_IO_CAN_REMOTE_BUF
        //
        // Try to add a new RTR response message buffer for automatic transmisson
        // of data frame on reception of a remote frame
        //
        case CYGNUM_CAN_MSGBUF_REMOTE_BUF_ADD :
             {
                 cyg_can_remote_buf *rtr_buf    = (cyg_can_remote_buf*) buf;
                 rtr_buf->handle = at91sam7_can_alloc_mbox(info);
                     
                 if (rtr_buf->handle > CYGNUM_CAN_MSGBUF_NA)
                 {
                     //
                     // if we have a free message buffer then we setup this buffer
                     // for remote frame reception
                     //
                     at91sam7_can_setup_rtrmbox(chan, rtr_buf->handle, &rtr_buf->msg, true);
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
                 if ((rtr_buf->handle >= 0) && (rtr_buf->handle <= CAN_MBOX_RX_MAX))
                 {
                      if (!at91sam7_can_setup_rtrmbox(chan, rtr_buf->handle, &rtr_buf->msg, false))
                      {
                          res = -EAGAIN;
                      }
                 }
                 else
                 {
                    res = -EINVAL;
                 }  
             }
             break;
#endif // #ifdef CYGOPT_IO_CAN_REMOTE_BUF
    } // switch (buf->cfg_id)
    
    return res;
}
#endif // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG


//===========================================================================
// Read state of CAN controller
// The CAN state variable for each channel is modiefied by DSR so if we 
// read the state we need to lock DSRs to protect the data access
//===========================================================================
static cyg_can_state at91sam7_get_state(at91sam7_can_info_t *info)
{
    cyg_can_state result;
    
    cyg_drv_dsr_lock();
    result = info->state;
    cyg_drv_dsr_unlock();
    
    return result;
}


//===========================================================================
// Enter low power mode
// Before stopping the CAN clock (PMC), the CAN Controller must be in 
// Low-power Mode to complete the current transfer. After restarting the 
// clock, the application must disable the Low-power Mode of the 
// CAN controller. If the power mode is entered, a sleep interrupt is 
// generated.
//===========================================================================
static void at91sam7_enter_lowpower_mode(can_channel *chan)
{
    CAN_DECLARE_INFO(chan);
    
    
    cyg_uint32 mr;
    HAL_READ_UINT32(CAN_MR(info), mr);
    HAL_WRITE_UINT32(CAN_MR(info), mr | MR_LOW_POWER); 
    HAL_WRITE_UINT32(CAN_IER(info), INT_SLEEP);
}


//===========================================================================
// Start CAN module (or leave the low power mode)
// If the CAN module is in STANDBY state then we enable the module clock
// and leave the low power mode by clearing the low power flag.
//===========================================================================
static void at91sam7_start_module(can_channel *chan)
{
    CAN_DECLARE_INFO(chan);
    cyg_uint32           mr;
    
    HAL_WRITE_UINT32(CAN_IER(info), INT_DEFAULT);                  // enable wakeup interrupt 
    HAL_WRITE_UINT32(AT91_PMC+AT91_PMC_PCER, 1 << CAN_PID(info));  // restart peripheral clock
    HAL_READ_UINT32(CAN_MR(info), mr);                             
    mr &= ~MR_LOW_POWER ;
    HAL_WRITE_UINT32(CAN_MR(info), mr | MR_CAN_ENABLE);            // clear the low power flag to leave standby     
}

//===========================================================================
// Change device configuration
//===========================================================================
static Cyg_ErrNo at91sam7_can_set_config(can_channel *chan, cyg_uint32 key, const void* buf, cyg_uint32* len)
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
                 if (!at91sam7_can_config_channel(chan, config, false))
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
                cyg_can_msgbuf_cfg *msg_buf = (cyg_can_msgbuf_cfg *)buf;

                if (*len != sizeof(cyg_can_msgbuf_cfg))
                {
                    return -EINVAL;
                }
                
                res = at91sam7_can_set_config_msgbuf(chan, msg_buf);
             }
             break;
#endif // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
             
        //
        // Change CAN state of AT91SAM7 CAN module
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
                    //
                    // The controller does not support a stopped and standby state so we
                    // simply enter the low power state here. This state is also safe for
                    // message buffer configuration
                    //
                    case CYGNUM_CAN_MODE_STOP :    at91sam7_enter_lowpower_mode(chan); break; 
                    case CYGNUM_CAN_MODE_START :   at91sam7_start_module(chan);        break;                       
                    case CYGNUM_CAN_MODE_STANDBY : at91sam7_enter_lowpower_mode(chan); break;
                    case CYGNUM_CAN_MODE_CONFIG :  at91sam7_enter_lowpower_mode(chan); break;
                }
             }
             break; // case CYG_IO_SET_CONFIG_CAN_MODE :         
    } // switch (key)
    
    return res;
}


//===========================================================================
// Query device configuration
//===========================================================================
static Cyg_ErrNo at91sam7_can_get_config(can_channel *chan, cyg_uint32 key, const void* buf, cyg_uint32* len)
{
    Cyg_ErrNo            res  = ENOERR;
    at91sam7_can_info_t *info = (at91sam7_can_info_t *)chan->dev_priv;
    
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
                *can_state = at91sam7_get_state(info);
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
                *len = sizeof(cyg_can_msgbuf_info);
                
                 mbox_info->count = CAN_MBOX_RX_CNT;
                 mbox_info->free  = info->free_mboxes;
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
                                   | CYGNUM_CAN_HDI_FULLCAN
                                   | CYGNUM_CAN_HDI_AUTBAUD;
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
// Send single message
//===========================================================================
static bool at91sam7_can_putmsg(can_channel *priv, CYG_CAN_MSG_T *pmsg, void *pdata)
{
    CAN_DECLARE_INFO(priv);
    cyg_uint32            msr;   
    cyg_uint32            mcr = 0;   
    
    //
    // First check if this message box is ready fro transmission or if it still transmits
    // a message - we read the MSR register to check the ready flag
    //
    HAL_READ_UINT32(CAN_MB_MSR(info, CAN_MBOX_TX(info)), msr);    
    if (!(msr & MSR_RDY))
    {
        AT91SAM7_DBG_PRINT("!MSR_RDY\n");
        return false;
    }
    
    //
    // To prevent concurrent access with the internal CAN core, the application must disable 
    // the mailbox before writing to CAN_MIDx registers - so we do this now
    // 
    HAL_WRITE_UINT32(CAN_MB_MMR(info, CAN_MBOX_TX(info)), MMR_MB_TYPE_DISABLED); 
    
    //
    // Setup the message identifier - this depends on the frame type (standard or extended)
    //
#ifdef CYGOPT_IO_CAN_EXT_CAN_ID
    if (AT91SAM7_CAN_IS_EXT(*pmsg))
    {
        HAL_WRITE_UINT32(CAN_MB_MID(info, CAN_MBOX_TX(info)), 
                         pmsg->id | MID_MIDE);                                   // set extended message id
    }
    else
#endif // CYGOPT_IO_CAN_EXT_CAN_ID
    {
#ifdef CYGOPT_IO_CAN_STD_CAN_ID
        HAL_WRITE_UINT32(CAN_MB_MID(info, CAN_MBOX_TX(info)), 
                        (pmsg->id << MID_MIDvA_SHIFTER) & MID_MIDvA_BITMASK);    // set standard message id
#endif // CYGOPT_IO_CAN_STD_CAN_ID  
    }

    HAL_WRITE_UINT32(CAN_MB_MDL(info, CAN_MBOX_TX(info)), pmsg->data.dwords[0]); // set data
    HAL_WRITE_UINT32(CAN_MB_MDH(info, CAN_MBOX_TX(info)), pmsg->data.dwords[1]); // set data
    HAL_WRITE_UINT32(CAN_MB_MMR(info, CAN_MBOX_TX(info)), MMR_MB_TYPE_TX);       // reenable the message box
    mcr = (AT91SAM7_CAN_GET_DLC(*pmsg) << MCR_DLC_SHIFTER) | MCR_TRANSFER_CMD;   // set data lengt and transfer request
    
    if (AT91SAM7_CAN_IS_RTR(*pmsg))
    {
        mcr |= MCR_RTR;
    }
    
    HAL_WRITE_UINT32(CAN_MB_MCR(info, CAN_MBOX_TX(info)), mcr);
    return true;
}


//===========================================================================
// Read event from device driver
//===========================================================================
static bool at91sam7_can_getevent(can_channel *chan, CYG_CAN_EVENT_T *pevent, void *pdata)
{
    at91sam7_can_info_t  *info       = (at91sam7_can_info_t *)chan->dev_priv;
    cyg_uint32*           pstat      = (cyg_uint32 *)pdata;
    cyg_uint8             mboxflags  = (*pstat & INT_MB_RX);
    cyg_uint8             mbox       = 0;
    bool                  res        = true;
    
    //
    // First check if a message box interrupt occured if a message box interrupt
    // occured process the lowest message box that caused an interrupt
    //
    if (mboxflags)
    {
        cyg_uint32 msr;
        cyg_uint32 mid;
        cyg_uint32 mmr;
        
        while (!(mboxflags & 0x01))
        {
            mboxflags >>= 1;
            mbox++;
        }
        
        //
        // If the message box that caused the interrupt is an PRODUCER message box,
        // then we received an remote request message, if not, then this is a normal
        // RX message
        //
        HAL_READ_UINT32(CAN_MB_MMR(info, mbox), mmr);
        HAL_READ_UINT32(CAN_MB_MSR(info, mbox), msr);   
       *pstat &= ~(0x01 << mbox);                                 // clear flag
        
        if (MMR_MB_GET_TYPE(mmr) != MMR_MB_TYPE_PRODUCE)
        {
            HAL_READ_UINT32(CAN_MB_MID(info, mbox), mid);
            pevent->flags |= CYGNUM_CAN_EVENT_RX; 
            if (msr & MSR_MSG_IGNORED)
            {
                pevent->flags |= CYGNUM_CAN_EVENT_OVERRUN_RX;
            }
            
            //
            // It is important to set the DLC first because this also clears the ctrl
            // field if extended identifiers are supported
            //
            AT91SAM7_CAN_SET_DLC(pevent->msg, MSR_DLC_GET(msr));  
            
#ifdef CYGOPT_IO_CAN_EXT_CAN_ID     
            if (mid & MID_MIDE)
            {  
                pevent->msg.id = MID_GET_EXT(mid);
                AT91SAM7_CAN_SET_EXT(pevent->msg);
            }
            else
#endif // CYGOPT_IO_CAN_EXT_CAN_ID
            {
#ifdef CYGOPT_IO_CAN_STD_CAN_ID
                pevent->msg.id = MID_GET_STD(mid);
#endif // CYGOPT_IO_CAN_STD_CAN_ID
            }
            
            if (msr & MSR_RTR)
            {
                AT91SAM7_CAN_SET_RTR(pevent->msg);
            }
            else
            {
                HAL_READ_UINT32(CAN_MB_MDL(info, mbox), pevent->msg.data.dwords[0]);
                HAL_READ_UINT32(CAN_MB_MDH(info, mbox), pevent->msg.data.dwords[1]);
            }
#ifdef CYGOPT_IO_CAN_SUPPORT_TIMESTAMP 
            pevent->timestamp = msr & MSR_TIMESTAMP;
#endif
      
            HAL_WRITE_UINT32(CAN_MB_MCR(info, mbox), MCR_TRANSFER_CMD);                  // transfer request        
            AT91SAM7_DBG_PRINT("RXID: %x\n", AT91SAM7_CAN_GET_ID(pevent->msg));
        } // if (!(mbox & info->rtr_mboxes)
        else
        {
            HAL_WRITE_UINT32(CAN_MB_MCR(info, mbox), (msr & MSR_DLC) | MCR_TRANSFER_CMD); // transfer request 
            //
            // We do not need to store an event into receive queue if the stat field does
            // not contain any further event flags. If stat is empty we can set res
            // to false and no event will bestore
            //
            res = !(*pstat == 0);
        }
        
        HAL_WRITE_UINT32(CAN_IER(info), 0x01 << mbox);                  // enable interruptfor this message box
    } // if (mboxflags)
    
    //
    // Now check if additional events occured
    //
    if (*pstat)
    {
        if (*pstat & INT_WAKEUP)
        {
            AT91SAM7_DBG_PRINT("WAKE\n");
            pevent->flags |= CYGNUM_CAN_EVENT_LEAVING_STANDBY;
            *pstat &= ~INT_WAKEUP;
            info->state = CYGNUM_CAN_STATE_ACTIVE;       
        }
    
        if (*pstat & INT_ERR_PASSIVE)
        {
            AT91SAM7_DBG_PRINT("ERRP\n");
            pevent->flags |= CYGNUM_CAN_EVENT_ERR_PASSIVE;  
            *pstat &= ~INT_ERR_PASSIVE;
            info->state = CYGNUM_CAN_STATE_ERR_PASSIVE; 
            HAL_WRITE_UINT32(CAN_IER(info), INT_WAKEUP); 
        }
        
        if (*pstat & INT_WARN)
        {
            //
            // check which counter reached its warning level (> 96)
            //
            cyg_uint8 ecr;
            HAL_READ_UINT32(CAN_ECR(info), ecr);
            if (ECR_GET_REC(ecr) > 96)
            {
                pevent->flags |= CYGNUM_CAN_EVENT_WARNING_RX;
                AT91SAM7_DBG_PRINT("WARN TX\n");    
            }
            if (ECR_GET_TEC(ecr) > 96)
            {
                pevent->flags |= CYGNUM_CAN_EVENT_WARNING_TX;
                AT91SAM7_DBG_PRINT("WARN RX\n"); 
            }
            *pstat &= ~INT_WARN;
            info->state = CYGNUM_CAN_STATE_BUS_WARN;
            HAL_WRITE_UINT32(CAN_IER(info), INT_ERR_PASSIVE | INT_BUS_OFF); 
        }
        
        if (*pstat & INT_BUS_OFF)
        {
            pevent->flags |= CYGNUM_CAN_EVENT_BUS_OFF;  
            AT91SAM7_DBG_PRINT("BOFF\n");
            *pstat &= ~INT_BUS_OFF;
            info->state = CYGNUM_CAN_STATE_BUS_OFF;
            HAL_WRITE_UINT32(CAN_IER(info), INT_WAKEUP); 
        }
        
        if (*pstat & INT_SLEEP)
        {
            pevent->flags |= CYGNUM_CAN_EVENT_ENTERING_STANDBY;
            AT91SAM7_DBG_PRINT("SLEEP\n");
            *pstat &= ~INT_SLEEP;
            HAL_WRITE_UINT32(AT91_PMC+AT91_PMC_PCDR, 1 << CAN_PID(info)); // disable module clock
            info->state = CYGNUM_CAN_STATE_STANDBY;                       // set state variable
            HAL_WRITE_UINT32(CAN_IER(info), INT_WAKEUP);                  // enable wakeup interrupt
        }
        
        if (*pstat & (INT_CRC_ERR | INT_STUFF_ERR | INT_ACKN_ERR | INT_FORM_ERR | INT_BIT_ERR))
        {
            pevent->flags |= CYGNUM_CAN_EVENT_CAN_ERR;
            AT91SAM7_DBG_PRINT("CERR\n");
            *pstat &= ~(INT_CRC_ERR | INT_STUFF_ERR | INT_ACKN_ERR | INT_FORM_ERR | INT_BIT_ERR);  
        }
    } // if (*pstat)
      
    return res;
}


//===========================================================================
// Kick transmitter
//===========================================================================
static void at91sam7_can_start_xmit(can_channel* chan)
{
    CAN_DECLARE_INFO(chan);
    
    AT91SAM7_DBG_PRINT("start_xmit\n");
    cyg_drv_dsr_lock();
    HAL_WRITE_UINT32(CAN_IER(info), 0x01 << CAN_MBOX_TX(info)); // enable tx interrupt
    cyg_drv_dsr_unlock();
}


//===========================================================================
// Stop transmitter
//===========================================================================
static void at91sam7_can_stop_xmit(can_channel* chan)
{
     CAN_DECLARE_INFO(chan);
    
     HAL_WRITE_UINT32(CAN_IDR(info), 0x01 << CAN_MBOX_TX(info)); // disable tx interrupt 
     AT91SAM7_DBG_PRINT("stop_xmit\n");   
}


//===========================================================================
// Configure can channel
//===========================================================================
static bool at91sam7_can_config_channel(can_channel* chan, cyg_can_info_t* config, cyg_bool init)
{
    CAN_DECLARE_INFO(chan);
    cyg_uint32 temp32;
    bool       res = true;
    
    if (init)
    {
    	//
    	// If the platform that uses the driver needs to do some platform specific
    	// initialisation steps, it can do it inside of this macro. I.e. some platforms
    	// need to setup the CAN transceiver properly here (this is necessary for the
    	// Atmel AT91SAM7X-EK)
    	//
#if CYGINT_DEVS_CAN_AT91SAM7_CAN_CHANNELS == 1 && defined(HAL_AT91SAM7_CAN0_PLF_INIT)
        HAL_AT91SAM7_CAN0_PLF_INIT();
#else // CYGINT_DEVS_CAN_AT91SAM7_CAN_CHANNELS > 1
#if defined(CYGPKG_DEVS_CAN_AT91SAM7_CAN0) && defined(HAL_AT91SAM7_CAN0_PLF_INIT)
        if (info == &at91sam7_can0_info) {
            HAL_AT91SAM7_CAN0_PLF_INIT();
        }
#endif // defined(CYGPKG_DEVS_CAN_AT91SAM7_CAN0) && defined(HAL_AT91SAM7_CAN0_PLF_INIT)
#if defined(CYGPKG_DEVS_CAN_AT91SAM7_CAN1) && defined(HAL_AT91SAM7_CAN1_PLF_INIT)
        if (info == &at91sam7_can1_info) {
            HAL_AT91SAM7_CAN1_PLF_INIT();
        }
#endif // defined(CYGPKG_DEVS_CAN_AT91SAM7_CAN0) && defined(HAL_AT91SAM7_CAN0_PLF_INIT)
#endif // CYGINT_DEVS_CAN_AT91SAM7_CAN_CHANNELS == 1

        HAL_WRITE_UINT32(CAN_IDR(info), 0xFFFFFFFF);   // disable all interrupts
        HAL_WRITE_UINT32(CAN_MR(info), 0x00);          // disable CAN module
        HAL_ARM_AT91_PIO_CFG(AT91_CAN_CANRX);          // Enable the CAN module to drive the CAN port pins
        HAL_ARM_AT91_PIO_CFG(AT91_CAN_CANTX);      
            
        HAL_WRITE_UINT32(CAN_MB_MMR(info, CAN_MBOX_TX(info)), MMR_MB_TYPE_DISABLED); // first disable tx message box
        HAL_WRITE_UINT32(CAN_MB_MAM(info, CAN_MBOX_TX(info)), 0x00000000);           // set acceptance mask once
        HAL_WRITE_UINT32(CAN_MB_MMR(info, CAN_MBOX_TX(info)), MMR_MB_TYPE_TX);       // setup as tx message box
        
        HAL_WRITE_UINT32(CAN_MR(info), MR_CAN_ENABLE); // enable CAN module  
        
        //
        // The device should go into error active state right after enabling it
        //
        HAL_READ_UINT32(CAN_SR(info), temp32);
        if (!(temp32 & INT_ERR_ACTIVE))
        {
            res = false;
        }   
    } // if (init)
    
    res = at91sam7_can_set_baud(chan, &config->baud);        // set baudrate
            
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
// Low level interrupt handler
//===========================================================================
static cyg_uint32 at91sam7_can_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    can_channel                 *chan    = (can_channel *)data;
    at91sam7_can_info_t * const info = (at91sam7_can_info_t *)chan->dev_priv;
    cyg_uint32                   sr;
    cyg_uint32                   imr;
    
    
    HAL_READ_UINT32(CAN_IMR(info), imr);
    HAL_READ_UINT32(CAN_SR(info), sr);
    AT91SAM7_DBG_PRINT("CAN_ISR SR %x\n", sr);   
    sr &= imr;
    HAL_WRITE_UINT32(CAN_IDR(info), sr);
   
    info->stat |= sr;
    cyg_drv_interrupt_acknowledge(vector);
    return CYG_ISR_CALL_DSR;
}


//===========================================================================
// High level interrupt handler
//===========================================================================
static void at91sam7_can_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    can_channel                 *chan    = (can_channel *)data;
    at91sam7_can_info_t * const info = (at91sam7_can_info_t *)chan->dev_priv;
    cyg_uint32                   stat = 0;
       
    do
    {   
        //
        // If a number of events occured then we process all events now in
        // in this DSR the get_event() function clears the flags in the stat
        // field
        //
        while (stat)
        {
            if (stat & (0x01 << CAN_MBOX_TX(info)))
            {
                AT91SAM7_DBG_PRINT("TX_DSR\n");    
                chan->callbacks->xmt_msg(chan, 0);   // send next message 
                stat &= ~INT_MB_TX;                  // clear flag
            }
            else if (stat)
            {
                AT91SAM7_DBG_PRINT("EVENT_DSR\n");   
                chan->callbacks->rcv_event(chan, &stat);
            }
        }
        
        //
        // We check, if a new event occured while we processed other events. If new events
        // occured, then we process the new events
        //
        cyg_drv_interrupt_mask(vector);
        stat = info->stat;
        info->stat = 0;
        cyg_drv_interrupt_unmask(vector);
    } while (stat);
}


//===========================================================================
// Set baudrate of certain can channel
//===========================================================================
static bool at91sam7_can_set_baud(can_channel *chan, cyg_can_baud_rate_t *baudrate)
{
    bool                  res = true;
    cyg_uint32            mrbck;
    cyg_uint32            canbr;
    CAN_DECLARE_INFO(chan);


#ifdef CYGOPT_IO_CAN_AUTOBAUD  
    if (CYGNUM_CAN_KBAUD_AUTO == *baudrate)
    {   
        cyg_can_baud_rate_t   i;
        cyg_uint8             j;
        cyg_uint32            sr;
        
        res = false;
        for (i = CYGNUM_CAN_KBAUD_10; i <= CYGNUM_CAN_KBAUD_1000; ++i)
        {
            HAL_AT91SAM7_GET_CAN_BR(i, canbr);
            if (0 == canbr)
            {
                continue;
            }  
                      
            HAL_READ_UINT32(CAN_SR(info), sr);
            HAL_WRITE_UINT32(CAN_MR(info), 0);                            // disable the module
            HAL_WRITE_UINT32(CAN_BR(info), canbr);                        // write baudrate register
            HAL_WRITE_UINT32(CAN_MR(info), MR_CAN_ENABLE | MR_AUTOBAUD);  // enable controller in auto aud mode
            for(j = 0; j < 200; ++j)
            {
                HAL_DELAY_US(1000);                                       // wait at least 11 bit times for synchronization
            }
            HAL_READ_UINT32(CAN_SR(info), sr);                            // read status register
            if (!(sr & INT_ALL_ERR) && (sr & INT_WAKEUP))
            {
                HAL_WRITE_UINT32(CAN_MR(info), 0);                        // disable the module 
                HAL_WRITE_UINT32(CAN_MR(info), MR_CAN_ENABLE);            // enable controller
                *baudrate = i;                                            // store baudrate
                return true;
            } // if (!(sr & INT_ALL_ERR))         
        }
    }
    else
#endif // CYGOPT_IO_CAN_AUTOBAUD 
    { 
        //
        // Get bit timings from HAL because bit timings depend on sysclock
        // For main clock of 48 MHz this macro is implemented in this device
        // driver. If the macro fills the canbr value with 0 then the baudrate
        // is not supported and the function returns false
        //
        HAL_AT91SAM7_GET_CAN_BR(*baudrate, canbr);   
        if (0 == canbr)
        {
            return false;
        }
        
        //
        // Any modificatons to the baudrate register must be done while CAN
        // module is disabled. So we first disable CAN module, then we set
        // baudrate and then we reenable the CAN module by setting the CAN enable
        // flag
        //
        HAL_READ_UINT32(CAN_MR(info), mrbck);                   // backup value of mode register
        HAL_WRITE_UINT32(CAN_MR(info), mrbck &~MR_CAN_ENABLE);  // disable controller
        HAL_WRITE_UINT32(CAN_BR(info), canbr);                  // write baudrate register
        
        //
        // Now restore the previous state - if the module was started then
        // it will no be started again, if it was stopped, then it remains stopped
        //
        HAL_WRITE_UINT32(CAN_MR(info), mrbck);
    }
    
    return res;
}


//===========================================================================
// Setup one single message box for reception of can message
//===========================================================================
static void at91sam7_can_setup_mbox(can_channel *chan, cyg_uint8 mbox, cyg_uint32 mid, cyg_uint32 mam, cyg_uint32 rxtype)
{
    CAN_DECLARE_INFO(chan);
    CYG_ASSERT(mbox < 7, "invalid rx mbox number");
    
  
    //
    // To prevent concurrent access with the internal CAN core, the application
    // must disable the mailbox before writing to CAN_MIDx registers - so we
    // do this here
    //
    HAL_WRITE_UINT32(CAN_MB_MMR(info, mbox), MMR_MB_TYPE_DISABLED); // first disable message box
    HAL_WRITE_UINT32(CAN_MB_MAM(info, mbox), mam);                  // set acceptance mask
    HAL_WRITE_UINT32(CAN_MB_MID(info, mbox), mid);                  // set message identifier                          
    HAL_WRITE_UINT32(CAN_MB_MMR(info, mbox), rxtype);               // setup message box as rx message box (with or without overwrite)
    HAL_WRITE_UINT32(CAN_MB_MCR(info, mbox), MCR_TRANSFER_CMD);     // transfer request - we do not enable interrupts here
}


//===========================================================================
// Configure message boxes for reception of any CAN message
//===========================================================================
static void at91sam7_can_mbox_config_rx_all(can_channel *chan)
{
    at91sam7_can_info_t * const info = (at91sam7_can_info_t *)chan->dev_priv;
    cyg_uint8  i;
    cyg_uint8  mbox_int_mask    = 0;
    cyg_uint8  mbox_rx_all_cnt  = CAN_MBOX_RX_ALL_CNT(info);
#ifdef CYGOPT_IO_CAN_STD_CAN_ID 
    cyg_uint8  last_std_rx_mbox = CAN_MBOX_STD_CNT(info) - 1;
#endif // CYGOPT_IO_CAN_STD_CAN_ID
#ifdef CYGOPT_IO_CAN_EXT_CAN_ID
    cyg_uint8  last_ext_rx_mbox = mbox_rx_all_cnt - 1;
#endif// CYGOPT_IO_CAN_EXT_CAN_ID 

    //
    // Now setup all rx message boxes. One message box (the last one - no 8) is
    // used for transmission so we have 7 message boxes for reception of can messages
    // We setup the message boxes 0 - 5 as RX mboxes and message box 6 as RX mbox with
    // overwrite. 
    //    
    for (i = 0; i < mbox_rx_all_cnt; ++i)
    { 
#ifdef CYGOPT_IO_CAN_STD_CAN_ID     
        if (i < CAN_MBOX_STD_CNT(info))
        {
            //
            // setup message boxes for standard frames
            //
            if (i < last_std_rx_mbox) 
            {
                at91sam7_can_setup_mbox(chan, i, 0, MID_MIDE, MMR_MB_TYPE_RX);
            }
            else
            {
                at91sam7_can_setup_mbox(chan, i, 0, MID_MIDE, MMR_MB_TYPE_RX_OVW);
            }
        }
        else
#endif // CYGOPT_IO_CAN_STD_CAN_ID 
        {
#ifdef CYGOPT_IO_CAN_EXT_CAN_ID
            //
            // setup message boxes for extended frames
            //
            if (i < last_ext_rx_mbox)
            {
                at91sam7_can_setup_mbox(chan, i, MID_MIDE, MID_MIDE, MMR_MB_TYPE_RX);
            }
            else
            {
                at91sam7_can_setup_mbox(chan, i, MID_MIDE, MID_MIDE, MMR_MB_TYPE_RX_OVW);
            }
#endif// CYGOPT_IO_CAN_EXT_CAN_ID 
        } // if (i < CAN_MBOX_STD_CNT(info))
        
        mbox_int_mask = (mbox_int_mask << 1) | 0x01; // enable interrupt 
    } // for (i = 0; i < CAN_MBOX_RX_CNT; ++i)*/
    
    info->free_mboxes = CAN_MBOX_RX_CNT - mbox_rx_all_cnt;
    info->rx_all      = true;
    HAL_WRITE_UINT32(CAN_IER(info), mbox_int_mask); // Now finally enable the interrupts for als RX mboxes
}


//---------------------------------------------------------------------------
// EOF can_at91am7.c
