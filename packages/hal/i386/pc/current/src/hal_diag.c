//=============================================================================
//
//      hal_diag.c
//
//      HAL diagnostic output code
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
// Author(s):   proven
// Contributors:proven
// Date:        1998-10-05
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types

#include <cyg/hal/hal_diag.h>


#include <cyg/hal/plf_misc.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#if defined(CYGSEM_HAL_I386_PC_DIAG_SCREEN)

//-----------------------------------------------------------------------------
// Screen output definitions...

static short 		*DisplayBuffer = (short *)0xB8000;
static short		DisplayAttr = 0x0700;

static	short		DisplayPort = 0x03d4;

static	int		XPos;
static	int		YPos;

static	int		ScreenWidth = 80;
static  int		ScreenLength = 25;

//-----------------------------------------------------------------------------

static void MoveLine
(
	short 	*dest,
	short 	*src,
	int	count		
)
{

	while( count-- ) *dest++ = *src++;
	
} /* MoveLine */

//-----------------------------------------------------------------------------

static void FillLine
(
	short  *dest,
	short  val,
	int    count
)
{
	while( count-- ) *dest++ = val;
	
} /* FillLine */

//-----------------------------------------------------------------------------

void ClearScreen(void)
{
    FillLine(DisplayBuffer, ' ' | DisplayAttr, ScreenWidth*ScreenLength);
	
} /* ClearScreen */

void MoveCursor
(
	void
)
{
	int pos = XPos + YPos * ScreenWidth;

	pc_outb(DisplayPort, 0x0e );
	pc_outb(DisplayPort+1, pos >> 8 );

	pc_outb(DisplayPort, 0x0f );
	pc_outb(DisplayPort+1, pos & 0xFF );
	
} /* MoveCursor */

//-----------------------------------------------------------------------------

void ScrollUp
(
	int	lines
)
{
//	Report_Function(ScrollUp)

	int rest = ScreenLength - lines;
	
	MoveLine
	(
		DisplayBuffer,
		DisplayBuffer+(lines*ScreenWidth),
		rest*ScreenWidth
	);

	FillLine
	(
		DisplayBuffer+(rest*ScreenWidth),
		' ' | DisplayAttr,
		lines*ScreenWidth
	);
	
} /* ScrollUp */

//-----------------------------------------------------------------------------

void ScrollDown
(
	int	lines
)
{
//	Report_Function(ScrollDown)

	int rest = ScreenLength - lines;
	short *db = DisplayBuffer+(ScreenWidth*(ScreenLength-1));
	
	while( rest )
	{
		MoveLine
		(
			db,
			db-ScreenWidth,
			ScreenWidth
		);

		rest--;
		db -= ScreenWidth;
	}

	FillLine
	(
		DisplayBuffer,
		' ' | DisplayAttr,
		lines*ScreenWidth
	);
	
} /* ScrollDown */

//-----------------------------------------------------------------------------

void NewLine
(
	void
)
{

	XPos = 0;
	YPos++;
	
	if( YPos >= ScreenLength )
	{
		YPos = ScreenLength-1;
		ScrollUp(1);
	}

	MoveCursor();
	
} /* NewLine */

//-----------------------------------------------------------------------------

void DisplayChar
(
	char	ch
)
{

	DisplayBuffer[XPos + YPos*ScreenWidth] = ch | DisplayAttr;

	XPos++;

	if( XPos >= ScreenWidth )
	{
		XPos = 0;
		YPos++;
		if( YPos >= ScreenLength )
		{
			YPos = ScreenLength-1;
			ScrollUp(1);
		}
	}

	MoveCursor();
	
} /* DisplayChar */

//-----------------------------------------------------------------------------
// Keyboard definitions

#define	KBDATAPORT	0x0060		// data I/O port
#define	KBCMDPORT	0x0064		// command port (write)
#define	KBSTATPORT	0x0064		// status port	(read)

// Scan codes

#define	LSHIFT		0x2a
#define	RSHIFT		0x36
#define	CTRL		0x1d
#define	ALT		0x38
#define	CAPS		0x3a
#define	NUMS		0x45

#define	BREAK		0x80

// Bits for KBFlags

#define	KBNormal	0x0000
#define	KBShift		0x0001
#define	KBCtrl		0x0002
#define KBAlt		0x0004
#define	KBIndex		0x0007	// mask for the above

#define	KBExtend	0x0010
#define	KBAck		0x0020
#define	KBResend	0x0040
#define	KBShiftL	(0x0080 | KBShift)
#define	KBShiftR	(0x0100 | KBShift)
#define	KBCtrlL		(0x0200 | KBCtrl)
#define	KBCtrlR		(0x0400 | KBCtrl)
#define	KBAltL		(0x0800 | KBAlt)
#define	KBAltR		(0x1000 | KBAlt)
#define	KBCapsLock	0x2000
#define	KBNumLock	0x4000

