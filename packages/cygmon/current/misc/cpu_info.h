#ifndef __CYGMON_CPU_INFO_H__
#define __CYGMON_CPU_INFO_H__
//==========================================================================
//
//      cpu_info.h
//
//      Architecture specific information
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
// Date:         1999-10-20
// Purpose:      
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================
#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#ifdef CYGPKG_HAL_ARM
#include "arm/cpu_info.h"
#endif

#ifdef CYGPKG_HAL_MIPS
#include "mips/cpu_info.h"
#endif
#endif // __CYGMON_CPU_INFO_H__
