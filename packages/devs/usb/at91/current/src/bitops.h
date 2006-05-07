#ifndef CYGONCE_USBS_AT91_BITOPS_H
#define CYGONCE_USBS_AT91_BITOPS_H

//==========================================================================
//
//      bitops.h
//
//      Hardware Bit manipulation macros for the AT91 USB device
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2002 Bart Veer
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Oliver Munz
// Contributors: bartv
// Date:         2006-02-22
//
//####DESCRIPTIONEND####
//==========================================================================

// Set the given bits in a device register
#define SET_BITS(_register_, _bits_)          \
    CYG_MACRO_START                           \
    cyg_uint32 _value_;                       \
    HAL_READ_UINT32(_register_, _value_);     \
    _value_ |= _bits_;                        \
    HAL_WRITE_UINT32(_register_, _value_);    \
    CYG_MACRO_END

// Clear the given bits in a device register
#define CLEAR_BITS(_register_, _bits_)        \
    CYG_MACRO_START                           \
    cyg_uint32 _value_;                       \
    HAL_READ_UINT32(_register_, _value_);     \
    _value_ &= ~_bits_;                       \
    HAL_WRITE_UINT32(_register_, _value_);    \
    CYG_MACRO_END

#define BITS_ARE_SET(_register_, _bits)       \
    bits_are_set(_register_, _bits)

#define BITS_ARE_CLEARED(_register_, _bits)   \
    bits_are_cleared(_register_, _bits)

static inline cyg_bool
bits_are_set (cyg_addrword_t addr, cyg_uint32 bits)
{
  cyg_uint32 read;
  
  HAL_READ_UINT32 (addr, read);
  
  return (read & bits) == bits;
}

static inline cyg_bool
bits_are_cleared (cyg_addrword_t addr, cyg_uint32 bits)
{
  cyg_uint32 read;

  HAL_READ_UINT32 (addr, read);

  return (read | ~bits) == ~bits;
}


#endif // CYGONCE_USBS_AT91_BITOPS_H

