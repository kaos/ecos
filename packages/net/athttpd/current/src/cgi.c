/* =================================================================
 *
 *      cgi.c
 *
 *      Handles the CGI requests via OBJLOADER or tcl
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
 *  Contributors: Sergei Gavrikov (w3sg@SoftHome.net)
 *  Date:         2006-06-12
 *  Purpose:      
 *  Description:  
 *               
 * ####DESCRIPTIONEND####
 * 
 * =================================================================
 */
#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <cyg/hal/hal_tables.h>
#include <cyg/fileio/fileio.h>
#include <dirent.h>
#include <network.h>

#include <string.h>
#include <stdio.h>                     // sprintf().

#define JIM_EMBEDDED
#include <cyg/athttpd/http.h>
#include <cyg/athttpd/socket.h>
#include <cyg/athttpd/handler.h>
#include <cyg/athttpd/forms.h>

#ifdef CYGOPT_NET_ATHTTPD_USE_CGIBIN_OBJLOADER
#include <cyg/objloader/elf.h>
#include <cyg/objloader/objelf.h>
#endif

// =============================================================================
// Objloader CGI Support
// =============================================================================
#ifdef CYGOPT_NET_ATHTTPD_USE_CGIBIN_OBJLOADER
CYG_LDR_TABLE_STDIO()
CYG_LDR_TABLE_STRING()

// Chunked transfers.
CYG_LDR_TABLE_ENTRY(cyg_httpd_start_chunked_entry,      \
                    "cyg_httpd_start_chunked",          \
                    cyg_httpd_start_chunked);
CYG_LDR_TABLE_ENTRY(cyg_httpd_write_chunked_entry,      \
                    "cyg_httpd_write_chunked",          \
                    cyg_httpd_write_chunked);
CYG_LDR_TABLE_ENTRY(cyg_httpd_end_chunked_entry,      \
                    "cyg_httpd_end_chunked",          \
                    cyg_httpd_end_chunked);

// Mime strings.
CYG_LDR_TABLE_ENTRY(cyg_httpd_write_entry, \
                    "cyg_httpd_write",     \
                    cyg_httpd_write);
                     
// Entry to find a GET/POST variable in the table.                     
CYG_LDR_TABLE_ENTRY(cyg_httpd_find_form_variable_entry, \
                    "cyg_httpd_find_form_variable",     \
                    cyg_httpd_find_form_variable);

// Finds an internal resource given the URL. Returns a pointer to the entry
//  in the resource table.
CYG_LDR_TABLE_ENTRY(cyg_httpd_find_ires_entry, \
                    "cyg_httpd_find_ires",     \
                    cyg_httpd_find_ires);
// Sends an internal resource out, given the entry in the reource table.
CYG_LDR_TABLE_ENTRY(cyg_httpd_send_ires_entry, \
                    "cyg_httpd_send_ires",     \
                    cyg_httpd_send_ires);
                     
CYG_LDR_TABLE_ENTRY(cyg_httpd_format_header_entry, \
                    "cyg_httpd_format_header",     \
                    cyg_httpd_format_header);
CYG_LDR_TABLE_ENTRY(diag_printf_entry, "diag_printf", diag_printf);
CYG_LDR_TABLE_ENTRY(cyg_httpd_find_mime_string_entry, \
                    "cyg_httpd_find_mime_string",     \
                    cyg_httpd_find_mime_string);
#endif // CYGOPT_NET_ATHTTPD_USE_CGIBIN_OBJLOADER

#ifdef CYGOPT_NET_ATHTTPD_USE_CGIBIN_OBJLOADER
cyg_int32 cyg_httpd_exec_cgi_objloader(char *file_name)
{
    // Check if the file is in the file system.
    struct stat sp;
    cyg_int32 (*fn)(CYG_HTTPD_STATE*);
    
    cyg_int32 rc = stat(file_name, &sp);
    if(rc < 0)
    {
        cyg_httpd_send_error(CYG_HTTPD_STATUS_NOT_FOUND);
        return 0;
    }    
    
    void *lib_handle = cyg_ldr_open_library((CYG_ADDRWORD)file_name, 0);
    CYG_ASSERT(lib_handle != 0, "Error opening the library");
    if (lib_handle == 0)
    {
        cyg_httpd_send_error(CYG_HTTPD_STATUS_SYSTEM_ERROR);
        return 0;
    }    
    fn = cyg_ldr_find_symbol(lib_handle, "exec_cgi");
    if (fn)
    {
        cyg_int32 rc = fn(&httpstate);
        cyg_ldr_close_library(lib_handle);
        return rc;
    }    
    else
    {
        cyg_ldr_close_library(lib_handle);
        cyg_httpd_send_error(CYG_HTTPD_STATUS_SYSTEM_ERROR);
        return 0;
    }    
}
#endif

