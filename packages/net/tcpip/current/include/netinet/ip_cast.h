//==========================================================================
//
//      include/netinet/ip_cast.h
//
//      
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
// Date:         2000-01-10
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================


/*      $OpenBSD: ip_cast.h,v 1.3 1999/02/17 18:09:55 deraadt Exp $       */

/*
 *	CAST-128 in C
 *	Written by Steve Reid <sreid@sea-to-sky.net>
 *	100% Public Domain - no warranty
 *	Released 1997.10.11
 */

#ifndef _NETINET_IP_CAST_H_
#define _NETINET_IP_CAST_H_

typedef struct {
	u_int32_t	xkey[32];	/* Key, after expansion */
	int		rounds;		/* Number of rounds to use, 12 or 16 */
} cast_key;

void cast_setkey __P((cast_key * key, u_int8_t * rawkey, int keybytes));
void cast_encrypt __P((cast_key * key, u_int8_t * inblock, u_int8_t * outblock));
void cast_decrypt __P((cast_key * key, u_int8_t * inblock, u_int8_t * outblock));

#endif /* ifndef _NETINET_IP_CAST_H_ */
