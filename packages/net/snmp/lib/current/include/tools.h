//==========================================================================
//
//      ./lib/current/include/tools.h
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
//####UCDSNMPCOPYRIGHTBEGIN####
//
// -------------------------------------------
//
// Portions of this software may have been derived from the UCD-SNMP
// project,  <http://ucd-snmp.ucdavis.edu/>  from the University of
// California at Davis, which was originally based on the Carnegie Mellon
// University SNMP implementation.  Portions of this software are therefore
// covered by the appropriate copyright disclaimers included herein.
//
// The release used was version 4.1.2 of May 2000.  "ucd-snmp-4.1.2"
// -------------------------------------------
//
//####UCDSNMPCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    hmt
// Contributors: hmt
// Date:         2000-05-30
// Purpose:      Port of UCD-SNMP distribution to eCos.
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================
/********************************************************************
       Copyright 1989, 1991, 1992 by Carnegie Mellon University

			  Derivative Work -
Copyright 1996, 1998, 1999, 2000 The Regents of the University of California

			 All Rights Reserved

Permission to use, copy, modify and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appears in all copies and
that both that copyright notice and this permission notice appear in
supporting documentation, and that the name of CMU and The Regents of
the University of California not be used in advertising or publicity
pertaining to distribution of the software without specific written
permission.

CMU AND THE REGENTS OF THE UNIVERSITY OF CALIFORNIA DISCLAIM ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL CMU OR
THE REGENTS OF THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR ANY SPECIAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
FROM THE LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*********************************************************************/
/*
 * tools.h
 */

#ifndef _TOOLS_H
#define _TOOLS_H

#ifdef __cplusplus
extern "C" {
#endif



/* 
 * General acros and constants.
 */
#ifdef WIN32
#  define SNMP_MAXPATH MAX_PATH
#else
#  ifdef PATH_MAX
#    define SNMP_MAXPATH PATH_MAX
#  else
#    define SNMP_MAXPATH MAXPATHLEN
#  endif
#endif

#define SNMP_MAXBUF		(1024 * 4)
#define SNMP_MAXBUF_MEDIUM	1024
#define SNMP_MAXBUF_SMALL	512

#define SNMP_MAXBUF_MESSAGE	1500

#define SNMP_MAXOID		64

#define SNMP_FILEMODE_CLOSED	0600
#define SNMP_FILEMODE_OPEN	0644

#define BYTESIZE(bitsize)       ((bitsize + 7) >> 3)
#define ROUNDUP8(x)		( ( (x+7) >> 3 ) * 8 )



#define SNMP_FREE(s)		if (s) { free((void *)s); s=NULL; }

					/* XXX Not optimal everywhere. */
#define SNMP_MALLOC_STRUCT(s)   (struct s *) calloc(1, sizeof(struct s))
#define SNMP_ZERO(s,l)		if (s) memset(s, 0, l);


#define TOUPPER(c)	(c >= 'a' && c <= 'z' ? c - ('a' - 'A') : c)
#define TOLOWER(c)	(c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c)

#define HEX2VAL(s) \
	((isalpha(s) ? (TOLOWER(s)-'a'+10) : (TOLOWER(s)-'0')) & 0xf)
#define VAL2HEX(s)	( (s) + (((s) >= 10) ? ('a'-10) : '0') )


#define SNMP_MAX(a,b) ((a) > (b) ? (a) : (b))
#define SNMP_MIN(a,b) ((a) > (b) ? (b) : (a))

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif

/*
 * QUIT the FUNction:
 *	e	Error code variable
 *	l	Label to goto to cleanup and get out of the function.
 *
 * XXX	It would be nice if the label could be constructed by the
 *	preprocessor in context.  Limited to a single error return value.
 *	Temporary hack at best.
 */
#define QUITFUN(e, l)			\
	if ( (e) != SNMPERR_SUCCESS) {	\
		rval = SNMPERR_GENERR;	\
		goto l ;		\
	}

/*
 * DIFFTIMEVAL
 *	Set <diff> to the difference between <now> (current) and <then> (past).
 *
 * ASSUMES that all inputs are (struct timeval)'s.
 * Cf. system.c:calculate_time_diff().
 */
#define DIFFTIMEVAL(now, then, diff) 			\
{							\
	now.tv_sec--;					\
	now.tv_usec += 1000000L;			\
	diff.tv_sec  = now.tv_sec  - then.tv_sec;	\
	diff.tv_usec = now.tv_usec - then.tv_usec;	\
	if (diff.tv_usec > 1000000L){			\
		diff.tv_usec -= 1000000L;		\
		diff.tv_sec++;				\
	}						\
}


/*
 * ISTRANSFORM
 * ASSUMES the minimum length for ttype and toid.
 */
#define USM_LENGTH_OID_TRANSFORM	10

#define ISTRANSFORM(ttype, toid)					\
	!snmp_oid_compare(ttype, USM_LENGTH_OID_TRANSFORM,		\
		usm ## toid ## Protocol, USM_LENGTH_OID_TRANSFORM)

#define ENGINETIME_MAX	2147483647	/* ((2^31)-1) */
#define ENGINEBOOT_MAX	2147483647	/* ((2^31)-1) */




/* 
 * Prototypes.
 */
void	free_zero (void *buf, size_t size);

u_char *malloc_random (size_t *size);
u_char *malloc_zero (size_t size);
int     memdup (u_char **to, const u_char *from, size_t size);

u_int	binary_to_hex (const u_char *input, size_t len, char **output);
int	hex_to_binary2 (const u_char *input, size_t len, char **output);

void	dump_chunk (const char *debugtoken, const char *title, const u_char *buf, int size);
char   *dump_snmpEngineID (const u_char *buf, size_t *buflen);

typedef void * marker_t;
marker_t atime_newMarker(void);
void atime_setMarker(marker_t pm);
int atime_ready( marker_t pm, int deltaT);

#ifdef __cplusplus
}
#endif

#endif /* _TOOLS_H */
