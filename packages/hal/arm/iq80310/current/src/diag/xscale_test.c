//=============================================================================
//
//      xscale_test.c - Cyclone Diagnostics
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

/************************************************************************/
/* iq80310_test.c - Main diagnostics for IQ80310 board					*/
/*																		*/
/* Modification History													*/
/* --------------------													*/
/* 11oct00, ejb, Created for IQ80310 StrongARM2							*/
/* 18dec00  jwf                                                         */
/* 02feb01  jwf	added tests: _coy_tight_loop, cache_loop, LoopMemTest,  */
/*              special_mem_test written by snc							*/     
/* 07feb01  jwf added function calls to a variable delay time generator */
/* 09feb01  jwf added function version_info to show version information */
/*              about OS, BOARD, CPLD, 80200 ID, 80312 ID.              */
/************************************************************************/


#include "7_segment_displays.h"
#include "test_menu.h"
#include "iq80310.h"
#include "pci_bios.h"

extern void read_int_status();
extern void _disableDCache();
extern void _enableDCache();
extern void _enableFiqIrq();
extern void _usec_delay();
extern void _msec_delay();
extern void _enable_timer();
extern void _disable_timer();
extern long _read_timer();
extern long _read_cpsr();

/* 02/02/01 jwf */
extern void _coy_tight_loop();

extern long decIn(void);
extern long hexIn(void);
extern void hex32out(unsigned long num);
extern char* sgets(char *s);

extern void flash_test(void) RAM_FUNC_SECT;

extern STATUS pci_isr_connect (int intline, int bus, int device, int (*handler)(int), int arg);
extern STATUS pci_to_xint(int device, int intpin, int *xint);
extern void timer_test (void);

/* 02/07/01 jwf */
extern void time_delay (UINT32 count, volatile int num_tmr_int);

extern int memTest (long startAddr, long endAddr);

/* 02/02/01 jwf */
extern int LoopMemTest (long	startAddr, long	endAddr);

extern void uart_test(void);
extern void pci_ether_test (UINT32 busno, UINT32 devno, UINT32 funcno);
extern void config_ints(void);	/* configure interrupts */
extern void sys_pci_device_initialization (PCI_DATA* pci_data);
extern STATUS sys_find_pci_device (int vendor_id, int device_id, int index, PCI_DEVICE_LOCATION *devloc);
extern int eeprom_write (unsigned long pci_base, int eeprom_addr, unsigned short *p_data, int nwords);
extern ULONG sys_read_config_dword (UINT32 busno,UINT32 devno,UINT32 funcno,UINT32 offset,UINT32 *data);
extern int enable_external_interrupt (int int_id);
extern int disable_external_interrupt (int int_id);
extern int isr_connect(int int_num, void (*handler)(int), int arg);
extern int isr_disconnect(int int_num);
extern void init_external_timer();
extern void uninit_external_timer();
extern int isHost();

void pci_int_test (void);
void hdwr_diag (void);
void rotary_switch (void);
void seven_segment_display (void);
void backplane_detection(void);
void battery_status(void);
/* void timer_test (void); */
void ether_test (void);
void gpio_test (void);

/* 02/02/01 jwf */
void static cache_loop (void);

/* 02/09/01 jwf */
void version_info (void);
void read_coyanosa_id_reg (void);
char board_revision (void);

static void battery_test_menu (void);
static void battery_test_write (void);
static void battery_test_read (void);

/* 01/11/01 jwf */
void select_host_test_system (void);

void internal_timer(void);
static void enet_setup (void);
static void memory_tests (void);
static void repeat_mem_test (void);

/* 02/02/01 jwf */
static void special_mem_test (void);

static void spci_tests (void), ppci_tests (void);
STATUS Device_Seek(
		int verbose,
		unsigned long adapter_vendor,
		unsigned long adapter_device_id,
		unsigned long dev_index,
    	PCI_DEVICE_LOCATION *dev_info
		);

#define VENDOR_INTEL    	0x8086
#define INTEL_NAME		"Intel Corporation Inc."

#define I80303_BRIDGE	0x0309
#define I80303_NAME0	"80303 PCI-PCI Bridge"

#define I80303_ATU		0x5309
#define I80303_NAME1	"80303 Address Translation Unit"

#define I82557			0x1229
#define I82557_NAME		"82557/82558/82559 10/100 LAN Controller"

#define I82559ER		0x1209
#define I82559ER_NAME	"82559ER 10/100 LAN Controller"


/* Test Menu Table */
static MENU_ITEM testMenu[] =
{
	{"Memory Tests",							memory_tests,		  0},
	{"Repeating Memory Tests",					repeat_mem_test,	  0},
	{"16C552 DUART Serial Port Tests", 			uart_test, 			  0},
	{"Rotary Switch S1 Test",					rotary_switch,		  0},
	{"7 Segment LED Tests",						seven_segment_display,0},
	{"Backplane Detection Test",				backplane_detection,  0},
	{"Battery Status Test",						battery_status,       0},
	{"External Timer Test",						timer_test,           0},
#ifdef CYGPKG_IO_FLASH
	{"Flash Test",								flash_test,			  0},
#endif
	{"i82559 Ethernet Configuration",			enet_setup,			  0},
	{"i82559 Ethernet Test",					ether_test,			  0},
	{"i960Rx/303 PCI Interrupt Test",			pci_int_test,		  0},
	{"Internal Timer Test",						internal_timer,		  0},
	{"Secondary PCI Bus Test",					spci_tests,			  0},
	{"Primary PCI Bus Test",					ppci_tests,			  0},
	{"Battery Backup SDRAM Memory Test",		battery_test_menu,	  0},
	{"GPIO Test",								gpio_test,			  0},
/* 02/02/01 jwf */
	{"Repeat-On-Fail Memory Test",				special_mem_test,	  0},
	{"Coyonosa Cache Loop (No return)",			cache_loop,			  0},
/* 02/09/01 jwf */
	{"Show Software and Hardware Revision",		version_info,		  0}
};

#define NUM_MENU_ITEMS	(sizeof (testMenu) / sizeof (testMenu[0]))

#define MENU_TITLE	"\n  IQ80310 Hardware Tests"


PCI_DATA pci_devices = {0, 0};
extern void __reset(void);

void hdwr_diag (void)
{
unsigned char* led0 = (unsigned char*)MSB_DISPLAY_REG;
unsigned char* led1 = (unsigned char*)LSB_DISPLAY_REG;

	*led0 = LETTER_S;
	*led1 = LETTER_S;

	printf ("Entering Hardware Diagnostics - Disabling Data Cache!\n\n");

	__disableDCache();
	
	sys_pci_device_initialization (&pci_devices);

	_enableFiqIrq(); /* enable FIQ and IRQ interrupts */
	 
	config_ints();  /* configure interrupts for diagnostics */

	init_external_timer();

		/* 01/11/01 jwf */
	select_host_test_system();
	
	menu (testMenu, NUM_MENU_ITEMS, MENU_TITLE, MENU_OPT_NONE);

	printf ("Exiting Hardware Diagnostics - Reenabling Data Cache!\n\n");

	// _enableDCache(); /* reenable DCache */

	uninit_external_timer();


	*led0 = ZERO;
	*led1 = ZERO;

 	__reset();  /* reset the board so RedBoot starts with a clean slate */
}

// Use "naked" attribute to suppress C prologue/epilogue
static void __attribute__ ((naked)) _coy_tight_loop(void)
{
    asm ( "0: mov r0,r0\n"
	  "b 0b\n");
}


/* 02/02/01 jwf */
static void cache_loop (void)
{
	printf ("Putting Processor in a Tight Loop Forever...\n\n");

	_coy_tight_loop();

	/* not reached */
}


