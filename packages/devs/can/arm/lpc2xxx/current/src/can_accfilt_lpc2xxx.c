//==========================================================================
//
//      devs/can/arm/lpc2xxx/current/src/can_accfilt_lpc2xxx.c
//
//      Acceptance filter management for LPC2xxx CAN driver
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Gary Thomas
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
// Author(s):    Uwe Kindler
// Contributors: Uwe Kindler
// Date:         2007-05-28
// Purpose:      Support LPC2xxx on-chip CAN acceptance filters
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================


//===========================================================================
// Data types
//===========================================================================
//
// Acceptance filter entry
//
typedef struct lpc2xxx_accfilt_entry
{
    cyg_uint32 data;          // the value inclusive channel number
    cyg_uint32 id;
    cyg_uint32 lower_id_bound;
    cyg_uint32 upper_id_bound;
    cyg_uint8  channel_no;
} lpc2xxx_accfilt_entry_t;


//===========================================================================
// Declarations
//===========================================================================
//--------------------------------------------------------------------------
// On no-suffix and /00 devices, the CAN controllers are numbered 1 to n 
// (n = 2 or 4) in the LUT tables. However, on /01 devices, the CAN controllers
// are numbered 0 to nâ1 in the LUT tables.
//
// On the LPC2468 the LUT channel numbers are also numbered from 0 - 4.
//
#if defined(CYGHWR_HAL_ARM_LPC2XXX_SUFFIX_01) || (CYGHWR_HAL_ARM_LPC2XXX_VARIANT_VERSION == 4)
# define LPC2XXX_CAN_FIRST_IN_LUT   (0)
#else
# define LPC2XXX_CAN_FIRST_IN_LUT   (1) 
#endif

//
// This macro calculates the chanel number from the channel info. The channel
// number is numbered from 0 - 3 but in the LUT the channel number may differ
// depending on the device suffix. For some devices the channel number in
// LUT are numbered 0 - 3 and for other devices the channels in LUT are
// numbered 1 - 4. This macro abstrats this fact from the acceptance filter
// code
//
#define CAN_CHAN_NO_LUT(_info_) (CAN_CHAN_NO(_info_) + LPC2XXX_CAN_FIRST_IN_LUT)

//--------------------------------------------------------------------------
// Lowlevel acceptance filter access
//
#ifdef CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
static bool lpc2xxx_can_accfilt_add(lpc2xxx_can_info_t *info, 
                                    cyg_uint32          lower_id, 
                                    cyg_uint32          upper_id, 
                                    cyg_can_id_type     ext);
void lpc2xxx_can_accfilt_ram_insert_entry(cyg_uint32 TableAddress, cyg_uint16 EntryNo);
void lpc2xxx_can_accfilt_ram_remove_entry(cyg_uint32 TableAddress, cyg_uint16 EntryNo);
void lpc2xxx_can_accfilt_remove_all_ctrl_entries(lpc2xxx_can_info_t *info);
#else
static void lpc2xxx_can_accfilt_simple_rx_all(void);
#endif // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
void lpc2xxx_can_accfilt_reset(void);

#ifdef CYGDBG_DEVS_CAN_LPC2XXX_DEBUG
void lpc2xxx_can_accfilt_dbg_dump(void);
void lpc2xxx_can_reg_dump(struct cyg_devtab_entry* devtab_entry);
#endif


#ifdef CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
//===========================================================================
// Calculate address of entry in certain table
//===========================================================================
static cyg_uint32 lpc2xxx_can_accfilt_calc_entry_address(cyg_uint32 TableAddressRegister, cyg_uint16 EntryNo)
{
    cyg_uint32 EntryAddress = 0xFFFFFFFF;
    cyg_uint32 TableAddress;
    
    HAL_READ_UINT32(TableAddressRegister, TableAddress); 
    switch (TableAddressRegister) 
    {
#ifdef CYGOPT_IO_CAN_STD_CAN_ID
    case CAN_ACCFILT_SFF_SA:
         EntryAddress = ((EntryNo / 2) << 2) + TableAddress; 
         break;
             
    case CAN_ACCFILT_SFF_GRP_SA:
         EntryAddress = TableAddress + (EntryNo << 2);
         break;
#endif // CYGOPT_IO_CAN_STD_CAN_ID

#ifdef CYGOPT_IO_CAN_EXT_CAN_ID
    case CAN_ACCFILT_EFF_SA:
         EntryAddress = TableAddress + (EntryNo << 2);
         break;
                    
    case CAN_ACCFILT_EFF_GRP_SA:
         EntryAddress = TableAddress + (EntryNo << 3);
         break; 
#endif // CYGOPT_IO_CAN_EXT_CAN_ID
    default:
    	CYG_ASSERT(0, "Wrong TableAddressRegister");
    }  
    
    return EntryAddress;
}


