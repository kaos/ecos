#ifndef CYGONCE_HAL_PLATFORM_INTS_H
#define CYGONCE_HAL_PLATFORM_INTS_H
//==========================================================================
//
//      hal_platform_ints.h
//
//      HAL Interrupt and clock assignments for MAC7100/MACE1
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Nick Garnett <nickg@calivar.com>
// Copyright (C) 2996 eCosCentric Ltd
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
// Author(s):    Ilija Koco <ilijak@siva.com.mk>
// Contributors: 
// Date:         2006-04-13
// Purpose:      Define Interrupt support
// Description:  The interrupt specifics for the MAC7100/MACE1 board/platform 
//               are defined here.
//              
// Usage:        #include <cyg/hal/hal_platform_ints.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#define CYGNUM_HAL_ISR_MIN                     0
#define CYGNUM_HAL_ISR_MAX                     63
#define CYGNUM_HAL_ISR_COUNT                   (CYGNUM_HAL_ISR_MAX + 1)

// INTC levels

#define MAC7100_EDMA0_LEVEL        (0)
#define MAC7100_EDMA1_LEVEL        (0)
#define MAC7100_EDMA2_LEVEL        (0)
#define MAC7100_EDMA3_LEVEL        (0)
#define MAC7100_EDMA4_LEVEL        (0)
#define MAC7100_EDMA5_LEVEL        (0)
#define MAC7100_EDMA6_LEVEL        (0)
#define MAC7100_EDMA7_LEVEL        (0)
#define MAC7100_EDMA8_LEVEL        (0)
#define MAC7100_EDMA9_LEVEL        (0)
#define MAC7100_EDMA10_LEVEL       (0)
#define MAC7100_EDMA11_LEVEL       (0)
#define MAC7100_EDMA12_LEVEL       (0)
#define MAC7100_EDMA13_LEVEL       (0)
#define MAC7100_EDMA14_LEVEL       (0)
#define MAC7100_EDMA15_LEVEL       (0)
#define MAC7100_EDMA_Error_LEVEL   (0)
#define MAC7100_MCM_SWT_LEVEL      (0)
#define MAC7100_CRG_LEVEL          (0)
#define MAC7100_PIT1_LEVEL         (9)
#define MAC7100_PIT2_LEVEL         (9)
#define MAC7100_PIT3_LEVEL         (10)
#define MAC7100_PIT4_RTI_LEVEL     (9)
#define MAC7100_VREG_LEVEL         (4)
#define MAC7100_CAN_A_MB_LEVEL     (4)
#define MAC7100_CAN_A_MB14_LEVEL   (4)
#define MAC7100_CAN_A_Error_LEVEL  (4)
#define MAC7100_CAN_B_MB_LEVEL     (4)
#define MAC7100_CAN_B_MB14_LEVEL   (4)
#define MAC7100_CAN_B_Error_LEVEL  (4)
#define MAC7100_CAN_C_MB_LEVEL     (4)
#define MAC7100_CAN_C_MB14_LEVEL   (4)
#define MAC7100_CAN_C_Error_LEVEL  (4)
#define MAC7100_CAN_D_MB_LEVEL     (4)
#define MAC7100_CAN_D_MB14_LEVEL   (4)
#define MAC7100_CAN_D_Error_LEVEL  (4)
#define MAC7100_I2C_LEVEL          (4)
#define MAC7100_DSPI_A_LEVEL       (5)
#define MAC7100_DSPI_B_LEVEL       (5)
#define MAC7100_ESCI_A_LEVEL       (8)
#define MAC7100_ESCI_B_LEVEL       (8)
#define MAC7100_ESCI_C_LEVEL       (8)
#define MAC7100_ESCI_D_LEVEL       (8)
#define MAC7100_EMIOS0_LEVEL       (7)
#define MAC7100_EMIOS1_LEVEL       (7)
#define MAC7100_EMIOS2_LEVEL       (7)
#define MAC7100_EMIOS3_LEVEL       (7)
#define MAC7100_EMIOS4_LEVEL       (7)
#define MAC7100_EMIOS5_LEVEL       (7)
#define MAC7100_EMIOS6_LEVEL       (7)
#define MAC7100_EMIOS7_LEVEL       (7)
#define MAC7100_EMIOS8_LEVEL       (7)
#define MAC7100_EMIOS9_LEVEL       (7)
#define MAC7100_EMIOS10_LEVEL      (7)
#define MAC7100_EMIOS11_LEVEL      (7)
#define MAC7100_EMIOS12_LEVEL      (7)
#define MAC7100_EMIOS13_LEVEL      (7)
#define MAC7100_EMIOS14_LEVEL      (7)
#define MAC7100_EMIOS15_LEVEL      (7)
#define MAC7100_ATD_LEVEL          (11)
#define MAC7100_CFM_LEVEL          (7)
#define MAC7100_PIM_LEVEL          (11)
#define MAC7100_IRQ_LEVEL          (12)
#define MAC7100_XIRQ_LEVEL         (12)

// The vector used by the Real time clock
#if CYGNUM_PIT_CHAN_CLOCK==0
#define CYGNUM_HAL_INTERRUPT_RTC MAC7100_PIT4_RTI_IV
#else
#define CYGNUM_HAL_INTERRUPT_RTC (MAC7100_PIT1_IV+CYGNUM_PIT_CHAN_CLOCK-1)
#endif

//----------------------------------------------------------------------------
// Reset.
__externC void hal_mac7100_reset_cpu(void);
#define HAL_PLATFORM_RESET() hal_mac7100_reset_cpu()

#define HAL_PLATFORM_RESET_ENTRY 0x01000000

#endif // CYGONCE_HAL_PLATFORM_INTS_H
