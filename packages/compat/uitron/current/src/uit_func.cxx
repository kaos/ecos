//===========================================================================
//
//      uit_func.cxx
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
// Purpose:     uITRON compatibility functions
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
#define CYGPRI_UITRON_FUNCS_HERE_AND_NOW
#include <cyg/compat/uitron/uit_func.h>

cyg_uint32 cyg_uitron_dis_dsp_old_priority = 0;

// ------------------------------------------------------------------------
//                  STARTUP
// this routine is outside the uITRON specification; call it from
// cyg_start(), cyg_package_start(), cyg_prestart() or cyg_user_start()
// to start the uITRON tasks and scheduler.

#if CYGNUM_UITRON_START_TASKS < 0
#error CYGNUM_UITRON_START_TASKS should be >= 0
#endif

#if CYGNUM_UITRON_START_TASKS == 0
#define START_TASKS CYGNUM_UITRON_TASKS
#else
#define START_TASKS CYGNUM_UITRON_START_TASKS
#endif

#ifdef CYGPKG_UITRON_TASKS_CREATE_DELETE
#if START_TASKS > CYGNUM_UITRON_TASKS_INITIALLY
#undef START_TASKS
#define START_TASKS CYGNUM_UITRON_TASKS_INITIALLY
#endif
#endif

#if START_TASKS > CYGNUM_UITRON_TASKS
#undef START_TASKS
#define START_TASKS CYGNUM_UITRON_TASKS
#endif

#if START_TASKS <= 0
#error Number of uITRON tasks to start initially must be >= 0
#endif


#define SET_UP_PTRS( _which_ ) CYG_MACRO_START                            \
    for ( i = 0;                                                          \
          (i < CYGNUM_UITRON_ ## _which_ ## _INITIALLY) &&                \
          (i < CYGNUM_UITRON_ ## _which_              )    ;              \
          i++ ) {                                                         \
        CYG_UITRON_PTRS( _which_ )[ i ] = CYG_UITRON_OBJS( _which_ ) + i; \
    }                                                                     \
    if ( (CYGNUM_UITRON_ ## _which_ ## _INITIALLY)                        \
          < (CYGNUM_UITRON_ ## _which_) )                                 \
        for ( /* i as is */; i < CYGNUM_UITRON_ ## _which_ ; i++ ) {      \
            CYG_UITRON_PTRS( _which_ )[ i ] = NULL;                       \
        }                                                                 \
CYG_MACRO_END

void cyg_uitron_start( void )
{
    cyg_int32 i;
    for ( i = 0; i < START_TASKS; i++ ) {
#ifdef CYGIMP_THREAD_PRIORITY
        // save the initial priority in our private array
        cyg_uitron_task_initial_priorities[ i ] = 
            cyg_uitron_TASKS[ i ].get_priority();
#endif
        // and awaken the task:
        cyg_uitron_TASKS[ i ].resume();
    }
    for ( /* i as is */; i < CYGNUM_UITRON_TASKS; i++ ) {
#ifdef CYGIMP_THREAD_PRIORITY
        // save the initial priority in our private array
        cyg_uitron_task_initial_priorities[ i ] = 
            cyg_uitron_TASKS[ i ].get_priority();
#endif
        // but ensure the task state is dormant.
        cyg_uitron_TASKS[ i ].kill();
    }

#ifdef CYGPKG_UITRON_TASKS_CREATE_DELETE
    SET_UP_PTRS( TASKS );
#endif
#ifdef CYGPKG_UITRON_SEMAS_CREATE_DELETE
    SET_UP_PTRS( SEMAS );
#endif
#ifdef CYGPKG_UITRON_MBOXES_CREATE_DELETE
    SET_UP_PTRS( MBOXES );
#endif
#ifdef CYGPKG_UITRON_FLAGS_CREATE_DELETE
    SET_UP_PTRS( FLAGS );
#endif
#ifdef CYGPKG_UITRON_MEMPOOLFIXED_CREATE_DELETE
    SET_UP_PTRS( MEMPOOLFIXED );
#endif
#ifdef CYGPKG_UITRON_MEMPOOLVAR_CREATE_DELETE
    SET_UP_PTRS( MEMPOOLVAR );
#endif
}

// These allow programs to link when cyg_uitron_start() is called
// (often because of CYGSEM_START_UITRON_COMPATIBILITY from infra,
//  though we define these regardless just in case)
// even when there is no interest in uITRON and so the tasks are
// not externally defined; the reference to cyg_uitron_start()
// ensures the tasks array et al are still included...
extern "C" {
    void task1( unsigned int arg ) CYGBLD_ATTRIB_WEAK;
    void task2( unsigned int arg ) CYGBLD_ATTRIB_WEAK;
    void task3( unsigned int arg ) CYGBLD_ATTRIB_WEAK;
    void task4( unsigned int arg ) CYGBLD_ATTRIB_WEAK;
}

void task1( unsigned int arg ) {}
void task2( unsigned int arg ) {}
void task3( unsigned int arg ) {}
void task4( unsigned int arg ) {}

#endif // CYGPKG_UITRON

// EOF uit_func.cxx
