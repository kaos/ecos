//=============================================================================
//
//      usb_stm32.c
//
//      USB slave driver implementation for STM32
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2009, 2010 Free Software Foundation, Inc.
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
// Author(s):   Chris Holgate
// Date:        2009-05-19
// Purpose:     STM32 USB slave driver implementation
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>

#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>

#include <cyg/io/usb/usb.h>
#include <cyg/io/usb/usbs.h>

#include <string.h>

#include <pkgconf/io_usb_slave.h>
#include <pkgconf/devs_usb_cortexm_stm32.h>

//-----------------------------------------------------------------------------
// Maintenance and debug macros.

#define TODO_USB(_msg_) CYG_ASSERT(false, "TODO (USB) : " _msg_)
#define FAIL_USB(_msg_) CYG_ASSERT(false, "FAIL (USB) : " _msg_)
#define ASSERT_USB(_test_, _msg_) CYG_ASSERT(_test_, "FAIL (USB) : " _msg_)

#if defined(CYGBLD_DEVS_USB_CORTEXM_STM32_DEBUG_TRACE)
#define TRACE_USB(_msg_, _args_...) diag_printf ("STM32 USB : " _msg_, ##_args_)
#else
#define TRACE_USB(_msg_, _args_...) while(0){}
#endif

//-----------------------------------------------------------------------------
// Shorthand for some of the configuration options.

#define USB_BASE     CYGHWR_HAL_STM32_USB
#define USB_RAM_BASE CYGHWR_HAL_STM32_USB_CAN_SRAM
#define USB_RAM_SIZE 512
#define USB_EPNUM    (1 + CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM + CYGNUM_DEVS_USB_CORTEXM_STM32_RXEP_NUM)

//-----------------------------------------------------------------------------
// Work out the bus clock frequencies and external timing constraints.
// NOTE: These require that the clock source is set to HSE, which should be
// forced by the CDL.

#define PLL_FREQ (CYGARC_HAL_CORTEXM_STM32_INPUT_CLOCK * CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_MUL)

#define APB1_FREQ ((CYGARC_HAL_CORTEXM_STM32_INPUT_CLOCK * CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_MUL) / \
  (CYGHWR_HAL_CORTEXM_STM32_CLOCK_HCLK_DIV * CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK1_DIV)) 

#define USB_TSTARTUP 1

//-----------------------------------------------------------------------------
// Provide macros for accessing the buffer layout pseudo-registers.

// Pseudo-registers for single buffer configurations.
#define USB_RAM_SB_TXADDR(__ep)        ((__ep)*16)
#define USB_RAM_SB_TXCOUNT(__ep)       ((__ep)*16+4)
#define USB_RAM_SB_RXADDR(__ep)        ((__ep)*16+8)
#define USB_RAM_SB_RXCOUNT(__ep)       ((__ep)*16+12)

// Pseudo-registers for double buffer configurations.
#define USB_RAM_DB_TXADDR(__ep,__buf)  ((__ep)*16+(__buf)*8)
#define USB_RAM_DB_TXCOUNT(__ep,__buf) ((__ep)*16+(__buf)*8+4)
#define USB_RAM_DB_RXADDR(__ep,__buf)  ((__ep)*16+(__buf)*8)
#define USB_RAM_DB_RXCOUNT(__ep,__buf) ((__ep)*16+(__buf)*8+4)

#define USB_RAM_XX_RXCOUNT_BLOCKS(__x) VALUE_(10,__x)
#define USB_RAM_XX_RXCOUNT_COUNT_MASK  VALUE_(0, 0x03FF)

//-----------------------------------------------------------------------------
// Set up USB I/O pin configurations.

#if (CYGHWR_DEVS_USB_CORTEXM_STM32_DISC_PIN < 0x10)
#define USB_DISC_PIN CYGHWR_HAL_STM32_GPIO \
  (A, (CYGHWR_DEVS_USB_CORTEXM_STM32_DISC_PIN & 0x0F), OUT_2MHZ, OUT_PUSHPULL)

#elif (CYGHWR_DEVS_USB_CORTEXM_STM32_DISC_PIN < 0x20)
#define USB_DISC_PIN CYGHWR_HAL_STM32_GPIO \
  (B, (CYGHWR_DEVS_USB_CORTEXM_STM32_DISC_PIN & 0x0F), OUT_2MHZ, OUT_PUSHPULL)

#elif (CYGHWR_DEVS_USB_CORTEXM_STM32_DISC_PIN < 0x30)
#define USB_DISC_PIN CYGHWR_HAL_STM32_GPIO \
  (C, (CYGHWR_DEVS_USB_CORTEXM_STM32_DISC_PIN & 0x0F), OUT_2MHZ, OUT_PUSHPULL)

#elif (CYGHWR_DEVS_USB_CORTEXM_STM32_DISC_PIN < 0x40)
#define USB_DISC_PIN CYGHWR_HAL_STM32_GPIO \
  (D, (CYGHWR_DEVS_USB_CORTEXM_STM32_DISC_PIN & 0x0F), OUT_2MHZ, OUT_PUSHPULL)

#elif (CYGHWR_DEVS_USB_CORTEXM_STM32_DISC_PIN < 0x50)
#define USB_DISC_PIN CYGHWR_HAL_STM32_GPIO \
  (E, (CYGHWR_DEVS_USB_CORTEXM_STM32_DISC_PIN & 0x0F), OUT_2MHZ, OUT_PUSHPULL)

#elif (CYGHWR_DEVS_USB_CORTEXM_STM32_DISC_PIN < 0x60)
#define USB_DISC_PIN CYGHWR_HAL_STM32_GPIO \
  (F, (CYGHWR_DEVS_USB_CORTEXM_STM32_DISC_PIN & 0x0F), OUT_2MHZ, OUT_PUSHPULL)

#elif (CYGHWR_DEVS_USB_CORTEXM_STM32_DISC_PIN < 0x70)
#define USB_DISC_PIN CYGHWR_HAL_STM32_GPIO \
  (G, (CYGHWR_DEVS_USB_CORTEXM_STM32_DISC_PIN & 0x0F), OUT_2MHZ, OUT_PUSHPULL)
#endif

#define USB_DP_PIN CYGHWR_HAL_STM32_GPIO (A, 12, IN, AIN)
#define USB_DM_PIN CYGHWR_HAL_STM32_GPIO (A, 11, IN, AIN)

//=============================================================================
// Define USB transmit endpoint data structures.
//=============================================================================

typedef enum {
  TXTR_FLAGS_NONE   = 0x00, // No flags set.
  TXTR_FLAGS_ZLPKT  = 0x01, // Set to enable zero length packet termination.
  TXTR_FLAGS_DBUF   = 0x02, // Set to enable doubtle buffered operation.
} txtr_flags;

typedef enum {
  TXTR_STATE_RESET,         // TX transaction reset state. 
  TXTR_STATE_IDLE,          // TX transaction idle (between transactions).
  TXTR_STATE_SB_NEXT_PKT,   // TX transaction send next packet (single buffer).
  TXTR_STATE_SB_ZERO_PKT,   // TX transaction send zero length packet (single buffer). 
  TXTR_STATE_SB_DONE,       // TX transaction complete (single buffer).
  TXTR_STATE_DB_FIRST_PKT,  // TX transaction queue first packet (double buffer).
  TXTR_STATE_DB_NEXT_PKT,   // TX transaction queue next packet (double buffer).
  TXTR_STATE_DB_ZERO_PKT,   // TX transaction queue zero length packet (double buffer).
  TXTR_STATE_DB_LAST_PKT,   // TX transaction send last queued packet (double buffer).
  TXTR_STATE_DB_DONE,       // TX transaction complete (double buffer).
} txtr_state;

typedef struct txtr_impl {
  txtr_state        state;             // Current transaction state.
  txtr_flags        flags;             // Transaction flags.
  cyg_uint16        status;            // Return status. 
  cyg_uint8         ep_num;            // Endpoint number.
  const cyg_uint8*  buf_ptr;           // Pointer to next user buffer location.
  cyg_uint32        buf_size;          // Size of user transmit buffer.
  cyg_uint32        bytes_sent;        // Track number of bytes sent. 
} txtr_impl;

typedef struct txep_impl {
  usbs_tx_endpoint  common;            // High level driver data.
  txtr_impl         txtr;              // Transaction data.
} txep_impl;

//=============================================================================
// Define USB receive endpoint data structures.
//=============================================================================

typedef enum {
  RXTR_FLAGS_NONE   = 0x00, // No flags set.
  RXTR_FLAGS_ZLPKT  = 0x01, // Set to enable zero length packet termination.
  RXTR_FLAGS_DBUF   = 0x02, // Set to enable doubtle buffered operation.
} rxtr_flags;

typedef enum {
  RXTR_STATE_RESET,         // RX transaction reset state.
  RXTR_STATE_IDLE,          // RX transaction idle (between transactions).
  RXTR_STATE_SB_NEXT_PKT,   // RX transaction get next packet (single buffer).
  RXTR_STATE_DB_NEXT_PKT,   // RX transaction get next packet (double buffer).
} rxtr_state;

typedef struct rxtr_impl {
  rxtr_state        state;             // Current transaction state.
  rxtr_flags        flags;             // Transaction flags.
  cyg_uint16        status;            // Return status. 
  cyg_uint8         ep_num;            // Endpoint number.
  cyg_uint8*        buf_ptr;           // Pointer to next user buffer location.
  cyg_uint32        buf_size;          // Size of receive buffer.
  cyg_uint32        bytes_rcvd;        // Track number of bytes received.
} rxtr_impl;

typedef struct rxep_impl {
  usbs_rx_endpoint  common;            // High level driver data.
  rxtr_impl         rxtr;              // Transaction data.
} rxep_impl;

//=============================================================================
// Instantiate USB control endpoint data structure.
//=============================================================================

static void stm32_usb_start (usbs_control_endpoint*);
static void stm32_usb_poll  (usbs_control_endpoint*);
static usbs_rx_endpoint* stm32_usb_get_rxep (usbs_control_endpoint*, cyg_uint8);
static usbs_tx_endpoint* stm32_usb_get_txep (usbs_control_endpoint*, cyg_uint8);

// Track the control endpoint state.
typedef enum {
  CTRLEP_MSG_STATE_IDLE,
  CTRLEP_MSG_STATE_IN_DATA,
  CTRLEP_MSG_STATE_IN_STATUS,
  CTRLEP_MSG_STATE_OUT_DATA,
  CTRLEP_MSG_STATE_OUT_STATUS,
  CTRLEP_MSG_STATE_CTRL_ACK,
} ctrlep_msg_states;

// Provide the control message buffer.
static cyg_uint8 ctrlep_msg_buffer [CYGNUM_DEVS_USB_CORTEXM_STM32_EPO_MAX_MSG_SIZE];

// Provide STM32 control endpoint implementation.
typedef struct ctrlep_impl {
  usbs_control_endpoint   common;      // High level driver data.
  txtr_impl               txtr;        // Transmit transaction data.
  rxtr_impl               rxtr;        // Receive transaction data.
  ctrlep_msg_states       msg_state;   // Control endpoint messaging state.
} ctrlep_impl;

// Instantiate control endpoint data structure.
static ctrlep_impl ctrlep = {
  { // Set up data for common high level driver.
    state                  : USBS_STATE_POWERED,
    enumeration_data       : (usbs_enumeration_data*) 0,
    start_fn               : &stm32_usb_start,
    poll_fn                : &stm32_usb_poll,
    interrupt_vector       : CYGNUM_HAL_INTERRUPT_USB_LP,
    control_buffer         : { 0, 0, 0, 0, 0, 0, 0, 0 },
    state_change_fn        : 0,
    state_change_data      : 0,
    standard_control_fn    : 0,
    standard_control_data  : 0,
    class_control_fn       : 0,
    class_control_data     : 0,
    vendor_control_fn      : 0,
    vendor_control_data    : 0,
    reserved_control_fn    : 0,
    reserved_control_data  : 0,
    buffer                 : 0,
    buffer_size            : 0,
    fill_buffer_fn         : 0,
    fill_data              : 0,
    fill_index             : 0,
    complete_fn            : 0,
    get_rxep_fn            : &stm32_usb_get_rxep,
    get_txep_fn            : &stm32_usb_get_txep,
  },
  { // Initialise transmit transaction data.
    state                  : TXTR_STATE_RESET,
    flags                  : TXTR_FLAGS_ZLPKT,
    status                 : ENOERR,
    ep_num                 : 0,
  },
  { // Initialise receive transaction data.
    state                  : RXTR_STATE_RESET,
    flags                  : RXTR_FLAGS_ZLPKT,
    status                 : ENOERR,
    ep_num                 : 0,
  },
  msg_state                : CTRLEP_MSG_STATE_IDLE,
};

extern usbs_control_endpoint cyg_usbs_cortexm_stm32_ep0c __attribute__((alias ("ctrlep")));

//=============================================================================
// ISR/DSR shared data structure.  All data which is shared between ISR and
// DSR contexts is wrapped up in this data structure to make synchronisation
// between the two more manageable.
//=============================================================================

typedef enum {
  ISR_FLAGS_CLEARED       = 0x00,      // No flags set.
  ISR_FLAGS_SETUP_READY   = 0x01,      // Setup packet ready in staging buffer.
  ISR_FLAGS_DEVICE_RESET  = 0x02,      // Device reset detected.
} isr_flags;

