#ifndef CYGONCE_PCMCIA_H
#define CYGONCE_PCMCIA_H
// ====================================================================
//
//      pcmcia.h
//
//      Device I/O 
//
// ====================================================================
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
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     gthomas
// Contributors:  gthomas
// Date:          2000-07-06
// Purpose:       Interfaces for PCMCIA I/O drivers
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

// PCMCIA I/O interfaces

#include <pkgconf/system.h>
#include <pkgconf/io_pcmcia.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/drv_api.h>

struct cf_irq_handler {
    void         (*handler)(int, int, void *);
    void          *param;
};

// Basic information about a slot/device
struct cf_slot {
    int            index;       // In case hardware layer needs it
    int            state;
    unsigned char *attr;
    int            attr_length;
    unsigned char *io;
    int            io_length;
    unsigned char *mem;
    int            mem_length;
    int            int_num;    // Hardware interrupt number
    struct cf_irq_handler irq_handler;
};

#define CF_SLOT_STATE_Empty     0
#define CF_SLOT_STATE_Inserted  1
#define CF_SLOT_STATE_Powered   2
#define CF_SLOT_STATE_Reset     3
#define CF_SLOT_STATE_Ready     4
#define CF_SLOT_STATE_Removed   5

#define CF_CISTPL_VERS_1        0x15
#define CF_CISTPL_CONFIG        0x1A
#define CF_CISTPL_CFTABLE_ENTRY 0x1B
#define CF_CISTPL_MANFID        0x20
#define CF_CISTPL_FUNCID        0x21

#define CF_MAX_IO_ADDRS      8
struct cf_io_space {    
    unsigned long base[CF_MAX_IO_ADDRS];     // Base address of I/O registers
    unsigned long size[CF_MAX_IO_ADDRS];     // Length(-1) of I/O registers
    int           num_addrs;
    unsigned char mode;
};

// Corresponds to CISTPL_CFTABLE_ENTRY
struct cf_cftable {
    unsigned char      cor;              // Value to write to COR register
    unsigned char      interface;
    unsigned char      feature_select;
    struct cf_io_space io_space;
};

// Corresponds to CISTPL_CONFIG
struct cf_config {
    unsigned long base;
    int           mask_length;
    unsigned char mask[16];
};

// Function prototypes

bool cf_get_CIS(struct cf_slot *slot, unsigned char id, 
                unsigned char *buf, int *len, int *ptr);
void cf_set_COR(struct cf_slot *slot, unsigned long cor, unsigned char val);
bool cf_parse_cftable(unsigned char *buf, int len, struct cf_cftable *cftable);
bool cf_parse_config(unsigned char *buf, int len, struct cf_config *config);
struct cf_slot *cf_get_slot(int indx);
void cf_change_state(struct cf_slot *slot, int desired_state);
void cf_register_handler(struct cf_slot *, void (*handler)(int, int, void *), void *);
void cf_clear_interrupt(struct cf_slot *);
void cf_init(void);

#endif // CYGONCE_PCMCIA_H
