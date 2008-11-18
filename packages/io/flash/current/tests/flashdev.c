//=================================================================
//
//        flashdev.c
//
//        Simple tests for FLASHdev driver
//
//=================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2004 Gary Thomas
// Copyright (C) 2004, 2005, 2006, 2008 eCosCentric Limited
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
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     nickg, gthomas
// Contributors:  
// Date:          2005-10-25
// Description:   Simple test of FLASH I/O device. This is really
//                just a copy of the flash1 test, but using the
//                /dev/flash interface instead of the flash library.
//
// Options:
//####DESCRIPTIONEND####
//=================================================================

// #define DEBUG_PRINTFS

#include <pkgconf/hal.h>
#include <pkgconf/io_flash.h>
#include CYGHWR_MEMORY_LAYOUT_H

#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>

#include <cyg/io/io.h>
#include <cyg/io/config_keys.h>
#include <cyg/io/flash.h>
#include <errno.h>
#include <string.h>
#include <cyg/hal/hal_if.h>

//=================================================================
// Config options.

#define FLASH_TEST_OFFSET 0
#define FLASH_TEST_LENGTH 0x40000

//=================================================================
// A FIS entry named "flashtest" of at least FLASH_TEST_LENGTH bytes
// must exist for this test to work. It can be created with the
// following command:
//
// RedBoot> fis cre -b %{freememlo} -l 0x100000 flashtest

#define FLASH_TEST_DEVICE "/dev/flash/fis/flashtest"

// If it does not exist, a FIS entry named "jffs2test" will be used
// if present, as this may have been set up for jffs2 testing.
#define FLASH_TEST_DEVICE2 "/dev/flash/fis/jffs2test"

//=================================================================

#if !defined(CYGPKG_IO_FLASH_BLOCK_DEVICE)
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA("Only usable with flash block device driver");
}
#else

//=================================================================