// =============================================================================
// tcl CGI Support
// =============================================================================
#ifdef CYGOPT_NET_ATHTTPD_USE_CGIBIN_TCL
int Jim_AioInit(Jim_Interp *);
cyg_int32
cyg_httpd_exec_cgi_tcl(char *file_name)
{
    // Make sure that tcl sees the internal variables including the post_data.
    cyg_httpd_fvars_table_entry *entry = cyg_httpd_fvars_table;
    while (entry != cyg_httpd_fvars_table_end)
    {
        if (strlen(entry->buf) != 0)
            Jim_SetVariableStrWithStr(httpstate.jim_interp, 
                                      entry->name, 
                                      entry->buf);
        entry++;
    }

    if (httpstate.post_data != NULL)
        Jim_SetVariableStrWithStr(httpstate.jim_interp, 
                                  "post_data", 
                                  httpstate.post_data);
     
    char tcl_cmd[CYG_HTTPD_MAXPATH];
    sprintf(tcl_cmd, "source %s", file_name);
    Jim_Eval(httpstate.jim_interp, tcl_cmd);
    return 0;
}

int
cyg_httpd_Jim_Command_startchunked(Jim_Interp *interp, 
                                   int argc,
                                   Jim_Obj *const *argv)
{
    char *buf = (char*)Jim_GetString(argv[1], NULL);
    cyg_httpd_start_chunked(buf);
    return JIM_OK;
}
  
int
cyg_httpd_Jim_Command_writechunked(Jim_Interp *interp, 
                                   int argc,
                                   Jim_Obj *const *argv)
{
    int len;
    char *buf = (char*)Jim_GetString(argv[1], &len);
    cyg_httpd_write_chunked(buf, len);
    return JIM_OK;
}
  
int
cyg_httpd_Jim_Command_endchunked(Jim_Interp *interp, 
                                 int argc,
                                 Jim_Obj *const *argv)
{
    cyg_httpd_end_chunked();
    return JIM_OK;
}
  
void
cyg_httpd_init_tcl_interpreter(void)
{
    // Start the TCL interpreter.
    Jim_InitEmbedded();
    httpstate.jim_interp = Jim_CreateInterp();
    Jim_RegisterCoreCommands(httpstate.jim_interp);
    Jim_AioInit(httpstate.jim_interp);
    
    // Add a new cyg_httpd_write_chunked command for tcl.
    Jim_CreateCommand(httpstate.jim_interp,
                      "start_chunked",
                      cyg_httpd_Jim_Command_startchunked,
                      NULL, 
                      NULL);
    Jim_CreateCommand(httpstate.jim_interp,
                      "write_chunked",
                      cyg_httpd_Jim_Command_writechunked,
                      NULL, 
                      NULL);
    Jim_CreateCommand(httpstate.jim_interp,
                      "end_chunked",
                      cyg_httpd_Jim_Command_endchunked,
                      NULL, 
                      NULL);
}                      
#endif    

// =============================================================================
// CGI Support
// =============================================================================
#if defined(CYGOPT_NET_ATHTTPD_USE_CGIBIN_TCL ) || \
    defined(CYGOPT_NET_ATHTTPD_USE_CGIBIN_OBJLOADER)
cyg_int32 
cyg_httpd_exec_cgi(void)
{
    char  file_name[CYG_HTTPD_MAXPATH];
    
    strcpy(file_name, CYGDAT_NET_ATHTTPD_SERVEROPT_ROOTDIR);
    if (file_name[strlen(file_name)-1] != '/')
        strcat(file_name, "/");
    strcat(file_name, httpstate.url);
    cyg_httpd_cleanup_filename(file_name);
    
    char *extension = rindex(httpstate.url, '.');
    if (extension == NULL)
    {
        // File in CGI request _must_ have an extension.
        cyg_httpd_send_error(CYG_HTTPD_STATUS_SYSTEM_ERROR);
        return 0;
    }    

#ifdef CYGOPT_NET_ATHTTPD_USE_CGIBIN_OBJLOADER
    if (strcmp(extension, CYG_HTTPD_DEFAULT_CGIBIN_OBJLOADER_EXTENSION) == 0)
    {
        // Load a cgibin via OBJLOADER.
        cyg_int32 rc = cyg_httpd_exec_cgi_objloader(file_name);
        return rc;
    }    
#endif
#ifdef CYGOPT_NET_ATHTTPD_USE_CGIBIN_TCL
    if (strcmp(extension, CYG_HTTPD_DEFAULT_CGIBIN_TCL_EXTENSION) == 0)
    {
        // Load a cgibin via the TCL interpreter.
        cyg_int32 rc = cyg_httpd_exec_cgi_tcl(file_name);
        return rc;
    }    
#endif
    cyg_httpd_send_error(CYG_HTTPD_STATUS_SYSTEM_ERROR);
    return 0;
}
#endif 