typedef struct isr_shared_data {
  isr_flags               flags;       // Flags indicating events pending.
  cyg_uint8               txtr_done;   // Flags for indicating TX complete.
  cyg_uint8               rxtr_done;   // Flags for indicating RX complete.
} isr_shared_data;

static isr_shared_data isr_shared = {
  flags                    : ISR_FLAGS_CLEARED,
  txtr_done                : 0,
  rxtr_done                : 0,
};

//=============================================================================
// USB driver internal state variables.
//=============================================================================

// Buffer management.
static cyg_uint16 stm32_usb_buf_offset;
static cyg_uint16 stm32_usb_buf_sizes [USB_EPNUM];

// Endpoint descriptors.
static txep_impl  txep_list [CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM];
static rxep_impl  rxep_list [CYGNUM_DEVS_USB_CORTEXM_STM32_RXEP_NUM];
static txep_impl* txep_map  [15];
static rxep_impl* rxep_map  [15];

// Interrupts and synchronisation primitives.
static cyg_interrupt   interrupt_data;
static cyg_handle_t    interrupt_handle;
static cyg_uint32      interrupt_mask_count;

//=============================================================================
// Provide inlineable functions for implementing counted interrupt masking.
//=============================================================================

//-----------------------------------------------------------------------------
// Mask interrupts if not already masked.

static inline void stm32_usb_request_intr_mask
  (void)
{
  cyg_scheduler_lock();
  if (interrupt_mask_count++ == 0) {
    cyg_drv_interrupt_mask (CYGNUM_HAL_INTERRUPT_USB_LP);
  }
  cyg_scheduler_unlock();
}

//-----------------------------------------------------------------------------
// Release interrupt mask.  Interrupts will be unmasked once all mask requests
// have been released.

static inline void stm32_usb_release_intr_mask
  (void)
{
  cyg_scheduler_lock();
  if (interrupt_mask_count == 0) {
    FAIL_USB ("Interrupt mask counter decremented through 0.");
  }
  else if (--interrupt_mask_count == 0) {
    cyg_drv_interrupt_unmask (CYGNUM_HAL_INTERRUPT_USB_LP);
  }
  cyg_scheduler_unlock();
}

//=============================================================================
// Provide inlineable functions for setting the 'flip bit' register values.
//=============================================================================

//-----------------------------------------------------------------------------
// Set the transmit status bits to the desired value. 

static inline void stm32_usb_set_txep_status
  (cyg_uint32 ep, cyg_uint32 txep_status)
{
  cyg_uint32 reg_val;
  HAL_READ_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (ep), reg_val);
  reg_val &= ~((cyg_uint32) (CYGHWR_HAL_STM32_USB_EPXR_STATRX_MASK |
    CYGHWR_HAL_STM32_USB_EPXR_DTOGTX | CYGHWR_HAL_STM32_USB_EPXR_DTOGRX)); 
  reg_val ^= txep_status & CYGHWR_HAL_STM32_USB_EPXR_STATTX_MASK;
  HAL_WRITE_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (ep), reg_val);
}

//-----------------------------------------------------------------------------
// Set the receive status bits to the desired value. 

static inline void stm32_usb_set_rxep_status
  (cyg_uint32 ep, cyg_uint32 rxep_status)
{
  cyg_uint32 reg_val;
  HAL_READ_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (ep), reg_val);
  reg_val &= ~((cyg_uint32) (CYGHWR_HAL_STM32_USB_EPXR_STATTX_MASK |
    CYGHWR_HAL_STM32_USB_EPXR_DTOGTX | CYGHWR_HAL_STM32_USB_EPXR_DTOGRX)); 
  reg_val ^= rxep_status & CYGHWR_HAL_STM32_USB_EPXR_STATRX_MASK;
  HAL_WRITE_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (ep), reg_val);
}

//-----------------------------------------------------------------------------
// Assign conventional bits in the endpoint status registers without touching
// the flip-bit values.

static inline void stm32_usb_assign_epxr
  (cyg_uint32 ep, cyg_uint32 epxr_val)
{
  epxr_val &= ~((cyg_uint32) (CYGHWR_HAL_STM32_USB_EPXR_STATTX_MASK |
    CYGHWR_HAL_STM32_USB_EPXR_STATRX_MASK | CYGHWR_HAL_STM32_USB_EPXR_DTOGTX | 
    CYGHWR_HAL_STM32_USB_EPXR_DTOGRX));
  HAL_WRITE_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (ep), epxr_val);
}

//-----------------------------------------------------------------------------
// Set specified conventional bits in the endpoint status registers.

static inline void stm32_usb_set_epxr_bits
  (cyg_uint32 ep, cyg_uint32 epxr_mask)
{
  cyg_uint32 reg_val;
  HAL_READ_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (ep), reg_val);
  reg_val |= epxr_mask;
  stm32_usb_assign_epxr (ep, reg_val);
}

//-----------------------------------------------------------------------------
// Clear specified conventional bits in the endpoint status registers.

static inline void stm32_usb_clear_epxr_bits
  (cyg_uint32 ep, cyg_uint32 epxr_mask)
{
  cyg_uint32 reg_val;
  HAL_READ_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (ep), reg_val);
  reg_val &= ~epxr_mask;
  stm32_usb_assign_epxr (ep, reg_val);
}

//-----------------------------------------------------------------------------
// Set specified toggle bits in the endpoint status registers.

static inline void stm32_usb_set_epxr_toggle
  (cyg_uint32 ep, cyg_uint32 epxr_mask)
{
  cyg_uint32 reg_val;
  HAL_READ_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (ep), reg_val);
  reg_val &= epxr_mask | ~((cyg_uint32) (CYGHWR_HAL_STM32_USB_EPXR_STATTX_MASK | 
    CYGHWR_HAL_STM32_USB_EPXR_STATRX_MASK | CYGHWR_HAL_STM32_USB_EPXR_DTOGTX | 
    CYGHWR_HAL_STM32_USB_EPXR_DTOGRX)); 
  reg_val ^= epxr_mask;
  HAL_WRITE_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (ep), reg_val);
}

//-----------------------------------------------------------------------------
// Clear specified toggle bits in the endpoint status registers.

static inline void stm32_usb_clear_epxr_toggle
  (cyg_uint32 ep, cyg_uint32 epxr_mask)
{
  cyg_uint32 reg_val;
  HAL_READ_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (ep), reg_val);
  reg_val &= epxr_mask | ~((cyg_uint32) (CYGHWR_HAL_STM32_USB_EPXR_STATTX_MASK | 
    CYGHWR_HAL_STM32_USB_EPXR_STATRX_MASK | CYGHWR_HAL_STM32_USB_EPXR_DTOGTX | 
    CYGHWR_HAL_STM32_USB_EPXR_DTOGRX)); 
  HAL_WRITE_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (ep), reg_val);
}

//-----------------------------------------------------------------------------
// Flip specified toggle bits in the endpoint status registers.

static inline void stm32_usb_flip_epxr_toggle
  (cyg_uint32 ep, cyg_uint32 epxr_mask)
{
  cyg_uint32 reg_val;
  HAL_READ_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (ep), reg_val);
  reg_val &= ~((cyg_uint32) (CYGHWR_HAL_STM32_USB_EPXR_STATTX_MASK | 
    CYGHWR_HAL_STM32_USB_EPXR_STATRX_MASK | CYGHWR_HAL_STM32_USB_EPXR_DTOGTX | 
    CYGHWR_HAL_STM32_USB_EPXR_DTOGRX));
  reg_val |= epxr_mask; 
  HAL_WRITE_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (ep), reg_val);
}

//=============================================================================
// The following set of functions provide buffer management capabilities for
// allocating and then accessing USB buffers in the dual-port buffer RAM.
//=============================================================================

//-----------------------------------------------------------------------------
// Get the allocated buffer size for a specified endpoint.

static inline cyg_uint32 stm32_usb_buf_get_size
  (cyg_uint32 ep)
{
  return (stm32_usb_buf_sizes[ep]);
}

//-----------------------------------------------------------------------------
// Add a new set of endpoint buffers for the specified endpoint.

static cyg_bool stm32_usb_buf_add_ep
  (cyg_uint32 ep, cyg_uint32 ep_buf_size, cyg_bool is_tx, cyg_bool is_rx, cyg_bool is_db)
{
  cyg_uint32 alloc_size;
  cyg_uint32 rx_blocks;

  // Check for valid configuration.
  if ((is_db && is_tx && is_rx) || !(is_tx || is_rx)) {
    FAIL_USB ("Invalid buffer configuration.");
    goto failed;
  }

  // Round up the allocation size so that it matches a valid RX block size.
  alloc_size = (ep_buf_size & 1) ? ep_buf_size + 1 : ep_buf_size;
  if ((alloc_size > 62) && (alloc_size & 31)) {
    alloc_size &= ~31;
    alloc_size += 32;
  }

  // Check to see if there is enough RAM available.
  if (is_db || (is_tx && is_rx)) {
    if (stm32_usb_buf_offset + 2 * alloc_size > USB_RAM_SIZE) {
      FAIL_USB ("Insufficient endpoint RAM for configuration.");
      goto failed;
    }
  }
  else {  
    if (stm32_usb_buf_offset + alloc_size > USB_RAM_SIZE) {
      FAIL_USB ("Insufficient endpoint RAM for configuration.");
      goto failed;
    }
  }

  // Calculate the blocks (size + num) field for the receive count register.
  rx_blocks = (alloc_size <= 62) ? (alloc_size / 2) : (31 + alloc_size / 32);
  stm32_usb_buf_sizes[ep] = (cyg_uint16) ep_buf_size;
 
  // Program up the pseudo-registers for double buffered transmit.
  if (is_db && is_tx) {
    HAL_WRITE_UINT32 (USB_RAM_BASE + USB_RAM_DB_TXADDR (ep, 0), stm32_usb_buf_offset);
    HAL_WRITE_UINT32 (USB_RAM_BASE + USB_RAM_DB_TXCOUNT (ep, 0), 0);
    stm32_usb_buf_offset += (cyg_uint16) alloc_size;
    HAL_WRITE_UINT32 (USB_RAM_BASE + USB_RAM_DB_TXADDR (ep, 1), stm32_usb_buf_offset);
    HAL_WRITE_UINT32 (USB_RAM_BASE + USB_RAM_DB_TXCOUNT (ep, 1), 0);
    stm32_usb_buf_offset += (cyg_uint16) alloc_size;
  }

  // Program up the pseudo-registers for double buffered receive.
  else if (is_db) {
    HAL_WRITE_UINT32 (USB_RAM_BASE + USB_RAM_DB_RXADDR (ep, 0), stm32_usb_buf_offset);
    HAL_WRITE_UINT32 (USB_RAM_BASE + USB_RAM_DB_RXCOUNT (ep, 0), USB_RAM_XX_RXCOUNT_BLOCKS (rx_blocks));
    stm32_usb_buf_offset += (cyg_uint16) alloc_size;
    HAL_WRITE_UINT32 (USB_RAM_BASE + USB_RAM_DB_RXADDR (ep, 1), stm32_usb_buf_offset);
    HAL_WRITE_UINT32 (USB_RAM_BASE + USB_RAM_DB_RXCOUNT (ep, 1), USB_RAM_XX_RXCOUNT_BLOCKS (rx_blocks));
    stm32_usb_buf_offset += (cyg_uint16) alloc_size;
  }

  // Program up the pseudo-registers for single buffered transmit/receive.
  else {
    if (is_tx) {
      HAL_WRITE_UINT32 (USB_RAM_BASE + USB_RAM_SB_TXADDR (ep), stm32_usb_buf_offset);
      HAL_WRITE_UINT32 (USB_RAM_BASE + USB_RAM_SB_TXCOUNT (ep), 0);
      stm32_usb_buf_offset += (cyg_uint16) alloc_size;
    }
    if (is_rx) {
      HAL_WRITE_UINT32 (USB_RAM_BASE + USB_RAM_SB_RXADDR (ep), stm32_usb_buf_offset);
      HAL_WRITE_UINT32 (USB_RAM_BASE + USB_RAM_SB_RXCOUNT (ep), USB_RAM_XX_RXCOUNT_BLOCKS (rx_blocks));
      stm32_usb_buf_offset += (cyg_uint16) alloc_size;
    }
  }

  // Exit on success or failure.
  return true;
failed :
  return false;
}

//-----------------------------------------------------------------------------
// Clear the buffer RAM layout for non-control endpoints, prior to setting a
// new configuration.

static inline void stm32_usb_buf_clear_config
  (void)
{
  cyg_uint32 i;

  // Clear buffer size table for non-control endpoints.
  for (i = 1; i < USB_EPNUM; i++)
    stm32_usb_buf_sizes[i] = 0;    

  // Update the free pointer to point to the end of the control endpoint buffers.
  stm32_usb_buf_offset = 8 * USB_EPNUM + 2 * stm32_usb_buf_sizes[0];
}

//-----------------------------------------------------------------------------
// Reset the buffer RAM layout, preallocating the requested buffer area for
// control endpoint 0.

static inline cyg_bool stm32_usb_buf_reset_ep0
  (cyg_uint32 ep0_buf_size)
{
  cyg_uint32 i;

  // The buffer descriptor table is placed at the start of the dual-port RAM.
  HAL_WRITE_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_BTABLE, 0);
  stm32_usb_buf_offset = 8 * USB_EPNUM;

  // Clear buffer size table prior to adding endpoint 0.  A zero in this table
  // essentially prevents any host-side access to a given endpoint buffer.
  for (i = 0; i < USB_EPNUM; i++)
    stm32_usb_buf_sizes[i] = 0;    
  return stm32_usb_buf_add_ep (0, ep0_buf_size, true, true, false);
}

