//==========================================================================
//
//      iq80310_pci.c
//
//      HAL board support code for XScale IQ80310 PCI
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
// at http://sources.redhat.com/ecos/ecos-license
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    msalter
// Contributors: msalter
// Date:         2000-10-10
// Purpose:      PCI support
// Description:  Implementations of HAL PCI interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/system.h>
#include CYGBLD_HAL_PLATFORM_H
#include CYGHWR_MEMORY_LAYOUT_H

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_if.h>             // calling interface API
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // Interrupt names
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_iq80310.h>        // Hardware definitions
#include <cyg/io/pci_hw.h>
#include <cyg/io/pci.h>

static cyg_uint8 pbus_nr;
static cyg_uint8 sbus_nr;

void cyg_hal_plf_pci_init(void)
{
    cyg_uint32 limit_reg, adj_dram_size;
    cyg_uint8  next_bus;

    // Initialize Secondary PCI bus (bus 1)
    *(volatile cyg_uint16 *)BCR_ADDR |= 0x40;  // reset secondary bus
    hal_delay_us(10 * 1000); 	// 10ms enough??
    *(volatile cyg_uint16 *)BCR_ADDR &= ~0x40;  // release reset

    // *********  vendor / device id **********
    *(cyg_uint16 *)ASVIR_ADDR = 0x113C;
    *(cyg_uint16 *)ASIR_ADDR = 0x0700;

    // suppress secondary bus idsels to provide private secondary devices
    *(cyg_uint16 *)SISR_ADDR = 0x03FF;

    // ******* Primary Inbound ATU *********

    // set primary inbound ATU translate value register to point to
    // base of local DRAM
    *(cyg_uint32 *)PIATVR_ADDR = MEMBASE_DRAM & 0xFFFFFFFC;
    // set primary inbound ATU limit register to include all of installed DRAM.
    // This value used as a mask.
    adj_dram_size = hal_dram_size;
    limit_reg = (0xFFFFFFFF-(adj_dram_size-1)) & 0xFFFFFFF0;
    *(cyg_uint32 *)PIALR_ADDR = limit_reg;

    if (iq80310_is_host()) {

	// set the primary inbound ATU base address to the start of DRAM
	*(cyg_uint32 *)PIABAR_ADDR = MEMBASE_DRAM & 0xFFFFF000;

	// ********* Set Primary Outbound Windows *********

	// Note: The primary outbound ATU memory window value register
	//       and i/o window value registers are defaulted to 0

	// set the primary outbound windows to directly map Local - PCI
        // requests
	// outbound memory window
	*(cyg_uint32 *)POMWVR_ADDR = PRIMARY_MEM_BASE;

	// outbound DAC Window
	*(cyg_uint32 *)PODWVR_ADDR = PRIMARY_DAC_BASE;

	// outbound I/O window
        *(cyg_uint32 *)POIOWVR_ADDR = PRIMARY_IO_BASE;	
    }

#ifdef CYGSEM_HAL_ARM_IQ80310_CLEAR_PCI_RETRY
    // clear RETRY
    *(cyg_uint16 *)EBCR_ADDR = 0x0008;
#endif

    // ******** Secondary Inbound ATU ***********

    // set secondary inbound ATU translate value register to point to base
    // of local DRAM
    *(cyg_uint32 *)SIATVR_ADDR = MEMBASE_DRAM & 0xFFFFFFFC;

    // set secondary inbound ATU base address to start of DRAM
    *(cyg_uint32 *)SIABAR_ADDR = MEMBASE_DRAM & 0xFFFFF000;

    // set secondary inbound ATU limit register to include all of
    // installed DRAM. This value used as a mask.

    //  always allow secondary pci access to all memory (even with A0 step)
    limit_reg = (0xFFFFFFFF - (adj_dram_size - 1)) & 0xFFFFFFF0;
    *(cyg_uint32 *)SIALR_ADDR = limit_reg;


    // ********** Set Secondary Outbound Windows ***********

    // Note: The secondary outbound ATU memory window value register
    // and i/o window value registers are defaulted to 0

    // set the secondary outbound window to directly map Local - PCI requests
    // outbound memory window
    *(cyg_uint32 *)SOMWVR_ADDR = SECONDARY_MEM_BASE;

    // outbound DAC Window
    *(cyg_uint32 *)SODWVR_ADDR = SECONDARY_DAC_BASE;

    // outbound I/O window
    *(cyg_uint32 *)SOIOWVR_ADDR = SECONDARY_IO_BASE;

    // ***********  command / config / latency registers  ************

    if (iq80310_is_host()) {
	// allow primary ATU to act as a bus master, respond to PCI 
	// memory accesses, assert P_SERR#, and enable parity checking
	*(cyg_uint16 *)PATUCMD_ADDR = (CYG_PCI_CFG_COMMAND_SERR   | \
				       CYG_PCI_CFG_COMMAND_PARITY | \
				       CYG_PCI_CFG_COMMAND_MASTER | \
				       CYG_PCI_CFG_COMMAND_MEMORY);
    }

    // allow secondary ATU to act as a bus master, respond to PCI memory
    // accesses, and assert S_SERR#
    *(cyg_uint16 *)SATUCMD_ADDR = (CYG_PCI_CFG_COMMAND_SERR   | \
				   CYG_PCI_CFG_COMMAND_PARITY | \
				   CYG_PCI_CFG_COMMAND_MASTER | \
				   CYG_PCI_CFG_COMMAND_MEMORY);

    // enable primary and secondary outbound ATUs, BIST, and primary bus
    // direct addressing
    *(cyg_uint32 *)ATUCR_ADDR = 0x00000006;

    // ************ bridge registers *******************
    if (iq80310_is_host()) {

	// set the bridge command register
	*(cyg_uint16 *)PCR_ADDR = (CYG_PCI_CFG_COMMAND_SERR   | \
				   CYG_PCI_CFG_COMMAND_PARITY | \
				   CYG_PCI_CFG_COMMAND_MASTER | \
				   CYG_PCI_CFG_COMMAND_MEMORY);

	// set the secondary bus number to 1
	*(cyg_uint8 *)SBNR_ADDR = SECONDARY_BUS_NUM;
	*(cyg_uint16 *)BCR_ADDR = 0x0823;
	// set the primary bus number to 0
	*(cyg_uint8 *)PBNR_ADDR = PRIMARY_BUS_NUM;
    } else {
#ifdef CYGSEM_HAL_ARM_IQ80310_CLEAR_PCI_RETRY
	// Wait for PC BIOS to initialize bus number
	int i;

	for (i = 0; i < 15000; i++) {
	    if (*((volatile cyg_uint8 *)SBNR_ADDR) != 0)
		break;
	    hal_delay_us(1000);  // 1msec
	}
#endif
	if (*((volatile cyg_uint8 *)SBNR_ADDR) == 0)
	    *(cyg_uint8 *)SBNR_ADDR = SECONDARY_BUS_NUM;
    }

    pbus_nr = *(cyg_uint8 *)PBNR_ADDR;
    sbus_nr = *(cyg_uint8 *)SBNR_ADDR;

    // Now initialize the PCI busses.

    // Next assignable bus number. Yavapai primary bus is fixed as
    // bus zero and yavapai secondary is fixed as bus 1.
    next_bus = sbus_nr + 1;

    // If we are the host on the Primary bus, then configure it.
    if (iq80310_is_host()) {

	// Initialize Primary PCI bus (bus 0)
	cyg_pci_set_memory_base(PRIMARY_MEM_BASE);
	cyg_pci_set_io_base(PRIMARY_IO_BASE);
	cyg_pci_configure_bus(0, &next_bus);
    }

    // Initialize Secondary PCI bus (bus 1)
    cyg_pci_set_memory_base(SECONDARY_MEM_BASE);
    cyg_pci_set_io_base(SECONDARY_IO_BASE);
    cyg_pci_configure_bus(sbus_nr, &next_bus);
}

