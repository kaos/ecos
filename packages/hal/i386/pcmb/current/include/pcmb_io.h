#ifndef CYGONCE_PCMB_IO_H
#define CYGONCE_PCMB_IO_H

//=============================================================================
//
//      pcmb_io.h
//
//      PC Motherboard specific IO support
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    proven
// Contributors: proven, jskov, pjo
// Date:         1999-10-15
// Purpose:      PC Motherboard IO support
// Description:  The macros defined here provide the HAL APIs for handling
//               basic IO - specifically PCI config access.
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include CYGBLD_HAL_PLATFORM_H

#include <cyg/hal/pcmb_io.h>

//----------------------------------------------------------------------------
// The PCI resources required by the STPC 
// PCI Config registers in IO space

#define STPC_PCI_ADDR_REG  (0xCF8)
#define STPC_PCI_DATA_REG  (0xCFC)

//----------------------------------------------------------------------------

// Initialize the PCI bus.
#define HAL_PCI_INIT() \
CYG_MACRO_START    \
CYG_MACRO_END

//----------------------------------------------------------------------------
// Compute address necessary to access PCI config space for the given
// bus and device.

#define HAL_PCI_CONFIG_ADDRESS( __bus, __devfn, __offset )               \
	((1<<31) | ((__bus) << 16) | ((__devfn) << 8) | ((__offset) & ~3))               

//----------------------------------------------------------------------------
// Read a value from the PCI configuration space of the appropriate
// size at an address composed from the bus, devfn and offset.

#define HAL_PCI_CFG_READ_UINT32( __bus, __devfn, __offset, __val )                              \
CYG_MACRO_START                                                                                 \
    HAL_WRITE_UINT32(STPC_PCI_ADDR_REG,HAL_PCI_CONFIG_ADDRESS((__bus),(__devfn),(__offset)));   \
    HAL_READ_UINT32(STPC_PCI_DATA_REG,(__val));                                                 \
CYG_MACRO_END

#define HAL_PCI_CFG_READ_UINT8( __bus, __devfn, __offset, __val )                               \
CYG_MACRO_START                                                                                 \
    HAL_WRITE_UINT32(STPC_PCI_ADDR_REG,HAL_PCI_CONFIG_ADDRESS((__bus),(__devfn),(__offset)));   \
    HAL_READ_UINT8(STPC_PCI_DATA_REG + ((__offset)&3),(__val));                                 \
CYG_MACRO_END
    
#define HAL_PCI_CFG_READ_UINT16( __bus, __devfn, __offset, __val )                              \
CYG_MACRO_START                                                                                 \
    HAL_WRITE_UINT32(STPC_PCI_ADDR_REG,HAL_PCI_CONFIG_ADDRESS((__bus),(__devfn),(__offset)));   \
    HAL_READ_UINT16(STPC_PCI_DATA_REG + ((__offset)&2),(__val));                                \
CYG_MACRO_END
    
//----------------------------------------------------------------------------
// Write a value to the PCI configuration space of the appropriate
// size at an address composed from the bus, devfn and offset.

#define HAL_PCI_CFG_WRITE_UINT32( __bus, __devfn, __offset, __val )                             \
CYG_MACRO_START                                                                                 \
    HAL_WRITE_UINT32(STPC_PCI_ADDR_REG,HAL_PCI_CONFIG_ADDRESS((__bus),(__devfn),(__offset)));   \
    HAL_WRITE_UINT32(STPC_PCI_DATA_REG,(__val));                                                \
CYG_MACRO_END

#define HAL_PCI_CFG_WRITE_UINT8( __bus, __devfn, __offset, __val )                              \
CYG_MACRO_START                                                                                 \
    HAL_WRITE_UINT32(STPC_PCI_ADDR_REG,HAL_PCI_CONFIG_ADDRESS((__bus),(__devfn),(__offset)));   \
    HAL_WRITE_UINT8(STPC_PCI_DATA_REG+(__offset & 3),(__val));                                  \
CYG_MACRO_END

#define HAL_PCI_CFG_WRITE_UINT16( __bus, __devfn, __offset, __val )                             \
CYG_MACRO_START                                                                                 \
    HAL_WRITE_UINT32(STPC_PCI_ADDR_REG,HAL_PCI_CONFIG_ADDRESS((__bus),(__devfn),(__offset)));   \
    HAL_WRITE_UINT16(STPC_PCI_DATA_REG+(__offset & 2),(__val));                                 \
CYG_MACRO_END
 
    
//-----------------------------------------------------------------------------
// Resources

// This is where the PCI spaces are mapped in the CPU's address space.
// In the PC the PCI address space is mapped 1-1 into the CPU physical
// address space, so these values are both zero.

#define HAL_PCI_PHYSICAL_MEMORY_BASE    0x00000000
#define HAL_PCI_PHYSICAL_IO_BASE        0x0000

// Map PCI device resources starting from these addresses in PCI space.
// These are the addresses in PCI space where we should map device
// memory.  Since there is RAM and other devices in low memory and IO
// space, we allocate from high addresses. In most PC platforms, the
// BIOS will have actually allocated the PCI devices before we start,
// so these values are actually academic.

