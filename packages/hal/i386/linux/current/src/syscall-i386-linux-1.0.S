//=============================================================================
//
//      syscall-i386-linux-1.0.S
//
//      Linux system call interface functions for i386.
//
//=============================================================================
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
// October 31, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998, 1999 Cygnus Solutions.  
// All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   proven
// Contributors:proven
// Date:        1998-10-06
// Description: Machine dependent syscalls for i386/i486/i586
//
//####DESCRIPTIONEND####
//
//=============================================================================

// Force __ELF__ for eCos/Linux.
#ifndef __ELF__
#define __ELF__
#endif

#ifndef _SYS_SYSCALL_H
#define _SYS_SYSCALL_H

#define SYS_setup               0 // Used only by init, to get system going.
#define SYS_exit                1
#define SYS_fork                2
#define SYS_read                3
#define SYS_write               4
#define SYS_open                5
#define SYS_close               6
#define SYS_waitpid             7
#define SYS_creat               8
#define SYS_link                9
#define SYS_unlink              10
#define SYS_execve              11
#define SYS_chdir               12
#define SYS_time                13
#define SYS_prev_mknod          14
#define SYS_chmod               15
#define SYS_chown               16
#define SYS_break               17
#define SYS_oldstat             18
#define SYS_lseek               19
#define SYS_getpid              20
#define SYS_mount               21
#define SYS_umount              22
#define SYS_setuid              23
#define SYS_getuid              24
#define SYS_stime               25
#define SYS_ptrace              26
#define SYS_alarm               27
#define SYS_oldfstat            28
#define SYS_pause               29
#define SYS_utime               30
#define SYS_stty                31
#define SYS_gtty                32
#define SYS_access              33
#define SYS_nice                34
#define SYS_ftime               35
#define SYS_sync                36
#define SYS_kill                37
#define SYS_rename              38
#define SYS_mkdir               39
#define SYS_rmdir               40
#define SYS_dup                 41
#define SYS_pipe                42
#define SYS_times               43
#define SYS_prof                44
#define SYS_brk                 45
#define SYS_setgid              46
#define SYS_getgid              47
#define SYS_signal              48
#define SYS_geteuid             49
#define SYS_getegid             50
#define SYS_acct                51
#define SYS_phys                52
#define SYS_lock                53
#define SYS_ioctl               54
#define SYS_fcntl               55
#define SYS_mpx                 56
#define SYS_setpgid             57
#define SYS_ulimit              58
#define SYS_oldolduname         59
#define SYS_umask               60
#define SYS_chroot              61
#define SYS_prev_ustat          62
#define SYS_dup2                63
#define SYS_getppid             64
#define SYS_getpgrp             65
#define SYS_setsid              66
#define SYS_sigaction           67
#define SYS_siggetmask          68
#define SYS_sigsetmask          69
#define SYS_setreuid            70
#define SYS_setregid            71
#define SYS_sigsuspend          72
#define SYS_sigpending          73
#define SYS_sethostname         74
#define SYS_setrlimit           75
#define SYS_getrlimit           76
#define SYS_getrusage           77
#define SYS_gettimeofday        78
#define SYS_settimeofday        79
#define SYS_getgroups           80
#define SYS_setgroups           81
#define SYS_select              82
#define SYS_symlink             83
#define SYS_oldlstat            84
#define SYS_readlink            85
#define SYS_uselib              86
#define SYS_swapon              87
#define SYS_reboot              88
#define SYS_readdir             89
#define SYS_mmap                90
#define SYS_munmap              91
#define SYS_truncate            92
#define SYS_ftruncate           93
#define SYS_fchmod              94
#define SYS_fchown              95
#define SYS_getpriority         96
#define SYS_setpriority         97
#define SYS_profil              98
#define SYS_statfs              99
#define SYS_fstatfs             100
#define SYS_ioperm              101
#define SYS_socketcall          102
#define SYS_klog                103
#define SYS_setitimer           104
#define SYS_getitimer           105
#define SYS_prev_stat           106
#define SYS_prev_lstat          107
#define SYS_prev_fstat          108
#define SYS_olduname            109
#define SYS_iopl                110
#define SYS_vhangup             111
#define SYS_idle                112
#define SYS_vm86                113
#define SYS_wait4               114
#define SYS_swapoff             115
#define SYS_sysinfo             116
#define SYS_ipc                 117
#define SYS_fsync               118
#define SYS_sigreturn           119
#define SYS_clone               120
#define SYS_setdomainname       121
#define SYS_uname               122
#define SYS_modify_ldt          123
#define SYS_adjtimex            124
#define SYS_mprotect            125
#define SYS_sigprocmask         126
#define SYS_create_module       127
#define SYS_init_module         128
#define SYS_delete_module       129
#define SYS_get_kernel_syms     130
#define SYS_quotactl            131
#define SYS_getpgid             132
#define SYS_fchdir              133
#define SYS_bdflush             134
#define SYS_sysfs               135
#define SYS_personality         136
#define SYS_afs_syscall         137 // Syscall for Andrew File System
#define SYS_setfsuid            138
#define SYS_setfsgid            139
#define SYS__llseek             140
#define SYS_getdents            141
#define SYS__newselect          142
#define SYS_flock               143
#define SYS_syscall_flock       143
#define SYS_msync               144
#define SYS_readv               145
#define SYS_syscall_readv       145
#define SYS_writev              146
#define SYS_syscall_writev      146
#define SYS_getsid              147
#define SYS_fdatasync           148
#define SYS__sysctl             149
#define SYS_mlock               150
#define SYS_munlock             151
#define SYS_mlockall            152
#define SYS_munlockall          153
#define SYS_sched_setparam      154
#define SYS_sched_getparam      155
#define SYS_sched_setscheduler  156
#define SYS_sched_getscheduler  157
#define SYS_sched_yield         158
#define SYS_sched_get_priority_max      159
#define SYS_sched_get_priority_min      160
#define SYS_sched_rr_get_interval       161
#define SYS_nanosleep           162
#define SYS_mremap              163

