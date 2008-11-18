//==========================================================================
//
//      flash.c
//
//      Flash programming
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2004 Andrew Lunn
// Copyright (C) 2004, 2005, 2006 eCosCentric Ltd.
// Copyright (C) 2003 Gary Thomas
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
// Author(s):    gthomas
// Contributors: gthomas, Andrew Lunn, Bart Veer
// Date:         2000-07-26
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/io_flash.h>
#ifdef CYGPKG_KERNEL
#include <cyg/kernel/kapi.h>
#endif
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_tables.h>
#include <cyg/infra/cyg_ass.h>
#include <string.h>

#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>
#include "flash_legacy.h"

// When this flag is set, do not actually jump to the relocated code.
// This can be used for running the function in place (RAM startup
// only), allowing calls to diag_printf() and similar.
#undef RAM_FLASH_DEV_DEBUG
#if !defined(CYG_HAL_STARTUP_RAM) && defined(RAM_FLASH_DEV_DEBUG)
# warning "Can only enable the flash debugging when configured for RAM startup"
#endif

// Optional verbosity. Using a macro here avoids lots of ifdefs in the
// rest of the code.
#ifdef CYGSEM_IO_FLASH_CHATTER
# define CHATTER(_dev_, _fmt_, ...) (*(_dev_)->pf)((_fmt_), ## __VA_ARGS__)
#else
# define CHATTER(_dev_, _fmt_, ...) CYG_EMPTY_STATEMENT
#endif

// Per-thread locking. Again using macros avoids lots of ifdefs
#ifdef CYGPKG_KERNEL
# define LOCK_INIT(_dev_)   cyg_mutex_init(&((_dev_)->mutex))
# define LOCK(_dev_)        cyg_mutex_lock(&((_dev_)->mutex))
# define UNLOCK(_dev_)      cyg_mutex_unlock(&((_dev_)->mutex))
#else
# define LOCK_INIT(_dev_)   CYG_EMPTY_STATEMENT
# define LOCK(_dev_)        CYG_EMPTY_STATEMENT
# define UNLOCK(_dev_)      CYG_EMPTY_STATEMENT
#endif

// Software write-protect. Very rarely used.
#ifdef CYGSEM_IO_FLASH_SOFT_WRITE_PROTECT
# define CHECK_SOFT_WRITE_PROTECT(_addr_, _len_)    \
  CYG_MACRO_START                                   \
  if (plf_flash_query_soft_wp((_addr_), (_len_)))   \
    return CYG_FLASH_ERR_PROTECT;                   \
  CYG_MACRO_END
#else
#define CHECK_SOFT_WRITE_PROTECT(_addr_, _len_) CYG_EMPTY_STATEMENT
#endif

// Has the FLASH IO library been initialised?
static bool init;

// This array contains entries for all flash devices that are
// installed in the system.
__externC struct cyg_flash_dev cyg_flashdevtab[];
CYG_HAL_TABLE_BEGIN(cyg_flashdevtab, cyg_flashdev);

// end of the flashdev table
__externC struct cyg_flash_dev cyg_flashdevtab_end;
CYG_HAL_TABLE_END(cyg_flashdevtab_end, cyg_flashdev);

#if (1 == CYGHWR_IO_FLASH_DEVICE)

// Optimize the code for a single flash device, which is the common case.
// The flash subsystem must have been initialized, the single device must
// contain the specified address, and the device itself must have
// initialized successfully.
static struct cyg_flash_dev*
find_dev(cyg_flashaddr_t addr, int* stat)
{
  if (!init) {
    *stat = CYG_FLASH_ERR_NOT_INIT;
    return NULL;
  }
  if (! ((addr >= cyg_flashdevtab[0].start) && (addr <= cyg_flashdevtab[0].end))) {
    *stat = CYG_FLASH_ERR_INVALID;
    return NULL;
  }
  if (! cyg_flashdevtab[0].init) {
    *stat = CYG_FLASH_ERR_NOT_INIT;
    return NULL;
  }
  return &cyg_flashdevtab[0];
}

