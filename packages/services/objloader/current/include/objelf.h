#ifndef __OBJELF_H__
#define __OBJELF_H__

/* =================================================================
 *
 *      objelf.h
 *
 * ================================================================= 
 * ####ECOSGPLCOPYRIGHTBEGIN####
 * -------------------------------------------
 * This file is part of eCos, the Embedded Configurable Operating
 * System.
 * Copyright (C) 2005 eCosCentric Ltd.
 * 
 * eCos is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 or (at your option)
 * any later version.
 * 
 * eCos is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with eCos; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or
 * use macros or inline functions from this file, or you compile this
 * file and link it with other works to produce a work based on this
 * file, this file does not by itself cause the resulting work to be
 * covered by the GNU General Public License. However the source code
 * for this file must still be made available in accordance with
 * section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work
 * based on this file might be covered by the GNU General Public
 * License.
 *
 * -------------------------------------------
 * ####ECOSGPLCOPYRIGHTEND####
 * =================================================================
 * #####DESCRIPTIONBEGIN####
 * 
 *  Author(s):    Anthony Tonizzo (atonizzo@gmail.com)
 *  Contributors: nickg@ecoscentric.com
 *  Date:         2005-05-13
 *  Purpose:      
 *  Description:  
 *               
 * ####DESCRIPTIONEND####
 * 
 * =================================================================
 */

//==============================================================================

#include <pkgconf/system.h>
#include <pkgconf/objloader.h>
#include <cyg/hal/hal_tables.h>

//==============================================================================

extern char *cyg_ldr_last_error;

//==============================================================================

#define CYG_LDR_MODE_FILESYSTEM              0
#define CYG_LDR_MODE_FTP                     1
#define CYG_LDR_MODE_TFTP                    2
#define CYG_LDR_MODE_MEMORY                  3
#define CYG_LDR_MODE_HTTP                    4

//==============================================================================

typedef struct ELF_OBJECT
{
    CYG_ADDRWORD  ptr;
    cyg_uint32    mode;

    size_t    (*read)(struct ELF_OBJECT*, size_t, size_t, void*);
    cyg_int32 (*seek)(struct ELF_OBJECT*, cyg_uint32);
    cyg_int32 (*close)(struct ELF_OBJECT*);

    // This is the absolute address in memory where the library resides.
    Elf32_Ehdr*   p_elfhdr;

    // Start of the section header.
    Elf32_Shdr*   p_sechdr;

    cyg_int32     hdrndx_symtab;
    cyg_int32     hdrndx_strtab;

    cyg_uint32    **sections;

} ELF_OBJECT, *PELF_OBJECT;


//==============================================================================
// Debug functions.

#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL != 0
#define ELFDEBUG(a) diag_printf(a)
#else
#define ELFDEBUG(a)
#endif

#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 0
void       cyg_ldr_print_section_data(PELF_OBJECT);
void       cyg_ldr_print_symbol_names(PELF_OBJECT);
void       cyg_ldr_print_rel_names(PELF_OBJECT);
#endif

//==============================================================================
// Internal functions

cyg_uint32 *cyg_ldr_load_elf_section(PELF_OBJECT, cyg_uint32);
void        cyg_ldr_delete_elf_section(PELF_OBJECT, cyg_uint32);
cyg_uint32 *cyg_ldr_section_address(PELF_OBJECT, cyg_uint32);
void*       cyg_ldr_scan_file(PELF_OBJECT);
cyg_int32   cyg_ldr_relocate_section(PELF_OBJECT, cyg_uint32);
void*       cyg_ldr_find_symbol(void*, char*);
void*       cyg_ldr_symbol_address(PELF_OBJECT, cyg_uint32);
void*       cyg_ldr_external_address(PELF_OBJECT, cyg_uint32);

//==============================================================================
// User interface.

PELF_OBJECT cyg_ldr_open_library(CYG_ADDRWORD, cyg_int32);
void        cyg_ldr_close_library(void*);
char*       cyg_ldr_error(void);
void*       cyg_ldr_get_symbol(void*, char*);

//==============================================================================

void *cyg_ldr_malloc(size_t) CYGBLD_ATTRIB_WEAK;
void cyg_ldr_free(void *) CYGBLD_ATTRIB_WEAK;

//==============================================================================

struct cyg_ldr_table_entry
{
    char   *symbol_name;
    void   *handler;
} CYG_HAL_TABLE_TYPE;

typedef struct cyg_ldr_table_entry cyg_ldr_table_entry;

