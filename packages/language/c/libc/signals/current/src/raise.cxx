//========================================================================
//
//      raise.cxx
//
//      ISO C and POSIX 1003.1 signals implementation for raise()
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-04-18
// Purpose:       Provide implementation of ISO C and POSIX 1003.1 raise()
// Description:   This file provides the linkable symbol version of raise()
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc_signals.h>  // libc signals configuration

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support

// The following lines allow __raise() to be included non-inline which
// actually defines it as a function here
#define CYGPRI_LIBC_SIGNALS_RAISE_INLINE
#ifndef CYGIMP_LIBC_SIGNALS_RAISE_INLINE
# define CYGIMP_LIBC_SIGNALS_RAISE_INLINE
#endif

#include <signal.h>                // Main signals definitions

// GLOBALS

#ifdef CYGDBG_USE_TRACING
cyg_uint8
cyg_libc_signals_raise_trace_level = CYGNUM_LIBC_SIGNALS_RAISE_TRACE_LEVEL;
#endif

// EOF raise.cxx