//===========================================================================
// Remove one single entry from acceptance filter table
//===========================================================================
void lpc2xxx_can_accfilt_ram_remove_entry(cyg_uint32 Table, cyg_uint16 EntryNo)
{
    cyg_int32   remove_address = lpc2xxx_can_accfilt_calc_entry_address(Table, EntryNo);
    cyg_int32   entry_address;
    lsc_buf_t   lsc_val;
    cyg_uint8   entry_size = sizeof(cyg_uint32);
    cyg_uint32  sff_sa;
    cyg_uint32  sff_grp_sa;
    cyg_uint32  eff_sa;
    cyg_uint32  eff_grp_sa;
    cyg_int32   end_of_table;  
    
    HAL_READ_UINT32(CAN_ACCFILT_SFF_SA, sff_sa);
    HAL_READ_UINT32(CAN_ACCFILT_SFF_GRP_SA, sff_grp_sa);
    HAL_READ_UINT32(CAN_ACCFILT_EFF_SA, eff_sa);      
    HAL_READ_UINT32(CAN_ACCFILT_EFF_GRP_SA, eff_grp_sa);
    HAL_READ_UINT32(CAN_ACCFILT_ENDOFTABLE, end_of_table);
   
    //
    // Do not try to remove from an empty table
    //
    if (!end_of_table)
    {
        return;
    }
        
    entry_address = remove_address;
    
    if ((remove_address < eff_grp_sa) && (CAN_ACCFILT_EFF_GRP_SA != Table))
    {
        if ((remove_address < eff_sa) && (CAN_ACCFILT_EFF_SA != Table))
        {
            if ((remove_address < sff_grp_sa) && (CAN_ACCFILT_SFF_GRP_SA != Table))
            { 
                lsc_buf_t nextval;
                
                if (EntryNo % 2)
                {
                    HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + remove_address, lsc_val.dword);
                    HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + remove_address + sizeof(cyg_uint32), nextval.dword);
                    lsc_val.column.upper = nextval.column.lower;
                    entry_address += sizeof(cyg_uint32);
                }
                
                //
                // Start copy immediatelly after removed entry
                //   
                while (entry_address < sff_grp_sa)
                {
                    HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + entry_address, lsc_val.dword);
                    HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + entry_address + sizeof(cyg_uint32), nextval.dword);
                    lsc_val.column.lower = lsc_val.column.upper;
                    lsc_val.column.upper = nextval.column.lower;
                    HAL_WRITE_UINT32(CAN_ACCFILT_RAM_BASE + entry_address, lsc_val.dword);
                    entry_address += sizeof(cyg_uint32);
                }
                
                //
                // now check if the lower identifier is disabled - if it is disabled, then
                // also the upper identifier is invalid and we can remove the entry completely
                // if the lower identifier is not disabled, then it is valid and we need
                // to disable the upper identifier because it contains an invalid entry
                //           
                if (lsc_val.column.lower & ACCFILT_STD_DIS)
                {
                    sff_grp_sa -= sizeof(cyg_uint32);  
                    entry_address = sff_grp_sa;
                }
                else
                {
                    HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + sff_grp_sa - sizeof(cyg_uint32), lsc_val.dword);
                    lsc_val.column.upper = 0xffff;
                    HAL_WRITE_UINT32(CAN_ACCFILT_RAM_BASE + sff_grp_sa - sizeof(cyg_uint32), lsc_val.dword);
                    entry_size = 0; // we do not need to remove anything
                }
            } // if (pLine < pStdGrpStart)
            
            eff_sa -= entry_size;          
        } // if (pLine < pExtIdStart)
        
        eff_grp_sa -= entry_size;         
    } // if (pLine < pExtGrpStart)
    
    //
    // If no entry was removed then we can leave immediately without changing any
    // table pointers because we only did a change inside the sff table
    //
    if (!entry_size)
    {
#ifdef CYGDBG_DEVS_CAN_LPC2XXX_DEBUG
        lpc2xxx_can_accfilt_dbg_dump();
#endif
        return;
    }
    
    if (CAN_ACCFILT_EFF_GRP_SA == Table)
    {
        //
        // If we are in the area of extended groups then we need to remove
        // 2 lines because lower and upper identifier need 1 line each
        //  
        entry_size += sizeof(cyg_uint32);
    }
    
    end_of_table -= entry_size;
  
    //
    // Move all entries one or two dword downwards - that means we remove a line
    //
    while (entry_address < end_of_table)
    {
        HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + entry_address + entry_size, lsc_val.dword);
        HAL_WRITE_UINT32(CAN_ACCFILT_RAM_BASE + entry_address, lsc_val.dword);
        entry_address += sizeof(cyg_uint32);
    }
    
    HAL_WRITE_UINT32(CAN_ACCFILT_SFF_SA, sff_sa);
    HAL_WRITE_UINT32(CAN_ACCFILT_SFF_GRP_SA, sff_grp_sa);
    HAL_WRITE_UINT32(CAN_ACCFILT_EFF_SA, eff_sa);      
    HAL_WRITE_UINT32(CAN_ACCFILT_EFF_GRP_SA, eff_grp_sa);
    HAL_WRITE_UINT32(CAN_ACCFILT_ENDOFTABLE, end_of_table);
    
#ifdef CYGDBG_DEVS_CAN_LPC2XXX_DEBUG
    lpc2xxx_can_accfilt_dbg_dump();
#endif
}