#define CYG_LDR_TABLE_ENTRY(__name, __symbol_name, __handler)                \
                cyg_ldr_table_entry __name CYG_HAL_TABLE_ENTRY(ldr_table) =  \
                                                 { __symbol_name, __handler } 

//==============================================================================

#define CYG_LDR_TABLE_KAPI_ALARM()                                            \
CYG_LDR_TABLE_ENTRY(cyg_alarm_create_entry,                                   \
                     "cyg_alarm_create", cyg_alarm_create);                   \
CYG_LDR_TABLE_ENTRY(cyg_alarm_delete_entry,                                   \
                     "cyg_alarm_delete", cyg_alarm_delete);                   \
CYG_LDR_TABLE_ENTRY(cyg_alarm_initialize_entry,                               \
                     "cyg_alarm_initialize", cyg_alarm_initialize);           \
CYG_LDR_TABLE_ENTRY(cyg_alarm_get_times_entry,                                \
                     "cyg_alarm_get_times", cyg_alarm_get_times);             \
CYG_LDR_TABLE_ENTRY(cyg_alarm_enable_entry,                                   \
                     "cyg_alarm_enable", cyg_alarm_enable);                   \
CYG_LDR_TABLE_ENTRY(cyg_alarm_disable_entry,                                  \
                     "cyg_alarm_disable", cyg_alarm_disable);

#define CYG_LDR_TABLE_KAPI_CLOCK()                                            \
CYG_LDR_TABLE_ENTRY(cyg_clock_create_entry,                                   \
                     "cyg_clock_create", cyg_clock_create);                   \
CYG_LDR_TABLE_ENTRY(cyg_clock_delete_entry,                                   \
                     "cyg_clock_delete", cyg_clock_delete);                   \
CYG_LDR_TABLE_ENTRY(cyg_clock_to_counter_entry,                               \
                     "cyg_clock_to_counter", cyg_clock_to_counter);           \
CYG_LDR_TABLE_ENTRY(cyg_clock_set_resolution_entry,                           \
                     "cyg_clock_set_resolution", cyg_clock_set_resolution);   \
CYG_LDR_TABLE_ENTRY(cyg_clock_get_resolution_entry,                           \
                     "cyg_clock_get_resolution",cyg_clock_get_resolution);    \
CYG_LDR_TABLE_ENTRY(cyg_real_time_clock_entry,                                \
                     "cyg_real_time_clock", cyg_real_time_clock);             \
CYG_LDR_TABLE_ENTRY(cyg_current_time_entry,                                   \
                     "cyg_current_time", cyg_current_time);

#define CYG_LDR_TABLE_KAPI_COND()                                             \
CYG_LDR_TABLE_ENTRY(cyg_cond_init_entry,                                      \
                     "cyg_cond_init", cyg_cond_init);                         \
CYG_LDR_TABLE_ENTRY(cyg_cond_destroy_entry,                                   \
                     "cyg_cond_destroy", cyg_cond_destroy);                   \
CYG_LDR_TABLE_ENTRY(cyg_cond_wait_entry,                                      \
                     "cyg_cond_wait", cyg_cond_wait);                         \
CYG_LDR_TABLE_ENTRY(cyg_cond_signal_entry,                                    \
                     "cyg_cond_signal", cyg_cond_signal);                     \
CYG_LDR_TABLE_ENTRY(cyg_cond_broadcast_entry,                                 \
                     "cyg_cond_broadcast", cyg_cond_broadcast);               \
CYG_LDR_TABLE_ENTRY(cyg_cond_timed_wait_entry,                                \
                     "cyg_cond_timed_wait", cyg_cond_timed_wait);

#define CYG_LDR_TABLE_KAPI_COUNTER()                                          \
CYG_LDR_TABLE_ENTRY(cyg_counter_create_entry,                                 \
                     "cyg_counter_create", cyg_counter_create);               \
CYG_LDR_TABLE_ENTRY(cyg_counter_delete_entry,                                 \
                     "cyg_counter_delete", cyg_counter_delete);               \
CYG_LDR_TABLE_ENTRY(cyg_counter_current_value_entry,                          \
                     "cyg_counter_current_value", cyg_counter_current_value); \
CYG_LDR_TABLE_ENTRY(cyg_counter_set_value_entry,                              \
                     "cyg_counter_set_value", cyg_counter_set_value);         \
CYG_LDR_TABLE_ENTRY(cyg_counter_tick_entry,                                   \
                     "cyg_counter_tick", cyg_counter_tick);                   \
