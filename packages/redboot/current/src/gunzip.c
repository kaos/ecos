//==========================================================================
//
//      gunzip.c
//
//      RedBoot GZIP uncompress command
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2005 eCosCentric Ltd
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
// Author(s):    Peter Korsgaard
// Contributors: Peter Korsgaard
// Date:         2005-04-04
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <redboot.h>

RedBoot_cmd("gunzip",
	    "Uncompress GZIP compressed data",
	    "-s <location> -d <location>",
	    do_gunzip
    );

void
do_gunzip(int argc, char *argv[])
{
    struct option_info opts[2];
    unsigned long src, dst;
    bool src_set, dst_set;
    _pipe_t pipe;
    _pipe_t* p = &pipe;
    int err;

    init_opts(&opts[0], 's', true, OPTION_ARG_TYPE_NUM, 
              (void *)&src, (bool *)&src_set, "source address");
    init_opts(&opts[1], 'd', true, OPTION_ARG_TYPE_NUM, 
              (void *)&dst, (bool *)&dst_set, "destination address");
    if (!scan_opts(argc, argv, 1, opts, 2, 0, 0, "")) {
        return;
    }

    // Must have src and dst
    if (!src_set || !dst_set) {
        // try to use load_address for src 
        if (dst_set 
            && load_address >= (CYG_ADDRESS)ram_start 
            && load_address < load_address_end) {
            src = load_address;
            diag_printf("Decompressing from %p to %p\n",
			(void*)src, (void*)dst);
        }
        else
        {
            diag_printf("usage: gunzip -s <addr> -d <addr>\n");
            return;
        }
    }

    p->out_buf = (unsigned char*)dst;
    p->out_max = p->out_size = -1;
    p->in_buf = (unsigned char*)src;
    p->in_avail = -1;

    err = (*_dc_init)(p);

    if (0 == err)
        err = (*_dc_inflate)(p);

    // Free used resources, do final translation of error value.
    err = (*_dc_close)(p, err);

    if (0 != err && p->msg) {
       entry_address = (CYG_ADDRESS)NO_MEMORY;
       diag_printf("Decompression error: %s\n", p->msg);
    } else {
        load_address     = entry_address = (CYG_ADDRESS)dst;
        load_address_end = (CYG_ADDRESS)p->out_buf;
        diag_printf("Decompressed %lu bytes\n", 
                    load_address_end - load_address);
    }
}
