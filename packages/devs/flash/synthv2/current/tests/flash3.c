/* Hay, the copyright is usefull for something! */

static char copyright[] = 
"//=========================================================================="
"//"
"//      flash1.c"
"//"
"//      Test flash operations for the synth target synth flash driver"
"//"
"//=========================================================================="
"//####ECOSGPLCOPYRIGHTBEGIN####"
"// -------------------------------------------"
"// This file is part of eCos, the Embedded Configurable Operating System."
"// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc."
"//"
"// eCos is free software; you can redistribute it and/or modify it under"
"// the terms of the GNU General Public License as published by the Free"
"// Software Foundation; either version 2 or (at your option) any later version."
"//"
"// eCos is distributed in the hope that it will be useful, but WITHOUT ANY"
"// WARRANTY; without even the implied warranty of MERCHANTABILITY or"
"// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License"
"// for more details."
"//"
"// You should have received a copy of the GNU General Public License along"
"// with eCos; if not, write to the Free Software Foundation, Inc.,"
"// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA."
"//"
"// As a special exception, if other files instantiate templates or use macros"
"// or inline functions from this file, or you compile this file and link it"
"// with other works to produce a work based on this file, this file does not"
"// by itself cause the resulting work to be covered by the GNU General Public"
"// License. However the source code for this file must still be made available"
"// in accordance with section (3) of the GNU General Public License."
"//"
"// This exception does not invalidate any other reasons why a work based on"
"// this file might be covered by the GNU General Public License."
"//"
"// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc."
"// at http://sources.redhat.com/ecos/ecos-license/"
"// -------------------------------------------"
"//####ECOSGPLCOPYRIGHTEND####"
"//=========================================================================="
"//#####DESCRIPTIONBEGIN####"
"//"
"// Author(s):           andrew.lunn@ascom.ch"
"// Contributors:        andrew.lunn"
"// Date:                2000-10-31"
"// Purpose:             Test a flash driver"
"// Description:         Try out a number of flash operations and make sure"
"//                      what is in the flash is what we expeect."
"//                      "
"//####DESCRIPTIONEND####"
"//"
"//==========================================================================&"
;

#include <pkgconf/system.h>
#ifdef CYGPKG_DEVS_FLASH_SYNTH_V2
#include <pkgconf/devs_flash_synth_v2.h>
#endif
#ifdef CYGPKG_DEVS_FLASH_SYNTH
#include <pkgconf/devs_flash_synth.h>
#endif
#include <cyg/io/flash.h>
#include <cyg/infra/testcase.h>
#include <cyg/infra/diag.h>
#include <cyg/flash/synth.h>

#include <string.h>

#ifndef CYGINT_ISO_STRING_STRFUNCS
# define NA_MSG "Need string functions for test"
#endif

#ifdef NA_MSG
void cyg_user_start(void)
{
    CYG_TEST_INIT();
    CYG_TEST_NA( NA_MSG );
}
#else

static struct cyg_flash_synth_priv synth_flash_priv3 = {
    .block_size         = CYGNUM_FLASH_SYNTH_V2_BLOCKSIZE,
    .blocks             = CYGNUM_FLASH_SYNTH_V2_NUMBLOCKS,
    .boot_block_size    = CYGNUM_FLASH_SYNTH_V2_BOOT_BLOCKSIZE,
    .boot_blocks        = CYGNUM_FLASH_SYNTH_V2_NUMBOOT_BLOCKS,
    .boot_block_bottom  = CYGNUM_FLASH_SYNTH_V2_BOOT_BLOCK_BOTTOM,
    .filename           = "synth.flash3",
    .flashfd            = -1
};

CYG_FLASH_DRIVER(cyg_flash_synth_flashdev_flash3,
                 &cyg_flash_synth_funs,
                 0,             // flags
                 0x40020000,    // start
                 0,             // end, filled in by init
                 0,             // number of block_info's, filled in by init
                 synth_flash_priv3.block_info,
                 &synth_flash_priv3);

//==========================================================================
// main

