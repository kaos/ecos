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
// Author(s):    Philippe Robin
// Contributors: David A Rusling
// Date:         November 7, 2000
// Purpose:      Integrator PCI IO support macros
// Description: 
// Usage:        #include <cyg/hal/plf_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal_arm_integrator.h>

#include <cyg/hal/hal_integrator.h>

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_platform_ints.h>  // Interrupt vectors

// Initialization of the PCI bus.
#define HAL_PCI_INIT()                                                  	\
    CYG_MACRO_START                                                     	\
    volatile int i, j;								\
										\
    /* setting this register will take the V3 out of reset */			\
										\
    *(cyg_uint32 *)(INTEGRATOR_SC_PCIENABLE) = 1;				\
										\
    /* wait a few usecs to settle the device and the PCI bus */			\
										\
    for (i = 0; i < 100 ; i++)							\
	   j = i + 1;								\
										\
    /* Now write the Base I/O Address Word to V3_BASE + 0x6C */			\
										\
    *(cyg_uint16 *)(V3_BASE + V3_LB_IO_BASE) = (cyg_uint16)(V3_BASE >> 16);	\
										\
    do {									\
        *(cyg_uint8 *)(V3_BASE + V3_MAIL_DATA) = 0xAA;				\
	*(cyg_uint8 *)(V3_BASE + V3_MAIL_DATA + 4) = 0x55;			\
    } while (*(cyg_uint8 *)(V3_BASE + V3_MAIL_DATA) != 0xAA ||			\
	     *(cyg_uint8 *)(V3_BASE + V3_MAIL_DATA + 4) != 0x55);		\
										\
    /* Make sure that V3 register access is not locked, if it is, unlock it */	\
										\
    if ((*(cyg_uint16 *)(V3_BASE + V3_SYSTEM) & V3_SYSTEM_M_LOCK)  		\
				== V3_SYSTEM_M_LOCK)				\
	*(cyg_uint16 *)(V3_BASE + V3_SYSTEM) = 0xA05F;				\
										\
    /* Ensure that the slave accesses from PCI are disabled while we */		\
    /* setup windows */								\
										\
    *(cyg_uint16 *)(V3_BASE + V3_PCI_CMD) &= 					\
				~(V3_COMMAND_M_MEM_EN | V3_COMMAND_M_IO_EN);	\
										\
    /* Clear RST_OUT to 0; keep the PCI bus in reset until we've finished */	\
										\
    *(cyg_uint16 *)(V3_BASE + V3_SYSTEM) &= ~V3_SYSTEM_M_RST_OUT;		\
										\
    /* Make all accesses from PCI space retry until we're ready for them */	\
										\
    *(cyg_uint16 *)(V3_BASE + V3_PCI_CFG) |= V3_PCI_CFG_M_RETRY_EN;		\
										\
    /* Set up any V3 PCI Configuration Registers that we absolutely have to */	\
    /* LB_CFG controls Local Bus protocol. */					\
    /* Enable LocalBus byte strobes for READ accesses too. */			\
    /* set bit 7 BE_IMODE and bit 6 BE_OMODE */					\
										\
    *(cyg_uint16 *)(V3_BASE + V3_LB_CFG) |= 0x0C0;				\
										\
    /* PCI_CMD controls overall PCI operation. */				\
    /* Enable PCI bus master. */						\
										\
    *(cyg_uint16 *)(V3_BASE + V3_PCI_CMD) |= 0x04;				\
										\
    /* PCI_MAP0 controls where the PCI to CPU memory window is on Local Bus*/	\
										\
    *(cyg_uint32 *)(V3_BASE + V3_PCI_MAP0) = (INTEGRATOR_BOOT_ROM_BASE) |	\
					(V3_PCI_MAP_M_ADR_SIZE_512M |		\
					V3_PCI_MAP_M_REG_EN |			\
					V3_PCI_MAP_M_ENABLE);			\
										\
    /* PCI_BASE0 is the PCI address of the start of the window */		\
										\
    *(cyg_uint32 *)(V3_BASE + V3_PCI_BASE0) = INTEGRATOR_BOOT_ROM_BASE;		\
										\
    /* PCI_MAP1 is LOCAL address of the start of the window */			\
										\
    *(cyg_uint32 *)(V3_BASE + V3_PCI_MAP1) = (INTEGRATOR_HDR0_SDRAM_BASE) |	\
			(V3_PCI_MAP_M_ADR_SIZE_1024M | V3_PCI_MAP_M_REG_EN |	\
			 V3_PCI_MAP_M_ENABLE);					\
										\
    /* PCI_BASE1 is the PCI address of the start of the window */		\
										\
    *(cyg_uint32 *)(V3_BASE + V3_PCI_BASE1) = INTEGRATOR_HDR0_SDRAM_BASE;	\
										\
    /* Set up the windows from local bus memory into PCI configuration, */	\
    /* I/O and Memory. */							\
    /* PCI I/O, LB_BASE2 and LB_MAP2 are used exclusively for this. */		\
										\
    *(cyg_uint16 *)(V3_BASE +V3_LB_BASE2) = 					\
			((CPU_PCI_IO_ADRS >> 24) << 8) | V3_LB_BASE_M_ENABLE;	\
    *(cyg_uint16 *)(V3_BASE + V3_LB_MAP2) = 0;					\
										\
    /* PCI Configuration, use LB_BASE1/LB_MAP1. */				\
										\
    /* PCI Memory use LB_BASE0/LB_MAP0 and LB_BASE1/LB_MAP1 */			\
    /* Map first 256Mbytes as non-prefetchable via BASE0/MAP0 */		\
    /* (INTEGRATOR_PCI_BASE == PCI_MEM_BASE) */					\
										\
    *(cyg_uint32 *)(V3_BASE + V3_LB_BASE0) = 					\
			INTEGRATOR_PCI_BASE | (0x80 | V3_LB_BASE_M_ENABLE);	\
										\
    *(cyg_uint16 *)(V3_BASE + V3_LB_MAP0) = 					\
			((INTEGRATOR_PCI_BASE >> 20) << 0x4) | 0x0006;		\
										\
    /* Map second 256 Mbytes as prefetchable via BASE1/MAP1 */			\
										\
    *(cyg_uint32 *)(V3_BASE + V3_LB_BASE1) = 					\
			INTEGRATOR_PCI_BASE | (0x84 | V3_LB_BASE_M_ENABLE);	\
										\
    *(cyg_uint16 *)(V3_BASE + V3_LB_MAP1) = 					\
			(((INTEGRATOR_PCI_BASE + SZ_256M) >> 20) << 4) | 0x0006;\
										\
    /* Allow accesses to PCI Configuration space */				\
    /* and set up A1, A0 for type 1 config cycles */				\
										\
    *(cyg_uint16 *)(V3_BASE + V3_PCI_CFG) = 					\
			((*(cyg_uint16 *)(V3_BASE + V3_PCI_CFG)) &		\
			   ~(V3_PCI_CFG_M_RETRY_EN | V3_PCI_CFG_M_AD_LOW1) ) |	\
			   V3_PCI_CFG_M_AD_LOW0;				\
										\
    /* now we can allow in PCI MEMORY accesses */				\
										\
    *(cyg_uint16 *)(V3_BASE + V3_PCI_CMD) = 					\
		(*(cyg_uint16 *)(V3_BASE + V3_PCI_CMD)) | V3_COMMAND_M_MEM_EN;	\
										\
    /* Set RST_OUT to take the PCI bus is out of reset, PCI devices can */	\
    /* initialise and lock the V3 system register so that no one else */	\
    /* can play with it */							\
										\
   *(cyg_uint16 *)(V3_BASE + V3_SYSTEM) = 					\
		(*(cyg_uint16 *)(V3_BASE + V3_SYSTEM)) | V3_SYSTEM_M_RST_OUT;	\
										\
   *(cyg_uint16 *)(V3_BASE + V3_SYSTEM) = 					\
		(*(cyg_uint16 *)(V3_BASE + V3_SYSTEM)) | V3_SYSTEM_M_LOCK;	\
                                                                         	\
    CYG_MACRO_END