/************************************************/
/* Secondary PCI Bus Test						*/
/*												*/
/* This test assumes that a IQ80303 eval board  */
/* is installed in a secondary PCI slot. This   */
/* second board must be configured with 32 Meg  */
/* of SDRAM minimum.				            */
/*												*/
/************************************************/
static void spci_tests (void)
{
	long	start_addr;
	long	mem_size;
	long	end_addr;
	int first_ctrlr = 0; 
	PCI_DEVICE_LOCATION dev_loc;

	/* Look for ATU on the secondary PCI Bus */
	printf("\nLooking for a IQ80303 board on the Secondary PCI bus:\n");
	while (TRUE)
	{
		if (Device_Seek (FALSE, VENDOR_INTEL, I80303_ATU, first_ctrlr, &dev_loc) == OK)
		{
			if (dev_loc.bus_number != SECONDARY_BUS_NUM)
			{
				continue;
			}
			else
			{
				printf("An IQ80303 board has been detected on the Secondary PCI bus.");
				break;
			}
		}
		else
		{
			printf("No IQ80303 board detected on the SPCI bus!\n");
			return;
		}
	}

	printf ("i80303 ATU located at bus = 0x%02X, dev = 0x%02X, func = 0x%02X\n",
		dev_loc.bus_number, dev_loc.device_number, dev_loc.function_number);

	/* read the PCI address which corresponds to the start of DRAM */
	if (sys_read_config_dword (dev_loc.bus_number, dev_loc.device_number, dev_loc.function_number, 0x10, (UINT32 *)&start_addr) == ERROR)
	{
		printf ("Error reading PCI BAR for ATU\n");
		return;
	}

	/* strip off indicator bits */
	start_addr &= 0xfffffff0;

	printf ("i80303 DRAM starts at PCI address 0x%08X\n", start_addr);

	/* skip over 1st Mbyte of target DRAM */
	start_addr += 0x100000;

	mem_size = 0x1f00000;
	end_addr = start_addr + mem_size;

	printf("\n\nTesting memory from $");
	hex32out(start_addr);
	printf(" to $");
	hex32out(end_addr);
	printf(".\n");

	memTest(start_addr, end_addr);
	printf("\n");

	printf ("\nMemory test done.\n");
	printf ("Press return to continue.\n");
	(void) hexIn();
}

/************************************************/
/* Primary PCI Bus Test						    */
/*												*/
/* This test assumes that a IQ80303 eval board  */
/* is installed in a primary PCI slot. This     */
/* second board must be configured with 32 Meg  */
/* of SDRAM minimum.				            */
/*												*/
/************************************************/
static void ppci_tests (void)
{
	long	start_addr;
	long	mem_size;
	long	end_addr;
	int first_ctrlr = 0; 
	PCI_DEVICE_LOCATION dev_loc;

	/* check to see if we are the host of the backplane, if not 
	   return an error */
	if (isHost() == FALSE)
	{
		printf ("Invalid test configuration, must be PCI host!\n");
		return;
	}

	/* Look for ATU on the primary PCI Bus */
	printf("\nLooking for a IQ80303 board on the Primary PCI bus:\n");
	while (TRUE)
	{
		if (Device_Seek (FALSE, VENDOR_INTEL, I80303_ATU, first_ctrlr, &dev_loc) == OK)
		{
			if (dev_loc.bus_number != PRIMARY_BUS_NUM)
			{
				continue;
			}
			else
			{
				printf("An IQ80303 board has been detected on the Primary PCI bus\n");
				break;
			}
		}
		else
		{
			printf("No IQ80303 board detected on the PPCI bus!\n");
			return;
		}
	}

	printf ("i80303 ATU located at bus = 0x%02X, dev = 0x%02X, func = 0x%02X\n",
		dev_loc.bus_number, dev_loc.device_number, dev_loc.function_number);

	/* read the PCI address which corresponds to the start of DRAM */
	if (sys_read_config_dword (dev_loc.bus_number, dev_loc.device_number, dev_loc.function_number, 0x10, (UINT32 *)&start_addr) == ERROR)
	{
		printf ("Error reading PCI BAR for ATU\n");
		return;
	}

	/* strip off indicator bits */
	start_addr &= 0xfffffff0;

	printf ("i80303 DRAM starts at PCI address 0x%08X\n", start_addr);

	/* skip over 1st Mbyte of target DRAM */
	start_addr += 0x100000;

	mem_size = 0x1f00000;
	end_addr = start_addr + mem_size;

	printf("\n\nTesting memory from $");
	hex32out(start_addr);
	printf(" to $");
	hex32out(end_addr);
	printf(".\n");

	memTest(start_addr, end_addr);
	printf("\n");

	printf ("\nMemory test done.\n");
	printf ("Press return to continue.\n");
	(void) hexIn();
}


/*****************************************************************************
* memory_tests - Basic Memory Tests                       
*
* Memory tests can be run one of two ways - with the cache turned OFF to test
* physical memory, or with cache turned ON to test the caching
*/
static void memory_tests (void)
{
    long	start_addr;
    long	mem_size;
    long	end_addr;

	printf ("Base address of memory to test (in hex): ");
    start_addr = hexIn();
    printf("\n");
    printf ("Size of memory to test (in hex): ");
    mem_size = hexIn();
    printf("\n");
    end_addr = start_addr + mem_size;

    printf("Testing memory from $");
    hex32out(start_addr);
    printf(" to $");
    hex32out(end_addr);
    printf(".\n");
    memTest(start_addr, end_addr);
    printf("\n");

    printf ("\nMemory test done.\n");
    printf ("Press return to continue.\n");
    (void) xgetchar();
}


/*****************************************************************************
* repeat_mem_test - Repeating Memory Tests                       
*
*/
static void repeat_mem_test (void)
{
    unsigned long start_addr, mem_size, end_addr;
	char	cache_disable[10];

	printf ("Turn off Data Cache? (y/n): ");
	sgets (cache_disable);
	printf ("\n");
    printf ("Base address of memory to test (in hex): ");
    start_addr = hexIn();
    printf("\n");
    printf ("Size of memory to test (in hex): ");
    mem_size = hexIn();
    printf("\n");
    end_addr = start_addr + mem_size;
    printf("Testing memory from $");
    hex32out(start_addr);
    printf(" to $");
    hex32out(end_addr);
    while (memTest (start_addr, end_addr))
        ;
}

/* 02/02/01 jwf */
/*****************************************************************************
* special_mem_test - Repeat-On-Fail Memory Test                     
*
* Memory tests can be run one of two ways - with the cache turned OFF to test
* physical memory, or with cache turned ON to test the caching
*/
static void special_mem_test (void)
{
    long	start_addr;
    long	mem_size;
    long	end_addr;

	printf ("Base address of memory to test (in hex): ");
    start_addr = hexIn();
    printf("\n");
    printf ("Size of memory to test (in hex): ");
    mem_size = hexIn();
    printf("\n");
    end_addr = start_addr + mem_size;

    printf("Testing memory from $");
    hex32out(start_addr);
    printf(" to $");
    hex32out(end_addr);
    printf(".\n");
    LoopMemTest(start_addr, end_addr);
    printf("\n");

    printf ("\nMemory test done.\n");
    printf ("Press return to continue.\n");
    (void) xgetchar();
}

