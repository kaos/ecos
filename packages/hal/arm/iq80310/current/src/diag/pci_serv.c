//=============================================================================
//
//      pci_serv.c - Cyclone Diagnostics
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
// Copyright (C) 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Scott Coulter, Jeff Frazier, Eric Breeden
// Contributors:
// Date:        2001-01-25
// Purpose:     
// Description: 
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

/********************************************************************************/
/* PCI_SERV.C - PCI driver for IQ80310									        */
/*																			    */			
/* History:                                                                     */
/*  15sep00 ejb Ported to Cygmon on IQ80310									    */
/*  18dec00 snc                                                                 */
/********************************************************************************/
#include "iq80310.h"
#include "pci_bios.h"



#undef  DEBUG_PCI

#define	IB_MA_ERROR	0x2000

/*==========================================================================*/
/* Globals                                                                  */
/*==========================================================================*/
ULONG	memspace_ptr[NUM_PCI_BUSES];
ULONG	iospace_ptr[NUM_PCI_BUSES];
ULONG	memspace_limit[NUM_PCI_BUSES];
ULONG	iospace_limit[NUM_PCI_BUSES];
UINT	nextbus;
UINT	secondary_busno = SECONDARY_BUS_NUM;
UINT	primary_busno = PRIMARY_BUS_NUM;
UINT    lastbus;
unsigned long dram_size;		/* global storing the size of DRAM */	
int bus0_lastbus;        /* last secondary bus number behind bus 0 */
int bus1_lastbus;        /* last secondary bus number behind bus 1 */

int nmi_verbose;	/* global flag to indicate whether or not PCI Error messages should be
					   printed.  This flag is used to prevent a painful deluge of messages
					   when performing PCI configuration reads/writes to possibly non-existant
					   devices. */

int pci_config_error = FALSE; /* becomes TRUE if an NMI interrupt occurs due to a PCI config cycle */

#define PRINT_ON()	nmi_verbose = TRUE
#define PRINT_OFF() nmi_verbose = FALSE

/*==========================================================================*/
/* Function prototypes                                                      */
/*==========================================================================*/
static void sys_pci_bus_init (UINT bus, UINT root_bus, PCI_DATA* pci_data);
void print_config_space (int bus, int device, int function);


void show_pci(void);
void show_bus(int);
void init_312_pci(void);

typedef int (*FUNCPTR) ();	   /* ptr to function returning int */

typedef struct
{
	FUNCPTR	handler;
	int		arg;
	int		bus;
	int		device;
} INT_HANDLER;

#define NUM_PCI_XINTS		4		/* XINT0 - XINT3 */
#define MAX_PCI_HANDLERS	8		/* maximum handlers per PCI Xint */

/* Table where the interrupt handler addresses are stored. */
INT_HANDLER pci_int_handlers[4][MAX_PCI_HANDLERS];

//extern void printf();
extern void hexIn();

extern int pci_config_cycle;

extern void _enableFiqIrq();	 
extern void config_ints(void);	/* configure interrupts */

/*********************************************************************************
* pci_to_xint - convert a PCI device number and Interrupt line to an 80312 XINT
*
* This function converts a PCI slot number (0 - 7) and an Interrupt line
* (INTA - INTD) to a i960 processor XINT number (0 - 3)
*
* RETURNS: OK or ERROR if arguments are invalid
*
*/
STATUS pci_to_xint(int device, int intpin, int *xint)
{
	int device_base;	/* all devices mod 4 follow same interrupt mapping scheme */

    /* check validity of arguments */
	if ((intpin < INTA) || (intpin > INTD) || (device > 31))
		return (ERROR);

	device_base = device % 4;

	/* interrupt mapping scheme as per PCI-to-PCI Bridge Specification */
	switch (device_base)
    {
	case 0:
	    switch (intpin)
		{
		case INTA:
			*xint = XINT0;
			break;
		case INTB:
			*xint = XINT1;
			break;
		case INTC:
			*xint = XINT2;
			break;
		case INTD:
			*xint = XINT3;
			break;
		}
		break;
	case 1:
	    switch (intpin)
		{
	   	case INTA:
			*xint = XINT1;
			break;
		case INTB:
			*xint = XINT2;
			break;
		case INTC:
			*xint = XINT3;
			break;
		case INTD:
			*xint = XINT0;
			break;
		}
	    break;
	case 2:
	    switch (intpin)
		{
		case INTA:
			*xint = XINT2;
			break;
		case INTB:
			*xint = XINT3;
			break;
		case INTC:
			*xint = XINT0;
			break;
		case INTD:
			*xint = XINT1;
			break;
		}
		break;
	case 3:
	    switch (intpin)
		{
		case INTA:
			*xint = XINT3;
			break;
		case INTB:
			*xint = XINT0;
			break;
		case INTC:
			*xint = XINT1;
			break;
		case INTD:
			*xint = XINT2;
			break;
		}
	    break;
    }
	return (OK);
}


/******************************************************************************
*
* Checks to see if the "bus" argument identifies a PCI bus which is located 
* off of the Primary PCI bus of the board.
*/
int off_ppci_bus (int busno)
{
	if (busno == primary_busno) 
		return (TRUE);
	else if (busno == secondary_busno) 
		return (FALSE);
	else if (busno <= bus0_lastbus)
		return (TRUE);
	else
		return (FALSE);
}

static unsigned old_abort_vec;

/*************************************************************************
* pci_cycle_cleanup - cleanup after a PCI configuration cycle
*
* This function will clear the various PCI abort bits if a configuration
* cycle to a non-existant device is attempted. Covers both ATU and
* bridge functions.
*
* RETURNS: OK if no PCI abort bits were set or ERROR if abort bits were 
*          detected.
*/
static int pci_cycle_cleanup (ULONG busno)
{
  UINT16 *pci_status_reg16;
  UINT16 pci_status16;
  UINT32 *pci_status_reg;
  UINT32 pci_status;
  UINT8 status;
  UINT8 bus_select = 0;		/* quiet the compiler warning */
    
  pci_status16 = 0;
  pci_status = 0;
  status = 0;

	/* this if-else structure must be done in the correct order to
       ensure that the correct ATU is chosen */
	if (busno == primary_busno) 
		bus_select = PRIMARY_BUS_NUM;
	else if (busno == secondary_busno) 
		bus_select = SECONDARY_BUS_NUM;
	else if (busno <= bus0_lastbus)
		bus_select = PRIMARY_BUS_NUM;
    else if (busno <= bus1_lastbus)
		bus_select = SECONDARY_BUS_NUM;
	else return (ERROR);


  /* Read/clear bus status and bus interrupt status registers */

  switch (bus_select) 	
  {
		case 0:  /* Primary Bus */
			pci_status_reg16 = (UINT16 *) PATUSR_ADDR;
			pci_status16 = *pci_status_reg16;
		
			if ((pci_status16 & 0xF900) == 0) goto skip1;

			#ifdef DEBUG_PCI
				if (pci_status16 &  PARITY_ERROR)	printf("Parity Error Detected - Primary Bus - ATU\n");
				if (pci_status16 &  SERR_ERROR)		printf("P_SERR# Asserted - Primary Bus - ATU\n");
				if (pci_status16 &  MASTER_ABORT)	printf("Master Abort Detected - Primary Bus - ATU\n");
				if (pci_status16 &  TARGET_ABORT_M) printf("Target Abort Detected - Primary Bus - ATU is master\n");
				if (pci_status16 &  TARGET_ABORT_T) printf("Target Abort Detected - Primary Bus - ATU is target\n");
				if (pci_status16 &  MASTER_PAR_ERR) printf("Master Parity Error - Primary Bus - ATU\n");
			#endif
			status = 1;
			pci_status16 &= 0xF980;
			*pci_status_reg16 = pci_status16;
		
skip1:
			pci_status_reg16 = (UINT16 *) PSR_ADDR;
			pci_status16 = *pci_status_reg16;
	
			if ((pci_status16 & 0xF900) == 0) goto skip2;

			#ifdef DEBUG_PCI
				if (pci_status16 &  PARITY_ERROR)	printf("Parity Error Detected - Primary Bus - Bridge\n");
				if (pci_status16 &  SERR_ERROR)		printf("P_SERR# Asserted - Primary Bus - Bridge\n");
				if (pci_status16 &  MASTER_ABORT)	printf("Master Abort Detected - Primary Bus - Bridge\n");
				if (pci_status16 &  TARGET_ABORT_M) printf("Target Abort Detected - Primary Bus - Bridge is master\n");
				if (pci_status16 &  TARGET_ABORT_T) printf("Target Abort Detected - Primary Bus - Bridge is target\n");
				if (pci_status16 &  MASTER_PAR_ERR) printf("Master Parity Error - Primary Bus - Bridge\n");
			#endif
			status = 1;
			pci_status16 &= 0xF980;
			*pci_status_reg16 = pci_status16;
		
skip2:

			pci_status_reg = (UINT32 *) PATUISR_ADDR;
			pci_status = *pci_status_reg;
	
			if ((pci_status & 0x0000079F) == 0) goto skip3;

			#ifdef DEBUG_PCI
				if (pci_status &  ATU_BIST_ERR) printf("ATU BIST Error - Primary Bus\n");
				if (pci_status &  IB_MA_ERROR)	printf("Internal Bus Master Abort - Primary Bus - ATU\n");
			#endif
			status = 1;
			pci_status &= 0x0000079f;
			*pci_status_reg = pci_status;
skip3:

			pci_status_reg = (UINT32 *) PBISR_ADDR;
			pci_status = *pci_status_reg;
			
			if ((pci_status & 0x0000003F) == 0) goto skip4;

			#ifdef DEBUG_PCI
				if (pci_status16 &  BRIDGE_PERR)	printf("Parity Error Detected - Primary Bus - Bridge\n");
				if (pci_status16 &  SERR_ERROR)		printf("P_SERR# Asserted - Primary Bus - Bridge\n");
				if (pci_status16 &  MASTER_ABORT)	printf("Master Abort Detected - Primary Bus - Bridge\n");
				if (pci_status16 &  TARGET_ABORT_M) printf("Target Abort Detected - Primary Bus - Bridge is master\n");
				if (pci_status16 &  TARGET_ABORT_T) printf("Target Abort Detected - Primary Bus - Bridge is target\n");
				if (pci_status16 &  MASTER_PAR_ERR) printf("Master Parity Error - Primary Bus - Bridge\n");		
			#endif
			status = 1;
			pci_status &= 0x0000003F;
			*pci_status_reg = pci_status;
skip4:
			break;


		case 1:   /* Secondary Bus */	
			pci_status_reg16 = (UINT16 *) SATUSR_ADDR;
			pci_status16 = *pci_status_reg16;

			if ((pci_status16 & 0xF900) == 0) goto skip5;
			#ifdef DEBUG_PCI
				if (pci_status16 &  PARITY_ERROR)	printf("Parity Error Detected - Secondary Bus - ATU\n");
				if (pci_status16 &  SERR_ERROR)		printf("S_SERR# Asserted - Secondary Bus - ATU\n");
				if (pci_status16 &  MASTER_ABORT)	printf("Master Abort Detected - Secondary Bus - ATU\n");
				if (pci_status16 &  TARGET_ABORT_M) printf("Target Abort Detected - Secondary Bus - ATU is master\n");
				if (pci_status16 &  TARGET_ABORT_T) printf("Target Abort Detected - Secondary Bus - ATU is target\n");
				if (pci_status16 &  MASTER_PAR_ERR) printf("Master Parity Error - Secondary Bus - ATU\n");
			#endif	
			status = 1;
			pci_status16 &= 0xF900;
			*pci_status_reg16 = pci_status16;
skip5:
			pci_status_reg16 = (UINT16 *) SSR_ADDR;
			pci_status16 = *pci_status_reg16;

			if ((pci_status16 & 0xF900) == 0) goto skip6;

			#ifdef DEBUG_PCI
				if (pci_status16 &  PARITY_ERROR)	printf("Parity Error Detected - Secondary Bus - Bridge\n");
				if (pci_status16 &  SERR_ERROR)		printf("S_SERR# Asserted - Secondary Bus - Bridge\n");
				if (pci_status16 &  MASTER_ABORT)	printf("Master Abort Detected - Secondary Bus - Bridge\n");
				if (pci_status16 &  TARGET_ABORT_M) printf("Target Abort Detected - Secondary Bus - Bridge is master\n");
				if (pci_status16 &  TARGET_ABORT_T) printf("Target Abort Detected - Secondary Bus - Bridge is target\n");
				if (pci_status16 &  MASTER_PAR_ERR) printf("Master Parity Error - Secondary Bus - Bridge\n");
			#endif	
			status = 1;
			pci_status16 &= 0xF980;
			*pci_status_reg16 = pci_status16;

skip6:
			pci_status_reg = (UINT32 *) SATUISR_ADDR;
			pci_status = *pci_status_reg;

			if ((pci_status & 0x0000069F) == 0) goto skip7;

			#ifdef DEBUG_PCI
				if (pci_status &  IB_MA_ERROR) printf("Internal Bus Master Abort - Secondary Bus - ATU\n");
			#endif
			status = 1;
			pci_status &= 0x0000069F;
			*pci_status_reg = pci_status;
skip7:
			break;

		default: return (ERROR);
  }

	if (pci_config_error)  /* check to see if the NMI handler during the config cycle */
		status = 1;  

	pci_config_cycle = 0; /* turn on exception handling after pci config cycle */

  if (old_abort_vec)
  {
      ((volatile unsigned *)0x20)[4] = old_abort_vec;
      old_abort_vec = 0;
      _flushICache();
  }

  if (status) return (ERROR);
  else return (OK);
}

