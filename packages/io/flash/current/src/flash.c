//==========================================================================
//
//      flash.c
//
//      Flash programming
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-07-26
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/io_flash.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>

#define  _FLASH_PRIVATE_
#include <cyg/io/flash.h>


// When this flag is set, do not actually jump to the relocated code.
// This can be used for running the function in place (RAM startup only),
// allowing calls to diag_printf() and similar.
#undef RAM_FLASH_DEV_DEBUG
#if !defined(CYG_HAL_STARTUP_RAM) && defined(RAM_FLASH_DEV_DEBUG)
# warning "Can only enable the flash debugging when configured for RAM startup"
# undef RAM_FLASH_DEV_DEBUG
#endif

struct flash_info flash_info;

int
flash_init(void *work_space, int work_space_size)
{
    int err;

    if (flash_info.init) return FLASH_ERR_OK;
    flash_info.work_space = work_space;
    flash_info.work_space_size = work_space_size;
    if ((err = flash_hwr_init()) != FLASH_ERR_OK) {
        return err;
    }
    flash_info.block_mask = ~(flash_info.block_size-1);
    flash_info.init = 1;
    return FLASH_ERR_OK;
}

// FIXME: Want to change all drivers to use this function. But it may
// make sense to wait till device structure pointer arguments get
// added as well.
void
flash_dev_query(void* data)
{
    extern char flash_query[], flash_query_end[];
    typedef void code_fun(void*);
    int code_len;
    code_fun *_flash_query;
    int d_cache, i_cache;

    // Query the device driver - copy 'query' code to RAM for execution
    code_len = (unsigned long)&flash_query_end - (unsigned long)&flash_query;
    _flash_query = (code_fun *)flash_info.work_space;
    memcpy(_flash_query, &flash_query, code_len);

    HAL_FLASH_CACHES_OFF(d_cache, i_cache);
#ifdef RAM_FLASH_DEV_DEBUG
    _flash_query = &flash_query;
#endif
    (*_flash_query)(data);
    HAL_FLASH_CACHES_ON(d_cache, i_cache);
}

int
flash_verify_addr(void *target)
{
    if (!flash_info.init) {
        return FLASH_ERR_NOT_INIT;
    }
    if (((unsigned long)target >= (unsigned long)flash_info.start) &&
        ((unsigned long)target < (unsigned long)flash_info.end)) {
        return FLASH_ERR_OK;
    } else {
        return FLASH_ERR_INVALID;
    }
}

int
flash_get_limits(void *target, void **start, void **end)
{
    if (!flash_info.init) {
        return FLASH_ERR_NOT_INIT;
    }
    *start = flash_info.start;
    *end = flash_info.end;
    return FLASH_ERR_OK;
}

int
flash_get_block_info(int *block_size, int *blocks)
{
    if (!flash_info.init) {
        return FLASH_ERR_NOT_INIT;
    }
    *block_size = flash_info.block_size;
    *blocks = flash_info.blocks;
    return FLASH_ERR_OK;
}
int
flash_erase(void *addr, int len, void **err_addr)
{
    unsigned short *block, *end_addr;
    int stat = 0;
    extern char flash_erase_block[], flash_erase_block_end[];
    int code_len;
    typedef int code_fun(unsigned short *, unsigned int);
    code_fun *_flash_erase_block;
    int d_cache, i_cache;

    if (!flash_info.init) {
        return FLASH_ERR_NOT_INIT;
    }

    // Copy 'erase' code to RAM for execution
    code_len = (unsigned long)&flash_erase_block_end - (unsigned long)&flash_erase_block;
    _flash_erase_block = (code_fun *)flash_info.work_space;
    memcpy(_flash_erase_block, &flash_erase_block, code_len);

    HAL_FLASH_CACHES_OFF(d_cache, i_cache);
    block = (unsigned short *)((unsigned long)addr & flash_info.block_mask);
    end_addr = (unsigned short *)((unsigned long)addr+len);

    printf("... Erase from %p-%p: ", (void*)block, (void*)end_addr);

    while (block < end_addr) {
#ifdef RAM_FLASH_DEV_DEBUG
        _flash_erase_block = &flash_erase_block;
#endif
        // Supply the blocksize for a gross check for erase success
        stat = (*_flash_erase_block)(block, flash_info.block_size);
        stat = flash_hwr_map_error(stat);
        if (stat) {
            *err_addr = (void *)block;
            break;
        }
        block += flash_info.block_size / sizeof(*block);
        printf(".");
    }
    HAL_FLASH_CACHES_ON(d_cache, i_cache);
    printf("\n");
    return (stat);
}