#endif  // <sys/syscall.h>


#ifdef __ELF__

#define NAME(X) cyg_hal_sys_##X 
#define END(X)  1: ; .type NAME(X),@function ; .size NAME(X),1b - NAME(X)

#else

#define NAME(X) _cyg_hal_sys_##X
#define END(X)

#endif

#define SYSCALL0(x)                             \
        .globl NAME(x) ;                        \
                                                \
NAME(x):                                        \
                                                \
        push %ebx;                              \
        lea     SYS_##x, %eax;                  \
        int $0x80;                              \
        pop %ebx;                               \
        ret;                                    \
                                                \
        END(x)

#define SYSCALL1(x)                             \
        .globl NAME(x) ;                        \
                                                \
NAME(x):                                        \
                                                \
        push %ebx;                              \
        mov 8(%esp), %ebx;                      \
        lea     SYS_##x, %eax;                  \
        int $0x80;                              \
        pop %ebx;                               \
        ret;                                    \
                                                \
    END(x)

#define SYSCALL2(x)                             \
        .globl NAME(x) ;                        \
                                                \
NAME(x):                                        \
                                                \
        push %ebx;                              \
        mov 8(%esp), %ebx;                      \
        mov 12(%esp), %ecx;                     \
        lea     SYS_##x, %eax;                  \
        int $0x80;                              \
        pop %ebx;                               \
        ret;                                    \
                                                \
    END(x)

#define STATCALL2(x)                            \
   .globl NAME(x) ;                             \
                                                \
NAME(x):                                        \
                                                \
        push %ebx;                              \
        mov 8(%esp), %ebx;                      \
        mov 12(%esp), %ecx;                     \
        lea SYS_prev_##x, %eax;                 \
        int $0x80;                              \
        pop %ebx;                               \
        ret;                                    \
        END(x)

#define SYSCALL3(x)                             \
        .globl NAME(x) ;                        \
                                                \
NAME(x):                                        \
                                                \
        push %ebx;                              \
        mov 8(%esp), %ebx;                      \
        mov 12(%esp), %ecx;                     \
        mov 16(%esp), %edx;                     \
        lea     SYS_##x, %eax;                  \
        int $0x80;                              \
        pop %ebx;                               \
        ret;                                    \
        END(x)


#define SYSCALL4(x)                             \
        .globl NAME(x) ;                        \
                                                \
NAME(x):                                        \
                                                \
        push %ebx;                              \
        push %esi;                              \
        mov 12(%esp), %ebx;                     \
        mov 16(%esp), %ecx;                     \
        mov 20(%esp), %edx;                     \
        mov 24(%esp), %esi;                     \
        lea     SYS_##x, %eax;                  \
        int $0x80;                              \
        pop %esi;                               \
        pop %ebx;                               \
        ret;                                    \
        END(x)

#define SYSCALL5(x)                             \
        .globl NAME(x) ;                        \
                                                \
NAME(x):                                        \
                                                \
        push %ebx;                              \
        push %esi;                              \
        push %edi;                              \
        mov 16(%esp), %ebx;                     \
        mov 20(%esp), %ecx;                     \
        mov 24(%esp), %edx;                     \
        mov 28(%esp), %esi;                     \
        mov 32(%esp), %edi;                     \
        lea     SYS_##x, %eax;                  \
        int $0x80;                              \
        pop %edi;                               \
        pop %esi;                               \
        pop %ebx;                               \
        ret;                                    \
        END(x)