// V3 access routines
#define _V3Write16(o,v) (*(volatile cyg_uint16 *)(PCI_V3_BASE + (cyg_uint32)(o)) \
					 = (cyg_uint16)(v))
#define _V3Read16(o)    (*(volatile cyg_uint16 *)(PCI_V3_BASE + (cyg_uint32)(o)))

#define _V3Write32(o,v) (*(volatile cyg_uint32 *)(PCI_V3_BASE + (cyg_uint32)(o)) \
					= (cyg_uint32)(v))
#define _V3Read32(o)    (*(volatile cyg_uint32 *)(PCI_V3_BASE + (cyg_uint32)(o)))

// _V3OpenConfigWindow - open V3 configuration window
#define _V3OpenConfigWindow() 							\
    {										\
    /* Set up base0 to see all 512Mbytes of memory space (not	     */		\
    /* prefetchable), this frees up base1 for re-use by configuration*/		\
    /* memory */								\
										\
    _V3Write32 (V3_LB_BASE0, ((INTEGRATOR_PCI_BASE & 0xFFF00000) |		\
			     0x90 | V3_LB_BASE_M_ENABLE));			\
    /* Set up base1 to point into configuration space, note that MAP1 */	\
    /* register is set up by pciMakeConfigAddress(). */				\
										\
    _V3Write32 (V3_LB_BASE1, ((CPU_PCI_CNFG_ADRS & 0xFFF00000) |		\
			     0x40 | V3_LB_BASE_M_ENABLE));			\
    }

