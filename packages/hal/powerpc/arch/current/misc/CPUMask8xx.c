//==========================================================================
//
//        CPUMask8xx.c
//
//        Print PowerPC 8xx CPU Mask revision
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
// Date:          1999-06-24
// Description:   Prints the CPU Part and Mask numbers.
//        See http://www.mot.com/SPS/RISC/netcomm/aesop/mpc8XX/860/860revs.htm
//####DESCRIPTIONEND####

#include <cyg/infra/diag.h>
#include <cyg/hal/ppc_regs.h>
#include <cyg/hal/hal_io.h>

#define DPRAM_BASE    0x2000
#define CPM_MISC_BASE (DPRAM_BASE + 0x1cb0)

externC void
cyg_start( void )
{
    cyg_uint32 immr, part_no, mcrev_no;

    asm volatile ("mfspr %0, %1;": "=r" (immr): "I" (CYGARC_REG_IMMR));

    part_no = (immr & (CYGARC_REG_IMMR_PARTNUM|CYGARC_REG_IMMR_MASKNUM));
    
    HAL_READ_UINT16((immr&CYGARC_REG_IMMR_BASEMASK)+CPM_MISC_BASE, mcrev_no);

    diag_printf("MPC8xx IMMR[16:31]/Part# %04x, "
                "Microcode Rev No# %04x\n", part_no, mcrev_no);
}   
// EOF CPUMask8xx.c