#else

// There are multiple devices. For convenience these are kept in a
// linked list, sorted by address. This is the head of the list
static struct cyg_flash_dev *flash_head = NULL;

static bool flash_sort_and_check(void) 
{
  bool moved;
  struct cyg_flash_dev *dev, **previous_next;

  // Place all devices that initialised on the list, unsorted for now.
  for (dev = &cyg_flashdevtab[0]; dev != &cyg_flashdevtab_end; dev++) {
    if (dev->init) {
      dev->next  = flash_head;
      flash_head = dev;
    }
  }
  
  // If there are no valid devices, abort. This might happen if
  // all drivers failed to initialize.
  if (flash_head == NULL) {
    return false;
  }

  // Sort the linked list into ascending order of flash address. Use a
  // primitive ripple sort, but since we don't expect to have many
  // devices this should be OK. This loop may run safely with just one
  // entry on the list.
  do {
    moved=false;
    for (dev=flash_head, previous_next=&flash_head; 
         dev->next; 
         previous_next = &dev->next, dev=dev->next ){
      if (dev->start > dev->next->start) {
        *previous_next=dev->next;
        dev->next = (*previous_next)->next;
        (*previous_next)->next = dev;
        moved=true;          
        break;
      }
    }
  } while (moved);
  
  // Now walk the linked list and see if there are any overlaps in the
  // addresses the devices claim to use using.
  for (dev=flash_head; dev->next; dev=dev->next){
    if (dev->end >= dev->next->start)
      return false;
  }
  return true;
}

// Find the device at the specified address, if any.
static struct cyg_flash_dev*
find_dev(cyg_flashaddr_t addr, int* stat)
{
  struct cyg_flash_dev*   dev;
  if (!init) {
    *stat = CYG_FLASH_ERR_NOT_INIT;
    return NULL;
  }
  for (dev = flash_head; dev; dev = dev->next) {
    if ((dev->start <= addr) && (addr <= dev->end)) {
      return dev;
    }
  }
  *stat = CYG_FLASH_ERR_INVALID;
  return NULL;
}

#endif

// Initialise all registered device. Any device that fails to
// initialise we leave dev->init as false. Then sort the devices into
// ascending order of address and put them into a linked list. Lastly
// check if we have any overlap of the addresses.
__externC int 
cyg_flash_init(cyg_flash_printf *pf) 
{
  int err;
  struct cyg_flash_dev * dev;
  
  if (init) return CYG_FLASH_ERR_OK;

  CYG_ASSERT(&(cyg_flashdevtab[CYGHWR_IO_FLASH_DEVICE]) == &cyg_flashdevtab_end, "incorrect number of flash devices");
  
  for (dev = &cyg_flashdevtab[0]; dev != &cyg_flashdevtab_end; dev++) {
    dev->pf = pf;
    LOCK_INIT(dev);
    
    err = dev->funs->flash_init(dev);
    if (err != CYG_FLASH_ERR_OK) {
      continue;
    }
    CYG_ASSERT(dev->funs, "No flash functions");
    CYG_ASSERT(dev->num_block_infos, "No number of block infos");
    CYG_ASSERT(dev->block_info, "No block infos");
    CYG_ASSERT(!(((cyg_flashaddr_t)dev->block_info >= dev->start) && 
                 ((cyg_flashaddr_t)dev->block_info < dev->end)),
               "Block info is in the flash");
    CYG_ASSERT(dev->funs->flash_erase_block, "No erase function");
    CYG_ASSERT(dev->funs->flash_program, "No program function");
#ifdef CYGDBG_USE_ASSERTS
    {
         int i; 
         cyg_flashaddr_t addr = dev->start;
         for (i = 0; i < dev->num_block_infos; i++) {
              addr += dev->block_info[i].block_size * dev->block_info[i].blocks;
         }
         CYG_ASSERT(dev->end == addr-1, "Invalid end address");
    }
#endif
    dev->init = true;
  }
  
#if (1 == CYGHWR_IO_FLASH_DEVICE)
  // Make sure there is one device, otherwise we could end up
  // accessing a non-existent cyg_flash_dev structure.
  if (&(cyg_flashdevtab[0]) == &cyg_flashdevtab_end) {
      return CYG_FLASH_ERR_INVALID;
  }
#else
  // Place the devices on a sorted linked list and check that there
  // are no overlaps in the address space.
  if (! flash_sort_and_check() ) {
    return CYG_FLASH_ERR_INVALID;
  }
#endif

  // Only mark the flash subsystem as initialized if the world is
  // consistent.
  init = true;
  return CYG_FLASH_ERR_OK;
}

