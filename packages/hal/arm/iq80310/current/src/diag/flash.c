//=============================================================================
//
//      flash.c - Cyclone Diagnostics
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
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

#include <redboot.h>
#ifdef CYGPKG_IO_FLASH
#include <cyg/io/flash.h>

#include "iq80310.h"	/* 80310 chip set specific */
#include "7_segment_displays.h"

typedef unsigned char FLASH_TYPE;
#define MASK 0xff  /* for 1 bank */

/* 28F016S5/28F640J3A Command Definitions  - First Bus Cycle */
#define RESET_CMD				(0xffffffff & MASK)
#define WRITE_TO_BUF_CMD		(0xe8e8e8e8 & MASK)
#define WRITE_CONFIRM_CMD		(0xd0d0d0d0 & MASK)
#define READ_ID_CMD				(0x90909090 & MASK)
#define READ_STAT_REG			(0x70707070 & MASK)
#define CLEAR_STAT_REG			(0x50505050 & MASK)
#define ERASE_CMD				(0x20202020 & MASK)
#define PROGRAM_CMD				(0x40404040 & MASK)
#define BEP_SUSPEND				(0xb0b0b0b0 & MASK)
#define BEP_RESUME				(0xd0d0d0d0 & MASK)
#define LOCK_CMD				(0x60606060 & MASK)
#define CLEAR_LOCK_BIT_SETUP	(0x60606060 & MASK)	/* 10/06/00 */

/* 28F016S5/28F640J3A Command Definitions  - Second Bus Cycle */
#define ERASE_CONFIRM			(0xd0d0d0d0 & MASK)
#define LOCK_BLOCK_CONFIRM		(0x01010101 & MASK)
#define MASTER_LOCK_CONFIRM		(0xf1f1f1f1 & MASK)  /* DO NOT EVER set master enable bit!!! */
#define UNLOCK_BLOCK_CONFIRM	(0xd0d0d0d0 & MASK)
#define CLEAR_LOCK_BIT_CONFIRM	(0xd0d0d0d0 & MASK)	/* 10/06/00 */

/* Flash category definitions */
#define SECTOR_PROG	0
#define BLOCK_PROG	1

/* status register bits */
#define WSM_READY				(FLASH_TYPE) (1 << 7)
#define WSM_BUSY				(FLASH_TYPE) (0 << 7)
#define BE_SUSPENDED			(FLASH_TYPE) (1 << 6)
#define BE_COMPLETED			(FLASH_TYPE) (0 << 6)
#define ERASE_UNLOCK_ERROR		(FLASH_TYPE) (1 << 5)
#define ERASE_UNLOCK_SUCCESS	(FLASH_TYPE) (0 << 5)
#define CLEAR_LOCK_BIT_ERROR	(FLASH_TYPE) (1 << 5)	/* 10/06/00 */
#define CLEAR_LOCK_BIT_SUCCESS	(FLASH_TYPE) (0 << 5)	/* 10/06/00 */
#define PROGRAM_LOCK_ERROR		(FLASH_TYPE) (1 << 4)
#define PROGRAM_LOCK_SUCCESS	(FLASH_TYPE) (0 << 4)
#define SET_LOCK_BIT_ERROR		(FLASH_TYPE) (1 << 4)	/* 10/17/00 */
#define SET_LOCK_BIT_SUCCESS	(FLASH_TYPE) (0 << 4)	/* 10/17/00 */
#define VPP_LOW_DETECT			(FLASH_TYPE) (1 << 3)
#define VPP_OK					(FLASH_TYPE) (0 << 3)
#define PROGRAM_SUSPENDED		(FLASH_TYPE) (1 << 2)
#define PROGRAM_COMPLETED		(FLASH_TYPE) (0 << 2)
#define DEVICE_PROTECTED		(FLASH_TYPE) (1 << 1)
#define DEVICE_UNLOCKED			(FLASH_TYPE) (0 << 1)


/* Other Intel 28F016S5/28F640J3A definitions */
#define CMD_SEQ_ERR			(FLASH_TYPE) (ERASE_UNLOCK_ERROR | PROGRAM_LOCK_ERROR)
#define ALL_FLASH_STATUS	(FLASH_TYPE) (0xfe)
#define UNKNOWN_ERR			(FLASH_TYPE) (0xff)

#define TEST_BUF_LONGS		16384
#define TEST_BUF_CHARS		65536

#define MADE_BY_INTEL		(0x89898989 & MASK)		/* Manufacturer Code, read at address 0, note that address bit A0 is not used in x8 or x16 mode when obtaining identifier code */

/*#define I28F016S5			(0xAAAAAAAA & MASK)*/	/* 28F016S5 */
#define I28F640J3A			(0x17171717 & MASK)		/* Device Code, read at address 1, note that bit address A0 is not used in x8 or x16 mode when obtaining identifier code */

