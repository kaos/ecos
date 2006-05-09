#ifndef CYGONCE_HAL_ARCH_INC
#define CYGONCE_HAL_ARCH_INC
|=============================================================================
|
|  arch.inc
|
|  ColdFire architecture assembler header file
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
| Author(s):    Enrico Piria
| Contributors:
| Date:         2005-25-06
| Purpose:      MCF5272 variant definitions.
| Description:  This file contains macro definitions used in the
|               architecture HAL assembler file.
|
|####DESCRIPTIONEND####
|==========================================================================


#define FUNC_START(name)    \
    .text;                  \
    .balign 4;              \
    .type name,@function;   \
    .globl name;            \
name:


| ----------------------------------------------------------------------------
| Macros to deal with the interrupt priority level in the status register.

        .macro hal_cpu_int_disable
        move.w  #0x2700,%sr
        .endm

        
        .macro hal_cpu_int_enable work
        move.w  %sr,\work
        and.l   #0xf8ff,\work
        move.w  \work,%sr
        .endm

        
        .macro hal_cpu_int_merge from work
        move.w  %sr,\work
        and.l   #0xf8ff,\work
        and.l   #0x0700,\from
        or.l    \from,\work
        move.w \work,%sr
        .endm

| ----------------------------------------------------------------------------
| Macro to find the value the SP register had before an exception.
        
        .macro find_original_sp out
        move.b  CYGARC_CF_FMTVECWORD(%sp),\out
        lsr.l   #4,\out
        and.l   #0x00000003,\out
        add.l   #CYGARC_CF_EXCEPTION_SIZE,\out
        add.l   %sp,\out
        .endm


| ----------------------------------------------------------------------------
| Macros used to save and restore MAC registers during interrupts/exceptions.

#ifdef CYGHWR_HAL_COLDFIRE_MAC
        .macro  save_mac_registers work
        | Store MACSR register
        move.l  %macsr,\work
        move.l  \work,CYGARC_CFREG_MACSR(%sp)

        | Switch to integer mode. This allows to save the contents of ACC
        | without rounding
        and.l   #0x000000df,\work
        move.l  \work,%macsr

        | Store ACC register
        move.l  %acc,\work
        move.l  \work,CYGARC_CFREG_MACC(%sp)

        | Store MASK register
        move.l  %mask,\work
        move.l  \work,CYGARC_CFREG_MASK(%sp)
        .endm


        .macro  restore_mac_registers work
        | Load MACSR register
        move.l  CYGARC_CFREG_MACSR(%sp),\work
        move.l  \work,%macsr

        | Load ACC register
        move.l  CYGARC_CFREG_MACC(%sp),\work
        move.l  \work,%acc

        | Load MASK register
        move.l  CYGARC_CFREG_MASK(%sp),\work
        move.l  \work,%mask
        .endm
#endif


| ----------------------------------------------------------------------------
| Macros used to save registers in interrupt handlers. During an interrupt,
| we save registers %d0-%d1 and %a0-%a1 because of the GNU C calling
| conventions. During the handler we also need register %d2.

#ifdef CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT
        .macro int_pres_regs
        lea.l     -CYGARC_CF_EXCEPTION_DECREMENT(%sp),%sp
        movem.l   %d0-%d2,CYGARC_CFREG_DREGS(%sp)
        movem.l   %a0-%a1,CYGARC_CFREG_AREGS(%sp)
        .endm

        
        .macro int_rest_regs
        movem.l   CYGARC_CFREG_AREGS(%sp),%a0-%a1
        movem.l   CYGARC_CFREG_DREGS(%sp),%d0-%d2
        lea.l     CYGARC_CF_EXCEPTION_DECREMENT(%sp),%sp
        .endm

#else /* CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT */

        .macro int_pres_regs
        lea.l     -CYGARC_CF_EXCEPTION_DECREMENT(%sp),%sp
        movem.l   %d0-%d7,CYGARC_CFREG_DREGS(%sp)
        movem.l   %a0-%a6,CYGARC_CFREG_AREGS(%sp)

#ifdef CYGHWR_HAL_COLDFIRE_MAC
        save_mac_registers %d0
#endif

        | Save old SP (before interrupt)
        find_original_sp %d0
        move.l    %d0,CYGARC_CFREG_SP(%sp)
        .endm

        
        .macro int_rest_regs

#ifdef CYGHWR_HAL_COLDFIRE_MAC
        restore_mac_registers %d0
#endif
        
        movem.l   CYGARC_CFREG_AREGS(%sp),%a0-%a6
        movem.l   CYGARC_CFREG_DREGS(%sp),%d0-%d7
        lea.l     CYGARC_CF_EXCEPTION_DECREMENT(%sp),%sp
        .endm

#endif /* CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT */


| ----------------------------------------------------------------------------
| Macros used to save/restore registers during context switches.
| We don't save registers %d0-%d1 and %a0-%a1 because of the GNU C calling
| conventions: these macros are used in a routine called by C code.

#ifdef CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM

        .macro ctx_save_registers
        lea     -CYGARC_CF_CONTEXT_SIZE(%sp),%sp
        movem.l %d2-%d7,CYGARC_CFREG_D2(%sp)
        movem.l %a2-%a6,CYGARC_CFREG_A2(%sp)
#ifdef CYGHWR_HAL_COLDFIRE_MAC
        save_mac_registers %d0
#endif
        | Save SR and interrupt level
        move.w %sr,%d0
        move.w %d0,CYGARC_CF_SR(%sp)        
        .endm
                                                    

        .macro ctx_restore_registers
        | Restore SR and interrupt level
        move.w CYGARC_CF_SR(%sp),%d0
        move.w %d0,%sr
        
#ifdef CYGHWR_HAL_COLDFIRE_MAC
        restore_mac_registers %d0
#endif
        movem.l CYGARC_CFREG_D2(%sp),%d2-%d7
        movem.l CYGARC_CFREG_A2(%sp), %a2-%a6
        lea     CYGARC_CF_CONTEXT_SIZE(%sp),%sp
        .endm

#else /* CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM */

        .macro ctx_save_registers
        | Save all of the registers
        lea     -CYGARC_CF_CONTEXT_SIZE(%sp),%sp
        movem.l %d0-%d7,CYGARC_CFREG_DREGS(%sp)
        movem.l %a0-%a7,CYGARC_CFREG_AREGS(%sp)        
#ifdef CYGHWR_HAL_COLDFIRE_MAC
        save_mac_registers %d0
#endif

        | Save pc (useful during debugging with GDB)
        lea (%pc),%a0
        move.l %a0,CYGARC_CFREG_PC(%sp)

        | Save SR and interrupt level
        move.w %sr,%d0
        move.w %d0,CYGARC_CF_SR(%sp)        
        .endm
        

        .macro ctx_restore_registers
        | Restore SR and interrupt level
        move.w CYGARC_CF_SR(%sp),%d0
        move.w %d0,%sr
        
#ifdef CYGHWR_HAL_COLDFIRE_MAC
        restore_mac_registers %d0
#endif
        movem.l CYGARC_CFREG_DREGS(%sp),%d0-%d7
        movem.l CYGARC_CFREG_AREGS(%sp),%a0-%a6                       
        lea     CYGARC_CF_CONTEXT_SIZE(%sp),%sp     
        .endm
#endif /* CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM */

| ----------------------------------------------------------------------------
| End of arch.inc
#endif /* ifndef CYGONCE_HAL_ARCH_INC */