//-----------------------------------------------------------------------------
// Keyboard Variables

static	int	KBFlags = 0;

static	CYG_BYTE	KBPending = 0xFF;

static	CYG_BYTE	KBScanTable[128][4] =
{
//	Normal		Shift		Control		Alt
// 0x00
{	0xFF,		0xFF,		0xFF,		0xFF,   },
{	0x1b,		0x1b,		0x1b,		0xFF,	},
{	'1',		'!',		0xFF,		0xFF,	},
{	'2',		'"',		0xFF,		0xFF,	},
{	'3',		'#',		0xFF,		0xFF,	},
{	'4',		'$',		0xFF,		0xFF,	},
{	'5',		'%',		0xFF,		0xFF,	},
{	'6',		'^',		0xFF,		0xFF,	},
{	'7',		'&',		0xFF,		0xFF,	},
{	'8',		'*',		0xFF,		0xFF,	},
{	'9',		'(',		0xFF,		0xFF,	},
{	'0',		')',		0xFF,		0xFF,	},
{	'-',		'_',		0xFF,		0xFF,	},
{	'=',		'+',		0xFF,		0xFF,	},
{	'\b',		'\b',		0xFF,		0xFF,	},
{	'\t',		'\t',		0xFF,		0xFF,	},
// 0x10
{	'q',		'Q',		0x11,		0xFF,	},
{	'w',		'W',		0x17,		0xFF,	},
{	'e',		'E',		0x05,		0xFF,	},
{	'r',		'R',		0x12,		0xFF,	},
{	't',		'T',		0x14,		0xFF,	},
{	'y',		'Y',		0x19,		0xFF,	},
{	'u',		'U',		0x15,		0xFF,	},
{	'i',		'I',		0x09,		0xFF,	},
{	'o',		'O',		0x0F,		0xFF,	},
{	'p',		'P',		0x10,		0xFF,	},
{	'[',		'{',		0x1b,		0xFF,	},
{	']',		'}',		0x1d,		0xFF,	},
{	'\r',		'\r',		'\n',		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	'a',		'A',		0x01,		0xFF,	},
{	's',		'S',		0x13,		0xFF,	},
// 0x20
{	'd',		'D',		0x04,		0xFF,	},
{	'f',		'F',		0x06,		0xFF,	},
{	'g',		'G',		0x07,		0xFF,	},
{	'h',		'H',		0x08,		0xFF,	},
{	'j',		'J',		0x0a,		0xFF,	},
{	'k',		'K',		0x0b,		0xFF,	},
{	'l',		'L',		0x0c,		0xFF,	},
{	';',		':',		0xFF,		0xFF,	},
{	0x27,		'@',		0xFF,		0xFF,	},
{	'#',		'~',		0xFF,		0xFF,	},
{	'`',		'~',		0xFF,		0xFF,	},
{	'\\',		'|',		0x1C,		0xFF,	},
{	'z',		'Z',		0x1A,		0xFF,	},
{	'x',		'X',		0x18,		0xFF,	},
{	'c',		'C',		0x03,		0xFF,	},
{	'v',		'V',		0x16,		0xFF,	},
// 0x30
{	'b',		'B',		0x02,		0xFF,	},
{	'n',		'N',		0x0E,		0xFF,	},
{	'm',		'M',		0x0D,		0xFF,	},
{	',',		'<',		0xFF,		0xFF,	},
{	'.',		'>',		0xFF,		0xFF,	},
{	'/',		'?',		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	' ',		' ',		' ',		' ',	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xF1,		0xE1,		0xFF,		0xFF,	},
{	0xF2,		0xE2,		0xFF,		0xFF,	},
{	0xF3,		0xE3,		0xFF,		0xFF,	},
{	0xF4,		0xE4,		0xFF,		0xFF,	},
{	0xF5,		0xE5,		0xFF,		0xFF,	},
// 0x40
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},