// _V3CloseConfigWindow - close V3 configuration window
#define _V3CloseConfigWindow()							\
    {										\
    /* Reassign base1 for use by prefetchable PCI memory */			\
    _V3Write32 (V3_LB_BASE1, (((INTEGRATOR_PCI_BASE + SZ_256M) & 0xFFF00000)	\
					| 0x84 | V3_LB_BASE_M_ENABLE));		\
    _V3Write16 (V3_LB_MAP1,							\
	    (((INTEGRATOR_PCI_BASE + SZ_256M) & 0xFFF00000) >> 16) | 0x0006);	\
										\
    /* And shrink base0 back to a 256M window (NOTE: MAP0 already correct) */	\
										\
    _V3Write32 (V3_LB_BASE0, ((INTEGRATOR_PCI_BASE & 0xFFF00000) |		\
			     0x80 | V3_LB_BASE_M_ENABLE));			\
    }

// Compute address necessary to access PCI config space for the given
// bus and device.
#define HAL_PCI_CONFIG_ADDRESS( __bus, __devfn, __offset ) \
    ({                                                                   		\
    cyg_uint32 __address, __devicebit;							\
    cyg_uint16 __mapaddress;								\
    cyg_uint32 __dev = CYG_PCI_DEV_GET_DEV(__devfn);	/* FIXME to check!! (slot?) */	\
											\
    if (__bus == 0) {									\
	/* local bus segment so need a type 0 config cycle */				\
        /* build the PCI configuration "address" with one-hot in A31-A11 */		\
        __address = PCI_CONFIG_BASE;							\
        __address |= ((__devfn & 0x07) << 8);						\
        __address |= __offset & 0xFF;							\
        __mapaddress = 0x000A;    /* 101=>config cycle, 0=>A1=A0=0 */			\
        __devicebit = (1 << (__dev + 11));						\
											\
        if ((__devicebit & 0xFF000000) != 0) {						\
            /* high order bits are handled by the MAP register */			\
            __mapaddress |= (__devicebit >> 16);					\
        } else {									\
            /* low order bits handled directly in the address */			\
            __address |= __devicebit;							\
        }										\
    } else {	/* bus !=0 */								\
        /* not the local bus segment so need a type 1 config cycle */			\
        /* A31-A24 are don't care (so clear to 0) */					\
        __mapaddress = 0x000B;    /* 101=>config cycle, 1=>A1&A0 from PCI_CFG */	\
        __address = PCI_CONFIG_BASE;							\
        __address |= ((__bus & 0xFF) << 16);  	/* bits 23..16 = bus number 	*/	\
        __address |= ((__dev & 0x1F) << 11);  	/* bits 15..11 = device number  */	\
        __address |= ((__devfn & 0x07) << 8);  	/* bits 10..8  = function number*/	\
        __address |= __offset & 0xFF;  		/* bits  7..0  = register number*/	\
    }											\
    _V3Write16(V3_LB_MAP1, __mapaddress);						\
											\
    __address;										\
    })

// Read a value from the PCI configuration space of the appropriate
// size at an address composed from the bus, devfn and offset.
#define HAL_PCI_CFG_READ_UINT8( __bus, __devfn, __offset, __val )  		\
    {										\
    _V3OpenConfigWindow();							\
    __val = *(cyg_uint8 *)HAL_PCI_CONFIG_ADDRESS(__bus, __devfn, __offset); 	\
    _V3CloseConfigWindow();							\
    }

#define HAL_PCI_CFG_READ_UINT16( __bus, __devfn, __offset, __val )  		\
    {										\
    _V3OpenConfigWindow();							\
    __val = *(cyg_uint16 *)HAL_PCI_CONFIG_ADDRESS(__bus, __devfn, __offset); 	\
    _V3CloseConfigWindow();							\
    }