/* sequential test for LSD and MSD 7 segment Leds */
void seven_segment_display (void)
{

	unsigned char SevSegDecode;
	int DisplaySequence;
	int SelectLed;

/* 02/07/01 jwf */
/*	const unsigned long TIME_OUT=6000000;*/
/* 02/02/01 jwf */
/*	unsigned long Dwell;*/
/*	volatile unsigned long Dwell;*/

	*( unsigned char * ) 0xfe840000 = DISPLAY_OFF;		/* blank MSD 7 segment LEDS */

	*( unsigned char * ) 0xfe850000 = DISPLAY_OFF;		/* blank LSD 7 segment LEDS  */


	SelectLed=0; /* initialize 7 segment LED selection */
	
	do
	{
			/* run test data sequence for a 7 segment LED */
		for (DisplaySequence = 0; DisplaySequence <= 17; ++DisplaySequence )
		{
				/* fetch 7 segment decode byte */
			switch( DisplaySequence )
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

				case 16:
					SevSegDecode = DECIMAL_POINT;
					break;

				case 17:
					SevSegDecode = DISPLAY_OFF;
				
				default:
					break;

			} /* end switch( DisplaySequence ) */

		
			/* display test data on selected 7 segment LED */
			/* the test data sequence for a 7 segment led will be seen as:*/
			/* 0 1 2 3 4 5 6 7 8 9 A b C d e F . */
			switch( SelectLed )
			{
				case 0:
					*( unsigned char * ) 0xfe850000 = SevSegDecode;		/* write value on 7 segment LSD LED display */;	
					break;

				case 1:
					*( unsigned char * ) 0xfe840000 = SevSegDecode;		/* write value on 7 segment MSD LED display */;	
					break;

				default:
					break;
			} /* end switch( SelectLed ) */

/* 02/07/01 jwf */
			/* time delay, allows user enough time to read a value on display */
/*			for (Dwell=TIME_OUT; Dwell > 0; --Dwell );*/
			time_delay (0x325aa0, 4);/* Delay 0.4 second. Load counter with a 100ms count down (3300000)d per timer interrupt, 5 timer interrupts */

		} /* end for(DisplaySequence~) */

		++SelectLed;	/* select next 7 segment LED */
	}
	while (SelectLed < 2);	 /* tests a pair of 7 segment LEDs */

	*( unsigned char * ) 0xfe840000 = LETTER_S;	/* show S on the 7 segment MSD LED */
	*( unsigned char * ) 0xfe850000 = LETTER_S;	/* show S on the 7 segment LSD LED */
} /* end seven_segment_display() */


/* 12/18/00 jwf */
/* tests rotary switch status, S1 positions 0-3, a 2 bit output code */
void rotary_switch (void)
{

		/* CYGMON serial port J9 */
	unsigned char recv_data;	/* RHR */
	unsigned char recv_lsr;		/* LSR */

	const unsigned char MAX_SWITCH_SAMPLES = 9;
	unsigned char RotarySwitch[MAX_SWITCH_SAMPLES];	/* holds multiple samples of a 4 bit switch code */
	unsigned char index;									/* index for Rotary Switch array */
	unsigned char debounce;									/* keeps tally of equal rotary switch data reads in a loop */
	unsigned char SevSegDecode;					/* holds decode data for a 7 segment LED display */

	char board_rev;	/* holds a Board Revision number */

/* 02/07/01 jwf */
/*	const unsigned long TIME_OUT = 4000000;*/
/* 02/02/01 jwf */
/*	volatile unsigned int Dwell;*/

	*( unsigned char * ) 0xfe840000 = DISPLAY_OFF;	/* turn off the 7 segment MSD LED */
	*( unsigned char * ) 0xfe850000 = DISPLAY_OFF;	/* turn off the 7 segment LSD LED */

	board_rev= board_revision ();			/* Determine Board Revision Number */
	if (board_rev >= BOARD_REV_E)			/* Board Rev is at E or higher */
	{
		printf("\n\nThe 7-Segment LSD LED shows the Rotary Switch position selected, i.e., 0-F.");
		printf("\n\nSlowly dial the Rotary Switch through each position 0-F and confirm reading.");
	}
	else			/* Unknown Board Revision, might be D or B or A. */
	{
		printf("\n\nThe 7-Segment LSD LED shows the Rotary Switch position selected, i.e., 0-3.");
		printf("\n\nSlowly dial the Rotary Switch through each position 0-3 and confirm reading.");
	}

	printf( "\n\nStrike <CR> to exit this test." );
	while ( recv_data != 0x0d )	/* run until User types a <CR> to exit */
	{
	
		do	/* debounce the switch contacts */
		{
			for(index = 0; index <=	MAX_SWITCH_SAMPLES; index++)		/* sample rotary switch code */
			{
				RotarySwitch[index] = *( unsigned char * ) 0xfe8d0000;	/* read rotary switch code */
				RotarySwitch[index] &= 0x0f;							/* mask out bits b7-b4, preserve bits b0-b3 */
			}
			debounce = 0;
			for(index = 1; index <=	MAX_SWITCH_SAMPLES; index++)		/* test rotary switch code samples */
			{
				if (RotarySwitch[0] == RotarySwitch[index])
					debounce++;											/* keep tally of equal rotary switch code samples */
			}
		}
		while ( debounce < (MAX_SWITCH_SAMPLES - 1) );	/* exit when all rotary switch code readings are equal, when debounce = MAX_SWITCH_SAMPLES-1 */

			/* decipher state of rotary switch position */
		switch( RotarySwitch[0] )
			/* examine rotary switch position then display its position number on the 7 segment LSD LED  */
		{
			case 0x00:
				SevSegDecode = ZERO;	
				break;

			case 0x01:
				SevSegDecode = ONE;
				break;

			case 0x02:
				SevSegDecode = TWO;
				break;

			case 0x03:
				SevSegDecode = THREE;
				break;

			case 0x4:
				SevSegDecode = FOUR;
				break;

			case 0x5:
				SevSegDecode = FIVE;
				break;

			case 0x6:
				SevSegDecode = SIX;
				break;

			case 0x7:
				SevSegDecode = SEVEN;
				break;

			case 0x8:
				SevSegDecode = EIGHT;
				break;

			case 0x9:
				SevSegDecode = NINE;
				break;
			
			case 0xa:
				SevSegDecode = LETTER_A;
				break;

			case 0xb:
				SevSegDecode = LETTER_B;
				break;

			case 0xc:
				SevSegDecode = LETTER_C;
				break;
			
			case 0xd:
				SevSegDecode = LETTER_D;
				break;
			
			case 0xe:
				SevSegDecode = LETTER_E;
				break;

			case 0xf:
				SevSegDecode = LETTER_F;
				break;

			default:
				SevSegDecode = DECIMAL_POINT;
				break;
		}

		*( unsigned char * ) 0xfe850000 = SevSegDecode;			/* display the rotary switch position on the 7 segment LSD LED as: 0, 1, 2, 3 */

		recv_lsr = *(volatile unsigned char *) 0xfe810005;		/* read J9 serial port LSR */
		recv_lsr &= 0x1;
		if ( recv_lsr == 0x1)									/* a character is ready in receiver buffer */
		{
			recv_data = *(volatile unsigned char *) 0xfe810000;	/* read character from J9 serial port receiver buffer */
		}
/* 02/07/01 jwf */
/*		for (Dwell=TIME_OUT; Dwell > 0; --Dwell );*/
		time_delay (0x325aa0, 2);/* Delay 0.2 second. Load counter with a 100ms count down (3300000)d per timer interrupt, 2 timer interrupts */
	}

	*( unsigned char * ) 0xfe840000 = LETTER_S;	/* show S on the 7 segment MSD LED */
	*( unsigned char * ) 0xfe850000 = LETTER_S;	/* show S on the 7 segment LSD LED */

} /* end rotary_switch() */


