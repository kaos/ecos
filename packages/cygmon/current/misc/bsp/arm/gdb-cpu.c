//==========================================================================
//
//      gdb-cpu.c
//
//      CPU specific support for GDB stub.
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
// Author(s):    
// Contributors: gthomas
// Date:         1999-10-20
// Purpose:      CPU specific support for GDB stub.
// Description:  ARM is a Registered Trademark of Advanced RISC Machines
//               Limited.
//               Other Brands and Trademarks are the property of their
//               respective owners.
//
//####DESCRIPTIONEND####
//
//=========================================================================

#include <bsp/cpu.h>
#include <bsp/bsp.h>
#include "insn.h"
#include "gdb.h"

/*
 * Return byte offset within the saved register area of the
 * given register.
 */
int
bsp_regbyte(int regno)
{
    switch(regno)
    {
    case REG_R0:   return (int)&(((ex_regs_t*)0)->_r0);   break;
    case REG_R1:   return (int)&(((ex_regs_t*)0)->_r1);   break;
    case REG_R2:   return (int)&(((ex_regs_t*)0)->_r2);   break;
    case REG_R3:   return (int)&(((ex_regs_t*)0)->_r3);   break;
    case REG_R4:   return (int)&(((ex_regs_t*)0)->_r4);   break;
    case REG_R5:   return (int)&(((ex_regs_t*)0)->_r5);   break;
    case REG_R6:   return (int)&(((ex_regs_t*)0)->_r6);   break;
    case REG_R7:   return (int)&(((ex_regs_t*)0)->_r7);   break;
    case REG_R8:   return (int)&(((ex_regs_t*)0)->_r8);   break;
    case REG_R9:   return (int)&(((ex_regs_t*)0)->_r9);   break;
    case REG_R10:  return (int)&(((ex_regs_t*)0)->_r10);  break;
    case REG_R11:  return (int)&(((ex_regs_t*)0)->_r11);  break;
    case REG_R12:  return (int)&(((ex_regs_t*)0)->_r12);  break;
    case REG_SP:   return (int)&(((ex_regs_t*)0)->_sp);   break;
    case REG_LR:   return (int)&(((ex_regs_t*)0)->_lr);   break;
    case REG_PC:   return (int)&(((ex_regs_t*)0)->_pc);   break;

#ifndef __ECOS__
    case REG_F0:   return (int)&(((ex_regs_t*)0)->_f0);   break;
    case REG_F1:   return (int)&(((ex_regs_t*)0)->_f1);   break;
    case REG_F2:   return (int)&(((ex_regs_t*)0)->_f2);   break;
    case REG_F3:   return (int)&(((ex_regs_t*)0)->_f3);   break;
    case REG_F4:   return (int)&(((ex_regs_t*)0)->_f4);   break;
    case REG_F5:   return (int)&(((ex_regs_t*)0)->_f5);   break;
    case REG_F6:   return (int)&(((ex_regs_t*)0)->_f6);   break;
    case REG_F7:   return (int)&(((ex_regs_t*)0)->_f7);   break;
    case REG_FPS:  return (int)&(((ex_regs_t*)0)->_fps);  break;
#endif

    case REG_CPSR: return (int)&(((ex_regs_t*)0)->_cpsr);  break;
    case REG_SPSVC: return (int)&(((ex_regs_t*)0)->_spsvc);  break;
    }

    return 0;
}


/*
 * Return size in bytes of given register.
 */
