#ifndef CYGONCE_CONFIG_KEYS_H
#define CYGONCE_CONFIG_KEYS_H
// ====================================================================
//
//      config_keys.h
//
//      Device I/O "Keys" for get/put config functions
//
// ====================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Author(s):   gthomas
// Contributors:        gthomas
// Date:        1999-02-04
// Purpose:     Repository for all get/put config "keys"
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

// This file contains all of the 'key' values used by all I/O components.
// It is placed in this single repository to make it easy to reduce conflicts.

// Get/Set configuration 'key' values for low-level serial I/O
#define CYG_IO_GET_CONFIG_SERIAL_INFO            0x0101
#define CYG_IO_GET_CONFIG_SERIAL_OUTPUT_DRAIN    0x0102
#define CYG_IO_GET_CONFIG_SERIAL_OUTPUT_FLUSH    0x0103
#define CYG_IO_GET_CONFIG_SERIAL_INPUT_FLUSH     0x0104
#define CYG_IO_GET_CONFIG_SERIAL_ABORT           0x0105
#define CYG_IO_SET_CONFIG_SERIAL_INFO            0x0181

// Get/Set configuration 'key' values for tty-like driver
#define CYG_IO_GET_CONFIG_TTY_INFO       0x0201  // Get channel configuration
#define CYG_IO_SET_CONFIG_TTY_INFO       0x0281  // Set channel configuration

#endif  /* CYGONCE_CONFIG_KEYS_H */
/* EOF config_keys.h */
