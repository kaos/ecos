#ifndef CYGONCE_HAL_ARM_SA11X0_NANOENGINE_NANOENGINE_H
#define CYGONCE_HAL_ARM_SA11X0_NANOENGINE_NANOENGINE_H

/*=============================================================================
//
//      nano.h
//
//      Platform specific support (register layout, etc)
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas,hmt
// Date:         2001-02-12
// Purpose:      Intel SA1110/NanoEngine platform specific support routines
// Description: 
// Usage:        #include <cyg/hal/nano.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/system.h>
#include CYGHWR_MEMORY_LAYOUT_H

#include <cyg/hal/hal_sa11x0.h>

// 
// Signal assertion levels
//
#define SA1110_LOGIC_ONE(m)  (m & 0xFFFFFFFF)
#define SA1110_LOGIC_ZERO(m) (m & 0x00000000)

//
// SA1110/NanoEngine Control Status registers
//

//
// Special purpose GPIO interrupt mappings
//

//
// GPIO layout
//
// 0-19 unused
// 20-27 unused
// 28-31 not implemented
//
// BUT PCI action can make use of GPIO 21 and 22 - but that's all set up in
// HAL_PCI_INIT() below, as it should be.

#define SA1110_GPIO_GPDR_DEFAULT_VALUE 0 // No GPIO is used

#define SA1110_GPIO_GAFR_DEFAULT_VALUE 0 // No alternates

#define SA1110_GPIO_GRER_DEFAULT_VALUE 0 // No edge detection at all
#define SA1110_GPIO_GFER_DEFAULT_VALUE 0 // No edge detection at all

#define SA1110_GPIO_GPOSR_DEFAULT_VALUE 0xffffffff // Set all 1s
#define SA1110_GPIO_GPOCR_DEFAULT_VALUE 0x00000000 // and leave them set

// ------------------------------------------------------------------------
//
// Interrupt numbers
//
#define SA1110_GPIO_INTR_ETH0 0 // CYGNUM_HAL_INTERRUPT_GPIO0
#define SA1110_GPIO_INTR_ETH1 1 // CYGNUM_HAL_INTERRUPT_GPIO1

// ------------------------------------------------------------------------
//
// PCI stuff

#ifndef __ASSEMBLER__
#ifdef CYGPKG_IO_PCI

#define CYGHWR_HAL_ARM_NANO_PCI_MEM_MAP_BASE ((cyg_uint32)(&CYGMEM_SECTION_pci_window[0]))
#define CYGHWR_HAL_ARM_NANO_PCI_MEM_MAP_SIZE ((cyg_uint32)(CYGMEM_SECTION_pci_window_SIZE))

extern cyg_uint32 cyg_pci_window_real_base;

#define HAL_PCI_INIT() CYG_MACRO_START                                          \
    cyg_uint32 t;                                                               \
    /* Set up the DRAM system so that an alternate master can take control. */  \
    /* This is described in section 10.8 pp10-67..68 of the SA1110 book.    */  \
    /* This is how the nanoBridge allows the 82559 ethernet devices to      */  \
    /* access main memory.  Apparently.  So much for documentation.         */  \
                                                                                \
    /* Set GPIO pin direction: 21 out, 22 in.                               */  \
    t = *SA11X0_GPIO_PIN_DIRECTION;                                             \
    t |= SA11X0_GPIO_PIN_21;                                                    \
    t &=~SA11X0_GPIO_PIN_22;                                                    \
    *SA11X0_GPIO_PIN_DIRECTION = t;                                             \
                                                                                \
    /* Set alternate functions for GPIO 21 and 22.                          */  \
    t = *SA11X0_GPIO_ALTERNATE_FUNCTION;                                        \
    t |= SA11X0_GPIO_PIN_21 + SA11X0_GPIO_PIN_22;                               \
    *SA11X0_GPIO_ALTERNATE_FUNCTION = t;                                        \
                                                                                \
    /* Set the Test Unit Control Register to enable external memory mastery */  \
    t = *SA11X0_TUCR;                                                           \
    t &=~SA11X0_TUCR_RESERVED_BITS;                                             \
    t |= SA11X0_TUCR_EXTERNAL_MEMORY_MASTER;                                    \
    *SA11X0_TUCR = t;                                                           \
                                                                                \
    /* Set the interrupts on GPIO0 and GPIO1 to be falling-edge */              \
                                                                                \
    /* GPIO0 and GPIO1 be inputs: */                                            \
    t = *SA11X0_GPIO_PIN_DIRECTION;                                             \
    t &=~(SA11X0_GPIO_PIN_0 + SA11X0_GPIO_PIN_1);                               \
    *SA11X0_GPIO_PIN_DIRECTION = t;                                             \
    /* no rising edge */                                                        \
    t = *SA11X0_GPIO_RISING_EDGE_DETECT;                                        \
    t &=~(SA11X0_GPIO_PIN_0 + SA11X0_GPIO_PIN_1);                               \
    *SA11X0_GPIO_RISING_EDGE_DETECT = t;                                        \
    /* falling edge on */                                                       \
    t = *SA11X0_GPIO_FALLING_EDGE_DETECT;                                       \
    t |= (SA11X0_GPIO_PIN_0 + SA11X0_GPIO_PIN_1);                               \
    *SA11X0_GPIO_FALLING_EDGE_DETECT = t;                                       \
    /* cancel any pending edges */                                              \
    t = (SA11X0_GPIO_PIN_0 + SA11X0_GPIO_PIN_1);                                \
    *SA11X0_GPIO_EDGE_DETECT_STATUS = t;                                        \
                                                                                \
    /* and disconnect ethernet devices from the PCI bus so that they */         \
    /* respond to being scanned a second time - because RedBoot will */         \
    /* likely have enabled one or both of them already.              */         \
    /* We know that the two ethers are device #1 and #2:             */         \
    t = 0;                                                                      \
    HAL_PCI_CFG_WRITE_UINT16( 0, CYG_PCI_DEV_MAKE_DEVFN(1,0),                   \
                              CYG_PCI_CFG_COMMAND,            t );              \
    t = 0;                                                                      \
    HAL_PCI_CFG_WRITE_UINT16( 0, CYG_PCI_DEV_MAKE_DEVFN(2,0),                   \
                              CYG_PCI_CFG_COMMAND,            t );              \
