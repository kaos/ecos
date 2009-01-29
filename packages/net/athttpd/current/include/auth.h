/* =================================================================
 *
 *      auth.h
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
#ifndef __AUTH_H__
#define __AUTH_H__

#include <pkgconf/system.h>
#include <pkgconf/isoinfra.h>
#include <cyg/hal/hal_tables.h>

#define badchar(c,p) (!(p = memchr(b64string, c, 64)))

extern cyg_uint8 *cyg_httpd_current_authName;

typedef enum cyg_httpd_auth_type
{
    CYG_HTTPD_AUTH_BASIC = 0, 
    CYG_HTTPD_AUTH_DIGEST = 1
} cyg_httpd_auth_type;

#define AUTH_STORAGE_BUFFER_LENGTH_LOGIN             32
#define AUTH_STORAGE_BUFFER_LENGTH_PASSWORD          32
#define AUTH_STORAGE_BUFFER_LENGTH        (AUTH_STORAGE_BUFFER_LENGTH_LOGIN +\
                                           AUTH_STORAGE_BUFFER_LENGTH_PASSWORD)
// It must be stressed that the auth_dirname field is the directory name
//  that will be requested by the web server, and _not_ the absolute name
//  in the eCos file system. 
// Lets' make an example. Let's say that the files of your web site reside in 
//  the directory '/fs/jffs2/html'. Your CYG_HTTPD_DEFAULT_PATH will likely be
//  set to '/fs/jffs2/' and the CYG_HTTPD_DEFAULT_HTML_DIR will likely be
//  set to 'html/'. The web server will then request a file 
//  (i.e. 'images/foo.jpg') whose absolute location in the eCos file system is
//  '/fs/jffs2/html/images/foo.jpg', and if we want to protect the 'images/'
//  directory with some kind of authentication, the auth_dirname must be 
//  pointing to a string such as "images/".
struct cyg_httpd_auth_table_entry
{
    char                *auth_dirname;
    char                *auth_domainname;
    char                *auth_username;
    char                *auth_password;
    cyg_httpd_auth_type  auth_mode;
} CYG_HAL_TABLE_TYPE;

typedef struct cyg_httpd_auth_table_entry cyg_httpd_auth_table_entry;
#define CYG_HTTPD_AUTH_TABLE_ENTRY( __name, __path, __domain, __un, __pw, __mode )  \
 cyg_httpd_auth_table_entry __name CYG_HAL_TABLE_ENTRY( httpd_auth_table ) =  \
                             { __path, __domain, __un, __pw, __mode } 

cyg_httpd_auth_table_entry* cyg_httpd_auth_entry_from_path(char *);
cyg_httpd_auth_table_entry* cyg_httpd_auth_entry_from_domain(char *);
cyg_httpd_auth_table_entry* cyg_httpd_is_authenticated(char*);
char* cyg_httpd_digest_data(char *, char *);
char* cyg_httpd_digest_skip(char *);

// The following code is a slightly modified version of those available at the
//  end of RFC1270.
#define HASHLEN 16
typedef char HASH[HASHLEN];
#define HASHHEXLEN 32
typedef char HASHHEX[HASHHEXLEN+1];

//extern char cyg_httpd_md5_username[];
//extern char cyg_httpd_md5_opaque[];
//extern char cyg_httpd_md5_alg[];
//extern char cyg_httpd_md5_realm[];
extern char cyg_httpd_md5_nonce[];
extern char cyg_httpd_md5_cnonce[];
extern char cyg_httpd_md5_digest[];
extern char cyg_httpd_md5_qop[];
extern char cyg_httpd_md5_response[];
extern char cyg_httpd_md5_noncecount[];
extern char cyg_httpd_md5_ha2[];
extern char cyg_httpd_md5_ha1[];

#endif // __AUTH_H__