//-----------------------------------------------------------------------------
// Copy data from user memory to a specified USB buffer (double buffered).
// Returns the number of bytes transferred, which is capped at the buffer
// size.

static cyg_uint32 __attribute__((hot)) stm32_usb_copy_to_dbuf
  (const cyg_uint8* src, cyg_uint32 size, cyg_uint32 ep, cyg_uint32 buf)
{
  cyg_uint32     data, i;
  cyg_haladdress waddr;

  // Truncate the size parameter to the buffer length.
  if (size > stm32_usb_buf_sizes[ep])
    size = stm32_usb_buf_sizes[ep];

  // Get the offset of the start of the buffer from the buffer tables
  // and convert it into a host-side address.
  HAL_READ_UINT32 (USB_RAM_BASE + USB_RAM_DB_TXADDR (ep, buf), data);
  waddr = USB_RAM_BASE + data * 2;

  // Copy over the data, remembering to skip the half-word gaps.  If the size
  // is not an integer number of half-words, we stick garbage in the last byte.
  for (i = (size + 1) >> 1; i != 0; i--) {
    data = *(src++);
    data |= ((cyg_uint32) *(src++)) << 8;
    HAL_WRITE_UINT32 (waddr, data);
    waddr += 4;
  }

  // Update the buffer count field and return the buffer size.
  HAL_WRITE_UINT32 (USB_RAM_BASE + USB_RAM_DB_TXCOUNT (ep, buf), size);
  return size;
} 

//-----------------------------------------------------------------------------
// Copy data from user memory to transmit buffer (single buffered endpoint).
// Returns the number of bytes transferred, which is capped at the buffer
// size.

static inline cyg_uint32 stm32_usb_copy_to_sbuf
  (const cyg_uint8* src, cyg_uint32 size, cyg_uint32 ep)
{
  return stm32_usb_copy_to_dbuf (src, size, ep, 0);
}

//-----------------------------------------------------------------------------
// Copy data from a specified USB buffer to user memory (double buffered).
// Returns the number of bytes held by the buffer.  Will not write beyond the
// end of the user buffer, but if the user buffer is not large enough to hold 
// the received data (returned value > size) this implies an error condition.

static cyg_uint32 __attribute__((hot)) stm32_usb_copy_from_dbuf
  (cyg_uint8* dest, cyg_uint32 size, cyg_uint32 ep, cyg_uint32 buf)
{
  cyg_uint32     data, bufsize, i;
  cyg_haladdress raddr;

  // Get the receive buffer size.
  HAL_READ_UINT32 (USB_RAM_BASE + USB_RAM_DB_RXCOUNT (ep, buf), bufsize);
  bufsize &= USB_RAM_XX_RXCOUNT_COUNT_MASK;

  // Determine the actual amount of data to transfer.
  if (bufsize > size)
    FAIL_USB ("Receive buffer overflow detected.");
  else
    size = bufsize;

  // Get the offset of the start of the buffer from the buffer tables
  // and convert it into a host-side address.
  HAL_READ_UINT32 (USB_RAM_BASE + USB_RAM_DB_RXADDR (ep, buf), data);
  raddr = USB_RAM_BASE + data * 2;

  // Copy over the half-word aligned data.
  for (i = size; i > 1; i -= 2) {
    HAL_READ_UINT32 (raddr, data);
    *(dest++) = (cyg_uint8) data;
    *(dest++) = (cyg_uint8) (data >> 8);
    raddr += 4;
  }

  // Copy over the trailing byte if present.
  if (i) {
    HAL_READ_UINT32 (raddr, data);
    *(dest) = (cyg_uint8) data;
  }
  return bufsize;
}

//-----------------------------------------------------------------------------
// Copy data from a receive buffer to user memory (single buffered endpoint).
// Returns the number of bytes held by the buffer.  Will not write beyond the
// end of the user buffer, but if the user buffer is not large enough to hold 
// the received data (returned value > size) this implies an error condition.

static inline cyg_uint32 stm32_usb_copy_from_sbuf
  (cyg_uint8* dest, cyg_uint32 size, cyg_uint32 ep)
{
  return stm32_usb_copy_from_dbuf (dest, size, ep, 1);
}

//=============================================================================
// The following set of functions provide support for managing multi-packet
// USB transactions.
//=============================================================================

//-----------------------------------------------------------------------------
// Implement transmit transaction state machine.  This is called on transaction
// start and then on all applicable interrupt events in order to progress the 
// transmit transaction.

static cyg_bool stm32_usb_txtr_run
  (txtr_impl* txtr)
{
  cyg_bool completed = false;
  cyg_uint32 tx_bytes_req, tx_bytes_sent, buf_sel, reg_val;

  switch (txtr->state) {

    // Send next packet (single buffer transfers).
    case TXTR_STATE_SB_NEXT_PKT:
      tx_bytes_req = txtr->buf_size - txtr->bytes_sent;
      tx_bytes_sent = stm32_usb_copy_to_sbuf (txtr->buf_ptr, tx_bytes_req, txtr->ep_num);
      txtr->buf_ptr += tx_bytes_sent;
      txtr->bytes_sent += tx_bytes_sent;

      // All bytes sent - see if we need a zero length termination packet.
      if (txtr->bytes_sent == txtr->buf_size) {
        if ((tx_bytes_sent == stm32_usb_buf_get_size (txtr->ep_num)) && (txtr->flags & TXTR_FLAGS_ZLPKT))
          txtr->state = TXTR_STATE_SB_ZERO_PKT;
        else
          txtr->state = TXTR_STATE_SB_DONE;
      }

      // Set buffer valid via the endpoint control register.
      stm32_usb_set_txep_status (txtr->ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATTX_VALID);
      break;

    // Send zero length termination packet (single buffer transfers).
    case TXTR_STATE_SB_ZERO_PKT :
      txtr->state = TXTR_STATE_SB_DONE;
      stm32_usb_copy_to_sbuf (txtr->buf_ptr, 0, txtr->ep_num);
      stm32_usb_set_txep_status (txtr->ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATTX_VALID);
      break;

    // Complete transaction (single buffer transfers).
    case TXTR_STATE_SB_DONE :
      txtr->state = TXTR_STATE_IDLE;
      completed = true;
      break;

    // Sets up the first queued packet.  Start by clearing the data toggle
    // bits and leaving the endpoint in valid state.
    case TXTR_STATE_DB_FIRST_PKT :
      HAL_READ_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (txtr->ep_num), reg_val);
      buf_sel = (reg_val & CYGHWR_HAL_STM32_USB_EPXR_DTOGTX) ? 1 : 0;  
      goto queue_packet;

    // Commit the next queued packet for transmission then queue the next 
    // packet in the CPU-side buffer.
    case TXTR_STATE_DB_NEXT_PKT :
      HAL_READ_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (txtr->ep_num), reg_val);
      buf_sel = (reg_val & CYGHWR_HAL_STM32_USB_EPXR_DTOGTX) ? 0 : 1;  
      stm32_usb_flip_epxr_toggle (txtr->ep_num, CYGHWR_HAL_STM32_USB_EPXR_SWBUFTX);
      stm32_usb_set_txep_status (txtr->ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATTX_VALID);

    // This section is common to both DB_NEXT_PKT and DB_FIRST_PKT states.  
    // However, a straight fallthrough won't work and we have to resort to goto.
    queue_packet:
      tx_bytes_req = txtr->buf_size - txtr->bytes_sent;
      tx_bytes_sent = stm32_usb_copy_to_dbuf (txtr->buf_ptr, tx_bytes_req, txtr->ep_num, buf_sel);
      txtr->buf_ptr += tx_bytes_sent;
      txtr->bytes_sent += tx_bytes_sent;

      // All bytes queued - see if we need a zero length termination packet.
      if (txtr->bytes_sent == txtr->buf_size) {
        if ((tx_bytes_sent == stm32_usb_buf_get_size (txtr->ep_num)) && (txtr->flags & TXTR_FLAGS_ZLPKT))
          txtr->state = TXTR_STATE_DB_ZERO_PKT;
        else
          txtr->state = TXTR_STATE_DB_LAST_PKT;
      }

      // More data remaining - send next packet.
      else {
        txtr->state = TXTR_STATE_DB_NEXT_PKT;
      }
      break;

    // Commit the next queued packet for transmission then queue a
    // zero length packet in the CPU-side buffer.
    case TXTR_STATE_DB_ZERO_PKT :
      HAL_READ_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (txtr->ep_num), reg_val);
      buf_sel = (reg_val & CYGHWR_HAL_STM32_USB_EPXR_DTOGTX) ? 0 : 1;  
      stm32_usb_flip_epxr_toggle (txtr->ep_num, CYGHWR_HAL_STM32_USB_EPXR_SWBUFTX);
      stm32_usb_set_txep_status (txtr->ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATTX_VALID);
      stm32_usb_copy_to_dbuf (txtr->buf_ptr, 0, txtr->ep_num, buf_sel);
      txtr->state = TXTR_STATE_DB_LAST_PKT;
      break;

    // Commit the final queued packet for transmission.
    case TXTR_STATE_DB_LAST_PKT :
      stm32_usb_flip_epxr_toggle (txtr->ep_num, CYGHWR_HAL_STM32_USB_EPXR_SWBUFTX);
      stm32_usb_set_txep_status (txtr->ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATTX_VALID);
      txtr->state = TXTR_STATE_DB_DONE;
      break;

    // Complete transaction (double buffer transfers).
    case TXTR_STATE_DB_DONE :
      stm32_usb_set_txep_status (txtr->ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATTX_NAK);
      txtr->state = TXTR_STATE_IDLE;
      completed = true;
      break;

    // Unknown state - driver error.
    default :
      FAIL_USB ("TX transaction in invalid state.");
      txtr->state = TXTR_STATE_IDLE;
      txtr->status = EIO;
      completed = true;
      break;
  }
  return completed;
}

//-----------------------------------------------------------------------------
// Initiate a data transmit transaction.  This sets up a new transaction,
// priming the buffers and then kicking the state machine for the first time.

static cyg_bool stm32_usb_txtr_start
  (txtr_impl* txtr, cyg_bool int_safe) 
{
  cyg_bool completed;

  // Critical section - avoid races with the ISR.
  if (!int_safe) stm32_usb_request_intr_mask ();

  // Set up double buffer transactions.  We need to prime the buffers which
  // requires two ticks of the state machine.
  if (txtr->flags & TXTR_FLAGS_DBUF) {
    txtr->status = ENOERR;
    txtr->bytes_sent = 0;
    txtr->state = TXTR_STATE_DB_FIRST_PKT;
    stm32_usb_txtr_run (txtr);
  }

  // Set up single buffer transactions.
  else {
    txtr->status = ENOERR;
    txtr->bytes_sent = 0;
    txtr->state = TXTR_STATE_SB_NEXT_PKT;
  }

  // Run the state machine for the first step.
  completed = stm32_usb_txtr_run (txtr);  

  // Exit critical section.
  if (!int_safe) stm32_usb_release_intr_mask ();
  return completed;
}

//-----------------------------------------------------------------------------
// Halt a transmit endpoint.  This places the endpoint in the stall condition 
// and cancels any outstanding transaction, resetting the transaction state 
// machine.

static void stm32_usb_txep_halt
  (txep_impl* txep, cyg_bool int_safe)
{
  cyg_bool call_completion;

  // Critical section - avoid races with the ISR.
  if (!int_safe) stm32_usb_request_intr_mask ();
  stm32_usb_set_txep_status (txep->txtr.ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATTX_DIS);

  // Reset the transaction state.
  call_completion = ((txep->txtr.state != TXTR_STATE_IDLE) && 
    (txep->txtr.state != TXTR_STATE_RESET)) ? true : false;
  txep->txtr.state = TXTR_STATE_RESET;
  txep->common.halted = true;
  stm32_usb_clear_epxr_bits (txep->txtr.ep_num, CYGHWR_HAL_STM32_USB_EPXR_CTRTX);

  // Exit critical section.
  stm32_usb_set_txep_status (txep->txtr.ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATTX_STALL);
  if (!int_safe) stm32_usb_release_intr_mask ();
  
  // Indicate error via the completion callback.
  if (call_completion) {
    if (txep->common.complete_fn)
      (*txep->common.complete_fn) (txep->common.complete_data, -EIO);
    TRACE_USB ("TX Transaction cancelled on halt.\n");
  }
}

//-----------------------------------------------------------------------------
// Take a transmit endpoint out of halted state.  This takes the endpoint out
// of the halted state and resets the endpoint.  Note that according to the
// spec, the endpoint should also be reset and the toggle bits cleared if 
// 'clear halted' is called while in normal operation.

