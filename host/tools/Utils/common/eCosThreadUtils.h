//####COPYRIGHTBEGIN####
//                                                                          
// ----------------------------------------------------------------------------
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.
//
// This program is part of the eCos host tools.
//
// This program is free software; you can redistribute it and/or modify it 
// under the terms of the GNU General Public License as published by the Free 
// Software Foundation; either version 2 of the License, or (at your option) 
// any later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT 
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for 
// more details.
// 
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// ----------------------------------------------------------------------------
//                                                                          
//####COPYRIGHTEND####
//=================================================================
//
//        eCosThreadUtils.h
//
//
//=================================================================
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     sdf
// Contributors:  sdf
// Date:          1999-04-01
// Description:   Threading-related utiltities 
// Usage:
//
//####DESCRIPTIONEND####

#ifndef _ECOSTHREADUTILS_H
#define _ECOSTHREADUTILS_H
#include "eCosStd.h"
#include "Collections.h"
#define ENTERCRITICAL {CeCosThreadUtils::CS c
#define LEAVECRITICAL }

#ifndef _WIN32 // UNIX
  #ifndef NO_THREADS
    #include <pthread.h>
  #endif 
#endif

class CeCosThreadUtils {
public:
#ifdef _WIN32
  static int CALLBACK FilterFunction(LPEXCEPTION_POINTERS p);
  typedef DWORD THREAD_ID;
#else // UNIX
  #ifndef NO_THREADS
    typedef int THREAD_ID;
  #else
    typedef pthread_t THREAD_ID;
  #endif
#endif
  static THREAD_ID GetThreadId();
  
  class CS{
    // p and q routines (process-local recursive mutexes)
#ifdef _WIN32
    static CRITICAL_SECTION cs;
    static bool bCSInitialized;
#else // UNIX
    #ifndef NO_THREADS
    // Static recursive mutex for unix critical section
    static pthread_mutex_t cs;
    #endif
#endif
    
  public:
    
    static int m_nCriticalSectionLock;
    static THREAD_ID nCSOwner;
    
    static int AtomicIncrement (int &n); // return old value
    static int AtomicDecrement (int &n); // return old value
    CS();
    virtual ~CS();
  };
  
  // Wait for this boolean to become true, subject to the given timeout
  static bool WaitFor (bool &b, int dTimeout=0x7fffffff);
  
  ///////////////////////////////////////////////////////////////////////////
  // Define the characteristics of a callback procedure:
  
  // A callback procedure:
  typedef void (CALLBACK CallbackProc)(void *);

  // Run a thread: pThreadFunc is the entry point (passed pParam).  No notification of completion.
  static bool RunThread(CallbackProc *pThreadFunc, void *pParam, LPCTSTR pszName=_T("")) { return RunThread(pThreadFunc,pParam,0,0,pszName); }
  // Run a thread, setting the bool on completion
  static bool RunThread(CallbackProc *pThreadFunc, void *pParam, bool *pb, LPCTSTR pszName=_T("")) { *pb=false; return RunThread(pThreadFunc,pParam,0,pb,pszName); }
  // Run a thread, calling the callback on completion
  static bool RunThread(CallbackProc *pThreadFunc, void *pParam, CallbackProc *pCompletionFunc, LPCTSTR pszName=_T("")) { return RunThread(pThreadFunc,pParam,pCompletionFunc,pParam,pszName); }
  
protected:

  // If m_pProc is non-zero then it is called
  // If m_pProc is 0 and m_pParam is non-0 then the callback sets the boolean whose address is held in m_pParam
  // If m_pProc is 0 and m_pParam is 0 then the callback is "blocking"
  //bool IsBlocking() const { return 0==m_pProc && 0==m_pParam; }
  
  // Run a thread: arbitrary callbcak
  static bool RunThread(CallbackProc *pThreadFunc, void *pParam, CallbackProc *pCompletionFunc, void *pCompletionParam, LPCTSTR pszName);
  
  struct ThreadInfo {
    CallbackProc *pThreadFunc;
    void         *pThreadParam;
    CallbackProc *pCompletionFunc;  // Call this function
    void         *pCompletionParam; // With this parameter
    String       strName;
    ThreadInfo (CallbackProc *_pThreadFunc,void *_pThreadParam,CallbackProc *_pCompletionFunc,void  *_pCompletionParam,LPCTSTR pszName) :
      pThreadFunc(_pThreadFunc),
      pThreadParam(_pThreadParam),
      pCompletionFunc(_pCompletionFunc),
      pCompletionParam(_pCompletionParam),
      strName(pszName){}
  };

  // Result type of the thread function
  #ifdef _WIN32
    typedef unsigned long THREADFUNC; 
  #else // UNIX
    typedef void * THREADFUNC;
  #endif
  static THREADFUNC CALLBACK SThreadFunc (void *pParam);
    
};

#endif
