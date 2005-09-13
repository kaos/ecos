//==========================================================================
//
//        can_test_aux.inl
//
//        CAN test auxiliary functions
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Uwe Kindler
// Contributors:  Uwe Kindler
// Date:          2005-08-07
// Description:   Auxiliary functions for CAN driver tests
//####DESCRIPTIONEND####


//===========================================================================
//                           PRINT CAN EVENT
//===========================================================================
void print_can_msg(cyg_can_message *pmsg, char *pMsg)
{   
    char *pmsg_str;
    static char* msg_tbl[] =
    {
        "%s [ID:%03X] [RTR:%d] [EXT:%d] [DATA:]\n",
        "%s [ID:%03X] [RTR:%d] [EXT:%d] [DATA:%02X]\n",
        "%s [ID:%03X] [RTR:%d] [EXT:%d] [DATA:%02X %02X]\n",
        "%s [ID:%03X] [RTR:%d] [EXT:%d] [DATA:%02X %02X %02X]\n",
        "%s [ID:%03X] [RTR:%d] [EXT:%d] [DATA:%02X %02X %02X %02X]\n",
        "%s [ID:%03X] [RTR:%d] [EXT:%d] [DATA:%02X %02X %02X %02X %02X]\n",
        "%s [ID:%03X] [RTR:%d] [EXT:%d] [DATA:%02X %02X %02X %02X %02X %02X]\n",
        "%s [ID:%03X] [RTR:%d] [EXT:%d] [DATA:%02X %02X %02X %02X %02X %02X %02X]\n",
        "%s [ID:%03X] [RTR:%d] [EXT:%d] [DATA:%02X %02X %02X %02X %02X %02X %02X %02X]\n"
    };
    
    if (pmsg->rtr)
    {
        diag_printf("%s [ID:%03X] [RTR:%d] [EXT:%d]\n",
                    pMsg,
                    pmsg->id,
                    pmsg->rtr,
                    pmsg->ext);
                    
        return;
    }
    
    if (pmsg->dlc > 8)
    {
        pmsg_str = msg_tbl[8];
    }   
    else
    {
        pmsg_str = msg_tbl[pmsg->dlc];
    } 
    
    diag_printf(pmsg_str,
                pMsg,
                pmsg->id,
                pmsg->rtr,
                pmsg->ext,
                pmsg->data[0],
                pmsg->data[1],
                pmsg->data[2],
                pmsg->data[3],
                pmsg->data[4],
                pmsg->data[5],
                pmsg->data[6],
                pmsg->data[7]);
}


//===========================================================================
//                         PRINT CAN EVENT FLAGS
//===========================================================================
void print_can_flags(cyg_uint16 flags, char *pMsg)
{
    char      *pmsg_str;
    cyg_uint8  i ;
    static char* msg_tbl[] =
    {
        "RX  ",
        "TX  ",
        "WRX  ",
        "WTX  ",
        "ERRP  ",
        "BOFF  ",
        "OVRX  ",
        "OVTX  ",
        "CERR  ",
        "LSTY  ",
        "ESTY  ",
        "ALOS  ",
        "DEVC  ",
        "PHYF  ",
        "PHYH  ",
        "PHYL  "
    };
    i = 0;
    while (flags && (i < 16))
    {
        if (flags & 0x0001)
        {
            pmsg_str = msg_tbl[i];
            diag_printf(pmsg_str);
        }
        flags >>=1;
        i++;
    }
    
    diag_printf("\n");
}

//---------------------------------------------------------------------------
// end of can_test_aux.inl