CYG_LDR_TABLE_ENTRY(cyg_counter_multi_tick_entry,                             \
                     "cyg_counter_multi_tick", cyg_counter_multi_tick);

#define CYG_LDR_TABLE_KAPI_EXCEPTIONS()                                       \
CYG_LDR_TABLE_ENTRY(cyg_exception_set_handler_entry,                          \
                     "cyg_exception_set_handler", cyg_exception_set_handler); \
CYG_LDR_TABLE_ENTRY(cyg_exception_clear_handler_entry,                        \
                     "cyg_exception_clear_handler",                           \
                     cyg_exception_clear_handler);                            \
CYG_LDR_TABLE_ENTRY(cyg_exception_call_handler_entry,                         \
                     "cyg_exception_call_handler",                            \
                     cyg_exception_call_handler);

#define CYG_LDR_TABLE_KAPI_FLAG()                                             \
CYG_LDR_TABLE_ENTRY(cyg_flag_init_entry,                                      \
                     "cyg_flag_init", cyg_flag_init);                         \
CYG_LDR_TABLE_ENTRY(cyg_flag_destroy_entry,                                   \
                     "cyg_flag_destroy", cyg_flag_destroy);                   \
CYG_LDR_TABLE_ENTRY(cyg_flag_setbits_entry,                                   \
                     "cyg_flag_setbits", cyg_flag_setbits);                   \
CYG_LDR_TABLE_ENTRY(cyg_flag_maskbits_entry,                                  \
                     "cyg_flag_maskbits", cyg_flag_maskbits);                 \
CYG_LDR_TABLE_ENTRY(cyg_flag_wait_entry,                                      \
                     "cyg_flag_wait", cyg_flag_wait);                         \
CYG_LDR_TABLE_ENTRY(cyg_flag_timed_wait_entry,                                \
                     "cyg_flag_timed_wait", cyg_flag_timed_wait);             \
CYG_LDR_TABLE_ENTRY(cyg_flag_poll_entry,                                      \
                     "cyg_flag_poll", cyg_flag_poll);                         \
CYG_LDR_TABLE_ENTRY(cyg_flag_peek_entry,                                      \
                     "cyg_flag_peek", cyg_flag_peek);                         \
CYG_LDR_TABLE_ENTRY(cyg_flag_waiting_entry,                                   \
                     "cyg_flag_waiting", cyg_flag_waiting);                   

#define CYG_LDR_TABLE_KAPI_INTERRUPTS()                                       \
CYG_LDR_TABLE_ENTRY(cyg_interrupt_create_entry,                               \
                     "cyg_interrupt_create", cyg_interrupt_create);           \
CYG_LDR_TABLE_ENTRY(cyg_interrupt_delete_entry,                               \
                     "cyg_interrupt_delete", cyg_interrupt_delete);           \
CYG_LDR_TABLE_ENTRY(cyg_interrupt_attach_entry,                               \
                     "cyg_interrupt_attach", cyg_interrupt_attach);           \
CYG_LDR_TABLE_ENTRY(cyg_interrupt_detach_entry,                               \
                     "cyg_interrupt_detach", cyg_interrupt_detach);           \
CYG_LDR_TABLE_ENTRY(cyg_interrupt_get_vsr_entry,                              \
                     "cyg_interrupt_get_vsr", cyg_interrupt_get_vsr);         \
CYG_LDR_TABLE_ENTRY(cyg_interrupt_set_vsr_entry,                              \
                     "cyg_interrupt_set_vsr", cyg_interrupt_set_vsr);         \
CYG_LDR_TABLE_ENTRY(cyg_interrupt_disable_entry,                              \
                     "cyg_interrupt_disable", cyg_interrupt_disable);         \
CYG_LDR_TABLE_ENTRY(cyg_interrupt_enable_entry,                               \
                     "cyg_interrupt_enable", cyg_interrupt_enable);           \
CYG_LDR_TABLE_ENTRY(cyg_interrupt_mask_entry,                                 \
                     "cyg_interrupt_mask", cyg_interrupt_mask);               \
CYG_LDR_TABLE_ENTRY(cyg_interrupt_mask_intunsafe_entry,                       \
                     "cyg_interrupt_mask_intunsafe",                          \
                     cyg_interrupt_mask_intunsafe);                           \
CYG_LDR_TABLE_ENTRY(cyg_interrupt_unmask_entry,                               \
                     "cyg_interrupt_unmask", cyg_interrupt_unmask);           \
CYG_LDR_TABLE_ENTRY(cyg_interrupt_unmask_intunsafe_entry,                     \
                     "cyg_interrupt_unmask_intunsafe",                        \
                     cyg_interrupt_unmask_intunsafe);                         \