// Is the address within one of the flash drivers?
__externC int
cyg_flash_verify_addr(const cyg_flashaddr_t address)
{
  int stat = CYG_FLASH_ERR_OK;
  (void) find_dev(address, &stat);
  return stat;
}

// Return information about the Nth driver
__externC int
cyg_flash_get_info(cyg_uint32 Nth, cyg_flash_info_t * info)
{
  struct cyg_flash_dev * dev;

  if (!init) return CYG_FLASH_ERR_NOT_INIT;

#if (1 == CYGHWR_IO_FLASH_DEVICE)
  if ((0 == Nth) && cyg_flashdevtab[0].init) {
      dev = &(cyg_flashdevtab[0]);
  } else {
      return CYG_FLASH_ERR_INVALID;
  }
#else
  // Only initialized devices are on the list.
  for (dev = flash_head; dev && Nth; dev=dev->next, Nth--)
    ;
  if (!dev) {
      return CYG_FLASH_ERR_INVALID;
  }
#endif
  info->start = dev->start;
  info->end = dev->end;
  info->num_block_infos = dev->num_block_infos;
  info->block_info = dev->block_info;
  return CYG_FLASH_ERR_OK;
}

// Return information about the flash at the given address
__externC int
cyg_flash_get_info_addr(const cyg_flashaddr_t flash_base, cyg_flash_info_t * info)
{
  struct cyg_flash_dev *dev;
  int                   stat = CYG_FLASH_ERR_OK;

  dev = find_dev(flash_base, &stat);
  if (dev) {
    info->start = dev->start;
    info->end = dev->end;
    info->num_block_infos = dev->num_block_infos;
    info->block_info = dev->block_info;
  }
  return stat;
}

#ifdef CYGPKG_KERNEL
// Lock the mutex's for a range of addresses
__externC int
cyg_flash_mutex_lock(const cyg_flashaddr_t from, size_t len) 
{
  struct cyg_flash_dev *    dev;
  int                       stat    = CYG_FLASH_ERR_OK;

  dev = find_dev(from, &stat);
  if (dev) {
    LOCK(dev);
    if (len > (dev->end + 1 - from)) {
      stat = cyg_flash_mutex_lock(dev->end + 1, len - (dev->end + 1 - from));
      if (CYG_FLASH_ERR_OK != stat) {
        // Something went wrong, unlock what we just locked
        UNLOCK(dev);
      }
    }
  }
  return stat;
}

// Unlock the mutex's for a range of addresses
__externC int
cyg_flash_mutex_unlock(const cyg_flashaddr_t from, size_t len) 
{
  struct cyg_flash_dev *    dev;
  int                       stat = CYG_FLASH_ERR_OK;

  dev = find_dev(from, &stat);
  if (dev) {
    UNLOCK(dev);
    if (len > (dev->end + 1 - from)) {
      stat = cyg_flash_mutex_lock(dev->end + 1, len - (dev->end + 1 - from));
      if (CYG_FLASH_ERR_OK != stat) {
        // Something went wrong, relock what we just unlocked. This may not
        // be worth it since things must be pretty messed up, and could
        // conceivably end in deadlock if there is a concurrent call to
        // cyg_flash_mutex_lock();
        LOCK(dev);
      }
    }
  }
  return stat;
}
#endif

