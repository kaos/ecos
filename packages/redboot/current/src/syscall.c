/*==========================================================================
//
//      syscall.c
//
//      Redboot syscall handling for GNUPro bsp support
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
// Copyright (C) 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    msalter
// Contributors: msalter
// Date:         1999-02-20
// Purpose:      Temporary support for gnupro bsp
//
//####DESCRIPTIONEND####
//
//=========================================================================*/

#include <redboot.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>

#ifdef CYGSEM_REDBOOT_BSP_SYSCALLS

#define EIO 5           /* I/O error */

// These are required by the ANSI C part of newlib (excluding system() of
// course).
#define SYS_exit        1
#define SYS_open        2
#define SYS_close       3
#define SYS_read        4
#define SYS_write       5
#define SYS_lseek       6
#define SYS_unlink      7
#define SYS_getpid      8
#define SYS_kill        9
#define SYS_fstat       10
//#define SYS_sbrk      11 - not currently a system call, but reserved.

// ARGV support.
#define SYS_argvlen     12
#define SYS_argv        13

// These are extras added for one reason or another.
#define SYS_chdir       14
#define SYS_stat        15
#define SYS_chmod       16
#define SYS_utime       17
#define SYS_time        18

#define SYS_interrupt   1000
#define SYS_meminfo     1001

#define __GET_SHARED  0xbaad

/*
 * Clients of this BSP will need to have access to BSP functions and
 * data structures. Because, the client and the BSP may not be linked
 * together, a structure of vectors is used to gain this access. A
 * pointer to this structure can be gotten via a syscall. This syscall
 * is made automatically from within the crt0.o file.
 */
typedef struct {
    int         version;        /* version number for future expansion */
    const void **__ictrl_table;
    void **__exc_table;
    void *__dbg_vector;
    void *__kill_vector;
    void *__console_procs;
    void *__debug_procs;
    void (*__flush_dcache)(void *__p, int __nbytes);
    void (*__flush_icache)(void *__p, int __nbytes);
    void *__cpu_data;
    void *__board_data;
    void *__sysinfo;
    int  (*__set_debug_comm)(int __comm_id);
    int  (*__set_console_comm)(int __comm_id);
    int  (*__set_serial_baud)(int __comm_id, int baud);
    void *__dbg_data;
    void (*__reset)(void);
    int  __console_interrupt_flag;
} __shared_t;

static __shared_t __shared_data = { 2 };

static inline char __getc(void)
{
    char c;
    hal_virtual_comm_table_t* __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
    
    if (__chan)
        c = CYGACC_COMM_IF_GETC(*__chan);
    else {
        __chan = CYGACC_CALL_IF_DEBUG_PROCS();
        c = CYGACC_COMM_IF_GETC(*__chan);
    }
    return c;
}

static inline void __putc(char c)
{
    hal_virtual_comm_table_t* __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
    if (__chan)
        CYGACC_COMM_IF_PUTC(*__chan, c);
    else {
        __chan = CYGACC_CALL_IF_DEBUG_PROCS();
        CYGACC_COMM_IF_PUTC(*__chan, c);
    }
}


// Timer support

static cyg_handle_t  sys_timer_handle;
static cyg_interrupt sys_timer_interrupt;
static cyg_uint32    sys_timer_ticks;

static void
sys_timer_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    // do nothing
}


static cyg_uint32
sys_timer_isr(cyg_vector_t vector, cyg_addrword_t data)
{
    ++sys_timer_ticks;

    HAL_CLOCK_RESET(CYGNUM_HAL_INTERRUPT_RTC, CYGNUM_HAL_RTC_PERIOD);

    cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_RTC);

    return 0;
}


