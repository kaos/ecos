//==========================================================================
//
//      synth.c
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
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    andrew.lunn@ascom.ch
// Contributors: jlarmour
// Date:         2001-10-30
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/devs_flash_synth.h>

#include <cyg/hal/hal_io.h>
#include <cyg/infra/cyg_ass.h>

#define  _FLASH_PRIVATE_
#include <cyg/io/flash.h>

#include "synth.h"

/* Holds the fd for the flash file */
int cyg_dev_flash_synth_flashfd;

/* Holds the base address of the mmap'd region */
flash_t *cyg_dev_flash_synth_base;

/* Helper function. The Linux system call cannot pass 6 parameters. Instead
   a structure is filled in and passed as one parameter */
static int 
cyg_hal_sys_do_mmap(void *addr, unsigned long length, unsigned long prot, 
                    unsigned long flags, unsigned long fd, unsigned long off)
{

    struct cyg_hal_sys_mmap_args args;
  
    args.addr = (unsigned long) addr;
    args.len = length;
    args.prot = prot = prot;
    args.flags = flags;
    args.fd = fd;
    args.offset = off;

    return (cyg_hal_sys_mmap(&args));
}           

int
flash_hwr_init(void)
{
    flash_info.block_size = CYGNUM_FLASH_SYNTH_BLOCKSIZE;
    flash_info.buffer_size = 0;
    flash_info.blocks = CYGNUM_FLASH_SYNTH_NUMBLOCKS;

    cyg_dev_flash_synth_flashfd = cyg_hal_sys_open(CYGDAT_FLASH_SYNTH_FILENAME, 
                CYG_HAL_SYS_O_RDWR|CYG_HAL_SYS_O_CREAT, 
                CYG_HAL_SYS_S_IRWXU|CYG_HAL_SYS_S_IRWXG|CYG_HAL_SYS_S_IRWXO);
    CYG_ASSERT( cyg_dev_flash_synth_flashfd >= 0, 
                "Opening of the file for the synth flash failed!");
    if ( cyg_dev_flash_synth_flashfd <= 0 ) {
        return FLASH_ERR_HWR;
    }
    cyg_dev_flash_synth_base = cyg_hal_sys_do_mmap( NULL,
                (CYGNUM_FLASH_SYNTH_BLOCKSIZE * CYGNUM_FLASH_SYNTH_NUMBLOCKS),
                CYG_HAL_SYS_PROT_READ, CYG_HAL_SYS_MAP_PRIVATE,
                cyg_dev_flash_synth_flashfd,
                0 );
    CYG_ASSERT( cyg_dev_flash_synth_base > 0, "mmap of flash file failed!" );

    if (cyg_dev_flash_synth_base <= 0) {
        return FLASH_ERR_HWR;
    }
    flash_info.start = cyg_dev_flash_synth_base;
    flash_info.end = (void *)(((char *)cyg_dev_flash_synth_base) +
        (CYGNUM_FLASH_SYNTH_BLOCKSIZE * CYGNUM_FLASH_SYNTH_NUMBLOCKS));

    return FLASH_ERR_OK;
}

// Map a hardware status to a package error
int
flash_hwr_map_error(int err)
{
    return err;
}

// See if a range of FLASH addresses overlaps currently running code
bool
flash_code_overlaps(void *start, void *end)
{
    extern char _stext[], _etext[];

    return ((((unsigned long)&_stext >= (unsigned long)start) &&
             ((unsigned long)&_stext < (unsigned long)end)) ||
            (((unsigned long)&_etext >= (unsigned long)start) &&
             ((unsigned long)&_etext < (unsigned long)end)));
}

// EOF synth.c
