//=============================================================================
//
//      pci.c
//
//      PCI library
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jskov, from design by nickg
// Contributors: jskov
// Date:         1999-08-09
// Purpose:      PCI configuration
// Description: 
//               PCI bus support library.
//               Handles simple resource allocation for devices.
//               Can configure 64bit devices, but drivers may need special
//               magic to access all of this memory space - this is platform
//               specific and the driver must know how to handle it on its own.
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/io/pci_hw.h>

// CYG_PCI_PRESENT only gets defined for targets that provide PCI HAL support.
// See pci_hw.h for details.
#ifdef CYG_PCI_PRESENT

#include <cyg/io/pci.h>
#include <cyg/infra/cyg_ass.h>

static cyg_bool cyg_pci_lib_initialized = false;

void
cyg_pci_init( void )
{
    if (!cyg_pci_lib_initialized) {
        // Initialize the PCI bus, preparing it for general access.
        cyg_pcihw_init();

        cyg_pci_lib_initialized = true;
    }
}

//---------------------------------------------------------------------------
// Common device configuration access functions

void 
cyg_pci_get_device_info ( cyg_pci_device_id devid, cyg_pci_device *dev_info )
{
    int i;
    cyg_uint8 bus = CYG_PCI_DEV_GET_BUS(devid);
    cyg_uint8 devfn = CYG_PCI_DEV_GET_DEVFN(devid);

    dev_info->devid = devid;

    cyg_pcihw_read_config_uint16(bus, devfn, CYG_PCI_CFG_VENDOR,
                                 &dev_info->vendor);
    cyg_pcihw_read_config_uint16(bus, devfn, CYG_PCI_CFG_DEVICE,
                                 &dev_info->device);
    cyg_pcihw_read_config_uint16(bus, devfn, CYG_PCI_CFG_COMMAND,
                                 &dev_info->command);
    cyg_pcihw_read_config_uint16(bus, devfn, CYG_PCI_CFG_STATUS,
                                 &dev_info->status);
    cyg_pcihw_read_config_uint32(bus, devfn, CYG_PCI_CFG_CLASS_REV,
                                 &dev_info->class_rev);
    cyg_pcihw_read_config_uint8(bus, devfn, CYG_PCI_CFG_CACHE_LINE_SIZE,
                                &dev_info->cache_line_size);
    cyg_pcihw_read_config_uint8(bus, devfn, CYG_PCI_CFG_LATENCY_TIMER,
                                &dev_info->latency_timer);
    cyg_pcihw_read_config_uint8(bus, devfn, CYG_PCI_CFG_HEADER_TYPE,
                                (cyg_uint8*)&dev_info->header_type);
    cyg_pcihw_read_config_uint8(bus, devfn, CYG_PCI_CFG_BIST,
                                &dev_info->bist);

    for (i = 0; i < CYG_PCI_MAX_BAR; i++){
        cyg_uint32 bar;
        cyg_pcihw_read_config_uint32(bus, devfn, 
                                     CYG_PCI_CFG_BAR_BASE + 4*i,
                                     &bar);
        dev_info->base_address[i] = bar;
    }

    // If device is disabled, probe BARs for sizes.
    if ((dev_info->command & CYG_PCI_CFG_COMMAND_ACTIVE) == 0) {

        for (i = 0; i < CYG_PCI_MAX_BAR; i++){
            cyg_uint32 size;

            cyg_pcihw_write_config_uint32(bus, devfn, 
                                          CYG_PCI_CFG_BAR_BASE + 4*i,
                                          0xffffffff);
            cyg_pcihw_read_config_uint32(bus, devfn, 
                                         CYG_PCI_CFG_BAR_BASE + 4*i,
                                         &size);
            dev_info->base_size[i] = size;
            dev_info->base_map[i] = 0xffffffff;
            

            // Check for a 64bit memory region.
            if (CYG_PCI_CFG_BAR_SPACE_MEM == 
                (size & CYG_PCI_CFG_BAR_SPACE_MASK)) {
                if (size & CYG_PRI_CFG_BAR_MEM_TYPE_64) {
                    // Clear fields for next BAR - it's the upper 32 bits.
                    i++;
                    dev_info->base_size[i] = 0;
                    dev_info->base_map[i] = 0xffffffff;
                }
            }
        }
    } else {
        // Clear the base map so we can recognize an already configured
        // device.
        for (i = 0; i < CYG_PCI_MAX_BAR; i++) {
            dev_info->base_size[i] = 0;
        }
    }
            

    switch (dev_info->header_type & CYG_PCI_CFG_HEADER_TYPE_MASK) {
    case CYG_PCI_HEADER_NORMAL:
        cyg_pcihw_read_config_uint32(bus, devfn, CYG_PCI_CFG_CARDBUS_CIS,
                                     &dev_info->header.normal.cardbus_cis);
        cyg_pcihw_read_config_uint16(bus, devfn, CYG_PCI_CFG_SUB_VENDOR,
                                     &dev_info->header.normal.sub_vendor);
        cyg_pcihw_read_config_uint16(bus, devfn, CYG_PCI_CFG_SUB_ID,
                                     &dev_info->header.normal.sub_id);
        cyg_pcihw_read_config_uint32(bus, devfn, CYG_PCI_CFG_ROM_ADDRESS,
                                     &dev_info->header.normal.rom_address);
        cyg_pcihw_read_config_uint8(bus, devfn, CYG_PCI_CFG_CAP_LIST,
                                    &dev_info->header.normal.cap_list);

        cyg_pcihw_read_config_uint8(bus, devfn, CYG_PCI_CFG_INT_LINE,
                                    &dev_info->header.normal.int_line);
        cyg_pcihw_read_config_uint8(bus, devfn, CYG_PCI_CFG_INT_PIN,
                                    &dev_info->header.normal.int_pin);
        cyg_pcihw_read_config_uint8(bus, devfn, CYG_PCI_CFG_MIN_GNT,
                                    &dev_info->header.normal.min_gnt);
        cyg_pcihw_read_config_uint8(bus, devfn, CYG_PCI_CFG_MAX_LAT,
                                    &dev_info->header.normal.max_lat);
        break;
    case CYG_PCI_HEADER_BRIDGE:
        CYG_FAIL("PCI device header 'bridge' support not implemented");
        break;
    case CYG_PCI_HEADER_CARDBUS_BRIDGE:
        CYG_FAIL("PCI device header 'cardbus bridge' support not implemented");
        break;
    default:
        CYG_FAIL("Unknown PCI device header type");
        break;
    }
}