int
flash_program(void *_addr, void *_data, int len, void **err_addr)
{
    int stat = 0;
    int code_len, size;
    extern char flash_program_buf[], flash_program_buf_end[];
    typedef int code_fun(unsigned short *, unsigned short *, int);
    code_fun *_flash_program_buf;
    unsigned short *addr = (unsigned short *)_addr;
    unsigned short *data = (unsigned short *)_data;
    int d_cache, i_cache;

    if (!flash_info.init) {
        return FLASH_ERR_NOT_INIT;
    }

    // Copy 'program' code to RAM for execution
    code_len = (unsigned long)&flash_program_buf_end - (unsigned long)&flash_program_buf;
    _flash_program_buf = (code_fun *)flash_info.work_space;
    memcpy(_flash_program_buf, &flash_program_buf, code_len);
    HAL_FLASH_CACHES_OFF(d_cache, i_cache);

    printf("... Program from %p-%p at %p: ", (void*)data, (void*)(((unsigned long)data)+len), (void*)addr);

    while (len > 0) {
        size = len;
        if (size > flash_info.block_size) size = flash_info.block_size;
#ifdef RAM_FLASH_DEV_DEBUG
        _flash_program_buf = &flash_program_buf;
#endif
        stat = (*_flash_program_buf)(addr, data, size);
        stat = flash_hwr_map_error(stat);
        if (stat) {
            *err_addr = (void *)addr;
            break;
        }
        printf(".");
        len -= size;
        addr += size/sizeof(*addr);
        data += size/sizeof(*data);
    }
    HAL_FLASH_CACHES_ON(d_cache, i_cache);
    printf("\n");
    return (stat);
}

#if 0 < CYGHWR_IO_FLASH_BLOCK_LOCKING // This is an *interface*

int
flash_lock(void *addr, int len, void **err_addr)
{
    unsigned short *block, *end_addr;
    int stat = 0;
    extern char flash_lock_block[], flash_lock_block_end[];
    int code_len;
    typedef int code_fun(unsigned short *);
    code_fun *_flash_lock_block;
    int d_cache, i_cache;

    if (!flash_info.init) {
        return FLASH_ERR_NOT_INIT;
    }

    // Copy 'lock' code to RAM for execution
    code_len = (unsigned long)&flash_lock_block_end - (unsigned long)&flash_lock_block;
    _flash_lock_block = (code_fun *)flash_info.work_space;
    memcpy(_flash_lock_block, &flash_lock_block, code_len);
    HAL_FLASH_CACHES_OFF(d_cache, i_cache);

    block = (unsigned short *)((unsigned long)addr & flash_info.block_mask);
    end_addr = (unsigned short *)((unsigned long)addr+len);

    printf("... Lock from %p-%p: ", block, end_addr);

    while (block < end_addr) {
#ifdef RAM_FLASH_DEV_DEBUG
        _flash_lock_block = &flash_lock_block;
#endif
        stat = (*_flash_lock_block)(block);
        stat = flash_hwr_map_error(stat);
        if (stat) {
            *err_addr = (void *)block;
            break;
        }
        block += flash_info.block_size / sizeof(*block);
        printf(".");
    }
    HAL_FLASH_CACHES_ON(d_cache, i_cache);
    printf("\n");
    return (stat);
}

int
flash_unlock(void *addr, int len, void **err_addr)
{
    unsigned short *block, *end_addr;
    int stat = 0;
    extern char flash_unlock_block[], flash_unlock_block_end[];
    int code_len;
    typedef int code_fun(unsigned short *, int, int);
    code_fun *_flash_unlock_block;
    int d_cache, i_cache;

    if (!flash_info.init) {
        return FLASH_ERR_NOT_INIT;
    }

    // Copy 'lock' code to RAM for execution
    code_len = (unsigned long)&flash_unlock_block_end - (unsigned long)&flash_unlock_block;
    _flash_unlock_block = (code_fun *)flash_info.work_space;
    memcpy(_flash_unlock_block, &flash_unlock_block, code_len);
    HAL_FLASH_CACHES_OFF(d_cache, i_cache);

    block = (unsigned short *)((unsigned long)addr & flash_info.block_mask);
    end_addr = (unsigned short *)((unsigned long)addr+len);

    printf("... Unlock from %p-%p: ", block, end_addr);

    while (block < end_addr) {
#ifdef RAM_FLASH_DEV_DEBUG
        _flash_unlock_block = &flash_unlock_block;
#endif
        stat = (*_flash_unlock_block)(block, flash_info.block_size, flash_info.blocks);
        stat = flash_hwr_map_error(stat);
        if (stat) {
            *err_addr = (void *)block;
            break;
        }
        block += flash_info.block_size / sizeof(*block);
        printf(".");
    }
    HAL_FLASH_CACHES_ON(d_cache, i_cache);
    printf("\n");
    return (stat);
}
#endif

char *
flash_errmsg(int err)
{
    switch (err) {
    case FLASH_ERR_OK:
        return "No error - operation complete";
    case FLASH_ERR_ERASE_SUSPEND:
        return "Device is in erase suspend state";
    case FLASH_ERR_PROGRAM_SUSPEND:
        return "Device is in program suspend state";
    case FLASH_ERR_INVALID:
        return "Invalid FLASH address";
    case FLASH_ERR_ERASE:
        return "Error trying to erase";
    case FLASH_ERR_LOCK:
        return "Error trying to lock/unlock";
    case FLASH_ERR_PROGRAM:
        return "Error trying to program";
    case FLASH_ERR_PROTOCOL:
        return "Generic error";
    case FLASH_ERR_PROTECT:
        return "Device/region is write-protected";
    case FLASH_ERR_NOT_INIT:
        return "FLASH sub-system not initialized";
    case FLASH_ERR_DRV_VERIFY:
        return "Data verify failed after operation";
    default:
        return "Unknown error";
    }
}

// EOF io/flash/..../flash.c
