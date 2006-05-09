|=============================================================================
|
|      context.S
|
|      ColdFire architecture context switch code
|
|=============================================================================
|###ECOSGPLCOPYRIGHTBEGIN####
| -------------------------------------------
| This file is part of eCos, the Embedded Configurable Operating System.
| Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
| Copyright (C) 2006 eCosCentric Ltd.
|        
| eCos is free software; you can redistribute it and/or modify it under
| the terms of the GNU General Public License as published by the Free
| Software Foundation; either version 2 or (at your option) any later version.
|
| eCos is distributed in the hope that it will be useful, but WITHOUT ANY
| WARRANTY; without even the implied warranty of MERCHANTABILITY or
| FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
| for more details.
|
| You should have received a copy of the GNU General Public License along
| with eCos; if not, write to the Free Software Foundation, Inc.,
| 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
|
| As a special exception, if other files instantiate templates or use macros
| or inline functions from this file, or you compile this file and link it
| with other works to produce a work based on this file, this file does not
| by itself cause the resulting work to be covered by the GNU General Public
| License. However the source code for this file must still be made available
| in accordance with section (3) of the GNU General Public License.
|
| This exception does not invalidate any other reasons why a work based on
| this file might be covered by the GNU General Public License.
| -------------------------------------------
|###ECOSGPLCOPYRIGHTEND####
|=============================================================================
|#####DESCRIPTIONBEGIN####
|
| Author(s):     Enrico Piria
| Contributors:
| Date:          2005-25-06
| Purpose:       This file contains implementations of the thread context 
|                switch routines. It also contains the longjmp() and setjmp()
|                routines.
|
|####DESCRIPTIONEND####
|========================================================================


#include <pkgconf/hal.h>
#include <cyg/hal/cf_offsets.inc>
#include <cyg/hal/arch.inc>


| ----------------------------------------------------------------------------
| hal_thread_switch_context - Switch between two threads
|
| externC void hal_thread_switch_context(CYG_ADDRESS to, CYG_ADDRESS from)
|
| INPUT:
|   0(%sp) : return address
|   4(%sp) : to - address of sp of next thread to execute
|   8(%sp) : from - address of sp save location of current thread
|
| OUTPUT:
|   None
| 
| RETURN VALUE:
|   None
|
| d0, d1, a0, a1 are ours to abuse. Other registers are not touched.

FUNC_START(hal_thread_switch_context)

        ctx_save_registers

        | Read to and from parameters from the stack        
        move.l  CYGARC_CF_CONTEXT_SIZE+4(%sp),%a0  
        move.l  CYGARC_CF_CONTEXT_SIZE+8(%sp),%a1  

        | Store this thread's current stack pointer to *from
        move.l  %sp,(%a1)
                                            
        | Load the stack pointer for the next thread from *to
        move.l  (%a0),%sp
                                            
        ctx_restore_registers

        | Return to caller
        rts


| ----------------------------------------------------------------------------
| hal_thread_load_context - Load thread context
|
| externC void hal_thread_load_context(CYG_ADDRESS to)
| 
| INPUT:
|   4(%sp) : to - address of sp of next thread to execute
|
| OUTPUT:
|   None
| 
| RETURN VALUE:
|   None
|
| d0, d1, a0, a1 are ours to abuse.

FUNC_START(hal_thread_load_context)

        | Read the to parameter from the stack and switch to that stack
        | pointer
        move.l  4(%sp),%a0                  
        move.l  (%a0),%sp                   

        | Load all of the  preserved registers from the stack
        movem.l   CYGARC_CFREG_DREGS(%sp),%d0-%d7
        movem.l   CYGARC_CFREG_AREGS(%sp),%a0-%a6

        | Starting SR
        move.w CYGARC_CF_SR(%sp), %d0
        move.w %d0,%sr
        
        | Deallocate context frame
        lea CYGARC_CF_CONTEXT_SIZE(%sp),%sp

        | Return
        rts


| ----------------------------------------------------------------------------
| The following routines are based on the hal_jmp_buf structure layout, defined
| in hal_arch.h

| ----------------------------------------------------------------------------
| hal_setjmp - setjmp for the ColdFire architecture
| 
| externC int hal_setjmp(hal_jmp_buf env)
| 
| INPUT:
|   0(%sp) : return address
|   4(%sp) : env - address of a hal_jmp_buf structure
| 
| OUTPUT:
|   None 
| 
| RETURN VALUE:
|   This routine always returns zero in d0.l.
| 
| d0, d1, a0, a1 are ours to abuse.

FUNC_START(hal_setjmp)

        | Get a pointer to the register buffer
        move.l   4(%sp),%a0

        | Store all of the preserved registers
        movem.l %d2-%d7,CYGARC_JMPBUF_REG_D2(%a0)
        movem.l %a2-%a6,CYGARC_JMPBUF_REG_A2(%a0)

#ifdef CYGHWR_HAL_COLDFIRE_MAC
        | Store MAC registers

        | Store MACSR register
        move.l  %macsr,%d0
        move.l  %d0,CYGARC_JMPBUF_REG_MACSR(%a0)

        | Switch to integer mode. This allows to save the contents of ACC
        | without rounding
        and.l   #0x000000df,%d0
        move.l  %d0,%macsr

        | Store ACC register
        move.l  %acc,%d0
        move.l  %d0,CYGARC_JMPBUF_REG_MACC(%a0)

        | Store MASK register
        move.l  %mask,%d0
        move.l  %d0,CYGARC_JMPBUF_REG_MASK(%a0)
#endif

        | Store the stack pointer
        move.l  %sp,CYGARC_JMPBUF_REG_SP(%a0)

        | Store the return address into the structure
        move.l  (%sp),CYGARC_JMPBUF_REG_PC(%a0)

        | Load a zero return value
        clr.l %d0

        | Return
        rts


| ----------------------------------------------------------------------------
| hal_longjmp - longjmp for the ColdFire architecture
| 
| externC void hal_longjmp(hal_jmp_buf env, int val)
| 
| INPUT:
|   0(%sp): return address
|   4(%sp): env - address of a hal_jmp_buf structure
|   8(%sp): val - the non-zero value to return
| 
| OUTPUT:
|   None
| 
| RETURN VALUE:
|   This routine always returns the value from the val parameter in  d0.l
|   and to the location of the PC in the env structure.

FUNC_START(hal_longjmp)

        | Load the return value parameter
        move.l  8(%sp),%d0
                                            
        | Get a pointer to the buffer to read our state from
        move.l  4(%sp),%a0                  
                                            
        | Load all of the preserved registers
        movem.l CYGARC_JMPBUF_REG_D2(%a0),%d2-%d7
        movem.l CYGARC_JMPBUF_REG_A2(%a0),%a2-%a6

#ifdef CYGHWR_HAL_COLDFIRE_MAC
        | Load MAC registers

        | Load MACSR register
        move.l  CYGARC_JMPBUF_REG_MACSR(%a0),%d1
        move.l  %d1,%macsr

        | Load ACC register
        move.l  CYGARC_JMPBUF_REG_MACC(%a0),%d1
        move.l  %d1,%acc

        | Load MASK register
        move.l  CYGARC_JMPBUF_REG_MASK(%a0),%d1
        move.l  %d1,%mask        
#endif

        | Load the stack pointer
        move.l  CYGARC_JMPBUF_REG_SP(%a0),%sp                  

        | Load return address and store it on stack
        move.l  CYGARC_JMPBUF_REG_PC(%a0),(%sp)

        | Return to caller
        rts