extern void __ignore_abort(void);

/**************************************************************************
* sys_config_setup - this function sets up a PCI configuration cycle
*
* This function sets up either the primary or secondary outbound configuration
* cycle address register.  It is called by all the sys_*_config_* functions. 
*
*/
static int sys_config_setup(ULONG busno, ULONG devno, ULONG funcno, ULONG regno, UINT32 **pci_occ_addr, 
							UINT32 **pci_occ_data)
{
	
	/* First check the parameters for sanity */
	if ((busno > 255) || (devno > 31) || (funcno> 7) || (regno > 63)) return (ERROR);
	
	if (busno == primary_busno) 
	{
		*pci_occ_addr = (UINT32 *) POCCAR_ADDR;
		*pci_occ_data = (UINT32 *) POCCDR_ADDR;
	}
	else if (busno == secondary_busno) 
	{
		*pci_occ_addr = (UINT32 *) SOCCAR_ADDR;
		*pci_occ_data = (UINT32 *) SOCCDR_ADDR;
	}
	else if (busno <= bus0_lastbus)
	{
		*pci_occ_addr = (UINT32 *) POCCAR_ADDR;
		*pci_occ_data = (UINT32 *) POCCDR_ADDR;
	}
	else if (busno <= bus1_lastbus)
	{
		*pci_occ_addr = (UINT32 *) SOCCAR_ADDR;
		*pci_occ_data = (UINT32 *) SOCCDR_ADDR;
	}
	else return (ERROR);
	
	(void)pci_cycle_cleanup(busno);	/* start with clean slate */

	pci_config_cycle = 1; /* turn off exception handling during pci config cycle */

	if (old_abort_vec) {
	    printf("recursive config setup\n");
	    while (1);
	}

	old_abort_vec = ((volatile unsigned *)0x20)[4];
	((volatile unsigned *)0x20)[4] = (unsigned)__ignore_abort;
	_flushICache();

	pci_config_error = FALSE;

	/* turn off error messages which could be generated by non-existant devices */
	PRINT_OFF();

	if ((busno == PRIMARY_BUS_NUM) || (busno == SECONDARY_BUS_NUM))
	{
		/* set up the config access register for type 0 config cycles */
		**pci_occ_addr =
			(        
			(1 << ((devno & 0x1f) + 16)) |
			((funcno & 0x07) << 8)  |
			((regno & 0x3f) << 2)  |
			(0) );
    }
	else
	{
		/* set up the config access register for type 1 config cycles */
		**pci_occ_addr =
			(
			((busno & 0xff) << 16) |
			((devno & 0x1f) << 11) |
			((funcno& 0x07) << 8)  |
			((regno & 0x3f) << 2)  |
			(1) );
	}

	return (OK);
}

/**************************************************************************
* sys_read_config_byte - this function performs a PCI configuration cycle
*                        and returns a byte.
*
* This function will read a byte from config space on the PCI bus.  It is
* a user service intended to be called through the calls interface.
*/
ULONG sys_read_config_byte (UINT32 busno, UINT32 devno, UINT32 funcno, UINT32 offset, UINT8 *data)
{
	UINT32 *pci_occ_data;
	UINT32 regno;
    UINT32 *pci_occ_addr;

	/* initialize here to keep compiler happy */
	pci_occ_addr = (UINT32 *) POCCAR_ADDR;
	pci_occ_data = (UINT32 *) POCCDR_ADDR;
	
	/* Register numbers are DWORD indexes */
	regno = offset / 0x4;

	/* Set up the cycle. */
	if (sys_config_setup (busno, devno, funcno, regno, &pci_occ_addr, &pci_occ_data) != OK)
		return (ERROR);

	/* Now do the read */
	*data = (UINT8)(((*pci_occ_data) >> ((offset % 0x4) * 8)) & 0xff);
	
	if (pci_cycle_cleanup (busno) == OK)
	{
		PRINT_ON();
		return (OK);
	}
	else
	{
		PRINT_ON();
		return (ERROR);
	}
}

/**************************************************************************
* sys_read_config_word - this function performs a PCI configuration cycle
*                        and returns a 16-bit word.
*
* This function will read a word from config space on the PCI bus.  It is
* a user service intended to be called through the calls interface.
*/
ULONG sys_read_config_word (UINT32 busno, UINT32 devno, UINT32 funcno, UINT32 offset, UINT16 *data)
{
  UINT32 *pci_occ_data;
  UINT32 regno;
  UINT32 *pci_occ_addr;

	/* initialize here to keep compiler happy */
	pci_occ_addr = (UINT32 *) POCCAR_ADDR;
	pci_occ_data = (UINT32 *) POCCDR_ADDR;

	/* Offsets must be word-aligned */
	if (offset % 0x2) return (ERROR);
	
	/* Register numbers are DWORD indexes */
	regno = offset / 0x4;

	/* Set up the cycle. */
	if (sys_config_setup (busno, devno, funcno, regno, &pci_occ_addr, &pci_occ_data) != OK) return (ERROR);
	
	/* Now do the read */
	*data = (UINT16)(((*pci_occ_data) >> ((offset % 0x4) * 8)) & 0xffff);
	
	if (pci_cycle_cleanup(busno) == OK)
	{
		PRINT_ON();
		return (OK);
	}
	else
	{
		PRINT_ON();
		return (ERROR);
	}
}

/**************************************************************************
* sys_read_config_dword - this function performs a PCI configuration cycle
*                        and returns a 32-bit word.
*
* This function will read a dword from config space on the PCI bus.  It is
* a user service intended to be called through the calls interface.
*/
ULONG sys_read_config_dword (
	UINT32 busno,
	UINT32 devno,
	UINT32 funcno,
	UINT32 offset,
	UINT32 *data
	)
{
	UINT32 *pci_occ_data;
	UINT32 regno;
	UINT32 *pci_occ_addr;

	/* initialize here to keep compiler happy */
	pci_occ_addr = (UINT32 *) POCCAR_ADDR;
	pci_occ_data = (UINT32 *) POCCDR_ADDR;

	/* Offsets must be dword-aligned */
	if (offset % 0x4) return (ERROR);
	
	/* Register numbers are DWORD indexes */
	regno = offset / 0x4; 

	/* Set up the cycle. */
	if (sys_config_setup (busno, devno, funcno, regno, &pci_occ_addr, &pci_occ_data) != OK)
		return (ERROR);

	/* Now do the read */
	*data = *pci_occ_data;
	
	if (pci_cycle_cleanup (busno) == OK)
	{
		PRINT_ON();
		return (OK);
	}
	else
	{
		PRINT_ON();
		return (ERROR);
	}
}

/**************************************************************************
* sys_write_config_byte - this function performs a PCI configuration cycle
*                         and writes a byte.
*
* This function will write a byte to config space on the PCI bus.  It is
* a user service intended to be called through the calls interface.
*/
ULONG sys_write_config_byte (
	UINT32 busno,
	UINT32 devno,
	UINT32 funcno,
	UINT32 offset,
	UINT8 *data
	)
{
	UINT32 *pci_occ_data;
	UINT32 regno, temp;
    UINT32 *pci_occ_addr;
	UINT32 data_mask;

	/* initialize here to keep compiler happy */
	pci_occ_addr = (UINT32 *) POCCAR_ADDR;
	pci_occ_data = (UINT32 *) POCCDR_ADDR;
	
	/* Register numbers are DWORD indexes */
	regno = offset / 0x4;

	/* build mask for byte of interest */
	data_mask = ~(0x000000ff << ((offset % 0x4) * 8));

	/* Set up the cycle. */
	if (sys_config_setup (busno, devno, funcno, regno, &pci_occ_addr, &pci_occ_data) != OK)
		return (ERROR);

	/* set up 32-bit word, clear old data, OR in new data */
	temp = (UINT32)(((UINT32) *data) << ((offset % 0x4) * 8));
	*pci_occ_data &= data_mask;
	*pci_occ_data |= temp;
	
	if (pci_cycle_cleanup (busno) == OK)
	{
		PRINT_ON();
		return (OK);
	}
	else
	{
		PRINT_ON();
		return (ERROR);
	}
}

/**************************************************************************
* sys_write_config_word - this function performs a PCI configuration cycle
*                        and writes a 16-bit word.
*
* This function will write a word to config space on the PCI bus.  It is
* a user service intended to be called through the calls interface.
*/
ULONG sys_write_config_word (
	UINT32 busno,
	UINT32 devno,
	UINT32 funcno,
	UINT32 offset,
	UINT16 *data
	)
{
	UINT32 *pci_occ_data;
	UINT32 regno, temp;
    UINT32 *pci_occ_addr;
	UINT32 data_mask;

	/* initialize here to keep compiler happy */
	pci_occ_addr = (UINT32 *) POCCAR_ADDR;
	pci_occ_data = (UINT32 *) POCCDR_ADDR;

	/* Offsets must be word-aligned */
	if (offset % 0x2) return (ERROR);
	
	/* Register numbers are DWORD indexes */
	regno = offset / 0x4;

    /* build mask for word of interest */
	data_mask= ~(0x0000ffff << ((offset % 0x4) * 8));

	/* Set up the cycle. */
	if (sys_config_setup (busno, devno, funcno, regno, &pci_occ_addr, &pci_occ_data) != OK)
		return (ERROR);

	/* set up 32-bit word */
	temp = (UINT32)(((UINT32) *data) << ((offset % 0x4) * 8));
    *pci_occ_data &= data_mask; 
	*pci_occ_data |= temp;
		
	if (pci_cycle_cleanup (busno) == OK)
	{
		PRINT_ON();
		return (OK);
	}
	else
	{
		PRINT_ON();
		return (ERROR);
	}
}