void 
cyg_pci_set_device_info ( cyg_pci_device_id devid, cyg_pci_device *dev_info )
{
    cyg_uint8 bus = CYG_PCI_DEV_GET_BUS(devid);
    cyg_uint8 devfn = CYG_PCI_DEV_GET_DEVFN(devid);

    // Only writable entries are updated.
    cyg_pcihw_write_config_uint16(bus, devfn, CYG_PCI_CFG_COMMAND,
                                  dev_info->command);
    cyg_pcihw_write_config_uint16(bus, devfn, CYG_PCI_CFG_STATUS,
                                  dev_info->status);
    cyg_pcihw_write_config_uint8(bus, devfn, CYG_PCI_CFG_CACHE_LINE_SIZE,
                                 dev_info->cache_line_size);
    cyg_pcihw_write_config_uint8(bus, devfn, CYG_PCI_CFG_LATENCY_TIMER,
                                 dev_info->latency_timer);
    cyg_pcihw_write_config_uint8(bus, devfn, CYG_PCI_CFG_BIST,
                                 dev_info->bist);

    cyg_pcihw_write_config_uint32(bus, devfn, CYG_PCI_CFG_BAR_0,
                                  dev_info->base_address[0]);
    cyg_pcihw_write_config_uint32(bus, devfn, CYG_PCI_CFG_BAR_1,
                                  dev_info->base_address[1]);
    cyg_pcihw_write_config_uint32(bus, devfn, CYG_PCI_CFG_BAR_2,
                                  dev_info->base_address[2]);
    cyg_pcihw_write_config_uint32(bus, devfn, CYG_PCI_CFG_BAR_3,
                                  dev_info->base_address[3]);
    cyg_pcihw_write_config_uint32(bus, devfn, CYG_PCI_CFG_BAR_4,
                                  dev_info->base_address[4]);
    cyg_pcihw_write_config_uint32(bus, devfn, CYG_PCI_CFG_BAR_5,
                                  dev_info->base_address[5]);

    switch (dev_info->header_type & CYG_PCI_CFG_HEADER_TYPE_MASK) {
    case CYG_PCI_HEADER_NORMAL:
        cyg_pcihw_write_config_uint32(bus, devfn, CYG_PCI_CFG_CARDBUS_CIS,
                                      dev_info->header.normal.cardbus_cis);
        cyg_pcihw_write_config_uint16(bus, devfn, CYG_PCI_CFG_SUB_VENDOR,
                                      dev_info->header.normal.sub_vendor);
        cyg_pcihw_write_config_uint16(bus, devfn, CYG_PCI_CFG_SUB_ID,
                                      dev_info->header.normal.sub_id);
        cyg_pcihw_write_config_uint32(bus, devfn, CYG_PCI_CFG_ROM_ADDRESS,
                                      dev_info->header.normal.rom_address);

        cyg_pcihw_write_config_uint8(bus, devfn, CYG_PCI_CFG_INT_LINE,
                                     dev_info->header.normal.int_line);
        cyg_pcihw_write_config_uint8(bus, devfn, CYG_PCI_CFG_INT_PIN,
                                     dev_info->header.normal.int_pin);
        cyg_pcihw_write_config_uint8(bus, devfn, CYG_PCI_CFG_MIN_GNT,
                                     dev_info->header.normal.min_gnt);
        cyg_pcihw_write_config_uint8(bus, devfn, CYG_PCI_CFG_MAX_LAT,
                                     dev_info->header.normal.max_lat);
        break;
    case CYG_PCI_HEADER_BRIDGE:
        CYG_FAIL("PCI device header 'bridge' support not implemented");
        break;
    case CYG_PCI_HEADER_CARDBUS_BRIDGE:
        CYG_FAIL("PCI device header 'cardbus bridge' support not implemented");
        break;
    default:
        CYG_FAIL("Unknown PCI device header type");
        break;
    }

    // Update values in dev_info.
    cyg_pci_get_device_info(devid, dev_info);
}