//=========================================================================
//              exit             1              select           82
//              fork             2              socketcall      102
//              read             3              newselect       142
//              write            4              readv           145
//              open             5              writev          146
//              creat            8
//              link             9
//              unlink          10
//              execve          11
//              chdir           12
//              chmod           15
//              chown           16
//              lseek           19
//              rename          38
//              dup             41
//              pipe            42
//              ioctl           54
//              fcntl           55
//              dup2            63
//              readdir         89
//              ftruncate       93
//              fchmod          94
//              fchown          95
//

// Initial asm stuff for all functions.
        .text
        .align  2


//==========================================================================
// machdep_sys_gettimeofday()
SYSCALL3(gettimeofday)

//==========================================================================
// machdep_sys_setitimer()
SYSCALL3(sigaction)

//==========================================================================
// machdep_sys_setitimer()
SYSCALL3(setitimer)

//==========================================================================
// machdep_sys_getitimer()
SYSCALL2(getitimer)






//==========================================================================
// machdep_sys_read()
SYSCALL3(read)

//==========================================================================
// machdep_sys_write()
SYSCALL3(write)

//==========================================================================
// machdep_sys_open()
SYSCALL3(open)

//==========================================================================
// machdep_sys_close()
SYSCALL1(close)

//==========================================================================
// machdep_sys_creat()
SYSCALL2(creat)

//==========================================================================
// machdep_sys_dup2()
SYSCALL2(dup2)

//==========================================================================
// machdep_sys_fcntl()
SYSCALL3(fcntl)

//==========================================================================
// machdep_sys_fchown()
SYSCALL3(fchown)

//==========================================================================
// machdep_sys_fchmod()
SYSCALL2(fchmod)

//==========================================================================
// machdep_sys_ioctl()
SYSCALL3(ioctl)

//==========================================================================
// machdep_sys_chown()
SYSCALL3(chown)

//==========================================================================
// machdep_sys_chmod()
SYSCALL2(chmod)

//==========================================================================
// machdep_sys_chdir()
SYSCALL1(chdir)

//==========================================================================
// machdep_sys_unlink()
SYSCALL1(unlink)

//==========================================================================
// machdep_sys_link()
SYSCALL2(link)

//==========================================================================
// machdep_sys_rename()
SYSCALL2(rename)

//==========================================================================
// machdep_sys_lseek()
SYSCALL3(lseek)

//==========================================================================
// machdep_sys_pipe()
SYSCALL1(pipe)

//==========================================================================
// machdep_sys_dup()
SYSCALL1(dup)

//==========================================================================
// machdep_sys_fork()
SYSCALL0(fork)

//==========================================================================
// machdep_sys_execve()
SYSCALL3(execve)

//==========================================================================
// machdep_sys_stat()
#ifdef __ELF__
STATCALL2(stat)
#else
SYSCALL2(stat)
#endif

//==========================================================================
// machdep_sys_fstat()
#ifdef __ELF__
STATCALL2(fstat)
#else
SYSCALL2(fstat)
#endif

//==========================================================================
// machdep_sys_fstat()
#ifdef __ELF__
STATCALL2(lstat)
#else
SYSCALL2(lstat)
#endif

//==========================================================================
// machdep_sys_ftruncate()
SYSCALL2(ftruncate)

//==========================================================================
// Nonstandard calls used to make the system work
//
// ==========================================================================
// machdep_sys_select()

        .globl NAME(select)

NAME(select):

        push %ebx
        lea 8(%esp), %ebx
        lea     SYS_select, %eax
        int $0x80
        pop %ebx
        ret
        END(select)

//==========================================================================
// machdep_sys_wait4()
SYSCALL4(wait4)

//==========================================================================
// machdep_sys_readdir()
SYSCALL3(readdir)

//==========================================================================
// machdep_sys_socketcall()
SYSCALL2(socketcall)


SYSCALL1(exit)
SYSCALL3(sigprocmask)

//==========================================================================
// machdep_sys_sigsuspend()

        .globl NAME(sigsuspend)

NAME(sigsuspend):

    push %ebp
        mov %esp,%ebp
        push %edi
        push %esi
        push %ebx
        mov 8(%ebp), %edx
        mov (%edx), %edx
        lea     SYS_sigsuspend, %eax
        int $0x80
        pop %ebx
        pop %esi
        pop %edi
        mov %ebp,%esp
    pop %ebp
        ret
        END(sigsuspend)

//==========================================================================
// machdep_sys_readv()
#ifdef HAVE_SYSCALL_READV
SYSCALL3(readv)
#endif

//==========================================================================
// machdep_sys_writev()
#ifdef HAVE_SYSCALL_WRITEV 
SYSCALL3(writev)
#endif

//==========================================================================
// machdep_sys_newselect()
SYSCALL5(_newselect)
