#ifndef CYGONCE_PLF_IO_H
#define CYGONCE_PLF_IO_H

//=============================================================================
//
//      plf_io.h
//
//      Platform specific IO support
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
// Author(s):    msalter
// Contributors: hmt, jskov, msalter
// Date:         2000-10-10
// Purpose:      Intel IQ80310 PCI IO support macros
// Description: 
// Usage:        #include <cyg/hal/plf_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal_arm_iq80310.h>

#include <cyg/hal/hal_iq80310.h>

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_platform_ints.h>  // Interrupt vectors

extern cyg_uint32 cyg_hal_plf_pci_cfg_read_dword (cyg_uint32 bus,
						  cyg_uint32 devfn,
						  cyg_uint32 offset);
extern cyg_uint16 cyg_hal_plf_pci_cfg_read_word  (cyg_uint32 bus,
						  cyg_uint32 devfn,
						  cyg_uint32 offset);
extern cyg_uint8 cyg_hal_plf_pci_cfg_read_byte   (cyg_uint32 bus,
						  cyg_uint32 devfn,
						  cyg_uint32 offset);
extern void cyg_hal_plf_pci_cfg_write_dword (cyg_uint32 bus,
					     cyg_uint32 devfn,
					     cyg_uint32 offset,
					     cyg_uint32 val);
extern void cyg_hal_plf_pci_cfg_write_word  (cyg_uint32 bus,
					     cyg_uint32 devfn,
					     cyg_uint32 offset,
					     cyg_uint16 val);
extern void cyg_hal_plf_pci_cfg_write_byte   (cyg_uint32 bus,
					      cyg_uint32 devfn,
					      cyg_uint32 offset,
					      cyg_uint8 val);

/* primary PCI bus definitions */ 
#define PRIMARY_BUS_NUM		0
#define PRIMARY_MEM_BASE	0x80000000
#define PRIMARY_DAC_BASE	0x84000000
#define PRIMARY_IO_BASE		0x90000000
#define PRIMARY_MEM_LIMIT	0x83ffffff
#define PRIMARY_DAC_LIMIT	0x87ffffff
#define PRIMARY_IO_LIMIT	0x9000ffff


/* secondary PCI bus definitions */
#define	SECONDARY_BUS_NUM	1
#define SECONDARY_MEM_BASE	0x88000000
#define SECONDARY_DAC_BASE	0x8c000000
#define SECONDARY_IO_BASE	0x90010000
#define SECONDARY_MEM_LIMIT	0x8bffffff
#define SECONDARY_DAC_LIMIT	0x8fffffff
#define SECONDARY_IO_LIMIT	0x9001ffff

// Initialize the PCI bus.
externC void cyg_hal_plf_pci_init(void);
#define HAL_PCI_INIT() cyg_hal_plf_pci_init()

// Read a value from the PCI configuration space of the appropriate
// size at an address composed from the bus, devfn and offset.
#define HAL_PCI_CFG_READ_UINT8( __bus, __devfn, __offset, __val )  \
    __val = cyg_hal_plf_pci_cfg_read_byte((__bus),  (__devfn), (__offset))
    
#define HAL_PCI_CFG_READ_UINT16( __bus, __devfn, __offset, __val ) \
    __val = cyg_hal_plf_pci_cfg_read_word((__bus),  (__devfn), (__offset))

#define HAL_PCI_CFG_READ_UINT32( __bus, __devfn, __offset, __val ) \
    __val = cyg_hal_plf_pci_cfg_read_dword((__bus),  (__devfn), (__offset))

// Write a value to the PCI configuration space of the appropriate
// size at an address composed from the bus, devfn and offset.
#define HAL_PCI_CFG_WRITE_UINT8( __bus, __devfn, __offset, __val )  \
    cyg_hal_plf_pci_cfg_write_byte((__bus),  (__devfn), (__offset), (__val))

#define HAL_PCI_CFG_WRITE_UINT16( __bus, __devfn, __offset, __val ) \
    cyg_hal_plf_pci_cfg_write_word((__bus),  (__devfn), (__offset), (__val))

#define HAL_PCI_CFG_WRITE_UINT32( __bus, __devfn, __offset, __val ) \
    cyg_hal_plf_pci_cfg_write_dword((__bus),  (__devfn), (__offset), (__val))

//-----------------------------------------------------------------------------
// Resources

// Map PCI device resources starting from these addresses in PCI space.
#define HAL_PCI_ALLOC_BASE_MEMORY (SECONDARY_MEM_BASE)
#define HAL_PCI_ALLOC_BASE_IO     (SECONDARY_IO_BASE)

