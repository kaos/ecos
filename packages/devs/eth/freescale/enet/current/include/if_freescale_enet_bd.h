#ifndef CYGONCE_FREESCALE_ENET_BD_H
#define CYGONCE_FREESCALE_ENET_BD_H
//=============================================================================
//
//      freescale_enet_bd.h
//
//      Variant specific registers
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2011 Free Software Foundation, Inc.                        
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Ilija Kocho <ilijak@siva.com.mk>
// Date:          2011-06-05
// Purpose:       Freescale ENET buffer descriptors
// Description:
// Usage:         #include <cyg/io/freescale_enet_bd.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

//---------------------------------------------------------------------------
// ENET Buffer Descriptors
// All mrmbers except key must be big endian
typedef volatile struct enet_bd_s {
    cyg_uint16  ctrl;            // 00
    cyg_uint16  len;             // 02
    cyg_uint8   *buffer_p;       // 04
#ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
    cyg_uint32  ebd_ctrl;        // 08
    cyg_uint16  hlen_proto;      // 0c
    cyg_uint16  payload_csum;    // 0e
    cyg_uint32  bdu;             // 10
    cyg_uint32  timestamp_1588;  // 14
    cyg_uint32  reserved[2];     // 18
#endif //  CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
} enet_bd_t;


#if (CYG_BYTEORDER == CYG_LSBFIRST)
//======================================================================
// Little Endian
//----------------------------------------------------------------------

// ----------------------------------------------------------------------
// TX Buffer Descriptor Bit Definitions

# define ENET_TXBD_R        0x0080
# define ENET_TXBD_TO1      0x0040
# define ENET_TXBD_W        0x0020
# define ENET_TXBD_TO2      0x0010
# define ENET_TXBD_L        0x0008
# define ENET_TXBD_TC       0x0004
# define ENET_TXBD_ABC      0x0002

// ----------------------------------------------------------------------
// TX Enhanced BD Bit Definitions

# define ENET_TXBD_INT       0x00000040
# define ENET_TXBD_TS        0x00000020
# define ENET_TXBD_PINS      0x00000010
# define ENET_TXBD_IINS      0x00000008
# define ENET_TXBD_TXE       0x00800000
# define ENET_TXBD_UE        0x00200000
# define ENET_TXBD_EE        0x00100000
# define ENET_TXBD_FE        0x00080000
# define ENET_TXBD_LCE       0x00040000
# define ENET_TXBD_OE        0x00020000
# define ENET_TXBD_TSE       0x00010000

# define ENET_TXBD_BDU       0x00000080

// ----------------------------------------------------------------------
// RX Buffer Descriptor Bit Definitions
// Offset 0 flags - ctrl
# define ENET_RXBD_E        0x0080
# define ENET_RXBD_R01      0x0040
# define ENET_RXBD_W        0x0020
# define ENET_RXBD_R02      0x0010
# define ENET_RXBD_L        0x0008
# define ENET_RXBD_M        0x0001
# define ENET_RXBD_BC       0x8000
# define ENET_RXBD_MC       0x4000
# define ENET_RXBD_LG       0x2000
# define ENET_RXBD_NO       0x1000
# define ENET_RXBD_CR       0x0400
# define ENET_RXBD_OV       0x0200
# define ENET_RXBD_TR       0x0100

// ----------------------------------------------------------------------
// RX Enhanced BD Bit Definitions

# define ENET_RXBD_ME       0x00000080
# define ENET_RXBD_PE       0x00000004
# define ENET_RXBD_CE       0x00000002
# define ENET_RXBD_UC       0x00000001

# define ENET_RXBD_INT      0x00008000

# define ENET_RXBD_ICE      0x20000000
# define ENET_RXBD_PCR      0x10000000
# define ENET_RXBD_VLAN     0x04000000
# define ENET_RXBD_IPV6     0x02000000
# define ENET_RXBD_FRAG     0x01000000

# define ENET_RXBD_BDU      0x00000080

#else //(CYG_BYTEORDER == CYG_LSBFIRST)
//======================================================================
// Big Endian
//----------------------------------------------------------------------

// ----------------------------------------------------------------------
// TX Buffer Descriptor Bit Definitions

# define ENET_TXBD_R        0x8000
# define ENET_TXBD_TO1      0x4000
# define ENET_TXBD_W        0x2000
# define ENET_TXBD_TO2      0x1000
# define ENET_TXBD_L        0x0800
# define ENET_TXBD_TC       0x0400
# define ENET_TXBD_ABC      0x0200

// ----------------------------------------------------------------------
// TX Enhanced BD Bit Definitions

# define ENET_TXBD_INT       0x40000000
# define ENET_TXBD_TS        0x20000000
# define ENET_TXBD_PINS      0x10000000
# define ENET_TXBD_IINS      0x08000000
# define ENET_TXBD_TXE       0x00008000
# define ENET_TXBD_UE        0x00002000
# define ENET_TXBD_EE        0x00001000
# define ENET_TXBD_FE        0x00000800
# define ENET_TXBD_LCE       0x00000400
# define ENET_TXBD_OE        0x00000200
# define ENET_TXBD_TSE       0x00000100

# define ENET_TXBD_BDU       0x80000000

// ----------------------------------------------------------------------
// RX Buffer Descriptor Bit Definitions
// Offset 0 flags - ctrl
# define ENET_RXBD_E        0x8000
# define ENET_RXBD_R01      0x4000
# define ENET_RXBD_W        0x2000
# define ENET_RXBD_R02      0x1000
# define ENET_RXBD_L        0x0800
# define ENET_RXBD_M        0x0100
# define ENET_RXBD_BC       0x0080
# define ENET_RXBD_MC       0x0040
# define ENET_RXBD_LG       0x0020
# define ENET_RXBD_NO       0x0010
# define ENET_RXBD_CR       0x0004
# define ENET_RXBD_OV       0x0002
# define ENET_RXBD_TR       0x0001

// ----------------------------------------------------------------------
// RX Enhanced BD Bit Definitions

# define ENET_RXBD_ME       0x80000000
# define ENET_RXBD_PE       0x04000000
# define ENET_RXBD_CE       0x02000000
# define ENET_RXBD_UC       0x01000000
# define ENET_RXBD_INT      0x00800000
# define ENET_RXBD_ICE      0x00000020
# define ENET_RXBD_PCR      0x00000010
# define ENET_RXBD_VLAN     0x00000004
# define ENET_RXBD_IPV6     0x00000002
# define ENET_RXBD_FRAG     0x00000001

# define ENET_RXBD_BDU      0x80000000

#endif // (CYG_BYTEORDER == CYG_LSBFIRST)

// EOF freescale_enet_bd.h
#endif // CYGONCE_FREESCALE_ENET_BD_H
