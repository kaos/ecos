#ifndef CYGONCE_HAL_PLF_INTR_H
#define CYGONCE_HAL_PLF_INTR_H

//==========================================================================
//
//      plf_intr.h
//
//      i386/Linux Interrupt and clock support
//
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    proven
// Contributors: proven, jskov, pjo
// Date:         1999-10-15
// Purpose:      Define Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock.
//              
// Usage:
//               #include <cyg/hal/plf_intr.h>
//               ...
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_i386.h>
#include <cyg/infra/cyg_type.h>
//#include <cyg/hal/plf_intr.h>

#include <pkgconf/hal_i386_linux.h>
#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#include <cyg/kernel/ktypes.h>
#include <cyg/kernel/kapi.h>
#endif /* CYGPKG_KERNEL */


//--------------------------------------------------------------------------
// Interrupt vectors.

// Additional vectors on Linux. These match the Linux signals.
#define CYGNUM_HAL_VECTOR_SIGHUP             1
#define CYGNUM_HAL_VECTOR_SIGINT             2
#define CYGNUM_HAL_VECTOR_SIGQUIT            3
#define CYGNUM_HAL_VECTOR_SIGILL             4
#define CYGNUM_HAL_VECTOR_SIGTRAP            5
#define CYGNUM_HAL_VECTOR_SIGIOT             6
#define CYGNUM_HAL_VECTOR_SIGBUS             7
#define CYGNUM_HAL_VECTOR_SIGFPE             8
#define CYGNUM_HAL_VECTOR_SIGKILL            9
#define CYGNUM_HAL_VECTOR_SIGUSR1           10
#define CYGNUM_HAL_VECTOR_SIGSEGV           11
#define CYGNUM_HAL_VECTOR_SIGUSR2           12
#define CYGNUM_HAL_VECTOR_SIGPIPE           13
#define CYGNUM_HAL_VECTOR_SIGALRM           14
#define CYGNUM_HAL_VECTOR_SIGTERM           15
#define CYGNUM_HAL_VECTOR_SIGCHLD           17
#define CYGNUM_HAL_VECTOR_SIGCONT           18
#define CYGNUM_HAL_VECTOR_SIGSTOP           19
#define CYGNUM_HAL_VECTOR_SIGTSTP           20
#define CYGNUM_HAL_VECTOR_SIGTTIN           21
#define CYGNUM_HAL_VECTOR_SIGTTOU           22
#define CYGNUM_HAL_VECTOR_SIGURG            23
#define CYGNUM_HAL_VECTOR_SIGXCPU           24
#define CYGNUM_HAL_VECTOR_SIGXFSZ           25
#define CYGNUM_HAL_VECTOR_SIGVTALRM         26
#define CYGNUM_HAL_VECTOR_SIGPROF           27
#define CYGNUM_HAL_VECTOR_SIGWINCH          28
#define CYGNUM_HAL_VECTOR_SIGIO             29
#define CYGNUM_HAL_VECTOR_SIGPWR            30

#define CYGNUM_HAL_ISR_MIN                   0   
#define CYGNUM_HAL_ISR_MAX                    CYGNUM_HAL_VECTOR_SIGPWR
#define CYGNUM_HAL_ISR_COUNT                 ( CYGNUM_HAL_ISR_MAX + 1 )

// Generic exception vector names
#define CYGNUM_HAL_EXCEPTION_INTERRUPT            CYGNUM_HAL_VECTOR_SIGINT
#define CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION  CYGNUM_HAL_VECTOR_SIGILL
#define CYGNUM_HAL_EXCEPTION_TRAP                 CYGNUM_HAL_VECTOR_SIGTRAP
#define CYGNUM_HAL_EXCEPTION_DATA_ACCESS          CYGNUM_HAL_VECTOR_SIGBUS
#define CYGNUM_HAL_EXCEPTION_FPU                  CYGNUM_HAL_VECTOR_SIGFPE
#define CYGNUM_HAL_EXCEPTION_STACK_OVERFLOW       CYGNUM_HAL_VECTOR_SIGSEGV

#define CYGNUM_HAL_EXCEPTION_MIN             0
#define CYGNUM_HAL_EXCEPTION_MAX             CYGNUM_HAL_EXCEPTION_STACK_OVERFLOW
#define CYGNUM_HAL_EXCEPTION_COUNT           ( CYGNUM_HAL_EXCEPTION_MAX + 1 )

// The vector used by the Real time clock
#ifdef CYGSEM_HAL_I386_LINUX_REAL_TIME
#define CYGNUM_HAL_INTERRUPT_RTC              CYGNUM_HAL_VECTOR_SIGALRM
#else
#define CYGNUM_HAL_INTERRUPT_RTC              CYGNUM_HAL_VECTOR_SIGVTALRM
#endif

//--------------------------------------------------------------------------
// Static data used by HAL

// Interrupt controler emulation variables.
externC volatile int cyg_hal_interrupts_disabled;
externC volatile int cyg_hal_interrupts_deferred;
externC volatile int cyg_hal_interrupts_unhandled[CYGNUM_HAL_ISR_COUNT];

//--------------------------------------------------------------------------
// Interrupt control macros

externC void cyg_hal_default_interrupt_vsr(int vector);
externC void cyg_hal_default_exception_vsr(int vector);
externC int  cyg_hal_sys_sigprocmask(int, const long*, long*);

// FIXME: These should use block/unblock of the signals instead of
//        cyg_hal_interrupts_disabled.

