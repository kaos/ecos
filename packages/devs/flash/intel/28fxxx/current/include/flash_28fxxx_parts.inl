#ifndef CYGONCE_DEVS_FLASH_INTEL_28FXXX_PARTS_INL
#define CYGONCE_DEVS_FLASH_INTEL_28FXXX_PARTS_INL
//==========================================================================
//
//      flash_28fxxx_parts.inl
//
//      Intel 28Fxxx part descriptors
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jskov
// Contributors: jskov
// Date:         2001-08-07
// Purpose:
// Description:  Intel 28Fxxx part descriptors
// Usage:        Should be included from the flash_28fxxx.inl file only.
//
// FIXME:        Add configury for selecting bottom/top bootblocks
//####DESCRIPTIONEND####
//
//==========================================================================

#if CYGNUM_FLASH_WIDTH == 8

#else // 16 bit devices

#ifdef CYGHWR_DEVS_FLASH_INTEL_28F320C3
    {   // 28F320C3-T
        device_id  : FLASHWORD(0x88c4),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        locking    : true,
        buffered_w : false,
        bootblock  : true,
        bootblocks : { 0x3e0000 * CYGNUM_FLASH_INTERLEAVE,
                       0x001000 * CYGNUM_FLASH_INTERLEAVE,
                       0x001000 * CYGNUM_FLASH_INTERLEAVE,
                       0x001000 * CYGNUM_FLASH_INTERLEAVE,
                       0x001000 * CYGNUM_FLASH_INTERLEAVE,
                       0x001000 * CYGNUM_FLASH_INTERLEAVE,
                       0x001000 * CYGNUM_FLASH_INTERLEAVE,
                       0x001000 * CYGNUM_FLASH_INTERLEAVE,
                       0x001000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        banked     : false
    },
    {   // 28F320C3-B
        device_id  : FLASHWORD(0x88c5),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 64,
        device_size: 0x400000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x400000 * CYGNUM_FLASH_INTERLEAVE - 1),
        locking    : true,
        buffered_w : false,
        bootblock  : true,
        bootblocks : { 0x000000 * CYGNUM_FLASH_INTERLEAVE,
                       0x001000 * CYGNUM_FLASH_INTERLEAVE,
                       0x001000 * CYGNUM_FLASH_INTERLEAVE,
                       0x001000 * CYGNUM_FLASH_INTERLEAVE,
                       0x001000 * CYGNUM_FLASH_INTERLEAVE,
                       0x001000 * CYGNUM_FLASH_INTERLEAVE,
                       0x001000 * CYGNUM_FLASH_INTERLEAVE,
                       0x001000 * CYGNUM_FLASH_INTERLEAVE,
                       0x001000 * CYGNUM_FLASH_INTERLEAVE,
                       0
                     },
        banked     : false
    },
#endif

#ifdef CYGHWR_DEVS_FLASH_INTEL_28F160S5
    {   // 28F160S5
        device_id  : FLASHWORD(0x00d0),
        block_size : 0x10000 * CYGNUM_FLASH_INTERLEAVE,
        block_count: 32,
        device_size: 0x200000 * CYGNUM_FLASH_INTERLEAVE,
        base_mask  : ~(0x200000 * CYGNUM_FLASH_INTERLEAVE - 1),
        buffered_w : true,
        locking    : false,
        bootblock  : false,
        banked     : false
    },
#endif

#endif // 16 bit devices


#endif // CYGONCE_DEVS_FLASH_INTEL_28FXXX_PARTS_INL
