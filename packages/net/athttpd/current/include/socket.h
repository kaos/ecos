/* =================================================================
 *
 *      socket.h
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
#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <pkgconf/system.h>

#define CYG_HTTPD_SOCKET_IDLE_TIMEOUT          300 // In seconds

extern cyg_thread   cyg_httpd_thread_object;
extern cyg_handle_t cyg_httpd_thread_handle;
extern cyg_uint8    cyg_httpd_thread_stack[];


void cyg_httpd_daemon(cyg_addrword_t);
void cyg_httpd_start(void);
ssize_t cyg_httpd_write(char*, int);
ssize_t cyg_httpd_start_chunked(char*);
ssize_t cyg_httpd_write_chunked(char*, int);
void cyg_httpd_end_chunked(void);
cyg_int32 cyg_httpd_socket_setup(void);
cyg_int32 cyg_httpd_socket_handle_request(void);
ssize_t cyg_httpd_writev(cyg_iovec*, int );
#endif
