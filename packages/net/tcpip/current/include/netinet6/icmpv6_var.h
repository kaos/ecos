//==========================================================================
//
//      include/netinet6_icmpv6_var.h
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


/*
%%% copyright-nrl-95
This software is Copyright 1995-1998 by Randall Atkinson, Ronald Lee,
Daniel McDonald, Bao Phan, and Chris Winters. All Rights Reserved. All
rights under this copyright have been assigned to the US Naval Research
Laboratory (NRL). The NRL Copyright Notice and License Agreement Version
1.1 (January 17, 1995) applies to this software.
You should have received a copy of the license with this software. If you
didn't get a copy, you may request one from <license@ipv6.nrl.navy.mil>.

*/

#ifndef _NETINET6_ICMPV6_VAR_H
#define _NETINET6_ICMPV6_VAR_H 1

#if defined(_BSDI_VERSION) && _BSDI_VERSION >= 199802
#define _ICMPV6STAT_TYPE u_quad_t
#else /* defined(_BSDI_VERSION) && _BSDI_VERSION >= 199802 */
#define _ICMPV6STAT_TYPE u_long
#endif /* defined(_BSDI_VERSION) && _BSDI_VERSION >= 199802 */

struct icmpv6stat
{
  _ICMPV6STAT_TYPE icps_error;
  _ICMPV6STAT_TYPE icps_tooshort;
  _ICMPV6STAT_TYPE icps_checksum;
  _ICMPV6STAT_TYPE icps_outhist[ICMPV6_MAXTYPE+1];
  _ICMPV6STAT_TYPE icps_badlen;
  _ICMPV6STAT_TYPE icps_badcode;
  _ICMPV6STAT_TYPE icps_reflect;    /* Number of in-kernel responses */
  _ICMPV6STAT_TYPE icps_inhist[ICMPV6_MAXTYPE+1];
};

/*
 * Names for ICMPV6 sysctl objects
 */
#define	ICMPV6CTL_STATS		1	/* statistics */
#define ICMPV6CTL_MAXID		2

#define ICMPV6CTL_NAMES { \
	{ 0, 0 }, \
	{ "stats", CTLTYPE_STRUCT }, \
}

#define	ICMPV6CTL_VARS { \
	0, \
	0, \
}

#if defined(_KERNEL) || defined(KERNEL)
struct	icmpv6stat icmpv6stat;
#endif /* defined(_KERNEL) || defined(KERNEL) */
#endif /* _NETINET6_ICMPV6_VAR_H */