/*#define FLASH_BLOCK_SIZE	0x10000*/	/* 28F016S5 */
#define FLASH_BLOCK_SIZE	0x20000		/* 28F640J3A */

#define BLOCK_LOCKED		1
#define BLOCK_UNLOCKED		0

// First 4K page of flash at physical address zero is
// virtually mapped at address 0xd0000000.
#define FLASH_P2V(x) ((volatile FLASH_TYPE *)(((unsigned)(x) < 0x1000) ?  \
                           ((unsigned)(x) | 0xd0000000) :  \
                           (unsigned)(x)))


unsigned long *flash_buffer = (unsigned long *)0xa1000000;


extern void _flushICache();
extern void _enableICache();
extern void _disableICache();
extern void _switchMMUpageTables();


extern void _usec_delay();
extern void _msec_delay();

unsigned long eeprom_size;
unsigned long flash_base;

ADDR flash_addr=FLASH_ADDR, eeprom_prog_first, eeprom_prog_last;

extern long hexIn();
extern char * sgets();

/* forward declarations */
void init_eeprom() RAM_FUNC_SECT;
int reserved_check(ADDR addr, unsigned long length) RAM_FUNC_SECT;
int is_eeprom(ADDR addr, unsigned long length) RAM_FUNC_SECT;
int check_eeprom(ADDR addr, unsigned long length) RAM_FUNC_SECT;
int lock_breeze() RAM_FUNC_SECT;
int check_bstat(int block_num) RAM_FUNC_SECT;
int set_all_lock_bits(void) RAM_FUNC_SECT;		/* 10/11/00 added */
int clear_all_lock_bits(ADDR addr) RAM_FUNC_SECT;	/* 10/06/00 added */
int check_erase_unlock(volatile FLASH_TYPE *flash) RAM_FUNC_SECT;
int check_op_status(int cmd, volatile FLASH_TYPE *flash) RAM_FUNC_SECT;
int erase_eeprom(ADDR addr, unsigned long length) RAM_FUNC_SECT;
int check_program_lock(volatile FLASH_TYPE *flash) RAM_FUNC_SECT;
int write_eeprom(ADDR start_addr, const void *data_arg, int data_size) RAM_FUNC_SECT;
void flash_test(void) RAM_FUNC_SECT;
void delay_and_flush(void) RAM_FUNC_SECT;
void do_nothing(void) RAM_FUNC_SECT;
void display_val(int num) RAM_FUNC_SECT;
void display_out (int msb_flag, unsigned char val) RAM_FUNC_SECT;
void check_lock_bit_status(void) RAM_FUNC_SECT;

#define MSB_DISPLAY_REG		(volatile unsigned char *)0xfe840000
#define LSB_DISPLAY_REG		(volatile unsigned char *)0xfe850000

void display_out (int msb_flag, unsigned char val)
{
/*	unsigned char *ledPtr; */
	volatile unsigned char *ledPtr;
	unsigned char  SevSegDecode;

	if (msb_flag) ledPtr = MSB_DISPLAY_REG;
	else          ledPtr = LSB_DISPLAY_REG;

	switch (val)
	{
		case 0:
			SevSegDecode = ZERO;	
			break;

		case 1:
			SevSegDecode = ONE;
			break;

		case 2:
			SevSegDecode = TWO;
			break;

		case 3:
			SevSegDecode = THREE;
			break;

		case 4:
			SevSegDecode = FOUR;
			break;

		case 5:
			SevSegDecode = FIVE;
			break;

		case 6:
			SevSegDecode = SIX;
			break;

		case 7:
			SevSegDecode = SEVEN;
			break;

		case 8:
			SevSegDecode = EIGHT;
			break;

		case 9:
			SevSegDecode = NINE;
			break;
		
		case 10:
			SevSegDecode = LETTER_A;
			break;

		case 11:
			SevSegDecode = LETTER_B;
			break;

		case 12:
			SevSegDecode = LETTER_C;
			break;
		
		case 13:
			SevSegDecode = LETTER_D;
			break;
		
		case 14:
			SevSegDecode = LETTER_E;
			break;

		case 15:
			SevSegDecode = LETTER_F;
			break;

		default:
			SevSegDecode = DECIMAL_POINT;

	}
	*ledPtr = SevSegDecode;
}

void display_val (int number)
{
	unsigned char disp_val = number % 256;
	unsigned char msb, lsb;

	lsb = disp_val & 0x0f;
	msb = (disp_val & 0xf0) >> 4;

	display_out (0, lsb);
	display_out (1, msb);
}

