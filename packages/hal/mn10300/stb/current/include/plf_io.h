#ifndef CYGONCE_PLF_IO_H
#define CYGONCE_PLF_IO_H

//=============================================================================
//
//      plf_io.h
//
//      Platform specific IO support
//
//=============================================================================
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Contributors: dhowells
// Date:         2001-08-02
// Purpose:      STB platform IO support
// Description:
// Usage:        #include <cyg/hal/plf_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#ifdef __ASSEMBLER__
#define HAL_REG_8(x)              x
#define HAL_REG_16(x)             x
#define HAL_REG_32(x)             x
#else
#define HAL_REG_8(x)              (volatile cyg_uint8*)(x)
#define HAL_REG_16(x)             (volatile cyg_uint16*)(x)
#define HAL_REG_32(x)             (volatile cyg_uint32*)(x)
#endif

//-----------------------------------------------------------------------------

/* STB GPIO Registers */
#define HAL_GPIO_BASE                           0xDB000000

#define HAL_GPIO_0_MODE_OFFSET                  0x0000
#define HAL_GPIO_0_IN_OFFSET                    0x0004
#define HAL_GPIO_0_OUT_OFFSET                   0x0008
#define HAL_GPIO_1_MODE_OFFSET                  0x0100
#define HAL_GPIO_1_IN_OFFSET                    0x0104
#define HAL_GPIO_1_OUT_OFFSET                   0x0108
#define HAL_GPIO_2_MODE_OFFSET                  0x0200
#define HAL_GPIO_2_IN_OFFSET                    0x0204
#define HAL_GPIO_2_OUT_OFFSET                   0x0208
#define HAL_GPIO_3_MODE_OFFSET                  0x0300
#define HAL_GPIO_3_IN_OFFSET                    0x0304
#define HAL_GPIO_3_OUT_OFFSET                   0x0308
#define HAL_GPIO_4_MODE_OFFSET                  0x0400
#define HAL_GPIO_4_IN_OFFSET                    0x0404
#define HAL_GPIO_4_OUT_OFFSET                   0x0408
#define HAL_GPIO_5_MODE_OFFSET                  0x0500
#define HAL_GPIO_5_IN_OFFSET                    0x0504
#define HAL_GPIO_5_OUT_OFFSET                   0x0508

#define HAL_GPIO_0_MODE                         HAL_REG_16 (HAL_GPIO_BASE + HAL_GPIO_0_MODE_OFFSET)
#define HAL_GPIO_0_IN                           HAL_REG_8  (HAL_GPIO_BASE + HAL_GPIO_0_IN_OFFSET)
#define HAL_GPIO_0_OUT                          HAL_REG_8  (HAL_GPIO_BASE + HAL_GPIO_0_OUT_OFFSET)
#define HAL_GPIO_1_MODE                         HAL_REG_16 (HAL_GPIO_BASE + HAL_GPIO_1_MODE_OFFSET)
#define HAL_GPIO_1_IN                           HAL_REG_8  (HAL_GPIO_BASE + HAL_GPIO_1_IN_OFFSET)
#define HAL_GPIO_1_OUT                          HAL_REG_8  (HAL_GPIO_BASE + HAL_GPIO_1_OUT_OFFSET)
#define HAL_GPIO_2_MODE                         HAL_REG_16 (HAL_GPIO_BASE + HAL_GPIO_2_MODE_OFFSET)
#define HAL_GPIO_2_IN                           HAL_REG_8  (HAL_GPIO_BASE + HAL_GPIO_2_IN_OFFSET)
#define HAL_GPIO_2_OUT                          HAL_REG_8  (HAL_GPIO_BASE + HAL_GPIO_2_OUT_OFFSET)
#define HAL_GPIO_3_MODE                         HAL_REG_16 (HAL_GPIO_BASE + HAL_GPIO_3_MODE_OFFSET)
#define HAL_GPIO_3_IN                           HAL_REG_8  (HAL_GPIO_BASE + HAL_GPIO_3_IN_OFFSET)
#define HAL_GPIO_3_OUT                          HAL_REG_8  (HAL_GPIO_BASE + HAL_GPIO_3_OUT_OFFSET)
#define HAL_GPIO_4_MODE                         HAL_REG_16 (HAL_GPIO_BASE + HAL_GPIO_4_MODE_OFFSET)
#define HAL_GPIO_4_IN                           HAL_REG_8  (HAL_GPIO_BASE + HAL_GPIO_4_IN_OFFSET)
#define HAL_GPIO_4_OUT                          HAL_REG_8  (HAL_GPIO_BASE + HAL_GPIO_4_OUT_OFFSET)
#define HAL_GPIO_5_MODE                         HAL_REG_16 (HAL_GPIO_BASE + HAL_GPIO_5_MODE_OFFSET)
#define HAL_GPIO_5_IN                           HAL_REG_8  (HAL_GPIO_BASE + HAL_GPIO_5_IN_OFFSET)
#define HAL_GPIO_5_OUT                          HAL_REG_8  (HAL_GPIO_BASE + HAL_GPIO_5_OUT_OFFSET)

//-----------------------------------------------------------------------------
#define HAL_LED_ADDRESS                         0x83f90000
#define HAL_GPIO_MODE_ALL_OUTPUT                0x5555


#ifdef __ASSEMBLER__

#  include <cyg/hal/platform.inc>
#  define DEBUG_DISPLAY(hexdig)   hal_diag_led hexdig
#  define DEBUG_DELAY()                                        \
     mov    0x20000, d0;                                       \
0:   sub    1, d0;                                             \
     bne    0b;                                                \
     nop

#else

extern cyg_uint8 cyg_hal_plf_led_val(CYG_WORD hexdig);
#  define DEBUG_DISPLAY(hexdig) HAL_WRITE_UINT8(HAL_LED_ADDRESS, cyg_hal_plf_led_val(hexdig))
#  define DEBUG_DELAY()                                        \
   {                                                           \
     volatile int i = 0x80000;                                 \
     while (--i) ;                                             \
   }

#endif

//-----------------------------------------------------------------------------
// end of plf_io.h
#endif // CYGONCE_PLF_IO_H