externC void
cyg_start( void )
{
    Cyg_ErrNo stat;
    cyg_io_handle_t flash_handle;
    cyg_io_flash_getconfig_erase_t e;
    cyg_io_flash_getconfig_devsize_t d;
    cyg_io_flash_getconfig_blocksize_t b;
    cyg_io_flash_getconfig_lock_t l;
    cyg_io_flash_getconfig_unlock_t u;
    CYG_ADDRWORD flash_start, flash_end;
    CYG_ADDRWORD flash_test_start, flash_addr;
    cyg_uint32 flash_block_size, flash_num_blocks;
    CYG_ADDRWORD test_buf1, test_buf2;
    cyg_uint32 *lp1, *lp2;
    int i, len;
    cyg_bool passed, ok;

    CYG_TEST_INIT();

    passed = true;

    if ((stat = cyg_io_lookup(FLASH_TEST_DEVICE, &flash_handle)) == -ENOENT) {
        stat = cyg_io_lookup(FLASH_TEST_DEVICE2, &flash_handle);
    }

    if (stat != 0) {
        diag_printf("FLASH: driver init failed: %s\n", strerror(-stat));
        CYG_TEST_FAIL_FINISH("FLASH driver init failed");
    }

    len = sizeof(d);
    stat = cyg_io_get_config( flash_handle, CYG_IO_GET_CONFIG_FLASH_DEVSIZE, &d, &len );
    flash_start = 0;
    // Keep 'end' address as last valid location, to avoid wrap around problems
    flash_end = d.dev_size - 1;

    len = sizeof(b);
    b.offset = 0;
    stat = cyg_io_get_config( flash_handle, CYG_IO_GET_CONFIG_FLASH_BLOCKSIZE, &b, &len );
    flash_block_size = b.block_size;
    flash_num_blocks = d.dev_size/flash_block_size;

    diag_printf("FLASH: %p - %p, %d blocks of 0x%x bytes each.\n", 
                (void*)flash_start, (void*)(flash_end + 1), flash_num_blocks, flash_block_size);

    // Verify that the testing limits are within the bounds of the
    // physical device.  Also verify that the size matches with
    // the erase block size on the device
    if ((FLASH_TEST_OFFSET > (flash_end - flash_start)) ||
        ((FLASH_TEST_OFFSET + FLASH_TEST_LENGTH) > (flash_end - flash_start))) {
        CYG_TEST_FAIL_FINISH("FLASH test region outside physical limits");
    }
    if ((FLASH_TEST_LENGTH % flash_block_size) != 0) {
        CYG_TEST_FAIL_FINISH("FLASH test region must be integral multiple of erase block size");
    }

    // Allocate two buffers large enough for the test
    test_buf1 = (CYG_ADDRWORD)CYGMEM_SECTION_heap1;
    test_buf2 = test_buf1 + FLASH_TEST_LENGTH;
    if (CYGMEM_SECTION_heap1_SIZE < (FLASH_TEST_LENGTH * 2)) {
        CYG_TEST_FAIL_FINISH("FLASH not enough heap space - reduce size of test region");
    }
    diag_printf("... Using test buffers at %p and %p\n", (void *)test_buf1, (void *)test_buf2);
    flash_test_start = flash_start + FLASH_TEST_OFFSET;


#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING
    // Unlock test
    diag_printf("... Unlock test\n");
    ok = true;
    u.offset = flash_test_start;
    u.len = FLASH_TEST_LENGTH;
    len = sizeof(u);
    if ((stat = cyg_io_get_config(flash_handle, CYG_IO_GET_CONFIG_FLASH_UNLOCK, &u, &len ) ) != 0 || u.flasherr != 0)
    {
        diag_printf("FLASH: unlock failed: %s %s\n", strerror(stat), cyg_flash_errmsg(u.flasherr));
        ok = false;
    }
#endif
    
    // Erase test
    diag_printf("... Erase test\n");
    ok = true;

    e.offset = flash_test_start;
    e.len = FLASH_TEST_LENGTH;
    len = sizeof(e);
    if ((stat = cyg_io_get_config(flash_handle, CYG_IO_GET_CONFIG_FLASH_ERASE, &e, &len ) ) != 0 || e.flasherr != 0)
    {
        diag_printf("FLASH: erase failed: %s %s\n", cyg_flash_errmsg(stat),cyg_flash_errmsg(e.flasherr));
        ok = false;
    }
    len = FLASH_TEST_LENGTH;
    if (ok && (stat = cyg_io_bread(flash_handle, (void *)test_buf1, &len, flash_test_start)) != 0)
    {
        diag_printf("FLASH: read/verify after erase failed: %s\n", cyg_flash_errmsg(stat));
        ok = false;
    }    
    lp1 = (cyg_uint32 *)test_buf1;
    for (i = 0;  i < FLASH_TEST_LENGTH;  i += sizeof(cyg_uint32)) {
        if (*lp1++ != 0xFFFFFFFF) {
            diag_printf("FLASH: non-erased data found at offset %p\n", (void*)((CYG_ADDRWORD)(lp1-1) - test_buf1));
            diag_dump_buf((void *)(lp1-1), 32);
            ok = false;
            break;
        }
    }

    // Try reading in little pieces
    len = FLASH_TEST_LENGTH;
    flash_addr = flash_test_start;
    while (len > 0) {
        cyg_uint32 l = 0x200;
        if ((stat = cyg_io_bread(flash_handle, (void *)test_buf1, &l, flash_addr)) != CYG_FLASH_ERR_OK) {
            diag_printf("FLASH: read[short]/verify after erase failed: %s\n", strerror(stat));
            ok = false;
            break;
        }    
        flash_addr = flash_addr + l;
        len -= l;
        lp1 = (cyg_uint32 *)test_buf1;
        for (i = 0;  i < 0x200;  i += sizeof(cyg_uint32)) {
            if (*lp1++ != 0xFFFFFFFF) {
                diag_printf("FLASH: non-erased data found at offset %p\n", 
                            (cyg_uint8 *)flash_addr + (CYG_ADDRWORD)((lp1-1) - test_buf1));
                diag_dump_buf((void *)(lp1-1), 32);
                ok = false;
                len = 0;
                break;
            }
        }
    }
    
    if (!ok) {
        CYG_TEST_INFO("FLASH erase failed");
        passed = false;
    }

    // Simple write/verify test
    diag_printf("... Write/verify test\n");
    lp1 = (cyg_uint32 *)test_buf1;
    for (i = 0;  i < FLASH_TEST_LENGTH;  i += sizeof(cyg_uint32)) {
        *lp1 = (cyg_uint32)lp1;
        lp1++;
    }
    ok = true;
    len = FLASH_TEST_LENGTH;
    if (ok && (stat = cyg_io_bwrite(flash_handle, (void *)test_buf1,
                                    &len, flash_test_start)) != 0) {
        diag_printf("FLASH: write failed: %s\n", strerror(stat));
        ok = false;
    }


    len = FLASH_TEST_LENGTH;
    if (ok && (stat = cyg_io_bread(flash_handle,  (void *)test_buf2, &len, flash_test_start)) != CYG_FLASH_ERR_OK) {
        diag_printf("FLASH: read/verify after write failed: %s\n", strerror(stat));
        ok = false;
    }    
    lp1 = (cyg_uint32 *)test_buf1;
    lp2 = (cyg_uint32 *)test_buf2;
    for (i = 0;  i < FLASH_TEST_LENGTH;  i += sizeof(cyg_uint32)) {
        if (*lp2++ != *lp1++) {
            diag_printf("FLASH: incorrect data found at offset %p\n", (void *)((CYG_ADDRWORD)(lp2-1) - test_buf2));
            diag_dump_buf((void *)(lp2-1), 32);
            ok = false;
            break;
        }
    }

    // Try reading in little pieces
    len = FLASH_TEST_LENGTH;
    flash_addr = flash_test_start;
    lp1 = (cyg_uint32 *)test_buf1;
    lp2 = (cyg_uint32 *)test_buf2;
    while (len > 0) {
        cyg_uint32 l = 0x200;
        if ((stat = cyg_io_bread(flash_handle, (void *)lp2, &l, flash_addr)) != 0) {
            diag_printf("FLASH: read[short]/verify after erase failed: %s\n", strerror(stat));
            ok = false;
            break;
        }    
        flash_addr = flash_addr + l;
        len -= l;
        for (i = 0;  i < l;  i += sizeof(cyg_uint32)) {
            if (*lp2++ != *lp1++) {
                diag_printf("FLASH: incorrect data found at offset %p\n", 
                            (cyg_uint8 *)flash_addr + (CYG_ADDRWORD)((lp2-1) - test_buf2));
                diag_dump_buf((void *)(lp2-1), 32);
                ok = false;
                len = 0;
                break;
            }
        }
    }

#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING
    // Lock test
    diag_printf("... Lock test\n");
    ok = true;
    l.offset = flash_test_start;
    l.len = FLASH_TEST_LENGTH;
    len = sizeof(l);
    if ((stat = cyg_io_get_config(flash_handle, CYG_IO_GET_CONFIG_FLASH_LOCK, &l, &len ) ) != 0 || l.flasherr != 0 )
    {
        diag_printf("FLASH: unlock failed: %s %s\n", strerror(stat), cyg_flash_errmsg(l.flasherr));
        ok = false;
    }
#endif
    
    if (!ok) {
        CYG_TEST_INFO("FLASH write/verify failed");
    }

    if (passed) {
        CYG_TEST_PASS_FINISH("FLASH test1");
    } else {
        CYG_TEST_FAIL_FINISH("FLASH test1");
    }
}
#endif

//=================================================================
// EOF flashdev.c
