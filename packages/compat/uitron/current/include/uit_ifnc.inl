#ifndef CYGONCE_COMPAT_UITRON_UIT_IFNC_INL
#define CYGONCE_COMPAT_UITRON_UIT_IFNC_INL
//===========================================================================
//
//      uit_ifnc.inl
//
//      uITRON compatibility functions
//
//===========================================================================
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

#ifdef CYGPKG_UITRON

#ifdef CYGPRI_UITRON_FUNCS_HERE_AND_NOW

class Cyg_Uit_Action {
public:
    typedef enum {
        WUP_TSK,
        SIG_SEM,
        SET_FLG,
        SND_MSG
    } action;
};

extern volatile int cyg_uit_dsr_actions_head;
extern volatile int cyg_uit_dsr_actions_tail;

#define CYGNUM_UITRON_ISR_ACTION_QUEUEMASK (CYGNUM_UITRON_ISR_ACTION_QUEUESIZE-1)

#if ((~CYGNUM_UITRON_ISR_ACTION_QUEUEMASK) &            \
     ~((~CYGNUM_UITRON_ISR_ACTION_QUEUEMASK)-1))        \
    != CYGNUM_UITRON_ISR_ACTION_QUEUESIZE
#error CYGNUM_UITRON_ISR_ACTION_QUEUESIZE not a power of 2
#endif

extern Cyg_Uit_Action::action
cyg_uit_dsr_actions[ CYGNUM_UITRON_ISR_ACTION_QUEUESIZE ];

extern ID
cyg_uit_dsr_act_ids[ CYGNUM_UITRON_ISR_ACTION_QUEUESIZE ];

extern CYG_ADDRWORD
cyg_uit_dsr_act_a1s[ CYGNUM_UITRON_ISR_ACTION_QUEUESIZE ];

CYG_UIT_FUNC_INLINE
ER
iwup_tsk ( ID tskid )
{
#ifdef CYGSEM_UITRON_ISRFUNCS_TRY_IMMEDIATE_EXECUTION
    if ( 1 >= Cyg_Scheduler::get_sched_lock() ) {
        // then this ISR is the first one, and the sched was locked by the
        // interrupt code.  So this is safe.
        return wup_tsk( tskid );
    }
#endif
    
    register int i, head;
    i = cyg_uit_dsr_actions_head;
    head = CYGNUM_UITRON_ISR_ACTION_QUEUEMASK & ( 1 + i );
    // If interrupts can be recursive, then there is a race here where a
    // slot may be overwritten by a recursive interrupt, or actions from
    // such lost; better though than having a slot contain *mixed* data
    // from two intermingled interrupts.
    if ( head != cyg_uit_dsr_actions_tail ) {
        cyg_uit_dsr_actions_head = head;
        cyg_uit_dsr_actions[ i ] = Cyg_Uit_Action::WUP_TSK;
        cyg_uit_dsr_act_ids[ i ] = tskid;
    }
    return E_OK;
}
        
#ifdef CYGPKG_UITRON_SEMAS
#if 0 < CYG_UITRON_NUM( SEMAS )
CYG_UIT_FUNC_INLINE
ER
isig_sem ( ID semid )
{
#ifdef CYGSEM_UITRON_ISRFUNCS_TRY_IMMEDIATE_EXECUTION
    if ( 1 >= Cyg_Scheduler::get_sched_lock() ) {
        // then this ISR is the first one, and the sched was locked by the
        // interrupt code.  So this is safe.
        return sig_sem( semid );
    }
#endif
    
    register int i, head;
    i = cyg_uit_dsr_actions_head;
    head = CYGNUM_UITRON_ISR_ACTION_QUEUEMASK & ( 1 + i );
    // If interrupts can be recursive, then there is a race here where a
    // slot may be overwritten by a recursive interrupt, or actions from
    // such lost; better though than having a slot contain *mixed* data
    // from two intermingled interrupts.
    if ( head != cyg_uit_dsr_actions_tail ) {
        cyg_uit_dsr_actions_head = head;
        cyg_uit_dsr_actions[ i ] = Cyg_Uit_Action::SIG_SEM;
        cyg_uit_dsr_act_ids[ i ] = semid;
    }
    return E_OK;
}
#endif // 0 < CYG_UITRON_NUM( SEMAS )
#endif // CYGPKG_UITRON_SEMAS

#ifdef CYGPKG_UITRON_FLAGS
#if 0 < CYG_UITRON_NUM( FLAGS )
CYG_UIT_FUNC_INLINE
ER
iset_flg ( ID flgid, UINT setptn )
{
#ifdef CYGSEM_UITRON_ISRFUNCS_TRY_IMMEDIATE_EXECUTION
    if ( 1 >= Cyg_Scheduler::get_sched_lock() ) {
        // then this ISR is the first one, and the sched was locked by the
        // interrupt code.  So this is safe.
        return set_flg( flgid, setptn );
    }
#endif
    
    register int i, head;
    i = cyg_uit_dsr_actions_head;
    head = CYGNUM_UITRON_ISR_ACTION_QUEUEMASK & ( 1 + i );
    // If interrupts can be recursive, then there is a race here where a
    // slot may be overwritten by a recursive interrupt, or actions from
    // such lost; better though than having a slot contain *mixed* data
    // from two intermingled interrupts.
    if ( head != cyg_uit_dsr_actions_tail ) {
        cyg_uit_dsr_actions_head = head;
        cyg_uit_dsr_actions[ i ] = Cyg_Uit_Action::SET_FLG;
        cyg_uit_dsr_act_ids[ i ] = flgid;
        cyg_uit_dsr_act_a1s[ i ] = (CYG_ADDRWORD)setptn;
    }
    return E_OK;
}
#endif // 0 < CYG_UITRON_NUM( FLAGS )
#endif // CYGPKG_UITRON_FLAGS

#ifdef CYGPKG_UITRON_MBOXES
#if 0 < CYG_UITRON_NUM( MBOXES )
CYG_UIT_FUNC_INLINE
ER
isnd_msg ( ID mbxid, T_MSG *pk_msg )
{
#ifdef CYGSEM_UITRON_ISRFUNCS_TRY_IMMEDIATE_EXECUTION
    if ( 1 >= Cyg_Scheduler::get_sched_lock() ) {
        // then this ISR is the first one, and the sched was locked by the
        // interrupt code.  So this is safe.
        return snd_msg( mbxid, pk_msg );
    }
#endif
    
    register int i, head;
    i = cyg_uit_dsr_actions_head;
    head = CYGNUM_UITRON_ISR_ACTION_QUEUEMASK & ( 1 + i );
    // If interrupts can be recursive, then there is a race here where a
    // slot may be overwritten by a recursive interrupt, or actions from
    // such lost; better though than having a slot contain *mixed* data
    // from two intermingled interrupts.
    if ( head != cyg_uit_dsr_actions_tail ) {
        cyg_uit_dsr_actions_head = head;
        cyg_uit_dsr_actions[ i ] = Cyg_Uit_Action::SND_MSG;
        cyg_uit_dsr_act_ids[ i ] = mbxid;
        cyg_uit_dsr_act_a1s[ i ] = (CYG_ADDRWORD)pk_msg;
    }
    return E_OK;
}
#endif // 0 < CYG_UITRON_NUM( MBOXES )
#endif // CYGPKG_UITRON_MBOXES
        
// ========================================================================

#endif // CYGPKG_UITRON

#endif // CYGPRI_UITRON_FUNCS_HERE_AND_NOW

#endif // CYGONCE_COMPAT_UITRON_UIT_IFNC_INL
//EOF uit_ifnc.inl
