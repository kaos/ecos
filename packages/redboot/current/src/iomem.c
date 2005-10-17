//==========================================================================
//
//      iomem.c
//
//      RedBoot I/O memory peek and poke
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2004 eCosCentric Ltd
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
// Author(s):    icampbell
// Contributors: icampbell
// Date:         2004-11-09
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <redboot.h>
#include <cyg/hal/hal_io.h>

RedBoot_cmd("iopeek",
	    "Read I/O location",
	    "[-b <location>] [-1|2|4]",
	    do_iopeek
    );
RedBoot_cmd("iopoke",
	    "Write I/O location",
	    "[-b <location>] [-1|2|4] -v <value>",
	    do_iopoke
    );

void
do_iopoke(int argc, char *argv[])
{
    struct option_info opts[5];
    unsigned long base;
    bool base_set, value_set;
    bool set_32bit = false;
    bool set_16bit = false;
    bool set_8bit = false;
    cyg_uint32 value;
    int size = 1;

    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              &base, &base_set, "base address");
    init_opts(&opts[1], 'v', true, OPTION_ARG_TYPE_NUM, 
              &value, &value_set, "valuex");
    init_opts(&opts[2], '4', false, OPTION_ARG_TYPE_FLG,
              &set_32bit, 0, "output 32 bit units");
    init_opts(&opts[3], '2', false, OPTION_ARG_TYPE_FLG,
              &set_16bit, 0, "output 16 bit units");
    init_opts(&opts[4], '1', false, OPTION_ARG_TYPE_FLG,
              &set_8bit, 0, "output 8 bit units");
    if (!scan_opts(argc, argv, 1, opts, 5, 0, 0, "")) {
        return;
    }
    if (!base_set) {
        diag_printf("iopoke what <location>?\n");
        return;
    }
    if (!value_set) { 
        diag_printf("iopoke what <value>?\n");
        return;
    }
    if (set_32bit) {
        size = 4;
    } else if (set_16bit) {
        size = 2;
    } else if (set_8bit) {
        size = 1;
    }

    switch (size) {
    case 4:
        HAL_WRITE_UINT32 ( base, value );
        break;
    case 2:
        HAL_WRITE_UINT16 ( base, value );
        break;
    case 1: 
        HAL_WRITE_UINT8 ( base, value );
        break;
    }
}

void
do_iopeek(int argc, char *argv[])
{
    struct option_info opts[4];
    unsigned long base;
    bool base_set;
    bool set_32bit = false;
    bool set_16bit = false;
    bool set_8bit = false;
    int size = 1, value;

    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              &base, &base_set, "base address");
    init_opts(&opts[1], '4', false, OPTION_ARG_TYPE_FLG,
              &set_32bit, 0, "output 32 bit units");
    init_opts(&opts[2], '2', false, OPTION_ARG_TYPE_FLG,
              &set_16bit, 0, "output 16 bit units");
    init_opts(&opts[3], '1', false, OPTION_ARG_TYPE_FLG,
              &set_8bit, 0, "output 8 bit units");
    if (!scan_opts(argc, argv, 1, opts, 4, 0, 0, "")) {
        return;
    }
    if (!base_set) {
        diag_printf("iopeek what <location>?\n");
        return;
    }
    if (set_32bit) {
      size = 4;
    } else if (set_16bit) {
        size = 2;
    } else if (set_8bit) {
        size = 1;
    }

    switch (size) {
    case 4:
        HAL_READ_UINT32 ( base, value );
        diag_printf("0x%04lx = 0x%08x\n", base, value );
        break;
    case 2:
        HAL_READ_UINT16 ( base, value );
        diag_printf("0x%04lx = 0x%04x\n", base, value );
        break;
    case 1: 
        HAL_READ_UINT8 ( base, value );
        diag_printf("0x%04lx = 0x%02x\n", base, value );
        break;
    }
}
