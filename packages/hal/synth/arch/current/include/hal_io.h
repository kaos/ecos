#ifndef CYGONCE_HAL_HAL_IO_H
#define CYGONCE_HAL_HAL_IO_H

//=============================================================================
//
//      hal_io.h
//
//      HAL device IO register support.
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
// Author(s):   nickg
// Contributors:nickg, bartv
// Date:        1998-02-17
// Purpose:     Define IO register support
// Description: The macros defined here provide the HAL APIs for handling
//              device IO control registers.
//
//              For the synthetic target these macros should never
//              actually be used since the application will run as an
//              ordinary user application and should not have
//              permission to access any real hardware. Instead
//              hardware access should go via the auxiliary. Possibly
//              the macros should be #pragma poison'd, but some people
//              may want to run the synthetic target in a way that
//              does involve accessing real hardware.
//              
//              The synthetic target provides some additional I/O
//              facilities in the form of Linux system calls. A useful
//              subset of these are prototyped here, together with
//              associated constants. There are also I/O operations to
//              interact with the auxiliary.
//
// Usage:
//              #include <cyg/hal/hal_io.h>
//              ...
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>

//-----------------------------------------------------------------------------
// IO Register address.
// This type is for recording the address of an IO register.

typedef volatile CYG_ADDRWORD HAL_IO_REGISTER;

//-----------------------------------------------------------------------------
// BYTE Register access.
// Individual and vectorized access to 8 bit registers.

#define HAL_READ_UINT8( _register_, _value_ )           \
    CYG_MACRO_START                                     \
    ((_value_) = *((volatile CYG_BYTE *)(_register_))); \
    CYG_MACRO_END

#define HAL_WRITE_UINT8( _register_, _value_ )          \
    CYG_MACRO_START                                     \
    (*((volatile CYG_BYTE *)(_register_)) = (_value_)); \
    CYG_MACRO_END

#define HAL_READ_UINT8_VECTOR( _register_, _buf_, _count_, _step_ )     \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_,_j_;                                                \
    for( _i_ = 0, _j_ = 0; _i_ < (_count_); _i_++, _j_ += (_step_)) {   \
        (_buf_)[_i_] = ((volatile CYG_BYTE *)(_register_))[_j_];        \
    }                                                                   \
    CYG_MACRO_END

#define HAL_WRITE_UINT8_VECTOR( _register_, _buf_, _count_, _step_ )    \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_,_j_;                                                \
    for( _i_ = 0, _j_ = 0; _i_ < (_count_); _i_++, _j_ += (_step_)) {   \
        ((volatile CYG_BYTE *)(_register_))[_j_] = (_buf_)[_i_];        \
    }                                                                   \
    CYG_MACRO_END


//-----------------------------------------------------------------------------
// 16 bit access.
// Individual and vectorized access to 16 bit registers.
    
#define HAL_READ_UINT16( _register_, _value_ )                  \
    CYG_MACRO_START                                             \
    ((_value_) = *((volatile CYG_WORD16 *)(_register_)));       \
    CYG_MACRO_END

#define HAL_WRITE_UINT16( _register_, _value_ )                 \
    CYG_MACRO_START                                             \
    (*((volatile CYG_WORD16 *)(_register_)) = (_value_));       \
    CYG_MACRO_END

#define HAL_READ_UINT16_VECTOR( _register_, _buf_, _count_, _step_ )    \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_,_j_;                                                \
    for( _i_ = 0, _j_ = 0; _i_ < (_count_); _i_++, _j_ += (_step_)) {   \
        (_buf_)[_i_] = ((volatile CYG_WORD16 *)(_register_))[_j_];      \
    }                                                                   \
    CYG_MACRO_END

#define HAL_WRITE_UINT16_VECTOR( _register_, _buf_, _count_, _step_ )   \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_,_j_;                                                \
    for( _i_ = 0, _j_ = 0; _i_ < (_count_); _i_++, _j_ += (_step_)) {   \
        ((volatile CYG_WORD16 *)(_register_))[_j_] = (_buf_)[_i_];      \
    }                                                                   \
    CYG_MACRO_END

//-----------------------------------------------------------------------------
// 32 bit access.
// Individual and vectorized access to 32 bit registers.
    
#define HAL_READ_UINT32( _register_, _value_ )                  \
    CYG_MACRO_START                                             \
    ((_value_) = *((volatile CYG_WORD32 *)(_register_)));       \
    CYG_MACRO_END

#define HAL_WRITE_UINT32( _register_, _value_ )                 \
    CYG_MACRO_START                                             \
    (*((volatile CYG_WORD32 *)(_register_)) = (_value_));       \
    CYG_MACRO_END

#define HAL_READ_UINT32_VECTOR( _register_, _buf_, _count_, _step_ )    \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_,_j_;                                                \
    for( _i_ = 0, _j_ = 0; _i_ < (_count_); _i_++, _j_ += (_step_)) {   \
        (_buf_)[_i_] = ((volatile CYG_WORD32 *)(_register_))[_j_];      \
    }                                                                   \
    CYG_MACRO_END