#define HAL_PCI_ALLOC_BASE_MEMORY  0xf0000000
#define HAL_PCI_ALLOC_BASE_IO      0xDF00

//----------------------------------------------------------------------------
// Translate the PCI interrupt requested by the device (INTA#, INTB#,
// INTC# or INTD#) to the associated CPU interrupt (i.e., HAL vector).

#define HAL_PCI_TRANSLATE_INTERRUPT( __bus, __devfn, __vec, __valid)            \
CYG_MACRO_START                                                                 \
	HAL_PCI_CFG_READ_UINT8((__bus),(__devfn),CYG_PCI_CFG_INT_LINE,(__vec)); \
	if(__vec<=15) __valid=1;                                                \
	else __valid=0;                                                         \
	__vec += 0x20;                                                          \
CYG_MACRO_END

//-----------------------------------------------------------------------------
// CMOS RAM access

#define HAL_CMOS_ADDRESS        0x70    // CMOS address register
#define HAL_CMOS_DATA           0x71    // CMOS data register

#define HAL_READ_CMOS( __addr, __val )                  \
CYG_MACRO_START                                         \
{                                                       \
    HAL_WRITE_UINT8( HAL_CMOS_ADDRESS, __addr );        \
    HAL_READ_UINT8( HAL_CMOS_DATA, (__val) );           \
}                                                       \
CYG_MACRO_END

#define HAL_WRITE_CMOS( __addr, __val )                 \
CYG_MACRO_START                                         \
{                                                       \
    HAL_WRITE_UINT8( HAL_CMOS_ADDRESS, __addr );        \
    HAL_WRITE_UINT8( HAL_CMOS_DATA, (__val) );          \
}                                                       \
CYG_MACRO_END

//-----------------------------------------------------------------------------
// Debug macros
// Some simple macros for writing useful info to a PC ASCII display

#define PC_WRITE_SCREEN( __pos, __ch ) (*((short *)(0xB8000)+(__pos)) = (0x0700+(__ch)))

#define PC_SCREEN_LINE( __line ) ((__line)*80)

#define PC_WRITE_SCREEN_8( __pos, __val )                       \
{                                                               \
    char __hex[] = "0123456789ABCDEF";                          \
    PC_WRITE_SCREEN( (__pos), __hex[((int)(__val)>>4)&0xF] );        \
    PC_WRITE_SCREEN( ((__pos)+1), __hex[(int)(__val)&0xF] );         \
}

#define PC_WRITE_SCREEN_16( __pos, __val )      \
    PC_WRITE_SCREEN_8( __pos, (int)(__val)>>8 );     \
    PC_WRITE_SCREEN_8( (__pos)+2, (int)(__val) );

#define PC_WRITE_SCREEN_32( __pos, __val )      \
    PC_WRITE_SCREEN_16( __pos, (int)(__val)>>16 );   \
    PC_WRITE_SCREEN_16( (__pos)+4, (int)(__val) );

//-----------------------------------------------------------------------------
// IDE interface macros
//
#define HAL_IDE_NUM_CONTROLLERS 2

// Initialize the IDE controller(s).
#define HAL_IDE_INIT()

#define __PCMB_IDE_PRI_CMD   0x1f0
#define __PCMB_IDE_PRI_CTL   0x3f4
#define __PCMB_IDE_SEC_CMD   0x170
#define __PCMB_IDE_SEC_CTL   0x374

#define __CMD_ADDR(__n) ((__n) ? __PCMB_IDE_SEC_CMD : __PCMB_IDE_PRI_CMD)
#define __CTL_ADDR(__n) ((__n) ? __PCMB_IDE_SEC_CTL : __PCMB_IDE_PRI_CTL)

#define HAL_IDE_READ_UINT8( __ctlr, __regno, __val )  \
    HAL_READ_UINT8(__CMD_ADDR(__ctlr) + (__regno), (__val))
#define HAL_IDE_READ_UINT16( __ctlr, __regno, __val )  \
    HAL_READ_UINT16(__CMD_ADDR(__ctlr) + (__regno), (__val))
#define HAL_IDE_READ_ALTSTATUS( __ctlr, __val )  \
    HAL_READ_UINT16(__CTL_ADDR(__ctlr) + 2, (__val))

#define HAL_IDE_WRITE_UINT8( __ctlr, __regno, __val )  \
    HAL_WRITE_UINT8(__CMD_ADDR(__ctlr) + (__regno), (__val))
#define HAL_IDE_WRITE_UINT16( __ctlr, __regno, __val )  \
    HAL_WRITE_UINT16(__CMD_ADDR(__ctlr) + (__regno), (__val))
#define HAL_IDE_WRITE_CONTROL( __ctlr, __val )  \
    HAL_WRITE_UINT16(__CTL_ADDR(__ctlr) + 2, (__val))


//-----------------------------------------------------------------------------
// end of pcmb_io.h
#endif // CYGONCE_PCMB_IO_H
