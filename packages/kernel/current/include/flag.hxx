#ifndef CYGONCE_KERNEL_FLAG_HXX
#define CYGONCE_KERNEL_FLAG_HXX

//==========================================================================
//
//	flag.hxx
//
//	Flag object class declarations
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
// Author(s): 	hmt
// Contributors:	hmt
// Date:	1998-02-10
// Purpose:	Define Flag class interfaces
// Description:	The classes defined here provide the APIs for flags.
// Usage:	#include <cyg/kernel/flag.hxx>
//		
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>
#include <cyg/infra/cyg_ass.h>                     // assertion macros
#include <cyg/kernel/thread.hxx>                   // Cyg_Thread

// -------------------------------------------------------------------------

// Flag object.  This class implements a queue of threads waiting for a
// boolean expression of the flag value (an integer) to become true; all
// relevant threads are awoken, not just the first.  A variant on this is
// the single-bit flag, which is implemented by means of default arguments.

#ifdef CYGIMP_FLAGS_16BIT
typedef cyg_uint16 Cyg_FlagValue;
#define CYG_FLAGS_SIZE 16
#endif
#ifdef CYGIMP_FLAGS_32BIT
typedef cyg_uint32 Cyg_FlagValue;
#define CYG_FLAGS_SIZE 32
#endif
#ifdef CYGIMP_FLAGS_64BIT
typedef cyg_uint64 Cyg_FlagValue;
#define CYG_FLAGS_SIZE 64
#endif

#ifndef CYG_FLAGS_SIZE
typedef cyg_uint32 Cyg_FlagValue;
#define CYG_FLAGS_SIZE 32
#endif



class Cyg_Flag
{
private:
    Cyg_FlagValue value;

    class FlagWaitInfo
    {
    public:
        Cyg_FlagValue	allmask;        // these are separate words to
        Cyg_FlagValue	anymask;        // save time in wakeup.
        Cyg_FlagValue	value_out;      // return the value that satisfied
        cyg_bool	do_clear;

        FlagWaitInfo() { value_out = 0; }
    };

    Cyg_ThreadQueue     queue;          // Queue of waiting threads

public:

    CYGDBG_DEFINE_CHECK_THIS
    
    Cyg_Flag( Cyg_FlagValue init = 0 ); // Constructor
    ~Cyg_Flag();                        // Destructor
        
    void
    setbits( Cyg_FlagValue arg = ~0 );  // -OR- the arg in
    // not inlined; this function awakens affected threads.

    void
    maskbits( Cyg_FlagValue arg = 0 );  // -AND- it in
    // this is not inlined because it needs to lock the scheduler;
    // it only really does value &= arg; nobody can be awoken in consequence.

    typedef cyg_uint8 WaitMode;
    // These values are chosen to map directly to uITRON for emulation
    // purposes:
    static const WaitMode AND = 0;      // all specified bits must be set
    static const WaitMode OR  = 2;      // any specified bit must be set
    static const WaitMode CLR = 1;      // clear value when satisfied
    static const WaitMode MASK= 3;      // might be useful

    // Wait for a match on our pattern, according to the flags given.
    // Return the matching value, or zero if interrupted.
    Cyg_FlagValue
    wait( Cyg_FlagValue pattern, WaitMode mode );

    // Wait for a match on our pattern, with an absolute timeout.
    // Return the matching value, or zero if timed out/interrupted.
    // (zero cannot match any pattern).
#ifdef CYGFUN_KERNEL_THREADS_TIMER
    Cyg_FlagValue
    wait( Cyg_FlagValue pattern, WaitMode mode,
          cyg_tick_count abs_timeout );
#endif
    // Test for a match on our pattern, according to the flags given.
    // Return the matching value if success, else zero.
    Cyg_FlagValue
    poll( Cyg_FlagValue pattern, WaitMode mode ); 

    inline Cyg_FlagValue
    peek()                              // Get current value
    {
        return value;                   // NOT atomic wrt threads
    }

    inline cyg_bool
    waiting()                           // Any threads waiting?
    {
        return !queue.empty();
    }
};



// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_KERNEL_FLAG_HXX
// EOF flag.hxx
