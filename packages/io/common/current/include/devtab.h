#ifndef CYGONCE_IO_DEVTAB_H
#define CYGONCE_IO_DEVTAB_H
// ====================================================================
//
//      devtab.h
//
//      Device I/O Table
//
// ====================================================================
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
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   gthomas
// Contributors:        gthomas
// Date:        1999-02-04
// Purpose:     Describe low level I/O interfaces.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

// Private include file.  This file should only be used by device 
// drivers, not application code.

#include <pkgconf/system.h>
#include <cyg/io/io.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_tables.h>

// Set of functions which handle top level I/O functions
typedef struct {
    Cyg_ErrNo (*write)(cyg_io_handle_t handle, 
                       const void *buf, 
                       cyg_uint32 *len);
    Cyg_ErrNo (*read)(cyg_io_handle_t handle, 
                      void *buf, 
                      cyg_uint32 *len);
    Cyg_ErrNo (*bwrite)(cyg_io_handle_t handle, 
                       const void *buf, 
                       cyg_uint32 *len,
                       cyg_uint32 pos);
    Cyg_ErrNo (*bread)(cyg_io_handle_t handle, 
                      void *buf, 
                      cyg_uint32 *len,
                      cyg_uint32 pos);
    cyg_bool  (*select)(cyg_io_handle_t handle,
                        cyg_uint32 which,
                        CYG_ADDRWORD info);
    Cyg_ErrNo (*get_config)(cyg_io_handle_t handle, 
                            cyg_uint32 key, 
                            void *buf, 
                            cyg_uint32 *len);
    Cyg_ErrNo (*set_config)(cyg_io_handle_t handle, 
                            cyg_uint32 key, 
                            const void *buf, 
                            cyg_uint32 *len);
} cyg_devio_table_t;


// Default functions

__externC Cyg_ErrNo cyg_devio_cwrite(cyg_io_handle_t handle, 
                                     const void *buf, cyg_uint32 *len);
__externC Cyg_ErrNo cyg_devio_cread(cyg_io_handle_t handle, 
                                    void *buf, cyg_uint32 *len);
__externC Cyg_ErrNo cyg_devio_bwrite(cyg_io_handle_t handle, 
                                     const void *buf, cyg_uint32 *len,
                                     cyg_uint32 pos);
__externC Cyg_ErrNo cyg_devio_bread(cyg_io_handle_t handle, 
                                    void *buf, cyg_uint32 *len,
                                    cyg_uint32 pos);


// Initialization macros

#define CHAR_DEVIO_TABLE(_l,_write,_read,_select,_get_config,_set_config)    \
cyg_devio_table_t _l = {                                        \
    _write,                                                     \
    _read,                                                      \
    cyg_devio_bwrite,                                           \
    cyg_devio_bread,                                            \
    _select,                                                    \
    _get_config,                                                \
    _set_config,                                                \
};

#define BLOCK_DEVIO_TABLE(_l,_bwrite,_bread,_select,_get_config,_set_config)    \
cyg_devio_table_t _l = {                                        \
    cyg_devio_cwrite,                                           \
    cyg_devio_cread,                                            \
    _bwrite,                                                    \
    _bread,                                                     \
    _select,                                                    \
    _get_config,                                                \
    _set_config,                                                \
};

#define DEVIO_TABLE(_l,_write,_read,_select,_get_config,_set_config) \
        CHAR_DEVIO_TABLE(_l,_write,_read,_select,_get_config,_set_config)

typedef struct cyg_devtab_entry {
    const char        *name;
    const char        *dep_name;
    cyg_devio_table_t *handlers;
    bool             (*init)(struct cyg_devtab_entry *tab);
    Cyg_ErrNo        (*lookup)(struct cyg_devtab_entry **tab, 
                               struct cyg_devtab_entry *sub_tab,
                               const char *name);
    void              *priv;
    unsigned long     status;
} cyg_devtab_entry_t CYG_HAL_TABLE_TYPE;

#define CYG_DEVTAB_STATUS_AVAIL   0x0001
#define CYG_DEVTAB_STATUS_CHAR    0x1000
#define CYG_DEVTAB_STATUS_BLOCK   0x2000

extern cyg_devtab_entry_t __DEVTAB__[], __DEVTAB_END__;

#define CHAR_DEVTAB_ENTRY(_l,_name,_dep_name,_handlers,_init,_lookup,_priv)  \
cyg_devtab_entry_t _l CYG_HAL_TABLE_ENTRY(devtab) = {                   \
   _name,                                                               \
   _dep_name,                                                           \
   _handlers,                                                           \
   _init,                                                               \
   _lookup,                                                             \
   _priv,                                                               \
   CYG_DEVTAB_STATUS_CHAR                                               \
};

#define BLOCK_DEVTAB_ENTRY(_l,_name,_dep_name,_handlers,_init,_lookup,_priv)  \
cyg_devtab_entry_t _l CYG_HAL_TABLE_ENTRY(devtab) = {                   \
   _name,                                                               \
   _dep_name,                                                           \
   _handlers,                                                           \
   _init,                                                               \
   _lookup,                                                             \
   _priv,                                                               \
   CYG_DEVTAB_STATUS_BLOCK                                              \
};

#define DEVTAB_ENTRY(_l,_name,_dep_name,_handlers,_init,_lookup,_priv) \
        CHAR_DEVTAB_ENTRY(_l,_name,_dep_name,_handlers,_init,_lookup,_priv)


#define DEVTAB_ENTRY_NO_INIT(_l,_name,_dep_name,_handlers,_init,_lookup,_priv)  \
cyg_devtab_entry_t _l = {                                                       \
   _name,                                                                       \
   _dep_name,                                                                   \
   _handlers,                                                                   \
   _init,                                                                       \
   _lookup,                                                                     \
   _priv,                                                                       \
   CYG_DEVTAB_STATUS_CHAR                                                       \
};

#endif // CYGONCE_IO_DEVTAB_H
