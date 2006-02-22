//==========================================================================
//
//      tests/auto/memcheck.inl
//
//      Automated Testing by a Host Computer
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
// Date:         2000-11-10
// Purpose:      Check memory usage of the eCos TCP/IP Network Stack
// Description:  

#ifndef CYG_NET_GET_MEM_STATS_MISC
// API is not present, so dummies:
void memcheck_init( void )  {}
void memcheck_final( void ) {}
#else

#include <sys/mbuf.h>
extern struct  mbstat mbstat;

extern void cyg_kmem_print_stats( void );

cyg_mempool_info initial[3], final[3];

#define RATIO 15

static char *names[3] = { "Misc", "Mbufs", "Clusters" };

void memcheck_init( void )
{
    int i;

    cyg_kmem_print_stats();

    for ( i = 0; i < 3; i++ )
        if ( ! cyg_net_get_mem_stats( i, initial + i ) )
            CYG_TEST_FAIL( "Getting initial mem stats" );
}

void memcheck_final( void )
{
    int i;
    for ( i = 0; i < 3; i++ )
        if ( ! cyg_net_get_mem_stats( i, final + i ) )
            CYG_TEST_FAIL( "Getting final mem stats" );

    cyg_kmem_print_stats();

    // NB do NOT check clusters (index 2) because clusters are managed
    // differently.
    for ( i = 0; i < 3; i++ ) {
        int size = final[i].totalmem;
        int initial_used = size - initial[i].freemem;
        int margin = size * RATIO / 100;
        int final_used = size - final[i].freemem;
        int extra_used = final_used - initial_used;

        if ( 2 == i ) {
#ifdef XFAIL
            // ignore clusters if a faulty network is being simulated
            // because memory gets eaten for partial packets awaiting
            // reassembly.
            continue; 
#else            
            // Then it's clusters - they're managed differently
            final_used -= (mbstat.m_clfree * final[i].blocksize);
            extra_used = final_used - initial_used;
#endif
        }

        // Using up up to whatever was initially used -or- the percentage
        // we test for; whichever is larger, is OK.
        if ( (extra_used > initial_used) && (extra_used > margin) ) {
            CYG_TEST_INFO( "Memory leak suspected" );
            CYG_TEST_INFO( names[i] );
            CYG_TEST_INFO( (final[i].blocksize > 0) ?
                           "Variable mempool" : "Fixed mempool" );
            CYG_TEST_FAIL( "Too much memory used - storeleak suspected" );
        }
    }
}

#undef RATIO

#endif

// EOF memcheck.inl
