//==========================================================================
//
//      tests/dns1.c
//
//      Simple test of DNS client support
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    andrew.lunn
// Contributors: andrew.lunn, jskov
// Date:         2001-09-18
// Purpose:      
// Description:  Test FTPClient functions. Note that the _XXX defines below
//               control what addresses the test uses. These must be
//               changed to match the particular testing network in which
//               the test is to be run.
//              
//####DESCRIPTIONEND####
//
//==========================================================================
 
#include <network.h>
#include <ftpclient.h>
#include <cyg/infra/testcase.h>
 
#define STACK_SIZE (CYGNUM_HAL_STACK_SIZE_TYPICAL + 0x1000)
static char stack[STACK_SIZE];
static cyg_thread thread_data;
static cyg_handle_t thread_handle;
 
#define __string(_x) #_x
#define __xstring(_x) __string(_x)
 
#define _FTP_SRV           __xstring(172.16.19.254) // farmnet dns0 address
                                     
#define FTPBUFSIZE (1024 * 64)
char ftpbuf[FTPBUFSIZE];
char ftpbuf1[FTPBUFSIZE];

void
ftp_test(cyg_addrword_t p)
{
  int ret;

  CYG_TEST_INIT();
  
  init_all_network_interfaces();
 
  CYG_TEST_INFO("Getting /etc/passwd from %s\n" _FTP_SRV);
  ret = ftp_get(_FTP_SRV,"anonymous","ftpclient1",
                 "/etc/passwd",ftpbuf,FTPBUFSIZE,
                ftpclient_printf);

  if (ret > 0) {
    diag_printf("PASS:< %s bytes received>\n",ret);
  } else {
    diag_printf("FAIL:< ftp_get returned %d>\n",ret);
  }

  CYG_TEST_INFO("Putting passwd file back in /incoming/passwd\n");
  ret = ftp_put(_FTP_SRV,"anonymous","ftpclient1",
                "/incoming/passwd",ftpbuf,ret,
                ftpclient_printf);
  
  if (ret > 0) {
    diag_printf("PASS:\n");
  } else {
    diag_printf("FAIL:< ftp_get returned %d>\n",ret);
  }

  CYG_TEST_INFO("Reading back /incoming/passwd\n");
  ret = ftp_get(_FTP_SRV,"anonymous","ftpclient1",
                 "/incoming/passwd",ftpbuf1,FTPBUFSIZE,
                ftpclient_printf);
  
  if (ret > 0) {
    diag_printf("PASS:< %s bytes received>\n",ret);
  } else {
    diag_printf("FAIL:< ftp_get returned %d>\n",ret);
  }

  CYG_TEST_PASS_FAIL(!memcmp(ftpbuf,ftpbuf1,ret),"Transfer integrity");

  CYG_TEST_INFO("ftp_Get'ing with a bad username\n");
  ret = ftp_get(_FTP_SRV,"nosuchuser","ftpclient1",
                "/incoming/passwd",ftpbuf1,FTPBUFSIZE,
                ftpclient_printf);
  CYG_TEST_PASS_FAIL(ret==FTP_BADUSER,"Bad Username");

  CYG_TEST_INFO("ftp_get'ting with a bad passwd\n");
  ret = ftp_get(_FTP_SRV,"nobody","ftpclient1",
                "/incoming/passwd",ftpbuf1,FTPBUFSIZE,
                ftpclient_printf);
  CYG_TEST_PASS_FAIL(ret==FTP_BADUSER,"Bad passwd");

  CYG_TEST_INFO("ftp_get'ing from a with a bad passwd\n");
  ret = ftp_get(_FTP_SRV,"nobody","ftpclient1",
                "/incoming/passwd",ftpbuf1,FTPBUFSIZE,
                ftpclient_printf);
  CYG_TEST_PASS_FAIL(ret==FTP_BADUSER,"Bad passwd");

  CYG_TEST_INFO("ftp_get'ing from a bad server\n");
  ret = ftp_get("127.0.0.1","nobody","ftpclient1",
                "/incoming/passwd",ftpbuf1,FTPBUFSIZE,
                ftpclient_printf);
  CYG_TEST_PASS_FAIL(ret==FTP_NOSUCHHOST,"Bad server");

  CYG_TEST_INFO("ftp_get'ing a file which is too big");
  ret = ftp_get(_FTP_SRV,"nobody","ftpclient1",
                "/incoming/passwd",ftpbuf,2,
                ftpclient_printf);
  CYG_TEST_PASS_FAIL(ret==FTP_TOOBIG,"File too big");
}

void
cyg_start(void)
{
  /* Create a main thread, so we can run the scheduler and have time 'pass' */
  cyg_thread_create(10,                // Priority - just a number
                    ftp_test,          // entry
                    0,                 // entry parameter
                    "FTP test",        // Name
                    &stack[0],         // Stack
                    STACK_SIZE,        // Size
                    &thread_handle,    // Handle
                    &thread_data       // Thread data structure
                    );
  cyg_thread_resume(thread_handle);  /* Start it */
  cyg_scheduler_start();
}
