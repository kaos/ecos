#ifndef _LCD_SUPPORT_H_
#define _LCD_SUPPORT_H_
//==========================================================================
//
//        lcd_support.h
//
//        SA1110/iPAQ - LCD support routines
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     gthomas
// Contributors:  gthomas
// Date:          2001-02-24
// Description:   Simple LCD support
//####DESCRIPTIONEND####

struct lcd_info {
    short height, width;  // Pixels
    short bpp;            // Depth (bits/pixel)
    short type;
    short rlen;           // Length of one raster line in bytes
    void  *fb;            // Frame buffer
};

// Frame buffer types
#define FB_TRUE_RGB565 0x01

// Exported functions
void lcd_init(int depth);
void lcd_clear(void);
void lcd_brightness(int level);
void lcd_moveto(int X, int Y);
void lcd_putc(cyg_int8 c);
int  lcd_printf(char const *fmt, ...);
void lcd_setbg(int red, int green, int blue);
void lcd_setfg(int red, int green, int blue);
void lcd_comm_init(void);
int  lcd_getinfo(struct lcd_info *info);
void lcd_on(bool enable);

#endif //  _LCD_SUPPORT_H_
