##=============================================================================
##
##	mips.inc
##
##	MIPS assembler header file
##
##=============================================================================
#####COPYRIGHTBEGIN####
#
# -------------------------------------------
# The contents of this file are subject to the Cygnus eCos Public License
# Version 1.0 (the "License"); you may not use this file except in
# compliance with the License.  You may obtain a copy of the License at
# http://sourceware.cygnus.com/ecos
# 
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
# License for the specific language governing rights and limitations under
# the License.
# 
# The Original Code is eCos - Embedded Cygnus Operating System, released
# September 30, 1998.
# 
# The Initial Developer of the Original Code is Cygnus.  Portions created
# by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
# -------------------------------------------
#
#####COPYRIGHTEND####
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s): 	nickg
## Contributors:	nickg
## Date:	1997-10-16
## Purpose:	MIPS definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the MIPS CPU family.
## Usage:
##		#include <cyg/hal/mips.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

##-----------------------------------------------------------------------------
## Standard MIPS register names:

#define zero	$0
#define z0	$0
#define v0	$2
#define v1	$3
#define a0	$4
#define a1	$5
#define a2	$6
#define a3	$7
#define t0	$8
#define t1	$9
#define t2	$10
#define t3	$11
#define t4	$12
#define t5	$13
#define t6	$14
#define t7	$15
#define s0	$16
#define s1	$17
#define s2	$18
#define s3	$19
#define s4	$20
#define s5	$21
#define s6	$22
#define s7	$23
#define t8	$24
#define t9	$25
#define k0	$26	/* kernel private register 0 */
#define k1	$27	/* kernel private register 1 */
#define gp	$28	/* global data pointer */
#define sp 	$29	/* stack-pointer */
#define fp	$30	/* frame-pointer */
#define ra	$31	/* return address */
#define pc	$pc	/* pc, used on mips16 */

#define config	$3	// Configuration register	
#define cache	$7	// Cache control
#define badvr	$8	// Bad virtual address		
#define sr	$12	// Status register	
#define cause	$13	// Exception cause
#define	epc	$14	// Exception pc value
#define prid	$15	// processor ID	

##-----------------------------------------------------------------------------
## MIPS saved state
	
#define	mipsreg_regs	0
#define	mipsreg_hi	(4*32)
#define	mipsreg_lo	(mipsreg_hi+4)
#define	mipsreg_vector	(mipsreg_lo+4)
#define	mipsreg_pc	(mipsreg_vector+4)
#define	mipsreg_sr	(mipsreg_pc+4)
#define	mipsreg_cache	(mipsreg_sr+4)
#define	mipsreg_cause	(mipsreg_cache+4)
#define	mipsreg_badvr	(mipsreg_cause+4)
#define	mipsreg_prid	(mipsreg_badvr+4)
#define	mipsreg_config	(mipsreg_prid+4)
#define	mipsreg_size	(mipsreg_config+4)

#define mips_exception_decrement	(mipsreg_size*2)	

#define mips_stack_frame_size		24	// 3 (64 bit) args worth

#------------------------------------------------------------------------------
# end of mips.inc
