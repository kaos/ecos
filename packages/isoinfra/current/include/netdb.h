#ifndef CYGONCE_ISO_NETDB_H
#define CYGONCE_ISO_NETDB_H
/*==========================================================================
//
//      netdb.h
//
//      Network database functions
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
// Author(s):     jskov,jlarmour
// Contributors: 
// Date:          2001-09-28
// Purpose:       Provides network database types and function API.
// Description:   Much of the real contents of this file get set from the
//                configuration (set by the implementation)
// Usage:         #include <netdb.h>
//              
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/isoinfra.h>

#ifdef CYGINT_ISO_DNS
# ifdef CYGBLD_ISO_DNS_HEADER
#  include CYGBLD_ISO_DNS_HEADER
# endif
#endif

#ifdef CYGINT_ISO_NETDB_PROTO
# ifdef CYGBLD_ISO_NETDB_PROTO_HEADER
#  include CYGBLD_ISO_NETDB_PROTO_HEADER
# endif
#endif

#ifdef CYGINT_ISO_NETDB_SERV
# ifdef CYGBLD_ISO_NETDB_SERV_HEADER
#  include CYGBLD_ISO_NETDB_SERV_HEADER
# endif
#endif

#endif /* CYGONCE_ISO_NETDB_H multiple inclusion protection */

/* EOF netdb.h */