#define HAL_PCI_CFG_READ_UINT32( __bus, __devfn, __offset, __val )  		\
    {										\
    _V3OpenConfigWindow();							\
    __val = *(cyg_uint32 *)HAL_PCI_CONFIG_ADDRESS(__bus, __devfn, __offset); 	\
    _V3CloseConfigWindow();							\
    }

// Write a value to the PCI configuration space of the appropriate
// size at an address composed from the bus, devfn and offset.
#define HAL_PCI_CFG_WRITE_UINT8( __bus, __devfn, __offset, __val )		\
    {										\
    _V3OpenConfigWindow();							\
    *(cyg_uint8 *)HAL_PCI_CONFIG_ADDRESS(__bus, __devfn, __offset) = __val; 	\
    _V3CloseConfigWindow();							\
    }

#define HAL_PCI_CFG_WRITE_UINT16( __bus, __devfn, __offset, __val )		\
    {										\
    _V3OpenConfigWindow();							\
    *(cyg_uint16 *)HAL_PCI_CONFIG_ADDRESS(__bus, __devfn, __offset) = __val; 	\
    _V3CloseConfigWindow();							\
    }

#define HAL_PCI_CFG_WRITE_UINT32( __bus, __devfn, __offset, __val )		\
    {										\
    _V3OpenConfigWindow();							\
    *(cyg_uint32 *)HAL_PCI_CONFIG_ADDRESS(__bus, __devfn, __offset) = __val; 	\
    _V3CloseConfigWindow();							\
    }

//-----------------------------------------------------------------------------
// Resources

// Map PCI device resources starting from these addresses in PCI space.
#define HAL_PCI_ALLOC_BASE_MEMORY 0
#define HAL_PCI_ALLOC_BASE_IO     0x4000

// This is where the PCI spaces are mapped in the CPU's address space.
#define HAL_PCI_PHYSICAL_MEMORY_BASE    (PCI_MEM_BASE)
#define HAL_PCI_PHYSICAL_IO_BASE        (PCI_IO_BASE)

// Translate the PCI interrupt requested by the device (INTA#, INTB#,
// INTC# or INTD#) to the associated CPU interrupt (i.e., HAL vector).
#define INTA CYGNUM_HAL_INTERRUPT_PCIINT0
#define INTB CYGNUM_HAL_INTERRUPT_PCIINT1
#define INTC CYGNUM_HAL_INTERRUPT_PCIINT2
#define INTD CYGNUM_HAL_INTERRUPT_PCIINT3

#define HAL_PCI_TRANSLATE_INTERRUPT( __bus, __devfn, __vec, __valid)          \
    CYG_MACRO_START                                                           \
    cyg_uint8 __req;                                                          \
    cyg_uint32 __dev;							      \
    /* DANGER! For now this is the SDM interrupt table... */	      	      \
    static const cyg_uint8 irq_tab[12][4] = {	      			      \
		/* INTA  INTB  INTC  INTD */				      \
		{INTA, INTB, INTC, INTD},  /* idsel 20, slot  9 */	      \
		{INTB, INTC, INTD, INTA},  /* idsel 21, slot 10 */	      \
		{INTC, INTD, INTA, INTB},  /* idsel 22, slot 11 */	      \
		{INTD, INTA, INTB, INTC},  /* idsel 23, slot 12 */	      \
		{INTA, INTB, INTC, INTD},  /* idsel 24, slot 13 */	      \
		{INTB, INTC, INTD, INTA},  /* idsel 25, slot 14 */	      \
		{INTC, INTD, INTA, INTB},  /* idsel 26, slot 15 */	      \
		{INTD, INTA, INTB, INTC},  /* idsel 27, slot 16 */	      \
		{INTA, INTB, INTC, INTD},  /* idsel 28, slot 17 */	      \
		{INTB, INTC, INTD, INTA},  /* idsel 29, slot 18 */	      \
		{INTC, INTD, INTA, INTB},  /* idsel 30, slot 19 */	      \
		{INTD, INTA, INTB, INTC}   /* idsel 31, slot 20 */	      \
    };								      	      \
    HAL_PCI_CFG_READ_UINT8(__bus, __devfn, CYG_PCI_CFG_INT_PIN, __req);       \
    __dev  = CYG_PCI_DEV_GET_DEV(__devfn);	/* FIXME to check!! (slot?)*/ \
									      \
    /* if PIN = 0, default to A */					      \
    if (__req == 0)							      \
	__req = 1;							      \
									      \
    __vec = irq_tab[__dev - 9][__req - 1];				      \
    __valid = true;                                                           \
    CYG_MACRO_END

//-----------------------------------------------------------------------------
// end of plf_io.h
#endif // CYGONCE_PLF_IO_H
