#ifndef CYGONCE_HAL_HAL_IF_H
#define CYGONCE_HAL_HAL_IF_H

//=============================================================================
//
//      hal_if.h
//
//      HAL header for ROM/RAM calling interface.
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
// Author(s):   jskov
// Contributors:jskov
// Date:        2000-06-07
// Purpose:     HAL RAM/ROM calling interface
// Description: ROM/RAM calling interface table	definitions. The layout is a
//              combination of libbsp and vectors already in use by some
//              eCos platforms.
// Usage:       #include <cyg/hal/hal_if.h>
//                           
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>         // types & externC
#include <cyg/hal/dbg-threads-api.h>
#include <cyg/hal/dbg-thread-syscall.h>


//--------------------------------------------------------------------------
typedef int (*bsp_handler_t)(int __irq_nr, void *__regs);

//--------------------------------------------------------------------------
// Communication interface table. CYGNUM_CALL_IF_CONSOLE_PROCS and
// CYGNUM_CALL_IF_DEBUG_PROCS point to instances (possibly the same)
// of this table.

typedef enum {
    /*
     * For serial ports, the control function may be used to set and get the
     * current baud rate. Usage:
     * 
     *   err = (*__control)(COMMCTL_SETBAUD, int bits_per_second);
     *     err => Zero if successful, -1 if error.
     *
     *   baud = (*__control)(COMMCTL_GETBAUD);
     *     baud => -1 if error, current baud otherwise.
     */
    __COMMCTL_SETBAUD=0,
    __COMMCTL_GETBAUD,

    /*
     * Install and remove debugger interrupt handlers. These are the receiver
     * interrupt routines which are used to change control from a running
     * program to the debugger stub.
     */
    __COMMCTL_INSTALL_DBG_ISR,
    __COMMCTL_REMOVE_DBG_ISR,

    /*
     * Disable comm port interrupt. Returns TRUE if interrupt was enabled,
     * FALSE otherwise.
     */
    __COMMCTL_IRQ_DISABLE,

    /*
     * Enable comm port interrupt.
     */
    __COMMCTL_IRQ_ENABLE,
} __comm_control_cmd_t;


#define CYGNUM_COMM_IF_CH_DATA                    0
#define CYGNUM_COMM_IF_WRITE                      1
#define CYGNUM_COMM_IF_READ                       2
#define CYGNUM_COMM_IF_PUTC                       3
#define CYGNUM_COMM_IF_GETC                       4
#define CYGNUM_COMM_IF_CONTROL                    5

#define CYGNUM_COMM_IF_TABLE_SIZE                 8

typedef CYG_ADDRWORD hal_virtual_comm_table_t[CYGNUM_COMM_IF_TABLE_SIZE];

// The below is a (messy) attempt at adding some type safety to the
// above array. At the same time, the accessors allow the
// implementation to be easily changed in the future (both tag->data
// table and structure implementations have been suggested).

typedef void* __comm_if_ch_data_t;
#define CYGACC_COMM_IF_CH_DATA(_t_) \
 ((__comm_if_ch_data_t)(((_t_))[CYGNUM_COMM_IF_CH_DATA]))
#define CYGACC_COMM_IF_CH_DATA_SET(_t_, _x_) \
 (_t_)[CYGNUM_COMM_IF_CH_DATA]=(CYG_ADDRWORD)(_x_)

typedef void (*__comm_if_write_t)(void* __ch_data, const cyg_uint8* __buf,
                                  cyg_uint32 __len);
#define CYGACC_COMM_IF_WRITE(_t_) \
 ((__comm_if_write_t)(((_t_))[CYGNUM_COMM_IF_WRITE]))
#define CYGACC_COMM_IF_WRITE_SET(_t_, _x_) \
 (_t_)[CYGNUM_COMM_IF_WRITE]=(CYG_ADDRWORD)(_x_)

typedef int (*__comm_if_read_t)(void* __ch_data, cyg_uint8* __buf,
                                cyg_uint32 __len);
#define CYGACC_COMM_IF_READ(_t_) \
 ((__comm_if_read_t)(((_t_))[CYGNUM_COMM_IF_READ]))
#define CYGACC_COMM_IF_READ_SET(_t_, _x_) \
 (_t_)[CYGNUM_COMM_IF_READ]=(CYG_ADDRWORD)(_x_)