{	0x15,		0x15,		0x15,		0x15,	},
{	0x10,		0x10,		0x10,		0x10,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
// 0x50
{	0x04,		0x04,		0x04,		0x04,	},
{	0x0e,		0x0e,		0x0e,		0x0e,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
// 0x60
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
// 0x70
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
{	0xFF,		0xFF,		0xFF,		0xFF,	},
	
};

static int KBIndexTab[8] = { 0, 1, 2, 2, 3, 3, 3, 3 };

//-----------------------------------------------------------------------------

static void KeyboardInit
(
	void
)
{
    KBFlags = 0;
	
} /* KeyboardInit */

//-----------------------------------------------------------------------------

static CYG_BYTE KeyboardAscii
(
	CYG_BYTE	scancode
)
{
    CYG_BYTE ascii = 0xFF;

    // Start by handling all shift/ctl keys:

    switch( scancode )
    {
    case 0xe0:
        KBFlags |= KBExtend;
        return 0xFF;

    case 0xfa:
        KBFlags |= KBAck;
        return 0xFF;

    case 0xfe:
        KBFlags |= KBResend;
        return 0xFF;

    case LSHIFT:
        KBFlags |= KBShiftL;
        return 0xFF;

    case LSHIFT | BREAK:
        KBFlags &= ~KBShiftL;
        return 0xFF;

    case RSHIFT:
        KBFlags |= KBShiftR;
        return 0xFF;

    case RSHIFT | BREAK:
        KBFlags &= ~KBShiftR;
        return 0xFF;

    case CTRL:
        if( KBFlags & KBExtend )
        {
            KBFlags |= KBCtrlR;
            KBFlags &= ~KBExtend;
        }
        else	KBFlags |= KBCtrlL;
        return 0xFF;

    case CTRL | BREAK:
        if( KBFlags & KBExtend )
        {
            KBFlags &= ~KBCtrlR;
            KBFlags &= ~KBExtend;
        }
        else	KBFlags &= ~KBCtrlL;
        return 0xFF;


    case ALT:
        if( KBFlags & KBExtend )
        {
            KBFlags |= KBAltR;
            KBFlags &= ~KBExtend;
        }
        else	KBFlags |= KBAltL;
        return 0xFF;

    case ALT | BREAK:
        if( KBFlags & KBExtend )
        {
            KBFlags &= ~KBAltR;
            KBFlags &= ~KBExtend;
        }
        else	KBFlags &= ~KBAltL;
        return 0xFF;

    case CAPS:
        KBFlags ^= KBCapsLock;
    case CAPS | BREAK:
        return 0xFF;

    case NUMS:
        KBFlags ^= KBNumLock;
    case NUMS | BREAK:
        return 0xFF;

    }

    // Clear Extend flag if set
    KBFlags &= ~KBExtend;

    // Ignore all other BREAK codes
    if( scancode & 0x80 ) return 0xFF;

    // Here the scancode is for something we can turn
    // into an ASCII value

    ascii = KBScanTable[scancode & 0x7F][KBIndexTab[KBFlags & KBIndex]];

    return ascii;

} /* KeyboardAscii */

//-----------------------------------------------------------------------------

static int KeyboardTest
(
	void
)
{
    // If there is a pending character, return True
    if( KBPending != 0xFF ) return true;


    // If there is something waiting at the port, get it
    while( pc_inb(KBSTATPORT) & 0x01 )
    {
        CYG_BYTE code = pc_inb(KBDATAPORT);

        // Translate to ASCII
        CYG_BYTE c = KeyboardAscii(code);
		
        // if it is a real ASCII char, save it and
        // return True.
        if( c != 0xFF )
        {
            KBPending = c;
            return true;
        }

    }

    // Otherwise return False
    return false;
	
} /* KeyboardTest */

//-----------------------------------------------------------------------------

static CYG_BYTE KeyboardRead
(
	void
)
{
    CYG_BYTE c;

    // Loop until a character is ready
    while( !KeyboardTest() ) continue;

    // get it
    c = KBPending;
    KBPending = 0xFF;

    // and return it
    return c;
	

} /* KeyboardRead */


//-----------------------------------------------------------------------------

void hal_diag_init(void)
{
    KeyboardInit();
    
    XPos	= 0;
    YPos	= 0;

    ClearScreen();
    MoveCursor();

}

//-----------------------------------------------------------------------------

void hal_diag_write_char(char ch)
{
	switch( ch )
	{
	case '\n':
		NewLine();
		return;

	case '\r':
		XPos = 0;
		MoveCursor();
		return;

	case '\b':
		if( XPos == 0 ) return;
		XPos--;
		MoveCursor();
		return;

	case '\t':
		do
		{
			DisplayChar(' ');
		} while( (XPos % 8) != 0 );
		return;

	case 0x0c:
		ClearScreen();
		XPos = YPos = 0;
		MoveCursor();
		return;		

	case 1:
		ScrollUp(1);
		XPos = 0;
		YPos = ScreenLength-1;
		return;

	case 2:
		ScrollDown(1);
		XPos = 0;
		YPos = 0;
		return;
	
		
	default:
		DisplayChar(ch);
		return;
	}
}

//-----------------------------------------------------------------------------

void hal_diag_read_char(char *c)
{
    *c = KeyboardRead();
}

#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#	define	SERIAL_COM1	(0x3F8)
#	define	SERIAL_COM2	(0x2F8)


#if defined(CYGSEM_HAL_I386_PC_DIAG_SERIAL1)
# define SERIAL_COM SERIAL_COM1
#define CYG_KERNEL_DIAG_GDB                     // turn on GDB debug packets
#elif defined(CYGSEM_HAL_I386_PC_DIAG_SERIAL2)
# define SERIAL_COM SERIAL_COM2
#endif

#ifdef SERIAL_COM

#	define	TBR			(0x00)
#	define	RBR			(0x00)
#	define	IER			(0x01)
#	define	LCR			(0x03)
#	define	MCR			(0x04)
#	define	LSR			(0x05)
#	define	DLL			(0x00)
#	define	DLH			(0x01)
#	define	FCR			(0x02)

/* Configuration for 38400 baud, N81. */
#	define	BAUD_LOW	(3)
#if 0
/* Configuration for 19200 baud, N81. */
#	undef	BAUD_LOW
#	define	BAUD_LOW	(6)
/* Configuration for 9600 baud, N81. */
#	undef	BAUD_LOW
#	define	BAUD_LOW	(12)
#endif
#	define	BAUD_HIGH	(0)
#	define	WM_N81		(3)

//-----------------------------------------------------------------------------


int com1 ;

//-----------------------------------------------------------------------------

void hal_diag_init(void)
{
	com1 = SERIAL_COM ;

/* Initialize the serial port: don't send interrupts; don't go into test mode. */
	pc_outb(com1 + IER, 0);
	pc_outb(com1 + IER, 0);
	pc_outb(com1 + MCR, 0) ;

/* Configure the baud rate & word mode. */
	pc_outb(com1 + LCR, 0x80) ;
	pc_outb(com1 + DLL, BAUD_LOW);
	pc_outb(com1 + DLH, BAUD_HIGH);
	pc_outb(com1 + LCR, WM_N81) ;
}

//-----------------------------------------------------------------------------
/* Send out a character via debugger output. */

void hal_diag_write_char_serial(char __c)
{
	int x;

	do
	{	x = pc_inb(com1 + LSR) ;
	} while (!(x & 0x60)) ;

	pc_outb(com1 + TBR, __c) ;
}

//-----------------------------------------------------------------------------
/* read and return a single char */

void hal_diag_read_char_serial(char *c)
{
	int x;

	do
	{	x = pc_inb(com1 + LSR) ;
	} while (!(x & 0x01)) ;

	c[0] = pc_inb(com1 + RBR) ;
}

//-----------------------------------------------------------------------------

void hal_diag_write_char(char c)
{
#ifdef CYG_KERNEL_DIAG_GDB    
    static char line[100];
    static int pos = 0;

    // No need to send CRs
    if( c == '\r' ) return;

    line[pos++] = c;

    if( c == '\n' || pos == sizeof(line) )
    {

        // Disable interrupts. This prevents GDB trying to interrupt us
        // while we are in the middle of sending a packet. The serial
        // receive interrupt will be seen when we re-enable interrupts
        // later.
        CYG_INTERRUPT_STATE oldstate;
        HAL_DISABLE_INTERRUPTS(oldstate);
        
        while(1)
        {
            static char hex[] = "0123456789ABCDEF";
            cyg_uint8 csum = 0;
            int i;
            char c1;
        
            hal_diag_write_char_serial('$');
            hal_diag_write_char_serial('O');
            csum += 'O';
            for( i = 0; i < pos; i++ )
            {
                char ch = line[i];
                char h = hex[(ch>>4)&0xF];
                char l = hex[ch&0xF];
                hal_diag_write_char_serial(h);
                hal_diag_write_char_serial(l);
                csum += h;
                csum += l;
            }
            hal_diag_write_char_serial('#');
            hal_diag_write_char_serial(hex[(csum>>4)&0xF]);
            hal_diag_write_char_serial(hex[csum&0xF]);

            hal_diag_read_char_serial( &c1 );

            if( c1 == '+' ) break;

//            if( cyg_hal_is_break( &c1 , 1 ) )
//                cyg_hal_user_break( NULL );    

        }
        
        pos = 0;

        // Wait for all data from serial line to drain
        // and clear ready-to-send indication.
//        hal_diag_drain_serial0();

        // And re-enable interrupts
        HAL_RESTORE_INTERRUPTS( oldstate );
        
    }
#else
    hal_diag_write_char_serial(c);
#endif    
}

//-----------------------------------------------------------------------------

void hal_diag_read_char(char *c)
{
    hal_diag_read_char_serial(c);
}

#endif

//-----------------------------------------------------------------------------
// End of hal_diag.c

