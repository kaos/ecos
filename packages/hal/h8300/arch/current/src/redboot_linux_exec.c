//==========================================================================
//
//      redboot_linux_exec.c
//
//      RedBoot exec command for Linux booting
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
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    yoshinori sato
// Contributors: yoshinori sato
// Date:         2002-05-28
// Purpose:      RedBoot exec command for uClinux booting
//
//####DESCRIPTIONEND####
//
//===========================================================================

#include <redboot.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>

#define xstr(s) str(s)
#define str(s...) #s

static void do_exec(int argc, char *argv[]);
RedBoot_cmd("exec", 
            "Execute an image", 
            "[-b <command line addr>] [-c \"kernel command line\"]\n"
	    "        [<entry point>]",
            do_exec
    );

static void 
do_exec(int argc, char *argv[])
{
    cyg_uint32 entry = CYGDAT_REDBOOT_H8300_LINUX_BOOT_ENTRY;
    cyg_uint32 command_addr = CYGDAT_REDBOOT_H8300_LINUX_COMMAND_START;
    char *cmd_line = xstr( CYGDAT_REDBOOT_H8300_LINUX_BOOT_COMMAND_LINE );
    
    bool command_addr_set,command_line_set;

    struct option_info opts[2];
    char *pcmd;
    int oldints;

    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void **)&command_addr, &command_addr_set, "command line address");
    init_opts(&opts[1], 'c', true, OPTION_ARG_TYPE_STR, 
              (void **)&cmd_line, &command_line_set, "kernel command line");
    
    if (!scan_opts(argc, argv, 1, opts, 2, (void *)&entry, 
                   OPTION_ARG_TYPE_NUM, "entry address"))
	    return ;
  
    diag_printf("Now booting linux kernel:\n");
    diag_printf(" Entry Address 0x%08x\n", entry);
    diag_printf(" Cmdline : %s\n", cmd_line);

    HAL_DISABLE_INTERRUPTS(oldints);
    HAL_DCACHE_SYNC();
    HAL_ICACHE_DISABLE();
    HAL_DCACHE_DISABLE();
    HAL_DCACHE_SYNC();
    HAL_ICACHE_INVALIDATE_ALL();
    HAL_DCACHE_INVALIDATE_ALL();

    pcmd = (char *)command_addr;
    while ((*pcmd++ = *cmd_line++));

    asm ("jmp @%0" : : "r" (entry));
}
