//=============================================================================
//
//      io_utils.c - Cyclone Diagnostics
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

/*
 * i/o routines for tests.  Greg Ames, 9/17/90.
 *
 * Version: @(#)test_io.c	1.2 8/26/93
 */
#include <redboot.h>

#define TRUE	1
#define FALSE	0


#define ASCII_TO_DEC 48
void atod(char a, int* b)
{
	*b = (int)(a - ASCII_TO_DEC);
}

char xgetchar(void)
{
    char ch;
    hal_virtual_comm_table_t* __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
    
    if (__chan)
        ch = CYGACC_COMM_IF_GETC(*__chan);
    else {
        __chan = CYGACC_CALL_IF_DEBUG_PROCS();
        ch = CYGACC_COMM_IF_GETC(*__chan);
    }
    return ch;
}

/*
 * naive implementation of "gets"
 * (big difference from fgets == strips newline character)
 */
char* sgets(char *s)
{

	char *retval = s;
	char ch;
 
	while ((ch = (char)xgetchar()))
	{
		if (ch == 0x0d) /* user typed enter */
		{
			printf("\n");
			break;
		}
		else if (ch == 0x08) /* user typed backspace */
		{
			printf ("\b");
			printf (" ");
			printf ("\b");
			s--;
		}
		else /* user typed another character */
		{
			printf("%c", ch);
			*s++ = ch;   
		}

	}
  
	*s = '\0';
	return retval; 
}


/* Returns true if theChar is a valid hex digit, false if not */
char ishex(char theChar)
{
    switch(theChar) 
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'a':
		case 'B':
		case 'b':
		case 'C':
		case 'c':
		case 'D':
		case 'd':
		case 'E':
		case 'e':
		case 'F':
		case 'f':
			return 1;
		default:
			return 0;
    }
}


/* Returns true if theChar is a valid decimal digit, false if not */
char isdec(char theChar)
{
    switch(theChar) 
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return 1;
		default:
			return 0;
	}
}

/* Convert ascii code of hex digit to number (0-15) */
char    hex2dec(char hex)
{
    if ((hex >= '0') && (hex <= '9'))
        return hex - '0';
    else if ((hex >= 'a') && (hex <= 'f'))
        return hex - 'a' + 10;
    else
        return hex - 'A' + 10;
}
 

/* Convert number (0-15) to ascii code of hex digit */
char dec2hex(char dec)
{
    return (dec <= 9) ? (dec + '0') : (dec - 10 + 'A');
}


/* Output an 8 bit number as 2 hex digits */
void hex8out(unsigned char num)
{
	printf("%02X",num);
}


/* Output an 32 bit number as 8 hex digits */
void hex32out(unsigned long num)
{
	printf("%08X",num);
}


/* Input a number as (at most 8) hex digits - returns value entered */
long hexIn(void)
{
	char input[40];
	long num;
	register int i;

	i = 0;
	num = 0;

	if (sgets (input))  /* grab a line */
	{
        num = hex2dec(input[i++]);       	/* Convert MSD to dec */
        while(ishex(input[i]) && input[i])  /* Get next hex digit */
	    {
            num <<= 4;						/* Make room for next digit */
            num += hex2dec(input[i++]); 	/* Add it in */
        }
	}
	return num;
}


/* Input a number as decimal digits - returns value entered */
long decIn(void)
{
    char input[40];
	int num;
	int tmp;
	register int i;

	i = 0;
	num = 0;

	if (sgets (input))  /* grab a line */
	{
        atod(input[i++], &num);      	/* Convert MSD to decimal */
        while(isdec(input[i]) && input[i])  /* Get next decimal digit */
	    {
            num *= 10;                 	/* Make room for next digit */
			atod(input[i++], &tmp);
            num += tmp; 			/* Add it in */
        }
	}

	return (num);
}


