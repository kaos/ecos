//=============================================================================
//
//      7_segment_displays.h
//
//      Definitions for IQ80310 7-segment display.      
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
// Author(s):   Scott Coulter, Jeff Frazier, Eric Breeden
// Contributors:
// Date:        2001-01-02
// Purpose:     
// Description: 
//
//####DESCRIPTIONEND####
//
//===========================================================================*/


/* Addresses of the 7-segment displays registers */

/* 08/25/00 jwf */
/* iq80303 address decode */
/*
#define MSB_DISPLAY_REG		(volatile unsigned char *)0xe0040000
#define LSB_DISPLAY_REG		(volatile unsigned char *)0xe0050000
*/

/* 08/25/00 jwf */
/* iq80310 address decode */
#define MSB_DISPLAY_REG		(volatile unsigned char *)0xfe840000	/* 7 segment 0 */
#define LSB_DISPLAY_REG		(volatile unsigned char *)0xfe850000	/* 7 segment 1 */


/* Values for the 7-segment displays */
#define DISPLAY_OFF				0xFF
#define ZERO					0xC0
#define ONE						0xF9
#define TWO						0xA4
#define THREE					0xB0
#define FOUR					0x99
#define FIVE					0x92
#define SIX						0x82
#define SEVEN					0xF8
#define EIGHT					0x80
#define NINE					0x90
#define LETTER_A				0x88
#define LETTER_B				0x83
#define LETTER_C				0xC6
#define LETTER_D				0xA1
#define LETTER_E				0x86
#define LETTER_F				0x8E
#define	LETTER_I				0xCF
#define LETTER_L				0xC7
#define LETTER_P				0x8C
#define LETTER_S				0x92
#define DECIMAL_POINT			0x7F
#define DISPLAY_ERROR			0x06  /* Displays "E." */


/* Parameters for functions */
#define	MSB							0
#define	LSB							1
#define	BOTH						2




