#ifndef CYGONCE_SEMAPHORE_H
#define CYGONCE_SEMAPHORE_H
//=============================================================================
//
//      semaphore.h
//
//      POSIX semaphore header
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     nickg
// Contributors:  nickg
// Date:          2000-03-17
// Purpose:       POSIX semaphore header
// Description:   This header contains all the definitions needed to support
//                semaphores under eCos. The reader is referred to the POSIX
//                standard or equivalent documentation for details of the
//                functionality contained herein.
//              
// Usage:
//              #include <semaphore.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <pkgconf/posix.h>

#include <stddef.h>             // NULL, size_t

#include <limits.h>

#include <sys/types.h>

//-----------------------------------------------------------------------------
// Semaphore object definition

// This structure must exactly correspond in size and layout to the underlying
// eCos C++ class that implements this object.

typedef struct
{
  CYG_WORD32    sem_value;
  CYG_ADDRESS   sem_queue;
} sem_t;

//-----------------------------------------------------------------------------
// Semaphore functions

// Initialize semaphore to value.
// pshared is not supported under eCos.
externC int sem_init  (sem_t *sem, int pshared, unsigned int value);

// Destroy the semaphore.
externC int sem_destroy  (sem_t *sem);

// Decrement value if >0 or wait for a post.
externC int sem_wait  (sem_t *sem);

// Decrement value if >0, return -1 if not.
externC int sem_trywait  (sem_t *sem);

// Increment value and wake a waiter if one is present.
externC int sem_post  (sem_t *sem);

// Get current value
externC int sem_getvalue  (sem_t *sem, int *sval);

//-----------------------------------------------------------------------------
// Named semaphore functions
// These are an optional feature under eCos

// Open an existing named semaphore, or create it.
externC sem_t *sem_open  (const char *name, int oflag, ...);

// Close descriptor for semaphore.
externC int sem_close  (sem_t *sem);

// Remove named semaphore
externC int sem_unlink  (const char *name);

//-----------------------------------------------------------------------------
// Special return value for sem_open()

#define SEM_FAILED      ((sem_t *)NULL)

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_SEMAPHORE_H
// End of semaphore.h
