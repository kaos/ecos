#ifndef CYGONCE_HAL_VECTORS_H
#define CYGONCE_HAL_VECTORS_H

//=============================================================================
//
//      vectors.h
//
//      SPARClite Architecture specific vector numbers &c
//
//=============================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Author(s):   hmt
// Contributors:hmt
// Date:        1998-12-10
// Purpose:     Define architecture abstractions and some shared info;
//              this file is included by assembler files as well as C/C++.
// Usage:       #include <cyg/hal/vectors.h>

//              
//####DESCRIPTIONEND####
//
//=============================================================================

#define __WINSIZE 8

#if __WINSIZE <= 8
# define __WINBITS 7
#else
# error __WINSIZE window size probably not supported
#endif

// These should be generic to all SPARCs:

#define __WINBITS_MAXIMAL 0x1f

#define __WIN_INIT (__WINSIZE - 1)
#define __WIM_INIT (1 << __WIN_INIT)

// ------------------------------------------------------------------------

#define TRAP_WUNDER     6       // Window Underflow trap number
#define TRAP_WOVER      5       // Window Overflow trap number

#define TRAP_INTR_MIN   17      // smallest interrupt trap number
#define TRAP_INTR_MAX   31      // largest interrupt trap number

#define TT_MASK         0xff0   // trap type mask from tbr
#define TT_SHL          4       // shift to get a tbr value

// Alternatively, detect an interrupt by testing tbr for being in the range
// 16-31 by masking &c:
#define TT_IS_INTR_MASK         0xf00
#define TT_IS_INTR_VALUE        0x100

#if TT_IS_INTR_VALUE != ((TRAP_INTR_MIN << TT_SHL) & TT_IS_INTR_MASK)
#error "Bad *_INTR_* symbol definition (1)"
#endif

#if TT_IS_INTR_VALUE != ((TRAP_INTR_MAX << TT_SHL) & TT_IS_INTR_MASK)
#error "Bad *_INTR_* symbol definition (2)"
#endif

#if TT_IS_INTR_VALUE != (((TRAP_INTR_MIN+1) << TT_SHL) & TT_IS_INTR_MASK)
#error "Bad *_INTR_* symbol definition (3)"
#endif

#if TT_IS_INTR_VALUE != (((TRAP_INTR_MAX-1) << TT_SHL) & TT_IS_INTR_MASK)
#error "Bad *_INTR_* symbol definition (4)"
#endif


        
//#define SCHED_LOCK_MANGLED_NAME _18Cyg_Scheduler_Base.sched_lock
#define SCHED_LOCK_MANGLED_NAME cyg_scheduler_sched_lock



#define SAVE_REGS_SIZE (4 * 32) // 32 words of 4 bytes each




#endif // CYGONCE_HAL_VECTORS_H
// EOF vectors.h