/* test backplane detection, connector socket J19 pin 7 */
/* BP_DET#=0, no backplane */
/* BP_DET#=1, backplane installed */
/* b0 <--> BP_DET# */
void backplane_detection(void)
{
	unsigned char BpDetStatus;	/* L = pci700 board installed on backplane */

	BpDetStatus = *( unsigned char * ) 0xfe870000;	/* read backplane detection status port */

	BpDetStatus &= 0x01;		/* isolate bit b0 */

			/* examine bit 0 */
	switch( BpDetStatus )

	{
		case 0x00:		/* BpDetStatus = !(BP_DET#=1) = 0 */
			printf("\nBackplane detection bit read Low, no backplane installed\n");
			printf("\nPlace a jumper across J19.7 to J19.1, then run this test again.\n");
			break;

		case 0x01:		/* BpDetStatus = !(BP_DET#=0) = 1 */
			printf("\nBackplane detection bit read High, 1 backplane detected.\n");
			printf("\nRemove jumper from J19\n");
			break;

		default:
			break;
	}

/* 12/18/00 jwf */
	printf ("\n\nStrike <CR> to exit this test.\n\n");
	hexIn();

}


/* test battery status */
/* b0 - !(BATT_PRES#=0). A battery is installed.*/
/* b1 - BATT_CHRG=1. The battery is fully charged. */
/* b2 - BATT_DISCHRG=1. The battery is fully discharged. */
void battery_status(void)
{
	unsigned char BatteryStatus;

	unsigned char TestBit;

	BatteryStatus = *( unsigned char * ) 0xfe8f0000;	/* read battery status port */

	BatteryStatus &= 0x07;	/* isolate bits b0, b1, and b2 */

	TestBit = BatteryStatus;

	/* examine bit b0 BATT_PRES# */

	TestBit &= 0x01;

	if (TestBit == 0x01)	/* TestBit=!(BATT_PRES#=0)=1 */
	{

		printf("\nBATT_PRES#=0. A battery was detected.\n");
	}

	else	/* TestBit=!(BATT_PRES#=1)=0 */
	{
	
		printf("\nBATT_PRES#=1. No battery installed.\n");	/* skip testing bits b2 and b3 (BATT_CHRG and BATT_DISCHRG) here since no battery is installed yet */
	}

		/* examine bit b1 BATT_CHRG */
	TestBit |= BatteryStatus;
	
	TestBit &= 0x02;
	
	if (TestBit == 0x02)	/* BATT_CHRG=1 */ /* Assume V_BATT float=4.2V, then 1.2V<V(U20.5)<=1.33V so V_BATT>3.78V,*/
			
		printf("\nBATT_CHRG=1. Battery is fully charged.\n");

	else					/* BATT_CHRG=0 */ /* Assume V_BATT float=4.2V, then V(U20.5)<=1.2V so V_BATT<=3.78V */

		printf("\nBATT_CHRG=0. Battery is charging.\n");


		/* examine bit b2 BATT_DISCHRG */
	TestBit |= BatteryStatus;
	
	TestBit &= 0x04;
	

	if (TestBit == 0x04)		/* BATT_DISCHRG=1 */ /* Assume V_BATT float=4.2V, then V(U30.2)=<1.2V so V_BATT<=3.0V */
			
		printf("\nBATT_DISCHRG=1. Battery is fully discharged.\n");

	else						/* BATT_DISCHRG=0 */ /*  Assume V_BATT float=4.2V, then 1.2V<V(U30.2)=<1.68V so V_BATT>3.0V */ 

		printf("\nBATT_DISCHRG=0. Battery voltage measures with in normal operating range.\n");

	printf ("\n\nStrike <CR> to exit this test.\n\n");
	
	hexIn();

}






/* GPIO test */
/* Header J16 pin out is: J16.1=b0, J16.3=b1, J16.5=b2, J16.7=b3, J16.9=b4, J16.11=b5, J16.13=b6, J16.15=b7 */
/* This test will require use of 2 special test sockets wired as follows for the output and input tests. */
/* Intel specifies that each GPIO pin must be pulled down after P_RST# deasserts to swamp out their weak internal active pull up */
/* Note that the internal weak active pull up tends to have more of an affect on the GPIO input port rather than the output port */
/* Therefore for the input test, jumper J16 pins: 1-2, 3-4, 5-6, 7-8, 9-10, 11-12, 13-14, 15-16, and (TBD) provide an input source for each bit */
/* For the output test, jumper J16 pins: 1-2, 3-4, 5-6, 7-8, 9-10, 11-12, 13-14, 15-16 */
/* each jumpered pin connects a weak pull down resistor, resident on board, to each GPIO pin */
void gpio_test (void)
{
	/*unsigned char GpioInputPort;*/
	unsigned char GpioOutputPort;
	unsigned char GpioOutputEnablePort;

	/* GPIO output port test */

	printf("\n\nPlug output test socket into header J16, strike 'Enter' to continue" );
	while(xgetchar()!=0x0d);

	/* write test data pattern to GPIO Output Enable Register at address 0x0000171c */
	*( unsigned char * ) 0x0000171c = 0x55;

	/* read GPIO Output Enable Register from address 0x0000171c */
	GpioOutputEnablePort = *( unsigned char * ) 0x0000171c;

	if (GpioOutputEnablePort==0x55)
		printf("\nGPIO Output Enable first write/read test PASSED.");
	else
		printf("\nGPIO Output Enable first write/read test FAILED.");

/*
	printf("\n\nStrike Enter to continue" );
	printf("\n0x55" );
	while(xgetchar()!=0x0d);
*/
	
	/* write test data pattern to GPIO Output Enable Register at address 0x0000171c */
	*( unsigned char * ) 0x0000171c = 0xaa;

	/* read GPIO Output Enable Register from address 0x0000171c */
	GpioOutputEnablePort = *( unsigned char * ) 0x0000171c;

	if (GpioOutputEnablePort==0xaa)
		printf("\nGPIO Output Enable second write/read test PASSED.");
	else
		printf("\nGPIO Output Enable second write/read test FAILED.");


	
	/* enable output bits b0-b7, write test pattern to GPIO Output Enable Register at address 0x0000171c */
	*( unsigned char * ) 0x0000171c = 0x00;
	
	/* write test data pattern to GPIO Output Data Register at address 00001724h */
	*( unsigned char * ) 0x00001724 = 0x55;
	
	/* read test data pattern from GPIO Output Data Register at address 00001724h */
	GpioOutputPort = *( unsigned char * ) 0x00001724;

	if (GpioOutputPort==0x55)
		printf("\nGPIO Output Data Register first write/read test PASSED.");
	else
		printf("\nGPIO Output Data Register first write/read test FAILED.");

/*
	printf("\n\nStrike Enter to continue" );
	printf("\n0x55" );
	while(xgetchar()!=0x0d);
*/
	
	/* write output data pattern to GPIO Output Data Register at address 00001724h */
	*( unsigned char * ) 0x00001724 = 0xaa;
	
	/* read output data pattern from GPIO Output Data Register at address 00001724h */
	GpioOutputPort = *( unsigned char * ) 0x00001724;

	if (GpioOutputPort==0xaa)
		printf("\nGPIO Output Data Register second write/read test PASSED.");
	else
		printf("\nGPIO Output Data Register second write/read test FAILED.");
	
	
	printf("\n\nRemove output test socket from header J16, strike 'Enter' to continue" );
	while(xgetchar()!=0x0d);



	/* GPIO input port test */
/*
	printf("\n\nPlug input test socket into header J16, strike 'Enter' to continue" );
	while(xgetchar()!=0x0d);
*/

	/* GPIO Input Data Register address is 00001720h */	/* read port */
/*
	GpioInputPort = *( unsigned char * ) 0x00001720;
	if ( GpioInputPort==0x55 )
		printf("\nGPIO Input Data Register first read test PASSED");
	else
		printf("\nGPIO Input Data Register first read test FAILED");
*/

	
	/* GPIO Input Data Register address is 00001720h */	/* read port */
/*
	GpioInputPort = *( unsigned char * ) 0x00001720;

	if ( GpioInputPort==0xaa )
		printf("\nGPIO Input Data Register second read test PASSED");
	else
		printf("\nGPIO Input Data Register second read test FAILED");
*/	
/*
	printf("\n\nRemove input test socket from header J16, strike 'Enter' to continue" );
	while(xgetchar()!=0x0d);
*/

} /* end gpio_test() */



