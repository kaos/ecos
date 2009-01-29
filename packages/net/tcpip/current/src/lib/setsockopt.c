//==========================================================================
//
//      lib/setsockopt.c
//
//      setsockopt() system call
//
//==========================================================================
// ####BSDALTCOPYRIGHTBEGIN####                                             
// -------------------------------------------                              
// Portions of this software may have been derived from OpenBSD             
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


#include <sys/param.h>
#include <cyg/io/file.h>
#include <sys/socket.h>
#include <sys/socketvar.h>

#include <sys/syscallargs.h>

int 
setsockopt(int s, int level, int name, const void *val, socklen_t valsize)
{
    struct sys_setsockopt_args args;
    int res, error;
    SYSCALLARG(args,s) = s;
    SYSCALLARG(args,level) = level;
    SYSCALLARG(args,name) = name;
    SYSCALLARG(args,val) = val;
    SYSCALLARG(args,valsize) = valsize;
    error = sys_setsockopt(&args, &res);
    if (error) {
        errno = error;
        return -1;
    } else {
        return 0;
    }
}