#define HAL_WRITE_UINT32_VECTOR( _register_, _buf_, _count_, _step_ )   \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_,_j_;                                                \
    for( _i_ = 0, _j_ = 0; _i_ < (_count_); _i_++, _j_ += (_step_)) {   \
        ((volatile CYG_WORD32 *)(_register_))[_j_] = (_buf_)[_i_];      \
    }                                                                   \
    CYG_MACRO_END

// ----------------------------------------------------------------------------
// Linux system calls and associated structures and constants. This is
// by no means a complete list, but there is enough information for
// the needs of the relevant HAL packages. The information needs to be
// kept in synch with the Linux header files, but in practice
// divergence will be rare because that would imply incompatible
// changes in the Linux kernel API.
//
// It may seem tempting to import the Linux header files directly, but
// that would prevent cross-compilation and introduce all kinds of
// namespace pollution.
//
// The actual implementation lives in variant HAL packages since
// typically they involve direct system calls via bits of assembler.
// Note that only a subset of system calls are actually implemented,
// so the variant HALs may need to be updated if this list needs to
// be extended.

// Error codes.
#define CYG_HAL_SYS_EINTR                4
#define CYG_HAL_SYS_EAGAIN              11

// Signal-related information
#define CYG_HAL_SYS_SIGHUP               1
#define CYG_HAL_SYS_SIGINT               2
#define CYG_HAL_SYS_SIGQUIT              3
#define CYG_HAL_SYS_SIGILL               4
#define CYG_HAL_SYS_SIGBUS               7
#define CYG_HAL_SYS_SIGFPE               8
#define CYG_HAL_SYS_SIGSEGV             11
#define CYG_HAL_SYS_SIGPIPE             13
#define CYG_HAL_SYS_SIGALRM             14
#define CYG_HAL_SYS_SIGTERM             15
#define CYG_HAL_SYS_SIGCHLD             17
#define CYG_HAL_SYS_SIGCONT             18
#define CYG_HAL_SYS_SIGSTOP             19
#define CYG_HAL_SYS_SIGTSTP             20
#define CYG_HAL_SYS_SIGIO               29

#define CYG_HAL_SYS_SA_NOCLDSTOP        0x00000001
#define CYG_HAL_SYS_SA_NOCLDWAIT        0x00000002
#define CYG_HAL_SYS_SA_SIGINFO          0x00000004
#define CYG_HAL_SYS_SA_RESTART          0x10000000
#define CYG_HAL_SYS_SA_NODEFER          0x40000000
#define CYG_HAL_SYS_SIG_BLOCK           0
#define CYG_HAL_SYS_SIG_UNBLOCK         1
#define CYG_HAL_SYS_SIG_SETMASK         2

#define CYG_HAL_SYS__NSIG               64
#define CYG_HAL_SYS__SIGBITS            (8 * sizeof(unsigned long))
#define CYG_HAL_SYS__SIGELT(_d_)        ((_d_) / CYG_HAL_SYS__SIGBITS)
#define CYG_HAL_SYS__SIGMASK(_d_)       ((unsigned long)1 << ((_d_) % CYG_HAL_SYS__SIGBITS))

typedef struct cyg_hal_sys_sigset_t {
    unsigned long hal_sig_bits[CYG_HAL_SYS__NSIG / CYG_HAL_SYS__SIGBITS];
} cyg_hal_sys_sigset_t;

#define CYG_HAL_SYS_SIGFILLSET(_set_)                                                   \
    CYG_MACRO_START                                                                     \
        unsigned int __i;                                                               \
        for (__i = 0; __i < (CYG_HAL_SYS__NSIG / CYG_HAL_SYS__SIGBITS); __i++) {        \
            (_set_)->hal_sig_bits[__i] = ~0;                                            \
        }                                                                               \
    CYG_MACRO_END

#define CYG_HAL_SYS_SIGEMPTYSET(_set_)                                                  \
    CYG_MACRO_START                                                                     \
        unsigned int __i;                                                               \
        for (__i = 0; __i < (CYG_HAL_SYS__NSIG / CYG_HAL_SYS__SIGBITS); __i++) {        \
            (_set_)->hal_sig_bits[__i] = 0;                                             \
        }                                                                               \
    CYG_MACRO_END

#define CYG_HAL_SYS_SIGADDSET(_set_, _bit_)                                                     \
    CYG_MACRO_START                                                                             \
    (_set_)->hal_sig_bits[CYG_HAL_SYS__SIGELT(_bit_ - 1)] |= CYG_HAL_SYS__SIGMASK(_bit_ - 1);   \
    CYG_MACRO_END

#define CYG_HAL_SYS_SIGDELSET(_set_, _bit_)                                                     \
    CYG_MACRO_START                                                                             \
    (_set_)->hal_sig_bits[CYG_HAL_SYS__SIGELT(_bit_ - 1)] &= ~CYG_HAL_SYS__SIGMASK(_bit_ - 1);  \
    CYG_MACRO_END
               