// Return the size of the block which is at the given address
static size_t 
flash_block_size(struct cyg_flash_dev *dev, const cyg_flashaddr_t addr)
{
  int i;
  size_t offset;
  
  CYG_ASSERT((addr >= dev->start) && (addr <= dev->end), "Not inside device");
  
  offset = addr - dev->start;
  for (i=0; i < dev->num_block_infos; i++) {
    if (offset < (dev->block_info[i].blocks * dev->block_info[i].block_size))
      return dev->block_info[i].block_size;
    offset = offset - 
      (dev->block_info[i].blocks * dev->block_info[i].block_size);
  }
  CYG_FAIL("Programming error");
  return 0;
}

// Return the size of the block which is at the given address
__externC size_t
cyg_flash_block_size(const cyg_flashaddr_t flash_base) 
{
  struct cyg_flash_dev *    dev;
  int                       stat;

  dev = find_dev(flash_base, &stat);
  if (!dev) return stat;
  return flash_block_size(dev, flash_base);
}

// Return the first address of a block. The flash might not be aligned
// in terms of its block size. So we have to be careful and use
// offsets.
static inline cyg_flashaddr_t 
flash_block_begin(cyg_flashaddr_t addr, struct cyg_flash_dev *dev)
{
  size_t block_size;
  cyg_flashaddr_t offset;
  
  block_size = flash_block_size(dev, addr);
  
  offset = addr - dev->start;
  offset = (offset / block_size) * block_size;
  return offset + dev->start;
}


__externC int 
cyg_flash_erase(cyg_flashaddr_t flash_base, 
                size_t len, 
                cyg_flashaddr_t *err_address)
{
  cyg_flashaddr_t block, end_addr;
  struct cyg_flash_dev * dev;
  size_t erase_count;
  int stat = CYG_FLASH_ERR_OK;
  HAL_FLASH_CACHES_STATE(d_cache, i_cache);

  dev = find_dev(flash_base, &stat);
  if (!dev) return stat;

  CHECK_SOFT_WRITE_PROTECT(flash_base, len);
  
  LOCK(dev);

  // Check whether or not we are going past the end of this device, on
  // to the next one. If so the next device will be handled by a
  // recursive call later on.
  if (len > (dev->end + 1 - flash_base)) {
      end_addr = dev->end;
  } else {
      end_addr = flash_base + len - 1;
  }
  // erase can only happen on a block boundary, so adjust for this
  block         = flash_block_begin(flash_base, dev);
  erase_count   = (end_addr + 1) - block;

  CHATTER(dev, "... Erase from %p-%p: ", (void*)block, (void*)end_addr);
  
  HAL_FLASH_CACHES_OFF(d_cache, i_cache);
  FLASH_Enable(flash_base, end_addr);
  while (erase_count > 0) {
    int i;
    unsigned char *dp;
    bool erased = false;
    size_t block_size = flash_block_size(dev, block);

    // Pad to the block boundary, if necessary
    if (erase_count < block_size) {
        erase_count = block_size;
    }

    // If there is a read function it probably means the flash
    // cannot be read directly.
    if (!dev->funs->flash_read) {
      erased = true;
      dp = (unsigned char *)block;
      for (i = 0;  i < block_size;  i++) {
        if (*dp++ != (unsigned char)0xFF) {
          erased = false;
          break;
        }
      }
    }
    if (!erased) {
      stat = dev->funs->flash_erase_block(dev,block);
    }
    if (CYG_FLASH_ERR_OK != stat) {
        if (err_address)
            *err_address = block;
        break;
    }
    block       += block_size;
    erase_count -= block_size;
    CHATTER(dev, ".");
  }
  FLASH_Disable(flash_base, end_addr);
  HAL_FLASH_CACHES_ON(d_cache, i_cache);
  CHATTER(dev, "\n");
  UNLOCK(dev);
  if (stat != CYG_FLASH_ERR_OK) {
    return stat;
  }

  // If there are multiple flash devices in series the erase operation
  // may touch successive devices. This can be handled by recursion.
  // The stack overheads should be minimal because the number of
  // devices will be small.
  if (len > (dev->end + 1 - flash_base)) {
    return cyg_flash_erase(dev->end+1, 
                           len - (dev->end + 1 - flash_base),
                           err_address);
  }
  return CYG_FLASH_ERR_OK;
}

