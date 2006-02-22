//==========================================================================
//
//      awaitorder.c
//
//      Await an order from the target, and print it out...
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
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
// Author(s):    hmt,gthomas
// Contributors: hmt,gthomas
// Date:         2000-10-10
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <stdio.h>

#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <net/route.h>

void
pexit(char *s)
{
    perror(s);
    exit(1);
}

#define SOURCE_PORT 9980

int main( int argc, char **argv )
{
    int s_source, e_source;
    struct sockaddr_in e_source_addr, local;
    int one = 1;
    int len;
    char orders[256];
    char ack[] = "Acknowledged\n";
    int loop = 1;

    if ( 2 < argc ) {
    usage:
        fprintf( stderr, "usage: %s [once|many]\n", argv[0] );
        exit(1);
    } 

    if ( 2 == argc ) {
        if ( !strcmp( "once", argv[1] ) )
            loop = 0;
        else if ( !strcmp( "many", argv[1] ) )
            loop = 1;
        else 
            goto usage;
    }

    s_source = socket(AF_INET, SOCK_STREAM, 6 /* TCP */ );
    if (s_source < 0) {
        pexit("stream socket");
    }


    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_len = sizeof(local);
    local.sin_port = ntohs(SOURCE_PORT);
    local.sin_addr.s_addr = INADDR_ANY;
    if(bind(s_source, (struct sockaddr *) &local, sizeof(local)) < 0) {
        pexit("bind /source/ error");
    }
    if (setsockopt(s_source, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) {
        pexit("setsockopt /source/ SO_REUSEADDR");
    }
#ifdef SO_REUSEPORT
    if (setsockopt(s_source, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one))) {
        pexit("setsockopt /source/ SO_REUSEPORT");
    }
#endif

    listen(s_source, SOMAXCONN);

    do {
        len = sizeof(e_source_addr);
        if ((e_source = accept(s_source,
                 (struct sockaddr *)&e_source_addr, &len)) < 0) {
            pexit("accept /source/");
        }
        fprintf( stderr, "awaitorder: connection from %s:%d\n", 
                 inet_ntoa(e_source_addr.sin_addr), ntohs(e_source_addr.sin_port));
        
        // Wait for "source" to tell us the orders
        if ( 0 >= (len = read(e_source, orders, sizeof(orders)))) {
            pexit("Can't read orders");
        }
        if ( sizeof(ack) != write(e_source, ack, sizeof( ack ) ) ) {
            pexit("Can't write ACK");
        }

        orders[len] = 0;

        printf( "ORDERS: %s\n", orders );
        fflush( stdout );

        close( e_source );
    } while ( loop );

    close( s_source );
}
// EOF awaitorder.c
