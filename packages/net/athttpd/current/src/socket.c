/* =================================================================
 *
 *      socket.c
 *
 *      Opens socket and starts the daemon.
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
 *  Contributors: Sergei Gavrikov (w3sg@SoftHome.net), 
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
#include <cyg/kernel/ktypes.h>         // base kernel types.
#include <cyg/infra/diag.h>            // For diagnostic printing.
#include <network.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <stdio.h>                     // sprintf().
#include <time.h>                      // sprintf().

#include <cyg/athttpd/http.h>
#include <cyg/athttpd/socket.h>
#include <cyg/athttpd/cgi.h>

#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define CYG_HTTPD_DAEMON_STACK_SIZE (CYGNUM_HAL_STACK_SIZE_MINIMUM + \
                                          CYGNUM_NET_ATHTTPD_THREADOPT_STACKSIZE)
static cyg_int32 cyg_httpd_initialized = 0;
cyg_thread   cyg_httpd_thread_object;
cyg_handle_t cyg_httpd_thread_handle;
cyg_uint8    cyg_httpd_thread_stack[CYG_HTTPD_DAEMON_STACK_SIZE]     
                                       __attribute__((__aligned__ (16)));
CYG_HTTPD_STATE httpstate;
                                         
__inline__ ssize_t
cyg_httpd_write(char* buf, int buf_len)
{
    // We are not going to write anything in case
    ssize_t sent = send(httpstate.sockets[httpstate.client_index].descriptor, 
                        buf, 
                        buf_len,
                        0);
    return sent;
}

__inline__ ssize_t
cyg_httpd_writev(cyg_iovec *iovec_bufs, int count)
{
    int i;
    ssize_t sent = writev(httpstate.sockets[httpstate.client_index].descriptor, 
                          iovec_bufs, 
                          count);
    ssize_t buf_len = 0;
    for (i = 0; i < count; i++)
        buf_len += iovec_bufs[i].iov_len;
#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 1
    if (sent != buf_len)
        diag_printf("writev() did not send out all bytes (%ld of %ld)\n", 
                    sent,
                    buf_len);
#endif    
    return sent;
}
    
// The need for chunked transfers arises from the fact that with persistent
//  connections it is not always easy to tell when a packet end. Also, with
//  dynamic pages it is not always possible to know the packet size upfront,
//  and thus the value of the 'Content-Length:' field in the header is not
//  known upfront.
// Today's web browser use 'Content-Length:' when present in the header and 
//  when not present they read everything that comes in up to the last 2 \r\n
//  and then figure it out. The HTTP standard _mandates_ 'Content-Length:' to
//  be present in the header with a correct value, and whenever that is not
//  possible, chunked transfers must be used.
//
// A chunked transer takes the form of:
// -----------------------------------------------------------------------------
//    cyg_httpd_start_chunked("html");
//    sprintf(phttpstate->payload, ...);             
//    cyg_httpd_write_chunked(phttpstate->payload, strlen(phttpstate->payload));
//    ...                         
//    cyg_httpd_end_chunked();
// -----------------------------------------------------------------------------
ssize_t
cyg_httpd_start_chunked(char *extension)
{
    httpstate.status_code = CYG_HTTPD_STATUS_OK;

#if defined(CYGOPT_NET_ATHTTPD_CLOSE_CHUNKED_CONNECTIONS)
    // I am not really sure that this is necessary, but even if it isn't, the
    //  added overhead is not such a big deal. In simple terms, I am not sure 
    //  how much I can rely on the client to understand that the frame has ended 
    //  with the last 5 bytes sent out. In an ideal world, the data '0\r\n\r\n'
    //  should be enough, but several posting on the subject I read seem to
    //  imply otherwise, at least with early generation browsers that supported
    //  the "Transfer-Encoding: chunked" mechanism. Things might be getting 
    //  better now but I snooped some sites that use the chunked stuff (Yahoo!
    //  for one) and all of them with no exception issue a "Connection: close" 
    //  on chunked frames even if there is nothing in the HTTP 1.1 spec that
    //  requires it.
    httpstate.mode |= CYG_HTTPD_MODE_CLOSE_CONN;
#endif
    
    // We do not cache chunked frames. In case they are used to display dynamic
    //  data we want them to be executed every time they are requested.
    httpstate.mode |= 
              (CYG_HTTPD_MODE_TRANSFER_CHUNKED | CYG_HTTPD_MODE_NO_CACHE);
    
    httpstate.last_modified = -1;
    httpstate.mime_type = cyg_httpd_find_mime_string(extension);
    cyg_int32 header_length = cyg_httpd_format_header();
    return cyg_httpd_write(httpstate.outbuffer, header_length);
}

ssize_t
cyg_httpd_write_chunked(char* buf, int len)
{
    if (len == 0)
         return 0;

    char leader[16], trailer[] = {'\r', '\n'};
    cyg_iovec iovec_bufs[] = { {leader, 0}, {buf, len}, {trailer, 2} };
    iovec_bufs[0].iov_len = sprintf(leader, "%x\r\n", len);
    if (httpstate.mode & CYG_HTTPD_MODE_SEND_HEADER_ONLY)
        return (iovec_bufs[0].iov_len + len + 2);
    return cyg_httpd_writev(iovec_bufs, 3);
}

void
cyg_httpd_end_chunked(void)
{
    httpstate.mode &= ~CYG_HTTPD_MODE_TRANSFER_CHUNKED;
    if ((httpstate.mode & CYG_HTTPD_MODE_SEND_HEADER_ONLY) != 0)
        return;
    strcpy(httpstate.outbuffer, "0\r\n\r\n");
    cyg_httpd_write(httpstate.outbuffer, 5);
}    

// This function builds and sends out a standard header. It is likely going to
//  be used by a c language callback function, and thus followed by one or
//  more calls to cyg_httpd_write(). Unlike cyg_httpd_start_chunked(), this
//  call requires prior knowledge of the final size of the frame (browsers
//  _will_trust_ the "Content-Length:" field when present!), and the user 
//  is expected to make sure that the total number of bytes (octets) sent out
//  via 'cyg_httpd_write()' matches the number passed in the len parameter.
// Its use is thus more limited, and the more flexible chunked frames should 
//  be used whenever possible.
void
cyg_httpd_create_std_header(char *extension, int len)
{
    httpstate.status_code = CYG_HTTPD_STATUS_OK;
    httpstate.mode |= CYG_HTTPD_MODE_NO_CACHE;

    // We do not want to send out a "Last-Modified:" field for c language
    //  callbacks.
    httpstate.last_modified = -1;
    httpstate.mime_type = cyg_httpd_find_mime_string(extension);
    httpstate.payload_len = len;
    cyg_int32 header_length = cyg_httpd_format_header();
    cyg_httpd_write(httpstate.outbuffer, header_length);
}

void
cyg_httpd_process_request(cyg_int32 index)
{
    httpstate.client_index = index;
    cyg_int32 descr = httpstate.sockets[index].descriptor;

    // By placing a terminating '\0' not only we have a safe stopper point
    //  for our parsing, but also we can detect if we have a split header.
    // Since headers always end with an extra '\r\n', if we find a '\0'
    //  before the terminator than we can safely assume that the header has
    //  not been received completely and more is following (i.e. split headers.)
    httpstate.inbuffer[0] = '\0';
    httpstate.inbuffer_len = 0;

    cyg_bool done = false;
    do
    {   
        // At this point we know we have data pending because the corresponding
        //  bit in the fd_set structure was set.
        int len = recv(descr,
                       httpstate.inbuffer + httpstate.inbuffer_len,
                       CYG_HTTPD_MAXINBUFFER - httpstate.inbuffer_len,
                       0);
        if (len == 0)
        {
            // This is the client that has closed its TX socket, possibly as
            //  a response from a shutdown() initiated by the server. Another
            //  possibility is that the client was closed altogether, in
            //  which case the client sent EOFs on each open sockets before 
            //  dying.
            close(descr);
            FD_CLR(descr, &httpstate.rfds);
            httpstate.sockets[index].descriptor = 0;
#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 0
            printf("EOF received on descriptor: %d. Closing it.\n", descr);
#endif    
            return;
        }    
        
        if (len < 0)
        {
            // There was an error reading from this socket. Play it safe and
            //  close it. This will force the client to generate a shutdown
            //  and we will read a len = 0 the next time around.
            shutdown(descr, SHUT_WR);
#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 0
            diag_printf("ERROR reading from socket. read() returned: %d\n", 
                        httpstate.inbuffer_len);
#endif    
            return;
        }  

        httpstate.inbuffer[httpstate.inbuffer_len + len] = '\0';

        // It is always possible to receive split headers, in which case a
        //  header is only partially sent on one packet, with the rest on
        //  following packets. We can tell when a full packet is in the buffer
        //  by scanning for a header terminator ('\r\n\r\n'). Be smart and
        //  scan only the data received in the last read() operation, and not
        //  the full buffer each time.
        httpstate.request_end = 
               strstr(&httpstate.inbuffer[httpstate.inbuffer_len], "\r\n\r\n");
        httpstate.inbuffer_len += len;

        // Go through all the requests that were received in this packet.
        while (httpstate.request_end != 0)
        {
            httpstate.request_end += 4; // Include the terminator.
            
            // Timestamp the socket. 
            httpstate.sockets[index].timestamp = time(NULL);
                
            // This is where it all happens.
            cyg_httpd_process_method();
                
            if (httpstate.mode & CYG_HTTPD_MODE_CLOSE_CONN)
            {
                // There are 2 cases we can be here:
                // 1) chunked frames close their connection by default
                // 2) The client requested the connection be terminated with a
                //     "Connection: close" in the header
                // In any case, we close the TX pipe and wait for the client to
                //  send us an EOF on the receive pipe. This is a more graceful
                //  way to handle the closing of the socket, compared to just
                //  calling close() without first asking the opinion of the
                //  client, and  running the risk of stray data lingering 
                //  around.
                shutdown(descr, SHUT_WR);
            }
            
            // Move back the next request (if any) to the beginning of inbuffer.
            //  This way we avoid inching towards the end of inbuffer with
            //  consecutive requests.
            strcpy(httpstate.inbuffer, httpstate.request_end);
            httpstate.inbuffer_len -= (int)(httpstate.request_end - 
                                                       httpstate.inbuffer);
                                                       
            // If there is no data left over we are done processing all
            //  requests.
            if (httpstate.inbuffer_len == 0)
            {
                done = true;
                break;
            }    

            // Any other fully formed request pending?                                           
            httpstate.request_end = strstr(httpstate.inbuffer, "\r\n\r\n");
        }        
    }
    while (done == false);
}

void
cyg_httpd_handle_new_connection(cyg_int32 listener)
{
    cyg_int32 i;

    int fd_client = accept(listener, NULL, NULL);
    CYG_ASSERT(listener != -1, "accept() failed");
    if (fd_client == -1) 
        return;
    
#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 0
    diag_printf("Opening descriptor: %d\n", fd_client);
#endif    
    // Timestamp the socket and process the frame immediately, since the accept
    //  guarantees the presence of valid data on the newly opened socket.
    for (i = 0; i < CYGPKG_NET_MAXSOCKETS; i++)
        if (httpstate.sockets[i].descriptor == 0)
        {
            httpstate.sockets[i].descriptor = fd_client;
            httpstate.sockets[i].timestamp  = time(NULL);
            cyg_httpd_process_request(i);
            return;
        }    
}

// This is the "garbage collector" (or better, the "garbage disposer") of
//  the server. It closes any socket that has been idle for a time period
//  of CYG_HTTPD_SELECT_TIMEOUT seconds.
void
cyg_httpd_close_unused_sockets(cyg_int32 listener)
{
    cyg_int32 i;
    
#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 0
    diag_printf("Garbage collector called\r\n");
#endif    
    httpstate.fdmax = listener;
    for (i = 0; i < CYGPKG_NET_MAXSOCKETS; i++)
    {
        if (httpstate.sockets[i].descriptor != 0)
        {
            if (time(NULL) - httpstate.sockets[i].timestamp > 
                                          CYG_HTTPD_SOCKET_IDLE_TIMEOUT)
            {           
#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 0
                diag_printf("Closing descriptor: %d\n", 
                            httpstate.sockets[i].descriptor);
#endif    
                shutdown(httpstate.sockets[i].descriptor, SHUT_WR);
            }
            else
                httpstate.fdmax = MAX(httpstate.fdmax, 
                                      httpstate.sockets[i].descriptor);
        }                              
    }
}

void
cyg_httpd_daemon(cyg_addrword_t data)
{
    cyg_int32 rc;
    init_all_network_interfaces();

#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 0
#ifdef CYGHWR_NET_DRIVER_ETH0
    if (eth0_up)
    {
        struct bootp* bps = &eth0_bootp_data;
        diag_printf("ETH0 is up. IP address: %s\n", inet_ntoa(bps->bp_yiaddr));
    }
#endif
#endif

#ifdef CYGOPT_NET_ATHTTPD_USE_CGIBIN_TCL
    cyg_httpd_init_tcl_interpreter();
#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 0
    diag_printf("Tcl interpreter has been initialized...\n");
#endif
#endif    
    
    cyg_httpd_initialize();

    // Get the network going. This is benign if the application has
    //  already done this.
    cyg_int32 listener = socket(AF_INET, SOCK_STREAM, 0);
    CYG_ASSERT(listener > 0, "Socket create failed");
    if (listener < 0)
        return;

    cyg_int32 yes = 1;
    rc = setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (rc == -1) 
        return;
    
    memset(&(httpstate.server_conn), 0, sizeof(struct sockaddr_in));
    httpstate.server_conn.sin_family = AF_INET;
    httpstate.server_conn.sin_addr.s_addr = INADDR_ANY;
    httpstate.server_conn.sin_port = htons(CYGNUM_NET_ATHTTPD_SERVEROPT_PORT);
    rc = bind(listener,
              (struct sockaddr *)&httpstate.server_conn, 
              sizeof(struct sockaddr)); 
    CYG_ASSERT(rc == 0, "bind() returned error");
    if (rc != 0)
        return;

    rc = listen(listener, SOMAXCONN);
    CYG_ASSERT(rc == 0, "listen() returned error");
    if (rc != 0)
        return;

#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 0
    diag_printf("Web server Started and listening...\n");
#endif
    cyg_int32 i;
    for (i = 0; i < CYGPKG_NET_MAXSOCKETS; i++)
    {
        httpstate.sockets[i].descriptor  = 0;
        httpstate.sockets[i].timestamp   = (time_t)0;
    }
    
    FD_ZERO(&httpstate.rfds);
    httpstate.fdmax = listener;
    while (1)
    {
        // The listener is always added to the select() sensitivity list.
        FD_SET(listener, &httpstate.rfds); 
        struct timeval tv = {CYG_HTTPD_SOCKET_IDLE_TIMEOUT, 0};
        rc = select(httpstate.fdmax + 1, &httpstate.rfds, NULL, NULL, &tv);
        if (rc > 0)
        {
            if (FD_ISSET(listener, &httpstate.rfds))
                // If the request is from the listener socket, then 
                //  this must be a new connection.
                cyg_httpd_handle_new_connection(listener);

            httpstate.fdmax = listener;
            
            // The sensitivity list returned by select() can have multiple
            //  socket descriptors that need service. Loop through the whole
            //  descriptor list to see if one or more need to be served.
            for (i = 0; i < CYGPKG_NET_MAXSOCKETS; i ++)
            {
                cyg_int32 descr = httpstate.sockets[i].descriptor;
                if (descr != 0)
                {
                    // If the descriptor is set in the descriptor list, we
                    //  service it. Otherwise, we add it to the descriptor list
                    //  to listen for. The rfds list gets rewritten each time
                    //  select() is called and after the call it contains only
                    //  the descriptors that need be serviced. Before calling
                    //  select() again we must repopulate the list with all the
                    //  descriptors that must be listened for.
                    if (FD_ISSET(descr, &httpstate.rfds))
                        cyg_httpd_process_request(i);
                    else       
                        FD_SET(descr, &httpstate.rfds); 
                    if (httpstate.sockets[i].descriptor != 0)
                        httpstate.fdmax = MAX(httpstate.fdmax, descr);
                }
            }
        }
        else if (rc == 0)
        {
            cyg_httpd_close_unused_sockets(listener);
        }
        else
        {
#if CYGOPT_NET_ATHTTPD_DEBUG_LEVEL > 0
            cyg_int8 *ptr = (cyg_int8*)&httpstate.rfds;
            diag_printf("rfds: %x %x %x %x\n", ptr[0], ptr[1], ptr[2], ptr[3] );
            for (i = 0; i < CYGPKG_NET_MAXSOCKETS; i++)
                if (httpstate.sockets[i].descriptor != 0)
                     diag_printf("Socket in list: %d\n", 
                                 httpstate.sockets[i].descriptor);
#endif                                 
            CYG_ASSERT(rc != -1, "Error during select()");                 
        }
    }
}

void
cyg_httpd_start(void)
{
    if (cyg_httpd_initialized)
        return;
    cyg_httpd_initialized = 1;
    
    cyg_thread_create(CYGNUM_NET_ATHTTPD_THREADOPT_PRIORITY,
                      cyg_httpd_daemon,
                      (cyg_addrword_t)0,
                      "HTTPD Thread",
                      (void *)cyg_httpd_thread_stack,
                      CYG_HTTPD_DAEMON_STACK_SIZE,
                      &cyg_httpd_thread_handle,
                      &cyg_httpd_thread_object);
    cyg_thread_resume(cyg_httpd_thread_handle);
}                       



