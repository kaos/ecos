//==========================================================================
//
//      arm-mon.c
//
//      Support code to extend the generic monitor code to support
//      ARM(R) processors.
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
// Purpose:      Support code to extend the generic monitor code to support
//               ARM(R) processors.
// Description:  Further board specific support is in other files.
//               This file contains:
//                 register names lookup table
//
//               Empty Stubs:
//                 Interval timer - This should really belong to the application
//                 operating system.
//
//               Should not contain:
//                 low level uart getchar and putchar functions
//                 delay function to support uart
//
//               ARM is a Registered Trademark of Advanced RISC Machines
//               Limited.
//               Other Brands and Trademarks are the property of their
//               respective owners.               
//
//####DESCRIPTIONEND####
//
//=========================================================================

#include <setjmp.h>
#include <bsp/bsp.h>
#include <bsp/cpu.h>
#ifdef DISASSEMBLER
#include <dis-asm.h>
#endif

#include "cpu_info.h"
#include "monitor.h"

/* This module is required to provide many of the services defined
   in tservice.h */

static char *apcs_names[] = {
  "a1", "a2", "a3", "a4",
  "v1", "v2", "v3", "v4",
  "v5", "v6", "sl", "fp",
  "ip", "sp", "lr", "pc",
#if HAVE_FLOAT_REGS
  "f0", "f1", "f2", "f3",
  "f4", "f5", "f6", "f7",
  "fps",
#endif
  "ps"
};

static char *standard_names[] =
{
  "r0",  "r1",  "r2",  "r3",
  "r4",  "r5",  "r6",  "r7",
  "r8",  "r9",  "r10", "r11",
  "r12", "r13", "r14", "pc",
#if HAVE_FLOAT_REGS
  "f0",  "f1",  "f2",  "f3",
  "f4",  "f5",  "f6",  "f7",
  "fps",
#endif
  "ps",
};


void arm_othernames (void);

struct regstruct regtab[] =
{
  { 0, REG_R0,   REGTYPE_INT   },
  { 0, REG_R1,   REGTYPE_INT   },
  { 0, REG_R2,   REGTYPE_INT   },
  { 0, REG_R3,   REGTYPE_INT   },
  { 0, REG_R4,   REGTYPE_INT   },
  { 0, REG_R5,   REGTYPE_INT   },
  { 0, REG_R6,   REGTYPE_INT   },
  { 0, REG_R7,   REGTYPE_INT   },
  { 0, REG_R8,   REGTYPE_INT   },
  { 0, REG_R9,   REGTYPE_INT   },
  { 0, REG_R10,  REGTYPE_INT   },
  { 0, REG_R11,  REGTYPE_INT   },
  { 0, REG_R12,  REGTYPE_INT   },
  { 0, REG_SP,   REGTYPE_INT   },
  { 0, REG_LR,   REGTYPE_INT   },
  { 0, REG_PC,   REGTYPE_INT   },
#if HAVE_FLOAT_REGS
  { 0, REG_F0,   REGTYPE_FLOAT },
  { 0, REG_F1,   REGTYPE_FLOAT },
  { 0, REG_F2,   REGTYPE_FLOAT },
  { 0, REG_F3,   REGTYPE_FLOAT },
  { 0, REG_F4,   REGTYPE_FLOAT },
  { 0, REG_F5,   REGTYPE_FLOAT },
  { 0, REG_F6,   REGTYPE_FLOAT },
  { 0, REG_F6,   REGTYPE_FLOAT },
  { 0, REG_FPS,  REGTYPE_FLOAT },
#endif
  { 0, REG_CPSR, REGTYPE_INT   },
  { 0, 0,        0             } /* Terminating element must be last */
} ;

void
initialize_mon(void)
{
  /* FIXME: Convert ALL variable initializations to assignments
     in order to support ROMABLE CODE.
     This includes the register table
     */

  /*
   * Call arm_othernames to sync up Cygmon and the disassembler
   * and ensure they are using the same set of registernames
   */
  arm_othernames();

  /*
   * Call arm_othernames again to revert to the assembler
   * default names
   */
  arm_othernames();

} /* initialize_mon */


/*
  read_memory
  write_memory
       Defaults to generic_mem.c
       */


/* SET_BREAKPOINT -
   CLEAR_BREAKPOINT
   Defaults to generic_bp32.c
   */


#ifndef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

/* Return the currently-saved value corresponding to register "regnum". */
target_regval_t
get_register (int regnum)
{
    struct regstruct *p;
    target_regval_t reg;
    int   offset, len;
    char  *dest = (char*)0;

    for (p = regtab; p->registername && p->registernumber != regnum; ++p)
	;

    if (p->registername) {
	len = bsp_regsize(p->registernumber);
	offset = bsp_regbyte(p->registernumber);
	switch (p->registertype) {
	  case REGTYPE_INT:
	    dest = (char*)&(reg.i);
	    break;
#if HAVE_FLOAT_REGS
        case REGTYPE_FLOAT:
	    dest = (char*)&(reg.f);
	    break;
#endif
#if HAVE_DOUBLE_REGS
        case REGTYPE_DOUBLE:
	    dest = (char*)&(reg.d);
	    break;
#endif
	}
        if (dest)
            memcpy(dest, (char *)mon_saved_regs + offset, len);
    } else
	reg.i = 0;

    return reg;
}



/* Store VALUE in the register corresponding to REGNUM. */
void
put_register (int regnum, target_regval_t value)
{
    int   offset, len;

    len = bsp_regsize(regnum);
    offset = bsp_regbyte(regnum);
    memcpy((char *)mon_saved_regs + offset, &value, len);
}

#endif // !CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

/* Change to the alternate register set names */
void
arm_othernames (void)
{
    static int regset = 1;
    char **name;
    int  i;

#ifdef DISASSEMBLER
    /* Change the disassembler */
    regset = arm_toggle_regnames();
#else
    regset = (regset == 0) ? 1 : 0;
#endif

    /* Change cygmon */
    if (regset == 1)
        /* Disassembler is using apcs names. */
        name = apcs_names;
    else
        /* Disassembler is using standard names. */
        name = standard_names;

    for (i = 0; i < (sizeof(apcs_names)/sizeof(apcs_names[0])); i++)
	regtab[i].registername = name[i];
}

#ifdef __ECOS__

#include <pkgconf/hal.h>
#include <cyg/hal/hal_arch.h>
#include "bsp/common/bsp_if.h"

int
machine_syscall(HAL_SavedRegisters *regs)
{
    int res, err;
    err = _bsp_do_syscall(regs->d[0], // Function
                          regs->d[1], regs->d[2], regs->d[3], 0, // arguments,
                          &res);
    regs->d[0] = res;
    regs->pc += 4;  // Advance PC
    return err;
}
#endif
