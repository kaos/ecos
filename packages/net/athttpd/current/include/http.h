/* =================================================================
 *
 *      http.h
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
#ifndef __HTTP_H__
#define __HTTP_H__

#include <pkgconf/system.h>
#include <pkgconf/isoinfra.h>
#include <cyg/hal/hal_tables.h>

#include <pkgconf/athttpd.h>
#include <cyg/athttpd/auth.h>

#ifdef CYGOPT_NET_ATHTTPD_USE_CGIBIN_TCL
#include <cyg/athttpd/jim.h>
#endif


typedef enum cyg_httpd_req_type
{
    CYG_HTTPD_METHOD_GET = 1,
    CYG_HTTPD_METHOD_HEAD = 2,
    CYG_HTTPD_METHOD_POST = 3
} cyg_httpd_req_type;

#define CYG_HTTPD_MAXURL                    128
#define CYG_HTTPD_MAXPATH                   128

#define CYG_HTTPD_MAXINBUFFER               CYGNUM_ATHTTPD_SERVER_BUFFER_SIZE
#define CYG_HTTPD_MAXOUTBUFFER              CYGNUM_ATHTTPD_SERVER_BUFFER_SIZE

#define CYG_HTTPD_DEFAULT_CGIBIN_OBJLOADER_EXTENSION ".o"
#define CYG_HTTPD_DEFAULT_CGIBIN_TCL_EXTENSION       ".tcl"

#define CYG_HTTPD_TIME_STRING_LEN                 32

#define CYG_HTTPD_STATUS_OK                      200
#define CYG_HTTPD_STATUS_MOVED_PERMANENTLY       301
#define CYG_HTTPD_STATUS_NOT_MODIFIED            304
#define CYG_HTTPD_STATUS_BAD_REQUEST             400
#define CYG_HTTPD_STATUS_NOT_AUTHORIZED          401
#define CYG_HTTPD_STATUS_FORBIDDEN               403
#define CYG_HTTPD_STATUS_NOT_FOUND               404
#define CYG_HTTPD_STATUS_METHOD_NOT_ALLOWED      405
#define CYG_HTTPD_STATUS_SYSTEM_ERROR            500
#define CYG_HTTPD_STATUS_NOT_IMPLEMENTED         501

#define CYG_HTTPD_MODE_CLOSE_CONN             0x0001
#define CYG_HTTPD_MODE_TRANSFER_CHUNKED       0x0002
#define CYG_HTTPD_SEND_HEADER_ONLY            0x0004

// This must be generated at random...
#define CYG_HTTPD_MD5_AUTH_NAME                "MD5"
#define CYG_HTTPD_MD5_AUTH_QOP                 "auth"
#define CYG_HTTPD_MD5_AUTH_OPAQUE              "0000000000000000"

typedef struct __socket_entry
{
    cyg_int32 descriptor;
    time_t    timestamp;
} socket_entry; 

// =============================================================================
// Main HTTP structure.
// =============================================================================
typedef struct
{
    cyg_httpd_req_type method;    
    fd_set       rfds;

    cyg_int32    host[4];

    char         url[CYG_HTTPD_MAXURL+1];
    char         inbuffer[CYG_HTTPD_MAXINBUFFER+1];  
    cyg_int32    inbuffer_len;


    // Packet status.
    //
    //   bit      Description
    // -------------------------------------------------------------------------
    //    0       When set inserts 'Pragma: no-cache' and 
    //             'Cache-control: no-cache' in the header. Default for callback
    //             functions.
    //    1       When to close the connection: 
    //             'Connection: close' when 0, 'Connection: keep-alive' when 1.
    cyg_uint16   mode;
    
    // Ouptut data.
    cyg_uint16   status_code;
    char        *mime_type;
    cyg_int32    payload_len;
    char         outbuffer[CYG_HTTPD_MAXOUTBUFFER+1];

    socket_entry sockets[CYGNUM_FILEIO_NFILE];
    cyg_int32    fdmax;
    
    // Socket handle.
    cyg_int32    client_index;

    time_t       modified_since;
    time_t       last_modified;
    
#ifdef CYGOPT_NET_ATHTTPD_USE_CGIBIN_TCL
    Jim_Interp *jim_interp;
#endif    

    // This pointer points to the information about the domain that needs
    //  to be authenticated. It is only used by the function that builds the
    //  header.
    cyg_httpd_auth_table_entry *needs_auth;

    struct sockaddr_in server_conn;
    struct sockaddr_in client_conn;
} CYG_HTTPD_STATE;

extern CYG_HTTPD_STATE httpstate;

extern const char *day_of_week[];
extern const char *month_of_year[]; 

struct cyg_httpd_mime_table_entry
{
    char *extension;
    char *mime_string;
} CYG_HAL_TABLE_TYPE;

typedef struct cyg_httpd_mime_table_entry cyg_httpd_mime_table_entry;

#define CYG_HTTPD_MIME_TABLE_ENTRY(__name, __pattern, __arg) \
cyg_httpd_mime_table_entry __name CYG_HAL_TABLE_ENTRY(httpd_mime_table) = \
                                                          { __pattern, __arg } 

extern cyg_int32 debug_print;

void cyg_httpd_format_time_string(char*, time_t*);
void cyg_httpd_set_home_page(cyg_int8*);
char* cyg_httpd_find_mime_string(char*);
cyg_int32 cyg_httpd_initialize(void);
void cyg_httpd_cleanup_filename(char*);
char* cyg_httpd_parse_GET(char*);
char* cyg_httpd_parse_POST(char*);
char* cyg_httpd_parse_HEAD(char*);
void cyg_httpd_handle_method_GET(void);
void cyg_httpd_handle_method_HEAD(void);
cyg_int32 cyg_httpd_process_method(void);
void cyg_httpd_send_file(char*);
void cyg_httpd_send_error(cyg_int32);
cyg_int32 cyg_httpd_format_header(void);

#endif