void cyg_hal_plf_pci_config_setup( cyg_uint32 bus,
				   cyg_uint32 devfn,
				   cyg_uint32 offset)
{
}

// Use "naked" attribute to suppress C prologue/epilogue
static void __attribute__ ((naked)) __pci_abort_handler(void) 
{
    asm ( "subs	pc, lr, #4\n" );
}

static cyg_uint32 orig_abort_vec;

static inline cyg_uint32 *pci_config_setup(cyg_uint32 bus,
					   cyg_uint32 devfn,
					   cyg_uint32 offset)
{
    cyg_uint32 *pdata, *paddr;
    cyg_uint32 dev = CYG_PCI_DEV_GET_DEV(devfn);
    cyg_uint32 fn  = CYG_PCI_DEV_GET_FN(devfn);

    if (bus == 0) {
        paddr = (cyg_uint32 *)POCCAR_ADDR;
        pdata = (cyg_uint32 *)POCCDR_ADDR;
    } else {
        paddr = (cyg_uint32 *)SOCCAR_ADDR;
        pdata = (cyg_uint32 *)SOCCDR_ADDR;
    }

    /* Offsets must be dword-aligned */
    offset &= ~3;
	
    /* Primary or secondary bus use type 0 config */
    /* all others use type 1 config */
    if (bus == pbus_nr || bus == sbus_nr)
	*paddr = ( (1 << (dev + 16)) | (fn << 8) | offset | 0 );
    else
        *paddr = ( (bus << 16) | (dev << 11) | (fn << 8) | offset | 1 );

    orig_abort_vec = ((volatile cyg_uint32 *)0x20)[4];
    ((volatile unsigned *)0x20)[4] = (unsigned)__pci_abort_handler;
    HAL_ICACHE_SYNC();

    return pdata;
}