__externC int 
cyg_flash_program(cyg_flashaddr_t flash_base, 
                  const void *ram_base, 
                  size_t len, 
                  cyg_flashaddr_t *err_address)
{
  struct cyg_flash_dev * dev;
  cyg_flashaddr_t addr, end_addr, block;
  const unsigned char * ram = ram_base;
  size_t write_count, offset;
  int stat = CYG_FLASH_ERR_OK;
  HAL_FLASH_CACHES_STATE(d_cache, i_cache);

  dev = find_dev(flash_base, &stat);
  if (!dev) return stat;

  CHECK_SOFT_WRITE_PROTECT(flash_base, len);
  
  LOCK(dev);
  addr = flash_base;
  if (len > (dev->end + 1 - flash_base)) {
    end_addr = dev->end;
  } else {
    end_addr = flash_base + len - 1;
  }
  write_count = (end_addr + 1) - flash_base;

  // The first write may be in the middle of a block. Do the necessary
  // adjustment here rather than inside the loop.
  block = flash_block_begin(flash_base, dev);
  if (addr == block) {
      offset = 0;
  } else {
      offset = addr - block;
  }
  
  CHATTER(dev, "... Program from %p-%p to %p: ", ram_base, ((CYG_ADDRESS)ram_base)+write_count, addr);
  
  HAL_FLASH_CACHES_OFF(d_cache, i_cache);
  FLASH_Enable(flash_base, end_addr);
  while (write_count > 0) {
    size_t block_size = flash_block_size(dev, addr);
    size_t this_write;
    if (write_count > (block_size - offset)) {
        this_write = block_size - offset;
    } else {
        this_write = write_count;
    }
    // Only the first block may need the offset.
    offset       = 0;
    
    stat = dev->funs->flash_program(dev, addr, ram, this_write);
#ifdef CYGSEM_IO_FLASH_VERIFY_PROGRAM
    if (CYG_FLASH_ERR_OK == stat) // Claims to be OK
      if (!dev->funs->flash_read && memcmp((void *)addr, ram, this_write) != 0) {                
        stat = CYG_FLASH_ERR_DRV_VERIFY;
        CHATTER(dev, "V");
      }
#endif
    if (CYG_FLASH_ERR_OK != stat) {
        if (err_address)
            *err_address = addr;
        break;
    }
    CHATTER(dev, ".");
    write_count -= this_write;
    addr        += this_write;
    ram         += this_write;
  }
  FLASH_Disable(flash_base, end_addr);
  HAL_FLASH_CACHES_ON(d_cache, i_cache);
  CHATTER(dev, "\n");
  UNLOCK(dev);
  if (stat != CYG_FLASH_ERR_OK) {
    return (stat);
  }
  if (len > (dev->end + 1 - flash_base)) {
    return cyg_flash_program(dev->end+1, ram, 
                             len - (dev->end + 1 - flash_base),
                             err_address);
  }
  return CYG_FLASH_ERR_OK;
}

