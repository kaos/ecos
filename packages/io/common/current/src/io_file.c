//==========================================================================
//
//      io/common/io_file.c
//
//      High-level file I/O support.
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


// File I/O support

#include <pkgconf/io.h>
#include <cyg/io/file.h>
#include <sys/errno.h>

struct file fds[CYGPKG_IO_NFILE];

//
// This function allcates a file "slot".
//
int
falloc(struct file **fp, int *fd)
{
    int i;
    struct file *f;
    f = &fds[0];
    for (i = 0;  i < CYGPKG_IO_NFILE;  i++, f++) {
        if (f->f_flag == 0) {
            f->f_flag = FALLOC;
            *fp = f;
            *fd = i;
            return 0;
        }
    }
    return -EMFILE;  // No more files
}

//
// This function is used to return a file slot.
//
void
ffree(struct file *fp)
{
    fp->f_flag = 0;  // Mark free
}

//
// This function provides the mapping from a file descriptor (small
// integer used by application code) to the corresponding file slot.
cyg_bool
getfp(int fd, struct file **fp)
{
    struct file *f;
    if (fd >= CYGPKG_IO_NFILE)
        return -1;
    f =  &fds[fd];
    if (f->f_flag == 0)
        return -1;
    *fp = f;
    return 0;
}

