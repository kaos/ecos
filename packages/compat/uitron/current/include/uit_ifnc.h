#ifndef CYGONCE_COMPAT_UITRON_UIT_IFNC_H
#define CYGONCE_COMPAT_UITRON_UIT_IFNC_H
//===========================================================================
//
//      uit_ifnc.h
//
//      uITRON compatibility functions
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   hmt
// Contributors:        hmt
// Date:        1999-08-16
// Purpose:     uITRON compatibility functions
// Description: 
//
//####DESCRIPTIONEND####
//
//===========================================================================

// ------------------------------------------------------------------------
// Source Code Organization
//
// First, see pkgconf/uitron.h for details of applicable configuration
// options.
//
// This file uit_ifnc.h provides prototypes for the task-independent parts
// of the uITRON API, that is functions named ixxx_yyy() for calling in
// ISRs.  We also define the uitron helper DSR that is needed to despool
// stored up requests.
// ------------------------------------------------------------------------

#include <pkgconf/uitron.h>             // uITRON setup CYGNUM_UITRON_SEMAS
                                        // CYGPKG_UITRON et al

#ifdef CYGPKG_UITRON

#include <cyg/infra/cyg_type.h>         // types; cyg_int32, CYG_ADDRWORD

#include <cyg/compat/uitron/uit_type.h> // uITRON types; ER ID TMO T_MSG
#include <cyg/compat/uitron/uit_func.h> // uITRON funcs and control macros.

// ========================================================================
//         u I T R O N   F U N C T I O N S
// The function declarations themselves:

CYG_UIT_FUNC_EXTERN_BEGIN

// ******************************************************
// ***    6.5 C Language Interfaces                   ***
// ******************************************************

// - Task Management Functions

// (None)
        
// - Task-Dependent Synchronization Functions
        
//ER      irsm_tsk ( ID tskid );
//ER      ifrsm_tsk ( ID tskid );

ER      iwup_tsk ( ID tskid );
        
// - Synchronization and Communication Functions
        
ER      isig_sem ( ID semid );

ER      iset_flg ( ID flgid, UINT setptn );

ER      isnd_msg ( ID mbxid, T_MSG *pk_msg );
        
// - Extended Synchronization and Communication Functions
        
// - Interrupt Management Functions
        
// (None)
   
// ---------------------------------------------------------------

#define CYGPRI_UITRON_SET_RETCODE( _z_ ) do {                                   \
    extern volatile int cyg_uit_dsr_actions_head;                               \
    extern volatile int cyg_uit_dsr_actions_tail;                               \
    (_z_) = (cyg_uit_dsr_actions_head == cyg_uit_dsr_actions_tail) ? 0 : 3;     \
} while ( 0 )

//void    ret_wup ( ID tskid );
// Awaken the task (safely) and return Cyg_Interrupt::CALL_DSR
#define ret_wup( _id_ ) do {                    \
    register int retcode;                       \
    (void)iwup_tsk( (_id_) );                   \
    CYGPRI_UITRON_SET_RETCODE( retcode );       \
    return retcode;                             \
} while ( 0 )

// Subsitute a version of ret_int that returns Cyg_Interrupt::CALL_DSR
#undef ret_int()
#define ret_int()  do {                         \
    register int retcode;                       \
    CYGPRI_UITRON_SET_RETCODE( retcode );       \
    return retcode;                             \
} while ( 0 )


// - Memorypool Management Functions

// (None)
        
// - Time Management Functions
        
// (None)
        
// - System Management Functions
        
// (None)
        
// - Network Support Functions
        
// (None)
        
// ========================================================================
// DSR: use this DSR with the uITRON-type ISR that uses the functions above
// to get delayed/safe execution of the wakeup-type functions above.

void cyg_uitron_dsr( unsigned int vector, unsigned int count, unsigned int data );

CYG_UIT_FUNC_EXTERN_END

// ========================================================================

#ifdef CYGPRI_UITRON_FUNCS_HERE_AND_NOW
// functions are inline OR we are in the outline implementation, so define
// the functions as inlines or plain functions depending on the value of
// CYG_UIT_FUNC_INLINE from above.
#include <cyg/compat/uitron/uit_ifnc.inl>
#endif // CYGPRI_UITRON_FUNCS_HERE_AND_NOW

// ------------------------------------------------------------------------
#endif // CYGPKG_UITRON

#endif // CYGONCE_COMPAT_UITRON_UIT_IFNC_H
// EOF uit_ifnc.h
