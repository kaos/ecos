#ifndef CYGONCE_HAL_VAR_IO_H
#define CYGONCE_HAL_VAR_IO_H

//=============================================================================
//
//      var_io.h
//
//      Processor-specific I/O support
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003, 2005 eCosCentric Ltd
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   bartv
// Date:        2003-10-08
// Usage:       #include <cyg/hal/hal_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>

// In theory an application signal handler can just return straight to
// the kernel. In reality this is usually the case as well, but with
// some kernel versions on some processors it is necessary instead to
// exit via a sigreturn system call.
externC void    cyg_hal_sys_restore(void);
externC void    cyg_hal_sys_restore_rt(void);

#define CYG_HAL_SYS_SIGACTION_ADJUST(_sig_, _sigaction_)    \
    CYG_MACRO_START                                         \
    (_sigaction_)->hal_flags    |= CYG_HAL_SYS_SA_RESTORER; \
    (_sigaction_)->hal_restorer = &cyg_hal_sys_restore;     \
    CYG_MACRO_END

// Additional information passed to a signal handler. This is useful
// for e.g. profiling.
struct cyg_hal_sys_sigcontext {
    unsigned short  hal_gs;
    unsigned short  hal_gsh;
    unsigned short  hal_fs;
    unsigned short  hal_fsh;
    unsigned short  hal_es;
    unsigned short  hal_esh;
    unsigned short  hal_ds;
    unsigned short  hal_dsh;
    unsigned long   hal_edi;
    unsigned long   hal_esi;
    unsigned long   hal_ebp;
    unsigned long   hal_esp;
    unsigned long   hal_ebx;
    unsigned long   hal_edx;
    unsigned long   hal_ecx;
    unsigned long   hal_eax;
    unsigned long   hal_trapno;
    unsigned long   hal_err;
    unsigned long   hal_eip;
    unsigned short  hal_cs;
    unsigned short  hal_csh;
    unsigned long   hal_eflags;
    unsigned long   hal_esp_at_signal;
    unsigned short  hal_ss;
    unsigned short  hal_ssh;
    void*           hal_fpstate;
    unsigned long   hal_oldmask;
    unsigned long   hal_cr2;
};

//--------------------------------------------------------------------------
#endif // CYGONCE_HAL_VAR_IO_H
// End of var_io.h
