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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
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

#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <pkgconf/posix.h>

#include <signal.h>

//=============================================================================
// sigjmp_buf structure
// The API requires this to be an array type, but this array actually
// contains three fields:
// 0..sizeof(hal_jmp_buf)-1               HAL jump buffer
// sizeof(hal_jmp_buf)                    savemask value (an int)
// sizeof(hal_jmp_buf)+sizeof(int)...     sigset_t containing saved mask

typedef cyg_uint8 sigjmp_buf[sizeof(hal_jmp_buf)+sizeof(int)+(sizeof(sigset_t))];

//=============================================================================
// sigsetjmp() macro

#define sigsetjmp( _env_, _savemask_ )                                        \
(                                                                             \
 (*(int *)&((_env_)[sizeof(hal_jmp_buf)])=(_savemask_)),                      \
 ((_savemask_)?pthread_sigmask(SIG_BLOCK,NULL,                                \
                                  (sigset_t *)&(_env_)[sizeof(hal_jmp_buf)+   \
                                                      sizeof(int)]):0),       \
 hal_setjmp(*(hal_jmp_buf *)&_env_[0])                                        \
)

//=============================================================================
// siglongjmp function

__externC void siglongjmp( sigjmp_buf env, int val );

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_SIGSETJMP_H
// End of sigsetjmp.h