//===========================================================================
// Insert one empty line into ram - all entries behind this line will be
// moved one entry upwards
//===========================================================================
void lpc2xxx_can_accfilt_ram_insert_entry(cyg_uint32 Table, cyg_uint16 EntryNo)
{
    cyg_int16   insert_address = lpc2xxx_can_accfilt_calc_entry_address(Table, EntryNo);
    cyg_int16   entry_address;
    cyg_int16   copy_start = insert_address;
    lsc_buf_t   lsc_val;
    cyg_uint8   entry_size = sizeof(cyg_uint32);
    cyg_uint32  sff_sa;
    cyg_uint32  sff_grp_sa;
    cyg_uint32  eff_sa;
    cyg_uint32  eff_grp_sa;
    cyg_uint32  end_of_table;
    
    
    HAL_READ_UINT32(CAN_ACCFILT_SFF_SA, sff_sa);
    HAL_READ_UINT32(CAN_ACCFILT_SFF_GRP_SA, sff_grp_sa);
    HAL_READ_UINT32(CAN_ACCFILT_EFF_SA, eff_sa);      
    HAL_READ_UINT32(CAN_ACCFILT_EFF_GRP_SA, eff_grp_sa);
    HAL_READ_UINT32(CAN_ACCFILT_ENDOFTABLE, end_of_table);
    
    if ((insert_address <= eff_grp_sa) && (CAN_ACCFILT_EFF_GRP_SA != Table))
    {
        if ((insert_address <= eff_sa) && (CAN_ACCFILT_EFF_SA != Table))
        {
            if ((insert_address <= sff_grp_sa) && (CAN_ACCFILT_SFF_GRP_SA != Table))
            {
                //
                // If we are in the range of standard identifiers then we need to
                // do some special copy procedure for this area because a standard entry
                // is only 2 byte long. Copy only til start of area with standard groups
                //
                if (sff_grp_sa)
                {
                    HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + sff_grp_sa - sizeof(cyg_uint32), lsc_val.dword); // read last entry
                    //
                    // now check if the upper identifier is disabled - if it is disabled, then
                    // we have an odd number of std ids in the list. Then we do not need to 
                    // insert a new line - we simply need to copy all entries 2 bytes upwards
                    // that means we only need to change the std id area and do not need to touch
                    // any other filter id area.
                    // If the last entry is not disabled, then we have a valid filter here.
                    // Then we need to insert a complete new line, that means we also have to move
                    // all following entries and filter tables one dword upwards.
                    //
                    if (lsc_val.words.low & ACCFILT_STD_DIS)
                    {
                        copy_start = end_of_table + sizeof(cyg_uint32); // we do not need to insert a new line and do not copy anything
                        entry_size = 0;          
                    }
                }
                
                if (entry_size)
                {
                    copy_start = sff_grp_sa;          // copy everything behind std id group
                    sff_grp_sa += entry_size;   
                }
            } // if (pLine < pStdGrpStart)
            
            eff_sa += entry_size;
        } // if (pLine < pExtIdStart)
        
        eff_grp_sa += entry_size;
    } // if (pLine < pExtGrpStart)
          
    if (CAN_ACCFILT_EFF_GRP_SA == Table)
    {
        //
        // If we are in the area of extended groups then we need to insert
        // 2 lines because lower and upper identifier need 1 line each
        //
        entry_size += sizeof(cyg_uint32); // one entry is 2 dword long
    }
    
    entry_address = end_of_table - sizeof(cyg_uint32);
    end_of_table  += entry_size;  // add one additional entry
        
    //
    // Move all entries one or two dwords upwards - that means we insert a new empty line
    //
    while (entry_address >= copy_start)
    {
        HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + (cyg_uint32)entry_address, lsc_val.dword);
        HAL_WRITE_UINT32(CAN_ACCFILT_RAM_BASE + (cyg_uint32)entry_address + entry_size, lsc_val.dword);
        entry_address -= sizeof(cyg_uint32);  
    }
    
    //
    // For the std ID area we need a special procedure
    //
    if (CAN_ACCFILT_SFF_SA == Table)
    {
        lsc_buf_t preval;
        //
        // Start copy with last entry of std id table
        //
        entry_address = sff_grp_sa - sizeof(cyg_uint32);    
                
        while (entry_address > insert_address)
        {
            HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + (cyg_uint32)entry_address, lsc_val.dword);
            HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + (cyg_uint32)entry_address - sizeof(cyg_uint32), preval.dword);
            lsc_val.column.upper = lsc_val.column.lower;
            lsc_val.column.lower = preval.column.upper;
            HAL_WRITE_UINT32(CAN_ACCFILT_RAM_BASE + (cyg_uint32)entry_address, lsc_val.dword);
            entry_address -= sizeof(cyg_uint32);
        }
        
        //
        // If we insert an entry into the lower column, then we need to move the
        // content of the lower column into the upper column
        //
        if (!(EntryNo % 2))
        {
            HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + (cyg_uint32)insert_address, lsc_val.dword);
            lsc_val.column.upper = lsc_val.column.lower;
            HAL_WRITE_UINT32(CAN_ACCFILT_RAM_BASE + (cyg_uint32)insert_address, lsc_val.dword);
        }
        
        //
        // If we inserted a new line, then we have an odd number of identifiers now
        // and need to disable the last (the upper) entry
        //
        if (entry_size)
        {
            HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + sff_grp_sa - sizeof(cyg_uint32) , lsc_val.dword);
            lsc_val.column.upper = 0xFFFF;  // disable the entry
            HAL_WRITE_UINT32(CAN_ACCFILT_RAM_BASE + sff_grp_sa - sizeof(cyg_uint32) , lsc_val.dword);
        }
    }
    
    HAL_WRITE_UINT32(CAN_ACCFILT_SFF_SA, sff_sa);
    HAL_WRITE_UINT32(CAN_ACCFILT_SFF_GRP_SA, sff_grp_sa);
    HAL_WRITE_UINT32(CAN_ACCFILT_EFF_SA, eff_sa);      
    HAL_WRITE_UINT32(CAN_ACCFILT_EFF_GRP_SA, eff_grp_sa);
    HAL_WRITE_UINT32(CAN_ACCFILT_ENDOFTABLE, end_of_table);
}


