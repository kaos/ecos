//=================================================================
//
//        intr0.c
//
//        Interrupt test 0
//
//=================================================================
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
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jskov
// Contributors:  jskov
// Date:          1998-12-01
// Description:   Simple test of MPC860 interrupt handling when the
//                kernel has not been configured. Uses timer interrupts.
// Options:
//####DESCRIPTIONEND####

//#define DEBUG_PRINTFS
#ifdef DEBUG_PRINTFS
extern diag_printf( char *format, ... );
#endif

#include <pkgconf/hal.h>

#define CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#include <cyg/hal/ppc_regs.h>

#include <cyg/hal/hal_intr.h>

#include <cyg/infra/testcase.h>

#if defined(CYG_HAL_POWERPC_MPC860)

#undef CHECK(b)
#define CHECK(b) CYG_TEST_CHECK(b,#b)

// Can't rely on Cyg_Interrupt class being defined.
#define Cyg_InterruptHANDLED 1

// This is the period between interrupts, measured in decrementer ticks.
// Period must be longer than the time required for setting up all the
// interrupt handlers.
#define PIT_PERIOD 5000

#ifdef CYG_HAL_POWERPC_MBX
#define TB_PERIOD (PIT_PERIOD*384)      // PTA period is 15.36 uS
#else
#define TB_PERIOD (PIT_PERIOD*32)       // assuming 512/16 divisors
#endif

#define ID_RTC_SEC   12345
#define ID_RTC_ALR   23451
#define ID_PIT       34512
#define ID_TBA       45123
#define ID_TBB       51234

volatile cyg_uint32 count = 0;

// Time/PERIOD    0   1   2   3   4   5   6   7   8   9   10
// Interrupt             PIT TBA PIT     PIT TBB PIT     PIT
// pit_count      0   0   0   1   1   2   2   3   3   4   4
// count          0   0   1   3   4   4   5   40  41      42

static cyg_uint32 count_verify_table[] = {1, 4, 5, 41, 42};
static int pit_count = 0;

// These are useful for debugging:
static cyg_uint32 count_actual_table[] = { -1, -1, -1, -1, -1};
static cyg_uint32 tbr_actual_table[] = { -1, -1, -1, -1, -1};

// Periodic timer ISR. Should be executing 5 times.
static cyg_uint32 isr_pit(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    cyg_uint32 verify_value;

    CYG_UNUSED_PARAM(CYG_ADDRWORD, data);

    CYG_ASSERT (CYGNUM_HAL_INTERRUPT_SIU_PIT == vector, "Wrong vector!");
    CYG_ASSERT (ID_PIT == data, "Wrong data!");

    HAL_INTERRUPT_ACKNOWLEDGE (CYGNUM_HAL_INTERRUPT_SIU_PIT);

    count++;

    count_actual_table[pit_count] = count;
    {
        cyg_uint32 tbl;
        CYGARC_MFTB (TBL_R, tbl);
        tbr_actual_table[pit_count] = tbl;
    }

    verify_value = count_verify_table[pit_count++];

#ifdef DEBUG_PRINTFS
    diag_printf( "ISR_PIT executed %d of 5\n", pit_count );
#endif

    CYG_ASSERT (count == verify_value, "Count wrong!");

    // End of test when count is 42. Mask interrupts and print PASS text.
    if (42 <= count || 5 == pit_count) {
        HAL_INTERRUPT_MASK (CYGNUM_HAL_INTERRUPT_SIU_PIT);
        HAL_INTERRUPT_MASK (CYGNUM_HAL_INTERRUPT_SIU_TB_A);
        HAL_INTERRUPT_MASK (CYGNUM_HAL_INTERRUPT_SIU_TB_B);

#ifdef DEBUG_PRINTFS
        diag_printf( "INFO: Actual counts: %d %d %d %d %d\n",
                     count_actual_table[0],
                     count_actual_table[1],
                     count_actual_table[2],
                     count_actual_table[3],
                     count_actual_table[4] );
        diag_printf( "INFO: Actuals tbrs: %d %d %d %d %d\n",
                     tbr_actual_table[0],
                     tbr_actual_table[1],
                     tbr_actual_table[2],
                     tbr_actual_table[3],
                     tbr_actual_table[4] );
#endif
        if (42 == count && 5 == pit_count)
            CYG_TEST_PASS_FINISH("Intr 0 OK");
        else
            CYG_TEST_FAIL_FINISH("Intr 0 Failed.");
    }

    return Cyg_InterruptHANDLED;
}

// TimeBase A ISR. Should be executing once.
static cyg_uint32 isr_tba(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    CYG_UNUSED_PARAM(CYG_ADDRWORD, data);

    CYG_ASSERT (CYGNUM_HAL_INTERRUPT_SIU_TB_A == vector, "Wrong vector!");
    CYG_ASSERT (ID_TBA == data, "Wrong data!");

    HAL_INTERRUPT_ACKNOWLEDGE (CYGNUM_HAL_INTERRUPT_SIU_TB_A);

    count = count * 3;

#ifdef DEBUG_PRINTFS
    diag_printf( "ISR_TBA executed\n" );
#endif

    return Cyg_InterruptHANDLED;
}

