#ifndef CYGONCE_HAL_PPC_REGS_H
#define CYGONCE_HAL_PPC_REGS_H

// ppc_regs.h - register defines for PowerPC processors
// 
// Copyright (c) 1998 Cygnus Support
//
// The authors hereby grant permission to use, copy, modify, distribute,
// and license this software and its documentation for any purpose, provided
// that existing copyright notices are retained in all copies and that this
// notice is included verbatim in any distributions. No written agreement,
// license, or royalty fee is required for any of the authorized uses.
// Modifications to this software may be copyrighted by their authors
// and need not follow the licensing terms described here, provided that
// the new terms are clearly indicated on the first page of each file where
// they apply.

// PPC registers, numbered in the order in which gdb expects to see them.

#define R0              0
#define R1              1
#define R2              2
#define R3              3
#define R4              4
#define R5              5
#define R6              6
#define R7              7
#define R8              8
#define R9              9
#define R10             10
#define R11             11
#define R12             12
#define R13             13
#define R14             14
#define R15             15
#define R16             16
#define R17             17
#define R18             18
#define R19             19
#define R20             20
#define R21             21
#define R22             22
#define R23             23
#define R24             24
#define R25             25
#define R26             26
#define R27             27
#define R28             28
#define R29             29
#define R30             30
#define R31             31

#define F0              32
#define F1              33
#define F2              34
#define F3              35
#define F4              36
#define F5              37
#define F6              38
#define F7              39
#define F8              40
#define F9              41
#define F10             42
#define F11             43
#define F12             44
#define F13             45
#define F14             46
#define F15             47
#define F16             48
#define F17             49
#define F18             50
#define F19             51
#define F20             52
#define F21             53
#define F22             54
#define F23             55
#define F24             56
#define F25             57
#define F26             58
#define F27             59
#define F28             60
#define F29             61
#define F30             62
#define F31             63

#define PC              64
#define PS              65
#define CND             66
#define LR              67
#define CNT             68
#define XER             69
#define MQ              70

// For convenience
#define SP              R1

#ifdef CYG_HAL_POWERPC_MP860
// General purpose registers. No floating point registers on MP860.
#define FIRST_GP_REG    R0
#define LAST_GP_REG     R31

// CPU state registers.
#define FIRST_CS_REG    PC
#define LAST_CS_REG     MQ
#endif


// MSR bits
#define MSR_LE          0x00000001      // little-endian mode enable
#define MSR_RI          0x00000002      // recoverable exception
#define MSR_DR          0x00000010      // data address translation
#define MSR_IR          0x00000020      // instruction address translation
#define MSR_IP          0x00000040      // exception prefix
#define MSR_FE1         0x00000100      // floating-point exception mode 1
#define MSR_BE          0x00000200      // branch trace enable
#define MSR_SE          0x00000400      // single-step trace enable
#define MSR_FE0         0x00000800      // floating-point exception mode 0
#define MSR_ME          0x00001000      // machine check enable
#define MSR_FP          0x00002000      // floating-point available
#define MSR_PR          0x00004000      // privilege level
#define MSR_EE          0x00008000      // external interrupt enable
#define MSR_ILE         0x00010000      // exception little-endian mode
#define MSR_POW         0x00040000      // power management enable



#ifdef CYG_HAL_POWERPC_MP860
// Instruction cache control.
#define IC_CST          560
#define IC_ADR          561
#define IC_DAT          562

#define IC_CMD_CE       0x02000000      // cache enable
#define IC_CMD_CD       0x04000000      // cache disable
#define IC_CMD_LL       0x06000000      // load & lock
#define IC_CMD_UL       0x08000000      // unlock line
#define IC_CMD_UA       0x0a000000      // unlock all
#define IC_CMD_IA       0x0c000000      // invalidate all

// Data cache control.
#define DC_CST          568
#define DC_ADR          569
#define DC_DAT          570

#define DC_CMD_CE       0x02000000      // cache enable
#define DC_CMD_CD       0x04000000      // cache disable
#define DC_CMD_LL       0x06000000      // lock line
#define DC_CMD_UL       0x08000000      // unlock line
#define DC_CMD_UA       0x0a000000      // unlock all
#define DC_CMD_IA       0x0c000000      // invalidate all
#define DC_CMD_FL       0x0e000000      // flush line
#define DC_CMD_SW       0x01000000      // set writethrough
#define DC_CMD_CW       0x03000000      // clear writethrough
#define DC_CMD_SS       0x05000000      // set little endian swap
#define DC_CMD_CS       0x07000000      // clear little endian swap


// Internal Memory Map
#define IMM_BASE        0xff000000      // The 860 internal memory map base

#define IMM_SIPEND      (IMM_BASE + 0x010) // Interrupt Pend Register
#define IMM_SIMASK      (IMM_BASE + 0x014) // Interrupt Mask
#define IMM_SIEL        (IMM_BASE + 0x018) // Interrupt Edge Level Mask
#define IMM_SIVEC       (IMM_BASE + 0x01c) // Interrupt Vector

#define IMM_TBSCR       (IMM_BASE + 0x200) // Timebase Status and Control
#endif

#endif // ifdef CYGONCE_HAL_PPC_REGS_H