//===========================================================================
// Query number of entries in a certain table
//===========================================================================
static cyg_uint16 lpc2xxx_can_accfilt_get_table_entries(cyg_uint32 TableStartAddress)
{
    cyg_uint32  start;
    cyg_uint32  end;
    
    switch (TableStartAddress)
    {
#ifdef CYGOPT_IO_CAN_STD_CAN_ID
    case CAN_ACCFILT_SFF_SA:
         HAL_READ_UINT32(CAN_ACCFILT_SFF_SA,     start);
         HAL_READ_UINT32(CAN_ACCFILT_SFF_GRP_SA, end);
         if (end - start)
         {
             lsc_buf_t data;
             HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + end - sizeof(cyg_uint32), data.dword);
             if (data.column.upper & ACCFILT_STD_DIS)
             {
                 return (((end - start) >> 1) - 1);
             }
         }
         return (end - start) >> 1;
         
    case CAN_ACCFILT_SFF_GRP_SA:
         HAL_READ_UINT32(CAN_ACCFILT_SFF_GRP_SA, start);
         HAL_READ_UINT32(CAN_ACCFILT_EFF_SA,     end);
         return (end - start) >> 2;
#endif // CYGOPT_IO_CAN_STD_CAN_ID

#ifdef CYGOPT_IO_CAN_EXT_CAN_ID        
    case CAN_ACCFILT_EFF_SA:
         HAL_READ_UINT32(CAN_ACCFILT_EFF_SA,     start);
         HAL_READ_UINT32(CAN_ACCFILT_EFF_GRP_SA, end);
         return (end - start) >> 2;
                 
    case CAN_ACCFILT_EFF_GRP_SA:
         HAL_READ_UINT32(CAN_ACCFILT_EFF_GRP_SA, start);
         HAL_READ_UINT32(CAN_ACCFILT_ENDOFTABLE, end);
         return (end - start) >> 3;
#endif // CYGOPT_IO_CAN_EXT_CAN_ID
    default:
        CYG_FAIL("Invalid identifier table address");
        return 0;         
    } // switch (TableStartAddress)
}

//===========================================================================
// Query certain entry from table
//===========================================================================
static void lpc2xxx_can_accfilt_get_entry(cyg_uint32 TableStartAddress, cyg_uint16 EntryNo, lpc2xxx_accfilt_entry_t *pEntry)
{
    cyg_uint32 EntryAddress = lpc2xxx_can_accfilt_calc_entry_address(TableStartAddress, EntryNo);
    lsc_buf_t  Data;

    HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + EntryAddress, Data.dword);
    pEntry->data = Data.dword;
    switch (TableStartAddress)
    {
#ifdef CYGOPT_IO_CAN_STD_CAN_ID
    case CAN_ACCFILT_SFF_SA:
    {
         cyg_uint16 column; 
         if (EntryNo % 2)
         {
             column = Data.column.upper;
         }
         else
         {
             column = Data.column.lower; 
         }
         pEntry->id         = ACCFILT_STD_GET_ID(column);
         pEntry->channel_no = ACCFILT_STD_GET_CTRL(column);
    }
    break;
                 
    case CAN_ACCFILT_SFF_GRP_SA:
         pEntry->lower_id_bound = ACCFILT_STD_GET_ID(Data.column.lower);
         pEntry->upper_id_bound = ACCFILT_STD_GET_ID(Data.column.upper);
         pEntry->channel_no     = ACCFILT_STD_GET_CTRL(Data.column.lower);
         break;
#endif // #ifdef CYGOPT_IO_CAN_STD_CAN_ID
    
#ifdef CYGOPT_IO_CAN_EXT_CAN_ID
    case CAN_ACCFILT_EFF_SA:
         pEntry->id         = ACCFILT_EXT_GET_ID(Data.dword);
         pEntry->channel_no = ACCFILT_EXT_GET_CTRL(Data.dword);
         break;
                        
    case CAN_ACCFILT_EFF_GRP_SA:
         pEntry->lower_id_bound = ACCFILT_EXT_GET_ID(Data.dword);
         pEntry->channel_no     = ACCFILT_EXT_GET_CTRL(Data.dword);
         HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE+ EntryAddress + sizeof(cyg_uint32), Data.dword);
         pEntry->upper_id_bound = ACCFILT_EXT_GET_ID(Data.dword);
         break;
#endif // #ifedf CYGOPT_IO_CAN_EXT_CAN_ID
    default:
        CYG_FAIL("Invalid identifier table address");
    } // switch ()
}


