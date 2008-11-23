//==========================================================================
//
//      redboot_cmds.c
//
//      OLPCH2294 [platform] specific RedBoot commands
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2008 eCosCentric Limited
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
// Author(s):     Sergei Gavrikov
// Contributors:  Sergei Gavrikov
// Date:          2008-08-31
// Purpose:
// Description:
//
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <cyg/hal/hal_diag.h>
#include <redboot.h>

// CLI functions
static void do_echo (int argc, char *argv[]);
RedBoot_cmd ("echo",
             "Outputs the args. If -n is specified, the trailing newline is suppressed.",
             "-n [arg ...]", do_echo);

static void do_led (int argc, char *argv[]);
RedBoot_cmd ("led", "Manage diagnostic led(s)", "[-m mask]", do_led);

//--------------------------------------------------------------------------
// do_echo --
//
static void
do_echo (int argc, char *argv[])
{
    bool newline = true;
    if (argc > 1) {
        int i = 1;
        if (strncmp (&argv[i][0], "-n", 2) == 0) {
            newline = false;
            i++;
        }
        for (; i < argc; i++) {
            diag_write_string (&argv[i][0]);
            if ((argc - i) > 1)
                diag_write_char (' ');
        }
    }
    if (newline)
        diag_write_char ('\n');
    return;
}

//--------------------------------------------------------------------------
// do_led --
//
static void
do_led (int argc, char *argv[])
{
    struct option_info opts[1];
    unsigned long mask;
    bool mask_set;

    init_opts (&opts[0], 'm', true, OPTION_ARG_TYPE_NUM,
               &mask, &mask_set, "mask");
    if (!scan_opts (argc, argv, 1, opts, 1, 0, 0, "")) {
        return;
    }
    if (!mask_set) {
        diag_printf ("led what <mask>?\n");
        return;
    }
    hal_lpc2xxx_set_leds (mask);
    return;
}

// indent: --indent-level4 -br -nut; vim: expandtab tabstop=4 shiftwidth=4
//--------------------------------------------------------------------------
// EOF redboot_cmds.c