/**************************************************************************
* sys_write_config_dword - this function performs a PCI configuration cycle
*                        and writes a 32-bit word.
*
* This function will write a dword to config space on the PCI bus.  It is
* a user service intended to be called through the calls interface.
*/
ULONG sys_write_config_dword (
	UINT32 busno,
	UINT32 devno,
	UINT32 funcno,
	UINT32 offset,
	UINT32 *data
	)
{
	UINT32 *pci_occ_data;
	UINT32 regno;
    UINT32 *pci_occ_addr;

	/* initialize here to keep compiler happy */
	pci_occ_addr = (UINT32 *) POCCAR_ADDR;
	pci_occ_data = (UINT32 *) POCCDR_ADDR;

	/* Offsets must be dword-aligned */
	if (offset % 0x4) return (ERROR);
	
	/* Register numbers are DWORD indexes */
	regno = offset / 0x4; 

	/* Set up the cycle. */
	if (sys_config_setup (busno, devno, funcno, regno, &pci_occ_addr, &pci_occ_data) != OK)
		return (ERROR);

	/* Now do the write */
	*pci_occ_data = *data;
	
	if (pci_cycle_cleanup (busno) == OK)
	{
		PRINT_ON();
		return (OK);
	}
	else
	{
		PRINT_ON();
		return (ERROR);
	}
}

/******************************************************************************
* sys_find_pci_device - find a PCI device based on Vendor ID and Device ID
*
* This function returns the location of PCI devices that have a specific
* Device ID and Vendor ID.  Given a Vendor ID, Device ID, and an Index, the
* function returns the Bus Number, Device Number, and Function Number of the
* Nth Device/Function whose Vendor ID and Device ID match the input parameters.
*
* Calling software can find all devices having the same Vendor ID and Device ID
* by making successive calls to this function starting with the index set to 0,
* and incrementing the index until the function returns DEVICE_NOT_FOUND.  A
* return value of BAD_VENDOR_ID indicates that the Vendor ID value passed had
* a value of all 1's.
*
*/
STATUS sys_find_pci_device (int vendor_id, int device_id, int index, PCI_DEVICE_LOCATION *devloc)
{
	int found;
	int multifunction = FALSE;
	USHORT vendid, devid;
	ULONG bus, device, function;
	UCHAR header_type;

	if (vendor_id == 0xffff) return (BAD_VENDOR_ID);

	found = 0;

	for (bus = 0; bus <= lastbus; bus++)
	{
		for (device = 0; device < MAX_DEVICE_NUMBER; device++)
		{
			for (function = 0; function < MAX_FUNCTION_NUMBER; function++)
			{

				/* before we go beyond function 0, make sure that the
				   device is truly a multi-function device, otherwise we may
				   get aliasing */
				if (function == 0)	
				{
					sys_read_config_byte (bus,device,function,HEADER_TYPE_OFFSET,&header_type);
					if (!(header_type & MULTIFUNCTION_DEVICE)) 
						multifunction = FALSE;
					else multifunction = TRUE;
				}


				/* If no device there, go on to next device */
				if ((sys_read_config_word (bus,device,function,VENDOR_ID_OFFSET,&vendid)== ERROR) ||
					(sys_read_config_word (bus,device,function,DEVICE_ID_OFFSET,&devid)== ERROR))
				{
					break;	/* go on to next device */
				}

				/* If not a match */
				if ((devid != device_id) || (vendid != vendor_id))
				{
					if (multifunction == FALSE) break;	/* go on to next device */
					else continue;						/* go on to next function */
				}

				/* If we've gotten this far we've found a match */

				/* check to see if we need to look for another occurrance */
				if (index-- != 0)
				{
					if (multifunction == FALSE) break;	/* go on to next device */
					else continue;						/* go on to next function */
				}

				/* found the correct occurrance (index) */
				else
				{
					devloc->bus_number = bus;
					devloc->device_number = device;
					devloc->function_number = function;
					return (OK);
				}
			} /* function */
		} /* device */
	} /* bus */

	/* If we haven't returned by this point, no match was found */
	return (DEVICE_NOT_FOUND);
}

/******************************************************************************
* sys_find_pci_class_code - find a PCI device based on a specific Class Code
*
* This function returns the location of PCI devices that have a specific
* Class Code.  Given a Class Code and an Index, the function returns the Bus
* Number, Device Number, and Function Number of the Nth Device/Function whose
* Class Code matches the input parameters.
*
* Calling software can find all devices having the same Class Code
* by making successive calls to this function starting with the index set to 0,
* and incrementing the index until the function returns DEVICE_NOT_FOUND.
*
*/
STATUS sys_find_pci_class_code (
    int class_code,
    int index,
	PCI_DEVICE_LOCATION *devloc
    )
{
    USHORT vendid;
    ULONG bus, device, function;
    UCHAR dev_class, header_type;
	int multifunction = FALSE;

    for (bus = 0; bus <= lastbus; bus++)   
	{
        for (device = 0; device <= MAX_DEVICE_NUMBER; device++)   
		{

			for (function = 0; function < MAX_FUNCTION_NUMBER; function++)	
			{

				/* before we go beyond function 0, make sure that the
				   device is truly a multi-function device, otherwise we may
				   get aliasing */
				if (function == 0)	
				{
					sys_read_config_byte (bus,device,function,HEADER_TYPE_OFFSET,&header_type);
					if (!(header_type & MULTIFUNCTION_DEVICE)) multifunction = FALSE;
					else multifunction = TRUE;
				}

				/* If no device there, go on to next device */
				if ((sys_read_config_word (bus,device,function,VENDOR_ID_OFFSET,&vendid)== ERROR) ||
					(sys_read_config_byte (bus,device,function,BASE_CLASS_OFFSET,&dev_class)== ERROR))
				{
					break;	/* go on to next device */
				}
				
				/* If not a match */
				if (dev_class != class_code)
				{
					if (multifunction == FALSE) break;	/* go on to next device */
					else continue;						/* go on to next function */
				}

				/* If we've gotten this far we've found a match */

				/* check to see if we need to look for another occurrance */
				if (index-- != 0)
				{
					if (multifunction == FALSE) break;	/* go on to next device */
					else continue;						/* go on to next function */
				}

				/* found the correct occurrance (index) */
				else
				{
					devloc->bus_number = bus;
					devloc->device_number = device;
					devloc->function_number = function;
					return (OK);
				}
			}
		}
    }  
    /* If we haven't returned by this point, no match was found */
    return (DEVICE_NOT_FOUND); 
}

/******************************************************************************
* sys_pci_bios_present - determine if a PCI BIOS is present
*
* This function allows the caller to determine whether the PCI BIOS interface
* function set is present, and what the current interface version level is.
* It also provides information about what hardware mechanism is used for
* accessing configuration space and whether or not the hardware supports
* generation of PCI Special Cycles.
*
*/
STATUS sys_pci_bios_present (PCI_BIOS_INFO *info)
{
	/* 0x00 indicates PCI BIOS functions present */
	info->present_status = 0x00;

	/* Config types 0 or 1, no special cycle mechanisms */
	info->hardware_mech_config = CONFIG_MECHANISM_1 | CONFIG_MECHANISM_2;
	info->hardware_mech_special = 0x00;

	info->if_level_major_ver = 0x02;
	info->if_level_minor_ver = 0x01;

	info->last_pci_bus = lastbus;

	return (OK);
}


/******************************************************************************
* sys_generate_special_cycle - generate a PCI Special Cycle
*
* This function allows for generation of PCI Special Cycles.  The generated
* special cycle will be broadcast on a specific PCI Bus in the system.
*
* PCI Special Cycles are not supported by Cyclone Hardware.
*
*/
STATUS sys_generate_special_cycle (int	bus_number, int	special_cycle_data)
{
    return (FUNC_NOT_SUPPORTED);
}

/******************************************************************************
* sys_get_irq_routing_options - get the PCI interrupt routing options
*
* The PCI Interrupt routing fabric on the Cyclone Hardware is not 
* reconfigurable (fixed mapping relationships).
*
*/
STATUS sys_get_irq_routing_options (PCI_IRQ_ROUTING_TABLE *table)
{
    return (FUNC_NOT_SUPPORTED);
}

/******************************************************************************
* sys_set_pci_irq - connect a PCI interrupt to a processor IRQ.
*
* The PCI Interrupt routing fabric on the Cyclone Hardware is not 
* reconfigurable (fixed mapping relationships) and therefore, this function
* is not supported.
*
*/
STATUS sys_set_pci_irq (
	int	int_pin,
	int	irq_num,
	int	bus_dev
	)
{
    return (FUNC_NOT_SUPPORTED);
}