/*************************************************************************
* Device_Seek - look for a PCI device
*
* During initialization, a device driver must call this function
* (or a similar one) with the specific PCI Vendor Id and Device Id
* of the device to be supported to determine its location (or lack
* thereof) on the PCI bus.  If multiple devices are to be supported,
* this function must be called repeatedly with an increasing dev_index
* until the function returns ERROR.  Each instance (if any) of the device
* will then have been identified.
*
* Once the device is located, the device driver can then call the 
* appropriate PCI BIOS function to read device information from PCI
* Configuration Space (i.e. Runtime Register PCI Base address, Local
* Memory PCI Base address, etc.)
*
*/
STATUS Device_Seek(
		int verbose,
		unsigned long adapter_vendor,
		unsigned long adapter_device_id,
		unsigned long dev_index,
    	PCI_DEVICE_LOCATION *dev_info
		)
{
	if (verbose)
	{
		printf("Looking for Adapter on PCI Bus with:\n");
		printf("Vendor Id = 0x%04x\n",adapter_vendor);
		printf("Device Id = 0x%04x\n",adapter_device_id);
		printf("Index     = %x\n",dev_index);
	}

    if (sys_find_pci_device (adapter_vendor,
			     adapter_device_id,
			     dev_index, dev_info) != OK)
    {
		if (verbose) printf("Failed to Find Adapter\n");
		return (ERROR);
    }
    else
    {
		if (verbose)
		{
			printf("Adapter found at :\n");
			printf("  PCI Bus Number : %d\n", dev_info->bus_number);
			printf("  Device Number  : %d\n", dev_info->device_number);
		}
		return (OK);
    }
}


/* i82559 Ethernet test */
void ether_test (void)
{

    PCI_DEVICE_LOCATION dev_loc[6];  /* 6 is the max Enet for now */
	int	unit = 0;
	int i, num_enet;

	
	for (i = 0, num_enet = 0; i < 6; i++, num_enet++)
	{
		if (Device_Seek (FALSE, VENDOR_INTEL,
			     I82557,
				 i,
				 &(dev_loc[num_enet])) != OK)
		{
			break;
		}

	}

	for (i = 0; i < 6; i++, num_enet++)
	{
		if (Device_Seek (FALSE, VENDOR_INTEL,
			     I82559ER,
				 i,
				 &(dev_loc[num_enet])) != OK)
		{
			break;
		}

	}
	
	if (num_enet == 0)
	{
		printf ("No supported Ethernet devices found\n");
		return;
	}
	
	printf ("Supported Ethernet Devices:\n\n");

	printf (" Unit#  Bus#  Device#\n");
	printf (" -----  ----  -------\n");
	for (i = 0; i < num_enet; i++)
	{
		printf ("   %d     %d       %d\n", i, dev_loc[i].bus_number, dev_loc[i].device_number); 
	}

	printf ("\nEnter the unit number to test : ");
	unit = decIn();
	printf ("\n");

	pci_ether_test (dev_loc[unit].bus_number,
					dev_loc[unit].device_number,
					dev_loc[unit].function_number);

}




/* Setup Serial EEPROM for Ethernet Configuration */
static void enet_setup (void)
{
	UINT32 adapter_ptr;   /* Ptr to PCI Ethernet adapter */

	PCI_DEVICE_LOCATION dev_loc;
	UINT16 eepromData[3] = 
	{
		0x4801,		/* Valid EEPROM, No Expansion ROM, Rev = ?, PHY Addr = 1 */
		0x0700,		/* Subsystem Id			- PCI700 */
		0x113c		/* Subsystem Vendor Id	- Cyclone Microsystems */
	};
	int config_data_offset = 0x0a;	/* offset into EEPROM for config. data storage */
	int ia_offset = 0x00;			/* offset into EEPROM for IA storage */
	UINT8  buffer[6];				/* temporary storage for IA */
    UINT16 temp_node_addr[3] = {0,0,0};
    UINT16 serial_no;
    UINT8  revision_id = 0, port_id = 0;
	char rev_string[8];

    /* Cyclone identifier */
    buffer[0] = 0x00;
    buffer[1] = 0x80;
    buffer[2] = 0x4D;
	buffer[3] = 0x46;    /* board identifier - PCI700 = 70 = 0x46 */

	serial_no = 10000;
	while (serial_no >= 10000)
	{
		printf ("\nEnter the board serial number (1 - 9999): ");
		serial_no = decIn();
		printf ("\n");
	}
	revision_id = 8;
	while ((revision_id < 1) || (revision_id > 7))
	{
		printf ("\nEnter the board revison (A - G)      : ");
		sgets (rev_string);
		rev_string[0] = (rev_string[0] & 0xdf);		/* convert to upper case */
		revision_id = (rev_string[0] - 'A') + 1;	/* convert to a number 1 - 7 */
		printf ("\n");
	}
	revision_id &= 0x7;
	eepromData[0] |= (revision_id << 8);	/* add the rev. id to data */
	
	/* we only want to set up on-board 559 */
	dev_loc.bus_number = 2;
	dev_loc.device_number = 0;
	dev_loc.function_number = 0;
	
	/* Get the PCI Base Address for mem. runtime registers */
	if (sys_read_config_dword (dev_loc.bus_number,
						    dev_loc.device_number,
	   					    dev_loc.function_number,
							0x10, &adapter_ptr) == ERROR)
	{
		printf("Unable to read i82559 PCI Base Address\n");
		return;
	}

	/* strip off indicator bits */
	adapter_ptr = adapter_ptr & 0xfffffff0;

	printf ("Writing the Configuration Data to the Serial EEPROM... ");
	if (eeprom_write (adapter_ptr,config_data_offset,eepromData,3) != OK)
	{
		printf ("Error writing the Configuration Data to Serial EEPROM\n");
		return;
	}
	printf ("Done\n");

	/* setup node's Ethernet address */
	port_id = ((0 << 6) & 0xc0);	/* two bits of port number ID */
	buffer[4] = (UINT8) (((serial_no & 0x3FFF) >> 8) | port_id);
	buffer[5] = (UINT8) (serial_no & 0x00FF); 
		
	temp_node_addr[0] = (UINT16) ((buffer[1] << 8) + buffer[0]);
	temp_node_addr[1] = (UINT16) ((buffer[3] << 8) + buffer[2]);
	temp_node_addr[2] = (UINT16) ((buffer[5] << 8) + buffer[4]);

	printf ("Writing the Individual Address to the Serial EEPROM... ");
	if (eeprom_write (adapter_ptr,ia_offset,temp_node_addr,3) != OK)
	{
		printf ("\nError writing the IA address to Serial EEPROM.\n");
		return;
	}
	printf ("Done\n"); 
	

	/* now that we have finished writing the configuration data, we must ask the
	   operator to reset the PCI916 to have the configuration changes take effect.
	   After the reset, the standard Enet. port diagnostics can be run on the 916
	   under test */

	printf ("\n\n******** Reset the IQ80310 Now to Have Changes Take Effect ********\n\n");

	/* wait forever as a reset will bring us back */
	while ((volatile int)TRUE)
		;
}