int
bsp_regsize(int regno)
{
    switch(regno)
    {
    case REG_R0:   return (sizeof (((ex_regs_t*)0)->_r0));   break;
    case REG_R1:   return (sizeof (((ex_regs_t*)0)->_r1));   break;
    case REG_R2:   return (sizeof (((ex_regs_t*)0)->_r2));   break;
    case REG_R3:   return (sizeof (((ex_regs_t*)0)->_r3));   break;
    case REG_R4:   return (sizeof (((ex_regs_t*)0)->_r4));   break;
    case REG_R5:   return (sizeof (((ex_regs_t*)0)->_r5));   break;
    case REG_R6:   return (sizeof (((ex_regs_t*)0)->_r6));   break;
    case REG_R7:   return (sizeof (((ex_regs_t*)0)->_r7));   break;
    case REG_R8:   return (sizeof (((ex_regs_t*)0)->_r8));   break;
    case REG_R9:   return (sizeof (((ex_regs_t*)0)->_r9));   break;
    case REG_R10:  return (sizeof (((ex_regs_t*)0)->_r10));  break;
    case REG_R11:  return (sizeof (((ex_regs_t*)0)->_r11));  break;
    case REG_R12:  return (sizeof (((ex_regs_t*)0)->_r12));  break;
    case REG_SP:   return (sizeof (((ex_regs_t*)0)->_sp));   break;
    case REG_LR:   return (sizeof (((ex_regs_t*)0)->_lr));   break;
    case REG_PC:   return (sizeof (((ex_regs_t*)0)->_pc));   break;

#ifndef __ECOS__
    case REG_F0:   return (sizeof (((ex_regs_t*)0)->_f0));   break;
    case REG_F1:   return (sizeof (((ex_regs_t*)0)->_f1));   break;
    case REG_F2:   return (sizeof (((ex_regs_t*)0)->_f2));   break;
    case REG_F3:   return (sizeof (((ex_regs_t*)0)->_f3));   break;
    case REG_F4:   return (sizeof (((ex_regs_t*)0)->_f4));   break;
    case REG_F5:   return (sizeof (((ex_regs_t*)0)->_f5));   break;
    case REG_F6:   return (sizeof (((ex_regs_t*)0)->_f6));   break;
    case REG_F7:   return (sizeof (((ex_regs_t*)0)->_f7));   break;
    case REG_FPS:  return (sizeof (((ex_regs_t*)0)->_fps));  break;
#endif
                                                         
    case REG_CPSR: return (sizeof (((ex_regs_t*)0)->_cpsr)); break;
    case REG_SPSVC: return (sizeof (((ex_regs_t*)0)->_spsvc)); break;
    }

    return 0;
}


/*
 *  Given an exception number and a pointer to saved registers,
 *  return a GDB signal value.
 */
int
bsp_get_signal(int exc_nr, void *saved_regs)
{
  int sig = TARGET_SIGNAL_TRAP;
  ex_regs_t *regs = (ex_regs_t *)saved_regs;

  switch (exc_nr) {
  case BSP_CORE_EXC_UNDEFINED_INSTRUCTION:
  {
      union arm_insn inst;
      if (bsp_memory_read((void *)regs->_pc, 0, ARM_INST_SIZE * 8, 1, &(inst.word)) != 0)
      {
          /*
           * We were able to read this address. It must be a valid address.
           */
          if (inst.word == BREAKPOINT_INSN)
              sig = TARGET_SIGNAL_TRAP;
      }
      else
          sig = TARGET_SIGNAL_ILL;
  }
  break;
  case BSP_CORE_EXC_SOFTWARE_INTERRUPT:        sig = TARGET_SIGNAL_TRAP;    break;
  case BSP_CORE_EXC_PREFETCH_ABORT:            sig = TARGET_SIGNAL_BUS;     break;
  case BSP_CORE_EXC_DATA_ABORT:                sig = TARGET_SIGNAL_BUS;     break;
  case BSP_CORE_EXC_ADDRESS_ERROR_26_BIT:      sig = TARGET_SIGNAL_BUS;     break;
  case BSP_CORE_EXC_IRQ:                       sig = TARGET_SIGNAL_INT;     break;
  case BSP_CORE_EXC_FIQ:                       sig = TARGET_SIGNAL_INT;     break;
  default:                                     sig = TARGET_SIGNAL_TRAP;    break;
  }

  return sig;
}


/*
 * Set the PC value in the saved registers.
 */
void
bsp_set_pc(unsigned long pc, void *saved_regs)
{
  ((ex_regs_t *)saved_regs)->_pc = pc;
}


/*
 * Get the PC value from the saved registers.
 */
unsigned long
bsp_get_pc(void *saved_regs)
{
  return ((ex_regs_t *)saved_regs)->_pc;
}
