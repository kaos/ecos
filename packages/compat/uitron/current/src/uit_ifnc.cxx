//===========================================================================
//
//      uit_ifnc.cxx
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
// Date:        1998-03-13
// Purpose:     uITRON compatibility functions for use in ISRs
// Description: 
//
//####DESCRIPTIONEND####
//
//===========================================================================

#include <pkgconf/uitron.h>             // uITRON setup CYGNUM_UITRON_SEMAS
                                        // CYGPKG_UITRON et al

#ifdef CYGPKG_UITRON

// invoke the inline function definition to create static C linkage
// functions here:
#define CYGIMP_UITRON_INLINE_FUNCS
#include <cyg/compat/uitron/uit_func.h>

// Now ensure that we create *outline* funcs for the ixxx_yyy() functions
// here, with C names or whatever, as required.

#undef CYGPRI_UITRON_FUNCS_HERE_AND_NOW
#undef CYGIMP_UITRON_INLINE_FUNCS
#undef CYG_UIT_FUNC_EXTERN_BEGIN
#undef CYG_UIT_FUNC_EXTERN_END

#ifdef CYGIMP_UITRON_CPP_OUTLINE_FUNCS
#define CYG_UIT_FUNC_EXTERN_BEGIN       extern "C++" {
#define CYG_UIT_FUNC_EXTERN_END         }
#else
#define CYG_UIT_FUNC_EXTERN_BEGIN       extern "C" {
#define CYG_UIT_FUNC_EXTERN_END         }
#endif

// Get extern C prototypes (or whatever uit_func.h above did)
#include <cyg/compat/uitron/uit_ifnc.h>

#undef  CYG_UIT_FUNC_INLINE
#define CYG_UIT_FUNC_INLINE /* blank */
#define CYGPRI_UITRON_FUNCS_HERE_AND_NOW
#include <cyg/compat/uitron/uit_ifnc.inl>

volatile int cyg_uit_dsr_actions_head = 0;
volatile int cyg_uit_dsr_actions_tail = 0;

Cyg_Uit_Action::action
cyg_uit_dsr_actions[ CYGNUM_UITRON_ISR_ACTION_QUEUESIZE ];

ID
cyg_uit_dsr_act_ids[ CYGNUM_UITRON_ISR_ACTION_QUEUESIZE ];

CYG_ADDRWORD
cyg_uit_dsr_act_a1s[ CYGNUM_UITRON_ISR_ACTION_QUEUESIZE ];

void
cyg_uitron_dsr( unsigned int vector, unsigned int count, unsigned int data )
{
    while ( cyg_uit_dsr_actions_tail != cyg_uit_dsr_actions_head ) {
        switch ( cyg_uit_dsr_actions[ cyg_uit_dsr_actions_tail ] ) {
        case Cyg_Uit_Action::WUP_TSK:
            (void)wup_tsk( cyg_uit_dsr_act_ids[ cyg_uit_dsr_actions_tail ] );
            break;
#ifdef CYGPKG_UITRON_SEMAS
#if 0 < CYG_UITRON_NUM( SEMAS )
        case Cyg_Uit_Action::SIG_SEM:
            (void)sig_sem( cyg_uit_dsr_act_ids[ cyg_uit_dsr_actions_tail ] );
            break;
#endif // 0 < CYG_UITRON_NUM( SEMAS )
#endif // CYGPKG_UITRON_SEMAS
#ifdef CYGPKG_UITRON_FLAGS
#if 0 < CYG_UITRON_NUM( FLAGS )
        case Cyg_Uit_Action::SET_FLG:
            (void)set_flg( cyg_uit_dsr_act_ids[ cyg_uit_dsr_actions_tail ],
                     (UINT)cyg_uit_dsr_act_a1s[ cyg_uit_dsr_actions_tail ] );
            break;
#endif // 0 < CYG_UITRON_NUM( FLAGS )
#endif // CYGPKG_UITRON_FLAGS
#ifdef CYGPKG_UITRON_MBOXES
#if 0 < CYG_UITRON_NUM( MBOXES )
        case Cyg_Uit_Action::SND_MSG:
            (void)snd_msg( cyg_uit_dsr_act_ids[ cyg_uit_dsr_actions_tail ],
                  (T_MSG *)cyg_uit_dsr_act_a1s[ cyg_uit_dsr_actions_tail ] );
            break;
#endif // 0 < CYG_UITRON_NUM( MBOXES )
#endif // CYGPKG_UITRON_MBOXES
        default:
            CYG_FAIL( "enum Cyg_Uit_Action out of range!" );
        }
        cyg_uit_dsr_actions_tail =
            CYGNUM_UITRON_ISR_ACTION_QUEUEMASK & (1+cyg_uit_dsr_actions_tail);
    }
}

#endif // CYGPKG_UITRON

// EOF uit_ifnc.cxx