/* use the clock in the Performance Monitoring Unit to do delays */
void polled_delay (int usec)
{
volatile int i;
	
	_enable_timer();

	for (i = 0; i < usec; i++)
		_usec_delay();

	_disable_timer();
}



void internal_timer()
{
	int j, i;

	printf ("\n");

	_enable_timer();

	printf ("Timer enabled...\n");

	for (j = 0; j < 20; j++)
	{		
		printf (".");
		for (i = 0; i < 1000; i++)
			_msec_delay();
	}

	_disable_timer();

	printf ("\nTimer disabled...\n");
}






#define I80960RP_BRIDGE		0x0960
#define I80960RP_NAME0		"80960RP PCI-PCI Bridge"

#define I80960RP_ATU		0x1960
#define I80960RP_NAME1		"80960RP Address Translation Unit"

#define I80960RM_BRIDGE		0x0962
#define I80960RM_NAME0		"80960RM PCI-PCI Bridge"

#define I80960RM_ATU		0x1962
#define I80960RM_NAME1		"80960RM Address Translation Unit"

#define I80960RN_BRIDGE		0x0964
#define I80960RN_NAME0		"80960RN PCI-PCI Bridge"

#define I80960RN_ATU		0x1964
#define I80960RN_NAME1		"80960RN Address Translation Unit"

#define I80303_BRIDGE		0x0309
#define I80303_NAME0		"80303 PCI-PCI Bridge"

#define I80303_ATU			0x5309
#define I80303_NAME1		"80303 Address Translation Unit"



/******************************************************************/
/* The following functions are all part of the PCI Interrupt Test */
/******************************************************************/
/* definitions pertaining to PCI interrupt test */
#define MAX_I960RX 31
typedef struct 
{
	UINT32 device_id;
	UINT32 busno;
	UINT32 devno;
	UINT32 funcno;
} I960RX_DEVICES;
I960RX_DEVICES i960Rx_devices[MAX_I960RX];
UINT32 num_rx_devices = 0; 
UINT32	messagingUnitBase = (UINT32)NULL;

/* Outbound Interrupt Status Register bits */
#define OB_STAT_INTA	(1 << 4)
#define OB_STAT_INTB	(1 << 5)
#define OB_STAT_INTC	(1 << 6)
#define OB_STAT_INTD	(1 << 7)

/* Outbound Doorbell Register bits */
#define OB_DBELL_INTA	(1 << 28)
#define OB_DBELL_INTB	(1 << 29)
#define OB_DBELL_INTC	(1 << 30)
#define OB_DBELL_INTD	(1 << 31)

/***********************************************************************
*
* line_to_string - Returns name as string of particular XINT number
*
*/
static char *line_to_string (int intline)
{
	switch (intline)
	{
		case XINT0: return("XINT0");
		case XINT1: return("XINT1");
		case XINT2: return("XINT2");
		case XINT3: return("XINT3");
		default:	return("ERROR");
	}
}


/***************************************************************************
*
* PCI_IntHandler - Interrupt handler for PCI interrupt test
*
* Used to verify that an interrupt was recieved during the PCI interrupt 
* test by the IQ80310 from the add-in i960Rx board.  This handler prints out
* which interrupt was recieved, and then clears the interrupt by clearing 
* the doorbell register on the i960Rx card.
*
*/
int PCI_IntHandler (int IntPin)
{
	UINT32 *OutboundDbReg	 = (UINT32 *)(messagingUnitBase + 0x2c);
	UINT32 *OutboundIstatReg = (UINT32 *)(messagingUnitBase + 0x30);

	switch (IntPin)
	{
	case INTA:

		/* check to see if we are looking at the correct interrupt */
		if (!(*OutboundIstatReg & OB_STAT_INTA))
		{		
		#if 0
			printf ("OISR Mismatch!\n");
			printf ("OISR = 0x%X\n", *OutboundIstatReg);
			printf ("**** PCI INTA Error ****\n\n");

			/* try to clear all sources */
			*OutboundDbReg = (OB_DBELL_INTA|OB_DBELL_INTB|OB_DBELL_INTC|OB_DBELL_INTD);
		#endif
			return (0);
		}
		else
		{			
			printf ("PCI INTA generated/received\n\n");
			printf ("OISR OK!\n");
			printf ("OISR = 0x%X\n", *OutboundIstatReg);
			printf ("**** PCI INTA Success ****\n\n");
			*OutboundDbReg |= OB_DBELL_INTA;		/* try to clear specific source */
			return (1);
		}

		break;

	case INTB:

		/* check to see if we are looking at the correct interrupt */
		if (!(*OutboundIstatReg & OB_STAT_INTB))
		{
		#if 0
			printf ("OISR Mismatch!\n");
			printf ("OISR = 0x%X\n", *OutboundIstatReg);
			printf ("**** PCI INTB Error ****\n\n");

			/* try to clear all sources */
			*OutboundDbReg = (OB_DBELL_INTA|OB_DBELL_INTB|OB_DBELL_INTC|OB_DBELL_INTD);
		#endif
			return (0);
		}
		else
		{
			printf ("PCI INTB generated/received\n\n");
			printf ("OISR OK!\n");
			printf ("OISR = 0x%X\n", *OutboundIstatReg);
			printf ("**** PCI INTB Success ****\n\n");
			*OutboundDbReg |= OB_DBELL_INTB;		/* try to clear specific source */
			return (1);
		}

		break;

	case INTC:

		/* check to see if we are looking at the correct interrupt */
		if (!(*OutboundIstatReg & OB_STAT_INTC))
		{
		#if 0	
			printf ("OISR Mismatch!\n");
			printf ("OISR = 0x%X\n", *OutboundIstatReg);
			printf ("**** PCI INTC Error ****\n\n");
	
			/* try to clear all sources */
			*OutboundDbReg = (OB_DBELL_INTA|OB_DBELL_INTB|OB_DBELL_INTC|OB_DBELL_INTD);
		#endif
			return (0);
		}
		else
		{
			printf ("PCI INTC generated/received\n\n");
			printf ("OISR OK!\n");
			printf ("OISR = 0x%X\n", *OutboundIstatReg);
			printf ("**** PCI INTC Success ****\n\n");
			*OutboundDbReg |= OB_DBELL_INTC;		/* try to clear specific source */
			return (1);
		}

		break;

	case INTD:

		/* check to see if we are looking at the correct interrupt */
		if (!(*OutboundIstatReg & OB_STAT_INTD))
		{
		#if 0
			printf ("OISR Mismatch!\n");
			printf ("OISR = 0x%X\n", *OutboundIstatReg);
			printf ("**** PCI INTD Error ****\n\n");

			/* try to clear all sources */
			*OutboundDbReg = (OB_DBELL_INTA|OB_DBELL_INTB|OB_DBELL_INTC|OB_DBELL_INTD);
		#endif
			return (0);
		}
		else
		{
			printf ("PCI INTD generated/received\n\n");
			printf ("OISR OK!\n");
			printf ("OISR = 0x%X\n", *OutboundIstatReg);
			printf ("**** PCI INTD Success ****\n\n");
			*OutboundDbReg |= OB_DBELL_INTD;		/* try to clear specific source */
			return (1);
		}

		break;

	default:
		printf ("Unknown interrupt received\n");
		return (0);
		break;
	}
	return (1);		/* interrupt sharing support requirement */
}


