//==========================================================================
//
//      ./lib/current/include/snmpusm.h
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
 * snmpusm.h
 *
 * Header file for USM support.
 */

#ifndef SNMPUSM_H
#define SNMPUSM_H

#ifdef __cplusplus
extern "C" {
#endif

#define WILDCARDSTRING "*"

/*
 * General.
 */
#define USM_MAX_ID_LENGTH		1024		/* In bytes. */
#define USM_MAX_SALT_LENGTH		64		/* In BITS. */
#define USM_MAX_KEYEDHASH_LENGTH	128		/* In BITS. */

#define USM_TIME_WINDOW			150


/*
 * Structures.
 */
struct usmStateReference {
	char		*usr_name;
	size_t		 usr_name_length;
	u_char		*usr_engine_id;
	size_t		 usr_engine_id_length;
	oid		*usr_auth_protocol;
	size_t		 usr_auth_protocol_length;
	u_char		*usr_auth_key;
	size_t		 usr_auth_key_length;
	oid		*usr_priv_protocol;
	size_t		 usr_priv_protocol_length;
	u_char		*usr_priv_key;
	size_t		 usr_priv_key_length;
	u_int		 usr_sec_level;
};


/* struct usmUser: a structure to represent a given user in a list */
/* Note: Any changes made to this structure need to be reflected in
   the following functions: */

struct usmUser;
struct usmUser {
   u_char         *engineID;
   size_t          engineIDLen;
   char           *name;
   char           *secName;
   oid            *cloneFrom;
   size_t          cloneFromLen;
   oid            *authProtocol;
   size_t          authProtocolLen;
   u_char         *authKey;
   size_t          authKeyLen;
   oid            *privProtocol;
   size_t          privProtocolLen;
   u_char         *privKey;
   size_t          privKeyLen;
   u_char         *userPublicString;
   int             userStatus;
   int             userStorageType;
   struct usmUser *next;
   struct usmUser *prev;
};



/*
 * Prototypes.
 */
void	usm_set_reportErrorOnUnknownID (int value);

struct usmStateReference *
	usm_malloc_usmStateReference (void);

void	usm_free_usmStateReference (void *old);

int	usm_set_usmStateReference_name (
		struct usmStateReference	*ref,
		char				*name,
		size_t				 name_len);

int	usm_set_usmStateReference_engine_id (
		struct usmStateReference	*ref,
		u_char				*engine_id,
		size_t				 engine_id_len);

int	usm_set_usmStateReference_auth_protocol (
		struct usmStateReference *ref,
		oid *auth_protocol,
		size_t auth_protocol_len);

int	usm_set_usmStateReference_auth_key (
		struct usmStateReference *ref,
		u_char *auth_key,
		size_t auth_key_len);

int	usm_set_usmStateReference_priv_protocol (
		struct usmStateReference *ref,
		oid *priv_protocol,
		size_t priv_protocol_len);

int	usm_set_usmStateReference_priv_key (
		struct usmStateReference *ref,
		u_char *priv_key,
		size_t priv_key_len);

int	usm_set_usmStateReference_sec_level (
		struct usmStateReference *ref,
		int sec_level);

#ifdef SNMP_TESTING_CODE
void	emergency_print (u_char *field, u_int length);
#endif

int	asn_predict_int_length (int type, long number, size_t len);

int	asn_predict_length (int type, u_char *ptr, size_t u_char_len);

int	usm_set_salt (
		u_char		*iv,
		size_t		*iv_length,
		u_char		*priv_salt,
		size_t		 priv_salt_length,
		u_char		*msgSalt );

int	usm_parse_security_parameters (
		u_char  *secParams,
		size_t   remaining,
		u_char  *secEngineID,
		size_t  *secEngineIDLen,
		u_int   *boots_uint,
		u_int   *time_uint,
		char    *secName,
		size_t  *secNameLen,
		u_char  *signature,
		size_t  *signature_length,
		u_char  *salt,
		size_t  *salt_length,
		u_char **data_ptr);

int	usm_check_and_update_timeliness (
		u_char *secEngineID,
		size_t  secEngineIDLen,
		u_int   boots_uint,
		u_int   time_uint,
		int    *error);

int usm_generate_out_msg (int, u_char *, size_t, int, int, u_char *, size_t,
			      char *,  size_t, int, u_char *, size_t, void *,
			      u_char *, size_t *, u_char **, size_t *);

int usm_process_in_msg (int, size_t, u_char *, int, int, u_char *, size_t,
			    u_char *, size_t *, char *, size_t *, u_char **, size_t *,
			    size_t *, void **);

int             usm_check_secLevel(int level, struct usmUser *user);
void            usm_update_engine_time(void);
struct usmUser *usm_get_userList(void);
struct usmUser *usm_get_user(u_char *engineID, size_t engineIDLen, char *name);
struct usmUser *usm_get_user_from_list(u_char *engineID, size_t engineIDLen,
                                       char *name, struct usmUser *userList,
                                       int use_default);
struct usmUser *usm_add_user(struct usmUser *user);
struct usmUser *usm_add_user_to_list(struct usmUser *user,
                                     struct usmUser *userList);
struct usmUser *usm_free_user(struct usmUser *user);
struct usmUser *usm_create_user(void);
struct usmUser *usm_create_initial_user(const char *name,
                                     oid *authProtocol, size_t authProtocolLen,
                                     oid *privProtocol, size_t privProtocolLen);
struct usmUser *usm_cloneFrom_user(struct usmUser *from, struct usmUser *to);
struct usmUser *usm_remove_user(struct usmUser *user);
struct usmUser *usm_remove_user_from_list(struct usmUser *user,
                                          struct usmUser **userList);
char           *get_objid(char *line, oid **optr, size_t *len);
void            usm_save_users(const char *token, const char *type);
void            usm_save_users_from_list(struct usmUser *user, const char *token,
                                        const char *type);
void            usm_save_user(struct usmUser *user, const char *token, const char *type);
SNMPCallback    usm_store_users;
struct usmUser *usm_read_user(char *line);
void            usm_parse_config_usmUser(const char *token, char *line);

void            usm_set_password(const char *token, char *line);
void            usm_set_user_password(struct usmUser *user, const char *token,
                                      char *line);
void		init_usm(void);
int		init_usm_post_config(int majorid, int minorid, void *serverarg,
                                     void *clientarg);

#ifdef __cplusplus
}
#endif

#endif /* SNMPUSM_H */