#define CYG_HAL_SYS_SIGISMEMBER(_set_, _bit_)                                                   \
    (0 != ((_set_)->hal_sig_bits[CYG_HAL_SYS__SIGELT(_bit_ - 1)] & CYG_HAL_SYS__SIGMASK(_bit_ - 1)))

struct cyg_hal_sys_sigaction {
    void        (*hal_handler)(int);
    long        hal_mask;
    int         hal_flags;
    void        (*hal_bogus)(int);
};

// Time support.
struct cyg_hal_sys_timeval {
    long        hal_tv_sec;
    long        hal_tv_usec;
};

struct cyg_hal_sys_timezone {
    int         hal_tz_minuteswest;
    int         hal_tz_dsttime;
};

// Select support. Initially this is used only by the idle handler.
#define CYG_HAL_SYS_FD_SETSIZE          1024
#define CYG_HAL_SYS__NFDBITS            (8 * sizeof(unsigned long))
#define CYG_HAL_SYS__FDELT(_d_)         ((_d_) / CYG_HAL_SYS__NFDBITS)
#define CYG_HAL_SYS__FDMASK(_d_)        ((unsigned long)1 << ((_d_) % CYG_HAL_SYS__NFDBITS))

struct cyg_hal_sys_fd_set {
    unsigned long hal_fds_bits[CYG_HAL_SYS_FD_SETSIZE / CYG_HAL_SYS__NFDBITS];
};
#define CYG_HAL_SYS_FD_ZERO(_fdsp_)                                     \
    do {                                                                \
        unsigned int __i;                                               \
        for (__i = 0;                                                   \
             __i < (CYG_HAL_SYS_FD_SETSIZE / CYG_HAL_SYS__NFDBITS);     \
             __i++) {                                                   \
           (_fdsp_)->hal_fds_bits[__i] = 0;                             \
     } while (0);
    
#define CYG_HAL_SYS_FD_SET(_fd_, _fdsp_)                                \
    CYG_MACRO_START                                                     \
    (_fdsp_)->hal_fds_bits[CYG_HAL_SYS__FDELT(_fd_)] |= CYG_HAL_SYS__FDMASK(_fd_); \
    CYG_MACRO_END
    
#define CYG_HAL_SYS_FD_CLR(_fd_, _fdsp_)                                \
    CYG_MACRO_START                                                     \
    (_fdsp_)->hal_fds_bits[CYG_HAL_SYS__FDELT(_fd_)] &= ~CYG_HAL_SYS__FDMASK(_fd_); \
    CYG_MACRO_END

#define CYG_HAL_SYS_FD_ISSET(_fd_, _fdsp_) \
    (0 != ((_fdsp_)->hal_fds_bits[CYG_HAL_SYS__FDELT(_fd_)] & CYG_HAL_SYS__FDMASK(_fd_)))

// Interval timer support, needed for the clock.
#define CYG_HAL_SYS_ITIMER_REAL     0
#define CYG_HAL_SYS_ITIMER_VIRTUAL  1
#define CYG_HAL_SYS_ITIMER_PROF     2
 
struct cyg_hal_sys_itimerval {
    struct cyg_hal_sys_timeval  hal_it_interval;
    struct cyg_hal_sys_timeval  hal_it_value;
};
 
// System calls, or rather the subset that is needed internally.
externC unsigned long   cyg_hal_sys_write(int, const void*, long);
externC unsigned long   cyg_hal_sys_read(int, void*, long);
externC int             cyg_hal_sys_fdatasync(int); 
externC int             cyg_hal_sys_sigaction(int, 
                                              const struct cyg_hal_sys_sigaction*,
                                              struct cyg_hal_sys_sigaction*);
externC int             cyg_hal_sys_sigprocmask(int,
                                                const cyg_hal_sys_sigset_t*,
                                                cyg_hal_sys_sigset_t*);
externC int             cyg_hal_sys__newselect(int,
                                               struct cyg_hal_sys_fd_set*,
                                               struct cyg_hal_sys_fd_set*,
                                               struct cyg_hal_sys_fd_set*,
                                               struct cyg_hal_sys_timeval*);
externC int             cyg_hal_sys_setitimer(int,
                                              const struct cyg_hal_sys_itimerval*,
                                              struct cyg_hal_sys_itimerval*);
externC int             cyg_hal_sys_gettimeofday(struct cyg_hal_sys_timeval*,
                                                 struct cyg_hal_sys_timezone*);

// The actual implementation appears to return the new brk() value.
externC void*           cyg_hal_sys_brk(void*);
 
// ----------------------------------------------------------------------------
// Interaction between the application and the auxiliary.
// Not yet available, but there is a hardware-initialization routine.
externC void hal_synthetic_target_init(void);
 
//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_HAL_IO_H
// End of hal_io.h