//---------------------------------------------------------------------------
// Specific device configuration access functions
void 
cyg_pci_read_config_uint8( cyg_pci_device_id devid,
                           cyg_uint8 offset, cyg_uint8 *val)
{
    cyg_pcihw_read_config_uint8(CYG_PCI_DEV_GET_BUS(devid),
                                CYG_PCI_DEV_GET_DEVFN(devid),
                                offset, val);
}

void 
cyg_pci_read_config_uint16( cyg_pci_device_id devid,
                            cyg_uint8 offset, cyg_uint16 *val)
{
    cyg_pcihw_read_config_uint16(CYG_PCI_DEV_GET_BUS(devid),
                                 CYG_PCI_DEV_GET_DEVFN(devid),
                                 offset, val);
}

void
cyg_pci_read_config_uint32( cyg_pci_device_id devid,
                            cyg_uint8 offset, cyg_uint32 *val)
{
    cyg_pcihw_read_config_uint32(CYG_PCI_DEV_GET_BUS(devid),
                                 CYG_PCI_DEV_GET_DEVFN(devid),
                                 offset, val);
}


// Write functions
void
cyg_pci_write_config_uint8( cyg_pci_device_id devid,
                            cyg_uint8 offset, cyg_uint8 val)
{
    cyg_pcihw_write_config_uint8(CYG_PCI_DEV_GET_BUS(devid),
                                 CYG_PCI_DEV_GET_DEVFN(devid),
                                 offset, val);
}

void
cyg_pci_write_config_uint16( cyg_pci_device_id devid,
                             cyg_uint8 offset, cyg_uint16 val)
{
    cyg_pcihw_write_config_uint16(CYG_PCI_DEV_GET_BUS(devid),
                                  CYG_PCI_DEV_GET_DEVFN(devid),
                                  offset, val);
}

void
cyg_pci_write_config_uint32( cyg_pci_device_id devid,
                             cyg_uint8 offset, cyg_uint32 val)
{
    cyg_pcihw_write_config_uint32(CYG_PCI_DEV_GET_BUS(devid),
                                  CYG_PCI_DEV_GET_DEVFN(devid),
                                  offset, val);
}

//------------------------------------------------------------------------
// Device find functions