/******************************************************************************
*
* print_config_space - print contents of config space
*
* This function prints out the contents of the PCI configuration space for
* the selected PCI device.  The local RN functions are accessible using bus= -1
* and function=0 for the Bridge and bus = -1 function=1 for the ATU.
*
*/
void print_config_space (int busno, int devno, int function)
{
	ULONG offset;
	UINT32 long_data;
	UINT16 short_data;
	UINT8 byte_data;
	register PCI_CONFIG_SPACE *cptr = 0;
	USHORT vendor_id = 0xffff;
	USHORT device_id = 0xffff;
    USHORT subvendor_id = 0xffff;
    USHORT subdevice_id = 0xffff;
	int header_type;

	if ((busno > (int)lastbus) || (busno < -1))
	{
		printf("Invalid bus number = %d\n", busno);
		return;
	}
 
	if (devno < -1)
	{
		printf("Invalid device number = %d\n", devno);
		return;
	}

	if ((busno == -1) && (devno == -1) && (function == 0))	/* local Bridge device */
	{
		cptr = (PCI_CONFIG_SPACE *) VIDR_ADDR;
		printf("\n\n\nReading Configuration Space for 80960RN PCI-PCI Bridge\n");
	}
	else if ((busno == -1) && (devno == -1) && (function == 1))	/* local ATU device */
	{
		cptr = (PCI_CONFIG_SPACE *) ATUVID_ADDR;
		printf("\n\n\nReading Configuration Space for 80960RN ATU\n");
	}

	if ((busno == -1) && (devno == -1) && (function == 0))
	{
        /* this is the bridge function */
		printf("-----------------------------------------------------------------\n\n");
		printf("Vendor ID             = 0x%04X   ",cptr->pci1_config.vendor_id);
		printf("Device ID             = 0x%04X\n",cptr->pci1_config.device_id);
		printf("Command Register      = 0x%04X   ",cptr->pci1_config.command);
		printf("Status Register       = 0x%04X\n",cptr->pci1_config.status);
		printf("Revision ID           = 0x%02X     ",cptr->pci1_config.revision_id);
		printf("Programming Interface = 0x%02X\n",cptr->pci1_config.prog_if);
		printf("Sub Class             = 0x%02X     ",cptr->pci1_config.sub_class);
		printf("Base Class            = 0x%02X\n",cptr->pci1_config.base_class);
		printf("Cache Line Size       = 0x%02X     ",cptr->pci1_config.cache_line_size);
		printf("Latency Timer         = 0x%02X\n",cptr->pci1_config.latency_timer);
		printf("Header Type           = 0x%02X     ",cptr->pci1_config.header_type);
		printf("BIST                  = 0x%02X\n",cptr->pci1_config.bist);
		printf("Primary Bus Number    = 0x%02X     ",cptr->pci1_config.primary_busno);
		printf("Secondary Bus Number  = 0x%02X\n",cptr->pci1_config.secondary_busno);
		printf("Sub Bus Number        = 0x%02X     ",cptr->pci1_config.subordinate_busno);
		printf("Secondary Latency     = 0x%02X\n",cptr->pci1_config.secondary_latency_timer);
		printf("Secondary I/O Base    = 0x%02X     ",cptr->pci1_config.io_base);
		printf("Secondary I/O Limit   = 0x%02X\n",cptr->pci1_config.io_limit);
		printf("Secondary Status      = 0x%04X   ",cptr->pci1_config.secondary_status);
		printf("Secondary Memory Base = 0x%04X\n",cptr->pci1_config.mem_base);
		printf("Secondary Mem Limit   = 0x%04X   ",cptr->pci1_config.mem_limit);
		printf("Prefetch Memory Base  = 0x%04X\n",cptr->pci1_config.pfmem_base);
		printf("Prefetch Memory Limit = 0x%04X   ",cptr->pci1_config.pfmem_limit);
		printf("I/O Base (Upper)      = 0x%04X\n",cptr->pci1_config.iobase_upper16);
		printf("I/O Limit (Upper)     = 0x%04X   ",cptr->pci1_config.iolimit_upper16);
		printf("Subsystem Vendor ID   = 0x%04X\n",cptr->pci1_config.sub_vendor_id);
 		printf("Subsystem ID          = 0x%04X   ",cptr->pci1_config.sub_device_id);
		printf("Interrupt Line        = 0x%02X\n",cptr->pci1_config.int_line);
		printf("Interrupt Pin         = 0x%02X     ",cptr->pci1_config.int_pin);
		printf("Bridge Control        = 0x%04X\n",cptr->pci1_config.bridge_control);
		printf("PCI Range 0 Base      = 0x%08X\n",cptr->pci1_config.pcibase_addr0);
		printf("PCI Range 1 Base      = 0x%08X\n",cptr->pci1_config.pcibase_addr1);
		printf("Prefetch Base (Upper) = 0x%08X\n",cptr->pci1_config.pfbase_upper32);
		printf("Prefetch Limit(Upper) = 0x%08X\n",cptr->pci1_config.pflimit_upper32);
		printf("Expansion ROM Base    = 0x%08X\n",cptr->pci1_config.pcibase_exp_rom);
	}
	else if ((busno == -1) && (devno == -1) && (function == 1))
	{
        /* this is the ATU function */
		printf("------------------------------------------------------------------\n\n");
		printf("Vendor ID             = 0x%04X   ",cptr->pci0_config.vendor_id);
		printf("Device ID             = 0x%04X\n",cptr->pci0_config.device_id);
		printf("Command Register      = 0x%04X   ",cptr->pci0_config.command);
		printf("Status Register       = 0x%04X\n",cptr->pci0_config.status);
		printf("Revision ID           = 0x%02X     ",cptr->pci0_config.revision_id);
		printf("Programming Interface = 0x%02X\n",cptr->pci0_config.prog_if);
		printf("Sub Class             = 0x%02X     ",cptr->pci0_config.sub_class);
		printf("Base Class            = 0x%02X\n",cptr->pci0_config.base_class);
		printf("Cache Line Size       = 0x%02X     ",cptr->pci0_config.cache_line_size);
		printf("Latency Timer         = 0x%02X\n",cptr->pci0_config.latency_timer);
		printf("Header Type           = 0x%02X     ",cptr->pci0_config.header_type);
		printf("BIST                  = 0x%02X\n",cptr->pci0_config.bist);
		printf("Interrupt Line        = 0x%02X     ",cptr->pci0_config.int_line);
		printf("Interrupt Pin         = 0x%02X\n",cptr->pci0_config.int_pin);
		printf("Minimum Grant         = 0x%02X     ",cptr->pci0_config.min_gnt);
		printf("Maximum Latency       = 0x%02X\n",cptr->pci0_config.max_lat);
		printf("Subsystem Vendor ID   = 0x%04X   ",cptr->pci0_config.sub_vendor_id);
 		printf("Subsystem ID          = 0x%04X\n",cptr->pci0_config.sub_device_id);
		printf("Base Address 0        = 0x%08X\n",cptr->pci0_config.pcibase_addr0);
		printf("Base Address 1        = 0x%08X\n",cptr->pci0_config.pcibase_addr1);
 		printf("Base Address 2        = 0x%08X\n",cptr->pci0_config.pcibase_addr2);
		printf("Base Address 3        = 0x%08X\n",cptr->pci0_config.pcibase_addr3);
 		printf("Base Address 4        = 0x%08X\n",cptr->pci0_config.pcibase_addr4);
		printf("Base Address 5        = 0x%08X\n",cptr->pci0_config.pcibase_addr5);
 		printf("Cardbus CIS Pointer   = 0x%08X\n",cptr->pci0_config.cardbus_cis_ptr);
		printf("Expansion ROM Base    = 0x%08X\n",cptr->pci0_config.pcibase_exp_rom);

	}

	else		/* device on PCI bus */
	{
		printf("\n\n\nReading Configuration Space for PCI Bus 0x%02X, Device 0x%02X, Function 0x%02X\n",
			busno, devno, function);
		printf("------------------------------------------------------------------------\n\n");

		/* get Vendor Id */
		offset = VENDOR_ID_OFFSET;
		if (sys_read_config_word (busno,devno,function,offset,&short_data)
			== ERROR)
		{
			printf("Error reading Vendor Id\n");
			return;
		}
		if (short_data == 0xffff)	/* non-existant device */
		{
			printf("No such device.\n");
			return;
		}
		printf("Vendor ID             = 0x%04X   ",short_data);
		vendor_id = short_data;

        /* get Device Id */
		offset = DEVICE_ID_OFFSET;
        if (sys_read_config_word (busno,devno,function,offset,&short_data)
                == ERROR)
        {
            printf("Error reading Device Id\n");
            return;
        }
		printf("Device ID             = 0x%04X\n",short_data);
		device_id = short_data;

		/* get Command Register */
        offset = COMMAND_OFFSET;
        if (sys_read_config_word (busno,devno,function,offset,&short_data)
                == ERROR)
        {
            printf("Error reading Command Register\n");
            return;
        }
        printf("Command Register      = 0x%04X   ",short_data);
 
        /* get Status Register */
        offset = STATUS_OFFSET;
        if (sys_read_config_word (busno,devno,function,offset,&short_data)
                == ERROR)
        {
            printf("Error reading Status Register\n");
            return;
        }
        printf("Status Register       = 0x%04X\n",short_data);

		/* get Revision Id */
        offset = REVISION_OFFSET;
        if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
        {
            printf("Error reading Revision Id\n");
            return;
        }
        printf("Revision ID           = 0x%02X     ",byte_data);
 
        /* get Programming Interface */
        offset = PROG_IF_OFFSET;
        if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
        {
            printf("Error reading Programming Interface\n");
            return;
        }
        printf("Programming Interface = 0x%02X\n",byte_data);

		/* get Sub Class */
        offset = SUB_CLASS_OFFSET;
        if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
        {
            printf("Error reading Sub Class\n");
            return;
        }
        printf("Sub Class             = 0x%02X     ",byte_data);
 
        /* get Base Class */
        offset = BASE_CLASS_OFFSET;
        if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
        {
            printf("Error reading Base Class\n");
            return;
		}
        printf("Base Class            = 0x%02X\n",byte_data);

		/* get Cache Line Size */
        offset = CACHE_LINE_OFFSET;
        if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
        {
            printf("Error reading Cache Line Size\n");
            return;
        }
        printf("Cache Line Size       = 0x%02X     ",byte_data);
 
        /* get Latency Timer */
        offset = LATENCY_TIMER_OFFSET;
        if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
        {
            printf("Error reading Latency Timer\n");
            return;
		}
        printf("Latency Timer         = 0x%02X\n",byte_data);

		/* get Header Type */
        offset = HEADER_TYPE_OFFSET;
        if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
        {
            printf("Error reading Header Type\n");
            return;
        }
        printf("Header Type           = 0x%02X     ",byte_data);
		header_type = (((int)byte_data) & 0x7f); /* strip multifunction bit */
        
		/* get BIST */
        offset = BIST_OFFSET;
        if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
        {
            printf("Error reading BIST\n");
            return;
		}
        printf("BIST                  = 0x%02X\n",byte_data);

        /* get Interrupt Line info */
        offset = INT_LINE_OFFSET;
        if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
        {
            printf("Error reading Interrupt Line\n");
            return;
        }
        printf("Interrupt Line        = 0x%02X     ",byte_data);
 
        /* get Interrupt Pin info */
        offset = INT_PIN_OFFSET;
        if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
        {
            printf("Error reading Interrupt Pin\n");
            return;
        }
        printf("Interrupt Pin         = 0x%02X\n",byte_data);

        /* get Capabilities Pointer info */
        offset = CAP_PTR_OFFSET;
        if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
        {
            printf("Error reading Capabilities Pointer register\n");
            return;
        }
        printf("Capabilities Pointer  = 0x%02X\n",byte_data);

		if (header_type == 0) /* type 0 header-specific info */ 
		{
			/* get Min Gnt info */
			offset = MIN_GNT_OFFSET;
			if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
			{
				printf("Error reading Minimum Grant\n");
				return;
			}
			printf("Minimum Grant         = 0x%02X     ",byte_data);
 
			/* get Max Lat info */
			offset = MAX_LAT_OFFSET;
			if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
			{
				printf("Error reading Maximum Latency\n");
				return;
			}
			printf("Maximum Latency       = 0x%02X\n",byte_data);

			/* get subsystem vendor ID */
			offset = SUB_VENDOR_ID_OFFSET;
			if (sys_read_config_word (busno,devno,function,offset,&short_data)
                == ERROR)
			{
				printf("Error reading Subsystem Vendor ID\n");
				return;
			}
			printf("Subsystem Vendor ID   = 0x%04X   ",short_data);
			subvendor_id = short_data;
		
			/* get subsystem device ID */
			offset = SUB_DEVICE_ID_OFFSET;
			if (sys_read_config_word (busno,devno,function,offset,&short_data)
                == ERROR)
			{
				printf("Error reading Subsystem Device ID\n");
				return;
			}
			printf("Subsystem Device ID   = 0x%04X\n",short_data);
			subdevice_id = short_data;

			/* get Region 0 Base Address */
			offset = REGION0_BASE_OFFSET;
			if (sys_read_config_dword (busno,devno,function,offset,&long_data)
                == ERROR)
			{
				printf("Error reading Region 0 Base\n");
				return;
			}
			printf("PCI Region 0 Base     = 0x%08X\n",long_data);

			/* get Region 1 Base Address */
			offset = REGION1_BASE_OFFSET;
			if (sys_read_config_dword (busno,devno,function,offset,&long_data)
                == ERROR)
			{
				printf("Error reading Region 1 Base\n");
				return;
			}
			printf("PCI Region 1 Base     = 0x%08X\n",long_data);

			/* get Region 2 Base Address */
			offset = REGION2_BASE_OFFSET;
			if (sys_read_config_dword (busno,devno,function,offset,&long_data)
                == ERROR)
			{
				printf("Error reading Region 2 Base\n");
				return;
			}
			printf("PCI Region 2 Base     = 0x%08X\n",long_data);

			/* get Region 3 Base Address */
			offset = REGION3_BASE_OFFSET;
			if (sys_read_config_dword (busno,devno,function,offset,&long_data)
                == ERROR)
			{
				printf("Error reading Region 3 Base\n");
				return;
			}
			printf("PCI Region 3 Base     = 0x%08X\n",long_data);

			/* get Region 4 Base Address */
			offset = REGION4_BASE_OFFSET;
			if (sys_read_config_dword (busno,devno,function,offset,&long_data)
                == ERROR)
			{
				printf("Error reading Region 4 Base\n");
				return;
			}
			printf("PCI Region 4 Base     = 0x%08X\n",long_data);

			/* get Region 5 Base Address */
			offset = REGION5_BASE_OFFSET;
			if (sys_read_config_dword (busno,devno,function,offset,&long_data)
                == ERROR)
			{
				printf("Error reading Region 5 Base\n");
				return;
			}
			printf("PCI Region 5 Base     = 0x%08X\n",long_data);

			/* get Expansion ROM Base Address */
			offset = EXP_ROM_OFFSET;
			if (sys_read_config_dword (busno,devno,function,offset,&long_data)
                == ERROR)
			{
				printf("Error reading Expansion ROM Base\n");
				return;
			}
			printf("Expansion ROM Base    = 0x%08X\n",long_data);

			/* get Cardbus CIS pointer */
			offset = CARDBUS_CISPTR_OFFSET;
			if (sys_read_config_dword (busno,devno,function,offset,&long_data)
                == ERROR)
			{
				printf("Error reading Cardbus CIS Pointer\n");
				return;
			}
			printf("Cardbus CIS Pointer   = 0x%08X\n",long_data);
		} /* end type 0 header */

		else /* type 1 header_specific info */
		{
			/* get Bridge Control */
			offset = BRIDGE_CTRL_OFFSET;
			if (sys_read_config_word (busno,devno,function,offset,&short_data)
                == ERROR)
			{
				printf("Error reading Bridge Control Register\n");
				return;
			}
			printf("Bridge Control Reg    = 0x%04X   ",short_data);
	
			/* get Secondary Status */
			offset = SECONDARY_STAT_OFFSET;
			if (sys_read_config_word (busno,devno,function,offset,&short_data)
                == ERROR)
			{
				printf("Error reading Secondary Status Register\n");
				return;
			}
			printf("Secondary Status Reg  = 0x%04X\n",short_data);
		
			/* get Primary Bus Number */
			offset = PRIMARY_BUSNO_OFFSET;
			if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
			{
				printf("Error reading Primary Bus Number\n");
				return;
			}
			printf("Primary Bus No.       = 0x%02X     ",byte_data);
 	
			/* get Secondary Bus Number */
			offset = SECONDARY_BUSNO_OFFSET;
			if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
			{
				printf("Error reading Secondary Bus Number\n");
				return;
			}
			printf("Secondary Bus No.     = 0x%02X\n",byte_data);
	
			/* get Subordinate Bus Number */
			offset = SUBORD_BUSNO_OFFSET;
			if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
			{
				printf("Error reading Subordinate Bus Number\n");
				return;
			}
			printf("Subordinate Bus No.   = 0x%02X     ",byte_data);
	
			/* get Secondary Latency Timer */
			offset = SECONDARY_LAT_OFFSET;
			if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
			{
				printf("Error reading Secondary Latency Timer\n");
				return;
			}
			printf("Secondary Latency Tmr = 0x%02X\n",byte_data);
 
			/* get IO Base */
			offset = IO_BASE_OFFSET;
			if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
			{
				printf("Error reading IO Base\n");
				return;
			}
			printf("IO Base               = 0x%02X     ",byte_data);

			/* get IO Limit */
			offset = IO_LIMIT_OFFSET;
			if (sys_read_config_byte (busno,devno,function,offset,&byte_data)
                == ERROR)
			{
				printf("Error reading IO Limit\n");
				return;
			}
			printf("IO Limit              = 0x%02X\n",byte_data);
	
			/* get Memory Base */
			offset = MEMORY_BASE_OFFSET;
			if (sys_read_config_word (busno,devno,function,offset,&short_data)
                == ERROR)
			{
				printf("Error reading Memory Base\n");
				return;
			}
			printf("Memory Base           = 0x%04X   ",short_data);
		
			/* get Memory Limit */
			offset = MEMORY_LIMIT_OFFSET;
			if (sys_read_config_word (busno,devno,function,offset,&short_data)
                == ERROR)
			{
				printf("Error reading Memory Limit\n");
				return;
			}
			printf("Memory Limit          = 0x%04X\n",short_data);
	
			/* get Prefetchable Memory Base */
			offset = PREF_MEM_BASE_OFFSET;
			if (sys_read_config_word (busno,devno,function,offset,&short_data)
			    == ERROR)
			{
				printf("Error reading Prefetchable Memory Base\n");
				return;
			}
			printf("Pref. Memory Base     = 0x%04X   ",short_data);	

			/* get Prefetchable Memory Limit */
			offset = PREF_MEM_LIMIT_OFFSET;
			if (sys_read_config_word (busno,devno,function,offset,&short_data)
                == ERROR)
			{
				printf("Error reading Prefetchable Memory Limit\n");
				return;
			}
			printf("Pref. Memory Limit    = 0x%04X\n",short_data);

			/* get IO Base Upper 16 Bits */
			offset = IO_BASE_UPPER_OFFSET;
			if (sys_read_config_word (busno,devno,function,offset,&short_data)
                == ERROR)
			{
				printf("Error reading IO Base Upper 16 Bits\n");
				return;
			}
			printf("IO Base Upper 16 Bits = 0x%04X   ",short_data);	

			/* get IO Limit Upper 16 Bits */
			offset = IO_LIMIT_UPPER_OFFSET;
			if (sys_read_config_word (busno,devno,function,offset,&short_data)
                == ERROR)
			{
				printf("Error reading IO Limit Upper 16 Bits\n");
				return;
			}
			printf("IO Limit Upper 16 Bits= 0x%04X\n",short_data);        
	
			/* get Prefetchable Base Upper 32 Bits */
			offset = PREF_BASE_UPPER_OFFSET;
			if (sys_read_config_dword (busno,devno,function,offset,&long_data)
                == ERROR)
			{
				printf("Error reading Prefetchable Base Upper 32 Bits\n");
				return;
			}
			printf("Pref. Base Up. 32 Bits= 0x%08X\n",long_data);

			/* get Prefetchable Limit Upper 32 Bits */
			offset = PREF_LIMIT_UPPER_OFFSET;
			if (sys_read_config_dword (busno,devno,function,offset,&long_data)
                == ERROR)
			{
				printf("Error reading Prefetchable Limit Upper 32 Bits\n");
				return;
			}
			printf("Pref. Lmt. Up. 32 Bits= 0x%08X\n",long_data);
	

			/* get Region 0 Base Address */
			offset = REGION0_BASE_OFFSET;
			if (sys_read_config_dword (busno,devno,function,offset,&long_data)
                == ERROR)
			{
				printf("Error reading Region 0 Base\n");
				return;
			}
			printf("PCI Region 0 Base     = 0x%08X\n",long_data);

			/* get Region 1 Base Address */
			offset = REGION1_BASE_OFFSET;
			if (sys_read_config_dword (busno,devno,function,offset,&long_data)
                == ERROR)
			{
				printf("Error reading Region 1 Base\n");
				return;
			}
			printf("PCI Region 1 Base     = 0x%08X\n",long_data);

			/* get Expansion ROM Base Address */
			offset = TYPE1_EXP_ROM_OFFSET;
			if (sys_read_config_dword (busno,devno,function,offset,&long_data)
                == ERROR)
			{
				printf("Error reading Expansion ROM Base\n");
				return;
			}
			printf("Expansion ROM Base    = 0x%08X\n",long_data);

		} /* end type 1 header */

    }
}

