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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	sdf
// Contact(s):	sdf
// Date:		1998/08/11
// Version:		0.01
// Purpose:	
// Description:	Interface of the subprocess clas
// Requires:	
// Provides:	
// See also:    
// Known bugs:	
// Usage:	
//
//####DESCRIPTIONEND####
//
//===========================================================================
#ifndef _SUBPROCESS_H
#define _SUBPROCESS_H

#define WM_SUBPROCESS (WM_USER+42) 

#include "eCosStd.h"
#include "Collections.h"

class CSubprocess {
	// Class to spawn subprocesses.  

public:

  // ctors and dtors.  Nothing exciting here.
  CSubprocess(bool bVerbose=false);
	virtual ~CSubprocess();
 
  // Get the process exit code.  This can be:
  //   exit code of process (if terminated)
  //   0xffffffff (if process not yet run)
  //   GetLastError result (if process could not be run)
  DWORD GetExitCode() { return m_dwExitCode; }
  
  // Close the process's stdin
  void CloseInput();
  
  // Various forms of the Run function.  In each case the result is the process ID created (0 if failure)

  // Run a process (blocking)
  unsigned int Run(String &strOutput,LPCTSTR pszCmd,LPCTSTR pszDir=_T(""));
#ifdef _WIN32
  // Run a process (non-blocking): caller gets WM_SUBPROCESS messages:
  // wParam is the this pointer, lParam is text string [for process output]
  // which callee deletes, or 0 [for process completion]
  unsigned int Run(HWND  hwndParent,      LPCTSTR pszCmd,LPCTSTR pszDir=_T("")); // post messages to given window
  unsigned int Run(DWORD dwParentThreadId,LPCTSTR pszCmd,LPCTSTR pszDir=_T("")); // post messages to given thread
#endif
  
  // Run (non-blocking and discarding output)
  static unsigned int Run(LPCTSTR pszCmd,LPCTSTR pszDir=_T(""));

  // Run (blocking and sending output to callback)
  unsigned int Run(LogFunc *pfnLog,void * pLogparam,LPCTSTR pszCmd,LPCTSTR pszDir=_T(""));
  
  // Kill the process:
  bool Kill();
  
  // Kill the process and its children the Cygwin way (requires ps and kill)
  void CygKill();
  
  // Send some input to the process:
  void Send (LPCTSTR psz); 
  
protected:
  LogFunc *m_pfnLogfunc;
  void *m_pLogparam;
	bool m_bAutoDelete;
  DWORD m_dwExitCode;
  static const UINT PROCESS_KILL_EXIT_CODE;
	bool m_bVerbose;
	bool m_bThreadRunning;
	HANDLE m_hrPipe;
	HANDLE m_hwPipe;
	unsigned int CreateProcess();

  struct CygProcessInfo {
    int nPid;
    int nPpid;
    int nPgid;
    int nWinpid;
  };

	String * m_pstrOutput;
	HANDLE m_hThread;
	void Post (LPARAM lParam);
  DWORD m_idProcess;

  HANDLE m_hProcess;
  void Output(LPCTSTR psz);
  String m_strCmd,m_strDir;
#ifdef _WIN32
  HWND  m_hwndParent;
#endif
  DWORD m_dwParentThreadId;
  static DWORD WINAPI ThreadFunc(LPVOID pParam) { return ((CSubprocess *)pParam)->ThreadFunc(); }
  DWORD ThreadFunc();
};

#endif