CYG_LDR_TABLE_ENTRY(cyg_interrupt_acknowledge_entry,                          \
                     "cyg_interrupt_acknowledge",                             \
                     cyg_interrupt_acknowledge);                              \
CYG_LDR_TABLE_ENTRY(cyg_interrupt_configure_entry,                            \
                     "cyg_interrupt_configure", cyg_interrupt_configure);     \
CYG_LDR_TABLE_ENTRY(cyg_interrupt_set_cpu_entry,                              \
                     "cyg_interrupt_set_cpu", cyg_interrupt_set_cpu);         \
CYG_LDR_TABLE_ENTRY(cyg_interrupt_get_cpu_entry,                              \
                     "cyg_interrupt_get_cpu", cyg_interrupt_get_cpu);

#define CYG_LDR_TABLE_KAPI_MBOX()                                             \
CYG_LDR_TABLE_ENTRY(cyg_mbox_create_entry,                                    \
                     "cyg_mbox_create", cyg_mbox_create);                     \
CYG_LDR_TABLE_ENTRY(cyg_mbox_delete_entry,                                    \
                     "cyg_mbox_delete", cyg_mbox_delete);                     \
CYG_LDR_TABLE_ENTRY(cyg_mbox_get_entry,                                       \
                     "cyg_mbox_get", cyg_mbox_get);                           \
CYG_LDR_TABLE_ENTRY(cyg_mbox_timed_get_entry,                                 \
                     "cyg_mbox_timed_get", cyg_mbox_timed_get);               \
CYG_LDR_TABLE_ENTRY(cyg_mbox_tryget_entry,                                    \
                     "cyg_mbox_tryget", cyg_mbox_tryget);                     \
CYG_LDR_TABLE_ENTRY(cyg_mbox_peek_item_entry,                                 \
                     "cyg_mbox_peek_item", cyg_mbox_peek_item);               \
CYG_LDR_TABLE_ENTRY(cyg_mbox_put_entry,                                       \
                     "cyg_mbox_put", cyg_mbox_put);                           \
CYG_LDR_TABLE_ENTRY(cyg_mbox_timed_put_entry,                                 \
                     "cyg_mbox_timed_put", cyg_mbox_timed_put);               \
CYG_LDR_TABLE_ENTRY(cyg_mbox_tryput_entry,                                    \
                     "cyg_mbox_tryput", cyg_mbox_tryput);                     \
CYG_LDR_TABLE_ENTRY(cyg_mbox_peek_entry,                                      \
                     "cyg_mbox_peek", cyg_mbox_peek);                         \
CYG_LDR_TABLE_ENTRY(cyg_mbox_waiting_to_get_entry,                            \
                     "cyg_mbox_waiting_to_get",                               \
                     cyg_mbox_waiting_to_get);                                \
CYG_LDR_TABLE_ENTRY(cyg_mbox_waiting_to_put_entry,                            \
                     "cyg_mbox_waiting_to_put",                               \
                     cyg_mbox_waiting_to_put);

#define CYG_LDR_TABLE_KAPI_MEMPOOL_FIX()                                      \
CYG_LDR_TABLE_ENTRY(cyg_mempool_fix_create_entry,                             \
                     "cyg_mempool_fix_create", cyg_mempool_fix_create);       \
CYG_LDR_TABLE_ENTRY(cyg_mempool_fix_delete_entry,                             \
                     "cyg_mempool_fix_delete", cyg_mempool_fix_delete);       \
CYG_LDR_TABLE_ENTRY(cyg_mempool_fix_alloc_entry,                              \
                     "cyg_mempool_fix_alloc", cyg_mempool_fix_alloc);         \
CYG_LDR_TABLE_ENTRY(cyg_mempool_fix_timed_alloc_entry,                        \
                     "cyg_mempool_fix_timed_alloc",                           \
                     cyg_mempool_fix_timed_alloc);                            \
CYG_LDR_TABLE_ENTRY(cyg_mempool_fix_try_alloc_entry,                          \
                     "cyg_mempool_fix_try_alloc",                             \
                     cyg_mempool_fix_try_alloc);                              \
CYG_LDR_TABLE_ENTRY(cyg_mempool_fix_free_entry,                               \
                     "cyg_mempool_fix_free", cyg_mempool_fix_free);           \
CYG_LDR_TABLE_ENTRY(cyg_mempool_fix_waiting_entry,                            \
                     "cyg_mempool_fix_waiting", cyg_mempool_fix_waiting);     \