/* Used in write to buffer routine */
void do_nothing (void)
{

	volatile int i;
	for (i = 0; i < 50; i++);		/* Rev 2.0B and Rev 2.0C */
/*	for (i = 0; i < 100; i++); */	/* Rev 2.0A */
}


void delay_and_flush (void)
{

	do_nothing(); 

	_flushICache();

}


/********************************************************/
/* INIT FLASH											*/
/*														*/
/* This routine initializes the variables for timing    */
/* with any board configuration.  This is used to get   */
/* exact timing every time.								*/
/********************************************************/

void init_eeprom()
{
#if 1
	unsigned char MfgCode=MADE_BY_INTEL;
	unsigned char DevCode=I28F640J3A;

	eeprom_size = 0x800000;
#else
	unsigned char MfgCode=0;
	unsigned char DevCode=0;

	flash_addr = FLASH_ADDR;

	flash_base = FLASH_BASE_ADDR;

    /* Set defaults */
    eeprom_size = 0;

    /* Note: the PCI-700 platform has only 1 memory bank */

/*
	*( volatile unsigned char * ) FLASH_BASE_ADDR = RESET_CMD;
	printf( "Wrote 1rst Read Array Command\n");
*/

	*FLASH_P2V(FLASH_BASE_ADDR) = RESET_CMD;	/* issue read array command */	
	delay_and_flush();		  /* wait for Flash to re-enter Read Array mode */

		
	*FLASH_P2V(FLASH_BASE_ADDR) = READ_ID_CMD;	/* issue read id command */	
	
	MfgCode = *FLASH_P2V(FLASH_BASE_ADDR);		/* read a manufacturer code at addr 0, address bit A0 is not used  */
	
	DevCode = *FLASH_P2V(DEV_CODE_ADDR);		/* read a device code at addr 1 */

    if (MfgCode == (MADE_BY_INTEL)) 
    {
		switch ( DevCode )								/* device code stored in addr 1, address bit A0 is not used, must shift 0x00000001<<1=0x00000002 */ 
		{
			case I28F640J3A:
				eeprom_size += 0x800000 * FLASH_WIDTH;		/* I28F640J3A */
				break;
            default:
                break;
        }
	}

	*FLASH_P2V(FLASH_BASE_ADDR) = READ_ID_CMD;	/* issue 2nd read id command */	
	
	*FLASH_P2V(FLASH_BASE_ADDR) = RESET_CMD; /* issue read array command */	
	
	delay_and_flush();	/* wait for Flash to re-enter Read Array mode */
#endif

	printf( "\nManufacturer Code = 0x%x\n", MfgCode);
	printf( "Device Code = %x\n", DevCode);
	printf( "Flash Memory size = 0x%x\n", eeprom_size);

    return;
}

/********************************************************/
/* RESERVED AREA CHECK                         			*/
/*                                   					*/
/* returns TRUE if the address falls into the 			*/
/* 	reserved system area								*/
/* returns FALSE if the address is outside  			*/
/* 	the reserved system area							*/
/********************************************************/
int reserved_check(ADDR addr, unsigned long length)
{

    /* check start address */
    if ( ( addr >= RESERVED_AREA1 ) && ( addr <= ( FLASH_BLK4_BASE_ADDR - 1 ) ) )
		return TRUE;
	

    /* must be outside the area */
	else
		return FALSE;
}

/********************************************************/
/* IS EEPROM											*/
/* Check if memory is Flash								*/
/*														*/
/* returns TRUE if it is; FALSE if not eeprom ;			*/
/* returns ERROR bad addr or partial eeprom				*/
/*														*/
/********************************************************/
int is_eeprom(ADDR addr, unsigned long length)
{
    ADDR eeprom_end = flash_addr + eeprom_size - 1;
    ADDR block_end = addr + length - 1;

    /* Check for wrap: if the address and length given wrap past
     * the end of memory, it is an error. */
    if (block_end < addr)
        return ERR;
    if (addr >= flash_addr && block_end <= eeprom_end)
        return TRUE;
    if (addr > eeprom_end || block_end < flash_addr)
        return FALSE;
    /* If the block was partly within the Flash, it is an error. */
    return ERR;
}