static void stm32_usb_txep_unhalt
  (txep_impl* txep, cyg_bool int_safe)
{
  cyg_bool call_completion;

  // Critical section - avoid races with the ISR.
  if (!int_safe) stm32_usb_request_intr_mask ();
  stm32_usb_set_txep_status (txep->txtr.ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATTX_DIS);

  // Reset the transaction state.
  call_completion = ((txep->txtr.state != TXTR_STATE_IDLE) && 
    (txep->txtr.state != TXTR_STATE_RESET)) ? true : false;
  txep->txtr.state = TXTR_STATE_IDLE;
  txep->common.halted = false;
  stm32_usb_clear_epxr_bits (txep->txtr.ep_num, CYGHWR_HAL_STM32_USB_EPXR_CTRTX);
  stm32_usb_clear_epxr_toggle (txep->txtr.ep_num, 
    CYGHWR_HAL_STM32_USB_EPXR_DTOGTX | CYGHWR_HAL_STM32_USB_EPXR_SWBUFTX);

  // Exit critical section.
  stm32_usb_set_txep_status (txep->txtr.ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATTX_NAK);
  if (!int_safe) stm32_usb_release_intr_mask ();
  
  // Indicate error via the completion callback.
  if (call_completion) {
    if (txep->common.complete_fn)
      (*txep->common.complete_fn) (txep->common.complete_data, -EIO);
    TRACE_USB ("TX Transaction cancelled on resume.\n");
  }
}

//-----------------------------------------------------------------------------
// Implement receive transaction state machine.  This is called on all
// applicable interrupt events in order to progress the transaction.

static cyg_bool stm32_usb_rxtr_run
  (rxtr_impl* rxtr)
{
  cyg_bool completed = false;
  cyg_uint32 rx_bytes_req, rx_bytes_rcvd, buf_sel, reg_val;

  switch (rxtr->state) {

    // Receive new packet (single buffer transfers).
    case RXTR_STATE_SB_NEXT_PKT:
      rx_bytes_req = rxtr->buf_size - rxtr->bytes_rcvd;
      rx_bytes_rcvd = stm32_usb_copy_from_sbuf (rxtr->buf_ptr, rx_bytes_req, rxtr->ep_num);

      // Check for buffer overflow condition before updating buffer pointer.
      if (rx_bytes_rcvd > rx_bytes_req) {
        FAIL_USB ("RX message exceeds allocated buffer size.");
        rxtr->state = RXTR_STATE_IDLE;
        rxtr->status = EMSGSIZE;
        completed = true;
        break;
      }
      rxtr->buf_ptr += rx_bytes_rcvd;
      rxtr->bytes_rcvd += rx_bytes_rcvd;

      // Short packet received - transaction complete.
      if (rx_bytes_rcvd < stm32_usb_buf_get_size (rxtr->ep_num)) {
        rxtr->state = RXTR_STATE_IDLE;
        completed = true;
        break;
      }

      // All bytes received - see if we need a zero length termination packet.
      // Note that zero length packets can be treated as normal short packets
      // here, so an additional state transition is not required.
      if (rxtr->bytes_rcvd == rxtr->buf_size) {
        if (!((rx_bytes_rcvd == stm32_usb_buf_get_size (rxtr->ep_num)) && (rxtr->flags & RXTR_FLAGS_ZLPKT))) {
          rxtr->state = RXTR_STATE_IDLE;
          completed = true;
          break;
        }
      }

      // Set buffer clear via the endpoint control register.
      stm32_usb_set_rxep_status (rxtr->ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATRX_VALID);
      break;

    // Receive new packet (double buffer transfers).  Start by switching buffers.
    case RXTR_STATE_DB_NEXT_PKT:
      HAL_READ_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (rxtr->ep_num), reg_val);
      buf_sel = (reg_val & CYGHWR_HAL_STM32_USB_EPXR_DTOGRX) ? 0 : 1;  

      // Get the receive buffer size.
      HAL_READ_UINT32 (USB_RAM_BASE + USB_RAM_DB_RXCOUNT (rxtr->ep_num, buf_sel), rx_bytes_rcvd);
      rx_bytes_req = rxtr->buf_size - rxtr->bytes_rcvd;
      rx_bytes_rcvd &= USB_RAM_XX_RXCOUNT_COUNT_MASK;
 
      // Check for buffer overflow condition before updating buffer pointer.
      if (rx_bytes_rcvd > rx_bytes_req) {
        FAIL_USB ("RX message exceeds allocated buffer size.");
        rxtr->state = RXTR_STATE_IDLE;
        rxtr->status = EMSGSIZE;
        completed = true;
        break;
      }

      // Short packet received - transaction complete.
      if (rx_bytes_rcvd < stm32_usb_buf_get_size (rxtr->ep_num)) {
        rxtr->state = RXTR_STATE_IDLE;
        completed = true;
      }

      // All bytes received - see if we need a zero length termination packet.
      // Note that zero length packets can be treated as normal short packets
      // here, so an additional state transition is not required.
      if (rxtr->bytes_rcvd + rx_bytes_rcvd == rxtr->buf_size) {
        if (!((rx_bytes_rcvd == stm32_usb_buf_get_size (rxtr->ep_num)) && (rxtr->flags & RXTR_FLAGS_ZLPKT))) {
          rxtr->state = RXTR_STATE_IDLE;
          completed = true;
        }
      }

      // Only enable the next receive buffer if this is not the last packet.
      if (!completed) {
        stm32_usb_flip_epxr_toggle (rxtr->ep_num, CYGHWR_HAL_STM32_USB_EPXR_SWBUFRX);
        stm32_usb_set_rxep_status (rxtr->ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATRX_VALID);
      }

      // Set the endpoint to NAK on completion.
      else {
        stm32_usb_set_rxep_status (rxtr->ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATRX_NAK);
      }

      // Copy the received data to our local buffer.
      rx_bytes_rcvd = stm32_usb_copy_from_dbuf (rxtr->buf_ptr, rx_bytes_req, rxtr->ep_num, buf_sel);
      rxtr->bytes_rcvd += rx_bytes_rcvd;
      rxtr->buf_ptr += rx_bytes_rcvd;
      break;

    // Unknown state - driver error.
    default :
      FAIL_USB ("RX transaction in invalid state.");
      rxtr->state = RXTR_STATE_IDLE;
      rxtr->status = EIO;
      completed = true;
      break;

  }
  return completed;
}

//-----------------------------------------------------------------------------
// Initiate a data receive transaction.  This sets up a new transaction,
// enabling the receive buffers and then waiting for incoming data.

static cyg_bool stm32_usb_rxtr_start
  (rxtr_impl* rxtr, cyg_bool int_safe)
{
  // Critical section - avoid races with the ISR.
  if (!int_safe) stm32_usb_request_intr_mask ();

  // Set up double buffer transactions.
  if (rxtr->flags & RXTR_FLAGS_DBUF) {
    rxtr->state = RXTR_STATE_DB_NEXT_PKT;
    rxtr->status = ENOERR;
    rxtr->bytes_rcvd = 0;
    stm32_usb_flip_epxr_toggle (rxtr->ep_num, CYGHWR_HAL_STM32_USB_EPXR_SWBUFRX);
    stm32_usb_set_rxep_status (rxtr->ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATRX_VALID);
  }

  // Set up single buffer transactions.
  else {
    rxtr->state = RXTR_STATE_SB_NEXT_PKT;
    rxtr->status = ENOERR;
    rxtr->bytes_rcvd = 0;
    stm32_usb_set_rxep_status (rxtr->ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATRX_VALID);
  }

  // Exit critical section.
  if (!int_safe) stm32_usb_release_intr_mask ();
  return false;
}

//-----------------------------------------------------------------------------
// Halt a receive endpoint.  This places the endpoint in the stall condition 
// and cancels any outstanding transaction, resetting the transaction state 
// machine.

static void stm32_usb_rxep_halt
  (rxep_impl* rxep, cyg_bool int_safe)
{
  cyg_bool call_completion;

  // Critical section - avoid races with the ISR.
  if (!int_safe) stm32_usb_request_intr_mask ();
  stm32_usb_set_rxep_status (rxep->rxtr.ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATRX_DIS);

  // Reset the transaction state.
  call_completion = ((rxep->rxtr.state != RXTR_STATE_IDLE) && 
    (rxep->rxtr.state != RXTR_STATE_RESET)) ? true : false;
  rxep->rxtr.state = RXTR_STATE_RESET;
  rxep->common.halted = true;
  stm32_usb_clear_epxr_bits (rxep->rxtr.ep_num, CYGHWR_HAL_STM32_USB_EPXR_CTRRX);

  // Exit critical section.
  stm32_usb_set_rxep_status (rxep->rxtr.ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATRX_STALL);
  if (!int_safe) stm32_usb_release_intr_mask ();
  
  // Indicate error via the completion callback.
  if (call_completion) {
    if (rxep->common.complete_fn)
      (*rxep->common.complete_fn) (rxep->common.complete_data, -EIO);
    TRACE_USB ("RX Transaction cancelled on halt.\n");
  }
}

//-----------------------------------------------------------------------------
// Take a receive endpoint out of halted state.  This takes the endpoint out
// of the halted state and resets the endpoint.  Note that according to the
// spec, the endpoint should also be reset and the toggle bits cleared if 
// 'clear halted' is called while in normal operation.

static void stm32_usb_rxep_unhalt
  (rxep_impl* rxep, cyg_bool int_safe)
{
  cyg_bool call_completion;

  // Critical section - avoid races with the ISR.
  if (!int_safe) stm32_usb_request_intr_mask ();
  stm32_usb_set_rxep_status (rxep->rxtr.ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATRX_DIS);

  // Reset the transaction state.
  call_completion = ((rxep->rxtr.state != RXTR_STATE_IDLE) && 
    (rxep->rxtr.state != RXTR_STATE_RESET)) ? true : false;
  rxep->rxtr.state = RXTR_STATE_IDLE;
  rxep->common.halted = false;
  stm32_usb_clear_epxr_bits (rxep->rxtr.ep_num, CYGHWR_HAL_STM32_USB_EPXR_CTRRX);
  stm32_usb_clear_epxr_toggle (rxep->rxtr.ep_num, 
    CYGHWR_HAL_STM32_USB_EPXR_DTOGRX | CYGHWR_HAL_STM32_USB_EPXR_SWBUFRX);

  // Exit critical section.
  stm32_usb_set_rxep_status (rxep->rxtr.ep_num, CYGHWR_HAL_STM32_USB_EPXR_STATRX_NAK);
  if (!int_safe) stm32_usb_release_intr_mask ();
  
  // Indicate error via the completion callback.
  if (call_completion) {
    if (rxep->common.complete_fn)
      (*rxep->common.complete_fn) (rxep->common.complete_data, -EIO);
    TRACE_USB ("RX Transaction cancelled on resume.\n");
  }
}

//=============================================================================
// Implement control endpoint protocol handling.
//=============================================================================

//-----------------------------------------------------------------------------
// Forward endpoint 0 state change notifications.

static void stm32_usb_ctrl_update_state
  (int new_state, usbs_state_change state_change)
{
  int old_state = ctrlep.common.state;
  ctrlep.common.state = new_state;
  if (ctrlep.common.state_change_fn)
    (*ctrlep.common.state_change_fn) (&ctrlep.common, 
      ctrlep.common.state_change_data, state_change, old_state);
}

// --------------------------------------------------------------------------
// Called on completion of an endpoint 0 control request transaction.

static void stm32_usb_ctrl_completed 
  (int status)
{
  ctrlep.msg_state = CTRLEP_MSG_STATE_IDLE;
  if (ctrlep.common.complete_fn)
    (*ctrlep.common.complete_fn) (&cyg_usbs_cortexm_stm32_ep0c, -status);
  if (status != 0)
    TRACE_USB ("Transaction failed (status %d).\n", status);
}

//-----------------------------------------------------------------------------
// Clear endpoint configuration.  This disables all non-control endpoints,
// resetting their state.

static void stm32_usb_ctrl_clear_config
  (void)
{
  cyg_uint32 i;
  txep_impl* txep = txep_list;
  rxep_impl* rxep = rxep_list;

  // Disable all non-control endpoints, clearing any outstanding interrupts.
  // This also ensures that the toggle bits are reset to 0.
  for (i = 1; i < 8; i++) {
    stm32_usb_set_txep_status (i, CYGHWR_HAL_STM32_USB_EPXR_STATTX_DIS);
    stm32_usb_set_rxep_status (i, CYGHWR_HAL_STM32_USB_EPXR_STATRX_DIS);
    stm32_usb_clear_epxr_toggle (i, 
      CYGHWR_HAL_STM32_USB_EPXR_DTOGTX | CYGHWR_HAL_STM32_USB_EPXR_DTOGRX);
    stm32_usb_clear_epxr_bits (i, 0xFFFF);
  }

  // Reset the transaction state for all transmit endpoints.
  for (i = 0; i < CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM; i++) {
    txep->common.halted = true;
    txep->txtr.state = TXTR_STATE_RESET;
    txep++;
  }

  // Reset the transaction state for all receive endpoints.
  for (i = 0; i < CYGNUM_DEVS_USB_CORTEXM_STM32_RXEP_NUM; i++) {
    rxep->common.halted = true;
    rxep->rxtr.state = RXTR_STATE_RESET;
    rxep++;
  }

  // Clear the logical to physical endpoint mappings.
  for (i = 0; i < 15; i++) {
    txep_map [i] = NULL;
    rxep_map [i] = NULL;
  }

  // Clear buffer RAM for non-control endpoints (preseverves endpoint 0).
  stm32_usb_buf_clear_config ();
}

//-----------------------------------------------------------------------------
// Reset the control endpoint - placing the device in the 'default' state.