CYG_LDR_TABLE_ENTRY(cyg_mempool_fix_get_info_entry,                           \
                     "cyg_mempool_fix_get_info", cyg_mempool_fix_get_info);

#define CYG_LDR_TABLE_KAPI_MEMPOOL_VAR()                                      \
CYG_LDR_TABLE_ENTRY(cyg_mempool_var_create_entry,                             \
                     "cyg_mempool_var_create", cyg_mempool_var_create);       \
CYG_LDR_TABLE_ENTRY(cyg_mempool_var_delete_entry,                             \
                     "cyg_mempool_var_delete", cyg_mempool_var_delete);       \
CYG_LDR_TABLE_ENTRY(cyg_mempool_var_alloc_entry,                              \
                     "cyg_mempool_var_alloc", cyg_mempool_var_alloc);         \
CYG_LDR_TABLE_ENTRY(cyg_mempool_var_timed_alloc_entry,                        \
                     "cyg_mempool_var_timed_alloc",                           \
                     cyg_mempool_var_timed_alloc);                            \
CYG_LDR_TABLE_ENTRY(cyg_mempool_var_try_alloc_entry,                          \
                     "cyg_mempool_var_try_alloc",                             \
                     cyg_mempool_var_try_alloc);                              \
CYG_LDR_TABLE_ENTRY(cyg_mempool_var_free_entry,                               \
                     "cyg_mempool_var_free", cyg_mempool_var_free);           \
CYG_LDR_TABLE_ENTRY(cyg_mempool_var_waiting_entry,                            \
                     "cyg_mempool_var_waiting", cyg_mempool_var_waiting);     \
CYG_LDR_TABLE_ENTRY(cyg_mempool_var_get_info_entry,                           \
                     "cyg_mempool_var_get_info", cyg_mempool_var_get_info);

#define CYG_LDR_TABLE_KAPI_MUTEX()                                            \
CYG_LDR_TABLE_ENTRY(cyg_mutex_init_entry,                                     \
                     "cyg_mutex_init", cyg_mutex_init);                       \
CYG_LDR_TABLE_ENTRY(cyg_mutex_destroy_entry,                                  \
                     "cyg_mutex_destroy", cyg_mutex_destroy);                 \
CYG_LDR_TABLE_ENTRY(cyg_mutex_trylock_entry,                                  \
                     "cyg_mutex_trylock", cyg_mutex_trylock);                 \
CYG_LDR_TABLE_ENTRY(cyg_mutex_unlock_entry,                                   \
                     "cyg_mutex_unlock", cyg_mutex_unlock);                   \
CYG_LDR_TABLE_ENTRY(cyg_mutex_release_entry,                                  \
                     "cyg_mutex_release", cyg_mutex_release);                 \
CYG_LDR_TABLE_ENTRY(cyg_mutex_set_ceiling_entry,                              \
                     "cyg_mutex_set_ceiling", cyg_mutex_set_ceiling);         \
CYG_LDR_TABLE_ENTRY(cyg_mutex_set_protocol_entry,                             \
                     "cyg_mutex_set_protocol", cyg_mutex_set_protocol);

#define CYG_LDR_TABLE_KAPI_SCHEDULER()                                        \
CYG_LDR_TABLE_ENTRY(cyg_scheduler_start_entry,                                \
                     "cyg_scheduler_start", cyg_scheduler_start);             \
CYG_LDR_TABLE_ENTRY(cyg_scheduler_lock_entry,                                 \
                     "cyg_scheduler_lock", cyg_scheduler_lock);               \
CYG_LDR_TABLE_ENTRY(cyg_scheduler_safe_lock_entry,                            \
                     "cyg_scheduler_safe_lock", cyg_scheduler_safe_lock);     \
CYG_LDR_TABLE_ENTRY(cyg_scheduler_unlock_entry,                               \
                     "cyg_scheduler_unlock", cyg_scheduler_unlock);           \
CYG_LDR_TABLE_ENTRY(cyg_scheduler_read_lock_entry,                            \
                     "cyg_scheduler_read_lock", cyg_scheduler_read_lock);     \

#define CYG_LDR_TABLE_KAPI_SEMAPHORE()                                        \
CYG_LDR_TABLE_ENTRY(cyg_semaphore_init_entry,                                 \
                     "cyg_semaphore_init", cyg_semaphore_init);               \
CYG_LDR_TABLE_ENTRY(cyg_semaphore_destroy_entry,                              \
                     "cyg_semaphore_destroy", cyg_semaphore_destroy);         \