/* check if host of backplane */
int isHost()
{
	if (*BACKPLANE_DET_REG & BP_HOST_BIT)
		return TRUE;
	else
		return FALSE;
}




/*****************************************************************************
* sys_pci_device_initialization - initialize PCI I/O devices
*
* This function is responsible for initializing all PCI I/O devices for proper
* PCI operation.  All I/O devices are mapped into appropriate locations in the
* PCI address space (based on size and alignment requirements).  This function
* must ensure that no bus conflicts exist.  This function is also responsible
* for initializing cache line size, latency timer, DEVSEL# timing, and parity
* error response.  This function fills out the data structure which is passed
* in to it.  This function will return SUCCESSFUL if at least one I/O
* controller can be successfully configured.  This function will return ERROR
* if no I/O controllers can be initialized.
*
*/
void sys_pci_device_initialization (PCI_DATA* pci_data)
{   
	
	volatile int i;

	bus0_lastbus = PRIMARY_BUS_NUM;        /* last bus number behind bus 0 (primary bus ) */
    bus1_lastbus = SECONDARY_BUS_NUM;      /* last bus number behind bus 1 (secondary bus) */

	pci_data->num_devices = 0;	 /* total number of devices configured */
	pci_data->num_functions = 0; /* total number of functions configured */

	init_312_pci(); /* initialize the ATU, MU and bridge on the 80312 */

	if (isHost())
	{
	  /* If the IQ80310 board is connected to the backplane it has to initialize the Primary PCI bus */

      /* set PCI base addresses for primary bus */

	  memspace_ptr[PRIMARY_BUS_NUM] = PRIMARY_MEM_BASE;
	  memspace_limit[PRIMARY_BUS_NUM] = PRIMARY_MEM_LIMIT;

	  iospace_ptr[PRIMARY_BUS_NUM] = PRIMARY_IO_BASE;
	  iospace_limit[PRIMARY_BUS_NUM] = PRIMARY_IO_LIMIT;

	  PRINT_OFF();

	  /* Initialize Primary PCI bus */
	  sys_pci_bus_init (PRIMARY_BUS_NUM, PRIMARY_BUS_NUM, pci_data);
	}

	/* Initialization of the Secondary PCI bus */

    /* set PCI base addresses for secondary bus */

	memspace_ptr[SECONDARY_BUS_NUM] = SECONDARY_MEM_BASE;
	memspace_limit[SECONDARY_BUS_NUM] = SECONDARY_MEM_LIMIT;

	iospace_ptr[SECONDARY_BUS_NUM] = SECONDARY_IO_BASE;
	iospace_limit[SECONDARY_BUS_NUM] = SECONDARY_IO_LIMIT;

	PRINT_OFF();
    
	/* Initialize Secondary PCI bus */
	sys_pci_bus_init (SECONDARY_BUS_NUM, SECONDARY_BUS_NUM, pci_data);

	
	/* Set absolute last secondary bus */
	if (bus0_lastbus > bus1_lastbus)
	  lastbus = bus0_lastbus;	
	else
	  lastbus = bus1_lastbus;

	PRINT_OFF();

	/* before exiting, configure and enable FIQ and IRQ interrupts */
	
	_enableFiqIrq();  /* enable FIQ and IRQ interrupts in CP13 */
	
	config_ints();   /* configure interrupts */

	/* wait a while to clear out any unwanted NMI interrupts */
	for (i = 0; i < 100000; i++)
		;

	PRINT_ON();

	/* enable ECC single bit correction and multi-bit reporting
	   - initialization code only enables correction */
	*(volatile unsigned long *)ECCR_ADDR = 0x6;
}