cyg_bool
cyg_pci_find_next( cyg_pci_device_id cur_devid, 
                   cyg_pci_device_id *next_devid )
{
    cyg_uint8 bus = CYG_PCI_DEV_GET_BUS(cur_devid);
    cyg_uint8 devfn = CYG_PCI_DEV_GET_DEVFN(cur_devid);
    cyg_uint8 dev = CYG_PCI_DEV_GET_DEV(devfn);
    cyg_uint8 fn = CYG_PCI_DEV_GET_FN(devfn);

    // If this is the initializer, start with 0/0/0
    if (CYG_PCI_NULL_DEVID == cur_devid) {
        bus = dev = fn = 0;
    } else {
        // Otherwise, check multi-function bit of device's first function
        cyg_uint8 header;
        devfn = CYG_PCI_DEV_MAKE_DEVFN(dev, 0);
        cyg_pcihw_read_config_uint8(bus, devfn,
                                    CYG_PCI_CFG_HEADER_TYPE, &header);
        if (header & CYG_PCI_CFG_HEADER_TYPE_MF) {
            // Multi-function device. Increase fn.
            fn++;
            if (fn >= CYG_PCI_MAX_FN) {
                fn = 0;
                dev++;
            }
        } else {
            // Single-function device. Skip to next.
            dev++;
        }
    }

    // Note: Reset iterators in enclosing statement's "next" part.
    //       Allows resuming scan with given input values. 
    for (;bus < CYG_PCI_MAX_BUS; bus++, dev=0) {
        for (;dev < CYG_PCI_MAX_DEV; dev++, fn=0) {
            for (;fn < CYG_PCI_MAX_FN; fn++) {
                cyg_uint16 vendor;
                devfn = CYG_PCI_DEV_MAKE_DEVFN(dev, fn);
                cyg_pcihw_read_config_uint16(bus, devfn,
                                             CYG_PCI_CFG_VENDOR, &vendor);

                if (CYG_PCI_VENDOR_UNDEFINED != vendor) {
                    *next_devid = CYG_PCI_DEV_MAKE_ID(bus, devfn);
                    return true;
                }
            }
        }
    }

    return false;
}

cyg_bool
cyg_pci_find_device( cyg_uint16 vendor, cyg_uint16 device,
                     cyg_pci_device_id *devid )
{
    // Scan entire bus, check for matches on valid devices.
    while (cyg_pci_find_next(*devid, devid)) {
        cyg_uint8 bus = CYG_PCI_DEV_GET_BUS(*devid);
        cyg_uint8 devfn = CYG_PCI_DEV_GET_DEVFN(*devid);
        cyg_uint16 v, d;

        // Check that vendor matches.
        cyg_pcihw_read_config_uint16(bus, devfn,
                                     CYG_PCI_CFG_VENDOR, &v);
        if (v != vendor) continue;

        // Check that device matches.
        cyg_pcihw_read_config_uint16(bus, devfn,
                                     CYG_PCI_CFG_DEVICE, &d);
        if (d == device)
            return true;
    }

    return false;
}

cyg_bool
cyg_pci_find_class( cyg_uint32 dev_class, cyg_pci_device_id *devid )
{
    // Scan entire bus, check for matches on valid devices.
    while (cyg_pci_find_next(*devid, devid)) {
        cyg_uint8 bus = CYG_PCI_DEV_GET_BUS(*devid);
        cyg_uint8 devfn = CYG_PCI_DEV_GET_DEVFN(*devid);
        cyg_uint32 c;

        // Check that class code matches.
        cyg_pcihw_read_config_uint32(bus, devfn,
                                     CYG_PCI_CFG_CLASS_REV, &c);
        c >>= 8;
        if (c == dev_class)
            return true;
    }

    return false;
}

//------------------------------------------------------------------------
// Resource Allocation

static CYG_PCI_ADDRESS64 cyg_pci_memory_base = HAL_PCI_ALLOC_BASE_MEMORY;
static CYG_PCI_ADDRESS32 cyg_pci_io_base = HAL_PCI_ALLOC_BASE_IO;

void
cyg_pci_set_memory_base(CYG_PCI_ADDRESS64 base)
{
    cyg_pci_memory_base = base;
}

void
cyg_pci_set_io_base(CYG_PCI_ADDRESS32 base)
{
    cyg_pci_io_base = base;
}