CYG_LDR_TABLE_ENTRY(cyg_semaphore_wait_entry,                                 \
                     "cyg_semaphore_wait", cyg_semaphore_wait);               \
CYG_LDR_TABLE_ENTRY(cyg_semaphore_timed_wait_entry,                           \
                     "cyg_semaphore_timed_wait", cyg_semaphore_timed_wait);   \
CYG_LDR_TABLE_ENTRY(cyg_semaphore_try_wait_entry,                             \
                     "cyg_semaphore_trywait", cyg_semaphore_trywait);         \
CYG_LDR_TABLE_ENTRY(cyg_semaphore_post_entry,                                 \
                     "cyg_semaphore_post", cyg_semaphore_post);               \
CYG_LDR_TABLE_ENTRY(cyg_semaphore_peek_entry,                                 \
                     "cyg_semaphore_peek", cyg_semaphore_peek);

#define CYG_LDR_TABLE_KAPI_THREAD()                                           \
CYG_LDR_TABLE_ENTRY(cyg_thread_create_entry,                                  \
                     "cyg_thread_create", cyg_thread_create);                 \
CYG_LDR_TABLE_ENTRY(cyg_thread_exit_entry,                                    \
                     "cyg_thread_exit", cyg_thread_exit);                     \
CYG_LDR_TABLE_ENTRY(cyg_thread_delete_entry,                                  \
                     "cyg_thread_delete", cyg_thread_delete);                 \
CYG_LDR_TABLE_ENTRY(cyg_thread_suspend_entry,                                 \
                     "cyg_thread_suspend", cyg_thread_suspend);               \
CYG_LDR_TABLE_ENTRY(cyg_thread_resume_entry,                                  \
                     "cyg_thread_resume", cyg_thread_resume);                 \
CYG_LDR_TABLE_ENTRY(cyg_thread_kill_entry,                                    \
                     "cyg_thread_kill", cyg_thread_kill);                     \
CYG_LDR_TABLE_ENTRY(cyg_thread_release_entry,                                 \
                     "cyg_thread_release", cyg_thread_release);               \
CYG_LDR_TABLE_ENTRY(cyg_thread_yield_entry,                                   \
                     "cyg_thread_yield", cyg_thread_yield);                   \
CYG_LDR_TABLE_ENTRY(cyg_thread_self_entry,                                    \
                     "cyg_thread_self", cyg_thread_self);                     \
CYG_LDR_TABLE_ENTRY(cyg_thread_idle_thread_entry,                             \
                     "cyg_thread_idle_thread", cyg_thread_idle_thread);       \
CYG_LDR_TABLE_ENTRY(cyg_thread_set_priority_entry,                            \
                     "cyg_thread_set_priority", cyg_thread_set_priority);     \
CYG_LDR_TABLE_ENTRY(cyg_thread_get_priority_entry,                            \
                     "cyg_thread_get_priority", cyg_thread_get_priority);     \
CYG_LDR_TABLE_ENTRY(cyg_thread_get_current_priority_entry,                    \
                     "cyg_thread_get_current_priority",                       \
                     cyg_thread_get_current_priority);                        \
CYG_LDR_TABLE_ENTRY(cyg_thread_delay_entry,                                   \
                     "cyg_thread_delay", cyg_thread_delay);                   \
CYG_LDR_TABLE_ENTRY(cyg_thread_get_stack_base_entry,                          \
                     "cyg_thread_get_stack_base", cyg_thread_get_stack_base); \
CYG_LDR_TABLE_ENTRY(cyg_thread_get_stack_size_entry,                          \
                     "cyg_thread_get_stack_size", cyg_thread_get_stack_size); \
CYG_LDR_TABLE_ENTRY(cyg_thread_new_data_index_entry,                          \
                     "cyg_thread_new_data_index", cyg_thread_new_data_index); \
CYG_LDR_TABLE_ENTRY(cyg_thread_free_data_index_entry,                         \
                     "cyg_thread_free_data_index",                            \
                     cyg_thread_free_data_index);                             \
CYG_LDR_TABLE_ENTRY(cyg_thread_get_data_entry,                                \
                     "cyg_thread_get_data", cyg_thread_get_data);             \
CYG_LDR_TABLE_ENTRY(cyg_thread_get_data_ptr_entry,                            \
                     "cyg_thread_get_data_ptr", cyg_thread_get_data_ptr);     \
CYG_LDR_TABLE_ENTRY(cyg_thread_set_data_entry,                                \
                     "cyg_thread_set_data", cyg_thread_set_data);             

