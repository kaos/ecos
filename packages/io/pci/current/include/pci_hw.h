#ifndef CYGONCE_PCI_HW_H
#define CYGONCE_PCI_HW_H
//=============================================================================
//
//      pci_hw.h
//
//      PCI hardware library
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jskov, from design by nickg 
// Contributors: jskov
// Date:         1999-08-09
// Purpose:      PCI hardware configuration access
// Usage:
//              #include <cyg/io/pci_hw.h>
// Description: 
//          This API is used by the PCI library to access the PCI bus
//          configuration space. Although this should not normally be
//          necessary, this API may also be used by device driver or 
//          application code to perform PCI bus operations not supported
//          by the PCI library.
//
//####DESCRIPTIONEND####
//
//=============================================================================

// Rely on hal_io.h to include plf_io.h if it exists for the selected target.
#include <cyg/hal/hal_io.h>             // HAL_PCI_ macros

#ifdef HAL_PCI_INIT

// This varible selects whether the PCI library gets built (requires
// HAL_PCI_INIT to be defined by the platform io header file)
#define CYG_PCI_PRESENT

#include <cyg/infra/cyg_type.h>

#include <cyg/io/pci_cfg.h>


// This is the lowest level where devfns are used.
#define CYG_PCI_DEV_MAKE_DEVFN(__dev, __fn) (((__dev)<<3)|(__fn))
#define CYG_PCI_DEV_GET_DEV(__devfn) ((__devfn>>3)&0x1f)
#define CYG_PCI_DEV_GET_FN(__devfn) (__devfn&0x7)

// Some buggy PCI chips force us to ignore certain devices so that
// they may be handled specially.
#ifdef HAL_PCI_IGNORE_DEVICE
#define CYG_PCI_IGNORE_DEVICE(__bus, __dev, __fn) \
            HAL_PCI_IGNORE_DEVICE((__bus), (__dev), (__fn))
#else
#define CYG_PCI_IGNORE_DEVICE(__bus, __dev, __fn) 0
#endif


// Init
externC void cyg_pcihw_init(void);

// Read functions
externC void cyg_pcihw_read_config_uint8( cyg_uint8 bus, cyg_uint8 devfn,
                                          cyg_uint8 offset, cyg_uint8 *val);
externC void cyg_pcihw_read_config_uint16( cyg_uint8 bus, cyg_uint8 devfn,
                                           cyg_uint8 offset, cyg_uint16 *val);
externC void cyg_pcihw_read_config_uint32( cyg_uint8 bus, cyg_uint8 devfn,
                                           cyg_uint8 offset, cyg_uint32 *val);

// Write functions
externC void cyg_pcihw_write_config_uint8( cyg_uint8 bus, cyg_uint8 devfn,
                                           cyg_uint8 offset, cyg_uint8 val);
externC void cyg_pcihw_write_config_uint16( cyg_uint8 bus, cyg_uint8 devfn,
                                            cyg_uint8 offset, cyg_uint16 val);
externC void cyg_pcihw_write_config_uint32( cyg_uint8 bus, cyg_uint8 devfn,
                                            cyg_uint8 offset, cyg_uint32 val);

// Interrupt translation
externC cyg_bool cyg_pcihw_translate_interrupt( cyg_uint8 bus, cyg_uint8 devfn,
                                                CYG_ADDRWORD *vec);

#endif // ifdef HAL_PCI_INIT

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_PCI_HW_H
// End of pci_hw.h

