//==========================================================================
//
//        lcd_test.c
//
//        SA1110/Assabet - Compact Flash test
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     gthomas
// Contributors:  gthomas
// Date:          2000-06-05
// Description:   Tool used to test LCD stuff
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>       // Configuration header
#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>

#include <cyg/hal/hal_io.h>       // IO macros
#include <cyg/hal/hal_arch.h>     // Register state info
#include <cyg/hal/hal_intr.h>     // HAL interrupt macros

#include <cyg/hal/hal_sa11x0.h>   // Board definitions
#include <cyg/hal/assabet.h>
#include <cyg/hal/hal_cache.h>

#include "eCos.xpm"
#include "redhat.xpm"
#include "escw.xpm"

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

#define STACK_SIZE 4096
static char stack[STACK_SIZE];
static cyg_thread thread_data;
static cyg_handle_t thread_handle;

static struct lcd_frame {
    unsigned short palette[16];
    unsigned short pixels[240][320];
    unsigned char  pad[256];
} lcd_frame_buffer;

#define RGB_RED(x)   (((x)&0x1F)<<11)
#define RGB_GREEN(x) (((x)&0x3F)<<5)
#define RGB_BLUE(x)  ((x)&0x1F)

static volatile struct lcd_frame *fp;

// FUNCTIONS

static void
cyg_test_exit(void)
{
    while (TRUE) ;
}

static int
_hexdigit(char c)
{
    if ((c >= '0') && (c <= '9')) {
        return c - '0';
    } else
    if ((c >= 'A') && (c <= 'F')) {
        return (c - 'A') + 0x0A;
    } else
    if ((c >= 'a') && (c <= 'f')) {
        return (c - 'a') + 0x0a;
    }
}

static int
_hex(char *cp)
{
    return (_hexdigit(*cp)<<4) | _hexdigit(*(cp+1));
}

static unsigned short
parse_color(char *cp)
{
    int red, green, blue;

    if (*cp == '#') {
        red = _hex(cp+1);
        green = _hex(cp+3);
        blue = _hex(cp+5);
        return RGB_RED(red>>3) | RGB_GREEN(green>>2) | RGB_BLUE(blue>>3);
    } else {
        // Should be "None"
        return 0xFFFF;
    }
}

static void
show_xpm(char *xpm[])
{
    int i, row, col;
    char *cp;
    int nrows, ncols, nclrs;
    unsigned short colors[256];  // Mapped by character index

    cp = xpm[0];
    if (sscanf(cp, "%d %d %d", &ncols, &nrows, &nclrs) != 3) {
        diag_printf("Can't parse XPM data, sorry\n");
        return;
    }
    diag_printf("%d rows, %d cols, %d colors\n", nrows, ncols, nclrs);

    for (i = 0;  i < 256;  i++) {
        colors[i] = 0x0000;
    }
    for (i = 0;  i < nclrs;  i++) {
        cp = xpm[i+1];
        colors[(unsigned int)*cp] = parse_color(&cp[4]);
    }

    for (row = 0;  row < nrows;  row++) {            
        cp = xpm[nclrs+1+row];        
        for (col = 0;  col < ncols;  col++) {
#if 0
            if (*cp++ != '.') {
                fp->pixels[row][col] = 0x0000;
            } else {
                fp->pixels[row][col] = 0xFFFF;
            }
#else
            fp->pixels[row][col] = colors[(unsigned int)*cp++];
#endif
        }
    }
    cyg_thread_delay(100);
}

