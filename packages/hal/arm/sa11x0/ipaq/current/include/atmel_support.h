#ifndef CYGONCE_ATMEL_SUPPORT_H
#define CYGONCE_ATMEL_SUPPORT_H

//=============================================================================
//
//      atmel_support.h
//
//      Platform specific support (register layout, etc)
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
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2001-02-27
// Purpose:      Intel SA1110/iPAQ platform specific support routines
// Description: 
// Usage:        #include <cyg/hal/atmel_support.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

// Command codes 
#define ATMEL_CMD_VERSION      0
#define ATMEL_CMD_UNKNOWN      1
#define ATMEL_CMD_KEYBD        2
#define ATMEL_CMD_TOUCH        3
#define ATMEL_CMD_EEPROM_READ  4
#define ATMEL_CMD_EEPROM_WRITE 5
#define ATMEL_CMD_THERMAL      6
#define ATMEL_CMD_LED          8
#define ATMEL_CMD_BATTERY      9
#define ATMEL_CMD_SPI_READ     11
#define ATMEL_CMD_SPI_WRITE    12
#define ATMEL_CMD_LIGHT        13
#define NUM_ATMEL_CMDS         16

// Structure of actual packets
//
// Byte 0 - SOF
// Byte 1 - (cmd << 4) | length
// Byte 2 - Data   |
// ...             | 'length' bytes (0..15) 
// Byte m - Data   |
// Byte n - Checksum 
//
// Since the length must be less than 16, the entire
// packet will fit in 18 bytes
//
#define ATMEL_PKT_LEN 18
typedef struct _atmel_pkt {
    struct _atmel_pkt *next;
    unsigned char data[ATMEL_PKT_LEN];
    int len, size;
} atmel_pkt;
#define ATMEL_PKT_SOF  0
#define ATMEL_PKT_CMD  1
#define ATMEL_PKT_DATA 2

#define ATMEL_NUM_PKT  4

#define SOF 0x02

//
// Keyboard events are one byte with the button code
// and state or'd
//
#define ATMEL_BUTTON_STATE      0x80
#define ATMEL_BUTTON_STATE_DOWN 0x00
#define ATMEL_BUTTON_STATE_UP   0x80
#define ATMEL_BUTTON_VALUE      0x7F

#define ATMEL_BUTTON_RECORD     0x01
#define ATMEL_BUTTON_CALENDAR   0x02
#define ATMEL_BUTTON_MEMO       0x03
#define ATMEL_BUTTON_Q          0x04
#define ATMEL_BUTTON_RETURN     0x05
#define ATMEL_BUTTON_JOY_UP     0x06
#define ATMEL_BUTTON_JOY_RIGHT  0x07
#define ATMEL_BUTTON_JOY_DOWN   0x09
#define ATMEL_BUTTON_JOY_LEFT   0x08

struct key_event {
    unsigned char button_info;
};

//
// Touch screen events
// Note: up/down is really hard to discern
//
struct ts_event {
    bool   up;
    short  x, y;
};

// Functions
void atmel_init(void);
bool atmel_send(int, unsigned char *, int);
int  atmel_recv(unsigned char *, int);
typedef void atmel_handler(atmel_pkt *);
void atmel_register(int, atmel_handler *);
void atmel_interrupt_mode(bool enable);

bool ts_get_event(struct ts_event *);
bool key_get_event(struct key_event *);

#endif /* CYGONCE_ATMEL_SUPPORT_H */
