//==========================================================================
//
//      iap_test.c
//
//      IAP test for ARM LPC2XXX platforms
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Limited
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
// Author(s):    Sergei Gavrikov
// Contributors: Sergei Gavrikov
// Date:         2007-09-20
// Purpose:
// Description:  Basic test for IAP (In Application Programming) interface
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal_arm_lpc2xxx.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/testcase.h>
#include <cyg/infra/diag.h>
            
#if defined(CYGBLD_BUILD_HAL_LPC2XXX_WITH_IAP_CALL) \
    && (CYGINT_HAL_ARM_LPC2XXX_IAP_CALL == 1)

#include <cyg/hal/lpc2xxx_iap.h>

typedef struct {
    cyg_uint32 id;
    char part[8];
} lpc_part_id_info_t;

static lpc_part_id_info_t known_parts[] = {
  {0xFFF0FF12, "LPC2104"}, {0xFFF0FF22, "LPC2105"}, {0xFFF0FF32, "LPC2106"},
  {0x0101FF12, "LPC2114"}, {0x0201FF12, "LPC2119"}, {0x0101FF13, "LPC2124"},
  {0x0201FF13, "LPC2129"}, {0x0301FF13, "LPC2194"}, {0x0401FF12, "LPC2212"},
  {0x0601FF13, "LPC2214"}, {0x0401FF13, "LPC2292"}, {0x0501FF13, "LPC2294"},
  {0x0002FF11, "LPC2132"}, {0x0002FF12, "LPC2134"}, {0x0002FF23, "LPC2136"},
  {0x0002FF25, "LPC2138"}
};

static cyg_uint32 data[2];

static int
read_part_id (void *data)
{
    cyg_uint32     *id = (cyg_uint32 *) data;
    return hal_lpc2xxx_iap_call (HAL_LPC2XXX_IAP_COMMAND_READ_PART_ID, 0, 0,
                                 0, 0, (cyg_uint32 *) id);
}

void
cyg_start (void)
{
    int             i, rc;
    cyg_uint32      device_id;
    char           *part = "unknow";

    CYG_TEST_INIT ();
    CYG_TEST_INFO ("Starting LPC2XXX IAP test");

    // The simplest way to test IAP call is just to read NXP Device ID with it.
    // The Device ID is a unique number for every LPC2XXX chip. So, here we use
    // IAP command HAL_LPC2XXX_IAP_COMMAND_READ_PART_ID to get the number.
    rc = read_part_id (data);
    CYG_TEST_CHECK (rc == HAL_LPC2XXX_IAP_RETURN_CMD_SUCCESS,
                    "Read of Device ID failed");

    device_id = data[1];

    for (i = 0; i < CYG_NELEM (known_parts); i++) {
        if (device_id == known_parts[i].id) {
            part = known_parts[i].part;
            break;
        }
    }

    diag_printf ("INFO:<LPC2XXX device id 0x%08x>\n", device_id);
    diag_printf ("INFO:<LPC2XXX part %s>\n", part);

    CYG_TEST_PASS ("LPC2XXX IAP test OK");

    CYG_TEST_EXIT ("Done");
}
#else
void
cyg_start (void)
{
    CYG_TEST_INIT ();
    CYG_TEST_INFO ("LPC2XXX IAP test require:\n"
                   "CYGINT_HAL_ARM_LPC2XXX_IAP_CALL");
    CYG_TEST_NA ("LPC2XXX IAP test requirements");
}
#endif

// indent: -i4 -br -nut -di16 -ce; vim: expandtab tabstop=4 shiftwidth=4:
//--------------------------------------------------------------------------
// EOF iap_test.c
