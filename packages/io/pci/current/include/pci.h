#ifndef CYGONCE_PCI_H
#define CYGONCE_PCI_H
//=============================================================================
//
//      pci.h
//
//      PCI library
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jskov, from design by nickg 
// Contributors: jskov
// Date:         1999-08-09
// Purpose:      PCI configuration access
// Usage:
//              #include <cyg/io/pci.h>
// Description: 
//             This library provides a set of routines for accessing 
//             the PCI bus configuration space in a portable manner. 
//             This is provided by two APIs. The high level API (defined
//             by this file) is used by device drivers, or other code, to
//             access the PCI configuration space portably. The low level
//             API (see pci_hw.h) is used  by the PCI library itself
//             to access the hardware in a platform-specific manner and
//             may also be used by device drivers to access the PCI
//             configuration space directly.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/io/pci_cfg.h>
#include <cyg/io/pci_hw.h>

//------------------------------------------------------------------
// The PCI memory space can span 64 bits, IO space only 32 bits
typedef CYG_WORD64 CYG_PCI_ADDRESS64;
typedef CYG_WORD32 CYG_PCI_ADDRESS32;

//------------------------------------------------------------------
// Macros for manufacturing and decomposing device ids
typedef CYG_WORD32 cyg_pci_device_id;           // PCI device ID

#define CYG_PCI_DEV_MAKE_ID(__bus,__devfn) (((__bus)<<16)|((__devfn)<<8))
#define CYG_PCI_DEV_GET_BUS(__devid) ((__devid>>16)&0xFF)
#define CYG_PCI_DEV_GET_DEVFN(__devid) ((__devid>>8)&0xFF)

#define CYG_PCI_NULL_DEVID 0xffffffff
#define CYG_PCI_NULL_DEVFN 0xffff

//------------------------------------------------------------------
// PCI device data definitions and structures

typedef enum { 
    CYG_PCI_HEADER_NORMAL = 0, 
    CYG_PCI_HEADER_BRIDGE = 1,
    CYG_PCI_HEADER_CARDBUS_BRIDGE = 2
} cyg_pci_header_type;

typedef struct                          // PCI device data
{
    cyg_pci_device_id devid;            // ID of this device

    // The following fields are read out of the
    // config space for this device.

    cyg_uint16  vendor;                 // vendor ID
    cyg_uint16  device;                 // device ID
    cyg_uint16  command;                // command register
    cyg_uint16  status;                 // status register
    cyg_uint32  class_rev;              // class+revision
    cyg_uint8   cache_line_size;        // cache line size
    cyg_uint8   latency_timer;          // latency timer
    cyg_pci_header_type header_type;    // header type
    cyg_uint8   bist;                   // Built-in Self-Test
    cyg_uint32  base_address[6];        // Memory base address registers

    // The following fields are used by the resource allocation
    // routines to keep track of allocated resources.
    cyg_uint32 num_bars;
        
    cyg_uint32 base_size[6];            // Memory size for each base address
    cyg_uint32 base_map[6];             // Physical address mapped

    CYG_ADDRWORD hal_vector;            // HAL interrupt vector used by
                                        // device if int_line!=0

    // One of the following unions will be filled in according to
    // the value of the header_type field.

    union
    {
        struct
        {
            cyg_uint32  cardbus_cis;    // CardBus CIS Pointer
            cyg_uint16  sub_vendor;     // subsystem vendor id
            cyg_uint16  sub_id;         // subsystem id
            cyg_uint32  rom_address;    // ROM address register
            cyg_uint8   cap_list;       // capability list
            cyg_uint8   reserved1[7];
            cyg_uint8   int_line;       // interrupt line
            cyg_uint8   int_pin;        // interrupt pin
            cyg_uint8   min_gnt;        // timeslice request
            cyg_uint8   max_lat;        // priority-level request
        } normal;
        struct
        {
	    cyg_uint8	pri_bus;		// primary bus number
	    cyg_uint8	sec_bus;		// secondary bus number
	    cyg_uint8	sub_bus;		// subordinate bus number
	    cyg_uint8	sec_latency_timer;	// secondary bus latency
	    cyg_uint8	io_base;
	    cyg_uint8	io_limit;
	    cyg_uint16	sec_status;		// secondary bus status
	    cyg_uint16	mem_base;
	    cyg_uint16	mem_limit;
	    cyg_uint16	prefetch_base;
	    cyg_uint16	prefetch_limit;
	    cyg_uint32	prefetch_base_upper32;
	    cyg_uint32	prefetch_limit_upper32;	
	    cyg_uint16	io_base_upper16;
	    cyg_uint16	io_limit_upper16;
            cyg_uint8   reserved1[4];
            cyg_uint32  rom_address;    	// ROM address register
            cyg_uint8   int_line;       	// interrupt line
            cyg_uint8   int_pin;        	// interrupt pin
	    cyg_uint16	control;		// bridge control
        } bridge;
        struct
        {
            // Not yet supported
        } cardbus_bridge;
    } header;
} cyg_pci_device;
    