void cyg_user_start(void)
{
    int ret;
    cyg_flashaddr_t flash_start=0, flash_end=0;
    cyg_flash_info_t info;
    cyg_uint32 i=0;
    cyg_uint32 j;
    int block_size=0, blocks=0;
    cyg_flashaddr_t prog_start;
    unsigned char * ptr;
    size_t copyright_len;
    
    CYG_TEST_INIT();

    // Reference the flash dev so the linker does not throw it away
    CYG_REFERENCE_OBJECT(cyg_flash_synth_flashdev_flash3);  

    ret=cyg_flash_init((cyg_flash_printf *)diag_printf);
    
    CYG_TEST_PASS_FAIL((ret == CYG_FLASH_ERR_OK),"flash_init");

    do {
      ret = cyg_flash_get_info(i, &info);
      if (ret == CYG_FLASH_ERR_OK) {
        diag_printf("INFO: Nth=%d, start=%p, end=%p\n",
                    i, info.start, info.end);
        if (i == 0) {
          flash_start = info.start;
          flash_end = info.end;
          block_size = info.block_info[0].block_size;
          blocks = info.block_info[0].blocks;
        }
        
        for (j=0;j < info.num_block_infos; j++) {
          diag_printf("INFO:\t block_size %d, blocks %d\n",
                      info.block_info[j].block_size,
                      info.block_info[j].blocks);
        }
      }
      i++;
    } while (ret != CYG_FLASH_ERR_INVALID);
    
    /* Erase the whole flash. Not recommended on real hardware since this
     will probably erase the bootloader etc!!! */
    ret=cyg_flash_erase(flash_start,block_size * blocks,NULL);
    CYG_TEST_PASS_FAIL((ret == CYG_FLASH_ERR_OK),"flash_erase1");

    /* check that its actually been erased, and test the mmap area */
    for (ptr=(unsigned char *)flash_start,ret=0; 
         ptr <= (unsigned char *)flash_end; ptr++) {
        if (*ptr != 0xff) {
            ret++;
        }
    }
  
    CYG_TEST_PASS_FAIL((ret == 0),"flash empty check");

    // With small blocks we have to use less of the copyright messages
    // Since we make assumptions about fitting the message into a
    // block.
    if (block_size < sizeof(copyright)*2/3) {
      copyright_len = block_size / 3;
    } else {
      copyright_len = sizeof(copyright);
    }
    
    ret = cyg_flash_program(flash_start,&copyright,copyright_len,NULL);
    CYG_TEST_PASS_FAIL((ret == CYG_FLASH_ERR_OK),"flash_program1");
  
    /* Check the contents made it into the flash */
    CYG_TEST_PASS_FAIL(!strncmp((void *)flash_start,
                                copyright,copyright_len),
                       "flash program contents");

    /* .. and check nothing else changed */
    for (ptr=(unsigned char *)flash_start+copyright_len,ret=0; 
         ptr < (unsigned char *)flash_end; ptr++) {
        if (*ptr != 0xff) {
            ret++;
        }
    }
  
    CYG_TEST_PASS_FAIL((ret == 0),"flash program overrun check");

    prog_start = flash_start + block_size - copyright_len/2;
    ret = cyg_flash_program(prog_start,&copyright,copyright_len,NULL);
    CYG_TEST_PASS_FAIL((ret == CYG_FLASH_ERR_OK),"flash_program2");
  
    /* Check the first version is still OK */
    CYG_TEST_PASS_FAIL(!strncmp((void *)flash_start,
                                copyright,
                                copyright_len),
                       "Original contents");
  
    CYG_TEST_PASS_FAIL(!strncmp((void *)prog_start,
                                copyright,
                                copyright_len),
                       "New program contents");

    /* Check the bit in between is still erased */
    for (ptr=(unsigned char *)flash_start+copyright_len,ret=0; 
         ptr < (unsigned char *)prog_start; ptr++) {
        if (*ptr != 0xff) {
            ret++;
        }
    }
    CYG_TEST_PASS_FAIL((ret == 0),"flash erase check1");
  
    /* Erase the second block and make sure the first is not erased */
    ret=cyg_flash_erase(flash_start+block_size,block_size,NULL);
    CYG_TEST_PASS_FAIL((ret == CYG_FLASH_ERR_OK),"flash_erase2");

    /* Check the erase worked */
    for (ptr=(unsigned char *)flash_start+block_size,ret=0; 
         ptr < (unsigned char *)flash_start+block_size*2; 
         ptr++) {
        if (*ptr != 0xff) {
            ret++;
        }
    }

    CYG_TEST_PASS_FAIL((ret == 0), "flash erase check2");
  
    /* Lastly check the first half of the copyright message is still there */
    CYG_TEST_PASS_FAIL(!strncmp((void *)prog_start,
                                copyright,
                                copyright_len/2),
                       "Block 1 OK");

#if 0
    /* This test is be fatal! Its not run by default!
     Check the flash is read only, by trying to write to it. We expect
     to get an exception */

    *(char *)flash_start = 'a';
#endif

    CYG_TEST_PASS_FINISH("flash3");
}

#endif /* ifndef NA_MSG */

/* EOF flash1.c */