cyg_bool
cyg_pci_configure_device( cyg_pci_device *dev_info )
{
    int bar;
    cyg_uint32 flags;
    cyg_bool ret = false;

    // Check that device is inactive.
    if ((dev_info->command & CYG_PCI_CFG_COMMAND_ACTIVE) != 0)
        return false;

    for (bar = 0; bar < CYG_PCI_MAX_BAR; bar++) {
        flags = dev_info->base_size[bar];

        // No reason to scan beyond first inactive BAR.
        if (0 == flags)
            break;

        if ((flags & CYG_PCI_CFG_BAR_SPACE_MASK) == CYG_PCI_CFG_BAR_SPACE_MEM){
            ret |= cyg_pci_allocate_memory(dev_info, bar, 
                                           &cyg_pci_memory_base);

            // If this is a 64bit memory region, skip the next bar
            // since it will contain the top 32 bits.
            if (flags & CYG_PRI_CFG_BAR_MEM_TYPE_64)
                bar++;
        } else
            ret |= cyg_pci_allocate_io(dev_info, bar, &cyg_pci_io_base);

        cyg_pci_translate_interrupt(dev_info, &dev_info->hal_vector);
    }

    return ret;
}

// This is the function that handles resource allocation. It doesn't
// affect the device state.
// Should not be called with top32bit-bar of a 64bit pair.
inline cyg_bool
cyg_pci_allocate_memory_priv( cyg_pci_device *dev_info, cyg_uint32 bar,
                              CYG_PCI_ADDRESS64 *base, 
                              CYG_PCI_ADDRESS64 *assigned_addr)
{
    cyg_uint32 mem_type, flags;
    CYG_PCI_ADDRESS64 size, aligned_addr;

    // Get the probed size and flags
    flags = dev_info->base_size[bar];

    // Decode size
    size = (CYG_PCI_ADDRESS64) ((~(flags & CYG_PRI_CFG_BAR_MEM_MASK))+1);

    // Calculate address we will assign the device.
    // This can be made more clever, specifically:
    //  1) The lowest 1MB should be reserved for devices with 1M memory type.
    //     : Needs to be handled.
    //  2) The low 32bit space should be reserved for devices with 32bit type.
    //     : With the usual handful of devices it is unlikely that the
    //       low 4GB space will become full.
    //  3) A bitmap can be used to avoid fragmentation.
    //     : Again, unlikely to be necessary.
    //
    // For now, simply align to required size.
    aligned_addr = (*base+size-1) & ~(size-1);

    // Is the request for memory space?
    if (CYG_PCI_CFG_BAR_SPACE_MEM != (flags & CYG_PCI_CFG_BAR_SPACE_MASK))
        return false;
    
    // Check type of memory requested...
    mem_type = CYG_PRI_CFG_BAR_MEM_TYPE_MASK & flags;

    // We don't handle <1MB devices optimally yet.
    if (CYG_PRI_CFG_BAR_MEM_TYPE_1M == mem_type
        && (aligned_addr + size) > 1024*1024)
        return false;

    // Update the resource pointer and return values.
    *base = aligned_addr+size;
    *assigned_addr = aligned_addr;

    dev_info->base_map[bar] = (cyg_uint32) 
        (aligned_addr+HAL_PCI_PHYSICAL_MEMORY_BASE) & 0xffffffff;

    // If a 64bit region, store upper 32 bits in the next bar.
    // Note: The CPU is not necessarily able to access the region
    // linearly - it may have to do it in segments. Driver must handle that.
    if (CYG_PRI_CFG_BAR_MEM_TYPE_64 == mem_type) {
        dev_info->base_map[bar+1] = (cyg_uint32) 
            ((aligned_addr+HAL_PCI_PHYSICAL_MEMORY_BASE) >> 32) & 0xffffffff;
    }
    
    return true;
}

