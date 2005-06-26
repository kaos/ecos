#ifndef CYGONCE_HAL_VAR_INTR_H
#define CYGONCE_HAL_VAR_INTR_H

//=============================================================================
//
//      var_intr.h
//
//      Processor-specific interrupt support
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2005 eCosCentric Ltd.
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   bartv
// Date:        2005-06-26
// Usage:       #include <cyg/hal/hal_intr.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

// The architectural HAL provides the bogomips rating. This is an
// outer loop for the number of us and an inner loop for 1us. The
// alignment of the inner loop can greatly affect performance. The
// *16 of the bogomips rating seems to give about the right results
// on a range of hardware.
#define HAL_DELAY_US(_us_)                                              \
    CYG_MACRO_START                                                     \
    asm volatile (                                                      \
                   "1:\n"                                               \
                   "movl %2,%1\n"                                       \
                   "sal $0x4,%1\n"                                      \
                   ".align 16\n"                                        \
                   "2:\n"                                               \
                   "decl %1\n"                                          \
                   "jns 2b\n"                                           \
                   "decl %0\n"                                          \
                   "jns 1b\n"                                           \
                   :                                                    \
                   : "r" (_us_), "r" (0), "g" (hal_bogomips+1)          \
                   : "cc");                                             \
    CYG_MACRO_END
    
 
//--------------------------------------------------------------------------
#endif // CYGONCE_HAL_VAR_INTR_H
// End of var_intr.h
