//=============================================================================
//
//      pci_hw.c
//
//      PCI hardware library
//
//=============================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Description: 
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/io/pci_hw.h>

// CYG_PCI_PRESENT only gets defined for targets that provide PCI HAL support.
// See pci_hw.h for details.
#ifdef CYG_PCI_PRESENT

// Init
void 
cyg_pcihw_init(void)
{
    HAL_PCI_INIT();
}

// Read functions
void 
cyg_pcihw_read_config_uint8( cyg_uint8 bus, cyg_uint8 devfn, 
                             cyg_uint8 offset, cyg_uint8 *val)
{
    HAL_PCI_CFG_READ_UINT8(bus, devfn, offset, *val);
}

void 
cyg_pcihw_read_config_uint16( cyg_uint8 bus, cyg_uint8 devfn, 
                              cyg_uint8 offset, cyg_uint16 *val)
{
    HAL_PCI_CFG_READ_UINT16(bus, devfn, offset, *val);
}

void
cyg_pcihw_read_config_uint32( cyg_uint8 bus, cyg_uint8 devfn, 
                              cyg_uint8 offset, cyg_uint32 *val)
{
    HAL_PCI_CFG_READ_UINT32(bus, devfn, offset, *val);
}

// Write functions
void 
cyg_pcihw_write_config_uint8( cyg_uint8 bus, cyg_uint8 devfn, 
                             cyg_uint8 offset, cyg_uint8 val)
{
    HAL_PCI_CFG_WRITE_UINT8(bus, devfn, offset, val);
}

void 
cyg_pcihw_write_config_uint16( cyg_uint8 bus, cyg_uint8 devfn, 
                              cyg_uint8 offset, cyg_uint16 val)
{
    HAL_PCI_CFG_WRITE_UINT16(bus, devfn, offset, val);
}

void
cyg_pcihw_write_config_uint32( cyg_uint8 bus, cyg_uint8 devfn, 
                              cyg_uint8 offset, cyg_uint32 val)
{
    HAL_PCI_CFG_WRITE_UINT32(bus, devfn, offset, val);
}

// Interrupt translation
cyg_bool
cyg_pcihw_translate_interrupt( cyg_uint8 bus, cyg_uint8 devfn,
                               CYG_ADDRWORD *vec)
{
    cyg_bool valid;

    HAL_PCI_TRANSLATE_INTERRUPT(bus, devfn, *vec, valid);

    return valid;
}

#endif // ifdef CYG_PCI_PRESENT

//-----------------------------------------------------------------------------
// end of pci_hw.c
