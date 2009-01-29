/* =================================================================
 *
 *      forms.h
 *
 *      Improved HTTPD server.
 *
 * ================================================================= 
 * ####ECOSGPLCOPYRIGHTBEGIN####                                     
 * -------------------------------------------                       
 * This file is part of eCos, the Embedded Configurable Operating System.
 * Copyright (C) 2005 Free Software Foundation, Inc.                 
 *
 * eCos is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later
 * version.                                                          
 *
 * eCos is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.                                                 
 *
 * You should have received a copy of the GNU General Public License 
 * along with eCos; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.     
 *
 * As a special exception, if other files instantiate templates or use
 * macros or inline functions from this file, or you compile this file
 * and link it with other works to produce a work based on this file,
 * this file does not by itself cause the resulting work to be covered by
 * the GNU General Public License. However the source code for this file
 * must still be made available in accordance with section (3) of the GNU
 * General Public License v2.                                        
 *
 * This exception does not invalidate any other reasons why a work based
 * on this file might be covered by the GNU General Public License.  
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
#ifndef __FORMS_H__
#define __FORMS_H__

struct cyg_httpd_fvars_table_entry
{
    char *name;
    char *buf; 
    int  buflen; 
} CYG_HAL_TABLE_TYPE;

typedef struct cyg_httpd_fvars_table_entry cyg_httpd_fvars_table_entry;

#define CYG_HTTPD_FVAR_TABLE_ENTRY(__name, __fvar, __val, __len) \
cyg_httpd_fvars_table_entry __name CYG_HAL_TABLE_ENTRY( httpd_fvars_table ) = \
                                                      {__fvar, __val, __len} 

__externC cyg_httpd_fvars_table_entry cyg_httpd_fvars_table[];
__externC cyg_httpd_fvars_table_entry cyg_httpd_fvars_table_end[];

// Prototypes.
char *cyg_httpd_store_form_data(char*);
void cyg_httpd_handle_method_POST(void);
cyg_int8 cyg_httpd_from_hex (cyg_int8);
char *cyg_httpd_find_form_variable(char*);
#endif
