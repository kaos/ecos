//==========================================================================
//
//      flash_config.h
//
//      Flash configuration data tables for RedBoot
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
// Date:         2000-08-21
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef _FLASH_CONFIG_H_
#define _FLASH_CONFIG_H_

#define MAX_SCRIPT_LENGTH 512

#define CONFIG_BOOL    1
#define CONFIG_INT     2
#define CONFIG_STRING  3
#define CONFIG_SCRIPT  4
#ifdef CYGPKG_REDBOOT_NETWORKING
#define CONFIG_IP      5
#define CONFIG_ESA     6
#endif

struct config_option {
    char *key;
    char *title;
    char *enable;
    bool  enable_sense;
    int   type;
};

#define ALWAYS_ENABLED (char *)0

#define RedBoot_config_option(_t_,_n_,_e_,_ie_,_type_)                                  \
struct config_option _config_option_##_n_                                               \
CYG_HAL_TABLE_ENTRY(RedBoot_config_options) = {#_n_,_t_,/*&_v_,*/_e_,_ie_,_type_};

void flash_get_config(char *key, void *val, int type);

#endif // _FLASH_CONFIG_H_
