/* =================================================================
 *
 *      forms.c
 *
 *      Handles form variables of GET and POST requests.
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
#include <pkgconf/io_fileio.h>
#include <cyg/kernel/kapi.h>           // Kernel API.
#include <cyg/kernel/ktypes.h>         // base kernel types.
#include <cyg/infra/diag.h>            // For diagnostic printing.
#include <network.h>
#include <sys/uio.h>

#include <cyg/hal/hal_tables.h>
#include <stdlib.h>

#include <cyg/athttpd/http.h>
#include <cyg/athttpd/socket.h>
#include <cyg/athttpd/handler.h>
#include <cyg/athttpd/forms.h>

CYG_HAL_TABLE_BEGIN(cyg_httpd_fvars_table, httpd_fvars_table);
CYG_HAL_TABLE_END(cyg_httpd_fvars_table_end, httpd_fvars_table);

cyg_int8 blank[] = "";

cyg_int8
cyg_httpd_from_hex (cyg_int8 c)
{
    return  c >= '0' && c <= '9' ?  c - '0'
            : c >= 'A' && c <= 'F'? c - 'A' + 10
            : c - 'a' + 10;     
}

char*
cyg_httpd_store_form_variable(char *query, cyg_httpd_fvars_table_entry *entry)
{
    char *p = query;
    char *q = entry->buf;
    int   len = 0;
    
    while (len < (entry->buflen - 1))
        switch(*p)
        {
        case '%':
            p++;
            if (*p) 
                *q = cyg_httpd_from_hex(*p++) * 16;
            if (*p) 
                *q = (*q + cyg_httpd_from_hex(*p++));
            q++;
            len++;
            break;
        case '+':
            *q++ = ' ';
            p++;
            len++;
            break;
        case '&':
        case ' ':
            *q++ = '\0';
            return p;
        default:    
            *q++ = *p++;
            len++;
        }
        *q = '\0';
        while ((*p != ' ') && (*p != '&'))
            p++;
        return p;
} 

// We'll try to parse the data from the form, and store it in the variables
//  that have been defined by the user in the 'form_variable_table'.
char*
cyg_httpd_store_form_data(char *p)
{
    char      *p2;
    cyg_int32 var_length;
    cyg_httpd_fvars_table_entry *entry = cyg_httpd_fvars_table;
    
    // We'll clear all the variables first, to avoid stale data.
    while (entry != cyg_httpd_fvars_table_end)
    {
        entry->buf[0] = '\0';
        entry++;
    }

    while(*p != ' ')
    {
        p2 = strchr(p, '=');
        var_length = (cyg_int32)p2 - (cyg_int32)p;
        entry = cyg_httpd_fvars_table;
        while (entry != cyg_httpd_fvars_table_end)
        {
            if (!strncmp((const char*)p, entry->name, var_length ))
                break;
            entry++;
        }
                
        if (entry == cyg_httpd_fvars_table_end)
        {
            // No such variable. Run through the data.
            while ((*p != '&') && (*p != ' '))
                p++;
            continue;
        }
            
        p = p2;
        if (*p == '=')
        {
            ++p;
            // Found the variable, store the name.
            p = cyg_httpd_store_form_variable(p, entry);
#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 1
            diag_printf("Stored form variable: %s Value: %s\n",
                        entry->name,
                        entry->buf);
#endif    
        }
        if (*p == '&')
            p++;
    }
    return p;
}

char*
cyg_httpd_find_form_variable(char *p)
{
    cyg_httpd_fvars_table_entry *entry = cyg_httpd_fvars_table;
    
    while (entry != cyg_httpd_fvars_table_end)
    {
        if (!strcmp((const char*)p, entry->name))
            return entry->buf;
        entry++;
    }
            
    return (char*)0;
}

void
cyg_httpd_handle_method_POST(void)
{
    // TODO: Need to support the case in which a POST request will send
    //  LOTS of data that does not fit into a single inbuffer. For example,
    //  file transfer over HTTP.
    cyg_int32 len = read(httpstate.sockets[httpstate.client_index].descriptor,
                          httpstate.inbuffer, 
                          CYG_HTTPD_MAXINBUFFER);
    CYG_ASSERT(len > 0, "Cannot read POST data");
    if (len == 0)
        return;
    
    char  *cp = httpstate.inbuffer;
    while ((*cp == '\r') || (*cp == '\n'))
        cp++;
    cp[httpstate.inbuffer_len] = ' ';
    cyg_httpd_store_form_data(cp);

    handler h = cyg_httpd_find_handler();
    if (h != 0)
    {
        // A handler was found. We'll call the function associated to it. If 
        //  the return value is 0 we'll also call cyg_httpd_send_file().
        h(&httpstate);
        return;
    }

#if defined(CYGOPT_NET_ATHTTPD_USE_CGIBIN_OBJLOADER) || \
                           defined(CYGOPT_NET_ATHTTPD_USE_CGIBIN_TCL)
    // See if we are trying to execute a CGI via one of the supported methods.
    // If we the GET request is trying to access a file in the 
    //  CYGDAT_NET_ATHTTPD_SERVEROPT_CGIDIR directory then it is assumed that
    //  we are trying to execute a CGI script. The extension of the file will
    //  determine the appropriate interpreter to use.
    if (httpstate.url[0] == '/' &&
                    !strncmp(httpstate.url + 1, 
                              CYGDAT_NET_ATHTTPD_SERVEROPT_CGIDIR, 
                              strlen(CYGDAT_NET_ATHTTPD_SERVEROPT_CGIDIR)))
    {                              
        // Here we'll look for extension to the file. We'll call the cgi
        //  handler only if the extension is '.o'.
        cyg_httpd_exec_cgi();
        return;
    }
#endif    

    // No handler of any kind for a post request. Must send 404.
    cyg_httpd_send_error(CYG_HTTPD_STATUS_NOT_FOUND);
}