static void
lcd_test(cyg_addrword_t p)
{
    int i, pix, row, col;
    unsigned long _fp;
    int on;

    diag_printf("LCD test here\n");

//    HAL_DCACHE_SYNC();  
//    HAL_DCACHE_DISABLE();

    // Frame buffer must be aligned on a 16 byte boundary
    _fp = (((unsigned long)&lcd_frame_buffer) + 15) & ~15;
    fp = (struct lcd_frame *)(_fp + SA11X0_RAM_BANK0_BASE);
    // Enable LCD in 320x240 16bpp
    *SA1110_DBAR1 = (unsigned long)&(fp->palette);
    *SA1110_LCCR1 = 0x1D1D0930;
    *SA1110_LCCR2 = 0xEF;
    *SA1110_LCCR3 = 0x0c;
    fp->palette[0] = 0x2000;  // Tell controller 16 bits
    *SA1110_LCCR0 = 0xBB;  // B&W
    *SA1110_LCCR0 = 0xB9;  // Color
    assabet_BCR(SA1110_BCR_LCD_BPP, SA1110_BCR_LCD_12BPP);
    assabet_BCR(SA1110_BCR_LCD_BPP, SA1110_BCR_LCD_16BPP);
    assabet_BCR(SA1110_BCR_LCD, SA1110_BCR_LCD_ON);
#if 0
    for (i = 0;  i < 16;  i++) {
        on = true;
        diag_printf("Fill with 0x%x\n", i);
        for (row = 0;  row < 240;  row++) {            
            for (col = 0;  col < 320/2;  col++) {
                if (on) {
                    fp->pixels[row][col] = RGB_RED(i)|RGB_GREEN(i)|RGB_BLUE(i);
                } else {
                    fp->pixels[row][col] = 0xFFFF;
                }
            }
            for (col = 320/2;  col < 320;  col++) {
                if (!on) {
                    fp->pixels[row][col] = RGB_RED(i)|RGB_GREEN(i)|RGB_BLUE(i);
                } else {
                    fp->pixels[row][col] = 0xFFFF;
                }
            }
            if ((row & 0x0F) == 0x0F) {
                if (on) {
                    on = false;
                } else {
                    on = true;
                }
            }
        }
        cyg_thread_delay(100);
    }
#endif
#if 0
    for (i = 0;  i < 4;  i++) {
        for (row = 0;  row < 240;  row++) {            
            for (col = 0;  col < 320;  col++) {
                switch (row/40) {
                case 0:
                    pix = col / 20;  // 0..15
                    fp->pixels[row][col] = RGB_RED(pix);
                    break;
                case 1:
                    pix = col / 10;  // 0..31
                    fp->pixels[row][col] = RGB_GREEN(pix);
                    break;
                case 2:
                    pix = col / 20;  // 0..15
                    fp->pixels[row][col] = RGB_BLUE(pix);
                    break;
                case 3:
                    pix = col / 20;  // 0..15
                    fp->pixels[row][col] = RGB_BLUE(pix) | RGB_GREEN(pix);
                    break;
                case 4:
                    pix = col / 20;  // 0..15
                    fp->pixels[row][col] = RGB_BLUE(15) | RGB_GREEN(pix);
                    break;
                case 5:
                    fp->pixels[row][col] = 0xFFFF;
                    break;
                }
            }
        }
        cyg_thread_delay(100);
#if 0
        for (row = 0;  row < 240;  row++) {            
            for (col = 0;  col < 320;  col++) {
                pix = col / 20;  // 0..15
                switch (row/60) {
                case 0:
                    fp->pixels[row][col] = RGB_RED(pix);
                    break;
                case 1:
                    fp->pixels[row][col] = RGB_GREEN(pix);
                    break;
                case 2:
                    fp->pixels[row][col] = RGB_BLUE(pix);
                    break;
                case 3:
                    fp->pixels[row][col] = 0xFFFF;
                    break;
                }
            }
        }
        cyg_thread_delay(100);
#endif
#if 0
        on = true;
        for (row = 0;  row < 240;  row++) {            
            for (col = 0;  col < 320/2;  col++) {
                if (on) {
                    fp->pixels[row][col] = RGB_GREEN(15);
                } else {
                    fp->pixels[row][col] = RGB_BLUE(8);
                }
            }
            for (col = 320/2;  col < 320;  col++) {
                if (!on) {
                    fp->pixels[row][col] = RGB_GREEN(15);
                } else {
                    fp->pixels[row][col] = RGB_BLUE(8);
                }
            }
            if ((row & 0x0F) == 0x0F) {
                if (on) {
                    on = false;
                } else {
                    on = true;
                }
            }
        }
#endif
    }
#endif
#if 1
    for (row = 0;  row < 240;  row++) {            
        for (col = 0;  col < 320;  col++) {
            if (col == 59) {
                fp->pixels[row][col] = 0x0000;
            } else {
                fp->pixels[row][col] = 0xFFFF;
            }
        }
    }
    cyg_thread_delay(100);
#endif

    for (i = 0;  i < 5;  i++) {
    show_xpm(eCos_xpm);
    show_xpm(redhat_xpm);
    show_xpm(escw_xpm);
    }

    assabet_BCR(SA1110_BCR_MOTOR, SA1110_BCR_MOTOR_ON);
    cyg_thread_delay(2*100);
    assabet_BCR(SA1110_BCR_MOTOR, SA1110_BCR_MOTOR_OFF);
    cyg_test_exit();
}

externC void
cyg_start( void )
{
    // Create a main thread, so we can run the scheduler and have time 'pass'
    cyg_thread_create(10,                // Priority - just a number
                      lcd_test,          // entry
                      0,                 // entry parameter
                      "LCD test",        // Name
                      &stack[0],         // Stack
                      STACK_SIZE,        // Size
                      &thread_handle,    // Handle
                      &thread_data       // Thread data structure
            );
    cyg_thread_resume(thread_handle);  // Start it
    cyg_scheduler_start();
} // cyg_package_start()