static void stm32_usb_ctrl_reset
  (void)
{
  // Re-enable the device on address 0 only.
  HAL_WRITE_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_DADDR, CYGHWR_HAL_STM32_USB_DADDR_EF);
  
  // Reset the buffer RAM layout, allocating only the endpoint 0 buffers.
  stm32_usb_buf_reset_ep0 (ctrlep.common.enumeration_data->device.max_packet_size);

  // Clear the previous endpoint configuration.
  stm32_usb_ctrl_clear_config ();

  // Ensure endpoint 0 is assigned the correct endpoint type.
  stm32_usb_assign_epxr (0, CYGHWR_HAL_STM32_USB_EPXR_EPTYPE_CTRL);

  // Reset endpoint 0, leaving it configured for valid incoming packets.
  stm32_usb_set_txep_status (0, CYGHWR_HAL_STM32_USB_EPXR_STATTX_NAK);
  stm32_usb_set_rxep_status (0, CYGHWR_HAL_STM32_USB_EPXR_STATRX_VALID);
  stm32_usb_clear_epxr_toggle (0, 
    CYGHWR_HAL_STM32_USB_EPXR_DTOGTX | CYGHWR_HAL_STM32_USB_EPXR_DTOGRX);
  stm32_usb_clear_epxr_bits (0, 
    CYGHWR_HAL_STM32_USB_EPXR_CTRTX | CYGHWR_HAL_STM32_USB_EPXR_CTRRX);
  
  // Enter default state.
  stm32_usb_ctrl_update_state (USBS_STATE_DEFAULT, USBS_STATE_CHANGE_RESET);
}

//-----------------------------------------------------------------------------
// Fill the transmit control message buffer.  This function is used to
// assemble control response messages for transmission to the host.

static cyg_uint32 stm32_usb_ctrl_fill_msg_buffer 
  (void)
{
  cyg_uint32 msg_length = 0;

  // Deal with commands which place their data directly into the staging buffer.
  if (ctrlep.common.buffer == ctrlep_msg_buffer) {
    msg_length = ctrlep.common.buffer_size;
  }

  // Loop until there are no more message segments to append.  For buffer
  // overflows, debug builds will assert and production builds will send a 
  // truncated message.
  else do {
    if (ctrlep.common.buffer_size != 0) {
      if (msg_length + ctrlep.common.buffer_size <= CYGNUM_DEVS_USB_CORTEXM_STM32_EPO_MAX_MSG_SIZE) {
        memcpy (ctrlep_msg_buffer + msg_length, ctrlep.common.buffer, ctrlep.common.buffer_size);
        msg_length += ctrlep.common.buffer_size;
      }
      else {
        FAIL_USB ("Endpoint 0 transmit buffer overflow.");
        break;
      }

      // Get the next message segment, if available.
      if (ctrlep.common.fill_buffer_fn)
        (*ctrlep.common.fill_buffer_fn) (&cyg_usbs_cortexm_stm32_ep0c);
    }
  } while (ctrlep.common.fill_buffer_fn);
  
  return msg_length;
}

//-----------------------------------------------------------------------------
// Handle set configuration setup packets.  This is implemented in the low
// level driver because this is the point at which we need to configure the
// endpoint RAM and realise the endpoints.  It 'falls through' to the high
// level handlers so that they can deal with notifying the application.
// Note: Only a single (default) interface is supported per configuration.

static usbs_control_return stm32_usb_ctrl_set_config
  (cyg_uint32 config_id)
{
  cyg_uint32 i, j;  
  cyg_uint32 start_interface, start_endpoint, num_endpoints;
  cyg_uint32 total_interfaces, total_endpoints;
  cyg_uint32 txep_count = 0;
  cyg_uint32 rxep_count = 0;
  cyg_bool   alloc_ok;
  const usb_configuration_descriptor* configurations;
  const usb_interface_descriptor*     interfaces;
  const usb_endpoint_descriptor*      endpoints;

  // Check for valid config.
  if ((config_id <= 0) || 
    (config_id > ctrlep.common.enumeration_data->device.number_configurations))
    return USBS_CONTROL_RETURN_UNKNOWN;

  // Clear the previous endpoint configuration.
  stm32_usb_ctrl_clear_config ();

  // Get the base pointers for the descriptor tables.
  configurations = ctrlep.common.enumeration_data->configurations;
  interfaces     = ctrlep.common.enumeration_data->interfaces;
  endpoints      = ctrlep.common.enumeration_data->endpoints;

  start_interface = 0;
  start_endpoint  = 0;
  num_endpoints = 0;
  total_interfaces = ctrlep.common.enumeration_data->total_number_interfaces; 
  total_endpoints  = ctrlep.common.enumeration_data->total_number_endpoints;

  // Skip over the descriptors until we get to the ones we want.
  for (i = 0; i < (config_id-1); i++) {
    ASSERT_USB (start_interface + configurations[i].number_interfaces <= total_interfaces,
      "Invalid number of interfaces in enumeration data.");
    for (j = 0; j < configurations[i].number_interfaces; j++) {
      start_endpoint += interfaces[start_interface+j].number_endpoints;
      ASSERT_USB (start_endpoint < total_endpoints,
        "Invalid number of endpoints in enumeration data.");
    }
    start_interface += configurations[i].number_interfaces;
  }

  // Determine the number of endpoint descriptors which need to be included for
  // all interfaces within the specified configuration.
  ASSERT_USB (start_interface + configurations[i].number_interfaces <= total_interfaces,
    "Invalid number of interfaces in enumeration data.");
  for (j = 0; j < configurations[i].number_interfaces; j++) {
    num_endpoints += interfaces[start_interface+j].number_endpoints;
    ASSERT_USB (start_endpoint + num_endpoints <= total_endpoints,
      "Invalid number of endpoints in enumeration data.");
  }
  TRACE_USB ("Found %d interfaces and %d endpoints for configuration %d.\n",
    configurations[i].number_interfaces, num_endpoints, config_id);

  // Realise the endpoints for the specified interfaces.  Set up the buffer
  // RAM and configure them to NAK the host until the higher layer application
  // is ready to initiate transfers.  Failures to allocate the endpoint buffers
  // will put the endpoints in the stalled state.
  for (i = 0; i < num_endpoints; i++) {  
    const usb_endpoint_descriptor* ep_desc = endpoints + start_endpoint + i;
    cyg_uint32 pkt_size = ep_desc->max_packet_lo + (((cyg_uint32) ep_desc->max_packet_hi) << 8);

    // Deal with transmit (input) endpoints.
    if (ep_desc->endpoint & USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN) {
      if ((ep_desc->endpoint & 0x7F) < 1 || (ep_desc->endpoint & 0x7F) > 15) {
        FAIL_USB ("Invalid endpoint ID in configuration.");
        goto out;
      }
      if (txep_count >= CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM) {
        FAIL_USB ("Too many TX endpoints in configuration.");
        goto out;
      }

      // Set up transmit (input) buffers.
      if (ep_desc->attributes == USB_ENDPOINT_DESCRIPTOR_ATTR_BULK) {
        alloc_ok = stm32_usb_buf_add_ep (txep_count + 1, pkt_size, true, false, true);
        stm32_usb_set_epxr_bits (txep_count + 1, 
          CYGHWR_HAL_STM32_USB_EPXR_EPTYPE_BULK | CYGHWR_HAL_STM32_USB_EPXR_EPKIND);
#ifdef CYGHWR_DEVS_USB_CORTEXM_STM32_BULK_TERM_ZLP
        (txep_list + txep_count)->txtr.flags = TXTR_FLAGS_ZLPKT | TXTR_FLAGS_DBUF;
#else
        (txep_list + txep_count)->txtr.flags = TXTR_FLAGS_DBUF;
#endif
        TRACE_USB ("Configured BULK IN endpoint ID %d.\n", ep_desc->endpoint & 0x7F);  
      }
      else if (ep_desc->attributes == USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT) {
        alloc_ok = stm32_usb_buf_add_ep (txep_count + 1, pkt_size, true, false, false);
        (txep_list + txep_count)->txtr.flags = TXTR_FLAGS_NONE;
        stm32_usb_set_epxr_bits (txep_count + 1, CYGHWR_HAL_STM32_USB_EPXR_EPTYPE_INTR);
        TRACE_USB ("Configured INTERRUPT IN endpoint ID %d.\n", ep_desc->endpoint & 0x7F);  
      }
      else {
        FAIL_USB ("Isochronous endpoints are not currently supported.");
        goto out;
      }

      // Enable endpoints if buffer allocation was OK - stall them otherwise.
      if (alloc_ok) {
        txep_map [(ep_desc->endpoint & 0x7F) - 1] = txep_list + txep_count;
        (txep_list + txep_count)->common.halted = false;
        (txep_list + txep_count)->txtr.state = TXTR_STATE_IDLE;
        stm32_usb_set_txep_status (txep_count + 1, CYGHWR_HAL_STM32_USB_EPXR_STATTX_NAK);
      }
      else {
        stm32_usb_set_txep_status (txep_count + 1, CYGHWR_HAL_STM32_USB_EPXR_STATTX_STALL);
      }
      stm32_usb_set_epxr_bits (txep_count + 1, (ep_desc->endpoint & 0xF));
      txep_count++;
    }

    // Deal with receive (output) endpoints.
    else {
      if ((ep_desc->endpoint & 0x7F) < 1 || (ep_desc->endpoint & 0x7F) > 15) {
        FAIL_USB ("Invalid endpoint ID in configuration.");
        goto out;
      }
      if (rxep_count >= CYGNUM_DEVS_USB_CORTEXM_STM32_RXEP_NUM) {
        FAIL_USB ("Too many RX endpoints in configuration.");
        goto out;
      }

      // Set up receive (output) buffers.
      if (ep_desc->attributes == USB_ENDPOINT_DESCRIPTOR_ATTR_BULK) {
        alloc_ok = stm32_usb_buf_add_ep (rxep_count + CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM + 1, 
          pkt_size, false, true, true);
        stm32_usb_set_epxr_bits (rxep_count + CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM + 1, 
          CYGHWR_HAL_STM32_USB_EPXR_EPTYPE_BULK | CYGHWR_HAL_STM32_USB_EPXR_EPKIND);
#ifdef CYGHWR_DEVS_USB_CORTEXM_STM32_BULK_TERM_ZLP
        (rxep_list + rxep_count)->rxtr.flags = RXTR_FLAGS_ZLPKT | RXTR_FLAGS_DBUF;
#else
        (rxep_list + rxep_count)->rxtr.flags = RXTR_FLAGS_DBUF;
#endif
        TRACE_USB ("Configured BULK OUT endpoint ID %d.\n", ep_desc->endpoint & 0x7F);  
      }
      else if (ep_desc->attributes == USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT) {
        alloc_ok = stm32_usb_buf_add_ep (rxep_count + CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM + 1, 
          pkt_size, false, true, false);
        (rxep_list + rxep_count)->rxtr.flags = RXTR_FLAGS_NONE;
        stm32_usb_set_epxr_bits (rxep_count + CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM + 1, 
          CYGHWR_HAL_STM32_USB_EPXR_EPTYPE_INTR);
        TRACE_USB ("Configured INTERRUPT OUT endpoint ID %d.\n", ep_desc->endpoint & 0x7F);  
      }
      else {
        FAIL_USB ("Isochronous endpoints are not currently supported.");
        goto out;
      }

      // Enable endpoints if buffer allocation was OK - stall them otherwise.
      if (alloc_ok) {
        rxep_map [(ep_desc->endpoint & 0x7F) - 1] = rxep_list + rxep_count;
        (rxep_list + rxep_count)->common.halted = false;
        (rxep_list + rxep_count)->rxtr.state = RXTR_STATE_IDLE;
        stm32_usb_set_rxep_status (rxep_count + CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM + 1, 
          CYGHWR_HAL_STM32_USB_EPXR_STATRX_NAK);
      }
      else {
        stm32_usb_set_rxep_status (rxep_count + CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM + 1, 
          CYGHWR_HAL_STM32_USB_EPXR_STATRX_STALL);
      }
      stm32_usb_set_epxr_bits (rxep_count + CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM + 1, 
        (ep_desc->endpoint & 0xF));
      rxep_count++;
    }
  }

  // Pass up to higher layers.
out:
  return USBS_CONTROL_RETURN_UNKNOWN;
}

//-----------------------------------------------------------------------------
// Handle get status setup packets.  This function places the status
// information directly into the staging buffer.  We deal with the device
// and endpoint status responses here.  The interface status request is
// dealt with in the common USB slave layer.
// TODO - should #define the return fields in usb.h.