typedef void (*__comm_if_putc_t)(void* __ch_data, cyg_uint8 __ch);
#define CYGACC_COMM_IF_PUTC(_t_) \
 ((__comm_if_putc_t)(((_t_))[CYGNUM_COMM_IF_PUTC]))
#define CYGACC_COMM_IF_PUTC_SET(_t_, _x_) \
 (_t_)[CYGNUM_COMM_IF_PUTC]=(CYG_ADDRWORD)(_x_)

typedef cyg_uint8 (*__comm_if_getc_t)(void* __ch_data);
#define CYGACC_COMM_IF_GETC(_t_) \
 ((__comm_if_getc_t)(((_t_))[CYGNUM_COMM_IF_GETC]))
#define CYGACC_COMM_IF_GETC_SET(_t_, _x_) \
 (_t_)[CYGNUM_COMM_IF_GETC]=(CYG_ADDRWORD)(_x_)

typedef int (*__comm_if_control_t)(void *__ch_data, 
                                   __comm_control_cmd_t __func, ...);
#define CYGACC_COMM_IF_CONTROL(_t_) \
 ((__comm_if_control_t)(((_t_))[CYGNUM_COMM_IF_CONTROL]))
#define CYGACC_COMM_IF_CONTROL_SET(_t_, _x_) \
 (_t_)[CYGNUM_COMM_IF_CONTROL]=(CYG_ADDRWORD)(_x_)


//--------------------------------------------------------------------------
// Main calling interface table. Will be assigned a location by the 
// linker script. Both ROM and RAM startup applications will know about
// the location.
#define CYGNUM_CALL_IF_VERSION                    0
#define CYGNUM_CALL_IF_ICTRL_TABLE                1
#define CYGNUM_CALL_IF_EXC_TABLE                  2
#define CYGNUM_CALL_IF_DBG_VECTOR                 3
#define CYGNUM_CALL_IF_KILL_VECTOR                4
#define CYGNUM_CALL_IF_CONSOLE_PROCS              5
#define CYGNUM_CALL_IF_DEBUG_PROCS                6
#define CYGNUM_CALL_IF_FLUSH_DCACHE               7
#define CYGNUM_CALL_IF_FLUSH_ICACHE               8
#define CYGNUM_CALL_IF_CPU_DATA                   9
#define CYGNUM_CALL_IF_BOARD_DATA                 10
#define CYGNUM_CALL_IF_SYSINFO                    11
#define CYGNUM_CALL_IF_SET_DEBUG_COMM             12
#define CYGNUM_CALL_IF_SET_CONSOLE_COMM           13
#define CYGNUM_CALL_IF_SET_SERIAL_BAUD            14
#define CYGNUM_CALL_IF_DBG_DATA                   15
#define CYGNUM_CALL_IF_DBG_SYSCALL                15 // conflict!
#define CYGNUM_CALL_IF_RESET                      16
#define CYGNUM_CALL_IF_CONSOLE_INTERRUPT_FLAG     17

#define CYGNUM_CALL_IF_LAST_ENTRY                 CYGNUM_CALL_IF_CONSOLE_INTERRUPT_FLAG

#define CYGNUM_CALL_IF_INSTALL_BPT_FN             35

#define CYGNUM_CALL_IF_TABLE_SIZE                 64

externC CYG_ADDRWORD hal_virtual_vector_table[CYGNUM_CALL_IF_TABLE_SIZE];

// Table version is simply the number of the last active entry in the table
// (except INSTALL_BPT_FN since it's so high).
#define CYGNUM_CALL_IF_TABLE_VERSION              CYGNUM_CALL_IF_LAST_ENTRY

// The below is a (messy) attempt at adding some type safety to the
// above array. At the same time, the accessors allow the
// implementation to be easily changed in the future (both tag->data
// table and structure implementations have been suggested).

typedef int __call_if_version_t;
#define CYGACC_CALL_IF_VERSION() \
 ((__call_if_version_t)hal_virtual_vector_table[CYGNUM_CALL_IF_VERSION])
#define CYGACC_CALL_IF_VERSION_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_VERSION]=(CYG_ADDRWORD)(_x_)

typedef void* __call_if_ictrl_table_t;
#define CYGACC_CALL_IF_ICTRL_TABLE() \
 ((__call_if_ictrl_table_t)hal_virtual_vector_table[CYGNUM_CALL_IF_ICTRL_TABLE])
#define CYGACC_CALL_IF_ICTRL_TABLE_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_ICTRL_TABLE]=(CYG_ADDRWORD)(_x_)