CYG_MACRO_END

// This is nasty in the nanoBridge; it does NOT correctly return -1's for
// empty config slots, it lies and gives the impression of there being lots
// of extra nothing devices.  So we have to fake it.

// Compute address necessary to access PCI config space for the given bus
// and device.  With faked gaps...
#define HAL_PCI_CONFIG_ADDRESS( __bus, __devfn, __offset )                      \
    ({                                                                          \
    cyg_uint32 __addr;                                                          \
    cyg_uint32 __dev = CYG_PCI_DEV_GET_DEV(__devfn);                            \
    __addr = (0 == __bus) ? 0x18A00000 : 0xffffffffu;                           \
    __addr |= (1 == __dev || 2 == __dev) ? (__dev << 16) :  0xffffffffu;        \
    __addr |= CYG_PCI_DEV_GET_FN(__devfn) << 8;                                 \
    __addr |= __offset;                                                         \
    __addr;                                                                     \
    })

// Not used.  For experiments, or for a more general PCI bus...
#define scan_all_HAL_PCI_CONFIG_ADDRESS( __bus, __devfn, __offset )             \
    ({                                                                          \
    cyg_uint32 __addr;                                                          \
    cyg_uint32 __dev = CYG_PCI_DEV_GET_DEV(__devfn);                            \
    __addr = (0 == __bus) ? 0x18A00000 : 0xffffffffu;                           \
    __addr |= __dev << 16;                                                      \
    __addr |= CYG_PCI_DEV_GET_FN(__devfn) << 8;                                 \
    __addr |= __offset;                                                         \
    __addr;                                                                     \
    })

