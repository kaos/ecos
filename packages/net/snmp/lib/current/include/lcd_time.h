//==========================================================================
//
//      ./lib/current/include/lcd_time.h
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
 * lcd_time.h
 */

#ifndef _LCD_TIME_H
#define _LCD_TIME_H

#ifdef __cplusplus
extern "C" {
#endif


/* undefine to enable time synchronization only on authenticated packets */
#define LCD_TIME_SYNC_OPT 1

/*
 * Macros and definitions.
 */
#define ETIMELIST_SIZE	23



typedef struct enginetime_struct {
	u_char		*engineID;	
	u_int		 engineID_len;

	u_int		 engineTime;	
	u_int		 engineBoot;
		/* Time & boots values received from last authenticated
		 *   message within the previous time window.
		 */

	time_t		 lastReceivedEngineTime;
		/* Timestamp made when engineTime/engineBoots was last
		 *   updated.  Measured in seconds.
		 */

#ifdef LCD_TIME_SYNC_OPT	
        u_int  authenticatedFlag;
#endif      
	struct enginetime_struct	*next;
} enginetime, *Enginetime;




/*
 * Macros for streamlined engineID existence checks --
 *
 *	e	is char *engineID,
 *	e_l	is u_int engineID_len.
 *
 *
 *  ISENGINEKNOWN(e, e_l)
 *	Returns:
 *		TRUE	If engineID is recoreded in the EngineID List;
 *		FALSE	Otherwise.
 *
 *  ENSURE_ENGINE_RECORD(e, e_l)
 *	Adds the given engineID to the EngineID List if it does not exist
 *		already.  engineID is added with a <enginetime, engineboots>
 *		tuple of <0,0>.  ALWAYS succeeds -- except in case of a
 *		fatal internal error.
 *	Returns:
 *		SNMPERR_SUCCESS	On success;
 *		SNMPERR_GENERR	Otherwise.
 *
 *  MAKENEW_ENGINE_RECORD(e, e_l)
 *	Returns:
 *		SNMPERR_SUCCESS	If engineID already exists in the EngineID List;
 *		SNMPERR_GENERR	Otherwise -and- invokes ENSURE_ENGINE_RECORD()
 *					to add an entry to the EngineID List.
 *
 * XXX  Requres the following declaration in modules calling ISENGINEKNOWN():
 *	static u_int	dummy_etime, dummy_eboot;
 */
#define ISENGINEKNOWN(e, e_l)					\
	( (get_enginetime(e, e_l,				\
		&dummy_eboot, &dummy_etime, TRUE) == SNMPERR_SUCCESS)	\
		? TRUE						\
		: FALSE )

#define ENSURE_ENGINE_RECORD(e, e_l)				\
	( (set_enginetime(e, e_l, 0, 0, FALSE) == SNMPERR_SUCCESS)	\
		? SNMPERR_SUCCESS				\
		: SNMPERR_GENERR )

#define MAKENEW_ENGINE_RECORD(e, e_l)				\
	( (ISENGINEKNOWN(e, e_l) == TRUE)			\
		? SNMPERR_SUCCESS				\
		: (ENSURE_ENGINE_RECORD(e, e_l), SNMPERR_GENERR) )



/*
 * Prototypes.
 */
int	 get_enginetime (u_char *engineID, u_int  engineID_len,
			     u_int *engine_boot, u_int *engine_time,
			     u_int authenticated);

int	 get_enginetime_ex (u_char *engineID, u_int  engineID_len,
			    u_int *engine_boot, u_int *engine_time,
			    u_int *last_engine_time,
			    u_int authenticated);

int	 set_enginetime (u_char *engineID, u_int engineID_len,
			     u_int   engine_boot, u_int engine_time,
			     u_int authenticated);

Enginetime
	 search_enginetime_list (	u_char		*engineID,
					u_int		 engineID_len);

int	 hash_engineID (u_char *engineID, u_int engineID_len);

void	 dump_etimelist_entry (Enginetime e, int count);
void	 dump_etimelist (void);

#ifdef __cplusplus
}
#endif

#endif /* _LCD_TIME_H */