// This is where the PCI spaces are mapped in the CPU's address space.
#define HAL_PCI_PHYSICAL_MEMORY_BASE    0x00000000
#define HAL_PCI_PHYSICAL_IO_BASE        0x00000000

// Translate the PCI interrupt requested by the device (INTA#, INTB#,
// INTC# or INTD#) to the associated CPU interrupt (i.e., HAL vector).
#define HAL_PCI_TRANSLATE_INTERRUPT( __bus, __devfn, __vec, __valid)          \
    CYG_MACRO_START                                                           \
    cyg_uint32 __dev = CYG_PCI_DEV_GET_DEV(__devfn);                          \
    cyg_uint32 __fn = CYG_PCI_DEV_GET_FN(__devfn);                            \
    __valid = false;                                                          \
    if (__bus == (*((cyg_uint8 *)SBNR_ADDR) + 1) && __dev == 0 && __fn == 0) {\
        __vec = CYGNUM_HAL_INTERRUPT_ETHERNET;                                \
        __valid = true;                                                       \
    } else {                                                                  \
        cyg_uint8 __req;                                                      \
        HAL_PCI_CFG_READ_UINT8(__bus, __devfn, CYG_PCI_CFG_INT_PIN, __req);   \
        switch (__dev % 4) {                                                  \
          case 0:                                                             \
            switch(__req) {                                                   \
              case 1: /* INTA */                                              \
                __vec=CYGNUM_HAL_INTERRUPT_PCI_S_INTA; __valid=true; break;   \
              case 2: /* INTB */                                              \
                __vec=CYGNUM_HAL_INTERRUPT_PCI_S_INTB; __valid=true; break;   \
              case 3: /* INTC */                                              \
                __vec=CYGNUM_HAL_INTERRUPT_PCI_S_INTC; __valid=true; break;   \
              case 4: /* INTD */                                              \
                __vec=CYGNUM_HAL_INTERRUPT_PCI_S_INTD; __valid=true; break;   \
            }                                                                 \
 	    break;                                                            \
          case 1:                                                             \
            switch(__req) {                                                   \
              case 1: /* INTA */                                              \
                __vec=CYGNUM_HAL_INTERRUPT_PCI_S_INTB; __valid=true; break;   \
              case 2: /* INTB */                                              \
                __vec=CYGNUM_HAL_INTERRUPT_PCI_S_INTC; __valid=true; break;   \
              case 3: /* INTC */                                              \
                __vec=CYGNUM_HAL_INTERRUPT_PCI_S_INTD; __valid=true; break;   \
              case 4: /* INTD */                                              \
                __vec=CYGNUM_HAL_INTERRUPT_PCI_S_INTA; __valid=true; break;   \
            }                                                                 \
 	    break;                                                            \
          case 2:                                                             \
            switch(__req) {                                                   \
              case 1: /* INTA */                                              \
                __vec=CYGNUM_HAL_INTERRUPT_PCI_S_INTC; __valid=true; break;   \
              case 2: /* INTB */                                              \
                __vec=CYGNUM_HAL_INTERRUPT_PCI_S_INTD; __valid=true; break;   \
              case 3: /* INTC */                                              \
                __vec=CYGNUM_HAL_INTERRUPT_PCI_S_INTA; __valid=true; break;   \
              case 4: /* INTD */                                              \
                __vec=CYGNUM_HAL_INTERRUPT_PCI_S_INTB; __valid=true; break;   \
            }                                                                 \
 	    break;                                                            \
          case 3:                                                             \
            switch(__req) {                                                   \
              case 1: /* INTA */                                              \
                __vec=CYGNUM_HAL_INTERRUPT_PCI_S_INTD; __valid=true; break;   \
              case 2: /* INTB */                                              \
                __vec=CYGNUM_HAL_INTERRUPT_PCI_S_INTA; __valid=true; break;   \
              case 3: /* INTC */                                              \
                __vec=CYGNUM_HAL_INTERRUPT_PCI_S_INTB; __valid=true; break;   \
              case 4: /* INTD */                                              \
                __vec=CYGNUM_HAL_INTERRUPT_PCI_S_INTC; __valid=true; break;   \
            }                                                                 \
 	    break;                                                            \
        }                                                                     \
    }                                                                         \
    CYG_MACRO_END

//-----------------------------------------------------------------------------
// end of plf_io.h
#endif // CYGONCE_PLF_IO_H