static inline usbs_control_return stm32_usb_ctrl_get_status
  (cyg_uint32 recipient, cyg_uint32 ep_sel)
{
  cyg_uint8 dev_state = ctrlep.common.state & USBS_STATE_MASK;
  usbs_control_return result = USBS_CONTROL_RETURN_UNKNOWN;

  // Device status requests are valid in addressed and configured states.
  // TODO - remote wakeup is not currently supported by this driver.
  if (recipient == USB_DEVREQ_RECIPIENT_DEVICE) {
    if (dev_state == USBS_STATE_ADDRESSED || dev_state == USBS_STATE_CONFIGURED) {
#ifdef CYGHWR_DEVS_USB_CORTEXM_STM32_SELF_POWERED
      ctrlep_msg_buffer[0] = 0x01;
#else
      ctrlep_msg_buffer[0] = 0x00;
#endif
      ctrlep_msg_buffer[1] = 0x00;
      result = USBS_CONTROL_RETURN_HANDLED;
    }
  }

  // Endpoint 0 status requests are valid in addressed and configured states.
  // Endpoint 0 cannot be halted.  Endpoint number is in the lower 4 bits of the ID.
  else if (recipient == USB_DEVREQ_RECIPIENT_ENDPOINT && (ep_sel & USB_DEVREQ_INDEX_ENDPOINT_MASK) == 0) {
    if (dev_state == USBS_STATE_ADDRESSED || dev_state == USBS_STATE_CONFIGURED) {
      ctrlep_msg_buffer[0] = 0x00;
      ctrlep_msg_buffer[1] = 0x00;
      result = USBS_CONTROL_RETURN_HANDLED;
    }
  }

  // Non-control endpoint status requests are only valid in the configured state.
  // Returns the halted state of the requested endpoint.
  else if (recipient == USB_DEVREQ_RECIPIENT_ENDPOINT) {
    if (dev_state == USBS_STATE_CONFIGURED) {

      // Transmit (IN) endpoint IDs have the top bit set.
      if ((ep_sel & USB_DEVREQ_INDEX_DIRECTION_MASK) == USB_DEVREQ_INDEX_DIRECTION_IN) {
        ep_sel = (ep_sel & USB_DEVREQ_INDEX_ENDPOINT_MASK) - 1;
        if (txep_map[ep_sel] != NULL) {
          ctrlep_msg_buffer[0] = (txep_map[ep_sel]->common.halted) ? 0x01 : 0x00; 
          ctrlep_msg_buffer[1] = 0x00;
          result = USBS_CONTROL_RETURN_HANDLED;
        }
      }

      // Receive (OUT) endpoint IDs have the top bit clear.
      else {
        ep_sel = (ep_sel & USB_DEVREQ_INDEX_ENDPOINT_MASK) - 1;
        if (rxep_map[ep_sel] != NULL) {
          ctrlep_msg_buffer[0] = (rxep_map[ep_sel]->common.halted) ? 0x01 : 0x00; 
          ctrlep_msg_buffer[1] = 0x00;
          result = USBS_CONTROL_RETURN_HANDLED;
        }
      }
    }
  }

  // Point the endpoint 0 response buffer at the staging buffer.
  if (result != USBS_CONTROL_RETURN_UNKNOWN) {
    ctrlep.common.buffer_size = 2;
    ctrlep.common.buffer = ctrlep_msg_buffer;
  }
  return result;
}

//-----------------------------------------------------------------------------
// Handle set and clear feature commands.  Since remote wakeup support is not 
// implemented and interface features are dealt with at a higher layer we only 
// implement endpoint halting here.  This will only ever be called from within
// the DSR so is interrupt safe.

static usbs_control_return stm32_usb_ctrl_set_feature
  (cyg_uint32 recipient, cyg_uint32 ep_sel, cyg_uint32 feature, cyg_bool set)
{
  cyg_uint8 dev_state = ctrlep.common.state & USBS_STATE_MASK;
  usbs_control_return result = USBS_CONTROL_RETURN_UNKNOWN;

  // Non-control endpoint feature control is valid in the configured state.
  if (recipient == USB_DEVREQ_RECIPIENT_ENDPOINT && 
    dev_state == USBS_STATE_CONFIGURED && feature == USB_DEVREQ_FEATURE_ENDPOINT_HALT) {

    // Transmit (IN) endpoint IDs have the top bit set.
    if ((ep_sel & USB_DEVREQ_INDEX_DIRECTION_MASK) == USB_DEVREQ_INDEX_DIRECTION_IN) {
      ep_sel = (ep_sel & USB_DEVREQ_INDEX_ENDPOINT_MASK) - 1;
      if (txep_map[ep_sel] != NULL) {
        if (set)
          stm32_usb_txep_halt (txep_map[ep_sel], true);
        else
          stm32_usb_txep_unhalt (txep_map[ep_sel], true);
        result = USBS_CONTROL_RETURN_HANDLED;
      }
    }

    // Receive (OUT) endpoint IDs have the top bit clear.
    else {
      ep_sel = (ep_sel & USB_DEVREQ_INDEX_ENDPOINT_MASK) - 1;
      if (rxep_map[ep_sel] != NULL) {
        if (set)
          stm32_usb_rxep_halt (rxep_map[ep_sel], true);
        else
          stm32_usb_rxep_unhalt (rxep_map[ep_sel], true);
        result = USBS_CONTROL_RETURN_HANDLED;
      }
    }
  }
  return result;
}

//-----------------------------------------------------------------------------
// Process standard endpoint 0 setup packets.

static usbs_control_return stm32_usb_ctrl_setup_standard
  (void)
{
  usb_devreq* req = (usb_devreq*) &ctrlep.common.control_buffer[0];
  usbs_control_return result = USBS_CONTROL_RETURN_UNKNOWN;
  cyg_uint32 recipient = req->type & USB_DEVREQ_RECIPIENT_MASK;

  switch (req->request) {

    // Assign device address.  We can't actually update the address register 
    // until after the full bus handshake has completed, otherwise the ACK 
    // packet gets lost.
    case USB_DEVREQ_SET_ADDRESS :
      result = USBS_CONTROL_RETURN_HANDLED;
      break;

    // Set device configuration.
    case USB_DEVREQ_SET_CONFIGURATION :
      TRACE_USB ("Setting USB configuration = %d\n", (cyg_uint32) req->value_lo);
      result = stm32_usb_ctrl_set_config (req->value_lo);
      break;

    // TODO: Interfaces with multiple settings are not currently supported.
    // If a device only supports a default setting for a specified interface
    // the spec says a stall should be sent.  
    case USB_DEVREQ_SET_INTERFACE :
      TRACE_USB ("Using default interface settings.\n");
      result = USBS_CONTROL_RETURN_STALL;
      break;
 
    // Get device status.
    case USB_DEVREQ_GET_STATUS :
      result = stm32_usb_ctrl_get_status (recipient, req->index_lo);
      break;

    // Control endpoint halting.  Halt on 'set feature'.
    case USB_DEVREQ_SET_FEATURE :
      result = stm32_usb_ctrl_set_feature (recipient, req->index_lo, req->value_lo, true);
      break;

    // Control endpoint halting.  Resume on 'clear feature'.
    case USB_DEVREQ_CLEAR_FEATURE :
      result = stm32_usb_ctrl_set_feature (recipient, req->index_lo, req->value_lo, false);
      break;

    // Pass up to the user supplied handler, if present.
    default :
      if (ctrlep.common.standard_control_fn)
        result = (*ctrlep.common.standard_control_fn) 
          (&ctrlep.common, ctrlep.common.standard_control_data);
      break; 
  }

  // If not already handled, pass up to the high level driver.
  if (result == USBS_CONTROL_RETURN_UNKNOWN)
    result = usbs_handle_standard_control (&ctrlep.common);

  return result;
}

//-----------------------------------------------------------------------------
// Perform initial processing of endpoint 0 setup packets.

static void stm32_usb_ctrl_setup_handler
  (void)
{
  cyg_uint32 req_length, req_type, req_dir;
  usb_devreq* req = (usb_devreq*) &ctrlep.common.control_buffer[0];
  usbs_control_return result = USBS_CONTROL_RETURN_UNKNOWN;

  // Extract the required fields from the setup packet.
  req_length = 8 * (cyg_uint32) req->length_hi + req->length_lo;
  req_type = req->type & USB_DEVREQ_TYPE_MASK;
  req_dir = req->type & USB_DEVREQ_DIRECTION_MASK;

  // Always fail the transaction if the requested data stage exceeds the
  // allocated buffer area.
  if ((req_dir == USB_DEVREQ_DIRECTION_OUT) && 
    (req_length > CYGNUM_DEVS_USB_CORTEXM_STM32_EPO_MAX_MSG_SIZE)) {
    FAIL_USB ("Requested control data stage exceeds message buffer size.");
    result = USBS_CONTROL_RETURN_STALL;
  }

  // Pass standard requests to the standard handlers.
  else if (req_type == USB_DEVREQ_TYPE_STANDARD) {
    result = stm32_usb_ctrl_setup_standard ();
  }

  // Hand off non-standard requests to their respective handlers.
  else {
    usbs_control_return (*callback_fn) (usbs_control_endpoint*, void*);
    void* callback_arg;
    
    if (req_type == USB_DEVREQ_TYPE_CLASS) {
      callback_fn  = ctrlep.common.class_control_fn;
      callback_arg = ctrlep.common.class_control_data;
    }
    else if (req_type == USB_DEVREQ_TYPE_VENDOR) {
      callback_fn  = ctrlep.common.vendor_control_fn;
      callback_arg = ctrlep.common.vendor_control_data;
    }
    else {
      callback_fn  = ctrlep.common.reserved_control_fn;
      callback_arg = ctrlep.common.reserved_control_data;
    }
    
    result = (callback_fn) ? (*callback_fn) (&ctrlep.common, callback_arg)
      : USBS_CONTROL_RETURN_STALL;
  }

  // If correctly handled, initiate the data transfer phase.  This is only 
  // called from within the DSR, so the transaction start is interrupt safe.
  if (result == USBS_CONTROL_RETURN_HANDLED) {

    // ACK the transfer by sending a zero length packet.
    if (req_length == 0) {
      ctrlep.msg_state = CTRLEP_MSG_STATE_CTRL_ACK;
      ctrlep.txtr.state = TXTR_STATE_IDLE;
      ctrlep.txtr.buf_ptr = ctrlep_msg_buffer;
      ctrlep.txtr.buf_size = 0;
      stm32_usb_txtr_start (&ctrlep.txtr, true);
    }

    // Send the inbound data.
    else if (req_dir == USB_DEVREQ_DIRECTION_IN) {
      ctrlep.msg_state = CTRLEP_MSG_STATE_IN_DATA;
      ctrlep.txtr.state = TXTR_STATE_IDLE;
      ctrlep.txtr.buf_ptr = ctrlep_msg_buffer;
      ctrlep.txtr.buf_size = stm32_usb_ctrl_fill_msg_buffer ();
      stm32_usb_txtr_start (&ctrlep.txtr, true);
    }

    // Receive outbound data from the host.    
    else {
      ctrlep.msg_state = CTRLEP_MSG_STATE_OUT_DATA;
      ctrlep.rxtr.state = RXTR_STATE_IDLE;
      ctrlep.rxtr.buf_ptr = ctrlep_msg_buffer;
      ctrlep.rxtr.buf_size = CYGNUM_DEVS_USB_CORTEXM_STM32_EPO_MAX_MSG_SIZE;
      stm32_usb_rxtr_start (&ctrlep.rxtr, true);
    }
  }

  // Stall endpoint on request. 
  else if (result == USBS_CONTROL_RETURN_STALL) {
    stm32_usb_set_txep_status (0, CYGHWR_HAL_STM32_USB_EPXR_STATTX_STALL);
    stm32_usb_set_rxep_status (0, CYGHWR_HAL_STM32_USB_EPXR_STATRX_STALL);
  }

  // Unsupported setup commands also stall the control endpoint.
  else {
    TRACE_USB ("Stall EP0 on UNKNOWN control message : %02X %02X %02X %02X %02X %02X %02X %02X\n",
      ctrlep.common.control_buffer[0], ctrlep.common.control_buffer[1],
      ctrlep.common.control_buffer[2], ctrlep.common.control_buffer[3],
      ctrlep.common.control_buffer[4], ctrlep.common.control_buffer[5],
      ctrlep.common.control_buffer[6], ctrlep.common.control_buffer[7]);
    stm32_usb_set_txep_status (0, CYGHWR_HAL_STM32_USB_EPXR_STATTX_STALL);
    stm32_usb_set_rxep_status (0, CYGHWR_HAL_STM32_USB_EPXR_STATRX_STALL);    
  }
}

//-----------------------------------------------------------------------------
// Completion of control endpoint data transmit phase.

static inline void stm32_usb_ctrl_txtr_done 
  (void)
{
  cyg_uint32 reg_val;
  usb_devreq* req = (usb_devreq*) &ctrlep.common.control_buffer[0];

  // If this is confirmation that an ACK packet has been sent, complete the
  // transaction.  We also take the opportunity to update the device address
  // here if required.
  if (ctrlep.msg_state == CTRLEP_MSG_STATE_CTRL_ACK) {
    if (req->request == USB_DEVREQ_SET_ADDRESS) {
      TRACE_USB ("Setting USB device address = %d\n", (cyg_uint32) req->value_lo);
      reg_val = CYGHWR_HAL_STM32_USB_DADDR_EF;
      reg_val |= CYGHWR_HAL_STM32_USB_DADDR_ADD ((cyg_uint32) req->value_lo);
      HAL_WRITE_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_DADDR, reg_val);
      stm32_usb_ctrl_update_state (USBS_STATE_ADDRESSED, USBS_STATE_CHANGE_ADDRESSED);
    }
    stm32_usb_ctrl_completed (ctrlep.txtr.status);
  }

  // Complete after transmitting the output status handshake.
  else if (ctrlep.msg_state == CTRLEP_MSG_STATE_OUT_STATUS) {
    stm32_usb_ctrl_completed (ctrlep.txtr.status);
  }

  // If all packets in an inbound transaction have been sent, wait for status 
  // response from the host.  Called from DSR so is interrupt safe.
  else if (ctrlep.msg_state == CTRLEP_MSG_STATE_IN_DATA) {
    if (ctrlep.txtr.status == ENOERR) {
      ctrlep.msg_state = CTRLEP_MSG_STATE_IN_STATUS;
      ctrlep.rxtr.state = RXTR_STATE_IDLE;
      ctrlep.rxtr.buf_ptr = ctrlep_msg_buffer;
      ctrlep.rxtr.buf_size = 0;
      stm32_usb_rxtr_start (&ctrlep.rxtr, true);
    }
    else {
      stm32_usb_ctrl_completed (ctrlep.txtr.status);
    }
  }
}

