#ifndef CYGONCE_HAL_VAR_REGS_H
#define CYGONCE_HAL_VAR_REGS_H

//==========================================================================
//
//      var_regs.h
//
//      ColdFire MCF5272 variant CPU definitions
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Ltd.
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
// Author(s):    Enrico Piria
// Contributors:
// Date:         2005-25-06
// Purpose:      Provide register definitions for the MCF5272.
// Description:
// Usage:        Included via "coldfire_regs.h". Do not use directly.
//
//####DESCRIPTIONEND####
//==========================================================================

// CPU space registers definitions

#define CYGARC_REG_CACR             0x002
#define CYGARC_REG_ACR0             0x004
#define CYGARC_REG_ACR1             0x005
#define CYGARC_REG_VBR              0x801
#define CYGARC_REG_ROMBAR0          0xC00
#define CYGARC_REG_RAMBAR0          0xC04
#define CYGARC_REG_MBAR             0xC0F

// ---------------------------------------------------------------------------
// End of var_regs.h
#endif // ifdef CYGONCE_HAL_VAR_REGS_H
