//=============================================================================
//
//      i557_eep.h - Cyclone Diagnostics
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

/* Public define's and function prototypes */
#define EEPROM_SIZE			128	/* Maximum # bytes in serial eeprom */
#define EEPROM_WORD_SIZE	64	/* Maximum # shorts in serial eeprom */

/* result codes for the functions below */
#define OK						0	/* Operation completed successfully */
#define EEPROM_ERROR			1	/* generic error */
#define EEPROM_NOT_RESPONDING	2	/* eeprom not resp/not installed */
#define EEPROM_TO_SMALL			3	/* req write/read past end of eeprom */
#define EEPROM_INVALID_CMD      4       /* op code not supported */

/* layout of the Serial EEPROM register */
#define I557_EESK		(1 << 0)
#define I557_EECS		(1 << 1)
#define I557_EEDI		(1 << 2)
#define I557_EEDO		(1 << 3)

/* EEPROM commands */
#define EEPROM_WRITE	1
#define EEPROM_READ		2
#define EEPROM_ERASE	3
#define EEPROM_EWEN     4
#define EEPROM_EWDS		5
#define EEPROM_EWEN_OP  0x30
#define EEPROM_EWDS_OP 	0x00

/* EEPROM Chip Select */
#define SELECT_557_EEP(n)	(*(unsigned char *)(n+0x0e) |= I557_EECS)
#define DESELECT_557_EEP(n)	(*(unsigned char *)(n+0x0e) &= ~I557_EECS)

/* EEPROM Serial Clock */
#define SK_HIGH_557_EEP(n)	(*(unsigned char *)(n+0x0e) |= I557_EESK)
#define SK_LOW_557_EEP(n)	(*(unsigned char *)(n+0x0e) &= ~I557_EESK)

/* EEPROM Serial Data In -> out to eeprom */
#define EEDI_HIGH_557_EEP(n)	(*(unsigned char *)(n+0x0e) |= I557_EEDI)
#define EEDI_LOW_557_EEP(n)	(*(unsigned char *)(n+0x0e) &= ~I557_EEDI)

/* EEPROM Serial Data Out -> in from eeprom */
#define EEDO_557_EEP(n)	((*(unsigned char *)(n+0x0e) & I557_EEDO) >> 3)

/* global functions declared in serial_eep.c */

int eeprom_read (unsigned long pci_addr,
		 int eeprom_addr,	/* word offset from start of eeprom */
		 unsigned short *p_data,/* buffer pointer */
		 int nwords		/* number of bytes to read */
		 );