static void sys_timer_init(void)
{
    HAL_CLOCK_INITIALIZE(CYGNUM_HAL_RTC_PERIOD);
    
    cyg_drv_interrupt_create(
        CYGNUM_HAL_INTERRUPT_RTC,
        0,                      // Priority - unused
        (CYG_ADDRWORD)0,        // Data item passed to ISR & DSR
        sys_timer_isr,          // ISR
        sys_timer_dsr,          // DSR
        &sys_timer_handle,      // handle to intr obj
        &sys_timer_interrupt ); // space for int obj

    cyg_drv_interrupt_attach(sys_timer_handle);

    cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_RTC);
}


//
// read  -- read bytes from the serial port. Ignore fd, since
//          we only have stdin.
static int
sys_read(int fd, char *buf, int nbytes)
{
    int i = 0;

    for (i = 0; i < nbytes; i++) {
        *(buf + i) = __getc();
        if ((*(buf + i) == '\n') || (*(buf + i) == '\r')) {
            (*(buf + i + 1)) = 0;
            break;
        }
    }
    return (i);
}


//
// write -- write bytes to the serial port. Ignore fd, since
//          stdout and stderr are the same. Since we have no filesystem,
//          open will only return an error.
//
static int
sys_write(int fd, char *buf, int nbytes)
{
#define WBUFSIZE  256
    int  tosend;

    tosend = nbytes;

    while (tosend > 0) {
        if (*buf == '\n')
            __putc('\r');
        __putc(*buf++);
        tosend--;
    }

    return (nbytes);
}


//
// open -- open a file descriptor. We don't have a filesystem, so
//         we return an error.
//
static int
sys_open (const char *buf, int flags, int mode)
{
    return (-EIO);
}


//
// close -- We don't need to do anything, but pretend we did.
//
static int
sys_close(int fd)
{
    return (0);
}


//
// lseek --  Since a serial port is non-seekable, we return an error.
//
static int
sys_lseek(int fd,  int offset, int whence)
{
    return (-EIO);
}

#define NS_PER_TICK    (CYGNUM_HAL_RTC_NUMERATOR/CYGNUM_HAL_RTC_DENOMINATOR)
#define TICKS_PER_SEC  (1000000000L / NS_PER_TICK)

// This needs to match newlib HZ which is normally 60.
#define HZ 60

static int
sys_utime(unsigned long *p)
{
    static int inited = 0;

    if (!inited) {
        inited = 1;
        sys_timer_init();
    }

    /* target clock runs at CLOCKS_PER_SEC. Convert to HZ */
    if (p)
        *p = (sys_timer_ticks * HZ) / TICKS_PER_SEC;

    return 0;
}

//
//  Generic syscall handler.
//
//  Returns 0 if syscall number is not handled by this
//  module, 1 otherwise. This allows applications to
//  extend the syscall handler by using exception chaining.
//
CYG_ADDRWORD
__do_syscall(CYG_ADDRWORD func,                 // syscall function number
             CYG_ADDRWORD arg1, CYG_ADDRWORD arg2,      // up to four args.
             CYG_ADDRWORD arg3, CYG_ADDRWORD arg4,
             CYG_ADDRWORD *retval)              // syscall return value
{
    int err = 0;

    switch (func) {

      case SYS_read:
        err = sys_read((int)arg1, (char *)arg2, (int)arg3);
        break;

      case SYS_write:
        err = sys_write((int)arg1, (char *)arg2, (int)arg3);
        break;

      case SYS_open:
        err = sys_open((const char *)arg1, (int)arg2, (int)arg3);
        break;

      case SYS_close:
        err = sys_close((int)arg1);
        break;

      case SYS_lseek:
        err = sys_lseek((int)arg1, (int)arg2, (int)arg3);
        break;

      case SYS_utime:
        err = sys_utime((unsigned long *)arg1);
        break;

      case SYS_meminfo:
        err = 1;
        *(unsigned long *)arg1 = (unsigned long)(ram_end-ram_start);
        *(unsigned long *)arg2 = (unsigned long)ram_end;
        break;
        
      case __GET_SHARED:
        *(__shared_t **)arg1 = &__shared_data;
        break;

      default:
        return 0;
    }    

    *retval = err;
    return 1;
}

#endif
