#ifndef CYGONCE_DEVS_FLASH_AM29xxxxx_dev_V2_H
# define CYGONCE_DEVS_FLASH_AM29xxxxx_dev_V2_H
//==========================================================================
//
//      am29xxxxx_dev.h
//
//      Flash driver for the AMD family - driver details
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2004, 2005 eCosCentric Ltd
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
// Author(s):    bartv
// Contributors:
// Date:         2004-11-05
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/devs_flash_amd_am29xxxxx_v2.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>

externC int cyg_am29xxxxx_read_devid_8(     struct cyg_flash_dev*);
externC int cyg_am29xxxxx_read_devid_16(    struct cyg_flash_dev*);
externC int cyg_am29xxxxx_read_devid_32(    struct cyg_flash_dev*);
externC int cyg_am29xxxxx_read_devid_88(    struct cyg_flash_dev*);
externC int cyg_am29xxxxx_read_devid_8888(  struct cyg_flash_dev*);
externC int cyg_am29xxxxx_read_devid_1616(  struct cyg_flash_dev*);
externC int cyg_am29xxxxx_read_devid_16as8( struct cyg_flash_dev*);

externC int cyg_am29xxxxx_init_check_devid_8(     struct cyg_flash_dev*);
externC int cyg_am29xxxxx_init_check_devid_16(    struct cyg_flash_dev*);
externC int cyg_am29xxxxx_init_check_devid_32(    struct cyg_flash_dev*);
externC int cyg_am29xxxxx_init_check_devid_88(    struct cyg_flash_dev*);
externC int cyg_am29xxxxx_init_check_devid_8888(  struct cyg_flash_dev*);
externC int cyg_am29xxxxx_init_check_devid_1616(  struct cyg_flash_dev*);
externC int cyg_am29xxxxx_init_check_devid_16as8( struct cyg_flash_dev*);

externC int cyg_am29xxxxx_init_cfi_8(     struct cyg_flash_dev*);
externC int cyg_am29xxxxx_init_cfi_16(    struct cyg_flash_dev*);
externC int cyg_am29xxxxx_init_cfi_32(    struct cyg_flash_dev*);
externC int cyg_am29xxxxx_init_cfi_88(    struct cyg_flash_dev*);
externC int cyg_am29xxxxx_init_cfi_8888(  struct cyg_flash_dev*);
externC int cyg_am29xxxxx_init_cfi_1616(  struct cyg_flash_dev*);
externC int cyg_am29xxxxx_init_cfi_16as8( struct cyg_flash_dev*);

externC int cyg_am29xxxxx_erase_8(     struct cyg_flash_dev*, cyg_flashaddr_t);
externC int cyg_am29xxxxx_erase_16(    struct cyg_flash_dev*, cyg_flashaddr_t);
externC int cyg_am29xxxxx_erase_32(    struct cyg_flash_dev*, cyg_flashaddr_t);
externC int cyg_am29xxxxx_erase_88(    struct cyg_flash_dev*, cyg_flashaddr_t);
externC int cyg_am29xxxxx_erase_8888(  struct cyg_flash_dev*, cyg_flashaddr_t);
externC int cyg_am29xxxxx_erase_1616(  struct cyg_flash_dev*, cyg_flashaddr_t);
externC int cyg_am29xxxxx_erase_16as8( struct cyg_flash_dev*, cyg_flashaddr_t);

externC int cyg_am29xxxxx_program_8(     struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_am29xxxxx_program_16(    struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_am29xxxxx_program_32(    struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_am29xxxxx_program_88(    struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_am29xxxxx_program_8888(  struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_am29xxxxx_program_1616(  struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_am29xxxxx_program_16as8( struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);

externC int cyg_at49xxxx_softlock_8(     struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_softlock_16(    struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_softlock_32(    struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_softlock_88(    struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_softlock_8888(  struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_softlock_1616(  struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_softlock_16as8( struct cyg_flash_dev*, const cyg_flashaddr_t );

externC int cyg_at49xxxx_hardlock_8(     struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_hardlock_16(    struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_hardlock_32(    struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_hardlock_88(    struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_hardlock_8888(  struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_hardlock_1616(  struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_hardlock_16as8( struct cyg_flash_dev*, const cyg_flashaddr_t );

externC int cyg_at49xxxx_unlock_8(     struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_unlock_16(    struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_unlock_32(    struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_unlock_88(    struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_unlock_8888(  struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_unlock_1616(  struct cyg_flash_dev*, const cyg_flashaddr_t );
externC int cyg_at49xxxx_unlock_16as8( struct cyg_flash_dev*, const cyg_flashaddr_t );

// FIXME: add program_buffered() support as per e.g. the AM29LV128
// FIXME: add software lock/unlock support as per e.g. the AM29BDS640

// The driver-specific data, pointed at by the priv field in a
// a cyg_flash_dev structure.
typedef struct cyg_am29xxxxx_dev {
    // The device id, mainly for use by the init_check_devid() routines
    cyg_uint32              devid;
    // Space for the block_info fields needed for the cyg_flash_dev.
    // These can be statically initialized, or dynamically via
    // init_cfi().
    cyg_flash_block_info_t  block_info[CYGNUM_DEVS_FLASH_AMD_AM29XXXXX_V2_ERASE_REGIONS];
} cyg_am29xxxxx_dev;

// TODO: An AM29 specific macro could optimise the common case of CYG_FLASH_FUNS.
// Bart and Jifl discussed this in mid July/start Aug 2005.

#endif  // CYGONCE_DEVS_FLASH_AM29xxxxx_dev_V2_H