#define CYG_LDR_TABLE_STRING()                                               \
CYG_LDR_TABLE_ENTRY(memchr_entry,  "memchr",  memchr);                       \
CYG_LDR_TABLE_ENTRY(memcmp_entry,  "memcmp",  memcmp);                       \
CYG_LDR_TABLE_ENTRY(memcpy_entry,  "memcpy",  memcpy);                       \
CYG_LDR_TABLE_ENTRY(memmove_entry, "memmove", memmove);                      \
CYG_LDR_TABLE_ENTRY(memset_entry,  "memset",  memset);                       \
CYG_LDR_TABLE_ENTRY(strcpy_entry,  "strcpy",  strcpy);                       \
CYG_LDR_TABLE_ENTRY(strncpy_entry, "strncpy", strncpy);                      \
CYG_LDR_TABLE_ENTRY(strcat_entry,  "strcat",  strcat);                       \
CYG_LDR_TABLE_ENTRY(strncat_entry, "strncat", strncat);                      \
CYG_LDR_TABLE_ENTRY(strchr_entry,  "strchr",  strchr);                       \
CYG_LDR_TABLE_ENTRY(strrchr_entry, "strrchr", strrchr);                      \
CYG_LDR_TABLE_ENTRY(strcoll_entry, "strcoll", strcoll);                      \
CYG_LDR_TABLE_ENTRY(strlen_entry,  "strlen",  strlen);                       \
CYG_LDR_TABLE_ENTRY(strcmp_entry,  "strcmp",  strcmp);                       \
CYG_LDR_TABLE_ENTRY(strncmp_entry, "strncmp", strncmp);                      \
CYG_LDR_TABLE_ENTRY(strcspn_entry, "strcspn", strcspn);                      \
CYG_LDR_TABLE_ENTRY(strerror_entry, "strerror", strerror);                   \
CYG_LDR_TABLE_ENTRY(strpbrk_entry, "strpbrk", strpbrk);                      \
CYG_LDR_TABLE_ENTRY(strspn_entry,  "strspn",  strspn);                       \
CYG_LDR_TABLE_ENTRY(strstr_entry,  "strstr",  strstr);                       \
CYG_LDR_TABLE_ENTRY(strtok_entry,  "strtok",  strtok);                       \
CYG_LDR_TABLE_ENTRY(strxfrm_entry, "strxfrm", strxfrm); 

#define CYG_LDR_TABLE_STDIO()                                                \
CYG_LDR_TABLE_ENTRY(clearerr_entry,  "clearerr",  clearerr);                 \
CYG_LDR_TABLE_ENTRY(fclose_entry,  "fclose",  fclose);                       \
CYG_LDR_TABLE_ENTRY(feof_entry,    "feof",    feof);                         \
CYG_LDR_TABLE_ENTRY(ferror_entry,  "ferror",  ferror);                       \
CYG_LDR_TABLE_ENTRY(fflush_entry,  "fflush",  fflush);                       \
CYG_LDR_TABLE_ENTRY(fgetc_entry,   "fgetc",   fgetc);                        \
CYG_LDR_TABLE_ENTRY(fgetpos_entry, "fgetpos", fgetpos);                      \
CYG_LDR_TABLE_ENTRY(fgets_entry,   "fgets",   fgets);                        \
CYG_LDR_TABLE_ENTRY(fopen_entry,   "fopen",   fopen);                        \
CYG_LDR_TABLE_ENTRY(fprintf_entry, "fprintf", fprintf);                      \
CYG_LDR_TABLE_ENTRY(fputc_entry,   "fputc",   fputc);                        \
CYG_LDR_TABLE_ENTRY(fputs_entry,   "fputs",   fputs);                        \
CYG_LDR_TABLE_ENTRY(fread_entry,   "fread",   fread);                        \
CYG_LDR_TABLE_ENTRY(freopen_entry, "freopen", freopen);                      \
CYG_LDR_TABLE_ENTRY(fscanf_entry,  "fscanf",  fscanf);                       \
CYG_LDR_TABLE_ENTRY(fseek_entry,   "fseek",   fseek);                        \
CYG_LDR_TABLE_ENTRY(fsetpos_entry, "fsetpos", fsetpos);                      \
CYG_LDR_TABLE_ENTRY(ftell_entry,   "ftell",   ftell);                        \
CYG_LDR_TABLE_ENTRY(fwrite_entry,  "fwrite",  fwrite);                       \
CYG_LDR_TABLE_ENTRY(perror_entry,  "perror",  perror);                       \
CYG_LDR_TABLE_ENTRY(printf_entry,  "printf",  printf);                       \
CYG_LDR_TABLE_ENTRY(putc_entry,    "putc",    putc);                         \
CYG_LDR_TABLE_ENTRY(putchar_entry, "putchar", putchar);                      \
CYG_LDR_TABLE_ENTRY(puts_entry,    "puts",    puts);                         \
CYG_LDR_TABLE_ENTRY(remove_entry,  "remove",  remove);                       \
CYG_LDR_TABLE_ENTRY(rename_entry,  "rename",  rename);                       \
CYG_LDR_TABLE_ENTRY(rewind_entry,  "rewind",  rewind);                       \
CYG_LDR_TABLE_ENTRY(scanf_entry,   "scanf",   scanf);                        \
CYG_LDR_TABLE_ENTRY(setbuf_entry,  "setbuf",  setbuf);                       \
CYG_LDR_TABLE_ENTRY(setvbuf_entry, "setvbuf", setvbuf);                      \
CYG_LDR_TABLE_ENTRY(sprintf_entry, "sprintf", sprintf);                      \
CYG_LDR_TABLE_ENTRY(sscanf_entry,  "sscanf",  sscanf);                       \
CYG_LDR_TABLE_ENTRY(tmpfile_entry, "tmpfile", tmpfile);                      \
CYG_LDR_TABLE_ENTRY(tmpnam_entry,  "tmpnam",  tmpnam);                       \
CYG_LDR_TABLE_ENTRY(ungetc_entry,  "ungetc",  ungetc);

