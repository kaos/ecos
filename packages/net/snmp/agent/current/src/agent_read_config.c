//==========================================================================
//
//      ./agent/current/src/agent_read_config.c
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
 * agent_read_config.c
 */

#include <config.h>

#include <sys/types.h>
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include <stdio.h>
#include <ctype.h>
#include <errno.h>

#if TIME_WITH_SYS_TIME
# ifdef WIN32
#  include <sys/timeb.h>
# else
#  include <sys/time.h>
# endif
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if HAVE_NETINET_IN_SYSTM_H
#include <netinet/in_systm.h>
#endif
#if HAVE_NETINET_IP_H
#include <netinet/ip.h>
#endif
#ifdef INET6
#if HAVE_NETINET_IP6_H
#include <netinet/ip6.h>
#endif
#endif
#if HAVE_SYS_QUEUE_H
#include <sys/queue.h>
#endif
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#elif HAVE_WINSOCK_H
#include <winsock.h>
#endif 
#if HAVE_SYS_STREAM_H
#include <sys/stream.h>
#endif
#if HAVE_NET_ROUTE_H
#include <net/route.h>
#endif
#if HAVE_NETINET_IP_VAR_H
#include <netinet/ip_var.h>
#endif
#ifdef INET6
#if HAVE_NETINET6_IP6_VAR_H
#include <netinet6/ip6_var.h>
#endif
#endif
#if HAVE_NETINET_IN_PCB_H
#include <netinet/in_pcb.h>
#endif
#if HAVE_INET_MIB2_H
#include <inet/mib2.h>
#endif

#if HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

#include "mibincl.h"
#include "snmpusm.h"

#include "mibgroup/struct.h"
#include "read_config.h"
#include "agent_read_config.h"
#include "callback.h"
#include "snmp_agent.h"
#include "agent_trap.h"
#include "snmpd.h"
#include "system.h"
#include "snmp_debug.h"
#include "snmp_alarm.h"
#include "default_store.h"
#include "ds_agent.h"
#include "mib_module_includes.h"

char dontReadConfigFiles;
char *optconfigfile;

void init_agent_read_config (const char *app)
{
  if ( app != NULL )
      ds_set_string(DS_LIBRARY_ID, DS_LIB_APPTYPE, app);

  register_app_config_handler("authtrapenable",
                          snmpd_parse_config_authtrap, NULL,
                          "1 | 2\t\t(1 = enable, 2 = disable)");

  if ( ds_get_boolean(DS_APPLICATION_ID, DS_AGENT_ROLE) == MASTER_AGENT ) {
      register_app_config_handler("trapsink",
                          snmpd_parse_config_trapsink, snmpd_free_trapsinks,
                          "host [community]");
      register_app_config_handler("trap2sink",
                          snmpd_parse_config_trap2sink, NULL,
                          "host [community]");
      register_app_config_handler("informsink",
                          snmpd_parse_config_informsink, NULL,
                          "host [community]");
  }
  register_app_config_handler("trapcommunity",
                          snmpd_parse_config_trapcommunity,
                          snmpd_free_trapcommunity,
                          "community-string");
#include "mib_module_dot_conf.h"
#ifdef TESTING
  print_config_handlers();
#endif
}

void update_config(void)
{
  free_config();
  read_configs();
}


void
snmpd_register_config_handler(const char *token,
			      void (*parser) (const char *, char *),
			      void (*releaser) (void),
			      const char *help)
{
  DEBUGMSGTL(("snmpd_register_app_config_handler",
              "registering .conf token for \"%s\"\n", token));
  register_app_config_handler(token, parser, releaser, help);
}

void
snmpd_unregister_config_handler(const char *token)
{
  unregister_app_config_handler(token);
}

/* this function is intended for use by mib-modules to store permenant
   configuration information generated by sets or persistent counters */
void
snmpd_store_config(const char *line)
{
  read_app_config_store(line);
}