/*****************************************************************************
* sys_pci_bus_init - initialize PCI I/O devices
*
* This function is responsible for initializing all PCI I/O devices for proper 
* PCI operation.  All I/O devices are mapped into appropriate locations in the
* PCI address space (based on size and alignment requirements).  This function 
* must ensure that no bus conflicts exist.  This function is also responsible 
* for initializing cache line size, latency timer, DEVSEL# timing, and parity
* error response.  This function fills out the data structure which is passed
* in to it.  This function will return SUCCESSFUL if at least one I/O 
* controller can be successfully configured.  This function will return ERROR 
* if no I/O controllers can be initialized.
*/
static void sys_pci_bus_init(UINT bus, UINT root_bus, PCI_DATA* pci_data)
{
	UINT   device, function;
	USHORT vendor, device_id;
	UINT   regno;
	ULONG  regvalue;
	USHORT regshort;
	UCHAR  regchar;
	UCHAR  intpin, intline;
	UINT   line;
	UCHAR  header_type;
	ULONG  rom_enabled;
	UINT   base_addr_reg_start;
	UINT   base_addr_reg_end;
	UINT   exp_rom_baseaddr;
	int	   multifunction;

/* bridge initialization variables */
	ULONG class_code;
	UINT  secondary_bus_number;
	UCHAR iospace_type;
	UCHAR data_byte;
	USHORT data_word;
	ULONG membase, iobase;
	ULONG memlimit, iolimit;
	int no_memory, no_io;

#ifdef DEBUG_PCI
	printf ("Configuring PCI Bus %d.\n", bus);
	printf ("PCI Root Bus %d.\n", root_bus);
	PRINT_OFF();
#endif

for (device = 0; device <= MAX_DEVICE_NUMBER; device++)
{
#ifdef DEBUG_PCI
	printf ("\nConfiguring device %d.\n\n", device);
#endif
	
	/* assume non-multifunction device at start */
	multifunction = FALSE;		
	
	for (function = 0; function < MAX_FUNCTION_NUMBER; function ++)
	{

#ifdef DEBUG_PCI
		printf ("Configuring function %d.\n", function);
#endif	  
	  /* To determine whether a device is installed in a particular slot, */
	  /* we attempt to read the vendor ID from that slot.  If there's     */
	  /* nothing there, the bridge should return 0xffff (and signal a     */
	  /* master abort).  Otherwise, there is something there and we need  */
	  /* to configure it.                                                 */
	  if ((sys_read_config_word(bus, device, function, VENDOR_ID_OFFSET, &vendor) == ERROR) || (vendor == 0xffff)) 
		{
	   	/* This means no device found */
#ifdef DEBUG_PCI
			printf ("\nNo device.\n");
#endif
			break;		/* Go on to the next device */
		}

	  /* We'll only get here if we got a real device/function */
		
		/* Count the number of devices */
		if (function == 0) (pci_data->num_devices)++;
		
		/* Count the number of functions */
		(pci_data->num_functions)++;

#ifdef DEBUG_PCI
		printf ("Vendor:  %x\n", vendor);
#endif

	  /* Read the Device ID */
		sys_read_config_word (bus,device,function,DEVICE_ID_OFFSET,&device_id);

#ifdef DEBUG_PCI
		printf("\n");
		printf("Configuring PCI Bus   : %d\n", bus);
		printf("            PCI Device: %d\n", device);
		printf("            Vendor Id : 0x%08X\n", vendor);
		printf("            Device Id : 0x%08X\n", device_id);
#endif

	  /* Read the Configuration Header Type to determine configuration */
	  if (sys_read_config_byte (bus,device,function,HEADER_TYPE_OFFSET,&header_type)== ERROR) continue;

		/* determine if multifunction device */
		if ((header_type & MULTIFUNCTION_DEVICE) && (function == 0))
		{
#ifdef DEBUG_PCI
			printf ("Multifunction Device Found...\n");
#endif
			multifunction = TRUE;
		}

		/* strip off multifunction device indicator bit */
		header_type &= 0x7f;

		switch (header_type)
		{
			case STANDARD_HEADER:
		    base_addr_reg_start = 0x10;	/* Base Address Reg. Start Offset      */
		    base_addr_reg_end =	0x24;	/* Base Address Reg. End Offset        */
		    exp_rom_baseaddr = 0x30;	/* Expansion ROM Base Addr. Reg Offset */
		    break;

			case PCITOPCI_HEADER:
		    base_addr_reg_start = 0x10;	/* Base Address Reg. Start Offset      */
		    base_addr_reg_end =	0x14;	/* Base Address Reg. End Offset        */
		    exp_rom_baseaddr = 0x38;	/* Expansion ROM Base Addr. Reg Offset */
		    break;

			default:
		    printf ("Header Type Not Supported, 0x%02X\n", header_type);
		    continue;	/* skip over device */
		    break;
		}

		/* We cycle through the base registers, first writing out all  */
		/* ones to the register, then reading it back to determine the */
		/* requested size in either I/O or memory space.  Then we      */
		/* align the top of memory or i/o space pointer, write it to   */
		/* the register, and increment it.  I say we so you won't feel */
		/* excluded.                                                   */
		for (regno = base_addr_reg_start; regno <= base_addr_reg_end; regno += 4) 
		{	
		
			/* Write out all 1's to the base register, then read it back */
			regvalue = 0xffffffff;
			sys_write_config_dword(bus,device,function,regno,(UINT32*)&regvalue);
			sys_read_config_dword (bus,device,function,regno,(UINT32*)&regvalue);

			/* Some number of the lower bits of regvalue will be clear */
			/* indicating a don't care position.  The more clear bits, */
			/* the larger the requested mapping space. */

			if (regvalue == 0)				/* ...this reg not used. */
				;

			/*---------------------------------------------------------*/
			/* I/O space mapping                                       */
			/*---------------------------------------------------------*/
			else if ((regvalue & 3) == 1) /* ...this is I/O space */
			{	
				ULONG requested_size;

				/* Align iospace_ptr for the requested size.  All bit */
				/* positions clear in regvalue must be clear in       */
				/* iospace_ptr. */
				requested_size = ~(regvalue & 0xfffffffe);

#ifdef DEBUG_PCI
				printf ("Configuration for I/O space...\n");
				printf ("Size request:  %x; ", requested_size);
				printf ("IOspace_ptr:   %x\n", iospace_ptr[root_bus]);
#endif

				/* check to make sure that there is enought iospace left to grant */
				if ((iospace_ptr[root_bus] + requested_size) > iospace_limit[root_bus])
				{
					printf ("\nPCI Configuration ERROR: Out of I/O Space on Bus %d!\n", bus);
					printf ("  No I/O Space Allocated to Device %d, Function %d.\n", device, function);
					return;
				}

				/* Align the space pointer if necessary */
				if (iospace_ptr[root_bus] & requested_size) 
				{
					iospace_ptr[root_bus] &= ~requested_size;
					iospace_ptr[root_bus] += requested_size + 1;
				}
#ifdef DEBUG_PCI
				printf("Adjusted I/O Space Ptr:  %x\n", iospace_ptr[root_bus]);
	    		printf("       I/O Space Size : 0x%08X\n", requested_size);
	    		printf("       I/O Space Base : 0x%08X\n", iospace_ptr[root_bus]);
#endif				/* Write out the adjusted iospace pointer. */
				
				sys_write_config_dword (bus,device,function,regno,(UINT32*)&iospace_ptr[root_bus]);

				/* Update space pointer */
				iospace_ptr[root_bus] += requested_size + 1;
			}

			/*---------------------------------------------------------*/
			/* Memory space mapping                                    */
			/*---------------------------------------------------------*/
			else if ((regvalue & 1) == 0)  /* ...this is memory space */
			{
				ULONG requested_size;

				/* Type is encoded in bits 1 and 2 */
				/* 64 bit space (10) is an error for the moment, as is */
				/* the reserved value, 11. */
				if (regvalue & 0x4)	
				{
					printf ("Type error in base register.\n");
					break;
				}

				/* Align memspace_ptr for the requested size.  All bit */
				/* positions clear in regvalue must be clear in        */
				/* memspace_ptr. */
				requested_size = ~(regvalue & 0xfffffff0);

#ifdef DEBUG_PCI
				printf ("Configuration for memory space.\n");
				printf ("Size request:  %x; ", requested_size);
				printf ("Membase_ptr:  %x\n", memspace_ptr[root_bus]);
#endif
				
							
				/* check to make sure that there is enough memspace left to grant */
				if ((memspace_ptr[root_bus] + requested_size) > memspace_limit[root_bus])
				{
					printf ("\nPCI Configuration ERROR: Out of Memory Space on Bus %d!\n", bus);					
					printf ("  No Memory Space Allocated to Device %d, Function %d.\n", device, function);					
					printf ("Root Bus = %d, Memory Ptr = 0x%08X, Requested Size = 0x%08X, Memory Limit = 0x%08X\n",					
								 root_bus, memspace_ptr[root_bus], requested_size, memspace_limit[root_bus]);					
	
					return;
				}
					
				if (memspace_ptr[root_bus] & requested_size) 
				{
					memspace_ptr[root_bus] &= ~requested_size;
					memspace_ptr[root_bus] += requested_size + 1;
				}

#ifdef DEBUG_PCI
				printf ("Adjusted Membase_ptr:  %x\n", memspace_ptr[root_bus]);
	    		printf("    Memory Space Size : 0x%08X\n", requested_size);
	    		printf("    Memory Space Base : 0x%08X\n", memspace_ptr[root_bus]);
#endif
				/* Write out the adjusted memory pointer */
				sys_write_config_dword (bus,device,function,regno,(UINT32*)&memspace_ptr[root_bus]);
				memspace_ptr[root_bus] += requested_size + 1;
			}	
		}
		
		/*-------------------------------------------------------------*/
		/* Expansion ROM mapping                                       */
		/* The expansion ROM is handled in the same way as the other   */
		/* PCI base registers.  If the lowest bit in this register is  */
		/* set, memory-mapped accesses are not possible, since the ROM */
		/* is active.  (Only one decoder).                             */
		/*-------------------------------------------------------------*/

		/* Store the state of the ROM enabled bit so we can restore it */
		sys_read_config_dword (bus,device,function,exp_rom_baseaddr,(UINT32*)&regvalue);
		rom_enabled = regvalue & 1;

		/* Write out all 1's to the non-reserved bits of the base register, then read it back   */
		regvalue = 0xffffffff;
		sys_write_config_dword(bus,device,function,EXP_ROM_OFFSET,(UINT32*)&regvalue);
		sys_read_config_dword (bus,device,function,EXP_ROM_OFFSET,(UINT32*)&regvalue);

		/* Some number of the lower bits of regvalue will be clear */
		/* indicating a don't care position.  The more clear bits, */
		/* the larger the requested mapping space. */

		if ((regvalue & 0xffff800) != 0)   /* No mapping if it's 0 */
		{
			ULONG requested_size;

#ifdef DEBUG_PCI
			printf ("Expansion ROM detected.\n");
#endif

			/* Expansion ROMs will map into memory space, so...	   */
			/* Align memspace_ptr for the requested size.  All bit */
			/* positions clear in regvalue must be clear in        */
			/* memspace_ptr. */
			requested_size = ~(regvalue & 0xfffff800);

#ifdef DEBUG_PCI
			printf ("Size request:  %x; ", requested_size);
			printf ("Membase_ptr:  %x\n", memspace_ptr[root_bus]);
#endif

			/* check to make sure that there is enought memspace left to grant */
			if ((memspace_ptr[root_bus] + requested_size) > memspace_limit[root_bus])
			{
				printf ("\nPCI Configuration ERROR: Out of Memory Space on Bus %d!\n", bus);
				printf ("  No Expansion ROM Space Allocated to Device %d, Function %d.\n", device, function);
				return;
			}
					
			if (memspace_ptr[root_bus] & requested_size)	
			{
				memspace_ptr[root_bus] &= ~requested_size;
				memspace_ptr[root_bus] += requested_size + 1;
			}

#ifdef DEBUG_PCI
			printf("Adjusted Membase_ptr:  %x\n", memspace_ptr[root_bus]);
			printf("  Exp. ROM Space Size : 0x%08X\n", requested_size);
			printf("  Exp. ROM Space Base : 0x%08X\n", memspace_ptr[root_bus]);
#endif
			/* Write out the adjusted memory pointer */
			regvalue = memspace_ptr[root_bus] | rom_enabled;
			sys_write_config_dword (bus,device,function,EXP_ROM_OFFSET,(UINT32*)&regvalue);
			memspace_ptr[root_bus] += requested_size + 1;

		}	/* End of expansion ROM mapping. */
                
		/* No expansion ROM to map so disable expansion ROM address decodes */
		else	/* If no ROM space, clear the decode bit */
		{
		  regvalue = 0;
		  sys_write_config_dword (bus,device,function,EXP_ROM_OFFSET,(UINT32*)&regvalue);
		}


		/*-------------------------------------------------------------*/
		/* Miscellaneous settings                                      */
		/*-------------------------------------------------------------*/

		/* Read interrupt pin, write interrupt line according to PCI spec */
		sys_read_config_byte (bus,device,function,INT_PIN_OFFSET,&intpin);
		pci_to_xint(device, intpin, (int*)&line); 
		intline = line;
		sys_write_config_byte (bus,device,function,INT_LINE_OFFSET,&intline);

		/* Set Latency value to relatively and arbitrary small number */
		regchar = LATENCY_VALUE;
		sys_write_config_byte (bus,device,function,LATENCY_TIMER_OFFSET,&regchar);

		if (header_type != PCITOPCI_HEADER)
		{
			/* Set the master enable bit, and enable I/O and Mem spaces */
			/* in the device Command Register only for non-bridge devices */
			sys_read_config_word (bus,device,function,COMMAND_OFFSET,&regshort);
			
			regshort |= PCI_CMD_IOSPACE | PCI_CMD_MEMSPACE | PCI_CMD_BUS_MASTER;
			sys_write_config_word(bus,device,function,COMMAND_OFFSET,&regshort);
			
		}

		/****** Bridge Configuration *******/

		/* To determine whether an installed device is a PCI-to-PCI Bridge
	     device, read the Base Class and the Sub Class from the device's
	     Configuration header.  If the Base Class = 0x06 and the
	     Sub Class = 0x04, the device is a PCI-to-PCI Bridge and requires
	     additional initialization including the initialization of its
	     Secondary PCI bus. */

       if (sys_read_config_dword (bus,device,function,REVISION_OFFSET,(UINT32*)&class_code) != ERROR)
	   {
            /* Parse the class code into Base Class and Sub Class */
		    if ((((class_code >> 16) & 0x0000ffff) == 0x0604) && 
					(header_type == PCITOPCI_HEADER))
		    {
					
				#ifdef DEBUG_PCI
					printf ("PCI-to-PCI Bridge Device.\n");
					printf ("Hit a Key to Continue...\n");
                    hexIn();
				#endif

				if (root_bus == 0)		/* root bus = primary bus */
				{
			    
					if (bus0_lastbus == 0)
						bus0_lastbus = 2;
					else
						bus0_lastbus = bus0_lastbus + 1;

					/* assign new PCI bus number */
					secondary_bus_number = bus0_lastbus;
				}

				else	/* root bus = secondary bus */
				{
					if (bus1_lastbus == 1)
					{
						if (bus0_lastbus != 0)
							bus1_lastbus = bus0_lastbus + 1;
						else
							bus1_lastbus = 2;
					}
					else
						bus1_lastbus = bus1_lastbus + 1;

					/* assign new PCI bus number */
					secondary_bus_number = bus1_lastbus;
				}

			

				#ifdef DEBUG_PCI
					printf ("Secondary Bus Number = %d.\n",secondary_bus_number);
				#endif

				/* set up the bus numbers in the bridge's config. space */

				/* Primary Bus Number */
				data_byte = (UCHAR)root_bus;
				sys_write_config_byte (bus,device,function,PRIMARY_BUSNO_OFFSET,&data_byte);

				/* Secondary Bus Number */
				data_byte = (UCHAR)secondary_bus_number;
				sys_write_config_byte (bus,device,function,SECONDARY_BUSNO_OFFSET,&data_byte);

				/* Set Subordinate Bus Number to a maximum so that config cycles 
				will be passed on to devices located on subordinate buses */
				data_byte = (UCHAR)MAX_SUB_BUSNO;
				sys_write_config_byte (bus,device,function,SUBORD_BUSNO_OFFSET,&data_byte);

				#ifdef DEBUG_PCI
					sys_read_config_byte (bus,device,function,PRIMARY_BUSNO_OFFSET,&data_byte);
					printf ("Primary Bus = %d\n", data_byte);
					sys_read_config_byte (bus,device,function,SECONDARY_BUSNO_OFFSET,&data_byte);
					printf ("Secondary Bus = %d\n", data_byte);
					sys_read_config_byte (bus,device,function,SUBORD_BUSNO_OFFSET,&data_byte);
					printf ("Subordinate Bus = %d\n", data_byte);
				#endif

				/* To figure out the PCI Memory space and PCI I/O space
				windows on the bridge, read the memory and I/O space
				information before the secondary bus is configured
				and then read it afterwards to determine the window
				size for each PCI region

				For secondary busses, the Memory space window of the
				PCI-to-PCI bridge must start and end on a 1 Mbyte boundary

				For secondary busses, the I/O space window of the
				PCI-to-PCI bridge must start and end on a 4 Kbyte boundary
				*/

				/* round up start of PCI memory space to a 1 Mbyte start address */
				if (memspace_ptr[root_bus] & 0xfffff) 
				{
					memspace_ptr[root_bus] &= ~(0xfffff);
					memspace_ptr[root_bus] += 0x100000;
				}
			
				/* round up start of PCI I/O space to a 4 Kbyte start address */
				if (iospace_ptr[root_bus] & 0xfff) 
				{
					iospace_ptr[root_bus] &= ~(0xfff);
					iospace_ptr[root_bus] += 0x1000;
				}

				membase = memspace_ptr[root_bus];
				iobase = iospace_ptr[root_bus];
		
				#ifdef DEBUG_PCI
					printf ("*** About to do a recursive call ***\n");
					printf ("Hit a Key to Continue...\n");
					hexIn();
				#endif
			
				/* initialize the subordinate PCI bus */
				sys_pci_bus_init (secondary_bus_number, root_bus, pci_data);
			
				#ifdef DEBUG_PCI
					printf ("Returned from previous recursive call.\n");
				#endif

				/* Upon return, set the correct Subordinate Bus Number */
				if (root_bus == 0)
					data_byte = (UCHAR) bus0_lastbus;
				else
					data_byte = (UCHAR) bus1_lastbus;

				#ifdef DEBUG_PCI
					printf ("Subordinate Bus Number Now = %d\n", (int)data_byte);
					printf ("Hit a Key to Continue...\n");
					hexIn();
				#endif
				sys_write_config_byte (bus,device,function,SUBORD_BUSNO_OFFSET,&data_byte);

				/* round up end of PCI memory space to a 1 Mbyte start address */
				if (memspace_ptr[root_bus] & 0xfffff) 
				{
					memspace_ptr[root_bus] &= ~(0xfffff);
					memspace_ptr[root_bus] += 0x100000;
				}
			
				/* round up end of PCI I/O space to a 4 Kbyte start address */
				if (iospace_ptr[root_bus] & 0xfff) 
				{
					iospace_ptr[root_bus] &= ~(0xfff);
					iospace_ptr[root_bus] += 0x1000;
				}

				memlimit = memspace_ptr[root_bus] - 1;
				iolimit = iospace_ptr[root_bus] - 1;

				/* Assume bridge has memory request unless told otherwise */
				no_memory = FALSE;
				no_io = FALSE;

				/* if there is no requested memory range, set the membase and 
				memlimit values to zero and set the no_memory range flag */
				if (membase == (memspace_ptr[root_bus]))
				{
					membase = 0;
					memlimit = 0;
					no_memory = TRUE;
				}

				/* if there is no requested IO range, set the iobase and 
				iolimit values to zero and set the no_io range flag */
				if (iobase == (iospace_ptr[root_bus]))
				{
					iobase = 0;
					iolimit = 0;
					no_io = TRUE;
				}

				/* Determine if the Bridge supports 16 or 32 bit I/O space decodes */
				sys_read_config_byte (bus,device,function,IO_BASE_OFFSET,&iospace_type);

				/* 16 Bit I/O space for ISA compatibility
					- fill in only the IO Base and IO Limit registers */
				if (((iospace_type & 0x0f) == 0x00) &&
					(!(iolimit & 0xffff0000)))
				{
					#ifdef DEBUG_PCI
						printf ("Bridge supports 16 Bit I/O Space\n");
					#endif
					/* I/O Base Register */
					data_byte = (UCHAR)((iobase & 0x0000f000) >> 8);
					sys_write_config_byte (bus,device,function,IO_BASE_OFFSET,&data_byte);
					#ifdef DEBUG_PCI
						printf ("I/O Base Register = 0x%02X\n", data_byte);
					#endif

					/* I/O Limit Register */
					data_byte = (UCHAR)((iolimit & 0x0000f000) >> 8);
					sys_write_config_byte (bus,device,function,IO_LIMIT_OFFSET,&data_byte);
					#ifdef DEBUG_PCI
						printf ("I/O Limit Register = 0x%02X\n", data_byte);
					#endif
				}

				/* 32 Bit I/O space
					- fill in the IO Base and IO Limit registers
					- fill in the IO Base Upper and IO Limit Upper registers */
				else if ((iospace_type & 0x0f) == 0x01)
				{
					#ifdef DEBUG_PCI
						printf ("Bridge supports 32 Bit I/O Space\n");
					#endif
			    
					/* I/O Base Register */
					data_byte = (UCHAR)((iobase & 0x0000f000) >> 8);
					sys_write_config_byte (bus,device,function,IO_BASE_OFFSET,&data_byte);
					#ifdef DEBUG_PCI
						printf ("I/O Base Register = 0x%02X\n", data_byte);
					#endif

					/* I/O Limit Register */
					data_byte = (UCHAR)((iolimit & 0x0000f000) >> 8);
					sys_write_config_byte (bus,device,function,IO_LIMIT_OFFSET,&data_byte);
					#ifdef DEBUG_PCI
						printf ("I/O Limit Register = 0x%02X\n", data_byte);
					#endif

					/* I/O Base Upper 16 Bits Register */
					data_word = (USHORT)((iobase & 0xffff0000) >> 16);
					sys_write_config_word (bus,device,function,IO_BASE_UPPER_OFFSET,&data_word);
					#ifdef DEBUG_PCI
						printf ("I/O Base Upper 16 Bits Register = 0x%04X\n", data_word);
					#endif

					/* I/O Limit Upper 16 Bits Register */
					data_word = (USHORT)((iolimit & 0xffff0000) >> 16);
					sys_write_config_word (bus,device,function,IO_LIMIT_UPPER_OFFSET,&data_word);
					#ifdef DEBUG_PCI
						printf ("I/O Limit Upper 16 Bits Register = 0x%04X\n", data_word);
					#endif
				}

				/* Memory Base Register */
				data_word = (USHORT)((membase & 0xfff00000) >> 16);
				sys_write_config_word (bus,device,function,MEMORY_BASE_OFFSET,&data_word);
				#ifdef DEBUG_PCI
					printf ("Memory Base Register = 0x%04X\n", data_word);
				#endif

				/* Memory and Prefetchable Memory Limit Registers */
				data_word = (USHORT)((memlimit & 0xfff00000) >> 16);
				sys_write_config_word (bus,device,function,MEMORY_LIMIT_OFFSET,&data_word);
				sys_write_config_word (bus,device,function,PREF_MEM_LIMIT_OFFSET,&data_word);
				#ifdef DEBUG_PCI
					printf ("Memory Limit Register = 0x%04X\n", data_word);
				#endif

			
				/* Currently does not support prefetchable memory.
					Set the prefetchable memory range to an unused value so that
					a user can not make a prefetchable memory access */

				/* Prefetchable Memory Base Register*/
				data_word = 0xffff;
				sys_write_config_word (bus,device,function,PREF_MEM_BASE_OFFSET,&data_word);
				#ifdef DEBUG_PCI
					printf ("Prefetchable Memory Base Register = 0x%04X\n", data_word);
				#endif


				/* Prefetchable Memory Limit Register*/
				data_word = 0xffff;
				sys_write_config_word (bus,device,function,PREF_MEM_LIMIT_OFFSET,&data_word);
				#ifdef DEBUG_PCI
					printf ("Prefetchable Memory Limit Register = 0x%04X\n", data_word);
				#endif

				/* Set the secondary latency timer on the device.  We use the value 0x0f,*/
				/* which is approximately what you might get if you used the			 */
				/* formula found in the NCR SCSI adapter documentation:					 */
				/* Latency Timer = 2 + (Burst size * (typical wait states + 1))			 */
				regchar = LATENCY_VALUE;
				sys_write_config_byte (bus,device,function,SECONDARY_LAT_OFFSET,&regchar);
		    
				/* Reset the Command Register for PCI to PCI bridges */
				/* (bridges have different settings in command register) */
				sys_read_config_word (bus,device,function,COMMAND_OFFSET,&regshort);
				regshort |= BRIDGE_MASTER_ENAB | BRIDGE_SERR_ENAB;
				/* if memory space is requested, memory transactions should
					be turned on */
				if (!no_memory)
					regshort |= BRIDGE_MEMSPACE_ENAB;
				/* if io space is requested, memory transactions should
					be turned on */
				if (!no_io) 
					regshort |= BRIDGE_IOSPACE_ENAB; 
				sys_write_config_word(bus,device,function,COMMAND_OFFSET,&regshort);
			
				/* Set the Bridge Control Register */
				sys_read_config_word (bus,device,function,BRIDGE_CTRL_OFFSET,&regshort);
				regshort |= BRIDGE_PARITY_ERR | BRIDGE_SEER_ENAB | BRIDGE_MASTER_ABORT;
				sys_write_config_word(bus,device,function,BRIDGE_CTRL_OFFSET,&regshort);
		
			}
		}
	
		#ifdef DEBUG_PCI
				printf ("\nHit a Key to Continue...\n");
                hexIn();
		#endif
		
		/* if not a multifunction device, go on to next device */
		if (multifunction == FALSE)
			break;
	}	/* End for (function...) */
}	/* End for (device...) */

#ifdef DEBUG_PCI
	printf ("Leaving the init function for bus %d.\n", bus);
	printf ("\nHit a Key to Continue...\n");
	hexIn();
#endif
	
return;
}

