//==========================================================================
//
//      include/netinet/pim6.h
//
//      IPv6 protocol independent multicast
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

/*	$OpenBSD: pim6.h,v 1.1 1999/12/08 06:50:23 itojun Exp $	*/

/*
 * Copyright (C) 1998 WIDE Project.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _NETINET_PIM6_H_
#define _NETINET_PIM6_H_

/*
 * Protocol Independent Multicast (PIM) definitions
 *
 * Written by Ahmed Helmy, SGI, July 1996
 *
 * MULTICAST
 */

/*
 * PIM packet header
 */
#define PIM_VERSION	2
struct pim {
#if defined(BYTE_ORDER) && (BYTE_ORDER == LITTLE_ENDIAN)
	u_char	pim_type:4, /* the PIM message type, currently they are:
			     * Hello, Register, Register-Stop, Join/Prune,
			     * Bootstrap, Assert, Graft (PIM-DM only),
			     * Graft-Ack (PIM-DM only), C-RP-Adv
			     */
		pim_ver:4;  /* PIM version number; 2 for PIMv2 */
#else
	u_char	pim_ver:4,	/* PIM version */
		pim_type:4;	/* PIM type    */
#endif
	u_char  pim_rsv;	/* Reserved */
	u_short	pim_cksum;	/* IP style check sum */
};

#define PIM_MINLEN	8		/* The header min. length is 8    */
#define PIM6_REG_MINLEN	(PIM_MINLEN+40)	/* Register message + inner IP6 header */

/* 
 * Message types
 */
#define PIM_REGISTER	1	/* PIM Register type is 1 */

/* second bit in reg_head is the null bit */
#define PIM_NULL_REGISTER 0x40000000

#endif // _NETINET_PIM6_H_