typedef void* __call_if_exc_table_t;
#define CYGACC_CALL_IF_EXC_TABLE() \
 ((__call_if_exc_table_t)hal_virtual_vector_table[CYGNUM_CALL_IF_EXC_TABLE])
#define CYGACC_CALL_IF_EXC_TABLE_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_EXC_TABLE]=(CYG_ADDRWORD)(_x_)

typedef bsp_handler_t *__call_if_dbg_vector_t;
#define CYGACC_CALL_IF_DBG_VECTOR() \
 ((__call_if_dbg_vector_t)hal_virtual_vector_table[CYGNUM_CALL_IF_DBG_VECTOR])
#define CYGACC_CALL_IF_DBG_VECTOR_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_DBG_VECTOR]=(CYG_ADDRWORD)(_x_)

typedef bsp_handler_t __call_if_kill_vector_t;
#define CYGACC_CALL_IF_KILL_VECTOR() \
 ((__call_if_kill_vector_t)hal_virtual_vector_table[CYGNUM_CALL_IF_KILL_VECTOR])
#define CYGACC_CALL_IF_KILL_VECTOR_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_KILL_VECTOR]=(CYG_ADDRWORD)(_x_)

typedef hal_virtual_comm_table_t *__call_if_console_procs_t;
#define CYGACC_CALL_IF_CONSOLE_PROCS() \
 ((__call_if_console_procs_t)hal_virtual_vector_table[CYGNUM_CALL_IF_CONSOLE_PROCS])
#define CYGACC_CALL_IF_CONSOLE_PROCS_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_CONSOLE_PROCS]=(CYG_ADDRWORD)(_x_)

typedef hal_virtual_comm_table_t *__call_if_debug_procs_t;
#define CYGACC_CALL_IF_DEBUG_PROCS() \
 ((__call_if_debug_procs_t)hal_virtual_vector_table[CYGNUM_CALL_IF_DEBUG_PROCS])
#define CYGACC_CALL_IF_DEBUG_PROCS_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_DEBUG_PROCS]=(CYG_ADDRWORD)(_x_)

typedef void (*__call_if_flush_dcache_t)(void *__p, int __nbytes);
#define CYGACC_CALL_IF_FLUSH_DCACHE() \
 ((__call_if_flush_dcache_t)hal_virtual_vector_table[CYGNUM_CALL_IF_FLUSH_DCACHE])
#define CYGACC_CALL_IF_FLUSH_DCACHE_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_FLUSH_DCACHE]=(CYG_ADDRWORD)(_x_)

typedef void (*__call_if_flush_icache_t)(void *__p, int __nbytes);
#define CYGACC_CALL_IF_FLUSH_ICACHE() \
 ((__call_if_flush_icache_t)hal_virtual_vector_table[CYGNUM_CALL_IF_FLUSH_ICACHE])
#define CYGACC_CALL_IF_FLUSH_ICACHE_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_FLUSH_ICACHE]=(CYG_ADDRWORD)(_x_)

typedef void* __call_if_cpu_data_t;
#define CYGACC_CALL_IF_CPU_DATA() \
 ((__call_if_cpu_data_t)hal_virtual_vector_table[CYGNUM_CALL_IF_CPU_DATA])
#define CYGACC_CALL_IF_CPU_DATA_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_CPU_DATA]=(CYG_ADDRWORD)(_x_)

typedef void* __call_if_board_data_t;
#define CYGACC_CALL_IF_BOARD_DATA() \
 ((__call_if_board_data_t)hal_virtual_vector_table[CYGNUM_CALL_IF_BOARD_DATA])
#define CYGACC_CALL_IF_BOARD_DATA_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_BOARD_DATA]=(CYG_ADDRWORD)(_x_)

typedef int (*__call_if_sysinfo_t)(int __id, void* __ap);
//typedef int (*__call_if_sysinfo_t)(enum bsp_info_id __id, va_list __ap);
#define CYGACC_CALL_IF_SYSINFO() \
 ((__call_if_sysinfo_t)hal_virtual_vector_table[CYGNUM_CALL_IF_SYSINFO])
#define CYGACC_CALL_IF_SYSINFO_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_SYSINFO]=(CYG_ADDRWORD)(_x_)

typedef int (*__call_if_set_debug_comm_t)(int __comm_id);
#define CYGACC_CALL_IF_SET_DEBUG_COMM() \
 ((__call_if_set_debug_comm_t)hal_virtual_vector_table[CYGNUM_CALL_IF_SET_DEBUG_COMM])
