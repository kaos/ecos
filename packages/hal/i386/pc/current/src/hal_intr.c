/*=============================================================================
//
//      hal_intr.c
//
//      HAL interrupt support
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   pjo
// Contributors: nickg
// Date:        2000-2-10
// Purpose:     HAL interrupt support
// Description: Implementations of HAL interrupt support code.
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#	include <pkgconf/hal.h>
#	include <cyg/infra/cyg_type.h>
#	include <cyg/infra/cyg_ass.h>
#	include <cyg/hal/hal_arch.h>
#	include <cyg/hal/hal_intr.h>
#	include <cyg/hal/drv_api.h>


//===========================================================================*/

extern void hal_pc_generic_interrupt(void) ;
extern char hal_pc_generic_interrupt_start ;
extern char hal_pc_generic_interrupt_vector ;
extern char hal_pc_generic_interrupt_end ;

//===========================================================================*/

cyg_uint32 hal_pc_in_interrupt ;
cyg_uint32 hal_pc_interrupt_esp ;

//===========================================================================*/

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
char __interrupt_stack_base[CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE] ;
char * __interrupt_stack = &(__interrupt_stack_base[CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE]) ;
#endif /* CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK */


//===========================================================================*/
/* The interrupts are handled by stubs which are copied into
	hal_pc_interrupt_routine[vector].  See vectors.S, hal_pc_generic_interrupt,
	for the source code which is copied there.
*/
#define HAL_PC_INTERRUPT_ROUTINE_SIZE (16)
typedef char hal_pc_interrupt_routine_t[HAL_PC_INTERRUPT_ROUTINE_SIZE] ;

hal_pc_interrupt_routine_t hal_pc_interrupt_routine[CYGNUM_HAL_ISR_COUNT] ;

//===========================================================================*/

#if 0

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
void interrupt_end(cyg_uint32 isr_ret, cyg_interrupt * intr, HAL_SavedRegisters * sr);
#endif /* CYGFUN_HAL_COMMON_KERNEL_SUPPORT */


int hal_pc_interrupt(cyg_uint32 vector, cyg_uint32 * esp)
{
    int r ;
    cyg_ISR_t * routine ;
    HAL_SavedRegisters * regs ;

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
    extern cyg_uint32 cyg_scheduler_sched_lock ;
#endif /* CYGFUN_HAL_COMMON_KERNEL_SUPPORT */

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
    cyg_scheduler_sched_lock++ ;
#endif /* CYGFUN_HAL_COMMON_KERNEL_SUPPORT */

/* Save the registers into regs. */
    regs = (HAL_SavedRegisters*) esp[0] ;

    routine = (cyg_ISR_t *) cyg_hal_interrupt_handlers[vector] ;
    r = routine(vector, cyg_hal_interrupt_data[vector]);

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
    interrupt_end(r, (cyg_interrupt*) cyg_hal_interrupt_objects[vector], regs) ;
#endif /* CYGFUN_HAL_COMMON_KERNEL_SUPPORT */

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
//	cyg_scheduler_sched_lock-- ;
#endif /* CYGFUN_HAL_COMMON_KERNEL_SUPPORT */

    return r ;
}
#endif


//===========================================================================*/

void hal_pc_interrupt_attach(CYG_ADDRWORD vector, CYG_ADDRESS routine,
		CYG_ADDRWORD parameter, void * object)
{
    int l ;
    char * d ;
    short * q ;
    CYG_ADDRWORD r ;
    cyg_uint32 * id ;

    l = &hal_pc_generic_interrupt_end - &hal_pc_generic_interrupt_start  ;

    /* Let's make sure that hal_pc_interrupt_routine entries are large enough. */
    CYG_ASSERT(l <= HAL_PC_INTERRUPT_ROUTINE_SIZE, "HAL_PC_INTERRUPT_ROUTINE_SIZE is too small");

    /* Put the desired routine and parameter into the table. */
    cyg_hal_interrupt_data[vector] = (cyg_uint32) parameter ;
    cyg_hal_interrupt_handlers[vector] = (CYG_ADDRWORD) routine ;
    cyg_hal_interrupt_objects[vector] = (CYG_ADDRWORD) object ;

    d = hal_pc_interrupt_routine[vector] ;

    /* Initialize the entry we're interested in using. */
    memset(d, HAL_BREAKINST, HAL_PC_INTERRUPT_ROUTINE_SIZE);
    memcpy(d, hal_pc_generic_interrupt, l) ;

    l = &hal_pc_generic_interrupt_vector - &hal_pc_generic_interrupt_start ;
    id = (cyg_uint32*) &(d[l]) ;
    id[0] = vector ;

    /* Now write the vector into the IDT. */
    q = (short*) (vector * 8) ;
    r = (CYG_ADDRWORD) d ;
    q[0] = (r & 0xFFFF) ;
    q[1] = 8 ;
    q[2] = 0x8E00 ;
    q[3] = (r >> 16) ;
}


/*---------------------------------------------------------------------------*/
/* End of hal_intr.c */
