//==========================================================================
//
//      tests/tftp_client_test.c
//
//      Simple TFTP client test
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
// Date:         2000-04-07
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================
// TFTP test code

#include <network.h>
#include <tftp_support.h>

// Note: the TFTP client calls need at least (SEGSIZE==512)+4
// additional bytes of workspace, thus the padding.
#define STACK_SIZE (CYGNUM_HAL_STACK_SIZE_TYPICAL+1024)
static char stack[STACK_SIZE];
static cyg_thread thread_data;
static cyg_handle_t thread_handle;

#define min(x,y) (x<y ? x : y)

extern void
cyg_test_exit(void);

void
pexit(char *s)
{
    perror(s);
    cyg_test_exit();
}

static char buf[32*1024];

#define GETFILE "tftp_get"
#define PUTFILE "tftp_put"

static void
tftp_test(struct bootp *bp)
{
    int res, err, len;
    struct sockaddr_in host;

    memset((char *)&host, 0, sizeof(host));
    host.sin_len = sizeof(host);
    host.sin_family = AF_INET;
    host.sin_addr = bp->bp_siaddr;
    host.sin_port = 0;
    diag_printf("Trying tftp_get %s %16s...\n", GETFILE, inet_ntoa(host.sin_addr));
    res = tftp_get( GETFILE, &host, buf, sizeof(buf), TFTP_OCTET, &err);
    diag_printf("res = %d, err = %d\n", res, err);
    if (res > 0) {
        diag_dump_buf(buf, min(res,1024));
    }
    len = res;
    diag_printf("Trying tftp_put %d %16s, length %d\n",
                PUTFILE, inet_ntoa(host.sin_addr), len);
    res = tftp_put( PUTFILE, &host, buf, len, TFTP_OCTET, &err);
    diag_printf("put - res: %d\n", res);
}

void
net_test(cyg_addrword_t param)
{
    diag_printf("Start TFTP test\n");
    init_all_network_interfaces();
#ifdef CYGHWR_NET_DRIVER_ETH0
    if (eth0_up) {
        tftp_test(&eth0_bootp_data);
    }
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
    if (eth1_up) {
        tftp_test(&eth1_bootp_data);
    }
#endif
    cyg_test_exit();
}

void
cyg_start(void)
{
    // Create a main thread, so we can run the scheduler and have time 'pass'
    cyg_thread_create(10,                // Priority - just a number
                      net_test,          // entry
                      0,                 // entry parameter
                      "Network test",    // Name
                      &stack[0],         // Stack
                      STACK_SIZE,        // Size
                      &thread_handle,    // Handle
                      &thread_data       // Thread data structure
            );
    cyg_thread_resume(thread_handle);  // Start it
    cyg_scheduler_start();
}