/********************************************************/
/* CHECK EEPROM											*/
/* Check if Flash is Blank								*/
/*														*/
/* returns OK if it is; returns ERROR and sets cmd_stat */
/* to an error code if memory region is not Flash or if */
/* it is not blank.										*/
/*														*/
/********************************************************/
int check_eeprom(ADDR addr, unsigned long length)
{
    
    FLASH_TYPE *p, *end;

	if (eeprom_size == 0)
	{
		cmd_stat = E_NO_FLASH;
		return ERR;
    }

	if (addr == NO_ADDR)
    {
		addr = FLASH_BLK4_BASE_ADDR;	/* start at base address of block */

		length = eeprom_size - RESERVED_AREA_SIZE;

    }
	else if (length == 0)
			length = 1;

/* Original */
/*	if (is_eeprom(addr, length) != 1) */
/*
	if (is_eeprom(addr, length) != TRUE)
    {
		cmd_stat = E_EEPROM_ADDR;
		return ERR;
    }
*/

	p = (FLASH_TYPE *)addr;

/* Original */
/*	end = p + length; */
	/* find first non_blank address */
/*	while (p != end) */
/*	{ */
/*		if (*p != 0xff) */
/*      { */
/*			cmd_stat = E_EEPROM_PROG; */
/*			eeprom_prog_first = (ADDR)p; */

				/* find last non_blank address */
/*			for (p = end; *--p == 0xff; ); */
/*			eeprom_prog_last = (ADDR)p; */
/*			return ERR; */
/*       } */
/*		 p++; */
/*  } */
/*	return OK; */

	end = (FLASH_TYPE *)FLASH_TOP_ADDR; 
	/* search for first non blank address starting at base address of Flash Block 2 */
	while (p != end)
	{
		if (*FLASH_P2V(p) != 0xff)
        {

			eeprom_prog_first = (ADDR)p;	/* found first non blank memory cell */
		
				/* now find last non blank memory cell starting from top of Flash memory */
			for (p = end - 1; *FLASH_P2V(p) == 0xff; --p);

			eeprom_prog_last = (ADDR)p;	/* found last non blank memory cell */ 
			
			cmd_stat = E_EEPROM_PROG;

			return ERR;
        }
		p++;
    }
	return OK;
}

/********************************************************/
/* LOCK BREEZE FLASH AREA                               */
/*                                                      */
/* Lock the Flash ROM blocks which contain the Breeze   */
/* Development environment to prevent inadvertent       */
/* erasure/reprogramming.                               */
/*                                                      */
/* RETURNS:		1 = success, 0 = failure				*/
/********************************************************/
int lock_breeze()
{
    void *err_addr;

	if (flash_lock((void *)BREEZE_BLOCK_0, NUM_BREEZE_BLOCKS*FLASH_BLOCK_SIZE, (void **)&err_addr) != 0) {
		cmd_stat = E_EEPROM_FAIL;
		return (BLOCK_UNLOCKED);
	}
    return(BLOCK_LOCKED);
}


/********************************************************/
/* CHECK BLOCK STATUS                                   */
/*                                                      */
/* Check the lock status of a flash block				*/
/*														*/
/* Input:       block number to check					*/
/*				-1 = master lock						*/			
/*                                                      */
/* Returns:     1 = locked, 0 = unlocked                */
/*              2 = invalid block number                */
/********************************************************/
#if 0
int check_bstat(int block_num)
{
	volatile FLASH_TYPE *lock_data_addr; 
	FLASH_TYPE lock_data;
	
	/* shut the compiler up */
	lock_data_addr = 0x00000000;

	/* derive the address for block lock configuration data */
	if ((block_num >= 0) && (block_num <= NUM_FLASH_BLOCKS))
		lock_data_addr = (FLASH_TYPE*)(FLASH_ADDR + (FLASH_BLOCK_SIZE * block_num) + 2);
	else if (block_num == -1)
		lock_data_addr = (FLASH_TYPE*)(FLASH_ADDR + 3);
	else 
		return (2);
	
	lock_data_addr = FLASH_P2V(lock_data_addr);
	
	/* read block lock configuration data from address */
	*lock_data_addr = READ_ID_CMD;
	
	lock_data = *lock_data_addr;

	/* reset flash to read mode */
	*lock_data_addr = RESET_CMD;
	delay_and_flush();	/* wait for Flash to re-enter Read Array mode */

	/* now check data to see if block is indeed locked */
	if (lock_data & BLOCK_LOCKED)
		return (BLOCK_LOCKED);
	else
		return (BLOCK_UNLOCKED);
}
#endif

