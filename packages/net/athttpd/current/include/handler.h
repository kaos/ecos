/* =================================================================
 *
 *      handler.h
 *
 *      Improved HTTPD server.
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
 *  Contributors: 
 *  Date:         2006-06-12
 *  Purpose:      
 *  Description:  
 *               
 * ####DESCRIPTIONEND####
 * 
 * =================================================================
 */
#ifndef __HANDLER_H__
#define __HANDLER_H__

// =============================================================================
// Internal Resources
// =============================================================================
struct cyg_httpd_ires_table_entry
{
    char *f_pname;
    char *f_ptr;
    int   f_size; 
} CYG_HAL_TABLE_TYPE;

typedef struct cyg_httpd_ires_table_entry cyg_httpd_ires_table_entry;

#define CYG_HTTPD_IRES_TABLE_ENTRY(__name, __path_name, __pmem, __size) \
cyg_httpd_ires_table_entry __name \
   CYG_HAL_TABLE_ENTRY(httpd_ires_table) = {(char*)__path_name, \
                                            (char*)__pmem, (int)__size} 
      
cyg_httpd_ires_table_entry *cyg_httpd_find_ires(char *);
void cyg_httpd_send_ires(cyg_httpd_ires_table_entry *);

// =============================================================================
// C callbacks
// =============================================================================
typedef cyg_int32  (*handler)(CYG_HTTPD_STATE*);
struct cyg_httpd_handler_table_entry
{
    char     *path;
    handler   h;
} CYG_HAL_TABLE_TYPE;

typedef struct cyg_httpd_handler_table_entry cyg_httpd_handler_table_entry;

#define CYG_HTTPD_HANDLER_TABLE_ENTRY(__name, __pattern, __arg) \
cyg_httpd_handler_table_entry __name \
      CYG_HAL_TABLE_ENTRY(httpd_handler_table) = { __pattern, __arg } 

handler cyg_httpd_find_handler(void);
cyg_int32 cyg_httpd_exec_cgi(void);
void cyg_httpd_send_directory_listing(char*);

#endif