#define CYGACC_CALL_IF_SET_DEBUG_COMM_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_SET_DEBUG_COMM]=(CYG_ADDRWORD)(_x_)

typedef int (*__call_if_set_console_comm_t)(int __comm_id);
#define CYGACC_CALL_IF_SET_CONSOLE_COMM() \
 ((__call_if_set_console_comm_t)hal_virtual_vector_table[CYGNUM_CALL_IF_SET_CONSOLE_COMM])
#define CYGACC_CALL_IF_SET_CONSOLE_COMM_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_SET_CONSOLE_COMM]=(CYG_ADDRWORD)(_x_)

typedef int (*__call_if_set_serial_baud_t)(int __comm_id, int __baud);
#define CYGACC_CALL_IF_SET_SERIAL_BAUD() \
 ((__call_if_set_serial_baud_t)hal_virtual_vector_table[CYGNUM_CALL_IF_SET_SERIAL_BAUD])
#define CYGACC_CALL_IF_SET_SERIAL_BAUD_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_SET_SERIAL_BAUD]=(CYG_ADDRWORD)(_x_)

typedef void* __call_if_dbg_data_t;
#define CYGACC_CALL_IF_DBG_DATA() \
 ((__call_if_dbg_data_t)hal_virtual_vector_table[CYGNUM_CALL_IF_DBG_DATA])
#define CYGACC_CALL_IF_DBG_DATA_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_DBG_DATA]=(CYG_ADDRWORD)(_x_)

typedef int (*__call_if_dbg_syscall_t) (enum dbg_syscall_ids id,
                                        union dbg_thread_syscall_parms  *p );
#define CYGACC_CALL_IF_DBG_SYSCALL() \
 ((__call_if_dbg_syscall_t)hal_virtual_vector_table[CYGNUM_CALL_IF_DBG_SYSCALL])
#define CYGACC_CALL_IF_DBG_SYSCALL_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_DBG_SYSCALL]=(CYG_ADDRWORD)(_x_)

typedef void (*__call_if_reset_t)(void);
#define CYGACC_CALL_IF_RESET() \
 ((__call_if_reset_t)hal_virtual_vector_table[CYGNUM_CALL_IF_RESET])
#define CYGACC_CALL_IF_RESET_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_RESET]=(CYG_ADDRWORD)(_x_)

typedef int __call_if_console_interrupt_flag_t;
#define CYGACC_CALL_IF_CONSOLE_INTERRUPT_FLAG() \
 ((__call_if_console_interrupt_flag_t)hal_virtual_vector_table[CYGNUM_CALL_IF_CONSOLE_INTERRUPT_FLAG])
#define CYGACC_CALL_IF_CONSOLE_INTERRUPT_FLAG_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_CONSOLE_INTERRUPT_FLAG]=(CYG_ADDRWORD)(_x_)

typedef void (*__call_if_install_bpt_fn_t)(void *__epc);
#define CYGACC_CALL_IF_INSTALL_BPT_FN() \
 ((__call_if_install_bpt_fn_t)hal_virtual_vector_table[CYGNUM_CALL_IF_INSTALL_BPT_FN])
#define CYGACC_CALL_IF_INSTALL_BPT_FN_SET(_x_) \
 hal_virtual_vector_table[CYGNUM_CALL_IF_INSTALL_BPT_FN]=(CYG_ADDRWORD)(_x_)


//--------------------------------------------------------------------------
// Functions provided by the HAL interface.
externC void hal_if_init(void);


//--------------------------------------------------------------------------
// Diag wrappers.
externC void hal_if_diag_init(void);
externC void hal_if_diag_write_char(char c);
externC void hal_if_diag_read_char(char *c);

//--------------------------------------------------------------------------
// Determining if console code needs to be included, or if it will be
// provided by via the vector table:
//
// The console channel is already handled if (a) stubs are included and the
// console and debug channels are the same, or (b) the console channel is
// the same as the ROM monitor debug channel.
#if (defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS) \
     && (CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL == CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL)) \
    || (defined(CYGSEM_HAL_USE_ROM_MONITOR_GDB_stubs) \
        && (CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL == CYGNUM_HAL_VIRTUAL_VECTOR_ROM_DEBUG_CHANNEL))
# define CYGPRI_CONSOLE_PROCS_HANDLED
#endif

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_HAL_IF_H
// End of hal_if.h