/********************************************************/
/* CHECK ERASE OR UNLOCK STATUS							*/
/*														*/
/* Check the status of erase or unlock operation		*/
/* using the Status Register of the Flash				*/
/*														*/
/* Returns: 	OK		- Erase successful				*/
/*		VPP_LOW_DETECT	- Vpp low detected				*/
/*		ERASE_UNLOCK_ERROR - Erase / Unlock error		*/
/*		CMD_SEQ_ERR - Command sequencing error			*/	
/*		UNKNOWN_ERR - Unknown error condition			*/
/*														*/
/********************************************************/
#if 0
int check_erase_unlock(volatile FLASH_TYPE *flash)
{
    FLASH_TYPE stat;
    flash = FLASH_P2V(flash);
    *flash = READ_STAT_REG   ;
    stat = *flash;

    /* poll and wait for Write State Machine Ready */
    while ((stat & WSM_READY) == WSM_BUSY)
    {
        stat = *flash;
    }

    /* now check completion status */
    if (stat & VPP_LOW_DETECT)
    {
		*flash = CLEAR_STAT_REG;
		return VPP_LOW_DETECT;
    }
    if ((stat & CMD_SEQ_ERR) == CMD_SEQ_ERR)
    {
		*flash = CLEAR_STAT_REG;
		return CMD_SEQ_ERR;
    }
    if (stat & ERASE_UNLOCK_ERROR)
    {
		*flash = CLEAR_STAT_REG;
		return ERASE_UNLOCK_ERROR;
    }
    if ((stat & ALL_FLASH_STATUS) == WSM_READY)
    {
		*flash = CLEAR_STAT_REG;
		return OK;
    }
    else
    {
		*flash = CLEAR_STAT_REG;
		return UNKNOWN_ERR;
    }
}
#endif

/********************************************************/
/* CHECK OPERATION STATUS							    */
/*														*/
/* Check the status of an operation		                */
/* using the Status Register of the Flash				*/
/*														*/
/* if the "cmd" argument flag is TRUE, then a           */
/* READ_STAT_REG command should be issued first         */
/*                                                      */
/* Returns:												*/
/* 	OK		                 - Operation successful		*/
/*  value of Status register - Otherwise                */
/*														*/
/********************************************************/
#if 0
int check_op_status(int cmd, volatile FLASH_TYPE *flash)
{
    FLASH_TYPE stat;

	flash = FLASH_P2V(flash);

	if (cmd == TRUE)
	{
		*flash = READ_STAT_REG;
	}

    stat = *flash;

    /* poll and wait for Write State Machine Ready */
    while ((stat & WSM_READY) == WSM_BUSY)
    {
        stat = *flash;
    }

    /* now check completion status */
    if ((stat & ALL_FLASH_STATUS) == WSM_READY)
    {
		*flash = CLEAR_STAT_REG;
		return OK;
    }
    else
    {
		*flash = CLEAR_STAT_REG;
		return stat;
    }
}
#endif

#if 0
/* used for debugging only */
/* check block lock configuration and display status of 64 blocks, 1=locked, 0=unlocked */
void check_lock_bit_status (void)
{

	int block;

	volatile FLASH_TYPE *block_addr;

	/* address bit A0 is not used when obtaining identifier codes */

/* 11/01/00 */
/*	unsigned long addr = 0x2<<1; */
	unsigned long addr = 0x4;

	unsigned char block_lock_status[64];

	block_addr = (volatile FLASH_TYPE *) addr; 

/*	printf("Checking lock status of %d blocks, 1=locked, 0=unlocked...\n", block ); */

		/* address bit A0 is not used when obtaining identifier codes */
	for (block=0; block<=63; block++)
	{

		*FLASH_P2V(block_addr) = READ_ID_CMD; 

		block_lock_status[block] = *FLASH_P2V(block_addr);

		*FLASH_P2V(block_addr) = RESET_CMD;
		
		do_nothing();

/* 11/01/00 */
		do_nothing();

		block_lock_status[block] &= 0x01;	/* Checking lock status of block, 1=locked, 0=unlocked */

		block_addr = (volatile FLASH_TYPE *)((unsigned long)block_addr + (unsigned long)FLASH_BLOCK_SIZE); /* block address offset for byte wide data storage */
	}


	for (block=0; block<=63; block++)
	{
		if (block == 32)
		{
			printf("\n\r");
		}
		printf("%d ", block_lock_status[block] );
	}
	printf("\nDone!\n\n" );

/**	return; **/
}
#endif

/********************************************************/
/* SET ALL LOCK BITS									*/
/*														*/
/* returns OK if successful; otherwise returns ERROR    */
/* and sets cmd_stat to an error code					*/
/* The 28F640J3A is divided into 64, 128Kbyte blocks	*/
/* This routine sets a lock bit in the block specified  */
/* by a given address									*/
/********************************************************/
int set_all_lock_bits()
{
	unsigned long addr = 0x0; 
    void *err_addr;
	int stat;

    if ((stat = flash_lock((void *)addr, 4 * FLASH_BLOCK_SIZE, (void **)&err_addr)) != 0) {
		return stat;
    }
	return( OK );
}


/********************************************************/
/* CLEAR ALL LOCK BITS									*/
/*														*/
/* returns OK if successful; otherwise returns ERROR    */
/* and sets cmd_stat to an error code					*/
/* The 28F640J3A is divided into 64, 128Kbyte blocks	*/
/* This routine clears all block lock bits			 	*/
/********************************************************/
int clear_all_lock_bits(ADDR addr)
{
    void *err_addr;
	int stat;

    if ((stat = flash_unlock((void *)0, eeprom_size, (void **)&err_addr)) != 0)
		return stat;
	return OK;
}