/***************************************************************************
*
* show_pci - print out Device/Vendor ID of all PCI devices in system
*
*/
void show_pci(void)
{
unsigned char header_type;
unsigned short vendor, device_id;
unsigned int bus, device, function;


    printf (" Bus    Device   Function   Vendor ID   Device ID\n");
    printf ("-----   ------   --------   ---------   ---------\n");

	for (bus = 0; bus < MAX_PCI_BUSES; bus++)
	{
		for (device = 0; device <= MAX_DEVICE_NUMBER; device++)
		{
			for (function = 0; function <= MAX_FUNCTION_NUMBER; function++)
			{
				if ((sys_read_config_word (bus, device, function, VENDOR_ID_OFFSET, &vendor) == ERROR) || (vendor == 0xffff))
            /* This means no device found */
					break;          /* Go on to the next device */
				else
				{
					sys_read_config_word (bus, device, function, DEVICE_ID_OFFSET, &device_id);
					printf (" 0x%02X    0x%02X      0x%02X",bus,device,function);
					printf ("       0x%04X      0x%04X\n", vendor,device_id);
				}

				/* before we go on to the next function, make sure that the
					device is truly a multi-function device, otherwise we may
					get aliasing */
				sys_read_config_byte (bus, device, function, HEADER_TYPE_OFFSET, &header_type);
            
				if (!(header_type & MULTIFUNCTION_DEVICE)) break;
	   
			} /* End for (function) */
		} /* End for (device) */	  
	} /* End for (bus) */
}

