/*==========================================================================
//
//      dhcp_support.c
//
//      Support code == friendly API for DHCP client
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   hmt
// Contributors: gthomas
// Date:        2000-07-01
// Purpose:     DHCP support
// Description: 
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/net.h>

#ifdef CYGPKG_NET_DHCP

#include <network.h>
#include <dhcp.h>

// ---------------------------------------------------------------------------
#ifdef CYGHWR_NET_DRIVER_ETH0
cyg_uint8   eth0_dhcpstate = 0;
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
cyg_uint8   eth1_dhcpstate = 0;
#endif

cyg_sem_t dhcp_needs_attention;

#ifdef CYGHWR_NET_DRIVER_ETH0
struct dhcp_lease eth0_lease = { 0,0,0,0 };
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
struct dhcp_lease eth1_lease = { 0,0,0,0 };
#endif

// ---------------------------------------------------------------------------
//
// The point of this module is to deal with all the horrid written out in
// full stuff of having two interfaces; it's ugly but it's also most
// flexible.  The dhcp_prot.c module should do all the work... 
//
// ---------------------------------------------------------------------------

// return value: 1 => everything OK, no change.
// 0 => close your connections, then call do_dhcp_halt() to halt the
// interface(s) in question (it knows because the state will be NOTBOUND).
// After that you can return to the start and use
// init_all_network_interfaces(); as usual, or call do_dhcp_bind() by hand,
// or whatever...
int dhcp_bind( void )
{
#ifdef CYGHWR_NET_DRIVER_ETH0
    cyg_uint8 old_eth0_dhcpstate = eth0_dhcpstate;
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
    cyg_uint8 old_eth1_dhcpstate = eth1_dhcpstate;
#endif

    // If there are no interfaces at all, init it every time, doesn't
    // matter.
    if ( 1
#ifdef CYGHWR_NET_DRIVER_ETH0
         && eth0_dhcpstate == 0
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
         && eth1_dhcpstate == 0
#endif
        )
        cyg_semaphore_init( &dhcp_needs_attention, 0 );

    // Run the state machine...
#ifdef CYGHWR_NET_DRIVER_ETH0
    if (eth0_up
        && DHCPSTATE_FAILED != eth0_dhcpstate )
            eth0_up = do_dhcp(eth0_name, &eth0_bootp_data, &eth0_dhcpstate, &eth0_lease);
#endif            
#ifdef CYGHWR_NET_DRIVER_ETH0
    if (eth1_up
        && DHCPSTATE_FAILED != eth1_dhcpstate )
            eth1_up = do_dhcp(eth1_name, &eth1_bootp_data, &eth1_dhcpstate, &eth1_lease);
#endif            

    // If the interface newly came up, initialize it:
    // (this duplicates the code in init_all_network_interfaces() really).
#ifdef CYGHWR_NET_DRIVER_ETH0
    if ( eth0_up
         && eth0_dhcpstate == DHCPSTATE_BOUND
         && old_eth0_dhcpstate != eth0_dhcpstate ) {
        if (!init_net(eth0_name, &eth0_bootp_data)) {
            eth0_up = false;
        }
    }
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
    if ( eth1_up
         && eth1_dhcpstate == DHCPSTATE_BOUND
         && old_eth1_dhcpstate != eth1_dhcpstate ) {
        if (!init_net(eth1_name, &eth1_bootp_data)) {
            eth1_up = false;
        }
    }
#endif

#ifdef CYGHWR_NET_DRIVER_ETH0
    if ( old_eth0_dhcpstate == DHCPSTATE_BOUND &&
         eth0_dhcpstate == DHCPSTATE_NOTBOUND )
        return 0; // a lease timed out; we became unbound
#endif
#ifdef CYGHWR_NET_DRIVER_ETH0
    if ( old_eth1_dhcpstate == DHCPSTATE_BOUND &&
         eth1_dhcpstate == DHCPSTATE_NOTBOUND )
        return 0; // a lease timed out; we became unbound
#endif
    return 1; // all is well
}


// Shutdown any interface whose state is DHCPSTATE_NOTBOUND.
int dhcp_halt( void )
{
#ifdef CYGHWR_NET_DRIVER_ETH0
    if ( eth0_up
         && eth0_dhcpstate != DHCPSTATE_FAILED ) {
        do_dhcp_down_net(eth0_name, &eth0_bootp_data, &eth0_dhcpstate, &eth0_lease);
    }
    eth0_up = false;
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
    if ( eth1_up
         && eth1_dhcpstate != DHCPSTATE_FAILED ) {
        do_dhcp_down_net(eth1_name, &eth1_bootp_data, &eth1_dhcpstate, &eth1_lease);
    }
    eth1_up = false;
#endif
}


// Release (and set state to DHCPSTATE_NOTBOUND) all interfaces - we are
// closing down.  (unlikely but maybe useful for testing)
int dhcp_release( void )
{
#ifdef CYGHWR_NET_DRIVER_ETH0
    if (eth0_up)
        do_dhcp_release(eth0_name, &eth0_bootp_data, &eth0_dhcpstate, &eth0_lease);
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
    if (eth1_up)
        do_dhcp_release(eth1_name, &eth1_bootp_data, &eth1_dhcpstate, &eth1_lease);
#endif
}


// ------------------------------------------------------------------------
// The management thread function
void dhcp_mgt_entry( cyg_addrword_t loop_on_failure )
{
    while ( 1 ) {
        while ( 1 ) {
            cyg_semaphore_wait( &dhcp_needs_attention );
            if ( ! dhcp_bind() ) // a lease expired
                break; // If we need to re-bind
        }
        dhcp_halt(); // tear everything down
        if ( loop_on_failure )
            init_all_network_interfaces(); // re-initialize
        else
            return; // exit the thread/return
    }
}

#ifdef CYGOPT_NET_DHCP_DHCP_THREAD
// Then we provide such a thread...
cyg_handle_t dhcp_mgt_thread_h = 0;
cyg_thread   dhcp_mgt_thread;

#define STACK_SIZE (CYGNUM_HAL_STACK_SIZE_TYPICAL + sizeof(struct bootp))
static cyg_uint8 dhcp_mgt_stack[ STACK_SIZE ];

void dhcp_start_dhcp_mgt_thread( void )
{
    if ( ! dhcp_mgt_thread_h ) {
        cyg_thread_create(
            CYGPKG_NET_DHCP_THREAD_PRIORITY, /* scheduling info (eg pri) */
            dhcp_mgt_entry,             /* entry point function */
            CYGOPT_NET_DHCP_DHCP_THREAD_PARAM, /* entry data */
            "DHCP lease mgt",           /* optional thread name */
            dhcp_mgt_stack,             /* stack base, NULL = alloc */
            STACK_SIZE,                 /* stack size, 0 = default */
            &dhcp_mgt_thread_h,         /* returned thread handle */
            &dhcp_mgt_thread           /* put thread here */
            );

        cyg_thread_resume(dhcp_mgt_thread_h);
    }
}


#endif // CYGOPT_NET_DHCP_DHCP_THREAD

#endif // CYGPKG_NET_DHCP

// EOF dhcp_support.c