/********************************************************/
/* ERASE EEPROM											*/
/*														*/
/* returns OK if erase was successful,					*/
/* otherwise returns ERROR								*/
/* and sets cmd_stat to an error code					*/
/*														*/
/********************************************************/
int erase_eeprom(ADDR addr, unsigned long length)
{
    void *err_addr;
	int num_blocks;

	/********************************************************/
	/* The 28F640J3A is divided into 64, 128Kbyte blocks	*/
	/* each of which must be individually erased.			*/
	/* This routine and erases a whole number of blocks 	*/
	/********************************************************/

	/* don't erase boot area even if entire eeprom is specified */
	if (addr == NO_ADDR)
	{
		/* 10/06/00 *//* Original */
		/*addr = flash_addr;*/
		addr = FLASH_BLK4_BASE_ADDR;

		length = eeprom_size - RESERVED_AREA_SIZE;
	}

	/* Original */
	/* check for reserved area if one is used */
	/* check to see if the address is within the reserved area */
/*
	if (reserved_check(addr, length) == TRUE)
	{
		cmd_stat = E_EEPROM_ADDR;
		
		return ERR;
	}
*/


	if (length == 0)
	{

		/* 10/06/00 */
		printf( "erase_eeprom, return OK, length=0\n");

		return OK;
	}

	/* start address must be block-aligned */
	if ((addr % FLASH_BLOCK_SIZE) != 0)
	{
		cmd_stat = E_EEPROM_ADDR;

		printf( "erase_eeprom, addr = 0x%x\n", addr);
		printf( "erase_eeprom, FLASH_BLOCK_SIZE = 0x%x\n", FLASH_BLOCK_SIZE);
		printf( "erase_eeprom, return ERR, (addr %% FLASH_BLOCK_SIZE) = %d\n", addr % FLASH_BLOCK_SIZE);

		return ERR;
	}

    /* figure out how many blocks require erasure - round up using integer division */
	if (length % FLASH_BLOCK_SIZE)		/* non-multiple, round up */
		num_blocks = (length + FLASH_BLOCK_SIZE) / FLASH_BLOCK_SIZE;
	else								/* multiple number of blocks */
		num_blocks = length / FLASH_BLOCK_SIZE;

	if (eeprom_size == 0)
	{
		cmd_stat = E_NO_FLASH;
		return ERR;
	}

	/* Original */
	/* If it's already erased, don't bother */
	/*if (check_eeprom(addr, length) == OK)*/
	/*	return OK;*/
	/* check_bstat(int block_num); */

	if (flash_erase((void *)addr, num_blocks * FLASH_BLOCK_SIZE, (void **)&err_addr) != 0) {
		cmd_stat = E_EEPROM_FAIL;
		return ERR;
	}

	return OK;
}


/********************************************************/
/* CHECK PROGRAM OR LOCK STATUS							*/
/*														*/
/* Check the status of program or lock operations		*/
/* using the Status Register of the Flash				*/
/*														*/
/* Returns: 	OK		  - Write successful			*/
/*		VPP_LOW_DETECT	  - Vpp low detected			*/
/*		PROGRAM_LOCK_ERROR  - Write error				*/
/*		UNKNOWN_ERR - Unknown error condition			*/
/*														*/
/********************************************************/
#if 0
int check_program_lock(volatile FLASH_TYPE *flash)
{
    FLASH_TYPE stat;
    flash = FLASH_P2V(flash);
    *flash = READ_STAT_REG;
    stat = *flash;

    /* poll and wait for Write State Machine Ready */
    while (!(stat & WSM_READY))
    	stat = *flash;

    /* now check completion status */
    if (stat & VPP_LOW_DETECT)
    {
		*flash = CLEAR_STAT_REG;
		return VPP_LOW_DETECT;
    }

    if (stat & PROGRAM_LOCK_ERROR)
    {
		*flash = CLEAR_STAT_REG;
		return PROGRAM_LOCK_ERROR;
    }

    if ((stat & ALL_FLASH_STATUS) == (WSM_READY | PROGRAM_LOCK_SUCCESS))
    {
		*flash = CLEAR_STAT_REG;
		return OK;
    }
    else
    {
		*flash = CLEAR_STAT_REG;
		return UNKNOWN_ERR;
    }
}
#endif

/********************************************************/
/* WRITE EEPROM                          				*/
/*                                     					*/
/* returns OK if successful; otherwise returns ERROR    */
/* and sets cmd_stat to an error code				 	*/
/*														*/
/********************************************************/
int
write_eeprom(ADDR start_addr, const void *data_arg, int data_size)
{
    void *err_addr;

	if (flash_program(start_addr, data_arg, data_size, &err_addr) != 0) {
		cmd_stat = E_EEPROM_FAIL;
		return ERR;
	}
	return OK;
}


