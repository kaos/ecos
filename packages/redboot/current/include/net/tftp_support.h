//==========================================================================
//
//      net/tftp_support.h
//
//      Stand-alone TFTP support for RedBoot
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
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-07-14
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef _TFTP_SUPPORT_H_
#define _TFTP_SUPPORT_H_

#include "net.h"

/*
 * File transfer modes
 */
#define TFTP_NETASCII   0              // Text files
#define TFTP_OCTET      1              // Binary files

/*
 * Errors
 */
#define	TFTP_ENOTFOUND   1   /* file not found */
#define	TFTP_EACCESS     2   /* access violation */
#define	TFTP_ENOSPACE    3   /* disk full or allocation exceeded */
#define	TFTP_EBADOP      4   /* illegal TFTP operation */
#define	TFTP_EBADID      5   /* unknown transfer ID */
#define	TFTP_EEXISTS     6   /* file already exists */
#define	TFTP_ENOUSER     7   /* no such user */
#define TFTP_TIMEOUT     8   /* operation timed out */
#define TFTP_NETERR      9   /* some sort of network error */
#define TFTP_INVALID    10   /* invalid parameter */
#define TFTP_PROTOCOL   11   /* protocol violation */
#define TFTP_TOOLARGE   12   /* file is larger than buffer */

/*
 * Client support
 */

extern int   tftp_get(char *, struct sockaddr_in *, char *, int, int, int *);
extern char *tftp_error(int err);

extern int   tftp_stream_open(char *file, struct sockaddr_in *server, int mode, int *err);
extern int   tftp_stream_read(char *buf, int len, int *err);
extern int   tftp_stream_close(int *err);

#define TFTP_TIMEOUT_PERIOD 5
#define TFTP_TIMEOUT_MAX    5
#define TFTP_RETRIES_MAX    5

#define TFTP_PORT           69

#endif // _TFTP_SUPPORT_H_