/*******************************************************************************
*
* i960Rx_seek - look for i960Rx CPUs on the PCI Bus
*
* This function is used by the PCI interrupt test to find and print out
* all PCI location of a particular i960Rx processor on the PCI bus.  Thses include
* boards based on the RP, RD, RM, and RN processors.  The device ID of one of 
* these processors is the input to the function.
*
*/
static void i960Rx_seek (UINT32 adapter_device_id)
{
int dev_index = 0;
PCI_DEVICE_LOCATION dev_info;
static char *i960Rx_name;

	/* get the common name for the current Rx processor */
	switch (adapter_device_id)
	{
	case I80960RN_ATU:
		i960Rx_name = " i960RN  ";
		break;
	case I80960RM_ATU:
		i960Rx_name = " i960RM  ";
		break;
	case I80960RP_ATU:
		i960Rx_name = "i960RP/RD";
		break;
	case I80303_ATU:
	default:
		i960Rx_name = " i80303  ";
		break;
	} 

	while (Device_Seek (FALSE, VENDOR_INTEL,
							 adapter_device_id,
							 dev_index, &dev_info) == OK)
	{
		/* set up this entry into the device array */
		i960Rx_devices[num_rx_devices].device_id = adapter_device_id;
		i960Rx_devices[num_rx_devices].busno = dev_info.bus_number;
		i960Rx_devices[num_rx_devices].devno = dev_info.device_number;
		i960Rx_devices[num_rx_devices].funcno = dev_info.function_number;


		printf ("   %d        %s  %d       %d         %d\n",  
							num_rx_devices, i960Rx_name,
							i960Rx_devices[num_rx_devices].busno,
							i960Rx_devices[num_rx_devices].devno,
							i960Rx_devices[num_rx_devices].funcno);
	
		dev_index++;		/* increment number of current i960Rx devices found */
		num_rx_devices++;	/* increment total number of i960Rx devices found */
	}
}


/*********************************************************************************
*
* pci_int_test - tests PCI interrupts on IQ80310 PCI buses
*
* This test allows full testing of PCI interrupt routing to a particular
* slot on the PCI bus.  It runs in conjunction with a Cyclone i960Rx-based
* board plugged into the target slot.  A default interrupt handler is connected
* for all four PCI interrupt pins on the target board.  The test then waits for 
* the target board to trigger each interrupt using the i960Rx doorbell registers.
* The test passes if all four interrupts are recieved and properly handled by the
* IQ80310.
*
*/
void pci_int_test (void)
{
	PCI_DEVICE_LOCATION devloc;
	int indexChoice = 0;
	UINT32 long_data;
	UINT32 *OutboundImaskReg;
    int intline_INTA, intline_INTB, intline_INTC, intline_INTD;
	
	num_rx_devices = 0;	
	
	printf ("Scanning PCI Bus for all supported i960Rx ATU Devices.....\n\n");

	printf (" Index    Processor   Bus   Device   Function\n");
	printf (" -----    ---------   ---   ------   --------\n");



	i960Rx_seek (I80960RN_ATU);
	i960Rx_seek (I80960RM_ATU);
	i960Rx_seek (I80960RP_ATU);
	i960Rx_seek	(I80303_ATU);

	if (num_rx_devices == 0)
	{
		printf ("\n*** No i960Rx ATU Found on PCI Bus ***\n");
		return;
	}

	printf ("Enter index number to use for test : ");
	indexChoice = decIn();
	printf ("\n\n");

	if (indexChoice >= num_rx_devices)
	{
		printf ("Invalid index chosen, exiting\n");
		return;
	}

	devloc.bus_number = i960Rx_devices[indexChoice].busno;
	devloc.device_number = i960Rx_devices[indexChoice].devno;
	devloc.function_number = i960Rx_devices[indexChoice].funcno;

	
	sys_read_config_dword (devloc.bus_number,
						   devloc.device_number,
						   devloc.function_number,
						   REGION0_BASE_OFFSET,(UINT32*)&long_data);

	messagingUnitBase = long_data & 0xfffffff0;
	printf ("Messaging Unit PCI Base Address = 0x%X\n", messagingUnitBase);
	OutboundImaskReg = (UINT32 *)(messagingUnitBase + 0x34);

	/* Normally, we would just read the intline value from the configuration
	   space to determine where the interrupt is routed.  However, the i960Rx
	   only requested interrupt resources for one interrupt at a time... */

	/* compute interrupt routing values */
	if ((pci_to_xint(devloc.device_number, INTA, (int*)&intline_INTA)) ||
		(pci_to_xint(devloc.device_number, INTB, (int*)&intline_INTB)) ||
		(pci_to_xint(devloc.device_number, INTC, (int*)&intline_INTC)) ||
		(pci_to_xint(devloc.device_number, INTD, (int*)&intline_INTD)) == ERROR)
	{
		printf ("Error: Unable to connect PCI interrupts with IQ80310 interrupts\n");
		return;
	}

	printf ("i960Rx INTA pin mapped to intLine %s on IQ80310\n", line_to_string(intline_INTA));
	printf ("i960Rx INTB pin mapped to intLine %s on IQ80310\n", line_to_string(intline_INTB));
	printf ("i960Rx INTC pin mapped to intLine %s on IQ80310\n", line_to_string(intline_INTC));
	printf ("i960Rx INTD pin mapped to intLine %s on IQ80310\n", line_to_string(intline_INTD));


	/* Connect i960Rx PCI INTA Handler */
	if (pci_isr_connect (INTA, devloc.bus_number, devloc.device_number, PCI_IntHandler, INTA) 
		== ERROR )
	{
		printf ("Error Connecting INTA interrupt handler\n");
		return;
	}
	printf ("INTA Service Routine installed...\n");

	/* enable PCI INTA */
	enable_external_interrupt(SINTA_INT_ID);

	/* Connect i960Rx PCI INTB Handler */
	if (pci_isr_connect (INTB, devloc.bus_number, devloc.device_number, PCI_IntHandler, INTB) 
		== ERROR)
	{
		printf ("Error Connecting INTB interrupt handler\n");
		return;
	}
	printf ("INTB Service Routine installed...\n");
	
	/* enable PCI INTB */
	enable_external_interrupt(SINTB_INT_ID);

	/* Connect i960Rx PCI INTC Handler */
	if (pci_isr_connect (INTC, devloc.bus_number, devloc.device_number, PCI_IntHandler, INTC) 
		== ERROR)
	{
		printf ("Error Connecting INTC interrupt handler\n");
		return;
	}
	printf ("INTC Service Routine installed...\n");

	/* enable PCI INTC */
	enable_external_interrupt(SINTC_INT_ID);

	/* Connect i960Rx PCI INTD Handler */
	if (pci_isr_connect (INTD, devloc.bus_number, devloc.device_number, PCI_IntHandler, INTD) 
		== ERROR)
	{
		printf ("Error Connecting INTD interrupt handler\n");
		return;
	}
	printf ("INTD Service Routine installed...\n");

	/* enable PCI INTD */
	enable_external_interrupt(SINTD_INT_ID);

	/* make sure that the Outbound interrupts aren't masked */
	*OutboundImaskReg &= ~(OB_STAT_INTA | OB_STAT_INTB | OB_STAT_INTC | OB_STAT_INTD);

	/* let the ISR do the rest of the work... */
	printf ("Waiting for the PCI Interrupts to be received...\n\n");
	printf ("Hit <CR> when the test is complete\n\n\n");
	
	hexIn();

	return;
}


/*******************************************/
/*  Battery Backup SDRAM memory write test */
/*******************************************/
static void battery_test_write (void)
{																																					
	unsigned long start_addr = SDRAM_BATTERY_TEST_BASE;  /* Address to write to */

	/* Data to be written to address and read after the board has been powered off and powered back on */
	UINT32 junk = BATTERY_TEST_PATTERN;		

	*(volatile UINT32 *)start_addr = junk;

	printf("\nThe value '");
	hex32out(BATTERY_TEST_PATTERN);
	printf ("' is now written in DRAM at address $");
	hex32out(SDRAM_BATTERY_TEST_BASE);
	printf(".\n\nYou can now power the board off, wait 60 seconds and power it back on.");
	printf("\nThen come back in the battery test menu and select option 2 to check data from DRAM.\n");

	printf ("\nPress return to continue.\n");
	(void) hexIn();
}