/*****************************************************************************
*
* flash_test - System Flash ROM diagnostics
*
* A destructive Flash ROM Read/Write test.  Note that the area of Flash
* which is tested changes based on whether the diagnostic is being invoked
* from the System code or from the Factory code (can't write over MON960).
*
* This test basically does a Longword Address test to the Flash area.
*
*/
void flash_test(void)
{

    ADDR start_addr = (ADDR)flash_addr;	/* Original */

    int i;
    unsigned long *f_ptr = (unsigned long *)flash_addr;
    int bytes_written = 0;
    unsigned long flash_data;
    char answer[20];

/* 10/31/00 */
	int status;

#if 0
	printf ("Disabling Instruction Cache... ");
	_disableICache();  /* disable instruction cache */
	printf ("Done\n\n");

	/* switch the MMU over to use RAM-based page table entries */
	printf ("Switching MMU to RAM-based page table... ");
	_switchMMUpageTables();
	printf ("Done\n\n");
#endif


/* 10/31/00 */
/*
	check_lock_bit_status(); 

	printf ("Setting Block Lock Bits... \n");
    printf("Do you wish to continue? (y/n)\n");
    sgets(answer);
    printf("\n");
    if ((answer[0] != 'y') && (answer[0] != 'Y'))
   		return; 
	if( (status = set_all_lock_bits() ) == OK )
        printf("Done!\n");
	else
	{
        printf("Error!\n");
		printf( "error status = 0x%x\n", status );
	}

	check_lock_bit_status(); 

    printf ("\nClearing Block Lock Bits... \n");
    printf("Do you wish to continue? (y/n)\n");
    sgets(answer);
    printf("\n");
    if ((answer[0] != 'y') && (answer[0] != 'Y'))
   		return; 
	if( ( status=clear_all_lock_bits(NO_ADDR) ) == OK )
        printf("Done!\n");
	else
	{
        printf("Error!\n");
		printf( "error status = 0x%x\n", status );
	}

	check_lock_bit_status();
*/
/* 10/31/00 */


	init_eeprom();

    printf("***********************************\n");
    printf("***           WARNING           ***\n");
    printf("*** This test is destructive to ***\n");
    printf("*** all contents of the FLASH!  ***\n");
    printf("***********************************\n");

    printf("\nDo you wish to continue? (y/n)\n");
    sgets(answer);
    printf("\n\n");
    if ((answer[0] != 'y') && (answer[0] != 'Y'))
   		return; 


    printf ("FLASH begins at 0x%X\n", flash_addr);
    printf ("Total FLASH size = 0x%X\n\n", eeprom_size);

    printf ("Checking FLASH ...\n");
    if (check_eeprom(NO_ADDR, 0) == OK)
        printf("FLASH is erased\n\n");
    else
    {
        printf("FLASH is programmed between 0x%X and 0x%X\n\n",
                eeprom_prog_first, eeprom_prog_last);
    }


    printf ("\nClearing Block Lock Bits... \n");
	if(clear_all_lock_bits(NO_ADDR)==OK)
        printf("Done!\n\n");
	else
        printf("Error!\n\n");
	
/*	check_lock_bit_status(); */

    printf ("Erasing FLASH...\n");
    if (erase_eeprom(NO_ADDR, 0) != OK)
        printf("Error on erase_eeprom()\n\n");
    else
        printf("Done Erasing FLASH!\n\n");
 
    (ADDR)flash_addr =  FLASH_BLK4_BASE_ADDR;
	(ADDR)start_addr = (ADDR)flash_addr;

    printf ("Writing Longword Data to FLASH...\n");
 
    /* write to all of available Flash ROM.  Don't do this thousands of times
	   since the Flash has only 100,000 write cycles in its lifespan */

    while (bytes_written < (eeprom_size - RESERVED_AREA_SIZE))
    {
		flash_data = (unsigned long)start_addr;
    	for (i=0; i<TEST_BUF_LONGS; i++)
		{
            flash_buffer[i] = flash_data;	/* put address in buffer */
			flash_data += 4;				/* increment address     */
		}
        if (write_eeprom (start_addr, (void *)flash_buffer,
				 TEST_BUF_CHARS) != OK)
        {
            printf("Error on write_eeprom()\n");
            goto finish;
        }
        start_addr = (unsigned long)start_addr + TEST_BUF_CHARS;
        bytes_written += TEST_BUF_CHARS;
    }

    printf ("Write Complete, Verifying Data...\n");
    bytes_written = 0;
 
   (ADDR)flash_addr = FLASH_BLK4_BASE_ADDR;
    f_ptr = (unsigned long *)flash_addr;
    
	while (bytes_written < (eeprom_size - RESERVED_AREA_SIZE))
    {
        if (*f_ptr != (unsigned long)f_ptr)
        {
            printf ("Data verification error at 0x%X\n",
                        (unsigned long)f_ptr);
            printf ("Expected 0x%X Got 0x%X\n", (unsigned long)f_ptr, *f_ptr);
            goto finish;
        }
        f_ptr++;
        bytes_written += 4;
    }
    printf ("Done Verifying Longword Data!\n\n");


    printf ("Checking FLASH...\n");
    if (check_eeprom(NO_ADDR, 0) == OK)
        printf("FLASH is erased\n\n");
    else
    {
        printf("FLASH is programmed between 0x%X and 0x%X\n\n",
                eeprom_prog_first, eeprom_prog_last);
    }


    printf ("Erasing FLASH...\n");
    if (erase_eeprom(NO_ADDR, 0) != OK)
        printf("Error on erase_eeprom()\n\n");
    else
        printf("Done Erasing FLASH!\n\n");


    printf ("Checking FLASH...\n");
    if (check_eeprom(NO_ADDR, 0) == OK)
        printf("FLASH is erased\n\n");
    else
    {
        printf("FLASH is programmed between 0x%X and 0x%X\n\n",
                eeprom_prog_first, eeprom_prog_last);
    }


    /* reinitialize variables */
    bytes_written = 0;
 
    (ADDR)flash_addr = FLASH_BLK4_BASE_ADDR;

	start_addr = (ADDR)flash_addr;
    f_ptr = (unsigned long *)flash_addr;

    printf ("Writing Inverted Longword Data to FLASH...\n");
 
    /* write to all of available Flash ROM.  Don't do this thousands of times
	   since the Flash has only 100,000 write cycles in its lifespan */

    while (bytes_written < (eeprom_size - RESERVED_AREA_SIZE))
    {
		flash_data = (unsigned long)start_addr;
    	for (i=0; i<TEST_BUF_LONGS; i++)
		{
            flash_buffer[i] = ~flash_data;	/* put address BAR in buffer */
			flash_data += 4;			/* increment address     */
		}
        if (write_eeprom (start_addr, (void *)flash_buffer,
				 TEST_BUF_CHARS) != OK)
        {
            printf("Error on write_eeprom()\n");
            goto finish;
        }
        start_addr = (unsigned long)start_addr + TEST_BUF_CHARS;
        bytes_written += TEST_BUF_CHARS;
    }
 
    printf ("Write Complete, Verifying Data...\n");
    bytes_written = 0;
 
    while (bytes_written < (eeprom_size - RESERVED_AREA_SIZE))
    {
        if (*f_ptr != (~(unsigned long)f_ptr))
        {
            printf ("Data verification error at 0x%X\n",
                        (unsigned long)f_ptr);
            printf ("Expected 0x%X Got 0x%X\n", (~(unsigned long)f_ptr), *f_ptr);
            goto finish;
        }
        f_ptr++;
        bytes_written += 4;
    }
    printf ("Done Verifying Inverted Longword Data!\n\n");


    printf ("Checking FLASH...\n");
    if (check_eeprom(NO_ADDR, 0) == OK)
        printf("FLASH is erased\n\n");
    else
    {
        printf("FLASH is programmed between 0x%X and 0x%X\n\n",
                eeprom_prog_first, eeprom_prog_last);
    }


    printf ("Erasing FLASH...\n");
    if (erase_eeprom(NO_ADDR, 0) != OK)
        printf("Error on erase_eeprom()\n\n");
    else
        printf("Done Erasing FLASH!\n\n");


    printf ("Checking FLASH...\n");
    if (check_eeprom(NO_ADDR, 0) == OK)
        printf("FLASH is erased\n\n");
    else
    {
        printf("FLASH is programmed between 0x%X and 0x%X\n\n",
                eeprom_prog_first, eeprom_prog_last);
    }


/* 11/02/00 */
	printf ("Setting Lock Bits for Blocks 0-3... \n");
	if( (status = set_all_lock_bits() ) == OK )
	{
        printf("Done!\n");
	}
	else
	{
        printf("Error!\n");
		printf( "error status = 0x%x\n", status );
		//		check_lock_bit_status();
	}
/*
	printf ("Setting Lock Bits for Blocks 0-3... \n");
	if(set_all_lock_bits()==OK)
	{
        printf("Done!\n\n");
	}
	else
	{
        printf("Error!\n\n");
		check_lock_bit_status();
	}
*/
/* 11/02/00 */


finish:
	_flushICache();
#if 0
	_enableICache();
#endif

    printf ("\nHit <CR> to Continue...\n");
    (void)hexIn();
    return;

}

#endif // CYGPKG_IO_FLASH
