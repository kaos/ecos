#ifndef CYGONCE_IO_FILEIO_LIMITS_H
#define CYGONCE_IO_FILEIO_LIMITS_H
/*============================================================================
//
//      limits.h
//
//      Fileio limits header
//
//============================================================================
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
//============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-09-11
// Purpose:       Provide POSIX limits for file I/O related properties
// Description:   
//              
// Usage:         #include <limits.h>
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================*/

#include <pkgconf/io_fileio.h>

#define OPEN_MAX CYGNUM_FILEIO_NFILE

/*--------------------------------------------------------------------------*/
#endif /* ifndef CYGONCE_IO_FILEIO_LIMITS_H */
/* End of fileio.h */
