#ifndef _NETDEV_H_
#define _NETDEV_H_
//==========================================================================
//
//      include/netdev.h
//
//      Network device description
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
// Date:         2000-01-10
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/hal/hal_tables.h>

// Network device support

typedef struct cyg_netdevtab_entry {
    const char        *name;
    bool             (*init)(struct cyg_netdevtab_entry *tab);
    void              *device_instance;  // Local data, instance specific
    unsigned long     status;
} CYG_HAL_TABLE_TYPE cyg_netdevtab_entry_t;

#define CYG_NETDEVTAB_STATUS_AVAIL   0x0001

extern cyg_netdevtab_entry_t __NETDEVTAB__[], __NETDEVTAB_END__;

#define NETDEVTAB_ENTRY(_l,_name,_init,_instance)  \
static bool _init(struct cyg_netdevtab_entry *tab);                  \
cyg_netdevtab_entry_t _l CYG_HAL_TABLE_ENTRY(netdev) = {             \
   _name,                                                            \
   _init,                                                            \
   _instance                                                         \
};

#endif // _NETDEV_H_
