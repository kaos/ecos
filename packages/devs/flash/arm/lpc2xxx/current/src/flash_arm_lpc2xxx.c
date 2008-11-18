//==========================================================================
//
//      flash_arm_lpc2xxx.c
//
//      Flash programming for LPC2xxx
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
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
// Author(s):    Hans Rosenfeld <rosenfeld@grumpf.hope-2000.org>
// Contributors: 
// Date:         2007-07-12
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/devs_flash_arm_lpc2xxx.h>
#include <pkgconf/hal.h>
#include <pkgconf/hal_arm.h>
#include <cyg/hal/hal_intr.h>

#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>

#include "flash_arm_lpc2xxx.h"

/* gcc builtins */
extern void* memcpy(void *, const void *, size_t);
extern void* memset(void *, int, size_t);

/* wrapper for simpler IAP access */
static void
iap(struct iap_param *param, struct iap_param *result)
{
  static void (* const iap)(struct iap_param *, struct iap_param *)
    = (void (*)(struct iap_param *, struct iap_param *)) IAP_LOCATION;
  cyg_uint32 cpsr;
  
  HAL_DISABLE_INTERRUPTS(cpsr);
  iap(param, result);
  HAL_RESTORE_INTERRUPTS(cpsr);
}

void 
flash_query(void *data)
{
  /* nothing to do here */
}

/*
 * 248k in 31 blocks by 8k there actually less blocks since two of
 * them are 64k, but accessing anything but the last few 8k blocks is
 * not supported anyway
 */
int 
flash_hwr_init(void)
{
  flash_info.block_size = 8 * 1024;
  flash_info.blocks = 31;
  flash_info.start = (void *) 0;
  flash_info.end = (void *) (248 * 1024);
  
  return FLASH_ERR_OK;
}


static const cyg_uint8 flash_errors[12] = {
  FLASH_ERR_OK,          /* IAP_CMD_SUCCESS */
  FLASH_ERR_PROTOCOL,    /* IAP_INV_COMMAND */
  FLASH_ERR_INVALID,     /* IAP_SRC_ADDRERR */
  FLASH_ERR_INVALID,     /* IAP_DST_ADDRERR */
  FLASH_ERR_INVALID,     /* IAP_SRC_ADDRMAP */
  FLASH_ERR_INVALID,     /* IAP_DST_ADDRMAP */
  FLASH_ERR_INVALID,     /* IAP_CNT_INVALID */
  FLASH_ERR_INVALID,     /* IAP_SEC_INVALID */
  FLASH_ERR_PROTOCOL,    /* IAP_SEC_NOTBLNK */
  FLASH_ERR_PROTOCOL,    /* IAP_SEC_NOTPREP */
  FLASH_ERR_DRV_VERIFY,  /* IAP_CMP_INEQUAL */
  FLASH_ERR_DRV_TIMEOUT, /* IAP_BSY         */
};

int
flash_hwr_map_error(e)
{
  if(e > 11)
    return FLASH_ERR_PROTOCOL;
  return flash_errors[e];
}

/* this will not work for flash addresses < 0x30000 */
static int
block_by_addr(cyg_uint32 addr)
{
  int block;
  
  block = (addr >> 13) & 0x1f;
  block -= 14;
  
  return block;
}

int
flash_erase_block(void *block, unsigned int size)
{
  struct iap_param param, result;
  
  param.code = IAP_PREPARE;
  
  param.p[0] = param.p[1] = block_by_addr((cyg_uint32) block);
  if(param.p[0] < 10)
    return FLASH_ERR_INVALID;
  
  /* prepare sector(s) */
  iap(&param, &result);
  if(result.code != IAP_CMD_SUCCESS)
    return result.code;
  
  param.code = IAP_ERASE;
  param.p[2] = CYGNUM_HAL_ARM_LPC2XXX_CLOCK_SPEED / 1000;
  
  /* erase sector(s) */
  iap(&param, &result);
  return result.code;
}

int
flash_program_buf(void *addr, void *data, int len)
{
  static const int size = CYGPKG_DEVS_FLASH_ARM_LPC2XXX_BUFSIZE;
  static const cyg_uint32 b = (0x40004000 - 32 - 
                               CYGPKG_DEVS_FLASH_ARM_LPC2XXX_BUFSIZE);
  static void * const buf = (void *) (0x40004000 - 32 - 
                                      CYGPKG_DEVS_FLASH_ARM_LPC2XXX_BUFSIZE);
  cyg_uint32 a = (cyg_uint32) addr;
  char *d = (char *) data;
  struct iap_param param, result;
  
  param.code = IAP_PREPARE;
  param.p[0] = block_by_addr(a);
  param.p[1] = block_by_addr(a + len - 1);
  if(param.p[0] < 10 || param.p[1] > 16)
    return FLASH_ERR_INVALID;
  
  do {
    /* prepare sector(s) */
    iap(&param, &result);
    if(result.code != IAP_CMD_SUCCESS)
      return result.code;
    
    if(len < size)
      memset(buf, 0xff, size);
    memcpy(buf, d, size);
    
    param.code = IAP_COPY;
    param.p[0] = a;
    param.p[1] = b;
    param.p[2] = size;
    param.p[3] = CYGNUM_HAL_ARM_LPC2XXX_CLOCK_SPEED / 1000;
    
    /* copy ram to flash */
    iap(&param, &result);
    if(result.code != IAP_CMD_SUCCESS)
      return result.code;
    
    len -= size;
    a += size;
    d += size;
  } while(len > 0);
  
  return(result.code);
}

bool
flash_code_overlaps(void *start, void *end)
{
  extern unsigned char _stext[], _etext[];
  
  return ((((unsigned long)&_stext >= (unsigned long)start) &&
           ((unsigned long)&_stext < (unsigned long)end)) ||
          (((unsigned long)&_etext >= (unsigned long)start) &&
           ((unsigned long)&_etext < (unsigned long)end)));
}