/******************************************/
/*  Battery Backup SDRAM memory read test */
/******************************************/
static void battery_test_read (void)
{
	unsigned long start_addr = SDRAM_BATTERY_TEST_BASE;  /* Address to read from */
	UINT32 value_written = BATTERY_TEST_PATTERN;  /* Data that was written */
	UINT32 value_read;	

	value_read = *(volatile UINT32 *)start_addr;

	printf ("Value written at address $");
	hex32out(SDRAM_BATTERY_TEST_BASE);
	printf (": ");
	hex32out(BATTERY_TEST_PATTERN);
	printf ("\nValue read at address $");
	hex32out(SDRAM_BATTERY_TEST_BASE);
	printf("   : ");
	hex32out(value_read);

	if (value_read == value_written) printf ("\n\nThe battery test is a success !\n");
	else
	{
		printf ("\n\n****************************\n");
		printf ("* The battery test failed. *\n");
		printf ("****************************\n");
	}

  printf ("\nBattery test done.\n");
  printf ("Press return to continue.\n");
  (void) hexIn();
}


/*************************************/
/*  Battery Backup SDRAM memory menu */
/*************************************/
static void battery_test_menu (void)
{
	/* Test Menu Table */
	static MENU_ITEM batteryMenu[] =
	{
		{"Write data to SDRAM",			battery_test_write,		NULL},
		{"Check data from SDRAM",		battery_test_read,		NULL},
	};

	unsigned int num_menu_items =	(sizeof (batteryMenu) / sizeof (batteryMenu[0]));

/*  char menu_title[15] = "\n Battery Backup SDRAM memory test."; */
  char menu_title[36] = "\n Battery Backup SDRAM memory test.";

	printf ("\n*************************************************************************\n");
	printf ("* This test will enable you to perform a battery test in 4 steps:       *\n"); 
	printf ("*  1/  Select option 1 to write the value '");
	hex32out(BATTERY_TEST_PATTERN);
	printf ("' to DRAM at address *\n*      $");
	hex32out(SDRAM_BATTERY_TEST_BASE);
	printf (",                                                       *\n"); 
	printf ("*  2/  Power the board off and wait 60 seconds,                         *\n"); 
	printf ("*  3/  Power the board back on,                                         *\n"); 
	printf ("*  4/  Select option 2 to read at address $");
	hex32out(SDRAM_BATTERY_TEST_BASE);
	printf (" and compare the     *\n*      value to the value written '");
	hex32out(BATTERY_TEST_PATTERN); 
	printf ("'.                           *\n");
	printf ("*************************************************************************");

	menu (batteryMenu, num_menu_items, menu_title, MENU_OPT_NONE);
	printf ("\n");
}

/* 01/11/01 jwf */
/* Make the user select their host test platform type from a list of choices. */
/* If the user picks a Cyclone SB923 then modify the Outbound PCI Translate Register on the IQ80310 for the SB923 */
void select_host_test_system (void)
{
	char selection;

	printf("Select your Host test system\n\n");
	printf("Make a selection by typing a number.\n\n");
	printf("1 - Cyclone SB923\n");
	printf("2 - Personal Computer or other\n");

	do
	{
		selection = xgetchar();
	}
	while( (selection != '1') && (selection != '2') );

	if (selection == '1')
	{
			/* Modify the Outbound PCI Translate Register for a SB923, at address 0x1254 */
		*(volatile UINT32 *) POMWVR_ADDR = 0xa0000000;
	}
}


/* 02/09/01 jwf */
/* Read the 80200 Coyanosa ID register */
/* Use a base address equal to the fourth memory location from the last memory location */
/* in a 32MB SDRAM DIMM */
/* Store from coprocessor register 15 to memory. */
/* ARM register R0 is the address after the transfer */
void read_coyanosa_id_reg (void)
{

	__asm__ ("ldr r0, = 0xA1FFFFFC");

	__asm__ ("mrc p15, 0, r1, c0, c0, 0");

	__asm__ ("str r1, [r0], #0");

}


/* 02/09/01 jwf */
/*
*Display the following version information.
*1. Software version of Red Boot or Cygnus Cygmon Rom Monitor.
*2. Cpld version.  Located in the 0xFE840000 (Read)
*3. Board Revision. Located at 0xFE830000 (Read)
*4. 80200 ID data Located in CP15 Register 0
*5. 80312 Stepping Located at 0x00001008
*/
void version_info (void)
{
	char board_rev;

/* show revision information for operating system */
#if CYGNUS_CYGMON_OS
	extern void version(void); /* is defined in monitor.c */
	version();
#endif

#if REDHAT_REDBOOT_OS
	extern void do_version(int argc, char *argv[]);/* is defined in main.c */
	do_version(0,0);
#endif

	board_rev = board_revision();
	if ( board_rev >= BOARD_REV_E )
	{
			/* read Board revision register and adjust numeric revision to letter revision, 0x1 <--> A */	
		printf("\nBoard Revision = %c\n",(*BOARD_REV_REG_ADDR & BOARD_REV_MASK) + 'A' - 1 );
	}
	else
	{
			/* Board letter revision might be A or B or C or D */	
		printf("\nBoard Revision Unknown!\n");
	}

		/* read CPLD revision register and adjust numeric revision to letter revision, 0x1 <--> A */
	printf("CPLD Revision = %c\n",(*CPLD_REV_REG_ADDR & BOARD_REV_MASK) + 'A' - 1 );

		/* Read the 80200 Coyanosa ID register */
	read_coyanosa_id_reg();

	printf( "80200 Revision ID = 0x%x\n", ( *(volatile unsigned long *) COYANOSA_ID_BASE_ADDR) );
	
		/* read the 80312 Yavapai Revision ID register */
	printf( "80312 Revision ID = %d\n", (*(volatile unsigned char *) RIDR_ADDR) );

	printf ("\n\nStrike <CR> to exit this test.\n\n");
	
	hexIn();

}


/* 02/09/01 jwf */
/* Determine if the CPLD supports a Board Revision Register. */
/* If a Board Revision Register exists then return the board revision number read from a dedicated CPLD register at memory address 0xfe830000 */
/* Otherwise return a default board revision number. */
char board_revision ()
{
		/* represents the ring indicator bit logic level in UART2 */
	unsigned char ri_state;

		/* holds a board revision number */
	char board_rev;

		/* access UART2 MSR at memory address 0xfe810006 through the CYGMON serial port, J9 */
	ri_state = *( unsigned char * ) 0xfe810006;
	ri_state &= RI_MASK;

		/* RI# pin on UART2 is grounded */
		/* CPLD design supports a Board Revision Register implemention */
	if(ri_state == RI_MASK)
	{
			/* read Board Revision register and isolate LSN */
		board_rev = (*BOARD_REV_REG_ADDR & BOARD_REV_MASK);

			/* Board Rev is at E or higher */
		if (board_rev >= BOARD_REV_E)
		{
			return (board_rev);
		}
	}

		/* RI# pin on UART2 is pulled up to 3.3V. */
		/* Unknown Board Revision! */
		/* Unable to determine a board revision because the CPLD Board Revision Register */
		/* was never implemented for IQ80310 PCI-700 board REVs A,B,C, or D. */
		/* set a default board revision value of 0x2 <--> Rev B. */
	board_rev = 0x2;

		/* return a default value */
	return (board_rev);
}