__externC int 
cyg_flash_read(const cyg_flashaddr_t flash_base, 
               void *ram_base, 
               size_t len, 
               cyg_flashaddr_t *err_address)
{
  struct cyg_flash_dev * dev;
  cyg_flashaddr_t addr, end_addr;
  unsigned char * ram = (unsigned char *)ram_base;
  size_t read_count;
  int stat = CYG_FLASH_ERR_OK;

  dev = find_dev(flash_base, &stat);
  if (!dev) return stat;

  LOCK(dev);
  addr = flash_base;
  if (len > (dev->end + 1 - flash_base)) {
      end_addr = dev->end;
  } else {
      end_addr = flash_base + len - 1;
  }
  read_count = (end_addr + 1) - flash_base;

  //  CHATTER(dev, "... Read from %p-%p to %p: ", addr, end_addr, ram_base);

  // If the flash is directly accessible, just read it in one go. This
  // still happens with the mutex locked to protect against concurrent
  // programs/erases.
  if (! dev->funs->flash_read) {
      memcpy(ram, (void*)addr, read_count);
  } else {
#ifndef CYGHWR_IO_FLASH_INDIRECT_READS
      CYG_FAIL("read function supplied but indirect reads not enabled");
      stat = CYG_FLASH_ERR_PROTOCOL;
      if (err_address) {
          *err_address = addr;
      }
#else
      // We have to indirect through the device driver.
      // The first read may be in the middle of a block. Do the necessary
      // adjustment here rather than inside the loop.
      size_t            offset;
      cyg_flashaddr_t   block = flash_block_begin(flash_base, dev);
      HAL_FLASH_CACHES_STATE(d_cache, i_cache);
      if (addr == block) {
          offset = 0;
      } else {
          offset = addr - block;
      }
      HAL_FLASH_CACHES_OFF(d_cache, i_cache);
      FLASH_Enable(flash_base, end_addr);
      while (read_count > 0) {
          size_t block_size = flash_block_size(dev, addr);
          size_t this_read;
          if (read_count > (block_size - offset)) {
              this_read = block_size - offset;
          } else {
              this_read = read_count;
          }
          // Only the first block may need the offset
          offset      = 0;
    
          stat = dev->funs->flash_read(dev, addr, ram, this_read);
          if (CYG_FLASH_ERR_OK != stat && err_address) {
              *err_address = addr;
              break;
          }
          //          CHATTER(dev, ".");
          read_count  -= this_read;
          addr        += this_read;
          ram         += this_read;
      }
      FLASH_Disable(flash_base, end_addr);
      HAL_FLASH_CACHES_ON(d_cache, i_cache);
#endif      
  }
  //  CHATTER(dev, "\n");
  UNLOCK(dev);
  if (stat != CYG_FLASH_ERR_OK) {
    return (stat);
  }
  if (len > (dev->end + 1 - flash_base)) {
      return cyg_flash_read(dev->end+1, ram,
                            len - (dev->end + 1 - flash_base),
                            err_address);
  }
  return CYG_FLASH_ERR_OK;
}