//------------------------------------------------------------------------
// Init
externC void cyg_pci_init( void );

//------------------------------------------------------------------------
// Common device configuration access functions

// This function gets the PCI configuration information for the
// device indicated in devid. The common fields of the cyg_pci_device
// structure, and the appropriate fields of the relevant header union
// member are filled in from the device's configuration space. If the
// device has not been enabled, then this function will also fetch
// the size and type information from the base address registers and
// place it in the base_size[] array.
externC void cyg_pci_get_device_info ( cyg_pci_device_id devid, 
                                        cyg_pci_device *dev_info );


// This function sets the PCI configuration information for the
// device indicated in devid. Only the configuration space registers
// that are writable are actually written. Once all the fields have
// been written, the device info will be read back into *dev_info, so
// that it reflects the true state of the hardware.
externC void cyg_pci_set_device_info ( cyg_pci_device_id devid, 
                                       cyg_pci_device *dev_info );

//------------------------------------------------------------------------
// Device find functions

// Searches the PCI bus configuration space for a device with the
// given vendor and device ids. The search starts at the device
// pointed to by devid, or at the first slot if it contains
// CYG_PCI_NULL_DEVID. *devid will be updated with the ID of the next
// device found. Returns true if one is found and false if not.
externC cyg_bool cyg_pci_find_device( cyg_uint16 vendor, cyg_uint16 device,
                                      cyg_pci_device_id *devid );


// Searches the PCI bus configuration space for a device with the
// given class code.  The search starts at the device pointed to by
// devid, or at the first slot if it contains CYG_PCI_NULL_DEVID.
// *devid will be updated with the ID of the next device found.
// Returns true if one is found and false if not.
externC cyg_bool cyg_pci_find_class( cyg_uint32 dev_class,
                                     cyg_pci_device_id *devid );



// Searches the PCI bus configuration space for a device whose properties
// match those required by the match_func, which the user supplies.  The
// match_func's arguments are vendor, device, class exactly as they might
// be in the two APIs above.  The additional parameter is for any state
// which a caller might wish available to its callback routine.
// The search starts at the device pointed to by
// devid, or at the first slot if it contains CYG_PCI_NULL_DEVID.  *devid
// will be updated with the ID of the next device found.  Returns true if
// one is found and false if not.
typedef cyg_bool (cyg_pci_match_func)( cyg_uint16,/* vendor */
                                       cyg_uint16,/* device */
                                       cyg_uint32,/* class  */
                                       void * /* arbitrary user data */ );
externC cyg_bool cyg_pci_find_matching( cyg_pci_match_func *matchp,
                                        void * match_callback_data,
                                        cyg_pci_device_id *devid );


// Searches the PCI configuration space for the next valid device
// after cur_devid. If cur_devid is given the value
// CYG_PCI_NULL_DEVID, then the search starts at the first slot. It
// is permitted for next_devid to point to the cur_devid.  Returns
// true if another device is found and false if not.
externC cyg_bool cyg_pci_find_next( cyg_pci_device_id cur_devid, 
                                    cyg_pci_device_id *next_devid );

//------------------------------------------------------------------------
// Resource Allocation
// These routines allocate memory and IO space to PCI devices.

// Allocate memory and IO space to all base address registers using
// the current memory and IO base addresses in the library. If
// dev_info does not contain valid base_size[] entries, then the
// result is false.
externC cyg_bool cyg_pci_configure_device( cyg_pci_device *dev_info );

