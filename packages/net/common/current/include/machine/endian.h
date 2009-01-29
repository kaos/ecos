//==========================================================================
//
//      include/machine/endian.h
//
//      Architecture/platform specific byte ordering support
//
//==========================================================================
// ####BSDALTCOPYRIGHTBEGIN####                                             
// -------------------------------------------                              
// Portions of this software may have been derived from FreeBSD, OpenBSD,   
// or other sources, and if so are covered by the appropriate copyright     
// and license included herein.                                             
// -------------------------------------------                              
// ####BSDALTCOPYRIGHTEND####                                               
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-01-10
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================


#ifndef _MACHINE_ENDIAN_H_
#define _MACHINE_ENDIAN_H_

#include <cyg/hal/basetype.h>

#if CYG_BYTEORDER == CYG_MSBFIRST
#define BYTE_ORDER BIG_ENDIAN
#else
#define BYTE_ORDER LITTLE_ENDIAN
#endif
#include <sys/endian.h>

#endif // _MACHINE_ENDIAN_H_