//===========================================================================
// Set certain entry in table
//===========================================================================
static void lpc2xxx_can_accfilt_set_entry(cyg_uint32 TableStartAddress, cyg_uint16 EntryNo, lpc2xxx_accfilt_entry_t *pEntry)
{
    cyg_uint32 EntryAddress = lpc2xxx_can_accfilt_calc_entry_address(TableStartAddress, EntryNo);
    lsc_buf_t Data;

    switch (TableStartAddress)
    {
#ifdef CYGOPT_IO_CAN_STD_CAN_ID
    case CAN_ACCFILT_SFF_SA:
         {
             HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + EntryAddress, Data.dword);            
             if (EntryNo % 2)
             {
                 Data.column.upper = (pEntry->channel_no << 13) | (pEntry->id & ACCFILT_STD_ID_MASK);
             }
             else
             {
                 Data.column.lower = (pEntry->channel_no << 13) | (pEntry->id & ACCFILT_STD_ID_MASK);
             }
         }
         break;
                 
    case CAN_ACCFILT_SFF_GRP_SA:
         Data.column.lower = (pEntry->channel_no << 13) | (pEntry->lower_id_bound & ACCFILT_STD_ID_MASK);
         Data.column.upper = (pEntry->channel_no << 13) | (pEntry->upper_id_bound & ACCFILT_STD_ID_MASK);
         break;
#endif // #ifdef CYGOPT_IO_CAN_STD_CAN_ID

#ifdef CYGOPT_IO_CAN_EXT_CAN_ID
    case CAN_ACCFILT_EFF_SA:
         Data.dword = (pEntry->channel_no << 29) | (pEntry->id & ACCFILT_EXT_ID_MASK); 
         break;
                        
    case CAN_ACCFILT_EFF_GRP_SA:
         {
             lsc_buf_t Data2;
             
             Data.dword  = (pEntry->channel_no << 29) | (pEntry->lower_id_bound & ACCFILT_EXT_ID_MASK);
             Data2.dword = (pEntry->channel_no << 29) | (pEntry->upper_id_bound & ACCFILT_EXT_ID_MASK);
             HAL_WRITE_UINT32(CAN_ACCFILT_RAM_BASE + EntryAddress + sizeof(cyg_uint32), Data2.dword);
         }
         break;
#endif // #ifdef CYGOPT_IO_CAN_EXT_CAN_ID
         
    default:
        CYG_FAIL("Invalid identifier table address");
    } // switch ()
    
    HAL_WRITE_UINT32(CAN_ACCFILT_RAM_BASE + EntryAddress, Data.dword);
}


//===========================================================================
// Add one entry to acceptance filter RAM
// If upper ID is > lower ID then we have to add a group filter - else we
// have to add a single message filter here
//===========================================================================
static bool lpc2xxx_can_accfilt_add(lpc2xxx_can_info_t *info, 
                                    cyg_uint32          lower_id, 
                                    cyg_uint32          upper_id, 
                                    cyg_can_id_type     ext)
{
    cyg_uint32              accfilt_bck; // acceptance filter backup
    cyg_uint32              end_of_table;
    cyg_uint32              table;
    lpc2xxx_accfilt_entry_t entry;
    lpc2xxx_accfilt_entry_t new_entry;
    
    
    //
    // first step: disable acceptance filter and prepare it for modification
    //
    HAL_READ_UINT32(CAN_ACCFILT_AFMR, accfilt_bck);
    HAL_WRITE_UINT32(CAN_ACCFILT_AFMR, AFMR_OFF | AFMR_BYPASS);
    
    //
    // Check if table is full
    //
    HAL_READ_UINT32(CAN_ACCFILT_ENDOFTABLE, end_of_table);
    if (end_of_table >= ACCFILT_RAM_SIZE)
    {
        return false;   
    }
    
    new_entry.id             = lower_id;
    new_entry.lower_id_bound = lower_id;
    new_entry.upper_id_bound = upper_id;
    
    //
    // Here we rely on the ISR vector ordering for calculation of channel number
    // Maybe this is not the right way for newer LPC parts
    //
    new_entry.channel_no = CAN_CHAN_NO_LUT(info);
    
    //
    // If lower_id == upper_id then we know that we have to setup a single message filter 
    // here. If it is not equal the it is group of identifiers to receive
    //
    if ((lower_id == upper_id) || (lower_id > upper_id))
    {
        //
        // setup single message filter (standard or extended) here
        //
#ifdef CYGOPT_IO_CAN_EXT_CAN_ID
        if (ext)
        {
            table = CAN_ACCFILT_EFF_SA;
        }
        else
#endif // #ifdef CYGOPT_IO_CAN_EXT_CAN_ID
        {
#ifdef CYGOPT_IO_CAN_STD_CAN_ID
            table = CAN_ACCFILT_SFF_SA;
#endif // CYGOPT_IO_CAN_STD_CAN_ID
        }
    }
    else
    {
        //
        // setup single message filter (standard or extended) here
        //
#ifdef CYGOPT_IO_CAN_EXT_CAN_ID
        if (ext)
        {
            table = CAN_ACCFILT_EFF_GRP_SA;
        }
        else
#endif // #ifdef CYGOPT_IO_CAN_EXT_CAN_ID
        {
#ifdef CYGOPT_IO_CAN_STD_CAN_ID
            table = CAN_ACCFILT_SFF_GRP_SA;
#endif // #ifdef CYGOPT_IO_CAN_STD_CAN_ID
        }
    }
        
    cyg_uint16 entries = lpc2xxx_can_accfilt_get_table_entries(table);
    cyg_uint16 i;
    
           
    for (i = 0; i < entries; ++i)
    {
        lpc2xxx_can_accfilt_get_entry(table, i, &entry);

        if (entry.channel_no > new_entry.channel_no)
        {
            break;
        }
        
        if ((entry.channel_no == new_entry.channel_no)
        &&  (entry.id > new_entry.id))
        {
            break;
        }
    } // for (i = 0; i < entries; ++i)
    
    lpc2xxx_can_accfilt_ram_insert_entry(table, i);
    lpc2xxx_can_accfilt_set_entry(table, i, &new_entry);

    //
    // finally restore the previous state of the acceptance filter
    //
    HAL_WRITE_UINT32(CAN_ACCFILT_AFMR, accfilt_bck);
    return true;
}


