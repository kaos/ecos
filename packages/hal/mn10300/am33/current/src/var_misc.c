//==========================================================================
//
//      var_misc.c
//
//      HAL CPU variant miscellaneous functions
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
// Author(s):    nickg
// Contributors: nickg, jlarmour, dmoseley
// Date:         1999-01-21
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // Base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_cache.h>

/*------------------------------------------------------------------------*/
/* Variant specific initialization routine.                               */

void hal_variant_init(void)
{
}

/*------------------------------------------------------------------------*/
/* Cache functions.                                                       */

#if 0 //!defined(CYG_HAL_MN10300_SIM)
void cyg_hal_dcache_store(CYG_ADDRWORD base, int size)
{
    volatile register CYG_BYTE *way0 = HAL_DCACHE_PURGE_WAY0;
    volatile register CYG_BYTE *way1 = HAL_DCACHE_PURGE_WAY1;
    volatile register CYG_BYTE *way2 = HAL_DCACHE_PURGE_WAY2;
    volatile register CYG_BYTE *way3 = HAL_DCACHE_PURGE_WAY3;
    register int i;
    register CYG_ADDRWORD state;

    HAL_DCACHE_IS_ENABLED(state);
    if (state)
        HAL_DCACHE_DISABLE();

    way0 += base & 0x000003f0;
    way1 += base & 0x000003f0;
    way2 += base & 0x000003f0;
    way3 += base & 0x000003f0;
    for( i = 0; i < size; i += HAL_DCACHE_LINE_SIZE )
    {
        *(CYG_ADDRWORD *)way0 = 0;
        *(CYG_ADDRWORD *)way1 = 0;
        *(CYG_ADDRWORD *)way2 = 0;
        *(CYG_ADDRWORD *)way3 = 0;
        way0 += HAL_DCACHE_LINE_SIZE;
        way1 += HAL_DCACHE_LINE_SIZE;
        way2 += HAL_DCACHE_LINE_SIZE;
        way3 += HAL_DCACHE_LINE_SIZE;
    }
    if (state)
        HAL_DCACHE_ENABLE();
}
#endif

#ifdef CYGPKG_CYGMON
/*------------------------------------------------------------------------*/
/* GDB Register functions.                                                */
#include <cyg/hal/var_arch.h>
#include <cyg/hal/hal_stub.h>


int msp_read = 0;

extern int *_registers_valid;

void am33_get_gdb_extra_registers(CYG_ADDRWORD *registers, HAL_SavedRegisters *regs)
{
  register CYG_ADDRWORD epsw;
  asm volatile ("  mov epsw, %0 " : "=r" (epsw) : );

  registers[15] = regs->e0;
  registers[16] = regs->e1;
  registers[17] = regs->e2;
  registers[18] = regs->e3;
  registers[19] = regs->e4;
  registers[20] = regs->e5;
  registers[21] = regs->e6;
  registers[22] = regs->e7;

  registers[26] = regs->mcrh;
  registers[27] = regs->mcrl;
  registers[28] = regs->mcvf;

  registers[14] = regs->mdrq;

  {
    register CYG_ADDRWORD ssp, usp, msp;
    asm volatile (" mov usp,  %0 " : "=a" (usp) : );
    asm volatile (" mov ssp,  %0 " : "=a" (ssp) : );
    if ((epsw & HAL_ARCH_AM33_PSW_ML) == HAL_ARCH_AM33_PSW_ML)
      {
        // We are running in Monitor mode.  Go ahead and read the MSP.
        asm volatile (" mov msp, %0 " : "=a" (msp) : );
        msp_read = 1;
      } else {
        msp = 0;
        msp_read = 0;
      }

    // Now we need to determine which sp was in effect when we hit this exception,
    // since we want the register image to reflect the state at the time of the
    // exception.
    if ((regs->psw & HAL_ARCH_AM33_PSW_ML) == HAL_ARCH_AM33_PSW_ML)
      {
        msp = regs->sp;
      }
    else if ((regs->psw & HAL_ARCH_AM33_PSW_nSL) == 0)
      {
        ssp = regs->sp;
      }
    else
      {
        usp = regs->sp;
      }

    registers[23] = ssp;
    registers[24] = msp;
    registers[25] = usp;
  }


#ifdef CYGHWR_REGISTER_VALIDITY_CHECKING
  {
    int i;

    // Initially set all registers to valid
    for (i = 0; i < NUMREGS; i++)
      _registers_valid[i] = 1;

    if (msp_read == 0)
      _registers_valid[MSP] = 0;

    if (fpu_regs_read == 0)
      {
        for (i = FP_START; i <= FP_END; i++)
          _registers_valid[i] = 0;
      }
  }
#endif
}

void am33_set_gdb_extra_registers(CYG_ADDRWORD *registers, HAL_SavedRegisters *regs)
{
  regs->e0 = registers[15];
  regs->e1 = registers[16];
  regs->e2 = registers[17];
  regs->e3 = registers[18];
  regs->e4 = registers[19];
  regs->e5 = registers[20];
  regs->e6 = registers[21];
  regs->e7 = registers[22];

  regs->mcrh = registers[26];
  regs->mcrl = registers[27];
  regs->mcvf = registers[28];

  regs->mdrq = registers[14];

  {
    register CYG_ADDRWORD ssp, usp, msp;
    ssp = registers[23];
    msp = registers[24];
    usp = registers[25];
    if ((registers[11] & HAL_ARCH_AM33_PSW_ML) == HAL_ARCH_AM33_PSW_ML)
      {
        // We were running in monitor mode.
        // Go ahead and manually restore ssp and usp.
        // msp will be restored by the rti.
        asm volatile (" mov %0, usp " : : "a" (usp));
        asm volatile (" mov %0, ssp " : : "a" (ssp));
      }
    else if ((registers[11] & HAL_ARCH_AM33_PSW_nSL) == 0)
      {
        // We were running in system mode.
        // Go ahead and manually restore msp and usp.
        // ssp will be restored by the rti.
        asm volatile (" mov %0, usp " : : "a" (usp));
        if (msp_read)
          {
          asm volatile (" mov %0, msp " : : "a" (msp));
          }
      }
    else
      {
        // We were running in user mode.
        // Go ahead and manually restore msp and ssp.
        // usp will be restored by the rti.
        asm volatile (" mov %0, ssp " : : "a" (ssp));
        if (msp_read)
          {
          asm volatile (" mov %0, msp " : : "a" (msp));
          }
      }
  }

}
#endif // CYGPKG_CYGMON

/*------------------------------------------------------------------------*/
/* End of var_misc.c                                                      */
