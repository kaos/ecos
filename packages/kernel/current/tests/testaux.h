#ifndef CYGONCE_KERNEL_TESTS_TESTAUX_H
#define CYGONCE_KERNEL_TESTS_TESTAUX_H

/*=================================================================
//
//        testaux.h
//
//        Auxiliary test header file
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Date:          1998-03-09
// Description:
//     Defines some convenience functions to get us going.  In
//     particular this file reserves space for NTHREADS threads,
//     which can be created by calls to aux_new_thread()
//     It also defines a CHECK function.
//
//####DESCRIPTIONEND####
*/

#define CHECK(b) CYG_TEST_CHECK(b,#b)

#endif /* ifndef CYGONCE_KERNEL_TESTS_TESTAUX_H */

/* EOF testaux.h */
