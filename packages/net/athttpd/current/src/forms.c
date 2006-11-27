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
 *                Lars Povlsen    (lpovlsen@vitesse.com)
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

    if (!p)    /* No form data? just return after clearing variables */
        return NULL;

    while (*p && *p != ' ')
    {
        if (!(p2 = strchr(p, '=')))
            return NULL;        /* Malformed post? */
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
            while ((*p != '&') && (*p && *p != ' '))
                p++;
            if(*p == '&')
                p++;
            continue;
        }
            
        // Found the variable, store the name.
        p = cyg_httpd_store_form_variable(++p2, entry);
#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 1
        diag_printf("Stored form variable: %s Value: %s\n",
                    entry->name,
                    entry->buf);
#endif    
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
    CYG_ASSERT(httpstate.post_data == NULL, "Leftover content data");
    CYG_ASSERT(httpstate.header_end != NULL, "Cannot see POST data");
    if (httpstate.content_len == 0 || 
        httpstate.content_len > CYGNUM_ATHTTPD_SERVER_MAX_POST) {
        cyg_httpd_send_error(CYG_HTTPD_STATUS_BAD_REQUEST);
        return;
    }
    
    // The content data is only valid during a POST.
    httpstate.post_data = (char*)malloc(httpstate.content_len + 1);
    CYG_ASSERT(httpstate.post_data != NULL, "Cannot malloc POST buffer");
    if (httpstate.post_data == NULL)
    {
        cyg_httpd_send_error(CYG_HTTPD_STATUS_SYSTEM_ERROR);
        return;
    }

    /* Grab partial/all content from data read with headers */
    /*
     * TODO: This does NOT (yet) support multipart/form-data!
     */
    int header_len = (int)httpstate.header_end - (int)httpstate.inbuffer;
    int post_data_len = httpstate.inbuffer_len - header_len;
    
    // Some POST data might have come along with the header frame, and the
    //  rest is coming in on following frames. Copy the data that already
    //  arriced into the post buffer.
    memcpy(httpstate.post_data, httpstate.header_end, post_data_len);
    // Do we need additional data?
    if (post_data_len < httpstate.content_len) 
    {   
        while (post_data_len < httpstate.content_len)
        {
            cyg_int32 len = read(httpstate.sockets[httpstate.client_index].
                                                                   descriptor,
                                 httpstate.post_data + post_data_len,
                                 httpstate.content_len - post_data_len);
            if (len < 0)
            {
                /* This releases POST data area*/
                free(httpstate.post_data);
                httpstate.post_data = NULL;
                return;
            }    
            post_data_len += len;
        }    
    }
    CYG_ASSERT(post_data_len == httpstate.content_len, "Partial read");

    /* httpstate.content remains available in handler */
    httpstate.post_data[httpstate.content_len] = '\0';
    
    // This assumes that the data that arrived in the POST body is of
    //  multipart/form-data MIME type. We need to change this, if we are to
    //  support things such as HTTP file transfer.
    if (httpstate.mode & CYG_HTTPD_MODE_FORM_DATA)
        cyg_httpd_store_form_data(httpstate.post_data);

    handler h = cyg_httpd_find_handler();
    if (h != 0)
    {
        // A handler was found. We'll call the function associated to it.
        h(&httpstate);
        free(httpstate.post_data);
        httpstate.post_data = NULL;
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
        free(httpstate.post_data);
        httpstate.post_data = NULL;
        return;
    }
#endif    

    // No handler of any kind for a post request. Must send 404.
    cyg_httpd_send_error(CYG_HTTPD_STATUS_NOT_FOUND);
    free(httpstate.post_data);
    httpstate.post_data = NULL;
    return;
}


