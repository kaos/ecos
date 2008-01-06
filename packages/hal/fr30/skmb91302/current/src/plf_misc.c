//==========================================================================
//
//      plf_misc.c
//
//      HAL platform miscellaneous functions
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg, larsi
// Contributors: nickg, jlarmour, dmoseley
// Date:         2007-07-09
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#define CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // Base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // architectural definitions

#include <cyg/hal/hal_intr.h>           // Interrupt handling

#include <cyg/hal/hal_cache.h>          // Cache handling

#include <cyg/hal/hal_if.h>


/*------------------------------------------------------------------------*/

void hal_platform_init(void)
{
#if defined(CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT)
    // Set up eCos/ROM interfaces
    hal_if_init();
#endif

#if defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT)      && \
    (defined(CYGSEM_HAL_USE_ROM_MONITOR_CygMon)    || \
     defined(CYGSEM_HAL_USE_ROM_MONITOR_GDB_stubs))

{
    extern CYG_ADDRESS hal_virtual_vector_table[32];
    void patch_dbg_syscalls( void * );
    patch_dbg_syscalls( (void *)(&hal_virtual_vector_table[0]) );
}
#endif

}

/*------------------------------------------------------------------------*/
/* Reset support                                                          */


/*------------------------------------------------------------------------*/
/* Syscall support                                                        */
#ifdef CYGPKG_CYGMON
// Cygmon provides syscall handling for this board
#include <cyg/hal/hal_stub.h>
int __get_syscall_num (void)
{
    return SIGSYS;
}
#endif


/*------------------------------------------------------------------------*/
/* End of plf_misc.c                                                      */