//-----------------------------------------------------------------------------
// Completion of control endpoint data receive phase.

static inline void stm32_usb_ctrl_rxtr_done 
  (void)
{
  // If waiting for a status response, we should get a zero length packet.
  if (ctrlep.msg_state == CTRLEP_MSG_STATE_IN_STATUS) {
    stm32_usb_ctrl_completed (ctrlep.rxtr.status);
  }

  // Handle conventional data packets.  Incoming packets on endpoint 0 can
  // be overwritten by setup packets.  Called from DSR so is interrupt safe.
  else if (ctrlep.msg_state == CTRLEP_MSG_STATE_OUT_DATA) {
    if (ctrlep.rxtr.status == ENOERR) {
      ctrlep.msg_state = CTRLEP_MSG_STATE_OUT_STATUS;
      ctrlep.txtr.state = TXTR_STATE_IDLE;
      ctrlep.txtr.buf_ptr = ctrlep_msg_buffer;
      ctrlep.txtr.buf_size = 0;
      stm32_usb_txtr_start (&ctrlep.txtr, true);
    }
    else {
      stm32_usb_ctrl_completed (ctrlep.rxtr.status);
    }
  }
}

//=============================================================================
// Implement ISRs for low level data transfer.
//=============================================================================

//-----------------------------------------------------------------------------
// ISR for handling control endpoint interrupts.

static inline cyg_bool stm32_usb_ctrlep_ISR
  (void)
{
  cyg_bool completed;
  cyg_bool call_dsr = false; 
  cyg_uint32 usb_epxr;

  // Process packet transmit events.
  HAL_READ_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EP0R, usb_epxr);
  if (usb_epxr & CYGHWR_HAL_STM32_USB_EPXR_CTRTX) {
    stm32_usb_clear_epxr_bits (0, CYGHWR_HAL_STM32_USB_EPXR_CTRTX);

    completed = stm32_usb_txtr_run (&ctrlep.txtr);
    if (completed) {
      isr_shared.txtr_done |= (1 << 0);
      call_dsr = true;
    }
  }

  // Deal with incoming setup packets.  These are copied directly to the
  // staging buffer in the ISR/DSR shared area for subsequent handling in
  // DSR context.
  if (usb_epxr & CYGHWR_HAL_STM32_USB_EPXR_CTRRX) {
    stm32_usb_clear_epxr_bits (0, CYGHWR_HAL_STM32_USB_EPXR_CTRRX);

    if (usb_epxr & CYGHWR_HAL_STM32_USB_EPXR_SETUP) {
      stm32_usb_copy_from_sbuf (ctrlep.common.control_buffer, 8, 0);
      isr_shared.flags |= ISR_FLAGS_SETUP_READY;
      call_dsr = true;
    }
 
    // Handle conventional incoming packets.
    else {
      completed = stm32_usb_rxtr_run (&ctrlep.rxtr);
      if (completed) {
        isr_shared.rxtr_done |= (1 << 0);
        call_dsr = true;
      }
    }
  }
  return call_dsr;
}

//-----------------------------------------------------------------------------
// ISR for handling transmit endpoint interrupts.  

static inline cyg_bool stm32_usb_txep_ISR
  (cyg_uint32 txep_id)
{
  cyg_bool completed;
  cyg_bool call_dsr = false; 
  cyg_uint32 usb_epxr;
  txep_impl* txep = txep_list + txep_id - 1;

  // Receive events for a transmit endpoint are an error - discard them.
  HAL_READ_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (txep_id), usb_epxr);
  if (usb_epxr & CYGHWR_HAL_STM32_USB_EPXR_CTRRX) {
    FAIL_USB ("Received RX interrupt for TX endpoint.");
    stm32_usb_clear_epxr_bits (txep_id, CYGHWR_HAL_STM32_USB_EPXR_CTRRX);
  }

  // Handle transmit interrupt events.
  if (usb_epxr & CYGHWR_HAL_STM32_USB_EPXR_CTRTX) {
    stm32_usb_clear_epxr_bits (txep_id, CYGHWR_HAL_STM32_USB_EPXR_CTRTX);
    completed = stm32_usb_txtr_run (&txep->txtr);
    if (completed) {
      isr_shared.txtr_done |= (1 << txep_id);
      call_dsr = true;
    }
  }
  return call_dsr;
}

//-----------------------------------------------------------------------------
// ISR for handling receive endpoint interrupts.  

static inline cyg_bool stm32_usb_rxep_ISR
  (cyg_uint32 rxep_id)
{
  cyg_bool completed;
  cyg_bool call_dsr = false; 
  cyg_uint32 usb_epxr;
  rxep_impl* rxep = rxep_list + rxep_id - CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM - 1;

  // Transmit events for a receive endpoint are an error - discard them.
  HAL_READ_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_EPXR (rxep_id), usb_epxr);
  if (usb_epxr & CYGHWR_HAL_STM32_USB_EPXR_CTRTX) {
    FAIL_USB ("Received TX interrupt for RX endpoint.");
    stm32_usb_clear_epxr_bits (rxep_id, CYGHWR_HAL_STM32_USB_EPXR_CTRTX);
  }

  // Handle receive interrupt events.
  if (usb_epxr & CYGHWR_HAL_STM32_USB_EPXR_CTRRX) {
    stm32_usb_clear_epxr_bits (rxep_id, CYGHWR_HAL_STM32_USB_EPXR_CTRRX);
    completed = stm32_usb_rxtr_run (&rxep->rxtr);
    if (completed) {
      isr_shared.rxtr_done |= (1 << rxep_id);
      call_dsr = true;
    }
  }
  return call_dsr;
}

//-----------------------------------------------------------------------------
// Main ISR for handling interrupt events.  The interrupt generation for the 
// USB endpoints is a little strange, with endpoint interrupts effectively being
// queued through the same interrupt status register.  Clearing the interrupt
// condition has the effect of popping an endpoint interrupt from the queue so
// that the next endpoint can be serviced.  This means that serialising the
// interrupt processing between ISR and DSR would carry an unacceptable latency
// penalty - which is why we have to do the buffer copies within the ISR.

static cyg_uint32 stm32_usb_ISR
  (cyg_vector_t vector, cyg_addrword_t data)
{
  cyg_bool call_dsr = false; 
  cyg_uint32 usb_istr, ep_id;
  cyg_uint32 ret_val = CYG_ISR_HANDLED;

  // Check for device interrupts first.
  HAL_READ_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_ISTR, usb_istr);

  // Detect reset event and hand it up to the DSR.  Note that these will not
  // be merged with other events for DSR processing.
  if (usb_istr & CYGHWR_HAL_STM32_USB_ISTR_RESET) {
    usb_istr &= ~((cyg_uint32) CYGHWR_HAL_STM32_USB_ISTR_RESET);
    HAL_WRITE_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_ISTR, usb_istr);
    isr_shared.flags |= ISR_FLAGS_DEVICE_RESET;
    call_dsr = true;
  }

  // TODO: Support for suspend and wake events can be added here if required.

  // Check for endpoint interrupts.  These are indicated by the CTR flag.
  // The endpoint direction is inferred from the endpoint number, since
  // transmit endpoints are enumerated before receive endpoints.
  else while (usb_istr & CYGHWR_HAL_STM32_USB_ISTR_CTR) {
    ep_id = usb_istr & CYGHWR_HAL_STM32_USB_ISTR_EPID_MASK;

    // Service the control endpoint.
    if (ep_id == 0)
      call_dsr |= stm32_usb_ctrlep_ISR ();

    // Service transmit endpoints.
    else if (ep_id <= CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM)
      call_dsr |= stm32_usb_txep_ISR (ep_id);

    // Service receive endpoints.
    else if (ep_id < USB_EPNUM)
      call_dsr |= stm32_usb_rxep_ISR (ep_id);

    // Invalid endpoint.  Fail in debug, clear down in production builds.
    else {
      FAIL_USB ("Interrupt for invalid endpoint detected.");
      stm32_usb_clear_epxr_bits (ep_id, 
        CYGHWR_HAL_STM32_USB_EPXR_CTRTX | CYGHWR_HAL_STM32_USB_EPXR_CTRRX);
    }

    // Check for all endpoints having been serviced.
    HAL_READ_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_ISTR, usb_istr);
  }

  // If interrupt has been handled, acknowledge it and return.  Leave the
  // interrupt unmasked until the DSR is called, since intervening ISR calls
  // will safely post their events to the event masks.
  cyg_drv_interrupt_acknowledge (vector);
  if (call_dsr) {
    ret_val |= CYG_ISR_CALL_DSR;
  }
  return ret_val;
}

//=============================================================================
// Implement DSRs for handling high-level interrupt responses.
//=============================================================================

//-----------------------------------------------------------------------------
// Main DSR for high-level interrupt processing.  Information about the
// interrupt conditions is passed up via the ISR shared data area.

static void stm32_usb_DSR 
  (cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
  cyg_uint32 i;

  // Disable interrupts on entry to the DSR to avoid further events getting
  // added to the event masks.
  stm32_usb_request_intr_mask ();

  // Process device reset notifications.
  if (isr_shared.flags & ISR_FLAGS_DEVICE_RESET) {
    stm32_usb_ctrl_reset ();
  }

  // Process control endpoint message transmit completions.
  if (isr_shared.txtr_done & 1) {
    stm32_usb_ctrl_txtr_done ();
  }

  // Process control endpoint message receive completions.
  if (isr_shared.rxtr_done & 1) {
    stm32_usb_ctrl_rxtr_done ();
  }

  // Process non-control endpoint message transmit completions.
  if (isr_shared.txtr_done) {
    for (i = 1; i < 8; i++) {
      if (isr_shared.txtr_done & (1 << i)) {
        txep_impl* txep = txep_list + i - 1;
        int retval = (txep->txtr.status != ENOERR) ? -txep->txtr.status : txep->txtr.bytes_sent;
        if (txep->common.complete_fn)
          (*txep->common.complete_fn) (txep->common.complete_data, retval);
        if (retval < 0)
          TRACE_USB ("TX transaction failed (endpoint %d, status %d).\n", txep->txtr.ep_num, retval);
      }
    }
  }

  // Process non-control endpoint message received completions.
  if (isr_shared.rxtr_done) {
    for (i = 1; i < 8; i++) {
      if (isr_shared.rxtr_done & (1 << i)) {
        rxep_impl* rxep = rxep_list + i - CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM - 1;
        int retval = (rxep->rxtr.status != ENOERR) ? -rxep->rxtr.status : rxep->rxtr.bytes_rcvd;
        if (rxep->common.complete_fn)
          (*rxep->common.complete_fn) (rxep->common.complete_data, retval);
        if (retval < 0)
          TRACE_USB ("RX transaction failed (endpoint %d, status %d).\n", rxep->rxtr.ep_num, retval);
      }
    }
  }

  // Process setup packets after ensuring that all outstanding EP0 completions
  // associated with a previous setup transaction have been dealt with.
  if (isr_shared.flags & ISR_FLAGS_SETUP_READY) {
    stm32_usb_ctrl_setup_handler ();
  }

  // Clear the ISR shared flags before unmasking the interrupt - they should 
  // all have been dealt with.
  isr_shared.flags = ISR_FLAGS_CLEARED;
  isr_shared.txtr_done = 0;
  isr_shared.rxtr_done = 0;

  // Release the interrupt mask if possible.
  stm32_usb_release_intr_mask ();
}

//=============================================================================
// Provide standard USB driver API entry points.
//=============================================================================

//-----------------------------------------------------------------------------
// API entry point for endpoint transmit requests.

static void stm32_usb_tx_start
  (usbs_tx_endpoint* usbs_txep)
{
  txep_impl* txep = (txep_impl*) usbs_txep;
  txtr_impl* txtr = &txep->txtr;
  cyg_int32 status = -EIO;

  // Check to see whether a transaction is in progress.
  if (txtr->state != TXTR_STATE_IDLE) {
    FAIL_USB ("Endpoint TX request when endpoint already busy.");
    status = -EBUSY;
    goto out;
  }

  // Do a sanity check on the descriptor.
  if (!txep->common.complete_fn) {
    FAIL_USB ("Endpoint TX requires a completion function.");
    goto out;
  }
  if (!txep->common.buffer) {
    FAIL_USB ("Endpoint TX requires a valid transmit buffer.");
    goto out;
  }
  if (stm32_usb_buf_get_size (txtr->ep_num) == 0) {
    FAIL_USB ("TX request when endpoint buffers not allocated.");
    goto out;
  }

  // Attempt to start a transmit transaction and extract error status if it 
  // completes too early.  This is not an interrupt safe call.
  txtr->buf_ptr = txep->common.buffer;
  txtr->buf_size = (cyg_uint32) txep->common.buffer_size;
  if (stm32_usb_txtr_start (txtr, false))
    status = -txtr->status;
  else
    status = -ENOERR;

  // If the transaction failed to start, fire the completion handler.
out:
  if ((status < 0) && (txep->common.complete_fn)) {
    txep->common.complete_fn (txep->common.complete_data, status);
  }
}

//-----------------------------------------------------------------------------
// API entry point for endpoint receive requests.