// Allocate memory and IO space for all devices found on the given
// bus and its subordinate busses. This routine recurses when a
// PCI-to-PCI bridge is encountered. The next_bus argument points
// to a variable holding the bus number of the next PCI bus to
// be allocated when a bridge is encountered. This routine returns
// true if successful, false if unsuccessful.
externC cyg_bool cyg_pci_configure_bus( cyg_uint8 bus,
					cyg_uint8 *next_bus );

// These routines set the base addresses for memory and IO mappings
// to be used by the memory allocation routines. Normally these base
// addresses will be set to default values based on the platform,
// these routines allow those to be changed by application code if
// necessary.
externC void cyg_pci_set_memory_base( CYG_PCI_ADDRESS64 base );
externC void cyg_pci_set_io_base( CYG_PCI_ADDRESS32 base );

// These routines allocate memory or IO space to the base address
// register indicated by bar. The base address in *base will be
// correctly aligned and the address of the next free location will
// be written back into it if the allocation succeeds. If the base
// address register is of the wrong type for this allocation, or
// dev_info does not contain valid base_size[] entries, the result is
// false.
externC cyg_bool cyg_pci_allocate_memory( cyg_pci_device *dev_info,
                                          cyg_uint32 bar, 
                                          CYG_PCI_ADDRESS64 *base );
externC cyg_bool cyg_pci_allocate_io( cyg_pci_device *dev_info,
                                      cyg_uint32 bar, 
                                      CYG_PCI_ADDRESS32 *base );

// Translate the device's PCI interrupt (INTA#-INTD#) to the
// associated HAL vector. This may also depend on which slot the
// device occupies. If the device may generate interrupts, the
// translated vector number will be stored in vec and the result is
// true. Otherwise the result is false.
externC cyg_bool cyg_pci_translate_interrupt( cyg_pci_device *dev_info,
                                              CYG_ADDRWORD *vec );


//----------------------------------------------------------------------
// Specific device configuration access functions

// Read functions
// These functions read registers of the appropriate size from the
// configuration space of the given device. They should mainly be
// used to access registers that are device specific. General PCI
// registers are best accessed through cyg_pci_get_device_info().
externC void cyg_pci_read_config_uint8( cyg_pci_device_id devid,
                                        cyg_uint8 offset, cyg_uint8 *val);
externC void cyg_pci_read_config_uint16( cyg_pci_device_id devid,
                                         cyg_uint8 offset, cyg_uint16 *val);
externC void cyg_pci_read_config_uint32( cyg_pci_device_id devid,
                                         cyg_uint8 offset, cyg_uint32 *val);

// Write functions
// These functions write registers of the appropriate size to the
// configuration space of the given device. They should mainly be
// used to access registers that are device specific. General PCI
// registers are best accessed through
// cyg_pci_get_device_info(). Writing the general registers this way
// may render the contents of a cyg_pci_device structure invalid.
externC void cyg_pci_write_config_uint8( cyg_pci_device_id devid,
                                         cyg_uint8 offset, cyg_uint8 val);
externC void cyg_pci_write_config_uint16( cyg_pci_device_id devid,
                                          cyg_uint8 offset, cyg_uint16 val);
externC void cyg_pci_write_config_uint32( cyg_pci_device_id devid,
                                          cyg_uint8 offset, cyg_uint32 val);


//----------------------------------------------------------------------
// Functions private to the PCI library. These should only be used by
// tests.
externC cyg_bool cyg_pci_allocate_memory_priv(cyg_pci_device *dev_info,
                                              cyg_uint32 bar,
                                              CYG_PCI_ADDRESS64 *base,
                                             CYG_PCI_ADDRESS64 *assigned_addr);
externC cyg_bool cyg_pci_allocate_io_priv( cyg_pci_device *dev_info,
                                           cyg_uint32 bar, 
                                           CYG_PCI_ADDRESS32 *base,
                                           CYG_PCI_ADDRESS32 *assigned_addr);


//----------------------------------------------------------------------
// Bus probing limits.
#define CYG_PCI_MAX_BUS                      8  // Eight is enough?
#define CYG_PCI_MAX_DEV                      32
#define CYG_PCI_MAX_FN                       8
#define CYG_PCI_MAX_BAR                      6

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_PCI_H
// End of pci.h
