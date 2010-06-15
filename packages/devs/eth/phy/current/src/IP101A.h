#ifndef CYGONCE_PHY_IP101A_HEADER_
#define CYGONCE_PHY_IP101A_HEADER_
//==========================================================================
//
//      phy/ip101a.h
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
// Author(s):    Edgar Grimberg
// Contributors: 
// Date:         2009-11-01
// Purpose:      
// Description:  Hardware driver for Ethernet PHY IC+ IP101A
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#define MII_CTRL_REG                0x00
#define MII_STAT_REG                0x01
#define MII_PHY_ID1_REG             0x02
#define MII_PHY_ID2_REG             0x03
#define MII_AUTO_NEG_ADV_REG        0x04
#define MII_AUTO_NEG_LPA_REG        0x05
#define MII_AUTO_NEG_EXP_REG        0x06
#define MII_PHY_CTRL_REG            0x10
#define MII_PHY_IRQ_REG             0x11
#define MII_PHY_STAT_REG            0x12
#define MII_PHY_CTRL2_REG           0x1E

#define MII_PHY_IRQ_INTR            0x8000
#define MII_PHY_IRQ_ALL_MASK        0x0800
#define MII_PHY_IRQ_SPEED_MASK      0x0400
#define MII_PHY_IRQ_DUPLEX_MASK     0x0200
#define MII_PHY_IRQ_LINK_MASK       0x0100
#define MII_PHY_IRQ_ARBITER_MASK    0x0080
#define MII_PHY_IRQ_ARBITER_CHANGE  0x0040
#define MII_PHY_IRQ_SPEED_CHANGE    0x0004
#define MII_PHY_IRQ_DUPLEX_CHANGE   0x0002
#define MII_PHY_IRQ_LINK_CHANGE     0x0001

#define IP101A_CTRL_DUPLEX          0x0100
#define IP101A_CTRL_100MB           0x2000
#define IP101A_CTRL_AUTO_NEG        0x1000
#define IP101A_CTRL_AUTO_NEG_RST    0x0200

#define IP101A_LINK_STATUS          0x0004
#define IP101A_AUTO_COMPLETED       0x20

#define IP101A_DUPLEX_MODE          0x2000
#define IP101A_SPEED_100MB          0x4000
#define IP101A_LINK_STATUS2         0x0400

#endif // CYGONCE_PHY_IP101A_HEADER_