#define HAL_PCI_DO_CONFIG_ACCESS( __bus, __devfn, __offset, __val, __action, __type )   \
{                                                                                       \
    cyg_uint32 __doaddr = HAL_PCI_CONFIG_ADDRESS( __bus, __devfn, __offset );           \
    if ( 0xffffffffu == __doaddr )                                                      \
        __val = (__type)0xffffffffu;                                                    \
    else                                                                                \
        __action( __doaddr, __val );                                                    \
}

// Read/write a value from the PCI configuration space of the appropriate
// size at an address composed from the bus, devfn and offset.
#define HAL_PCI_CFG_READ_UINT8( __bus, __devfn, __offset, __val )   \
     HAL_PCI_DO_CONFIG_ACCESS( __bus, __devfn, __offset, __val, HAL_READ_UINT8  , cyg_uint8  )
#define HAL_PCI_CFG_READ_UINT16( __bus, __devfn, __offset, __val )  \
     HAL_PCI_DO_CONFIG_ACCESS( __bus, __devfn, __offset, __val, HAL_READ_UINT16 , cyg_uint16 )
#define HAL_PCI_CFG_READ_UINT32( __bus, __devfn, __offset, __val )  \
     HAL_PCI_DO_CONFIG_ACCESS( __bus, __devfn, __offset, __val, HAL_READ_UINT32 , cyg_uint32 )
#define HAL_PCI_CFG_WRITE_UINT8( __bus, __devfn, __offset, __val )  \
     HAL_PCI_DO_CONFIG_ACCESS( __bus, __devfn, __offset, __val, HAL_WRITE_UINT8 , cyg_uint8  )
#define HAL_PCI_CFG_WRITE_UINT16( __bus, __devfn, __offset, __val ) \
     HAL_PCI_DO_CONFIG_ACCESS( __bus, __devfn, __offset, __val, HAL_WRITE_UINT16, cyg_uint16 )
#define HAL_PCI_CFG_WRITE_UINT32( __bus, __devfn, __offset, __val ) \
     HAL_PCI_DO_CONFIG_ACCESS( __bus, __devfn, __offset, __val, HAL_WRITE_UINT32, cyg_uint32 )

//-----------------------------------------------------------------------------
// Resources

// Map PCI device resources starting from these addresses in PCI space.
#define HAL_PCI_ALLOC_BASE_MEMORY       (0x00000000)
#define HAL_PCI_ALLOC_BASE_IO           (0x00000000)

// This is where the PCI spaces are mapped in the CPU's address space.
#define HAL_PCI_PHYSICAL_MEMORY_BASE    (0x18620000)
#define HAL_PCI_PHYSICAL_IO_BASE        (0x18200000)

// Translate the PCI interrupt requested by the device (INTA#, INTB#,
// INTC# or INTD#) to the associated CPU interrupt (i.e., HAL vector).
#define HAL_PCI_TRANSLATE_INTERRUPT( __bus, __devfn, __vec, __valid) {  \
    cyg_uint32 __dev = CYG_PCI_DEV_GET_DEV(__devfn);                    \
    __valid = false;                                                    \
    if ( 1 == __dev || 2 == __dev ) {                                   \
        __vec = ( 1 == __dev ) ? SA1110_GPIO_INTR_ETH0                  \
                               : SA1110_GPIO_INTR_ETH1;                 \
        __valid = true;                                                 \
    }                                                                   \
}


#endif // CYGPKG_IO_PCI
#endif // #ifndef __ASSEMBLER__
// ------------------------------------------------------------------------

#endif // CYGONCE_HAL_ARM_SA11X0_NANOENGINE_NANOENGINE_H
// EOF nano.h