#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING
__externC int 
cyg_flash_lock(const cyg_flashaddr_t flash_base, 
               size_t len, 
               cyg_flashaddr_t *err_address)
{
  cyg_flashaddr_t block, end_addr;
  struct cyg_flash_dev * dev;
  size_t lock_count;
  int stat = CYG_FLASH_ERR_OK;
  HAL_FLASH_CACHES_STATE(d_cache, i_cache);

  dev = find_dev(flash_base, &stat);
  if (!dev) return stat;
  if (!dev->funs->flash_block_lock) return CYG_FLASH_ERR_INVALID;

  CHECK_SOFT_WRITE_PROTECT(flash_base, len);
  
  LOCK(dev);
  if (len > (dev->end + 1 - flash_base)) {
      end_addr = dev->end;
  } else {
      end_addr = flash_base + len - 1;
  }
  block         = flash_block_begin(flash_base, dev);
  lock_count    = (end_addr + 1) - block;
  
  CHATTER(dev, "... Locking from %p-%p: ", (void*)block, (void*)end_addr);
  
  HAL_FLASH_CACHES_OFF(d_cache, i_cache);
  FLASH_Enable(flash_base, end_addr);
  while (lock_count > 0) {
    size_t  block_size  = flash_block_size(dev, block);
    if (lock_count < block_size) {
        lock_count = block_size;
    }
    stat = dev->funs->flash_block_lock(dev,block);
    
    if (CYG_FLASH_ERR_OK != stat && err_address) {
      *err_address = block;
      break;
    }
    block       += block_size;
    lock_count  -= block_size;
    CHATTER(dev, ".");
  }
  FLASH_Disable(flash_base, end_addr);
  HAL_FLASH_CACHES_ON(d_cache, i_cache);
  CHATTER(dev, "\n");
  UNLOCK(dev);
  if (stat != CYG_FLASH_ERR_OK) {
    return stat;
  }

  // Recurse if necessary for the next device
  if (len > (dev->end + 1 - flash_base)) {
    return cyg_flash_lock(dev->end+1, 
                          len - (dev->end + 1 - flash_base),
                          err_address);
  }

  return CYG_FLASH_ERR_OK;
}

__externC int 
cyg_flash_unlock(const cyg_flashaddr_t flash_base, 
                 size_t len, 
                 cyg_flashaddr_t *err_address)
{
  cyg_flashaddr_t block, end_addr;
  struct cyg_flash_dev * dev;
  size_t unlock_count;
  int stat = CYG_FLASH_ERR_OK;
  HAL_FLASH_CACHES_STATE(d_cache, i_cache);

  dev = find_dev(flash_base, &stat);
  if (!dev) return stat;
  if (!dev->funs->flash_block_unlock) return CYG_FLASH_ERR_INVALID;

  CHECK_SOFT_WRITE_PROTECT(flash_base, len);
  
  LOCK(dev);
  if (len > (dev->end + 1 - flash_base)) {
      end_addr = dev->end;
  } else {
      end_addr = flash_base + len - 1;
  }
  block         = flash_block_begin(flash_base, dev);
  unlock_count  = (end_addr + 1) - block;
  
  CHATTER(dev, "... Unlocking from %p-%p: ", (void*)block, (void*)end_addr);
  
  HAL_FLASH_CACHES_OFF(d_cache, i_cache);
  FLASH_Enable(flash_base, end_addr);
  while (unlock_count > 0) {
    size_t    block_size  = flash_block_size(dev, block);
    if (unlock_count < block_size) {
        unlock_count = block_size;
    }
    stat = dev->funs->flash_block_unlock(dev,block);
    
    if (CYG_FLASH_ERR_OK != stat && err_address) {
      *err_address = block;
      break;
    }
    block           += block_size;
    unlock_count    -= block_size;
    
    CHATTER(dev, ".");
  }
  FLASH_Disable(flash_base, end_addr);
  HAL_FLASH_CACHES_ON(d_cache, i_cache);
  CHATTER(dev, "\n");
  UNLOCK(dev);
  if (stat != CYG_FLASH_ERR_OK) {
    return stat;
  }
  
  // Recurse if necessary for the next device
  if (len > (dev->end + 1 - flash_base)) {
    return cyg_flash_lock(dev->end+1, 
                          len - (dev->end + 1 - flash_base),
                          err_address);
  }
  return CYG_FLASH_ERR_OK;
}
#endif

