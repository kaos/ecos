//==========================================================================
//
//      flash.h
//
//      Flash programming - external interfaces
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
// Date:         2000-07-14
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef _IO_FLASH_H_
#define _IO_FLASH_H_

#include <pkgconf/io_flash.h>

#define FLASH_MIN_WORKSPACE 0x10000  // Space used by FLASH code

extern int flash_init(void *work_space, int work_space_length);
extern int flash_erase(void *base, int len, void **err_address);
extern int flash_program(void *flash_base, void *ram_base, int len, void **err_address);
#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING
extern int flash_lock(void *base, int len, void **err_address);
extern int flash_unlock(void *base, int len, void **err_address);
#endif
extern int flash_verify_addr(void *base);
extern int flash_get_limits(void *base, void **start, void **end);
extern int flash_get_block_info(int *block_size, int *blocks);
extern bool flash_code_overlaps(void *start, void *end);
extern char *flash_errmsg(int err);

#define FLASH_ERR_OK       0x00  // No error - operation complete
#define FLASH_ERR_INVALID  0x01  // Invalid FLASH address
#define FLASH_ERR_ERASE    0x02  // Error trying to erase
#define FLASH_ERR_LOCK     0x03  // Error trying to lock/unlock
#define FLASH_ERR_PROGRAM  0x04  // Error trying to program
#define FLASH_ERR_PROTOCOL 0x05  // Generic error
#define FLASH_ERR_PROTECT  0x06  // Device/region is write-protected
#define FLASH_ERR_NOT_INIT 0x07  // FLASH info not yet initialized
#define FLASH_ERR_HWR      0x08  // Hardware (configuration?) problem

#ifdef _FLASH_PRIVATE_

struct flash_info {
    void *work_space;
    int   work_space_size;
    int   block_size;   // Assuming fixed size "blocks"
    int   blocks;       // Number of blocks
    unsigned long block_mask;
    void *start, *end;  // Address range
    int   init;
};

extern struct flash_info flash_info;
extern int  flash_hwr_init(void);
extern int  flash_hwr_map_error(int err);

#endif

#endif  // _IO_FLASH_H_
