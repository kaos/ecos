#ifndef CYGONCE_KERNEL_INTR_HXX
#define CYGONCE_KERNEL_INTR_HXX

//==========================================================================
//
//	intr.hxx
//
//	Interrupt class declaration(s)
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	nickg
// Contributors:	nickg
// Date:	1997-09-09
// Purpose:	Define Interrupt class interfaces
// Description:	The classes defined here provide the APIs for handling
//              interrupts.
// Usage:       #include "intr.hxx"
//		
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>

// -------------------------------------------------------------------------
// Function prototype typedefs


// VSR = Vector Service Routine. This is the code attached directly to an
// interrupt vector. It is very architecture/platform specific and usually
// must be written in assembler.

typedef void  cyg_VSR();

// ISR = Interrupt Service Routine. This is called from the default
// VSR in response to an interrupt. It may access shared data but may
// not call kernel routines. The return value may be
// Cyg_Interrupt::HANDLED and/or Cyg_Interrupt::CALL_DSR.

typedef cyg_uint32 cyg_ISR(cyg_vector vector, CYG_ADDRWORD data);

// DSR = Deferred Service Routine. This is called if the ISR returns
// the Cyg_Interrupt::CALL_DSR bit. It is called at a "safe" point in
// the kernel where it may make calls on kernel routines. The count
// argument indicates how many times the ISR has asked for the DSR to
// be posted since the last time the DSR ran.

typedef void cyg_DSR(cyg_vector vector, cyg_ucount32 count, CYG_ADDRWORD data);

// -------------------------------------------------------------------------
// Include HAL definitions

class Cyg_Interrupt;

#include <cyg/hal/hal_arch.h>

#include <cyg/hal/hal_intr.h>

externC void interrupt_end(
    cyg_uint32          isr_ret,
    Cyg_Interrupt       *intr,
    HAL_SavedRegisters  *ctx
    );

// -------------------------------------------------------------------------
// Interrupt class. This both represents each interrupt and provides a static
// interface for controlling the interrupt hardware.

class Cyg_Interrupt
{

    friend class Cyg_Scheduler;
    friend void interrupt_end( cyg_uint32,
                               Cyg_Interrupt *,
                               HAL_SavedRegisters *);
    
    cyg_vector          vector;         // Interrupt vector

    cyg_priority        priority;       // Queuing priority
    
    cyg_ISR             *isr;           // Pointer to ISR

    cyg_DSR             *dsr;           // Pointer to DSR

    CYG_ADDRWORD        data;           // Data pointer


    
    // DSR handling interface called by the scheduler

                                        // Check for pending DSRs
    static cyg_bool     DSRs_pending();

                                        // Call any pending DSRs
    static void         call_pending_DSRs();

    void                post_dsr();     // Post the DSR for this interrupt
    
    // Data structures for handling DSR calls.  We implement two DSR
    // handling mechanisms, a list based one and a table based
    // one. The list based mechanism is safe with respect to temporary
    // overloads and will not run out of resource. However it requires
    // extra data per interrupt object, and interrupts must be turned
    // off briefly when delivering the DSR. The table based mechanism
    // does not need unnecessary interrupt switching, but may be prone
    // to overflow on overload. However, since a correctly programmed
    // real time application should not experience such a condition,
    // the table based mechanism is more efficient for real use. The
    // list based mechainsm is enabled by default since it is safer to
    // use during development.

#ifdef CYGIMP_KERNEL_INTERRUPTS_DSRS_TABLE
    
    static Cyg_Interrupt *dsr_table[CYGNUM_KERNEL_INTERRUPTS_DSRS_TABLE_SIZE];

    static cyg_ucount32 dsr_table_head;

    static cyg_ucount32 dsr_table_tail;

#endif
#ifdef CYGIMP_KERNEL_INTERRUPTS_DSRS_LIST

    cyg_ucount32 dsr_count;             // Number of DSR posts made

    Cyg_Interrupt *next_dsr;            // next DSR in list
    
    static Cyg_Interrupt *dsr_list;     // static list of pending DSRs
    
#endif

#ifdef CYGIMP_KERNEL_INTERRUPTS_CHAIN

    // The default mechanism for handling interrupts is to attach just
    // one Interrupt object to each vector. In some cases, and on some
    // hardware, this is not possible, and each vector must carry a chain
    // of interrupts.

    Cyg_Interrupt       *next;          // Next Interrupt in list

    // Chaining ISR inserted in HAL vector
    static cyg_uint32 chain_isr(cyg_vector vector, CYG_ADDRWORD data);    

    // Table of interrupt chains
    static Cyg_Interrupt *chain_list[CYG_ISR_COUNT];
    
#endif
    
public:

    Cyg_Interrupt                       // Initialize interrupt
    (
        cyg_vector      vector,         // Vector to attach to
        cyg_priority    priority,       // Queue priority
        CYG_ADDRWORD    data,           // Data pointer
        cyg_ISR         *isr,           // Interrupt Service Routine
        cyg_DSR         *dsr            // Deferred Service Routine
        );

    ~Cyg_Interrupt();
        
    // ISR return values
    enum {
        HANDLED  = 1,                   // Interrupt was handled
        CALL_DSR = 2                    // Schedule DSR
    };

    // Interrupt management
        
    void        attach();               // Attach to vector


    void        detach();               // Detach from vector
        
    
    // Static Interrupt management functions

    // Get the current service routine
    static void get_vsr(cyg_vector vector, cyg_VSR **vsr);

    // Install a vector service routine
    static void set_vsr(
        cyg_vector vector,              // hardware vector to replace
        cyg_VSR *vsr,                   // my new service routine
        cyg_VSR **old = NULL            // pointer to old vsr, if required
        );


    // Static interrupt masking functions

    // Disable interrupts at the CPU
    static void disable_interrupts();

    // Re-enable CPU interrupts
    static void enable_interrupts();

    // Are interrupts enabled at the CPU?
    static inline cyg_bool interrupts_enabled()
    {
        CYG_WORD result;                // needs to be register sized
        HAL_QUERY_INTERRUPTS( result );
        return (0 != result);
    }
    
    // Get the vector for the following calls
    inline cyg_vector get_vector() 
    {
        return vector;
    }
    
    // Static PIC control functions
    
    // Mask a specific interrupt in a PIC
    static void mask_interrupt(cyg_vector vector);

    // Clear PIC mask
    static void unmask_interrupt(cyg_vector vector);

    // Acknowledge interrupt at PIC
    static void acknowledge_interrupt(cyg_vector vector);

    // Change interrupt detection at PIC
    static void configure_interrupt(
        cyg_vector vector,              // vector to control
        cyg_bool level,                 // level or edge triggered
        cyg_bool up                     // hi/lo level, rising/falling edge
        );
    
};

#ifdef CYGIMP_KERNEL_INTERRUPTS_DSRS
// -------------------------------------------------------------------------
// Check for pending DSRs

inline cyg_bool Cyg_Interrupt::DSRs_pending()
{
#ifdef CYGIMP_KERNEL_INTERRUPTS_DSRS_TABLE
    
    return dsr_table_head != dsr_table_tail;

#endif
#ifdef CYGIMP_KERNEL_INTERRUPTS_DSRS_LIST
    
    return dsr_list != NULL;

#endif
};
#endif // CYGIMP_KERNEL_INTERRUPTS_DSRS

// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_KERNEL_INTR_HXX
// EOF intr.hxx