static void stm32_usb_rx_start
  (usbs_rx_endpoint* usbs_rxep)
{
  rxep_impl* rxep = (rxep_impl*) usbs_rxep;
  rxtr_impl* rxtr = &rxep->rxtr;
  cyg_int32 status = -EIO;

  // Check to see whether a transaction is in progress.
  if (rxtr->state != RXTR_STATE_IDLE) {
    FAIL_USB ("Endpoint RX request when endpoint already busy.");
    status = -EBUSY;
    goto out;
  }

  // Do a sanity check on the descriptor.
  if (!rxep->common.complete_fn) {
    FAIL_USB ("Endpoint RX requires a completion function.");
    goto out;
  }
  if (!rxep->common.buffer) {
    FAIL_USB ("Endpoint RX requires a valid receive buffer.");
    goto out;
  }
  if (stm32_usb_buf_get_size (rxtr->ep_num) == 0) {
    FAIL_USB ("RX request when endpoint buffers not allocated.");
    goto out;
  }
  
  // Attempt to start a receive transaction and extract error status if it 
  // completes too early.  This is not an interrupt safe call.
  rxtr->buf_ptr = rxep->common.buffer;
  rxtr->buf_size = (cyg_uint32) rxep->common.buffer_size;
  if (stm32_usb_rxtr_start (rxtr, false))
    status = -rxtr->status;
  else
    status = -ENOERR;

  // If the transaction failed to start, fire the completion handler.
out:
  if ((status < 0) && (rxep->common.complete_fn)) {
    rxep->common.complete_fn (rxep->common.complete_data, status);
  }
}

//-----------------------------------------------------------------------------
// API entry point for setting transmit endpoint halted state.  

static void stm32_usb_set_txep_halted
  (usbs_tx_endpoint* txep, cyg_bool halted)
{
  if (halted)
    stm32_usb_txep_halt ((txep_impl*) txep, false); 
  else
    stm32_usb_txep_unhalt ((txep_impl*) txep, false);
}

//-----------------------------------------------------------------------------
// API entry point for setting receive endpoint halted state.

static void stm32_usb_set_rxep_halted
  (usbs_rx_endpoint* rxep, cyg_bool halted)
{
  if (halted)
    stm32_usb_rxep_halt ((rxep_impl*) rxep, false); 
  else
    stm32_usb_rxep_unhalt ((rxep_impl*) rxep, false);
}

//=============================================================================
// Initialise and reset the USB device.
//=============================================================================

//-----------------------------------------------------------------------------
// One-time initialisation.  This function is called during device startup
// in order to bring up the USB peripheral ready for operation.

static void CYGBLD_ATTRIB_C_INIT_PRI(CYG_INIT_DEV_CHAR) cyg_usbs_cortexm_stm32_init 
  (void)
{
  cyg_uint32 reg_data;

  // First ensure that the APB bus is being clocked fast enough.
  ASSERT_USB (APB1_FREQ > 8000000, "APB1 must be clocked faster than 8MHz.");

  // Check that the endpoint configuration is sane.
  ASSERT_USB (CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM + CYGNUM_DEVS_USB_CORTEXM_STM32_RXEP_NUM < 8,
    "Too many hardware endpoints allocated in configuration.");

  // Make sure that the CAN controller is disabled and held in reset.
  // TODO: If a CAN driver is to be added to the standard distribution, this
  // should check for USB/CAN configuration clashes.
  HAL_READ_UINT32 (CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_APB1ENR, reg_data);
  reg_data &= ~((cyg_uint32) CYGHWR_HAL_STM32_RCC_APB1ENR_CAN1);
  HAL_WRITE_UINT32 (CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_APB1ENR, reg_data);
  
  // Configure the IO pins for USB operation.
  CYGHWR_HAL_STM32_GPIO_SET (USB_DISC_PIN);
  CYGHWR_HAL_STM32_GPIO_SET (USB_DP_PIN);
  CYGHWR_HAL_STM32_GPIO_SET (USB_DM_PIN);
#ifdef CYGHWR_DEVS_USB_CORTEXM_STM32_DISC_PIN_ACT_LOW
  CYGHWR_HAL_STM32_GPIO_OUT (USB_DISC_PIN, 0);
#else
  CYGHWR_HAL_STM32_GPIO_OUT (USB_DISC_PIN, 1);
#endif

  // Ensure that the USB clock is disabled prior to setting prescaler. 
  HAL_READ_UINT32 (CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_APB1ENR, reg_data);
  reg_data &= ~((cyg_uint32) CYGHWR_HAL_STM32_RCC_APB1ENR_USB);
  HAL_WRITE_UINT32 (CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_APB1ENR, reg_data);
  
  // Set up the USB 48MHz serial clock.  There are only 2 valid prescaler
  // settings which correspond to 72MHz and 48MHz PLL clock outputs.
  HAL_READ_UINT32 (CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_CFGR, reg_data);
#if (PLL_FREQ == 72000000)
  reg_data &= ~((cyg_uint32) CYGHWR_HAL_STM32_RCC_CFGR_USBPRE);  
#elif (PLL_FREQ == 48000000)
  reg_data |= CYGHWR_HAL_STM32_RCC_CFGR_USBPRE;
#else
#error "SMT32 PLL clock must be set to 48MHz or 72MHz for correct USB operation."
#endif
  HAL_WRITE_UINT32 (CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_CFGR, reg_data);

  // Activate the USB clock after setting prescaler. 
  HAL_READ_UINT32 (CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_APB1ENR, reg_data);
  reg_data |= CYGHWR_HAL_STM32_RCC_APB1ENR_USB;
  HAL_WRITE_UINT32 (CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_APB1ENR, reg_data);

  // Take USB transceiver out of powerdown state, but leave it in reset until we
  // are ready to start.  Leave interrupts disabled at source.
  reg_data = CYGHWR_HAL_STM32_USB_CNTR_FRES;
  HAL_WRITE_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_CNTR, reg_data);
  CYGACC_CALL_IF_DELAY_US (USB_TSTARTUP);
  
  // Initialise interrupt mask request counter.
  interrupt_mask_count = 0;

  // Attach USB interrupts.  Everything is done via the standard interrupt - the 
  // high priority interrupt is not used.
  cyg_drv_interrupt_mask (CYGNUM_HAL_INTERRUPT_USB_HP);
  cyg_drv_interrupt_create (CYGNUM_HAL_INTERRUPT_USB_LP, CYGNUM_DEVS_USB_CORTEXM_STM32_ISR_PRIORITY,
    0, stm32_usb_ISR, stm32_usb_DSR, &interrupt_handle, &interrupt_data);
  cyg_drv_interrupt_attach (interrupt_handle);
}

//-----------------------------------------------------------------------------
// Device endpoint 0 startup.  This function is called once the application 
// code has set up the desired USB control endpoint configuration.

static void stm32_usb_start
  (usbs_control_endpoint* endpoint)
{
  cyg_uint32 i;
  txep_impl* txep = txep_list;
  rxep_impl* rxep = rxep_list;

  // Fill in the generic endpoint data structures.
  for (i = 0; i < CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM; i++) {
    txep->common.start_tx_fn   = stm32_usb_tx_start;
    txep->common.set_halted_fn = stm32_usb_set_txep_halted;
    txep->common.halted        = true;
    txep->txtr.state           = TXTR_STATE_RESET;
    txep->txtr.ep_num          = i + 1;
    txep++;
  }
  for (i = 0; i < CYGNUM_DEVS_USB_CORTEXM_STM32_RXEP_NUM; i++) {
    rxep->common.start_rx_fn   = stm32_usb_rx_start;
    rxep->common.set_halted_fn = stm32_usb_set_rxep_halted;
    rxep->common.halted        = true;
    rxep->rxtr.state           = RXTR_STATE_RESET;
    rxep->rxtr.ep_num          = i + CYGNUM_DEVS_USB_CORTEXM_STM32_TXEP_NUM + 1;
    rxep++;
  }  

  // Take the USB driver out of reset and cancel any spurious interrupts.
  HAL_WRITE_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_CNTR, 0);
  HAL_WRITE_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_ISTR, 0);

  // Enable interrupts at source.
  HAL_WRITE_UINT32 (USB_BASE + CYGHWR_HAL_STM32_USB_CNTR, 
    CYGHWR_HAL_STM32_USB_CNTR_CTRM | CYGHWR_HAL_STM32_USB_CNTR_RESETM);

  // Reconnect the device to the USB bus if required.
#ifdef CYGHWR_DEVS_USB_CORTEXM_STM32_DISC_PIN_ACT_LOW
  CYGHWR_HAL_STM32_GPIO_OUT (USB_DISC_PIN, 1);
#else
  CYGHWR_HAL_STM32_GPIO_OUT (USB_DISC_PIN, 0);
#endif
  stm32_usb_ctrl_update_state (USBS_STATE_POWERED, USBS_STATE_CHANGE_POWERED);

  // Allow interrupts to run - the bus reset must be driven by the host.
  cyg_drv_interrupt_unmask (CYGNUM_HAL_INTERRUPT_USB_LP);
}

//-----------------------------------------------------------------------------
// Device endpoint 0 poll.  Polled operation just calls the ISR followed by
// the DSR.  TODO: Polled operation needs further testing. 

static void stm32_usb_poll  
  (usbs_control_endpoint* endpoint)
{
  cyg_uint32 isr_retval;
  isr_retval = stm32_usb_ISR (CYGNUM_HAL_INTERRUPT_USB_LP, 0);
  if (isr_retval & CYG_ISR_CALL_DSR)
    stm32_usb_DSR (CYGNUM_HAL_INTERRUPT_USB_LP, 1, 0);
}

//-----------------------------------------------------------------------------
// Get a handle on the specified transmit (in) endpoint.

static usbs_tx_endpoint* stm32_usb_get_txep 
  (usbs_control_endpoint* control_endpoint, cyg_uint8 ep_id)
{
  txep_impl* txep = NULL;

  // Map from endpoint ID to physical endpoint.
  if (ep_id > 0 && ep_id < 16)
    txep = txep_map [ep_id - 1];

  // Return endpoint handle or null pointer for invalid endpoint.
  if (txep == NULL) {
    FAIL_USB ("Invalid endpoint ID when accessing transmit (in) endpoint.");
    return NULL;
  }
  return (usbs_tx_endpoint*) txep;
}

//-----------------------------------------------------------------------------
// Get a handle on the specified receive (out) endpoint.

static usbs_rx_endpoint* stm32_usb_get_rxep 
  (usbs_control_endpoint* control_endpoint, cyg_uint8 ep_id)
{
  rxep_impl* rxep = NULL;

  // Map from endpoint ID to physical endpoint.
  if (ep_id > 0 && ep_id < 16)
    rxep = rxep_map [ep_id - 1];

  // Return endpoint handle or null pointer for invalid endpoint.
  if (rxep == NULL) {
    FAIL_USB ("Invalid endpoint ID when accessing receive (out) endpoint.");
    return NULL;
  }
  return (usbs_rx_endpoint*) rxep;
}

//=============================================================================
// Instantiate the test endpoint data structures if required.  This creates a
// single endpoint of each supported type - bulk transmit, bulk receive, 
// interrupt transmit and interrupt receive.  
//=============================================================================

#ifdef CYGBLD_IO_USB_SLAVE_USBTEST

usbs_testing_endpoint usbs_testing_endpoints[] = {

  { // Control endpoint.
    endpoint_type      : USB_ENDPOINT_DESCRIPTOR_ATTR_CONTROL,
    endpoint_number    : 0,
    endpoint_direction : USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN,
    endpoint           : (void*) &ctrlep,
    devtab_entry       : 0,
    min_size           : 1,
    max_size           : CYGNUM_DEVS_USB_CORTEXM_STM32_EPO_MAX_MSG_SIZE,
    max_in_padding     : 0,
    alignment          : 0
  },

  { // Bulk transmit (input) endpoint.
    endpoint_type      : USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
    endpoint_number    : 1,
    endpoint_direction : USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN,
    endpoint           : (void*) &txep_list[0],
    devtab_entry       : 0,
    min_size           : 0,
    max_size           : 0x1000,  // 4k max for testing.
    max_in_padding     : 0,
    alignment          : 0
  },

  { // Bulk receive (output) endpoint.
    endpoint_type      : USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
    endpoint_number    : 2,
    endpoint_direction : USB_ENDPOINT_DESCRIPTOR_ENDPOINT_OUT,
    endpoint           : (void*) &rxep_list[0],
    devtab_entry       : 0,
    min_size           : 0,
    max_size           : 0x1000,  // 4k max for testing.
    max_in_padding     : 0,
    alignment          : 0
  },

  { // Interrupt transmit (input) endpoint
    endpoint_type      : USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT,
    endpoint_number    : 3,
    endpoint_direction : USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN,
    endpoint           : (void*) &txep_list[1],
    devtab_entry       : 0,
    min_size           : 0,
    max_size           : 8,  // Maximum for low speed devices.
    max_in_padding     : 0,
    alignment          : 0
  },

  { // Interrupt receive (output) endpoint.
    endpoint_type      : USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT,
    endpoint_number    : 4,
    endpoint_direction : USB_ENDPOINT_DESCRIPTOR_ENDPOINT_OUT,
    endpoint           : (void*) &rxep_list[1],
    devtab_entry       : 0,
    min_size           : 0,
    max_size           : 8,  // Maximum for low speed devices.
    max_in_padding     : 0,
    alignment          : 0
  },

  USBS_TESTING_ENDPOINTS_TERMINATOR
};

#endif

//=============================================================================
