//==========================================================================
//
//      lib/tftp_server.c
//
//      TFTP server support
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//####BSDCOPYRIGHTBEGIN####
//
// -------------------------------------------
//
// Portions of this software may have been derived from OpenBSD or other sources,
// and are covered by the appropriate copyright disclaimers included herein.
//
// -------------------------------------------
//
//####BSDCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-04-06
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

// TFTP server support

#include <network.h>          // Basic networking support
#include <arpa/tftp.h>        // TFTP protocol definitions
#include <tftp_support.h>     // TFTPD structures
#include <cyg/kernel/kapi.h>
#include <stdlib.h>           // For malloc

#define STACK_SIZE (CYGNUM_HAL_STACK_SIZE_TYPICAL+(3*(SEGSIZE+sizeof(struct tftphdr))))
static char *TFTP_tag = "TFTPD";
struct tftp_server {
    char                 *tag;
    char                  stack[STACK_SIZE];
    cyg_thread            thread_data;
    cyg_handle_t          thread_handle;
    int                   port;
    struct tftpd_fileops *ops;
};

//
// Receive a file from the client
//
static void
tftpd_write_file(struct tftp_server *server,
                 struct tftphdr *hdr, 
                 struct sockaddr_in *from_addr, int from_len)
{
    char data_out[SEGSIZE+sizeof(struct tftphdr)];
    char data_in[SEGSIZE+sizeof(struct tftphdr)];
    struct tftphdr *reply = (struct tftphdr *)data_out;
    struct tftphdr *response = (struct tftphdr *)data_in;
    int fd, block, len, ok, data_len, s;
    struct timeval timeout;
    fd_set fds;
    int total_timeouts = 0;
    struct sockaddr_in client_addr, local_addr;
    int client_len;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        diag_printf("TFTPD: can't open socket for 'write_file'\n");
        return;
    }
    memset((char *)&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(INADDR_ANY);
    if (bind(s, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        // Problem setting up my end
        diag_printf("TFTPD: can't bind to reply port for 'write_file'\n");
        close(s);
        return;
    }
    if ((fd = (server->ops->open)(hdr->th_stuff, O_WRONLY)) < 0) {
        reply->th_opcode = htons(ERROR);
        reply->th_code = htons(ENOTFOUND);
        strcpy(reply->th_msg, "Can't open file");
        sendto(s, reply, 4+strlen(reply->th_msg)+1, 0, 
               (struct sockaddr *)from_addr, from_len);
        close(s);
        return;
    }
    // Send ACK telling client he can send data
    reply->th_opcode = htons(ACK);
    reply->th_block = 0;
    sendto(s, reply, 4, 0, (struct sockaddr *)from_addr, from_len);
    block = 1;
    ok = true;
    while (ok) {
        timeout.tv_sec = TFTP_TIMEOUT_PERIOD;
        timeout.tv_usec = 0;
        FD_ZERO(&fds);
        FD_SET(s, &fds);
        if (select(s+1, &fds, 0, 0, &timeout) <= 0) {
            if (++total_timeouts > TFTP_TIMEOUT_MAX) {
                ok = false;
                break;
            }
            // Try resending last ACK
            sendto(s, reply, 4, 0, (struct sockaddr *)from_addr, from_len);
        } else {
            // Some data has arrived
            data_len = sizeof(data_in);
            client_len = sizeof(client_addr);
            if ((data_len = recvfrom(s, data_in, data_len, 0, 
                                     (struct sockaddr *)&client_addr, &client_len)) < 0) {
                // What happened?
                diag_printf("Can't read client data!\n");
                ok = false;
                break;
            }
            if (ntohs(response->th_opcode) == DATA) {
                if (ntohs(response->th_block) == block) {
                    // Good data - write to file
                    len = (server->ops->write)(fd, response->th_data, data_len-4);
                    if (len < (data_len-4)) {
                        // File is "full"
                        reply->th_opcode = htons(ERROR);
                        reply->th_code = htons(TFTP_ENOSPACE);
                        ok = false;  // Give up
                    } else {
                        reply->th_opcode = htons(ACK);
                        reply->th_block = htons(block++);
                    }
                    sendto(s, reply, 4, 0, (struct sockaddr *)from_addr, from_len);
                    if (data_len < (SEGSIZE+4)) { 
                        // End of file
                        ok = false;
                    }
                } else {
                    // Something is wrong - tell client last good block
                    sendto(s, reply, 4, 0, (struct sockaddr *)from_addr, from_len);
                }
            } else {
                // Client has sent something bogus - bag out!
                reply->th_opcode = htons(ERROR);
                reply->th_code = htons(TFTP_EBADOP);
                sendto(s, reply, 4, 0, (struct sockaddr *)from_addr, from_len);
                ok = false;
            }
        }
    }
    close(s);
    (server->ops->close)(fd);
}

//
// Send a file to the client
//
static void
tftpd_read_file(struct tftp_server *server,
                struct tftphdr *hdr, 
                struct sockaddr_in *from_addr, int from_len)
{
    char data_out[SEGSIZE+sizeof(struct tftphdr)];
    char data_in[SEGSIZE+sizeof(struct tftphdr)];
    struct tftphdr *reply = (struct tftphdr *)data_out;
    struct tftphdr *response = (struct tftphdr *)data_in;
    int fd, block, len, tries, ok, data_len, s;
    struct timeval timeout;
    fd_set fds;
    int total_timeouts = 0;
    struct sockaddr_in client_addr, local_addr;
    int client_len;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        diag_printf("TFTPD: can't open socket for 'read_file'\n");
        return;
    }
    memset((char *)&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(INADDR_ANY);
    if (bind(s, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        // Problem setting up my end
        diag_printf("TFTPD: can't bind to reply port for 'read_file'\n");
        close(s);
        return;
    }
    if ((fd = (server->ops->open)(hdr->th_stuff, O_RDONLY)) < 0) {
        reply->th_opcode = htons(ERROR);
        reply->th_code = htons(ENOTFOUND);
        strcpy(reply->th_msg, "Can't open file");
        sendto(s, reply, 4+strlen(reply->th_msg)+1, 0, 
               (struct sockaddr *)from_addr, from_len);
        close(s);
        return;
    }
    block = 0;
    ok = true;
    while (ok) {
        // Read next chunk of file
        len = (server->ops->read)(fd, reply->th_data, SEGSIZE);
        reply->th_block = htons(++block);
        reply->th_opcode = htons(DATA);
        for (tries = 0;  tries < TFTP_RETRIES_MAX;  tries++) {
            if (sendto(s, reply, 4+len, 0, (struct sockaddr *)from_addr, from_len) < 0) {
                // Something went wrong with the network!
                ok = false;
                break;
            }
            timeout.tv_sec = TFTP_TIMEOUT_PERIOD;
            timeout.tv_usec = 0;
            FD_ZERO(&fds);
            FD_SET(s, &fds);
            if (select(s+1, &fds, 0, 0, &timeout) <= 0) {
                if (++total_timeouts > TFTP_TIMEOUT_MAX) {
                    ok = false;
                    break;
                }
            }
            data_len = sizeof(data_in);
            client_len = sizeof(client_addr);
            if ((data_len = recvfrom(s, data_in, data_len, 0, 
                                     (struct sockaddr *)&client_addr, &client_len)) < 0) {
                // What happened?
                ok = false;
                break;
            }
            if ((ntohs(response->th_opcode) == ACK) &&
                (ntohs(response->th_block) == block)) {
                break;
            }
        }
        if (len < SEGSIZE) {
            break;
        }
    }
    close(s);
    (server->ops->close)(fd);
}

//
// Actual TFTP server
//
static void
tftpd_server(cyg_addrword_t p)
{
    struct tftp_server *server = (struct tftp_server *)p;
    int s;
    int data_len, recv_len, from_len;
    struct sockaddr_in local_addr, from_addr;
    struct servent *server_info;
    char data[SEGSIZE+sizeof(struct tftphdr)];
    struct tftphdr *hdr = (struct tftphdr *)data;

#ifndef CYGPKG_NET_TESTS_USE_RT_TEST_HARNESS
    // Otherwise routine printfs fail the test - interrupts disabled too long.
    diag_printf("TFTPD: %x, port: %d\n", p, server->port);
#endif

    // Set up port
    if (server->port == 0) {
        server_info = getservbyname("tftp", "udp");
        if (server_info == (struct servent *)0) {
            diag_printf("TFTPD: can't get TFTP service information\n");
            return;
        }
        server->port = server_info->s_port;
    }

    // Create socket
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        diag_printf("TFTPD: can't open socket\n");
        return;
    }
    memset((char *)&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(server->port);
    if (bind(s, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        // Problem setting up my end
        diag_printf("TFTPD: can't bind to service port\n");
        close(s);
        return;
    }

    while (true) {
        recv_len = sizeof(data);
        from_len = sizeof(from_addr);
        if ((data_len = recvfrom(s, hdr, recv_len, 0, 
                                 (struct sockaddr *)&from_addr, &from_len)) < 0) {
            diag_printf("TFTPD: can't read request\n");
        } else {
#ifndef CYGPKG_NET_TESTS_USE_RT_TEST_HARNESS
            diag_printf("TFTPD: received %x from %s:%d\n", 
                        ntohs(hdr->th_opcode), inet_ntoa(from_addr.sin_addr), from_addr.sin_port);
#endif
            switch (ntohs(hdr->th_opcode)) {
            case WRQ:
                tftpd_write_file(server, hdr, &from_addr, from_len);
                break;
            case RRQ:
                tftpd_read_file(server, hdr, &from_addr, from_len);
                break;
            case ACK:
            case DATA:
            case ERROR:
                // Ignore
                break;
            default:
                diag_printf("TFTPD: bogus request %x from %s:%d\n", 
                            ntohs(hdr->th_opcode), inet_ntoa(from_addr.sin_addr), from_addr.sin_port);
                hdr->th_opcode = htons(ERROR);
                hdr->th_code = htons(EBADOP);
                strcpy(hdr->th_msg, "Illegal request/operation");
                sendto(s, hdr, 4+strlen(hdr->th_msg)+1, 0, 
                       (struct sockaddr *)&from_addr, from_len);
            }
        }
    }
}

//
// This function is used to create a new server [thread] which supports
// the TFTP protocol on the given port.  A server 'id' will be returned
// which can later be used to destroy the server.  
//
// Note: all [memory] resources for the server thread will be allocated
// dynamically.  If there are insufficient resources, an error will be
// returned.
//
int 
tftpd_start(int port, struct tftpd_fileops *ops)
{
    struct tftp_server *server;
    if ((server = malloc(sizeof(struct tftp_server)))) {
        server->tag = TFTP_tag;
        server->port = port;
        server->ops = ops;
        cyg_thread_create(CYGPKG_NET_TFTPD_THREAD_PRIORITY, // Priority
                          tftpd_server,              // entry
                          (cyg_addrword_t)server,    // entry parameter
                          "TFTP server",             // Name
                          &server->stack[0],         // Stack
                          STACK_SIZE,                // Size
                          &server->thread_handle,    // Handle
                          &server->thread_data       // Thread data structure
            );
        cyg_thread_resume(server->thread_handle);  // Start it

    }
    return (int)server;
}

//
// Destroy a TFTP server, using a previously created server 'id'.
//
int 
tftpd_stop(int p)
{
    struct tftp_server *server = (struct tftp_server *)p;
    // Simple sanity check
    if (server->tag == TFTP_tag) {
        cyg_thread_kill(server->thread_handle);
        cyg_thread_set_priority(server->thread_handle, 0);
        cyg_thread_delay(1);  // Make sure it gets to die...
        if (cyg_thread_delete(server->thread_handle)) {
            // Success shutting down the thread
            free(server);  // Give up memory
            return 1;
        }
    }
    return 0;
}

// EOF tftp_server.c