//===========================================================================
// Remove all entries from a certain controller
//===========================================================================
void lpc2xxx_can_accfilt_remove_all_ctrl_entries(lpc2xxx_can_info_t *info)
{
    cyg_uint32               accfilt_bck; // acceptance filter backup
    cyg_uint16               i;
    cyg_uint16               entries;
    cyg_uint32               TableStartAddress = CAN_ACCFILT_SFF_SA;
    lpc2xxx_accfilt_entry_t  Entry;
    cyg_uint8                channel_no = CAN_CHAN_NO_LUT(info);
    cyg_uint16               entry_idx;
    
    //
    // first step: disable acceptance filter and prepare it for modification
    //
    HAL_READ_UINT32(CAN_ACCFILT_AFMR, accfilt_bck);
    HAL_WRITE_UINT32(CAN_ACCFILT_AFMR, AFMR_OFF | AFMR_BYPASS);
    
    //
    // now remove all entries for a certain controller
    //
    for (TableStartAddress = CAN_ACCFILT_SFF_SA; TableStartAddress < CAN_ACCFILT_ENDOFTABLE; TableStartAddress += 4)
    {
        entries = lpc2xxx_can_accfilt_get_table_entries(TableStartAddress);
        entry_idx = 0;
        for (i = 0; i < entries; ++i)
        {
            lpc2xxx_can_accfilt_get_entry(TableStartAddress, entry_idx, &Entry);
            if (Entry.channel_no == channel_no)
            {
               lpc2xxx_can_accfilt_ram_remove_entry(TableStartAddress, entry_idx);
            }
            else
            {
                entry_idx++;
            }
        } // for (i = 0; i < entries; ++i)
    } // for (TableStartAddress = CAN_ACCFILT_SFF_SA ...
    
    //
    // finally restore the previous state of the acceptance filter
    //
    HAL_WRITE_UINT32(CAN_ACCFILT_AFMR, accfilt_bck);
}
#endif // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG


#ifndef CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
//===========================================================================
// Setup reception of all CAN identifiers
// If runtime acceptance filter configuration is not required then we simply
// setup the acceptance filter here to receive all CAN identifiers
//===========================================================================
static void lpc2xxx_can_accfilt_simple_rx_all(void)
{
    cyg_uint32                regval;
    
    //
    // First check if it is really necessary to setup filters. If end of table is
    // != 0 then the acceptance filter is already setup properly
    //
    HAL_READ_UINT32(CAN_ACCFILT_ENDOFTABLE, regval);
    if (regval)
    {
        return;
    }
    
    cyg_uint32                accfilt_bck;         // acceptance filter backup
    cyg_uint8                 i = 0;               // loop counter
    lsc_buf_t                 accfilt_entry;       // std group entry
#ifdef CYGOPT_IO_CAN_STD_CAN_ID
    cyg_uint8                 std_address = 0;     // std group entry address
#ifdef CYGOPT_IO_CAN_EXT_CAN_ID
    cyg_uint8                 ext_address = lpc2xxx_global_can_info.init_cnt << 2;
#endif
#else
    cyg_uint8                 ext_address = 0;
#endif
    
    //
    // first step: disable acceptance filter and prepare it for modification
    //
    HAL_READ_UINT32(CAN_ACCFILT_AFMR, accfilt_bck);
    HAL_WRITE_UINT32(CAN_ACCFILT_AFMR, AFMR_OFF | AFMR_BYPASS);
    
    //
    // Write table start adresses - we use only standard group and extended filter
    // group
    //
    HAL_WRITE_UINT32(CAN_ACCFILT_SFF_SA,     0);
    HAL_WRITE_UINT32(CAN_ACCFILT_SFF_GRP_SA, 0);
    HAL_WRITE_UINT32(CAN_ACCFILT_EFF_SA,     ext_address);
    HAL_WRITE_UINT32(CAN_ACCFILT_EFF_GRP_SA, ext_address);
    
    //
    // Now loop through all active CAN channels and setup the acceptance filter for
    // each channel to receive all standard and extended CAN identifiers
    //
    while (lpc2xxx_global_can_info.active_channels[i])
    {
        lpc2xxx_can_info_t *info = (lpc2xxx_can_info_t *)lpc2xxx_global_can_info.active_channels[i++]->dev_priv; 
        cyg_uint8           channel_no = CAN_CHAN_NO_LUT(info);

#ifdef CYGOPT_IO_CAN_STD_CAN_ID
        accfilt_entry.column.lower = (channel_no << 13) | (0x000 & ACCFILT_STD_ID_MASK);
        accfilt_entry.column.upper = (channel_no << 13) | (0x7FF & ACCFILT_STD_ID_MASK);        
        HAL_WRITE_UINT32(CAN_ACCFILT_RAM_BASE + std_address, accfilt_entry.dword);   
        std_address += sizeof(cyg_uint32);
#endif // CYGOPT_IO_CAN_STD_CAN_ID

#ifdef CYGOPT_IO_CAN_EXT_CAN_ID
        accfilt_entry.dword = (channel_no << 29) | (0x00000000 & ACCFILT_EXT_ID_MASK);
        HAL_WRITE_UINT32(CAN_ACCFILT_RAM_BASE +  ext_address, accfilt_entry.dword);
        ext_address += sizeof(cyg_uint32);
        accfilt_entry.dword = (channel_no << 29) | (0x1FFFFFFF & ACCFILT_EXT_ID_MASK);
        HAL_WRITE_UINT32(CAN_ACCFILT_RAM_BASE +  ext_address, accfilt_entry.dword);
        ext_address += sizeof(cyg_uint32);
#endif // CYGOPT_IO_CAN_EXT_CAN_ID
    } // while (lpc2xxx_global_can_info.active_channels[i])
       
    //
    // finally store end of table value and restore the previous state of the 
    // acceptance filter
    //
    HAL_WRITE_UINT32(CAN_ACCFILT_ENDOFTABLE, ext_address);
    HAL_WRITE_UINT32(CAN_ACCFILT_AFMR, accfilt_bck);
}
#endif // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG


