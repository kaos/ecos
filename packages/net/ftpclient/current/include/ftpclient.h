#ifndef CYGONCE_NET_FTPCLIENT_FTPCLIENT_H
#define CYGONCE_NET_FTPCLIENT_FTPCLIENT_H

//==========================================================================
//
//      ftpclient.h
//
//      A simple FTP client
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
// Author(s):    andrew.lunn@ascom.ch
// Contributors: andrew.lunn@ascom.ch
// Date:         2001-11-4
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

typedef void (*ftp_printf_t)(unsigned error, const char *, ...);

/* Use the FTP protocol to retrieve a file from a server. Only binary
   mode is supported. The filename can include a directory name. Only
   use unix style / not M$'s \. The file is placed into buf. buf has
   maximum size buf_size. If the file is bigger than this, the
   transfer fails and FTP_TOOBIG is returned. Other error codes as
   listed below can also be returned. If the transfer is succseful the
   number of bytes received is returned. */

int ftp_get(char * hostname, 
            char * username, 
            char * passwd, 
            char * filename, 
            char * buf, 
            unsigned buf_size,
            ftp_printf_t ftp_printf);

/*Use the FTP protocol to send a file from a server. Only binary mode
  is supported. The filename can include a directory name. Only use
  unix style / not M$'s \. The contents of buf is placed into the file
  on the server. If an error occurs one of the codes as listed below
  will be returned. If the transfer is succseful zero is returned.*/

int ftp_put(char * hostname, 
            char * username, 
            char * passwd, 
            char * filename, 
            char * buf, 
            unsigned buf_size,
            ftp_printf_t ftp_printf);

/*ftp_get() and ftp_put take the name of a function to call to print
  out diagnostic and error messages. This is a sample implementation
  which can be used if you don't want to implement the function
  yourself. error will be true when the message to print is an error
  message. Otherwise the message is diagnostic, eg the commands sent
  and received from the server.*/

void ftpclient_printf(unsigned error, const char *fmt, ...);

/* Error codes */

#define FTP_BAD         -2 /* Catch all, socket errors etc. */
#define FTP_NOSUCHHOST  -3 /* The server does not exist. */
#define FTP_BADUSER     -4 /* Username/Password failed */
#define FTP_TOOBIG      -5 /* Out of buffer space or disk space */ 
#define FTP_BADFILENAME -6 /* The file does not exist */

#endif CYGONCE_NET_FTPCLIENT_FTPCLIENT

