// #========================================================================
// #
// #    i2s_audio_FIQ.S
// #
// #    FIQ handler for EB7209 audio stream
// #
// #========================================================================
// ####COPYRIGHTBEGIN####
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
// ####COPYRIGHTEND####
// #========================================================================
// ######DESCRIPTIONBEGIN####
// #
// # Author(s):     gthomas
// # Contributors:  gthomas
// # Date:          1999-09-10
// # Purpose:       Support EB7209 audio interrupts
// # Description:   This routine will send data to the audio FIFOs.
// #
// #####DESCRIPTIONEND####
// #
// #========================================================================
	
#include <cyg/hal/hal_edb7xxx.h>

        .file   "i2s_audio_FIQ.S"

        .extern cur_buf
        .extern next_buf
// These defines must mimic the data structure used by the main program        
#define audio_buf_left_ptr   0x0
#define audio_buf_right_ptr  0x4
#define audio_buf_length     0x8                      

        .code   32
        .text

// Note: when handling an FIQ, registers r8-r14 are free to use
// since they are unique (banked).
                
        .globl  i2s_FIQ
i2s_FIQ:

        ldr     r11,=FIQ_count
        ldr     r12,[r11]
        add     r12,r12,#1
        str     r12,[r11]
        
        ldr     r11,=I2S_LEFT_FIFO
        ldr     r12,=I2S_RIGHT_FIFO

// Empty the record FIFOs
// Note: without this, the audio hardware gets stuck        
        ldr     r13,[r11]
        ldr     r13,[r12]        
        ldr     r13,[r11]
        ldr     r13,[r12]        
        ldr     r13,[r11]
        ldr     r13,[r12]        
        ldr     r13,[r11]
        ldr     r13,[r12]        
        
// See if there is a buffer to play        
        ldr     r8,=cur_buf
        ldr     r9,[r8,#audio_buf_length]
        cmp     r9,#0
        beq     05f
        
        ldr     r9,[r8,#audio_buf_left_ptr]
        ldr     r10,[r8,#audio_buf_right_ptr]

// Move 4 [16-bit] values to each FIFO
        ldrh    r13,[r9],#2
        str     r13,[r11]
        ldrh    r13,[r10],#2
        str     r13,[r12]
        
        ldrh    r13,[r9],#2
        str     r13,[r11]
        ldrh    r13,[r10],#2
        str     r13,[r12]
        
        ldrh    r13,[r9],#2
        str     r13,[r11]
        ldrh    r13,[r10],#2
        str     r13,[r12]
        
        ldrh    r13,[r9],#2
        str     r13,[r11]
        ldrh    r13,[r10],#2
        str     r13,[r12]

// Update pointers
        str     r9,[r8,#audio_buf_left_ptr]
        str     r10,[r8,#audio_buf_right_ptr]

// Update count
        ldr     r9,[r8,#audio_buf_length]
        sub     r9,r9,#4
        str     r9,[r8,#audio_buf_length]
        cmp     r9,#0
        bgt     10f
        
// Switch to next buffer
05:     ldr     r9,=next_buf
        ldr     r10,[r9,#audio_buf_left_ptr]
        str     r10,[r8,#audio_buf_left_ptr]
        ldr     r10,[r9,#audio_buf_right_ptr]
        str     r10,[r8,#audio_buf_right_ptr]
        ldr     r10,[r9,#audio_buf_length]
        str     r10,[r8,#audio_buf_length]
        mov     r13,#0
        str     r13,[r9,#audio_buf_length]      // Free buffer
        cmp     r10,#0
        bne     10f

// Disable further interrupts
        ldr     r8,=INTMR3
        ldr     r9,[r8]
        bic     r9,r9,#INTSR3_I2SINT
        str     r9,[r8]

10:
        ldr     r11,=I2S_STAT   // Clear status bits
//        ldr     r10,=(I2S_STAT_RCTSR|I2S_STAT_RCRS|I2S_STAT_LCTSR|I2S_STAT_LCRSR|I2S_STAT_RCTNF|I2S_STAT_RCRNE|I2S_STAT_LCTNF|I2S_STAT_LCRNE)
        ldr     r10,=0xFFF0F
        str     r10,[r11]
        
        subs	pc,lr,#4	// return from interrupt

        .global hal_disable_interrupts
hal_disable_interrupts:             
        mrs     r0,cpsr                 // current state
        orr     r1,r0,#0x80             // mask IRQ
        msr     cpsr,r1
        bx      lr                      // exit, _old_ in r0        

        .data
        .global FIQ_count
FIQ_count:      
        .long   0        
