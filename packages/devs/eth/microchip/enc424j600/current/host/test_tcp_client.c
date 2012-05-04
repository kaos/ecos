//=================================================================
//
//        test_tcp_client.c
//
//        TCP/Ethernet testing - host-side
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2010 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Ilija Stanislevik
// Date:          2010-02-10
//####DESCRIPTIONEND####
//==========================================================================

/*
 The target hardware runs a listener/server application. The server
 calculates checksum on recived packets for the whole duration of 
 connection. Once conection ends, it displays the checksum.
 
 The host client generates pseudorandom data and sends it to the target.
 Before sending, it calculates checksum. Once data sent, it dispalys
 the checksum for comparison with the checksum at target side.
 
 For now, no attempts has been made to employ a host other than Linux.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define NUM_ARG 4

struct addrinfo * hostaddr_p;
int socket_d;
struct sockaddr serv_addr;

void print_syntax(char *name)
{
    printf("Syntax: %s ip_address port bytes_to_send\n", name);
    printf("        Format ip_address as d1.d2.d3.d4, where dx is integer 0-255\n\n");
}

int main(int argc, char *argv[])
{
    int retval;
    struct addrinfo hint_s;
    struct addrinfo *p;
    char ipstr[INET6_ADDRSTRLEN];
    unsigned long int msg_len;
    unsigned long int left_to_send;
    unsigned long int i;
    unsigned long int csum;
    char *msg_p = NULL;
    char *fill_p;
    unsigned char value;

    if(NUM_ARG != argc)
    {
        printf("\nError:  Wrong number of arguments %d instead of %d.\n", argc-1, NUM_ARG-1);
        print_syntax(argv[0]);
        return EXIT_FAILURE;
    }

    memset(&hint_s, 0, sizeof(struct addrinfo));
    hint_s.ai_family = AF_INET;
    hint_s.ai_socktype = SOCK_STREAM;
    hint_s.ai_flags = AI_NUMERICSERV;
    hint_s.ai_protocol = 0;          /* Any protocol */
    hint_s.ai_canonname = NULL;
    hint_s.ai_addr = NULL;
    hint_s.ai_next = NULL;
    retval = getaddrinfo(argv[1], argv[2], &hint_s, &hostaddr_p);
    if(0 != retval)
    {
        printf("\nError:  %s\n", gai_strerror(retval));
        print_syntax(argv[0]);
        return EXIT_FAILURE;
    }

    printf("IP addresses for %s:\n", argv[1]);

    for(p = hostaddr_p; p != NULL; p = p->ai_next)
    {
        void *addr;
        char *ipver;
        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET)
        { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        }
        else
        { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }
        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf(" %s: %s\n", ipver, ipstr);
    }

    socket_d = socket(hostaddr_p->ai_family, hostaddr_p->ai_socktype, hostaddr_p->ai_protocol);
    if(-1 == socket_d)
    {
        printf("Error:  Can't get socket. errno %d\n", errno);
        return EXIT_FAILURE;
    }

    retval = connect(socket_d, hostaddr_p->ai_addr, hostaddr_p->ai_addrlen);
    if(0 != retval)
    {
        perror("\nError:  Failed connect()\n");
        return EXIT_FAILURE;
    }

    // Prepare message to send
    msg_len = strtoul(argv[3], NULL, 0);
    msg_len &= ~0x3;
    if (0 == msg_len)
    {
        printf("Nothing to do (bytes_to_send == 0). Exiting.\n");
        return EXIT_SUCCESS;
    }
    else
    {
        printf("Preparing to send %lu bytes.\n", msg_len);
    }

    msg_p = malloc(msg_len);
    if(NULL == msg_p)
    {
        printf("Error:  Can't allocate %lu bytes.\n", msg_len);
        return EXIT_FAILURE;
    }

    fill_p = msg_p;
    csum = 0;

    for(i=0; i<msg_len; i++)
    {
        value = random() & 0xff;
        *fill_p++ = value;
        csum += value;
    }

    left_to_send = msg_len;
    do
    {
        retval = send(socket_d, msg_p, msg_len, 0);
        left_to_send -= retval;
        printf("%d bytes sent.\n", retval);
    } while(left_to_send);
    printf("Checksum %08lx\n", csum);

    freeaddrinfo(hostaddr_p); // free the linked list

    return EXIT_SUCCESS;
}