#define CYG_LDR_TABLE_INFRA_DIAG()                                            \
CYG_LDR_TABLE_ENTRY(diag_init_entry, "diag_init", diag_init);                 \
CYG_LDR_TABLE_ENTRY(diag_write_char_entry,                                    \
                     "diag_write_char", diag_write_char);                     \
CYG_LDR_TABLE_ENTRY(diag_write_string_entry,                                  \
                     "diag_write_string", diag_write_string);                 \
CYG_LDR_TABLE_ENTRY(diag_write_dec_entry,                                     \
                     "diag_write_dec", diag_write_dec);                       \
CYG_LDR_TABLE_ENTRY(diag_write_hex_entry,                                     \
                     "diag_write_hex", diag_write_hex);                       \
CYG_LDR_TABLE_ENTRY(diag_dump_buf_entry,                                      \
                     "diag_dump_buf", diag_dump_buf);                         \
CYG_LDR_TABLE_ENTRY(diag_dump_buf_32bit_entry,                                \
                     "diag_dump_buf_32bit", diag_dump_buf_32bit);             \
CYG_LDR_TABLE_ENTRY(diag_dump_buf_16bit_entry,                                \
                     "diag_dump_buf_16bit", diag_dump_buf_16bit);             \
CYG_LDR_TABLE_ENTRY(diag_vdump_buf_with_offset_entry,                         \
                     "diag_vdump_buf_with_offset",                            \
                     diag_vdump_buf_with_offset);                             \
CYG_LDR_TABLE_ENTRY(diag_dump_buf_with_offset_entry,                          \
                     "diag_dump_buf_with_offset",                             \
                     diag_dump_buf_with_offset);                              \
CYG_LDR_TABLE_ENTRY(diag_dump_buf_with_offset_32bit_entry,                    \
                     "diag_dump_buf_with_offset_32bit",                       \
                     diag_dump_buf_with_offset_32bit);                        \
CYG_LDR_TABLE_ENTRY(diag_dump_buf_with_offset_16bit_entry,                    \
                     "diag_dump_buf_with_offset_16bit",                       \
                     diag_dump_buf_with_offset_16bit);                        \
CYG_LDR_TABLE_ENTRY(diag_printf_entry, "diag_printf", diag_printf);           \
CYG_LDR_TABLE_ENTRY(diag_init_putc_entry, "diag_init_putc", diag_init_putc);  \
CYG_LDR_TABLE_ENTRY(diag_sprintf_entry, "diag_sprintf", diag_sprintf);        \
CYG_LDR_TABLE_ENTRY(diag_snprintf_entry, "diag_snprintf", diag_snprintf);     \
CYG_LDR_TABLE_ENTRY(diag_vsprintf_entry, "diag_vsprintf", diag_vsprintf);     \
CYG_LDR_TABLE_ENTRY(diag_vprintf_entry, "diag_vprintf", diag_vprintf);

//==============================================================================
#endif
