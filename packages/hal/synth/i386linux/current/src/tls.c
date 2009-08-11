//=============================================================================
//
//      tls.c
//
//      Support for the per-thread data expected by parts of gcc
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2009 Free Software Foundation, Inc.                  
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    bartv
// Contributors: bartv
// Date:         2009-08-09
//
//####DESCRIPTIONEND####
//=============================================================================

#include <pkgconf/mlt_synth_i386_rom.h>
#include <cyg/hal/hal_arch.h>

// At the time of writing some parts of gcc assume thread-local storage as
// provided by glibc. Of particular concern is -fstack-protector which is
// enabled by default in some distros. Without matching target-side support
// this causes synthetic target eCos applications to SEGV early on.

// eCos only needs a single TLS data structure. This will need alignment
// suitable for filling in an x86 descriptor table entry, so the linker
// script places this structure right at the start of RAM.
//
// Installing a TLS area involves a system call set_thread_area().
// This system call should be called from the assembler startup
// before any C code starts running, or we'll run into problems with
// -fstack-protector-all. However all relevant data can be
// statically initialized.

_HAL_TLS_Data _hal_synth_tls_data __attribute__ ((section (".tls._hal_synth_tls_data") )) = {
    // The meaning of most of the tls fields is not clear, so just
    // initialize them to 0 for now - until something else
    // stops working.
    .tls_tcb                = (void*) 0,
    .tls_dtv                = (void*) 0,
    .tls_self               = (void*) 0,
    .tls_multiple_threads   = 0,
    .tls_sysinfo            = (void*) 0,
    // This is the important one for the purposes of -fstack-protector.
    // The compiler assumes that %gs:0x14 points at the base of the stack.
    // For now we just point at the start of RAM. It should be possible
    // to update this during context switches and at the start of interrupt
    // handling to get a partial implementation of stack overflow checking.
    .tls_stack_guard        = (void*) CYGMEM_REGION_ram,

    .tls_pointer_guard      = (void*) 0,
    .tls_gscope_flag        = 0,
    .tls_private_futex      = 0,
    .tls_private_tm[0]      = (void*) 0,
    .tls_private_tm[1]      = (void*) 0,
    .tls_private_tm[2]      = (void*) 0,
    .tls_private_tm[3]      = (void*) 0,
    .tls_private_tm[4]      = (void*) 0
};

// The argument to set_thread_area() is not a _hal_synth_tls_data,
// unfortunately. Instead it is a user_desc structure as per
// <asm/ldt.h>.
typedef struct _HAL_user_desc {
    int             ud_entry_number;
    unsigned long   ud_base_addr;
    int             ud_limits;
    int             ud_flags;
} _HAL_user_desc;

#define _HAL_USER_DESC_FLAGS_SEG_32BIT          (0x01 << 0)
#define _HAL_USER_DESC_FLAGS_CONTENTS_MASK      (0x03 << 1)
#define _HAL_USER_DESC_FLAGS_CONTENTS_SHIFT     1
#define _HAL_USER_DESC_FLAGS_CONTENTS_DATA      (0x00 << 1)
#define _HAL_USER_DESC_FLAGS_CONTENTS_STACK     (0x01 << 1)
#define _HAL_USER_DESC_FLAGS_CONTENTS_CODE      (0x02 << 1)
#define _HAL_USER_DESC_FLAGS_READ_EXEC_ONLY     (0x01 << 3)
#define _HAL_USER_DESC_FLAGS_LIMIT_IN_PAGES     (0x01 << 4)
#define _HAL_USER_DESC_FLAGS_SEG_NOT_PRESENT    (0x01 << 5)
#define _HAL_USER_DESC_FLAGS_USEABLE            (0x01 << 6)


// And it can also be statically initialized. There is no need to worry
// about alignment this time. Note that one of the fields gets updated
// by the system call so this cannot be a const structure.

_HAL_user_desc   _hal_synth_user_desc = {
    .ud_entry_number        = -1,   // Let the kernel pick the descriptor table entry
    .ud_base_addr           = (unsigned long) &_hal_synth_tls_data,
    .ud_limits              = 1,    // A single page will do
    .ud_flags               = (_HAL_USER_DESC_FLAGS_SEG_32BIT       |
                               _HAL_USER_DESC_FLAGS_CONTENTS_DATA   |
                               _HAL_USER_DESC_FLAGS_READ_EXEC_ONLY  |
                               _HAL_USER_DESC_FLAGS_LIMIT_IN_PAGES  |
                               _HAL_USER_DESC_FLAGS_USEABLE)
};
