#ifndef CYGONCE_HAL_DRV_API_H
#define CYGONCE_HAL_DRV_API_H

/*==========================================================================
//
//      drv_api.h
//
//      Native API for Kernel
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
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg
// Date:        1999-02-24
// Purpose:     Driver API
// Description: This file defines the API used by device drivers to access
//              system services. When the kernel is present it maps directly
//              to the Kernel C API. When the kernel is absent, it is provided
//              by a set of HAL functions.
//              
// Usage:       #include <cyg/kernel/kapi.h>
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

#ifdef CYGPKG_KERNEL

/*------------------------------------------------------------------------*/
/* Kernel co-resident version of API                                      */

#include <pkgconf/kernel.h>

#ifndef CYGFUN_KERNEL_API_C
#error Driver API requres Kernel API to be present
#endif

#include <cyg/kernel/kapi.h>

#define cyg_drv_isr_lock               cyg_interrupt_disable
#define cyg_drv_isr_unlock             cyg_interrupt_enable

#define cyg_drv_dsr_lock               cyg_scheduler_lock
#define cyg_drv_dsr_unlock             cyg_scheduler_unlock

#define cyg_drv_mutex_t                cyg_mutex_t
#define cyg_drv_mutex_init             cyg_mutex_init
#define cyg_drv_mutex_destroy          cyg_mutex_destroy
#define cyg_drv_mutex_lock             cyg_mutex_lock
#define cyg_drv_mutex_trylock          cyg_mutex_trylock
#define cyg_drv_mutex_unlock           cyg_mutex_unlock
#define cyg_drv_mutex_release          cyg_mutex_release

#define cyg_drv_cond_t                 cyg_cond_t
#define cyg_drv_cond_init              cyg_cond_init
#define cyg_drv_cond_destroy           cyg_cond_destroy
#define cyg_drv_cond_wait              cyg_cond_wait
#define cyg_drv_cond_signal            cyg_cond_signal
#define cyg_drv_cond_broadcast         cyg_cond_broadcast

#define cyg_drv_interrupt_create       cyg_interrupt_create
#define cyg_drv_interrupt_delete       cyg_interrupt_delete
#define cyg_drv_interrupt_attach       cyg_interrupt_attach
#define cyg_drv_interrupt_detach       cyg_interrupt_detach
#define cyg_drv_interrupt_mask         cyg_interrupt_mask
#define cyg_drv_interrupt_unmask       cyg_interrupt_unmask
#define cyg_drv_interrupt_acknowledge  cyg_interrupt_acknowledge
#define cyg_drv_interrupt_configure    cyg_interrupt_configure
#define cyg_drv_interrupt_level        cyg_interrupt_level

#else /* CYGPKG_KERNEL */

/*------------------------------------------------------------------------*/
/* Non-kernel version of API                                              */

typedef CYG_ADDRWORD cyg_addrword_t;        /* May hold pointer or word   */
typedef cyg_addrword_t cyg_handle_t;        /* Object handle              */
typedef cyg_uint32   cyg_priority_t;        /* type for priorities        */
typedef cyg_uint32   cyg_vector_t;          /* Interrupt vector id        */
typedef int cyg_bool_t;

typedef cyg_uint32 cyg_ISR_t( cyg_vector_t vector, cyg_addrword_t data);
typedef void cyg_DSR_t(cyg_vector_t vector,
                       cyg_ucount32 count,
                       cyg_addrword_t data);


externC void cyg_drv_isr_lock(void);
externC void cyg_drv_isr_unlock(void);

externC void cyg_drv_dsr_lock(void);
externC void cyg_drv_dsr_unlock(void);

typedef struct
{
    cyg_atomic          lock;
} cyg_drv_mutex_t;

externC void cyg_drv_mutex_init( cyg_drv_mutex_t *mutex );
externC void cyg_drv_mutex_destroy( cyg_drv_mutex_t *mutex );
externC cyg_bool_t cyg_drv_mutex_lock( cyg_drv_mutex_t *mutex );
externC cyg_bool_t cyg_drv_mutex_trylock( cyg_drv_mutex_t *mutex );
externC void cyg_drv_mutex_unlock( cyg_drv_mutex_t *mutex );
externC void cyg_drv_mutex_release( cyg_drv_mutex_t *mutex );

typedef struct
{
    cyg_atomic          wait;
    cyg_drv_mutex_t       *mutex;
} cyg_drv_cond_t;

externC void cyg_drv_cond_init( cyg_drv_cond_t  *cond, cyg_drv_mutex_t *mutex );
externC void cyg_drv_cond_destroy( cyg_drv_cond_t  *cond );
externC void cyg_drv_cond_wait( cyg_drv_cond_t *cond );
externC void cyg_drv_cond_signal( cyg_drv_cond_t *cond );
externC void cyg_drv_cond_broadcast( cyg_drv_cond_t *cond );

typedef struct cyg_interrupt
{
    cyg_vector_t        vector;
    cyg_priority_t      priority;
    cyg_ISR_t           *isr;
    cyg_DSR_t           *dsr;
    CYG_ADDRWORD        data;

    volatile struct cyg_interrupt *next_dsr;
    cyg_int32           dsr_count;
    
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN
    struct cyg_interrupt *next;
#endif

} cyg_interrupt;

externC void cyg_drv_interrupt_create(
                     cyg_vector_t        vector,
                     cyg_priority_t      priority,
                     cyg_addrword_t      data,
                     cyg_ISR_t           *isr,
                     cyg_DSR_t           *dsr,
                     cyg_handle_t        *handle,
                     cyg_interrupt       *intr
                     );
externC void cyg_drv_interrupt_delete( cyg_handle_t interrupt );
externC void cyg_drv_interrupt_attach( cyg_handle_t interrupt );
externC void cyg_drv_interrupt_detach( cyg_handle_t interrupt );

externC void cyg_drv_interrupt_mask( cyg_vector_t vector );
externC void cyg_drv_interrupt_unmask( cyg_vector_t vector );
externC void cyg_drv_interrupt_acknowledge( cyg_vector_t vector );
externC void cyg_drv_interrupt_configure(
                     cyg_vector_t        vector,
                     cyg_bool_t          level,
                     cyg_bool_t          up
                     );
externC void cyg_drv_interrupt_level( cyg_vector_t vector, cyg_priority_t level );


enum cyg_ISR_results
{
    CYG_ISR_HANDLED  = 1,               /* Interrupt was handled             */
    CYG_ISR_CALL_DSR = 2                /* Schedule DSR                      */
};

#endif /* CYGPKG_KERNEL */

/*------------------------------------------------------------------------*/
/* EOF drv_api.h                                                          */
#endif // CYGONCE_HAL_DRV_API_H
