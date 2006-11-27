/* =================================================================
 *
 *      http.c
 *
 *      Handles the client requests.
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
#include <cyg/kernel/kapi.h>           // Kernel API.
#include <cyg/infra/diag.h>            // For diagnostic printing.
#include <network.h>
#include <time.h>

#include <cyg/hal/hal_tables.h>
#include <cyg/fileio/fileio.h>
#include <stdio.h>                     // sprintf().
#include <stdlib.h>

#ifdef CYGOPT_NET_ATHTTPD_USE_CGIBIN_OBJLOADER
#include <cyg/objloader/elf.h>
#include <cyg/objloader/objelf.h>
#endif

#include <cyg/athttpd/http.h>
#include <cyg/athttpd/socket.h>
#include <cyg/athttpd/handler.h>
#include <cyg/athttpd/forms.h>

cyg_int32 debug_print = 0;

const char *day_of_week[7] = {"Sun", "Mon", "Tue", "Wed", 
                                 "Thu", "Fri", "Sat"};
const char *month_of_year[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                                    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char *home_pages[] = {"index.html",   "index.htm",
                            "default.html", "home.html"};
CYG_HAL_TABLE_BEGIN(cyg_httpd_mime_table, httpd_mime_table);
CYG_HAL_TABLE_END(cyg_httpd_mime_table_end, httpd_mime_table);

__externC cyg_httpd_mime_table_entry cyg_httpd_mime_table[];
__externC cyg_httpd_mime_table_entry cyg_httpd_mime_table_end[];

// Standard handlers added by default. Correspond to the most used extensions.
// The user can add his/her own later.
CYG_HTTPD_MIME_TABLE_ENTRY(hal_htm_entry, "htm",
                                       "text/html; charset=iso-8859-1");
CYG_HTTPD_MIME_TABLE_ENTRY(hal_html_entry, "html", 
                                       "text/html; charset=iso-8859-1");
CYG_HTTPD_MIME_TABLE_ENTRY(hal_gif_entry, "gif", "image/gif");
CYG_HTTPD_MIME_TABLE_ENTRY(hal_jpg_entry, "jpg", "image/jpg");
CYG_HTTPD_MIME_TABLE_ENTRY(hal_png_entry, "png", "image/png");
CYG_HTTPD_MIME_TABLE_ENTRY(hal_css_entry, "css", "text/css");
CYG_HTTPD_MIME_TABLE_ENTRY(hal_js_entry, "js", "application/x-javascript");

void 
cyg_httpd_send_error(cyg_int32 err_type)
{
    httpstate.status_code = err_type;
    
    // Errors pages close the socket and are never cached.
    httpstate.mode |= CYG_HTTPD_MODE_NO_CACHE;

#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 1
    diag_printf("Sending error: %d\n", err_type);
#endif    

#ifdef CYGOPT_NET_ATHTTPD_USE_FS
    // Check if the user has defines his own error pages.
    struct stat sp;
    char file_name[CYG_HTTPD_MAXPATH];
    strcpy(file_name, CYGDAT_NET_ATHTTPD_SERVEROPT_ROOTDIR);
    if (file_name[strlen(file_name)-1] != '/')
        strcat(file_name, "/");
    strcat(file_name, CYGDAT_NET_ATHTTPD_SERVEROPT_ERRORDIR);
    if (file_name[strlen(file_name)-1] != '/')
        strcat(file_name, "/");
    sprintf(file_name + strlen(file_name), "error_%d.html", err_type);
    cyg_httpd_cleanup_filename(file_name);
    cyg_int32 rc = stat(file_name, &sp);
    if (rc == 0)
    {
        char *extension = rindex(file_name, '.');
        if (extension == NULL)
            // No extension in the file name.
            httpstate.mime_type = 0;
        else
            httpstate.mime_type = cyg_httpd_find_mime_string(++extension);

        httpstate.payload_len  = sp.st_size;
        cyg_int32 header_length = cyg_httpd_format_header();
        cyg_httpd_write(httpstate.outbuffer, header_length);
    
        // File found.
        FILE *fp = fopen(file_name, "r");
        if(fp == NULL)
            return;

        ssize_t payload_size = fread(httpstate.outbuffer, 
                                     1, 
                                     CYG_HTTPD_MAXOUTBUFFER, 
                                     fp);
        while (payload_size > 0)
        {
            ssize_t bytes_written = cyg_httpd_write_chunked(httpstate.outbuffer, 
                                                            payload_size);
            if (bytes_written != payload_size)
                break;

            payload_size = fread(httpstate.outbuffer, 
                                 1, 
                                 CYG_HTTPD_MAXOUTBUFFER, 
                                 fp);
        }

        fclose(fp);
        return;
    }
#endif    

    // Because the size of the frame is not known upfront (every error message
    //  is different and thus has different length) we use chunked frames to
    //  send the message out.
#if defined(CYGOPT_NET_ATHTTPD_CLOSE_CHUNKED_CONNECTIONS)
    httpstate.mode |= CYG_HTTPD_MODE_CLOSE_CONN;
#endif
    
    httpstate.mode |= CYG_HTTPD_MODE_TRANSFER_CHUNKED;
    httpstate.status_code = err_type;
    httpstate.last_modified = -1;
    httpstate.mime_type = "text/html";
    cyg_int32 header_length = cyg_httpd_format_header();
    cyg_httpd_write(httpstate.outbuffer, header_length);
    
    // If no file has been defined, send a simple notification. We must use
    //  chunked frames, because we do not know upfron the length of the
    //  packet we have to send.
    strcpy(httpstate.outbuffer,
           "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n");
    switch (err_type)
    {
    case CYG_HTTPD_STATUS_MOVED_PERMANENTLY:
        strcat(httpstate.outbuffer,
               "<html><head><title>301 Moved Permanently</title></head>\r\n"
               "<body><h1>Moved Permanently</h1>\r\n"
               "<p>The document has moved <a href=\"");
        strcat(httpstate.outbuffer, httpstate.url);
        strcat(httpstate.outbuffer, "\">here</a>.\r\n");
        break;
    case CYG_HTTPD_STATUS_MOVED_TEMPORARILY:
        strcat(httpstate.outbuffer, 
               "<html><head><title>302 Found</title></head>\r\n"
               "<body><h1>Redirect</h1>\r\n"
               "<p>Please continue <a href=\"");
        strcat(httpstate.outbuffer, httpstate.url);
        strcat(httpstate.outbuffer, "\">here</a>.\r\n");
        break;
    case CYG_HTTPD_STATUS_NOT_AUTHORIZED:
        strcat(httpstate.outbuffer, 
               "<html><head><title>401 Not Authorized</title></head>\r\n");
        strcat(httpstate.outbuffer, 
               "<body><p>Authorization required to access this URL.</p>\r\n");
        break;    
    case CYG_HTTPD_STATUS_NOT_MODIFIED:
        cyg_httpd_end_chunked();
        return;
    case CYG_HTTPD_STATUS_NOT_FOUND:
        strcat(httpstate.outbuffer, 
               "<html><head><title>404 Not Found</title></head>\r\n");
        sprintf(httpstate.outbuffer + strlen(httpstate.outbuffer),
                "<p>The requested URL: %s was not found on this server</p>\r\n",
                httpstate.url);
        break;
    case CYG_HTTPD_STATUS_SYSTEM_ERROR:
        strcat(httpstate.outbuffer, 
               "<html><head><title>500 Server Error</title></head>\r\n");
        strcat(httpstate.outbuffer, 
               "<p>The server encountered an unexpected condition that "
               "prevented it from fulfilling the request"
               " by the client</p>\r\n");
        break;
    case CYG_HTTPD_STATUS_NOT_IMPLEMENTED:
        strcat(httpstate.outbuffer, 
               "<html><head><title>501 Not Implemented</title></head>\r\n");
        strcat(httpstate.outbuffer, 
               "<p>The method requested is not implemented</p>\r\n");
        break;
    default:
        strcat(httpstate.outbuffer, 
               "<html><head><title>400 Bad Request</title></head>\r\n");
        strcat(httpstate.outbuffer, 
               "<p>Bad request</p>\r\n");
        break;
    }
    
    sprintf(httpstate.outbuffer + strlen(httpstate.outbuffer),
            "<hr>%s at %d.%d.%d.%d Port %d\r\n</body></html>\r\n",
            CYGDAT_NET_ATHTTPD_SERVEROPT_SERVERID,
            httpstate.host[0],
            httpstate.host[1],
            httpstate.host[2],
            httpstate.host[3],
            CYGNUM_NET_ATHTTPD_SERVEROPT_PORT);
    
    cyg_httpd_write_chunked(httpstate.outbuffer, strlen(httpstate.outbuffer));
    cyg_httpd_end_chunked();
}

// Return a time_t that is always UTC (aka GMT).
time_t
cyg_httpd_parse_date(char *time)
{
    int    i;
    char   month[4];
    struct tm tm_mod;

    // We are going to get rid of the day of the week. This is always the first
    //  part of the string, separated by a blank.
    time = strchr( time, ' ');
    if ( time == NULL)
        return 0;
    time++;

    /// RFC1123. The date is in the format: Sun, 06 Nov 1994 08:49:37 GMT.
    cyg_int32 rc = sscanf(time,
                          "%2d %3s %4d %2d:%2d:%2d GMT",
                          &tm_mod.tm_mday,
                          month,
                          &tm_mod.tm_year,
                          &tm_mod.tm_hour,
                          &tm_mod.tm_min,
                          &tm_mod.tm_sec);
    if (rc != 6)
    {
        // RFC1036. The date is in the format: Sunday, 06-Nov-94 08:49:37 GMT.
        rc = sscanf(time,
                    "%2d-%3s-%2d %2d:%2d:%2d GMT",
                    &tm_mod.tm_mday,
                    month,
                    &tm_mod.tm_year,
                    &tm_mod.tm_hour,
                    &tm_mod.tm_min,
                    &tm_mod.tm_sec);
        if (rc != 6)
        {
            // asctime() in the stdlibc library.
            // The date is in the format: Sun Nov 6 08:49:37 1994
            //  and needs to be converted to GMT.
            rc = sscanf(time,"%3s %2d %2d:%2d:%2d %4d",
                        month,
                        &tm_mod.tm_mday,
                        &tm_mod.tm_hour,
                        &tm_mod.tm_min,
                        &tm_mod.tm_sec,
                        &tm_mod.tm_year);
            if (rc != 6)
                return 0;
        }
    }

    for (i = 0; i < sizeof(month_of_year); i++)
        if (strcmp(month, month_of_year[i]) == 0)
        {
            tm_mod.tm_mon = i;
            if (tm_mod.tm_year > 1900)
                tm_mod.tm_year -= 1900;
            return mktime(&tm_mod);
        }

    return 0;
}

// Finds the mime string into the mime_table associated with a specific 
//  extension. Returns the MIME type to send in the header, or NULL if the
//  extension is not in the table.
char*
cyg_httpd_find_mime_string(char *ext)
{
    cyg_httpd_mime_table_entry *entry = cyg_httpd_mime_table;

    while (entry != cyg_httpd_mime_table_end)
    {
        if (!strcmp((const char*)ext, entry->extension))
            return entry->mime_string;
        entry++;
    }
            
    return (char*)0;
}

void
cyg_httpd_cleanup_filename(char *filename)
{
    char *src = strstr(filename, "//");
    while (src != 0)
    {
        strcpy(src + 1, src + 2);
        src = strstr(filename, "//");
    }

    src = strstr(filename, "/./");
    while (src != 0)
    {
        strcpy(src + 1, src + 3);
        src = strstr(filename, "/./");
    }

    src = strstr(filename, "/../");
    while (src != 0)
    {
        char *comp1 = filename, *comp2 = filename;

        // Search the path component before this redirection.
        while ((comp1 = strchr(comp1, '/')) != src)
            comp2 = ++comp1;

        strcpy(comp2, src + 4);
        src = strstr(filename, "/../");
    }
}

cyg_int32
cyg_httpd_initialize(void)
{
    httpstate.post_data = NULL;
    httpstate.needs_auth = (cyg_httpd_auth_table_entry *)0;
    return 0;
}

void
cyg_httpd_append_homepage(char *root)
{
#ifdef CYGOPT_NET_ATHTTPD_USE_FS
    struct stat sp;
    cyg_int32 i;

    cyg_int32 root_len = strlen(root);
    for (i = 0; i < sizeof(home_pages)/sizeof(char*); i++)
    {
        root[root_len] = '\0';
        sprintf(root + root_len, "%s", home_pages[i]);
        cyg_int32 rc = stat(root, &sp);
        if (rc == 0)
            return;
    }
    root[root_len] = 0;
#endif    
    
#ifdef CYGDAT_NET_ATHTTPD_ALTERNATE_HOME    
    if (strcmp(root, "/") == 0)
        // The client is trying to open the main index file.
        strcat(root, CYGDAT_NET_ATHTTPD_ALTERNATE_HOME);
#endif    
}

#ifdef CYGOPT_NET_ATHTTPD_USE_FS
void
cyg_httpd_send_file(char *name)
{
    cyg_int32  err;
    FILE      *fp;
    struct stat sp;
    char       file_name[CYG_HTTPD_MAXPATH];

#ifdef CYGOPT_NET_ATHTTPD_USE_AUTH
    // Let's check that the requested URL is not inside some directory that 
    //  needs authentication.
    cyg_httpd_auth_table_entry* auth = cyg_httpd_is_authenticated(name);
    if (auth != 0)
    {
        cyg_httpd_send_error(CYG_HTTPD_STATUS_NOT_AUTHORIZED);
        return;
    }
#endif

    strcpy(file_name, CYGDAT_NET_ATHTTPD_SERVEROPT_ROOTDIR);
    if (file_name[strlen(file_name)-1] != '/')
        strcat(file_name, "/");
    strcat(file_name, name);
    cyg_httpd_cleanup_filename(file_name);
        
    // Check if the file is in the file system. This will also give us the
    //  size of the file, to be used in the HTTP header.
    cyg_int32 rc = stat(file_name, &sp);
    if (rc < 0)
    {
        // Before giving up, we make a last ditch attempt at finding a file
        //  within the internal resources of the server. The user can add
        //  his/her own files to the table.
        cyg_httpd_ires_table_entry *p = cyg_httpd_find_ires(name);
        if (p != NULL)
        {
#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 1
            diag_printf("Sending Internal Resource: %s\n", name);
#endif    
            cyg_httpd_send_ires(p);
        }    
        else    
            cyg_httpd_send_error(CYG_HTTPD_STATUS_NOT_FOUND);
        return;
    }
    
    if (S_ISDIR(sp.st_mode))
    {
        char tmp_url[CYG_HTTPD_MAXURL];
        strcpy(tmp_url, httpstate.url);
        // Directories need a trialing slash, and if missing, we'll redirect
        //  the client to the right URL. This is called (appropriately
        //  enough) "Trailing-Slash Redirection". 
        if (name[strlen(name)-1] != '/')
        {
            if (CYGNUM_NET_ATHTTPD_SERVEROPT_PORT == 80)
                sprintf(httpstate.url,
                        "http://%d.%d.%d.%d%s/",
                        httpstate.host[0],
                        httpstate.host[1],
                        httpstate.host[2],
                        httpstate.host[3],
                        tmp_url);
            else            
                sprintf(httpstate.url,
                        "http://%d.%d.%d.%d:%d%s/",
                        httpstate.host[0],
                        httpstate.host[1],
                        httpstate.host[2],
                        httpstate.host[3],
                        CYGNUM_NET_ATHTTPD_SERVEROPT_PORT,
                        tmp_url);
            cyg_httpd_send_error(CYG_HTTPD_STATUS_MOVED_PERMANENTLY);
            return;
        }

        // We are going to try to locate an index page in the directory we got
        //  in the URL. 
        cyg_httpd_append_homepage(file_name);
        if (file_name[strlen(file_name)-1] == '/')
        {
#ifdef CYGOPT_NET_ATHTTPD_USE_DIRLIST
            // No home page found, we are sending a directory listing.
            cyg_httpd_send_directory_listing(name);
#else
            cyg_httpd_send_error(CYG_HTTPD_STATUS_NOT_FOUND);
#endif
            return;
        }
        stat(file_name, &sp);
    }
    
    httpstate.last_modified = sp.st_mtime;

    // Let's see if we luck out and can send a 304.
    if ((httpstate.modified_since != -1) && 
                   (httpstate.modified_since >= httpstate.last_modified))
    {                   
        cyg_httpd_send_error(CYG_HTTPD_STATUS_NOT_MODIFIED);
        return;
    }    
    else    
        httpstate.status_code = CYG_HTTPD_STATUS_OK;

    // Here we'll look for extension to the file. Consider the case where
    //  there might be more than one dot in the file name. We'll look for
    //  the last dot, then we'll check the extension.
    char *extension = rindex(file_name, '.');
    if (extension == NULL)
        httpstate.mime_type = 0;
    else    
        httpstate.mime_type = cyg_httpd_find_mime_string(++extension);

    httpstate.payload_len  = sp.st_size;
    httpstate.mode &= ~CYG_HTTPD_MODE_NO_CACHE;
    cyg_int32 payload_size = cyg_httpd_format_header();
    if ((httpstate.mode & CYG_HTTPD_MODE_SEND_HEADER_ONLY) != 0)
    {                 
#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 1
    diag_printf("Sending header only for URL: %s\n", file_name);
#endif    
        send(httpstate.sockets[httpstate.client_index].descriptor, 
             httpstate.outbuffer, 
             payload_size,
             0);
        return;
    }

#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 1
    diag_printf("Sending file: %s\n", file_name);
#endif    
    fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        // We should really read errno and send messages accordingly...
        cyg_httpd_send_error(CYG_HTTPD_STATUS_SYSTEM_ERROR);
        return;
    }
    
    // Fill up the rest of the buffer and send it out.
    cyg_int32 bread = fread(httpstate.outbuffer + strlen(httpstate.outbuffer),
                            1, 
                            CYG_HTTPD_MAXOUTBUFFER - payload_size,
                            fp);
    cyg_httpd_write(httpstate.outbuffer, payload_size + bread);

    ssize_t bytes_written = 0;
    sp.st_size -= bread;
    while (bytes_written < sp.st_size)
    {
        bread = fread(httpstate.outbuffer, 1, CYG_HTTPD_MAXOUTBUFFER, fp);
        bytes_written += cyg_httpd_write(httpstate.outbuffer, bread);
    }    
    
    err = fclose(fp);
    if(err < 0)
        cyg_httpd_send_error(CYG_HTTPD_STATUS_SYSTEM_ERROR);
}
#endif

cyg_int32
cyg_httpd_format_header(void)
{
    sprintf(httpstate.outbuffer, "HTTP/1.1 %d", httpstate.status_code);
    time_t time_val = time(NULL);
    
    // Error messages (i.e. with status other than OK, automatically add
    //  the no-cache header.
    switch (httpstate.status_code)
    {
    case CYG_HTTPD_STATUS_MOVED_PERMANENTLY:
        strcat(httpstate.outbuffer, " Moved Permanently\r\n");
        strcat(httpstate.outbuffer, "Location: ");
        strcat(httpstate.outbuffer, httpstate.url);
        strcat(httpstate.outbuffer, "\r\n");
        sprintf(httpstate.outbuffer + strlen(httpstate.outbuffer),
                "Content-Length: %d\r\n",
                httpstate.payload_len);
        break;
    case CYG_HTTPD_STATUS_MOVED_TEMPORARILY:
        strcat(httpstate.outbuffer, " Found\r\n");
        strcat(httpstate.outbuffer, "Location: ");
        strcat(httpstate.outbuffer, httpstate.url);
        strcat(httpstate.outbuffer, "\r\n");
        sprintf(httpstate.outbuffer + strlen(httpstate.outbuffer),
                "Content-Length: %d\r\n",
                httpstate.payload_len);
        break;
#ifdef CYGOPT_NET_ATHTTPD_USE_AUTH
    case CYG_HTTPD_STATUS_NOT_AUTHORIZED:
        // A 401 error closes the connection right away.
        httpstate.mode |= CYG_HTTPD_MODE_CLOSE_CONN;
        strcat(httpstate.outbuffer, " Not Authorized\r\n");
        
        // Here we should set the proper header based on the authentication
        //  required (httpstate.needs_authMode) but for now, with only
        //  Basic Authentication supported, there is no need to do so.
        if (httpstate.needs_auth->auth_mode == CYG_HTTPD_AUTH_BASIC)
        {
            sprintf(httpstate.outbuffer + strlen(httpstate.outbuffer),
                    "WWW-Authenticate: Basic realm=\"%s\"\r\n",
                    httpstate.needs_auth->auth_domainname);
        }
        else             
        {
            sprintf(httpstate.outbuffer + strlen(httpstate.outbuffer),
                     "WWW-Authenticate: Digest realm=\"%s\" ",
                     httpstate.needs_auth->auth_domainname);
            strftime(cyg_httpd_md5_nonce, 
                     33,
                     TIME_FORMAT_RFC1123,
                     gmtime(&time_val));
            sprintf(httpstate.outbuffer + strlen(httpstate.outbuffer),
                    "nonce=\"%s\" ", cyg_httpd_md5_nonce);
            sprintf(httpstate.outbuffer + strlen(httpstate.outbuffer),
                    "opaque=\"%s\", ", 
                    CYG_HTTPD_MD5_AUTH_OPAQUE);
            sprintf(httpstate.outbuffer + strlen(httpstate.outbuffer),
                    "stale=false, algorithm=%s, qop=\"%s\"\r\n",
                    CYG_HTTPD_MD5_AUTH_NAME,
                    CYG_HTTPD_MD5_AUTH_QOP);
        }
        break;
#endif
    case CYG_HTTPD_STATUS_NOT_MODIFIED:
        strcat(httpstate.outbuffer, " Not Modified\r\n");
        break;
    case CYG_HTTPD_STATUS_NOT_FOUND:
        strcat(httpstate.outbuffer, " Not Found\r\n");
        sprintf(httpstate.outbuffer + strlen(httpstate.outbuffer),
                "Content-Length: %d\r\n", 
                httpstate.payload_len);
        break;
    case CYG_HTTPD_STATUS_METHOD_NOT_ALLOWED:
        strcat(httpstate.outbuffer, " Method Not Allowed\r\n");
        break;
    default:
        strcat(httpstate.outbuffer, " OK\r\n");
        if ((httpstate.mode & CYG_HTTPD_MODE_TRANSFER_CHUNKED) == 0)
            sprintf(httpstate.outbuffer + strlen(httpstate.outbuffer),
                    "Content-Length: %d\r\n", 
                    httpstate.payload_len);
        break;
    }

    strcat(httpstate.outbuffer, "Date: ");
    strftime(httpstate.outbuffer + strlen(httpstate.outbuffer), 
             CYG_HTTPD_MAXOUTBUFFER - strlen(httpstate.outbuffer),
             TIME_FORMAT_RFC1123,
             gmtime(&time_val));
    strcat(httpstate.outbuffer, "\r\n");
    
    sprintf(httpstate.outbuffer + strlen(httpstate.outbuffer), 
            "Server: %s\r\n", 
            CYGDAT_NET_ATHTTPD_SERVEROPT_SERVERID);
    
    if (httpstate.mode & CYG_HTTPD_MODE_CLOSE_CONN)
        strcat(httpstate.outbuffer, "Connection: close\r\n");
    else
        strcat(httpstate.outbuffer, "Connection: keep-alive\r\n");

    // When we cannot find the appropriate MIME type, we'll send a default type.
    if (httpstate.mime_type == 0)
        httpstate.mime_type = CYGDAT_NET_ATHTTPD_DEFAULT_MIME_TYPE;
    sprintf(httpstate.outbuffer + strlen(httpstate.outbuffer),
            "Content-Type: %s\r\n", 
            httpstate.mime_type);

    if (httpstate.mode & CYG_HTTPD_MODE_TRANSFER_CHUNKED)
        strcat(httpstate.outbuffer, "Transfer-Encoding: chunked\r\n");

    if (httpstate.mode & CYG_HTTPD_MODE_NO_CACHE)
        strcat(httpstate.outbuffer, "Cache-Control: no-cache\r\n");
        
    if (httpstate.last_modified != -1)
    {
        time_val = httpstate.last_modified;
        strcat(httpstate.outbuffer, "Last-Modified: "); 
        strftime(httpstate.outbuffer + strlen(httpstate.outbuffer), 
                 CYG_HTTPD_MAXOUTBUFFER - strlen(httpstate.outbuffer),
                 TIME_FORMAT_RFC1123,
                 gmtime(&time_val));
        strcat(httpstate.outbuffer, "\r\n");

#if (CYGOPT_NET_ATHTTPD_DOCUMENT_EXPIRATION_TIME != 0)                 
        time_val += CYGOPT_NET_ATHTTPD_DOCUMENT_EXPIRATION_TIME;
        strcat(httpstate.outbuffer, "Expires: "); 
        strftime(httpstate.outbuffer + strlen(httpstate.outbuffer), 
                 CYG_HTTPD_MAXOUTBUFFER - strlen(httpstate.outbuffer),
                 TIME_FORMAT_RFC1123,
                 gmtime(&time_val));
        strcat(httpstate.outbuffer, "\r\n");
#endif
    }        
                 
    // There must be 2 carriage returns between the header and the body, 
    //  so if you modify this function make sure that there is another 
    //  CRLF already terminating the buffer thus far.
    strcat(httpstate.outbuffer, "\r\n");
    return strlen(httpstate.outbuffer);
}

void
cyg_httpd_handle_method_GET(void)
{
    // Use defined handlers take precedence over other forms of response.
    handler h = cyg_httpd_find_handler();
    if (h != 0)
    {
        h(&httpstate);
        return;
    }
    
#ifdef CYGOPT_NET_ATHTTPD_USE_CGIBIN_OBJLOADER
    // If the URL is a CGI script, there is a different directory...
    if (httpstate.url[0] == '/' &&
                    !strncmp(httpstate.url + 1, 
                              CYGDAT_NET_ATHTTPD_SERVEROPT_CGIDIR, 
                              strlen(CYGDAT_NET_ATHTTPD_SERVEROPT_CGIDIR)))
    {                              
        cyg_httpd_exec_cgi();
        return;
    }
    // If the OBJLOADER package is not loaded, then the request for a library
    //  will likely generate a 404.
#endif    

#ifdef CYGOPT_NET_ATHTTPD_USE_FS
    // No handler, we'll redirect to the file system.
    cyg_httpd_send_file(httpstate.url);
#else
    // If we do not have a file system, we look for the file within the 
    //  internal resources of the server. The user can add his/her own files
    //  to the table.
    if (strcmp(httpstate.url, "/") == 0)
    {
        int i;
        cyg_httpd_ires_table_entry *p;
        for (i = 0; i < sizeof(home_pages)/sizeof(char*); i++)
        {
            httpstate.url[1] = '\0';
            strcat(httpstate.url, home_pages[i]);
            p = cyg_httpd_find_ires(httpstate.url);
            if (p != NULL)
            {
                cyg_httpd_send_ires(p);
                return;
            }    
        }        
    }
    else
    {
        cyg_httpd_ires_table_entry *p = cyg_httpd_find_ires(httpstate.url);
        if (p != NULL)
        {
            cyg_httpd_send_ires(p);
            return;
        }    
    }        
    cyg_httpd_send_error(CYG_HTTPD_STATUS_NOT_FOUND);
#endif    
}

char*
cyg_httpd_get_URL(char* p)
{
    char* dest = httpstate.url;

    // First get rid of multiple leading slashes.
    while ((p[0] == '/') && (p[1] == '/'))
       p++;

    // Store the url, and check if there is a form result in it.
    while ((*p != ' ') && (*p != '?') &&
            ((dest - httpstate.url) <= CYG_HTTPD_MAXURL))
    {
        // Look for encoded characters in the URL.
        if (*p == '%') 
        {
            p++;
            if (*p) 
                *dest = cyg_httpd_from_hex(*p++) * 16;
            if (*p) 
                *dest += cyg_httpd_from_hex(*p++);
        }
        else 
            *dest++ = *p++;
    }

    // Terminate the file name...
    *dest = '\0';

    // The URL must start with a leading slash.
    if (httpstate.url[0] != '/') 
    {
        cyg_httpd_send_error(CYG_HTTPD_STATUS_BAD_REQUEST);
        return (char*)0;
    }
    return p;
}

char*
cyg_httpd_parse_POST(char* p)
{
    httpstate.method = CYG_HTTPD_METHOD_POST;
    httpstate.mode &= ~CYG_HTTPD_MODE_SEND_HEADER_ONLY;
    char *cp = cyg_httpd_get_URL(p);
    if (cp == 0)
        return (char*)0;
#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 1
    diag_printf("POST Request URL: %s\n", httpstate.url);
#endif    

    while (*cp++ != '\n');
    return cp;
}

char*
cyg_httpd_parse_GET(char* p)
{
    char *cp = cyg_httpd_get_URL(p);
    if (cp == 0)
        return 0;
#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 1
    if ( httpstate.method == CYG_HTTPD_METHOD_GET)
        diag_printf("GET Request URL: %s\n", httpstate.url);
    else    
        diag_printf("HEAD Request URL: %s\n", httpstate.url);
#endif    

    if (*cp == '?')
        // If we have a GET header with form variables we'll get the
        //  variables out of it and store them in the variable table.
        // Can we assume that HEAD request can have form variables?
        // That will be a yes until I learn otherwise.
        cp = cyg_httpd_store_form_data(++cp);

    // Run to end of line.
    while (*cp++ != '\n');
    return cp;
}

char*
cyg_httpd_process_header(char *p)
{
    // The deafult for HTTP 1.1 is keep-alive connections, unless specifically
    //  closed by the far end.
    httpstate.mode &= ~(CYG_HTTPD_MODE_CLOSE_CONN | CYG_HTTPD_MODE_FORM_DATA);
    httpstate.modified_since = -1;
    httpstate.content_len = 0;
    while ((*p != '\r') && (*p != '\n') & (*p != '\0'))
    {
        if (strncasecmp("GET ", p, 4) == 0)
        {
            // We need separate flags for HEAD and SEND_HEADERS_ONLY since
            //  we can send a header only even in the case of a GET request
            //  (as a 304 response.)
            httpstate.method = CYG_HTTPD_METHOD_GET;
            httpstate.mode &= ~CYG_HTTPD_MODE_SEND_HEADER_ONLY;
            p = cyg_httpd_parse_GET(p + 4);
            if (p ==0)
                return (char*)0;
        }
        else if (strncasecmp("POST ", p, 5) == 0)
        {
            p = cyg_httpd_parse_POST(p + 5);
            if (p ==0)
                return (char*)0;
        }
        else if (strncasecmp("HEAD ", p, 5) == 0)
        {
            httpstate.method = CYG_HTTPD_METHOD_HEAD;
            httpstate.mode |= CYG_HTTPD_MODE_SEND_HEADER_ONLY;
            p = cyg_httpd_parse_GET(p + 5);
            if (p ==0)
                return (char*)0;
        }
        else if (strncasecmp(p, "Content-Length: ", 16) == 0)
        {
            p = strchr(p, ':') + 2;
            if (p)
                // In the case of a POST request, this is the total length of
                //  the payload, which might be spread across several frames.
                httpstate.content_len = atoi(p);
            while (*p++ != '\n');
        }
        else if (strncasecmp(p, "Content-Type: ", 14) == 0)
        {
            p = strchr(p, ':') + 2;
            if (p)
                // In the case of a POST request, this is the total length of
                //  the payload, which might be spread across several frames.
                if (strncasecmp(p,
                                "application/x-www-form-urlencoded",
                                33) == 0)
                    httpstate.mode |= CYG_HTTPD_MODE_FORM_DATA;
            while (*p++ != '\n');
        }
        else if (strncasecmp("Host:", p, 5) == 0)
        {
            p += 5;
            if (*p == ' ')
                p++;
            sscanf(p,
                   "%d.%d.%d.%d",
                   &httpstate.host[0],
                   &httpstate.host[1],
                   &httpstate.host[2],
                   &httpstate.host[3]);
            while (*p++ != '\n');
        }
        else if (strncasecmp("If-Modified-Since:", p, 18) == 0)
        {
            p += 18;
            if ( *p == ' ')
                p++;
            httpstate.modified_since = cyg_httpd_parse_date(p);
            while (*p++ != '\n');
        }
#ifdef CYGOPT_NET_ATHTTPD_USE_AUTH
        else if (strncasecmp("Authorization:", p, 14) == 0)
        {
            p += 14;
            while (*p == ' ')
                p++;
            if (strncasecmp("Basic", p, 5) == 0)
            {
                char *cr = cyg_httpd_md5_digest;
                p += 5;
                while (*p == ' ')
                    p++;
                while ((*p != '\r') && (*p != '\n') && (*p != ' '))
                    *cr++ = *p++;
                *cr = '\0';
            }
            else if (strncasecmp(p, "Digest", 6) == 0)
            {
                p += 6;
                while (*p == ' ')
                   p++;
                while ((*p != '\r') && (*p != '\n'))
                {
                    if (strncasecmp(p, "realm=", 6) == 0)
                        p = cyg_httpd_digest_skip(p + 6);
                    else if (strncasecmp(p, "username=", 9) == 0)
                        p = cyg_httpd_digest_skip(p + 9);
                    else if (strncasecmp(p, "nonce=", 6) == 0)
                        p = cyg_httpd_digest_skip(p + 6);
                    else if (strncasecmp(p, "response=", 9) == 0)
                        p = cyg_httpd_digest_data(cyg_httpd_md5_response, 
                                                  p + 9);
                    else if (strncasecmp(p, "cnonce=", 7) == 0)
                        p = cyg_httpd_digest_data(cyg_httpd_md5_cnonce, p + 7);
                    else if (strncasecmp(p, "qop=", 4) == 0)
                        p = cyg_httpd_digest_skip(p + 4);
                    else if (strncasecmp(p, "nc=", 3) == 0)
                        p = cyg_httpd_digest_data(cyg_httpd_md5_noncecount, 
                                                  p + 3);
                    else if (strncasecmp(p, "algorithm=", 10) == 0)
                        p = cyg_httpd_digest_skip(p + 10);
                    else if (strncasecmp(p, "opaque=", 7) == 0)
                        p = cyg_httpd_digest_skip(p + 7);
                    else if (strncasecmp(p, "uri=", 4) == 0)
                        p = cyg_httpd_digest_skip(p + 4);
                }
            }    
            else
                while (*p++ != '\n');
        }   
#endif // CYGOPT_NET_ATHTTPD_USE_AUTH
        else if (strncasecmp(p, "Connection:", 11) == 0)
        {
            p += 11;
            while (*p == ' ')
               p++;
            if (strncasecmp(p, "close", 5) == 0)
                httpstate.mode |= CYG_HTTPD_MODE_CLOSE_CONN;
            while (*p++ != '\n');
        }
        else
            // We'll just dump the rest of the line and move on to the next.
            while (*p++ != '\n');
    }
    
    if (*p == '\0')
    {
        // This is the case of a header that is split in two or more frames.
        // We cannot process it right away and will have to wait for the rest
        //  of the data. This has _major_ implications because we implicitly
        //  and tacitly assume that the next frame that will be handled by the
        //  server is the continuation of this one. But if the next frame is
        //  for instance, a request from another client, we are in trouble
        //  since the new request will be processed and this request will
        //  be dropped.
        // Caveat: This is all untested. While theoretically this is possible,
        //  as much as I tried, I could not coerce any of the popular browser
        //  to split a header in multiple frames.
#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 1
        diag_printf("Split header found.\r\n");
#endif        
        return 0;
    }    

    // If this is the end of this request, but there might be other queued up
    //  because of pipelining of two requests in a single frame. This while()
    //  will get rid of the \r\n that terminates the header section of a
    //  request.
    while (*p++ != '\n');
    
    // In the case of large POST the payload comes with the header (and
    //  possibly further frames.) Here is where we mark the start of the
    //  POST data.
    httpstate.header_end = p;
    return p;
}

void
cyg_httpd_process_method(void)
{
    char* p = httpstate.inbuffer;
    
    // Some browsers send an extra '\r\n' after the POST data that is not
    //  accounted in the "Content-Length:" field. We are going to junk all
    //  the leading returns and line carriages we find.
    while ((*p == '\r') || (*p =='\n'))
        p++;
    while (*p != '\0')
    {
        p = cyg_httpd_process_header(p);
        if (p == 0)
            return;
        
        switch (httpstate.method)
        {
            case CYG_HTTPD_METHOD_GET:
            case CYG_HTTPD_METHOD_HEAD:
                cyg_httpd_handle_method_GET();
                break;
            case CYG_HTTPD_METHOD_POST:
                cyg_httpd_handle_method_POST();
                return;
                break;
            default:
                cyg_httpd_send_error(CYG_HTTPD_STATUS_NOT_IMPLEMENTED);
                return;
            break;
        }
    }    
}

