#ifndef CYGONCE_INFRA_CLIST_HXX
#define CYGONCE_INFRA_CLIST_HXX

//==========================================================================
//
//      clist.hxx
//
//      Standard types, and some useful coding macros.
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
// Author(s):     nickg
// Contributors:  nickg
// Date:        2000-11-08
// Purpose:     Simple circular list implementation
// Description: A simple implementation of circular lists.
// Usage:       #include "cyg/infra/clist.hxx"
//              ...
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/infra/cyg_type.h>

// -------------------------------------------------------------------------
// Class and structure conversion macros.
// CYG_CLASSFROMFIELD translates a pointer to a field of a struct or
// class into a pointer to the class.
// CYG_OFFSETOFBASE yields the offset of a base class of a derived
// class.
// CYG_CLASSFROMBASE translates a pointer to a base class into a pointer
// to a selected derived class. The base class object _must_ be part of
// the specified derived class. This is essentially a poor mans version
// of the RTTI dynamic_cast operator.
// Caveat: These macros do not work for virtual base classes.

// Note: These definitions are exact duplicates of definitions in
// ktypes.h. If either definition is changed, the other should be too
// to avoid compiler messages.

#define CYG_CLASSFROMFIELD(_type_,_member_,_ptr_)\
    ((_type_ *)((char *)(_ptr_)-((char *)&(((_type_ *)0)->_member_))))

#define CYG_OFFSETOFBASE(_type_,_base_)\
    ((char *)((_base_ *)((_type_ *)4)) - (char *)4)

# define CYG_CLASSFROMBASE(_class_,_base_,_ptr_)\
    ((_class_ *)((char *)(_ptr_) - CYG_OFFSETOFBASE(_class_,_base_)))


// -------------------------------------------------------------------------
// Cyg_DNode class.
// This simply represents a double linked node that is intended to
// be a base member of the class that is being managed.

class Cyg_DNode
{
    Cyg_DNode   *next;
    Cyg_DNode   *prev;

public:

    Cyg_DNode()
    {
        // Initialize pointers to point here
        next = prev = this;
    };

    ~Cyg_DNode()
    {
        // If this node is still linked, unlink it.
        if( next != this )
            unlink();
    };

    // Accessor and test functions
    Cyg_DNode *get_next() { return next; };
    Cyg_DNode *get_prev() { return prev; };
    cyg_bool  in_list() { return next != this; };
    
    // Insert a node into the list before this one.
    void insert( Cyg_DNode *node )
    {
        node->next = this;
        node->prev = prev;
        prev->next = node;
        prev = node;
    };

    // Append a node after this one
    void append( Cyg_DNode *node )
    {
        node->prev = this;
        node->next = next;
        next->prev = node;
        next = node;
    };

    // Unlink this node from it's list. It is safe to apply this to an
    // already unlinked node.
    void unlink()
    {
        next->prev = prev;
        prev->next = next;
        next = prev = this;
    };
};

// -------------------------------------------------------------------------
// Cyg_CList class.

// This is a simple class that manages a circular list of DNodes. This
// object points to the head of the list and provides functions to
// manipulate the head and tail of the list.

class Cyg_CList
{
    Cyg_DNode   *head;                  // list head pointer

public:

    Cyg_CList()
    {
        head = NULL;
    };

    ~Cyg_CList()
    {
        while( head != NULL )
            rem_head();
    };

    // Accessor and test functions
    Cyg_DNode *get_head() { return head; };
    Cyg_DNode *get_tail() { return head?head->get_prev():NULL; };
    cyg_bool empty() { return head == NULL; };
    
    // Add a node at the head of the list
    void add_head( Cyg_DNode *node )
    {
        if( head == NULL )
            head = node;
        else
        {
            head->append( node );
            head = node;
        }
    };

    // Remove the node at the head of the list
    Cyg_DNode *rem_head()
    {
        Cyg_DNode *node = head;
        if( node != NULL )
        {
            // There is a node available
            Cyg_DNode *next = node->get_next();
            if( next == node )
            {
                // Only node on list
                head = NULL;
            }
            else
            {
                // remove head node and move head to next.
                node->unlink();
                head = next;
            }
        }
        return node;
    };


    // Add a node at the tail of the list
    void add_tail( Cyg_DNode *node )
    {
        if( head == NULL )
            head = node;
        else
            head->append( node );
    };

    // Remove the node at the tail of the list
    Cyg_DNode *rem_tail()
    {
        if( head == NULL )
            return NULL;

        Cyg_DNode *node = head->get_prev();

        if( node == head )
            head = NULL;
        else node->unlink();
        
        return node;
    };

    // General removal. Deals with what happend if this is only
    // object on list, or is the head.
    void remove( Cyg_DNode *node )
    {
        if( node == head )
            rem_head();
        else node->unlink();
    };
};

// -------------------------------------------------------------------------
// Cyg_CList_T
// Template class that allows us to make use of the CList class in a
// type-specific way.

template <class T> class Cyg_CList_T
    : public Cyg_CList
{
public:

    Cyg_CList_T<T>() {};
    ~Cyg_CList_T<T>() {};

    T *get_head() { return CYG_CLASSFROMBASE( T, Cyg_DNode, Cyg_CList::get_head() ); };
    T *get_tail() { return CYG_CLASSFROMBASE( T, Cyg_DNode, Cyg_CList::get_tail() ); };
    
    T *rem_head()
    {
        Cyg_DNode *node = Cyg_CList::rem_head();
        return CYG_CLASSFROMBASE( T, Cyg_DNode, node );
    };

    T *rem_tail()
    {
        Cyg_DNode *node = Cyg_CList::rem_tail();
        return CYG_CLASSFROMBASE( T, Cyg_DNode, node );
    };

    // The rest just default to the Cyg_CList class operations.
};

// -------------------------------------------------------------------------
// Cyg_DNode_T
// Template class that allows us to make use of the DNode class in a
// type-specific way.

template <class T> class Cyg_DNode_T
    : public Cyg_DNode
{
public:

    Cyg_DNode_T<T>() {};
    ~Cyg_DNode_T<T>() {};

    T *get_next() { return CYG_CLASSFROMBASE( T, Cyg_DNode, Cyg_DNode::get_next() ); };
    T *get_prev() { return CYG_CLASSFROMBASE( T, Cyg_DNode, Cyg_DNode::get_prev() ); };

    // The rest just default to the Cyg_DNode class operations.
};

// -------------------------------------------------------------------------
#endif // CYGONCE_INFRA_CLIST_HXX multiple inclusion protection
// EOF clist.hxx