static inline int pci_config_cleanup(cyg_uint32 bus)
{
    cyg_uint32 status = 0, err = 0;

    if (bus == pbus_nr) {
	status = *(cyg_uint16 *) PATUSR_ADDR;
	if ((status & 0xF900) != 0) {
	    err = 1;
	    *(cyg_uint16 *)PATUSR_ADDR = status & 0xF980;
	}
	status = *(cyg_uint16 *) PSR_ADDR;
	if ((status & 0xF900) != 0) {
	    err = 1;
	    *(cyg_uint16 *)PSR_ADDR = status & 0xF980;
	}
	status = *(cyg_uint32 *) PATUISR_ADDR;
	if ((status & 0x79F) != 0) {
	    err = 1;
	    *(cyg_uint32 *) PATUISR_ADDR = status & 0x79f;
	}
	status = *(cyg_uint32 *) PBISR_ADDR;
	if ((status & 0x3F) != 0) {
	    err = 1;
	    *(cyg_uint32 *) PBISR_ADDR = status & 0x3F;
	}
    } else {
	status = *(cyg_uint16 *) SATUSR_ADDR;
	if ((status & 0xF900) != 0) {
	    err = 1;
	    *(cyg_uint16 *) SATUSR_ADDR = status & 0xF900;
	}
	status = *(cyg_uint16 *) SSR_ADDR;
	if ((status & 0xF900) != 0) {
	    err = 1;
	    *(cyg_uint16 *) SSR_ADDR = status & 0xF980;
	}
	status = *(cyg_uint32 *) SATUISR_ADDR;
	if ((status & 0x69F) != 0) {
	    err = 1;
	    *(cyg_uint32 *) SATUISR_ADDR = status & 0x69F;
	}
    }

    ((volatile unsigned *)0x20)[4] = orig_abort_vec;
    HAL_ICACHE_SYNC();

    return err;
}



cyg_uint32 cyg_hal_plf_pci_cfg_read_dword (cyg_uint32 bus,
					   cyg_uint32 devfn,
					   cyg_uint32 offset)
{
    cyg_uint32 *pdata, config_data;

    pdata = pci_config_setup(bus, devfn, offset);

    config_data = *pdata;

    if (pci_config_cleanup(bus))
      return 0xffffffff;
    else
      return config_data;
}


void cyg_hal_plf_pci_cfg_write_dword (cyg_uint32 bus,
				      cyg_uint32 devfn,
				      cyg_uint32 offset,
				      cyg_uint32 data)
{
    cyg_uint32 *pdata;

    pdata = pci_config_setup(bus, devfn, offset);

    *pdata = data;

    pci_config_cleanup(bus);
}


cyg_uint16 cyg_hal_plf_pci_cfg_read_word (cyg_uint32 bus,
					  cyg_uint32 devfn,
					  cyg_uint32 offset)
{
    cyg_uint32 *pdata;
    cyg_uint16 config_data;

    pdata = pci_config_setup(bus, devfn, offset);

    config_data = (cyg_uint16)(((*pdata) >> ((offset % 0x4) * 8)) & 0xffff);

    if (pci_config_cleanup(bus))
      return 0xffff;
    else
      return config_data;
}

void cyg_hal_plf_pci_cfg_write_word (cyg_uint32 bus,
				     cyg_uint32 devfn,
				     cyg_uint32 offset,
				     cyg_uint16 data)
{
    cyg_uint32 *pdata, mask, temp;

    pdata = pci_config_setup(bus, devfn, offset);

    mask = ~(0x0000ffff << ((offset % 0x4) * 8));

    temp = (cyg_uint32)(((cyg_uint32)data) << ((offset % 0x4) * 8));
    *pdata = (*pdata & mask) | temp; 

    pci_config_cleanup(bus);
}

cyg_uint8 cyg_hal_plf_pci_cfg_read_byte (cyg_uint32 bus,
					 cyg_uint32 devfn,
					 cyg_uint32 offset)
{
    cyg_uint32 *pdata;
    cyg_uint8 config_data;

    pdata = pci_config_setup(bus, devfn, offset);

    config_data = (cyg_uint8)(((*pdata) >> ((offset % 0x4) * 8)) & 0xff);

    if (pci_config_cleanup(bus))
	return 0xff;
    else
	return config_data;
}


void cyg_hal_plf_pci_cfg_write_byte (cyg_uint32 bus,
				     cyg_uint32 devfn,
				     cyg_uint32 offset,
				     cyg_uint8 data)
{
    cyg_uint32 *pdata, mask, temp;

    pdata = pci_config_setup(bus, devfn, offset);

    mask = ~(0x000000ff << ((offset % 0x4) * 8));
    temp = (cyg_uint32)(((cyg_uint32)data) << ((offset % 0x4) * 8));
    *pdata = (*pdata & mask) | temp; 

    pci_config_cleanup(bus);
}