const char *
cyg_flash_errmsg(const int err)
{
    switch (err) {
    case CYG_FLASH_ERR_OK:
        return "No error - operation complete";
    case CYG_FLASH_ERR_ERASE_SUSPEND:
        return "Device is in erase suspend state";
    case CYG_FLASH_ERR_PROGRAM_SUSPEND:
        return "Device is in program suspend state";
    case CYG_FLASH_ERR_INVALID:
        return "Invalid FLASH address";
    case CYG_FLASH_ERR_ERASE:
        return "Error trying to erase";
    case CYG_FLASH_ERR_LOCK:
        return "Error trying to lock/unlock";
    case CYG_FLASH_ERR_PROGRAM:
        return "Error trying to program";
    case CYG_FLASH_ERR_PROTOCOL:
        return "Generic error";
    case CYG_FLASH_ERR_PROTECT:
        return "Device/region is write-protected";
    case CYG_FLASH_ERR_NOT_INIT:
        return "FLASH sub-system not initialized";
    case CYG_FLASH_ERR_DRV_VERIFY:
        return "Data verify failed after operation";
    case CYG_FLASH_ERR_DRV_TIMEOUT:
        return "Driver timed out waiting for device";
    case CYG_FLASH_ERR_DRV_WRONG_PART:
        return "Driver does not support device";
    case CYG_FLASH_ERR_LOW_VOLTAGE:
        return "Device reports low voltage";
    default:
        return "Unknown error";
    }
}

// Dummy routines to put into the device function tables, to handle
// unsupported/unnecessary functionality. For example not all devices
// support block locking.
//
// A dummy initialization routine, for platforms where everything is
// done statically and there is no need to check device ids or
// anything similar.
int
cyg_flash_devfn_init_nop(struct cyg_flash_dev* dev)
{
    CYG_UNUSED_PARAM(struct cyg_flash_dev*, dev);
    return CYG_FLASH_ERR_OK;
}

// A dummy query routine. The implementation of this is specific to
// each device driver, so some device drivers may choose to do
// nothing.
size_t
cyg_flash_devfn_query_nop(struct cyg_flash_dev* dev, void* data, size_t len)
{
    CYG_UNUSED_PARAM(struct cyg_flash_dev*, dev);
    CYG_UNUSED_PARAM(void*, data);
    CYG_UNUSED_PARAM(size_t, len);
    return 0;
}

// Dummy lock/unlock routines
int
cyg_flash_devfn_lock_nop(struct cyg_flash_dev* dev, const cyg_flashaddr_t addr)
{
    CYG_UNUSED_PARAM(struct cyg_flash_dev*, dev);
    CYG_UNUSED_PARAM(cyg_flashaddr_t, addr);
#if defined(CYGHWR_IO_FLASH_BLOCK_LOCKING) && (1 < CYGHWR_IO_FLASH_DEVICE)
// If we've been built with locking, and there's more than one flash
// device in the system, then this is probably only being called because
// we can't tell what devices do and don't support locking, and for a _nop
// function this is the device that doesn't support locking. So we don't
// complain if we're asked to.
    return CYG_FLASH_ERR_OK;
#else
    return CYG_FLASH_ERR_DRV_WRONG_PART;
#endif
}

int
cyg_flash_devfn_unlock_nop(struct cyg_flash_dev* dev, const cyg_flashaddr_t addr)
{
    CYG_UNUSED_PARAM(struct cyg_flash_dev*, dev);
    CYG_UNUSED_PARAM(cyg_flashaddr_t, addr);
#if defined(CYGHWR_IO_FLASH_BLOCK_LOCKING) && (1 < CYGHWR_IO_FLASH_DEVICE)
// If we've been built with locking, and there's more than one flash
// device in the system, then this is probably only being called because
// we can't tell what devices do and don't support locking, and for a _nop
// function this is the device that doesn't support locking. So we don't
// complain if we're asked to.
    return CYG_FLASH_ERR_OK;
#else
    return CYG_FLASH_ERR_DRV_WRONG_PART;
#endif
}

// On some architectures there are problems calling the .2ram
// functions from the main ones. Specifically the compiler may issue a
// short call, even though the flash and ram are too far apart. The
// solution is to indirect via a function pointer, but the simplistic
// approach is vulnerable to compiler optimization. Hence the function
// pointer is passed through an anonymizer.
void*
cyg_flash_anonymizer(void* fn)
{
    return fn;
}

// EOF io/flash/..../flash.c
