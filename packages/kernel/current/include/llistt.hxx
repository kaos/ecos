#ifndef CYGONCE_KERNEL_LLISTT_HXX
#define CYGONCE_KERNEL_LLISTT_HXX

//==========================================================================
//
//      llistt.hxx
//
//      Llistt linked list template class declarations
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
// Author(s):   hmt
// Contributors:        hmt
// Date:        1998-02-10
// Purpose:     Define Llistt template class
// Description: The classes defined here provide the APIs for llistts.
// Usage:       #include <cyg/kernel/llistt.hxx>
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>
#include <cyg/infra/cyg_ass.h>            // assertion macros
#include <cyg/kernel/thread.hxx>

// -------------------------------------------------------------------------
// A simple linked list template; each item also contains a pointer of type
// T, and you can search for a particular T* in a list.
// 
// It is intended that this list be amenable to the trick of using the
// address of the pointer that is the list head, cast to an item pointer,
// as the "zeroth" element of the list; prev of the first item is the
// address of the head pointer, and inserting before the first item works
// correctly.  For this reason, a "getprev" is not provided; iteration may
// only be forwards, until a NULL is found.
//
// It is expected that derived classes will be used to hold other
// information than just the T* but that is beyond our discussion here;
// only the T* can be searched for using code provided here.
//
// This module is NOT thread-safe; it is expected that all clients will be
// seeing that that themselves.

template <class T>
class Cyg_Llistt
{
private:
    Cyg_Llistt<T> *next, *prev;
    T *tptr;

private:
    // make initialisation _without_ a T* impossible.
    Cyg_Llistt<T> &operator=(Cyg_Llistt<T> &);
    Cyg_Llistt(Cyg_Llistt<T> &);
    Cyg_Llistt();

public:

    CYGDBG_DEFINE_CHECK_THIS
    
    Cyg_Llistt( T *tvalue ) // Constructor
    {
        tptr = tvalue;
        next = prev = NULL;
    }

    ~Cyg_Llistt()                       // Destructor
    {
        CYG_ASSERT( NULL == next, "bad item next - still in list" );
        CYG_ASSERT( NULL == prev, "bad item prev - still in list" );
    }

    // iterator, basically.
    Cyg_Llistt<T> * getnext() { return next; }

    // get the value
    T * getitem() { return tptr; }

    // look up a particular T value in the llist
    static Cyg_Llistt<T> *
    find( Cyg_Llistt<T> *list, T *tvalue )
    {
        for ( ; list ; list = list->next ) {
            if ( list->tptr == tvalue )
                break;
        }
        return list;
    }

    // unlink an item from the list
    void
    unlink()
    {
        CYG_ASSERT( prev, "not in a list" );
        prev->next = next;
        if ( next ) {
            next->prev = prev;
        }
        next = prev = NULL;
    }

    // insert a new item in the list after "this"
    void
    insertafter( Cyg_Llistt<T> *item )
    {
        CYG_ASSERT( item, "null item" );
        CYG_ASSERT( NULL == item->next, "bad item next - already linked" );
        CYG_ASSERT( NULL == item->prev, "bad item prev - already linked" );
        item->next = next;
        item->prev = this;
        if ( next )
            next->prev = item;
        next = item;
    }   

    // insert a new item in the list before "this"
    void
    insertbefore( Cyg_Llistt<T> *item )
    {
        CYG_ASSERT( prev, "this not in a list" );
        CYG_ASSERT( item, "null item" );
        CYG_ASSERT( NULL == item->next, "bad item next - already linked" );
        CYG_ASSERT( NULL == item->prev, "bad item prev - already linked" );
        item->prev = prev;
        item->next = this;
        prev->next = item;
              prev = item;
    }   
};



// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_KERNEL_LLISTT_HXX
// EOF llistt.hxx
