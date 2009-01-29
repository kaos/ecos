#ifndef CYGONCE_CAN_LPC2XXX_H
#define CYGONCE_CAN_LPC2XXX_H
//==========================================================================
//
//      devs/can/arm/lpc2xxx/current/include/can_lpc2xxx.h
//
//      Extended configuration option for LPC2xxx CAN driver
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Uwe Kindler
// Contributors: Uwe Kindler
// Date:         2007-02-08
// Purpose:      Extended configuration options for LPC2xxx CAN driver
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================


//==========================================================================
//                                DEFINES
//==========================================================================
//
// The LPC2XXX supports enhanced configuration options that are not supported
// be the generic CAN I/O layer. Be careful with using this extension 
// because they may reduce portability of your application
//

//--------------------------------------------------------------------------
// Message filter configuration
//
#define CYG_IO_SET_CONFIG_LPC2XXX_ACCFILT_GROUP  CYG_IO_SET_CONFIG_CAN_ABORT + 0x10 // add message filter group


//--------------------------------------------------------------------------
// Mode setup of LPC2XXX
//
#define CYGNUM_CAN_MODE_LPC2XXX_LISTEN_ONLY       0x80 // set controller in listen only mode


//==========================================================================
//                               DATA TYPES
//==========================================================================
//
// structure for configuration of message filter groups
//
typedef struct cyg_can_filtergroup_cfg_st
{
    cyg_can_id_type        ext;   
    cyg_uint32             lower_id_bound;
    cyg_uint32             upper_id_bound;
} cyg_can_filtergroup_cfg;


//---------------------------------------------------------------------------
#endif // CYGONCE_CAN_LPC2XXX_H