cyg_bool
cyg_pci_allocate_memory( cyg_pci_device *dev_info, cyg_uint32 bar,
                         CYG_PCI_ADDRESS64 *base)
{
    cyg_uint8 bus = CYG_PCI_DEV_GET_BUS(dev_info->devid);
    cyg_uint8 devfn = CYG_PCI_DEV_GET_DEVFN(dev_info->devid);
    CYG_PCI_ADDRESS64 assigned_addr;
    cyg_bool ret;

    // Check that device is inactive.
    if ((dev_info->command & CYG_PCI_CFG_COMMAND_ACTIVE) != 0)
        return false;

    // Allocate memory space for the device.
    ret = cyg_pci_allocate_memory_priv(dev_info, bar, base, &assigned_addr);

    if (ret) {
        // Map the device and update the BAR in the dev_info structure.
        cyg_pcihw_write_config_uint32(bus, devfn,
                                      CYG_PCI_CFG_BAR_BASE+4*bar, 
                                      (cyg_uint32) 
                                      (assigned_addr & 0xffffffff));
        cyg_pcihw_read_config_uint32(bus, devfn,
                                     CYG_PCI_CFG_BAR_BASE+4*bar, 
                                     &dev_info->base_address[bar]);

        // Handle upper 32 bits if necessary.
        if (dev_info->base_size[bar] & CYG_PRI_CFG_BAR_MEM_TYPE_64) {
            cyg_pcihw_write_config_uint32(bus, devfn,
                                          CYG_PCI_CFG_BAR_BASE+4*(bar+1), 
                                          (cyg_uint32) 
                                          ((assigned_addr >> 32)& 0xffffffff));
            cyg_pcihw_read_config_uint32(bus, devfn,
                                         CYG_PCI_CFG_BAR_BASE+4*(bar+1), 
                                         &dev_info->base_address[bar+1]);
        }
    }

    return ret;
}    

cyg_bool
cyg_pci_allocate_io_priv( cyg_pci_device *dev_info, cyg_uint32 bar, 
                          CYG_PCI_ADDRESS32 *base, 
                          CYG_PCI_ADDRESS32 *assigned_addr)
{
    cyg_uint32 flags, size;
    CYG_PCI_ADDRESS32 aligned_addr;

    // Get the probed size and flags
    flags = dev_info->base_size[bar];

    // Decode size
    size = (~(flags & CYG_PRI_CFG_BAR_IO_MASK))+1;

    // Calculate address we will assign the device.
    // This can be made more clever.
    // For now, simply align to required size.
    aligned_addr = (*base+size-1) & ~(size-1);

    // Ensure the region fits within the 1MB IO space
    if (aligned_addr+size > 1024*1024)
        return false;

    // Is the request for IO space?
    if (CYG_PCI_CFG_BAR_SPACE_IO != (flags & CYG_PCI_CFG_BAR_SPACE_MASK))
        return false;

    // Update the resource pointer and return values.
    *base = aligned_addr+size;
    dev_info->base_map[bar] = aligned_addr+HAL_PCI_PHYSICAL_IO_BASE;
    *assigned_addr = aligned_addr;

    return true;
}


cyg_bool
cyg_pci_allocate_io( cyg_pci_device *dev_info, cyg_uint32 bar, 
                     CYG_PCI_ADDRESS32 *base)
{
    cyg_uint8 bus = CYG_PCI_DEV_GET_BUS(dev_info->devid);
    cyg_uint8 devfn = CYG_PCI_DEV_GET_DEVFN(dev_info->devid);
    CYG_PCI_ADDRESS32 assigned_addr;
    cyg_bool ret;
    
    // Check that device is inactive.
    if ((dev_info->command & CYG_PCI_CFG_COMMAND_ACTIVE) != 0)
        return false;

    // Allocate IO space for the device.
    ret = cyg_pci_allocate_io_priv(dev_info, bar, base, &assigned_addr);

    if (ret) {
        // Map the device and update the BAR in the dev_info structure.
        cyg_pcihw_write_config_uint32(bus, devfn,
                                      CYG_PCI_CFG_BAR_BASE+4*bar, 
                                      assigned_addr);
        cyg_pcihw_read_config_uint32(bus, devfn,
                                     CYG_PCI_CFG_BAR_BASE+4*bar, 
                                     &dev_info->base_address[bar]);
    }

    return ret;
}

cyg_bool
cyg_pci_translate_interrupt( cyg_pci_device *dev_info,
                             CYG_ADDRWORD *vec )
{
    cyg_uint8 bus = CYG_PCI_DEV_GET_BUS(dev_info->devid);
    cyg_uint8 devfn = CYG_PCI_DEV_GET_DEVFN(dev_info->devid);

    return cyg_pcihw_translate_interrupt(bus, devfn, vec);
}

#endif // ifdef CYG_PCI_PRESENT

//-----------------------------------------------------------------------------
// end of pci.c
