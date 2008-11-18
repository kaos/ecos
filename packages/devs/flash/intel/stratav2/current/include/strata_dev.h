#ifndef CYGONCE_DEVS_FLASH_STRATA_DEV_V2_H
# define CYGONCE_DEVS_FLASH_STRATA_DEV_V2_H
//==========================================================================
//
//      strata_dev.h
//
//      Flash driver for the Intel Strata family - driver details
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2005, 2006 eCosCentric Ltd
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
// Date:         2005-06-11
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/devs_flash_strata_v2.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>

externC int     cyg_strata_init_nop(struct cyg_flash_dev*);
externC size_t  cyg_strata_query_nop(struct cyg_flash_dev*, void*, const size_t);
externC int     cyg_strata_hwr_map_error_nop(struct cyg_flash_dev*, int);
externC int     cyg_strata_lock_nop(struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int     cyg_strata_unlock_nop(struct cyg_flash_dev*, const cyg_flashaddr_t);

externC void cyg_strata_read_devid_8(     struct cyg_flash_dev*, cyg_uint32*, cyg_uint32*);
externC void cyg_strata_read_devid_16(    struct cyg_flash_dev*, cyg_uint32*, cyg_uint32*);
externC void cyg_strata_read_devid_32(    struct cyg_flash_dev*, cyg_uint32*, cyg_uint32*);
externC void cyg_strata_read_devid_88(    struct cyg_flash_dev*, cyg_uint32*, cyg_uint32*);
externC void cyg_strata_read_devid_8888(  struct cyg_flash_dev*, cyg_uint32*, cyg_uint32*);
externC void cyg_strata_read_devid_1616(  struct cyg_flash_dev*, cyg_uint32*, cyg_uint32*);
externC void cyg_strata_read_devid_16as8( struct cyg_flash_dev*, cyg_uint32*, cyg_uint32*);

externC int cyg_strata_init_check_devid_8(     struct cyg_flash_dev*);
externC int cyg_strata_init_check_devid_16(    struct cyg_flash_dev*);
externC int cyg_strata_init_check_devid_32(    struct cyg_flash_dev*);
externC int cyg_strata_init_check_devid_88(    struct cyg_flash_dev*);
externC int cyg_strata_init_check_devid_8888(  struct cyg_flash_dev*);
externC int cyg_strata_init_check_devid_1616(  struct cyg_flash_dev*);
externC int cyg_strata_init_check_devid_16as8( struct cyg_flash_dev*);

externC int cyg_strata_init_cfi_8(     struct cyg_flash_dev*);
externC int cyg_strata_init_cfi_16(    struct cyg_flash_dev*);
externC int cyg_strata_init_cfi_32(    struct cyg_flash_dev*);
externC int cyg_strata_init_cfi_88(    struct cyg_flash_dev*);
externC int cyg_strata_init_cfi_8888(  struct cyg_flash_dev*);
externC int cyg_strata_init_cfi_1616(  struct cyg_flash_dev*);
externC int cyg_strata_init_cfi_16as8( struct cyg_flash_dev*);

externC int cyg_strata_erase_8(     struct cyg_flash_dev*, cyg_flashaddr_t);
externC int cyg_strata_erase_16(    struct cyg_flash_dev*, cyg_flashaddr_t);
externC int cyg_strata_erase_32(    struct cyg_flash_dev*, cyg_flashaddr_t);
externC int cyg_strata_erase_88(    struct cyg_flash_dev*, cyg_flashaddr_t);
externC int cyg_strata_erase_8888(  struct cyg_flash_dev*, cyg_flashaddr_t);
externC int cyg_strata_erase_1616(  struct cyg_flash_dev*, cyg_flashaddr_t);
externC int cyg_strata_erase_16as8( struct cyg_flash_dev*, cyg_flashaddr_t);

externC int cyg_strata_program_8(     struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_strata_program_16(    struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_strata_program_32(    struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_strata_program_88(    struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_strata_program_8888(  struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_strata_program_1616(  struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_strata_program_16as8( struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);

externC int cyg_strata_bufprogram_8(     struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_strata_bufprogram_16(    struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_strata_bufprogram_32(    struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_strata_bufprogram_88(    struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_strata_bufprogram_8888(  struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_strata_bufprogram_1616(  struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);
externC int cyg_strata_bufprogram_16as8( struct cyg_flash_dev*, cyg_flashaddr_t, const void*, size_t);

externC int cyg_strata_lock_j3_8(     struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_lock_j3_16(    struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_lock_j3_32(    struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_lock_j3_88(    struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_lock_j3_1616(  struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_lock_j3_16as8( struct cyg_flash_dev*, const cyg_flashaddr_t);

externC int cyg_strata_lock_k3_8(     struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_lock_k3_16(    struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_lock_k3_32(    struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_lock_k3_88(    struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_lock_k3_1616(  struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_lock_k3_16as8( struct cyg_flash_dev*, const cyg_flashaddr_t);

externC int cyg_strata_unlock_j3_8(     struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_unlock_j3_16(    struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_unlock_j3_32(    struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_unlock_j3_88(    struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_unlock_j3_1616(  struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_unlock_j3_16as8( struct cyg_flash_dev*, const cyg_flashaddr_t);

externC int cyg_strata_unlock_k3_8(     struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_unlock_k3_16(    struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_unlock_k3_32(    struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_unlock_k3_88(    struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_unlock_k3_1616(  struct cyg_flash_dev*, const cyg_flashaddr_t);
externC int cyg_strata_unlock_k3_16as8( struct cyg_flash_dev*, const cyg_flashaddr_t);

// An additional exported interface for application developers.
externC int cyg_strata_unlock_all_j3_8(     const cyg_flashaddr_t);
externC int cyg_strata_unlock_all_j3_16(    const cyg_flashaddr_t);
externC int cyg_strata_unlock_all_j3_32(    const cyg_flashaddr_t);
externC int cyg_strata_unlock_all_j3_88(    const cyg_flashaddr_t);
externC int cyg_strata_unlock_all_j3_1616(  const cyg_flashaddr_t);
externC int cyg_strata_unlock_all_j3_16as8( const cyg_flashaddr_t);

// The driver-specific data, pointed at by the priv field in a
// a cyg_flash_dev structure.
typedef struct cyg_strata_dev {
    // The device id, mainly for use by the init_check_devid() routines
    cyg_uint32              manufacturer_code;
    cyg_uint32              device_code;
    // The buffer size for buffered writes in words, usually 16
    cyg_uint32              bufsize;
    // Space for the block_info fields needed for the cyg_flash_dev.
    // These can be statically initialized, or dynamically via
    // init_cfi().
    cyg_flash_block_info_t  block_info[CYGNUM_DEVS_FLASH_STRATA_V2_ERASE_REGIONS];
} cyg_strata_dev;

#define CYG_FLASH_STRATA_MANUFACTURER_INTEL     0x0089
#define CYG_FLASH_STRATA_MANUFACTURER_STMICRO   0x0020

#endif  // CYGONCE_DEVS_FLASH_STRATA_DEV_V2_H
