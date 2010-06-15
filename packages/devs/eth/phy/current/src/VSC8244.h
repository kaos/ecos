#ifndef CYGONCE_PHY_VSC8244_HEADER_
#define CYGONCE_PHY_VSC8244_HEADER_
//==========================================================================
//
//      phy/VSC8244.h
//
//      Ethernet PHY driver
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System. 
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 
// 2008, 2009 Free Software Foundation, Inc. 
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
// Author(s):    ccoutand
// Contributors: 
// Date:         2009-11-01
// Purpose:      
// Description:  Hardware driver for Ethernet PHY Vitesse VSC8244
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#define VSC8244_MII_PHY_STAT_DUPLEX_FULL  0x20
#define VSC8244_MII_PHY_STAT_1000MB       0x10
#define VSC8244_MII_PHY_STAT_100MB        0x08

#define VSC8244_MII_PHY_STAT_LINK_UP      0x04

#define VSC8244_MII_STAT_REG              0x01
#define VSC8244_MII_PHY_STAT_REG          0x1C

#define VSC8244_AUTO_COMPLETED            0x20

#endif  // CYGONCE_PHY_VSC8244_HEADER_