// TimeBase B ISR. Should be executing once.
static cyg_uint32 isr_tbb(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    CYG_UNUSED_PARAM(CYG_ADDRWORD, data);

    CYG_ASSERT (CYGNUM_HAL_INTERRUPT_SIU_TB_B == vector, "Wrong vector!");
    CYG_ASSERT (ID_TBB == data, "Wrong data!");

    HAL_INTERRUPT_ACKNOWLEDGE (CYGNUM_HAL_INTERRUPT_SIU_TB_B);

    count = count * 8;

#ifdef DEBUG_PRINTFS
    diag_printf( "ISR_TBB executed\n" );
#endif

    return Cyg_InterruptHANDLED;
}

void intr0_main( void )
{
    CYG_TEST_INIT();

#if 0
    // The A.3 revision of the CPU I'm using at the moment generates a
    // machine check exception when writing to IMM_RTCSC.  Smells a
    // bit like the "SIU4. Spurious External Bus Transaction Following
    // PLPRCR Write." CPU errata. Have to find out for sure.  Run real
    // time clock interrupts on level 0
    {
        // Still to do.
    }
#endif

    // Run periodic timer interrupt on level 1
    {
        cyg_uint16 piscr;

        // Attach pit arbiter.
        HAL_INTERRUPT_ATTACH (CYGNUM_HAL_INTERRUPT_SIU_LVL1, 
                              &hal_arbitration_isr_pit, ID_PIT, 0);
        HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_SIU_LVL1);

        // Attach pit isr.
        HAL_INTERRUPT_ATTACH (CYGNUM_HAL_INTERRUPT_SIU_PIT, &isr_pit,
                              ID_PIT, 0);
        HAL_INTERRUPT_SET_LEVEL (CYGNUM_HAL_INTERRUPT_SIU_PIT, 1);
        HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_SIU_PIT);


        // Set period.
        HAL_WRITE_UINT32 (CYGARC_REG_IMM_PITC, 
                          (2*PIT_PERIOD) << CYGARC_REG_IMM_PITC_COUNT_SHIFT);

#ifdef DEBUG_PRINTFS
        diag_printf( "PIT set to %d\n", 2*PIT_PERIOD );
#endif
        // Enable.
        HAL_READ_UINT16 (CYGARC_REG_IMM_PISCR, piscr);
        piscr |= CYGARC_REG_IMM_PISCR_PTE;
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_PISCR, piscr);
    }

    // Run timebase interrupts on level 2
    {
        cyg_uint16 tbscr;
        cyg_uint32 tbl;

        // Attach tb arbiter.
        HAL_INTERRUPT_ATTACH (CYGNUM_HAL_INTERRUPT_SIU_LVL2, 
                              &hal_arbitration_isr_tb, ID_TBA, 0);
        HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_SIU_LVL2);

        // Attach tb isrs.
        HAL_INTERRUPT_ATTACH (CYGNUM_HAL_INTERRUPT_SIU_TB_A, &isr_tba,
                              ID_TBA, 0);
        HAL_INTERRUPT_ATTACH (CYGNUM_HAL_INTERRUPT_SIU_TB_B, &isr_tbb,
                              ID_TBB, 0);
        HAL_INTERRUPT_SET_LEVEL (CYGNUM_HAL_INTERRUPT_SIU_TB_A, 2);
        HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_SIU_TB_A);
        HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_SIU_TB_B);

        // Set reference A & B registers.
        CYGARC_MFTB (TBL_R, tbl);
        tbl += TB_PERIOD*3;
        HAL_WRITE_UINT32 (CYGARC_REG_IMM_TBREF0, tbl);
        tbl += TB_PERIOD*4;
        HAL_WRITE_UINT32 (CYGARC_REG_IMM_TBREF1, tbl);

#ifdef DEBUG_PRINTFS
        diag_printf( "TB initial %d, !1 %d !2 %d\n",
                     tbl - 7*TB_PERIOD,
                     tbl - 4*TB_PERIOD,
                     tbl - 0*TB_PERIOD );
#endif
        // Enable.
        HAL_READ_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
        tbscr |= (CYGARC_REG_IMM_TBSCR_REFA | CYGARC_REG_IMM_TBSCR_REFB |
                  CYGARC_REG_IMM_TBSCR_TBE);
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
        tbscr |= CYGARC_REG_IMM_TBSCR_REFAE | CYGARC_REG_IMM_TBSCR_REFBE;
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
    }

    HAL_ENABLE_INTERRUPTS();

    for (;;);
}

externC void
cyg_start( void )
{
    intr0_main();
}

#else  // ifdef CYG_HAL_POWERPC_MPC860

externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_PASS_FINISH("N/A: Not a MPC860");
}

#endif // ifdef CYG_HAL_POWERPC_MPC860

// EOF intr0.c
