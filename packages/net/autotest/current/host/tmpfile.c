//==========================================================================
//
//      tmpfile.c
//
//      Make a temporary data file for test purposes
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
// Author(s):    hmt
// Contributors: hmt
// Date:         2000-10-23
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <stdio.h>
#include <errno.h>

int main( int argc, char **argv )
{
    FILE *f;
    int i;
    unsigned int size, iirandom, z, csum;
    char sizec, scratchc;

    if ( 3 > argc || 4 < argc) {
    usage:
        fprintf( stderr, "usage: %s filename length [seed]\n", argv[0] );
        exit(1);
    } 

    f = fopen( argv[1], "rb" );
    if ( f ) {
        fclose( f );
        fprintf( stderr, "File %s already exists\n", argv[1] );
        goto usage;
    }

    i = sscanf( argv[2], "%u%1c%1c", &size, &sizec, &scratchc );
    if ( 0 >= i || 2 < i )
        goto usage;

    if ( 2 == i ) 
        switch ( sizec ) {
        case 'G': size *= 1024;
        case 'M': size *= 1024;
        case 'k': size *= 1024;
            break;
        case 'b': size *= 512;
            break;
        default:
            goto usage;
        }

    iirandom = 0;
    if ( 4 == argc ) {
        i = sscanf( argv[3], "%u%1c", &iirandom, &scratchc );
        if ( 0 >= i || 1 < i )
            goto usage;
    }

    f = fopen( argv[1], "wb" );
    if ( !f ) {
        perror( "fopen [create]" );
        goto usage;
    }

    csum = 0;
    iirandom += 0x23164920; // What date said

    if ( size > 4 ) i = size-4; else i = size;

    for ( /* i */ ; i > 0; i-- ) {
        iirandom = iirandom * 91 + 137;
        iirandom ^= iirandom >> 14 | iirandom << 7;
        z = (i ^ iirandom) & 0xff;
        // Keep a simple checksum in case we make some trick to check it in
        // the pretend filesystem.
        csum += z;
        csum = (csum >> (z & 31)) ^ (csum << (32 - (z & 31)));
        if ( 0 > putc( z, f ) ) {
            perror( "putc" );
            fclose( f );
            exit( 1 );
        }
    }
    // And bung the checksum on the end.
    for ( i = 4 ; i < size && i > 0; i-- ) {
        z = csum & 0xff;
        csum >>= 8;
        if ( 0 > putc( z, f ) ) {
            perror( "putc" );
            fclose( f );
            exit( 1 );
        }
    }
    fclose( f );
    exit( 0 );
}

// EOF awaitorder.c
