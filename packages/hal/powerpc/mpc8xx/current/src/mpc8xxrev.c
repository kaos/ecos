//==========================================================================
//
//        mpc8xxrev.c
//
//        MPC8xx Revision Identification
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jskov
// Contributors:  jskov
// Date:          2000-02-23
// Description:   Print MPC8xx revision numbers
//####DESCRIPTIONEND####

#include <cyg/infra/testcase.h>
#include <stdio.h>
#include <cyg/hal/ppc_regs.h>
#include <cyg/hal/hal_io.h>

int
main( void )
{
    cyg_uint32 pvr, part_mask;
    cyg_uint16 rev_num;

    CYG_TEST_INIT();

    asm volatile ("mfspr %0, 287;": "=r" (pvr));
    printf ("Processor PVR:         0x%08x\n", pvr);

    asm volatile ("mfspr %0, 638;": "=r" (part_mask));
    printf ("Processor IMMR[16:31]: 0x%04x\n", (part_mask & 0xffff));

    HAL_READ_UINT16(CYGARC_REG_REV_NUM, rev_num);
    printf ("Processor REV_NUM:     0x%04x\n", rev_num);


    printf("\nSee http://www.motorola.com/SPS/RISC/netcomm/ for erratas.\n");
    printf("Look for MPC8xx Versions and Masks in the Publications Library\n");

    CYG_TEST_PASS_FINISH("mpc8xx revision dump");
}
// EOF mpc8xxrev.c
