#ifndef CYGONCE_SIGSETJMP_H
#define CYGONCE_SIGSETJMP_H
//=============================================================================
//
//      sigsetjmp.h
//
//      POSIX sigsetjmp header
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.      
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     nickg, jlarmour
// Contributors:  
// Date:          2000-03-17
// Purpose:       POSIX sigsetjmp header
// Description:   This header contains all the definitions needed to support
//                the POSIX sigsetjmp/siglongjmp API under eCos.
//              
// Usage:         This file must be included indirectly via
//                the C library setjmp.h header.
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <signal.h>
#include <cyg/hal/hal_arch.h>       // hal_jmp_buf

//=============================================================================
// sigjmp_buf structure
// The API requires this to be an array type, but this array actually
// contains three fields:
// 0..sizeof(hal_jmp_buf)-1               HAL jump buffer
// sizeof(hal_jmp_buf)                    savemask value (an int)
// sizeof(hal_jmp_buf)+sizeof(int)...     sigset_t containing saved mask

typedef struct {
    hal_jmp_buf __jmp_buf;
    int  __savemask;
    sigset_t __sigsavemask;
} sigjmp_buf[1];

//=============================================================================
// sigsetjmp() macro

#define sigsetjmp( _env_, _savemask_ )                                        \
(                                                                             \
 ((_env_)[0].__savemask = _savemask_),                                        \
 ((_savemask_)?pthread_sigmask(SIG_BLOCK,NULL,&((_env_)[0].__sigsavemask)):0),\
 hal_setjmp((_env_)[0].__jmp_buf)                                             \
)

//=============================================================================
// siglongjmp function

__externC void siglongjmp( sigjmp_buf env, int val );

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_SIGSETJMP_H
// End of sigsetjmp.h