extern unsigned hal_dram_size;

/*********************************************************************************
* init_312_pci - Initialize the Primary and Secondary ATUs, Messaging Unit, and 
*                PCI-to-PCI bridge on the 80312 Companion chip
*
*/
void init_312_pci(void)
{
	UINT32	*ATU_reg;
	UINT32	*MU_reg;
	UINT16	*ATU_reg_16;
	UINT8	*ATU_reg_8;
	UINT32	limit_reg;
	UINT32	adj_dram_size;
	UINT16	*BR_reg_16;
	UINT8	*BR_reg_8;

	/*********  vendor / device id **********/
	
	/* set subsytem vendor ID */
	ATU_reg_16 = (UINT16 *) ASVIR_ADDR;
	*ATU_reg_16 = 0x113C;

	/* set subsytem ID = 700 hex for PCI700 */
	ATU_reg_16 = (UINT16 *) ASIR_ADDR;	
	*ATU_reg_16 = 0x0700;
	
	
	/******* Primary Inbound ATU *********/

	/* set primary inbound ATU translate value register to point to base of local DRAM */
	ATU_reg = (UINT32 *) PIATVR_ADDR;
	*ATU_reg = MEMBASE_DRAM & 0xFFFFFFFC;
	
	/* set primary inbound ATU limit register to include all of installed DRAM.
		 This value used as a mask. 																							*/
	ATU_reg = (UINT32 *) PIALR_ADDR;
	adj_dram_size = hal_dram_size;
	limit_reg = (0xFFFFFFFF-(adj_dram_size-1)) & 0xFFFFFFF0;
	*ATU_reg = limit_reg;

	if (isHost() == TRUE)
	{
		/* set the primary inbound ATU base address to the start of DRAM */
		ATU_reg = (UINT32 *) PIABAR_ADDR;
		*ATU_reg = MEMBASE_DRAM & 0xFFFFF000;

		/********* Set Primary Outbound Windows *********/

		/* Note: The primary outbound ATU memory window value register
			 and i/o window value registers are defaulted to 0 */

		/* set the primary outbound windows to directly map Local - PCI requests */
		/* outbound memory window */
		ATU_reg = (UINT32 *) POMWVR_ADDR;
		*ATU_reg = PRIMARY_MEM_BASE;

		/* outbound DAC Window */
		ATU_reg = (UINT32 *) PODWVR_ADDR;
		*ATU_reg = PRIMARY_DAC_BASE;

		/* outbound I/O window */
		ATU_reg = (UINT32 *) POIOWVR_ADDR;
		*ATU_reg = PRIMARY_IO_BASE;	
	}

	/******** Secondary Inbound ATU ***********/

	/* set secondary inbound ATU translate value register to point to base of local DRAM */
	ATU_reg = (UINT32 *) SIATVR_ADDR;
	*ATU_reg = MEMBASE_DRAM & 0xFFFFFFFC;

	/* set secondary inbound ATU base address to start of DRAM */
	ATU_reg = (UINT32 *) SIABAR_ADDR;
	*ATU_reg = MEMBASE_DRAM & 0xFFFFF000;

	/* set secondary inbound ATU limit register to include all of
	   installed DRAM. This value used as a mask.                  */

	/* cyclone merge 1/21/97 */
	/*  always allow secondary pci access to all memory (even with A0 step) */
	limit_reg = (0xFFFFFFFF - (adj_dram_size - 1)) & 0xFFFFFFF0;
	ATU_reg = (UINT32 *) SIALR_ADDR;
	*ATU_reg = limit_reg;


	/********** Set Secondary Outbound Windows ***********/

	/* Note: The secondary outbound ATU memory window value register
		 and i/o window value registers are defaulted to 0 */

	/* set the secondary outbound window to directly map Local - PCI requests */
	/* outbound memory window */
	ATU_reg = (UINT32 *) SOMWVR_ADDR;
	*ATU_reg = SECONDARY_MEM_BASE;

	/* outbound DAC Window */
	ATU_reg = (UINT32 *) SODWVR_ADDR;
	*ATU_reg = SECONDARY_DAC_BASE;

	/* outbound I/O window */
	ATU_reg = (UINT32 *) SOIOWVR_ADDR;
	*ATU_reg = SECONDARY_IO_BASE;

	/***********  command / config / latency registers  ************/

	if (isHost() == TRUE)
	{
		/* allow primary ATU to act as a bus master, respond to PCI 
		   memory accesses, assert P_SERR#, and enable parity checking */
		ATU_reg_16 = (UINT16 *) PATUCMD_ADDR;
		*ATU_reg_16 = (PCI_CMD_SERR_ENAB | PCI_CMD_PARITY | PCI_CMD_BUS_MASTER | PCI_CMD_MEMSPACE);
	}

	/* allow secondary ATU to act as a bus master, respond to PCI memory accesses, and assert S_SERR# */
	ATU_reg_16 = (UINT16 *) SATUCMD_ADDR;
	*ATU_reg_16 = (PCI_CMD_SERR_ENAB | PCI_CMD_PARITY | PCI_CMD_BUS_MASTER | PCI_CMD_MEMSPACE); 

	/* enable primary and secondary outbound ATUs, BIST, and primary bus direct addressing  */
	ATU_reg = (UINT32 *) ATUCR_ADDR;
	*ATU_reg = 0x00000006;  /* no direct addressing window - enable both ATUs */


	/************* bridge registers *******************/
	
	if (isHost() == TRUE)
	{
		/* set the bridge command register */
		BR_reg_16 = (UINT16 *) PCR_ADDR;
		*BR_reg_16 = (PCI_CMD_SERR_ENAB | PCI_CMD_PARITY | PCI_CMD_BUS_MASTER | PCI_CMD_MEMSPACE);	

		/* set the secondary bus number to 1 */
		BR_reg_8 = (UINT8 *) SBNR_ADDR;
		*BR_reg_8 = SECONDARY_BUS_NUM;

		/* set the bridge control register */
		BR_reg_16 = (UINT16 *) BCR_ADDR;
		*BR_reg_16 = 0x0823;

		/* set the primary bus number to 0 */
		BR_reg_8 = (UINT8 *) PBNR_ADDR;
		*BR_reg_8 = PRIMARY_BUS_NUM;
	}

	/* suppress secondary bus idsels to provide */
	/* private secondary devices                 */
	BR_reg_16 = (UINT16 *) SISR_ADDR;
	*BR_reg_16 = 0x03FF;  
}


