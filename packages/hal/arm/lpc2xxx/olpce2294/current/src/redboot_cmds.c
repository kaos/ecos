//==========================================================================
//
//      redboot_cmds.c
//
//      OLPCE2294 [platform] specific RedBoot commands
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

#include <redboot.h>

#include <cyg/hal/hal_diag.h>

// CLI functions
local_cmd_entry ("clear", "Clean up LCD entires", "", lcd_clear, LCD_cmds);

local_cmd_entry ("dark", "Turn LCD lighting off", "", lcd_dark, LCD_cmds);

local_cmd_entry ("echo",
                 "Output the args. If -n is specified, the trailing newline is suppressed.",
                 "-n [arg ...]", lcd_echo, LCD_cmds);

local_cmd_entry ("light", "Turn LCD lighting on", "", lcd_light, LCD_cmds);

CYG_HAL_TABLE_BEGIN (__LCD_cmds_TAB__, LCD_cmds);
CYG_HAL_TABLE_END (__LCD_cmds_TAB_END__, LCD_cmds);

extern struct cmd __LCD_cmds_TAB__[], __LCD_cmds_TAB_END__;

static cmd_fun do_lcd;
RedBoot_nested_cmd ("lcd",
                    "Manage LCD display",
                    "{cmds}",
                    do_lcd, __LCD_cmds_TAB__, &__LCD_cmds_TAB_END__);

//--------------------------------------------------------------------------
// lcd_usage --
//
static void
lcd_usage (char *why)
{
    diag_printf ("*** invalid 'lcd' command: %s\n", why);
    cmd_usage (__LCD_cmds_TAB__, &__LCD_cmds_TAB_END__, "lcd ");
}

//--------------------------------------------------------------------------
// do_lcd --
//
static void
do_lcd (int argc, char *argv[])
{
    struct cmd *cmd;

    if (argc < 2) {
        lcd_usage ("too few arguments");
        return;
    }
    if ((cmd = cmd_search (__LCD_cmds_TAB__, &__LCD_cmds_TAB_END__,
                           argv[1])) != (struct cmd *) 0) {
        (cmd->fun) (argc, argv);
        return;
    }
    lcd_usage ("unrecognized command");
}

//--------------------------------------------------------------------------
// lcd_echo --
//
static void
lcd_echo (int argc, char *argv[])
{
    bool newline;
    int cur =
        CYGACC_CALL_IF_SET_CONSOLE_COMM
        (CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
    CYGACC_CALL_IF_SET_CONSOLE_COMM (2);

    newline = true;
    if (argc > 2) {
        int i = 2;
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

    CYGACC_CALL_IF_SET_CONSOLE_COMM (cur);
    return;
}

//--------------------------------------------------------------------------
// lcd_clear --
//
static void
lcd_clear (int argc, char *argv[])
{
    // It clears a LCD screen. It doesn't touch a LCD controller, just a
    // scrolling.
    redboot_exec ("lcd", "echo", 0);
    redboot_exec ("lcd", "echo", 0);
    redboot_exec ("lcd", "echo", 0);
    return;
}

//--------------------------------------------------------------------------
// lcd_dark --
//
static void
lcd_dark (int argc, char *argv[])
{
    hal_diag_led (0);
    return;
}

//--------------------------------------------------------------------------
// lcd_light --
//
static void
lcd_light (int argc, char *argv[])
{
    hal_diag_led (1);
    return;
}

// indent: --indent-level4 -br -nut; vim: expandtab tabstop=4 shiftwidth=4
//--------------------------------------------------------------------------
// EOF redboot_cmds.c