//===========================================================================
// Reset acceptance filter to poweron defaults
//===========================================================================
void lpc2xxx_can_accfilt_reset(void)
{
    cyg_uint32  accfilt_bck; // acceptance filter backup
    //
    // first step: disable acceptance filter and prepare it for modification
    //
    HAL_READ_UINT32(CAN_ACCFILT_AFMR, accfilt_bck);
    HAL_WRITE_UINT32(CAN_ACCFILT_AFMR, AFMR_OFF | AFMR_BYPASS);
    
    //
    // Now write zero to all addresses of acceptance filter table
    //
    HAL_WRITE_UINT32(CAN_ACCFILT_SFF_SA,     0);
    HAL_WRITE_UINT32(CAN_ACCFILT_SFF_GRP_SA, 0);
    HAL_WRITE_UINT32(CAN_ACCFILT_EFF_SA,     0);
    HAL_WRITE_UINT32(CAN_ACCFILT_EFF_GRP_SA, 0);
    HAL_WRITE_UINT32(CAN_ACCFILT_ENDOFTABLE, 0);
    
    //
    // finally restore the previous state of the acceptance filter
    //
    HAL_WRITE_UINT32(CAN_ACCFILT_AFMR, accfilt_bck);
}


//===========================================================================
// Dump content of acceptance filter lookup table
//===========================================================================
#ifdef CYGDBG_DEVS_CAN_LPC2XXX_DEBUG
void lpc2xxx_can_accfilt_dbg_dump(void)
{
    cyg_uint32          sff_sa;
    cyg_uint32          sff_grp_sa;
    cyg_uint32          eff_sa;
    cyg_uint32          eff_grp_sa;
    cyg_uint32          end_of_table;
    cyg_uint32          entry_address;
    lsc_buf_t           data;
    
    
    HAL_READ_UINT32(CAN_ACCFILT_SFF_SA, sff_sa);
    HAL_READ_UINT32(CAN_ACCFILT_SFF_GRP_SA, sff_grp_sa);
    HAL_READ_UINT32(CAN_ACCFILT_EFF_SA, eff_sa);      
    HAL_READ_UINT32(CAN_ACCFILT_EFF_GRP_SA, eff_grp_sa);
    HAL_READ_UINT32(CAN_ACCFILT_ENDOFTABLE, end_of_table);
    
    entry_address = sff_sa;
    
    //
    // Print lookup table registers
    //
    diag_printf("\n\nDUMP CAN ACCEPTANCE FILTER REGISTERS\n");
    diag_printf("----------------------------------------\n");
    diag_printf("SFF_sa:\t\t0x%08x\n", sff_sa);
    diag_printf("SFF_GRP_sa:\t0x%08x\n", sff_grp_sa);
    diag_printf("EFF_sa:\t\t0x%08x\n", eff_sa);
    diag_printf("EFF_GRP_sa:\t0x%08x\n", eff_grp_sa);
    diag_printf("EOT:\t\t0x%08x\n", end_of_table);
    
    //
    // Print table of standard identifiers
    //
    diag_printf("\n\nDUMP CAN LOOKUP TABLE RAM");
    diag_printf("\nSFF_sa\t\tcolumn_lower\tcolumn_upper\traw_data\n");
    diag_printf("----------------------------------------------------------\n");
    while (entry_address < sff_grp_sa)
    {
        HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + entry_address, data.dword);
        diag_printf("0x%04x:\t\t0x%x\t\t0x%x\t\t0x%x\n", entry_address, data.column.lower, data.column.upper, data.dword);
        entry_address += sizeof(cyg_uint32);   
    }
    
    //
    // Print table of standard identifier groups
    //
    diag_printf("\nSFF_GRP_sa\tcolumn_lower\tcolumn_upper\traw_data\n");
    diag_printf("----------------------------------------------------------\n");
    while (entry_address < eff_sa)
    {  
        HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + entry_address, data.dword);
        diag_printf("0x%04x:\t\t0x%x\t\t0x%x\t\t0x%x\n", entry_address, data.column.lower, data.column.upper, data.dword);
        entry_address += sizeof(cyg_uint32); 
    }
    
    //
    // Print table of extended identifiers
    //
    diag_printf("\nEFF_sa\t\t-\t\t-\t\traw_data\n");
    diag_printf("----------------------------------------------------------\n");
    while (entry_address < eff_grp_sa)
    {
        HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + entry_address, data.dword);
        diag_printf("0x%04x:\t\t\t\t\t\t0x%x\n", entry_address, data.dword);
        entry_address += sizeof(cyg_uint32);
    }
    
    //
    // Print table of extended identifier groups
    //
    diag_printf("\nEFF_GRP_sa\t-\t\t-\t\traw_data\n");
    diag_printf("----------------------------------------------------------\n");
    while (entry_address < end_of_table)
    {
        HAL_READ_UINT32(CAN_ACCFILT_RAM_BASE + entry_address, data.dword);
        diag_printf("0x%04x:\t\t\t\t\t\t0x%x\n", entry_address, data.dword);
        entry_address += sizeof(cyg_uint32);    
    }
}
#endif // CYGDBG_DEVS_CAN_LPC2XXX_DEBUG

