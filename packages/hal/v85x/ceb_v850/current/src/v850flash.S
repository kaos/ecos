/*=============================================================================
//
//      v850flash.S
//
//      CEB v850 flash boot code
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
// Author(s):    jlarmour
// Contributors: 
// Date:         2001-03-21
// Purpose:      Loader image to program into V850 internal flash to allow
//               programming of eCos ROM startup applications into the EPROM.
// Description:  Compile with:
//            v850-elf-gcc -o v850flash.img v850flash.S -Ttext=0 -nostartfiles
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

        .text
        .org 0
        .globl _start // dummy _start to keep linker happy
_start:
        mov 7,r1
        st.b r1,0xFFFFF04C[r0]
        mov 1,r1
        st.b r1,0xFFFFF064[r0]

        jr 0x0FFFF4 // jump to 0x100000
        jr 0x100000 // jump to 0x100010
        
        .org 0x20
        .rept 0x3E
        jr 0xFFFC0000  // jump to symmetric location in RAM
        nop
        nop
        nop
        nop
        nop
        nop
        .endr

// EOF v850flash.S
