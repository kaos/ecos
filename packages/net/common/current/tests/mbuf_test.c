//==========================================================================
//
//      tests/mbuf_test.c
//
//      Initial test for network mbufs
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

// mbuf test code

#include <sys/param.h>
#include <sys/mbuf.h>

#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>

void
cyg_start(void)
{
    int i;
    struct mbuf *m[32];
    diag_printf("Start mbuf test\n");

    for (i = 0;  i < 32;  i++) {
        m[i] = m_get(M_DONTWAIT, MT_DATA);
        diag_printf("allocate mbuf = 0x%p\n", m[i]);
    }
    
    cyg_test_exit();
}