#define HAL_ENABLE_INTERRUPTS()                                            \
    CYG_MACRO_START                                                        \
    cyg_ucount8 _i_;                                                       \
    cyg_hal_interrupts_disabled = 0;                                       \
    if (cyg_hal_interrupts_deferred) {                                     \
        cyg_hal_interrupts_deferred = 0;                                   \
        for (_i_ = CYGNUM_HAL_ISR_MIN; _i_ <= CYGNUM_HAL_ISR_MAX; _i_++) { \
            if (cyg_hal_interrupts_unhandled[_i_]) {                       \
                cyg_hal_default_interrupt_vsr(_i_);                        \
            }                                                              \
        }                                                                  \
    }                                                                      \
    CYG_MACRO_END

#define HAL_DISABLE_INTERRUPTS(_old_)           \
    CYG_MACRO_START                             \
    (_old_) = cyg_hal_interrupts_disabled++;    \
    CYG_MACRO_END

#define HAL_RESTORE_INTERRUPTS(_old_)                                   \
    CYG_MACRO_START                                                     \
    cyg_ucount8 _i_;                                                    \
    cyg_hal_interrupts_disabled = (_old_);                              \
    if (0 == cyg_hal_interrupts_disabled) {                             \
        if (cyg_hal_interrupts_deferred) {                              \
            cyg_hal_interrupts_deferred = 0;                            \
            for (_i_ = CYGNUM_HAL_ISR_MIN; _i_ <= CYGNUM_HAL_ISR_MAX;   \
                 _i_++){                                                \
                if (cyg_hal_interrupts_unhandled[_i_]) {                \
                    cyg_hal_default_interrupt_vsr(_i_);                 \
                }                                                       \
            }                                                           \
        }                                                               \
    }                                                                   \
    CYG_MACRO_END

#define HAL_QUERY_INTERRUPTS(_old_)             \
    CYG_MACRO_START                             \
    (_old_) = !cyg_hal_interrupts_disabled;     \
    CYG_MACRO_END

#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )                   \
    CYG_MACRO_START                                             \
    cyg_hal_interrupts_unhandled[_vector_] = 0;                 \
    CYG_MACRO_END

//---------------------------------------------------------------------------
// Translate a vector number into an ISR table index.

// The native linux port doesn't support chained interrupts --proven 19981005
#define HAL_TRANSLATE_VECTOR(_vector_,_index_) (_index_) = (_vector_)

//---------------------------------------------------------------------------
// Interrupt controller access

#define LINUX_SIG_BLOCK   0
#define LINUX_SIG_UNBLOCK 1

#define HAL_INTERRUPT_MASK( _vector_ )                          \
    CYG_MACRO_START                                             \
    long _old_, _new_;                                          \
    _new_ = 1 << (_vector_);                                    \
    cyg_hal_sys_sigprocmask(LINUX_SIG_BLOCK, &_new_, &_old_);   \
    CYG_MACRO_END

#define HAL_INTERRUPT_UNMASK( _vector_ )                        \
    CYG_MACRO_START                                             \
    long _old_, _new_;                                          \
    _new_ = 1 << (_vector_);                                    \
    cyg_hal_sys_sigprocmask(LINUX_SIG_UNBLOCK, &_new_, &_old_); \
    CYG_MACRO_END

// The native linux port doesn't have configurable interrupts.
#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_, _up_ )      

// The native linux port doesn't have interrupt levels.
#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ )            

//---------------------------------------------------------------------------
// Clock control

struct hal_timeval {
    long tv_sec;
    long tv_usec;
};
struct hal_itimerval {
    struct hal_timeval it_interval;
    struct hal_timeval it_value;
};

externC int cyg_hal_sys_setitimer(int, const struct hal_itimerval*,
                                  struct hal_itimerval*);
externC int cyg_hal_sys_getitimer(int, struct hal_itimerval*);

#ifdef CYGSEM_HAL_I386_LINUX_REAL_TIME
#define CYGNUM_HAL_TIMER_TYPE 0         // ITIMER_REAL
#else
#define CYGNUM_HAL_TIMER_TYPE 1         // ITIMER_VIRTUAL
#endif

#define HAL_CLOCK_INITIALIZE( _period_ )                                \
    CYG_MACRO_START                                                     \
    struct hal_itimerval new_itimerval, old;                            \
                                                                        \
    new_itimerval.it_interval.tv_sec = 0;                               \
    new_itimerval.it_interval.tv_usec = (_period_);                     \
    new_itimerval.it_value.tv_sec = 0;                                  \
    new_itimerval.it_value.tv_usec = (_period_);                        \
    /* Use setitimer to set the realtime clock */                       \
    cyg_hal_sys_setitimer(CYGNUM_HAL_TIMER_TYPE, &new_itimerval, &old); \
    CYG_MACRO_END

// The sigalarm runs in one-shot mode. We have to re-enable it
#define HAL_CLOCK_RESET( _vector_, _period_ ) \
    HAL_INTERRUPT_UNMASK( _vector_ )

// This timer counts down, so subtract from set value.
#define HAL_CLOCK_READ( _pvalue_ )                                      \
    CYG_MACRO_START                                                     \
    struct hal_itimerval old_itimerval;                                 \
    cyg_hal_sys_getitimer(CYGNUM_HAL_TIMER_TYPE, &old_itimerval);       \
    *(_pvalue_) = old_itimerval.it_interval.tv_usec -                   \
      old_itimerval.it_value.tv_usec;                                   \
    CYG_MACRO_END

#define HAL_CLOCK_LATENCY( _pvalue_ )           \
    HAL_CLOCK_READ((_pvalue_))

//---------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_PLF_INTR_H
// End of plf_intr.h
