//==========================================================================
//
//        clock0.cxx
//
//        Clock test 0
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     dsm
// Contributors:    dsm
// Date:          1998-02-13
// Description:   Tests some basic clock functions.
// Omissions:     Doesn't test likely boundary conditions for
//                CYGNUM_KERNEL_COUNTERS_MULTI_LIST_SIZE
//                Real Time Clock Testing is limited
// Options:
//     CYGIMP_KERNEL_COUNTERS_SINGLE_LIST
//     CYGIMP_KERNEL_COUNTERS_MULTI_LIST
//     CYGVAR_KERNEL_COUNTERS_CLOCK
//     CYGNUM_KERNEL_COUNTERS_MULTI_LIST_SIZE
// Assumptions:   This assumes we have long long support and
//                that counters are 64 bits.
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/clock.hxx>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/clock.inl>

#include "testaux.hxx"

#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK

cyg_alarm_fn call_me;

bool flash( void )
{
    Cyg_Counter counter0 = Cyg_Counter();
    Cyg_Counter counter1 = Cyg_Counter(723);
    
    CYG_ASSERTCLASSO( counter0, "error" );
    CYG_ASSERTCLASSO( counter1, "error" );

    Cyg_Alarm alarm0 = Cyg_Alarm(&counter0, call_me, 12);

    CYG_ASSERTCLASSO( alarm0, "error" );

    Cyg_Clock::cyg_resolution res = {1,2};

    Cyg_Clock clock0(res);

    CYG_ASSERTCLASSO( clock0, "error" );

    return true;
}

// Testing alarms
//
// call_me is a function that will be called when an alarm is
// triggered.  It updates a global variable called which is CHECKed
// explicitly to see if the approriate alarms have been called.

cyg_uint16 called = 0x0;

void call_me(Cyg_Alarm *alarm, CYG_ADDRWORD data)
{
    called ^= data;
}

void call_me2(Cyg_Alarm *alarm, CYG_ADDRWORD data)
{
    call_me(alarm, data^0x10);
}


void clock0_main(void)
{
    CYG_TEST_INIT();

    CHECK(flash());
    CHECK(flash());

    const cyg_uint32 big_number = 3333222111u;
    Cyg_Counter counter0 = Cyg_Counter();

    CHECK( 0 == counter0.current_value() );
    CHECK( 0 == counter0.current_value_lo() );
    CHECK( 0 == counter0.current_value_hi() );

    counter0.tick();

    CHECK( 1 == counter0.current_value() );
    CHECK( 1 == counter0.current_value_lo() );
    CHECK( 0 == counter0.current_value_hi() );

    counter0.tick(6);

    CHECK( 7 == counter0.current_value() );
    CHECK( 7 == counter0.current_value_lo() );
    CHECK( 0 == counter0.current_value_hi() );

    counter0.set_value( 0xfffffffc );

    CHECK( 0xfffffffc == counter0.current_value() );
    CHECK( 0xfffffffc == counter0.current_value_lo() );
    CHECK( 0 == counter0.current_value_hi() );

    counter0.tick( 0x13 );      // Overflows 32 bits
    
    CHECK( 0x10000000fULL == counter0.current_value() );
    CHECK( 0xf == counter0.current_value_lo() );
    CHECK( 0x1 == counter0.current_value_hi() );

    
    Cyg_Counter counter1 = Cyg_Counter(big_number);

    CHECK( 0 == counter1.current_value() );
    CHECK( 0 == counter1.current_value_lo() );
    CHECK( 0 == counter1.current_value_hi() );

    counter1.tick(2);
    
    CHECK( 2ll*big_number == counter1.current_value() );
    CHECK( ((2ll*big_number) & 0xffffffff) ==
                counter1.current_value_lo() );
    CHECK( ((2ll*big_number) >> 32) == counter1.current_value_hi() );

    counter1.tick();
    
    CHECK( 3ll*big_number == counter1.current_value() );
    CHECK( ((3ll*big_number) & 0xffffffff) ==
                counter1.current_value_lo() );
    CHECK( ((3ll*big_number) >> 32) == counter1.current_value_hi() );

    counter1.tick();
    
    CHECK( 4ll*big_number == counter1.current_value() );
    CHECK( ((4ll*big_number) & 0xffffffff) ==
                counter1.current_value_lo() );
    CHECK( ((4ll*big_number) >> 32) == counter1.current_value_hi() );

    counter1.set_value(1222333444555ll);
    CHECK( 1222333444555ll == counter1.current_value() );

    counter0.set_value(11);
    CHECK( 11 == counter0.current_value() );
    
    // the call_me functions cause the "called" bits to toggle
    // CHECKing the value of called TEST_CHECKs the parity of # of calls
    // made by each alarm.

    Cyg_Alarm alarm0 = Cyg_Alarm(&counter0, call_me, 0x1);
    Cyg_Alarm alarm1 = Cyg_Alarm(&counter0, call_me, 0x2);
    Cyg_Alarm alarm2 = Cyg_Alarm(&counter0, call_me2, 0x4);
    
    CHECK( 0x00 == called );
    alarm0.initialize(12,3);
    alarm2.initialize(21,2);
    CHECK( 0x00 == called );

    counter0.tick();            // 12 a0
    CHECK( 0x01 == called );

    alarm1.initialize(13,0);
    counter0.tick();            // 13 a1
    CHECK( 0x03 == called );

    alarm1.initialize(17,0);
    counter0.tick();            // 14
    CHECK( 0x03 == called );

    counter0.tick();            // 15 a0
    CHECK( 0x02 == called );

    counter0.tick(2);           // 17 a1
    CHECK( 0x00 == called );

    counter0.tick();            // 18 a0
    CHECK( 0x01 == called );

    counter0.tick(3);           // 21 a0 a2
    CHECK( 0x14 == called );

    counter0.tick(2);           // 23 a2
    CHECK( 0x00 == called );
    
    alarm2.disable();  

    counter0.tick(2);           // 25 a0(24)
    CHECK( 0x01 == called );
    
    alarm2.enable();            // a2 (enabled at 25)
    CHECK( 0x15 == called );

    counter0.tick();            // 26
    CHECK( 0x15 == called );
    
    counter0.tick(2);           // 28 a0(27) a2(27)
    CHECK( 0x00 == called );
    
    counter0.tick(3);           // 31 a0(30) a2(29 31)
    CHECK( 0x01 == called );

    Cyg_Clock::cyg_resolution res0;

    res0.dividend = 100;
    res0.divisor = 3;

    Cyg_Clock::cyg_resolution res1;

    Cyg_Clock clock0 = Cyg_Clock(res0);

    res1 = clock0.get_resolution();
    CHECK( res0.dividend == res1.dividend );
    CHECK( res0.divisor == res1.divisor );

    res1.dividend = 12;
    res1.divisor = 25;

    clock0.set_resolution(res1);
    res0 = clock0.get_resolution();
    CHECK( res0.dividend == res1.dividend );
    CHECK( res0.divisor == res1.divisor );

    res0 = Cyg_Clock::real_time_clock->get_resolution();

    CYG_TEST_PASS_FINISH("Clock 0 OK");
}

externC void
cyg_start( void )
{
    clock0_main();
}

#else // def CYGVAR_KERNEL_COUNTERS_CLOCK

externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA( "Kernel real-time clock disabled");
}

#endif // def CYGVAR_KERNEL_COUNTERS_CLOCK
// EOF clock0.cxx