//===========================================================================
// Dump content of acceptance filter lookup table
//===========================================================================
#ifdef CYGDBG_DEVS_CAN_LPC2XXX_DEBUG
void lpc2xxx_can_reg_dump(struct cyg_devtab_entry* devtab_entry)
{
    can_channel *chan    = (can_channel*)devtab_entry->priv;
    cyg_uint32   reg_val;
    CAN_DECLARE_INFO(chan);
    
    chan = chan; // avoid compiler warnings for unused variables
    //
    // Print table of extended identifier groups
    //
    diag_printf("\n\nCAN REGISTER DUMP\n");
    diag_printf("\nRegister\tValue\n");
    diag_printf("----------------------------------------------------------\n"); 
    HAL_READ_UINT32(CAN_CTRL_MOD(info), reg_val);
    diag_printf("CANMOD\t\t0x%08x\n", reg_val);
    HAL_READ_UINT32(CAN_CTRL_CMR(info), reg_val);
    diag_printf("CANCMR\t\t0x%08x\n", reg_val);
    HAL_READ_UINT32(CAN_CTRL_GSR(info), reg_val);
    diag_printf("CANGSR\t\t0x%08x\n", reg_val);
    HAL_READ_UINT32(CAN_CTRL_ICR(info), reg_val);
    diag_printf("CANICR\t\t0x%08x\n", reg_val);
    HAL_READ_UINT32(CAN_CTRL_IER(info), reg_val);
    diag_printf("CANIER\t\t0x%08x\n", reg_val);
    HAL_READ_UINT32(CAN_CTRL_BTR(info), reg_val);
    diag_printf("CANBTR\t\t0x%08x\n", reg_val);
    HAL_READ_UINT32(CAN_CTRL_EWL(info), reg_val);
    diag_printf("CANEWL\t\t0x%08x\n", reg_val);
    HAL_READ_UINT32(CAN_CTRL_SR(info), reg_val);
    diag_printf("CANSR\t\t0x%08x\n", reg_val);
    HAL_READ_UINT32(CAN_CTRL_RFS(info), reg_val);
    diag_printf("CANRFS\t\t0x%08x\n", reg_val);
    HAL_READ_UINT32(CAN_CTRL_RID(info), reg_val);
    diag_printf("CANRID\t\t0x%08x\n", reg_val);
    HAL_READ_UINT32(CAN_CTRL_RDA(info), reg_val);
    diag_printf("CANRDA\t\t0x%08x\n", reg_val);
    HAL_READ_UINT32(CAN_CTRL_RDB(info), reg_val);
    diag_printf("CANRDB\t\t0x%08x\n", reg_val);
    
    diag_printf("\n\nCAN CENTRAL REGISTER DUMP\n");
    diag_printf("\nRegister\tValue\n");
    diag_printf("----------------------------------------------------------\n");
    HAL_READ_UINT32(CAN_CENTRAL_TXSR, reg_val);
    diag_printf("CANTxSR\t\t0x%08x\n", reg_val);
    HAL_READ_UINT32(CAN_CENTRAL_RXSR, reg_val);
    diag_printf("CANRxSR\t\t0x%08x\n", reg_val);
    HAL_READ_UINT32(CAN_CENTRAL_MSR, reg_val);
    diag_printf("CANMSR\t\t0x%08x\n", reg_val);
    
    diag_printf("\n\nCAN ACCEPTANCE FILTER REGISTER DUMP\n");
    diag_printf("\nRegister\tValue\n");
    diag_printf("----------------------------------------------------------\n");
    HAL_READ_UINT32(CAN_ACCFILT_AFMR, reg_val);
    diag_printf("AFMR\t\t0x%08x\n", reg_val); 
    HAL_READ_UINT32(CAN_ACCFILT_LUT_ERR, reg_val);
    diag_printf("LUTERR\t\t0x%08x\n", reg_val); 
    HAL_READ_UINT32(CAN_ACCFILT_LUT_ERR_ADDR, reg_val);
    diag_printf("LUTERRADDR\t0x%08x\n", reg_val); 
}
#endif // #ifdef CYGDBG_DEVS_CAN_LPC2XXX_DEBUG

//---------------------------------------------------------------------------
// EOF can_accfilt_lpc2xxx.c
