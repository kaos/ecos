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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	sdf
// Contact(s):	sdf
// Date:		1998/08/11
// Version:		0.01
// Purpose:	
// Description:	This is the implementation of the class which allows for spawning subprocesses
//
// Requires:	
// Provides:	
// See also:    
// Known bugs:	
// Usage:	
//
//####DESCRIPTIONEND####
//
//===========================================================================
#include "subprocess.h"

const UINT CSubprocess::PROCESS_KILL_EXIT_CODE=0xCCFFCCFF;

CSubprocess::CSubprocess(bool bVerbose):
  m_dwParentThreadId(0),
  m_pstrOutput(0),
  m_idProcess(0),
  m_bThreadRunning(false),
  m_bVerbose(bVerbose),
  m_dwExitCode(0xffffffff),
  m_bAutoDelete(false),
  m_hThread(0),
  m_pfnLogfunc(0),
  m_pLogparam(0),
  m_hwndParent(0)
{
}

// Run (non-blocking and discarding output)
/* static */ unsigned int CSubprocess::Run(LPCTSTR pszCmd,LPCTSTR pszDir)
{
  CSubprocess *psp=new CSubprocess(false);
  psp->m_bAutoDelete=true;
  String strOutput;
  return psp->Run(strOutput,pszCmd,pszDir);
}

unsigned int CSubprocess::Run(LogFunc *pfnLog,void * pLogparam, LPCTSTR pszCmd,LPCTSTR pszDir)
{
  unsigned int rc;
  if(m_bThreadRunning){
    rc=0;
  } else {
    m_pfnLogfunc=pfnLog;
    m_pLogparam=pLogparam;
    m_strCmd=pszCmd;
    m_strDir=pszDir;
    rc=CreateProcess();
    if(rc){
      ThreadFunc();
    }
  }
  return rc;
}

#ifdef _WIN32
// Non-blocking: WM_SUBPROCESS messages are posted to HWND supplied as argument
unsigned int CSubprocess::Run(HWND hwndParent,LPCTSTR pszCmd,LPCTSTR pszDir)
{
  unsigned int rc;
  if(m_bThreadRunning){
    rc=0;
  } else {
    m_hwndParent=hwndParent;
    m_strCmd=pszCmd;
    m_strDir=pszDir;
    rc=CreateProcess();
    if(rc){
      DWORD dwID;
      m_bThreadRunning=true;
      m_hThread=::CreateThread(NULL,0,ThreadFunc,this,0,&dwID);
    }
  }
  return rc;
}

// Non-blocking: WM_SUBPROCESS messages are posted to thread supplied as argument
unsigned int CSubprocess::Run(DWORD dwParentThreadId,LPCTSTR pszCmd,LPCTSTR pszDir)
{
  unsigned int rc;
  if(m_bThreadRunning){
    rc=0;
  } else {
    m_dwParentThreadId=dwParentThreadId;
    m_strCmd=pszCmd;
    m_strDir=pszDir;
    rc=CreateProcess();
    if(rc){
      DWORD dwID;
      m_bThreadRunning=true;
      m_hThread=::CreateThread(NULL,0,ThreadFunc,this,0,&dwID);
    }
  }
  return rc;
}
#endif

// Blocking: output is placed in string supplied as argument
unsigned int CSubprocess::Run(String &strOutput,LPCTSTR pszCmd,LPCTSTR pszDir)
{
  strOutput=_T("");
  m_strCmd=pszCmd;
  m_strDir=pszDir;
  m_pstrOutput=&strOutput;
  unsigned int rc=CreateProcess();
  if(rc){
    ThreadFunc();
  }
  return rc;
}

CSubprocess::~CSubprocess()
{
  Kill();
  
  if(m_hThread){ // running non-blocking and thread started (but possibly already finished)
    if(WAIT_TIMEOUT==WaitForSingleObject(m_hThread,1000)){
      // In general this is bad news, but we can't allow the thread to live on if
      // the class object is about to be deleted.
      if(m_bVerbose){
        Output(_T("*** CSubprocess: Forcibly terminating thread\n"));
      }
      ::TerminateThread(m_hThread,0);
    }
    ::CloseHandle(m_hThread);
  }
}

#ifdef _WIN32
unsigned int CSubprocess::CreateProcess()
{
  
  STARTUPINFO   si;                    // For CreateProcess call
  HANDLE        hrPipe,hwPipe,hwPipe2;
  // Create the anonymous pipe
  
  SECURITY_ATTRIBUTES saPipe;          // Security for anonymous pipe
  saPipe.nLength = sizeof(SECURITY_ATTRIBUTES);
  saPipe.lpSecurityDescriptor = NULL;
  saPipe.bInheritHandle = true;
  
  ::CreatePipe(&m_hrPipe,&hwPipe,&saPipe,80);
  
  // In most cases you can get away with using the same anonymous
  // pipe write handle for both the child's standard output and
  // standard error, but this may cause problems if the child app
  // explicitly closes one of its standard output or error handles. If
  // that happens, the anonymous pipe will close, since the child's
  // standard output and error handles are really the same handle. The
  // child won't be able to write to the other write handle since the
  // pipe is now gone, and parent reads from the pipe will return
  // ERROR_BROKEN_PIPE and child output will be lost. To solve this
  // problem, simply duplicate the write end of the pipe to create
  // another distinct, separate handle to the write end of the pipe.
  // One pipe write handle will serve as standard out, the other as
  // standard error. Now *both* write handles must be closed before the
  // write end of the pipe actually closes.
  
  ::DuplicateHandle(::GetCurrentProcess(),			// Source process
    hwPipe,		        // Handle to duplicate
    ::GetCurrentProcess(),   // Destination process
    &hwPipe2,	            // New handle, used as stderr by child 
    0,                     // New access flags - ignored since DUPLICATE_SAME_ACCESS
    true,                  // It's inheritable
    DUPLICATE_SAME_ACCESS);
  
  ::CreatePipe(&hrPipe,&m_hwPipe,&saPipe,80);
  
  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  
  si.hStdOutput = hwPipe;
  si.hStdError =  hwPipe2;
  si.hStdInput =  hrPipe;
  si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_SHOW;
  
  LPCTSTR pszDir=0;
  if(m_strDir.GetLength()){
    pszDir=m_strDir; // NULL = start in current
  }
  
  PROCESS_INFORMATION pi;
  unsigned int rc=(::CreateProcess(NULL,    // Application name
    m_strCmd.GetBuffer(m_strCmd.GetLength()),  // Full command line for child
    NULL,    // Process security descriptor
    NULL,    // Thread security descriptor
    true,    // Inherit handles? Also use if STARTF_USESTDHANDLES
    // Creation flags of 
    // CREATE_NEW_PROCESS_GROUP so we can use 
    // GenerateConsoleCtrlEvent to 
    // terminate the child
    DETACHED_PROCESS|CREATE_NEW_PROCESS_GROUP,
    NULL,    // Inherited environment address
    pszDir,&si,&pi) ? pi.dwProcessId : 0);
  
  m_strCmd.ReleaseBuffer();
  String strMsg;
  
  if(rc){
    m_idProcess=pi.dwProcessId;
    m_hProcess=pi.hProcess;
    strMsg.Format(_T("*** Process %d created \"%s\"\n"),m_idProcess,(LPCTSTR)m_strCmd);
    m_dwExitCode=STILL_ACTIVE;
  } else {
    strMsg.Format(_T("*** Failed to create process \"%s\"\n"),(LPCTSTR)m_strCmd);
    m_dwExitCode=GetLastError();
  }
  
  if(m_bVerbose){
    Output(strMsg);
  }
  
  ::CloseHandle(hrPipe);
  ::CloseHandle(hwPipe);
  ::CloseHandle(hwPipe2);
  ::CloseHandle(pi.hThread);
  
  return rc;
  
}
#else // UNIX
unsigned int CSubprocess::CreateProcess()
{
  int pipe_ends_w[2];
  if (pipe(pipe_ends_w) < 0 ) { 
    Log(_T("Failed to create pipe_ends_w - %s\n"),strerror(errno));
  } else {
    int pipe_ends_r[2];
    if (pipe(pipe_ends_r) < 0 ) { 
      Log(_T("Failed to create pipe_ends_r - %s\n"),strerror(errno));
    } else {
      int pid=fork();
      
      switch (pid) {
        // Fork failed
        case -1:
          Log(_T("Failed to create gdb process - %s\n"),strerror(errno));
          pid=0;
          break;
        case 0:
          // Process is created (we're the child)
          // No point in outputting except via output streams 
        
          // Input to child process
          if (dup2(pipe_ends_w[0], 0) < 0) {
            _ftprintf(stderr,(_T("dup2 error\n"));
            exit(1);
          }
        
          // Output from process
          if (dup2(pipe_ends_r[1], 2) < 0) {
            _ftprintf(stderr,(_T("dup2 error\n"));
            exit(2);
          }
          if (dup2(pipe_ends_r[1], 1) < 0) {
            _ftprintf(stderr,(_T("dup2 error\n"));
            exit(3);
          }
          setvbuf(stdout,0,_IONBF,0);
          setvbuf(stderr,0,_IONBF,0);
          if(m_bVerbose){
            strMsg.Format(_T("*** Process %d created \"%s\"\n"),pid,(LPCTSTR)m_strCmd);
          }

          StringArray ar;
          int argc=m_strCmd.Chop(ar,_TCHAR(' '),true);
          TCHAR **argv=new TCHAR *[1+argc];
          int i;
          for(i=0;i<argc;i++){
            argv[i]=ar[i];  
          }
          argv[i]=0;
          _texecvp(argv[0], argv);
          delete [] argv;
          _ftprintf(stderr,(_T("exec error - %s\n"),strerror(errno));
          
          exit(4);

        default:
          // Process is created (we're the parent)

          TRACE(_T("Forked to create gdb process %s - pid is <%d>\n"), pszCmdline, pid);
          if (fcntl(pipe_ends_r[0], F_SETFL, O_NONBLOCK) <0) {
            Log(_T("Couldn't set pipe non-blocking - %s\n"),strerror(errno));
          } else {
            m_rPipeHandle=(void *)pipe_ends_r[0];
            m_wPipeHandle=(void *)pipe_ends_w[1];
          }
          break;
      }
      close (pipe_ends_r[0]);
      close (pipe_ends_r[1]);
    }
    close (pipe_ends_w[0]);
    close (pipe_ends_w[1]);
  }
  return pid;
}
#endif

DWORD CSubprocess::ThreadFunc()

{
  
  DWORD dwAvail;
  
  while (::PeekNamedPipe(m_hrPipe, NULL, 0, 0, &dwAvail, NULL)){
    if(dwAvail){
      DWORD dwRead;
      char *buf=new char[dwAvail+1];
      if(!::ReadFile(m_hrPipe, buf, dwAvail, &dwRead, NULL)){
        delete [] buf;
        break;
      }
      buf[dwRead]='\0';
      Output(String::CStrToUnicodeStr(buf));
      delete [] buf;
    } else {
      DWORD dw;
      ::GetExitCodeProcess(m_hProcess, &dw);
      
      if(STILL_ACTIVE!=dw){
        break;
      }
      
      Sleep(250);
    }
  }
  
  ::GetExitCodeProcess(m_hProcess, &m_dwExitCode);
  String strMsg;
  if(m_bVerbose){
    if(PROCESS_KILL_EXIT_CODE==m_dwExitCode){
      strMsg.Format(_T("\n*** Process %d killed\n"),m_idProcess);
    } else {
      strMsg.Format(_T("\n*** Process %d terminated (rc=%d)\n"),m_idProcess,m_dwExitCode);
    }
    Output(strMsg);
  }

  if(m_dwParentThreadId || m_hwndParent){
    Post(0);
  }
  m_idProcess=0;

  ::CloseHandle(m_hrPipe);
  ::CloseHandle(m_hwPipe);
  ::CloseHandle(m_hProcess);
  m_bThreadRunning=false;
  DWORD rc=m_dwExitCode; // protect against auto-delete

  if(m_bAutoDelete){
    delete this;
  }

  return rc;

}

bool CSubprocess::Kill()
{
  if(0!=m_idProcess){
    //TRACE(_T("*** Killing process %d(ctrl/c)\n"),m_idProcess);
    //::GenerateConsoleCtrlEvent( CTRL_C_EVENT, m_idProcess);
    //if(WAIT_TIMEOUT==WaitForSingleObject(m_hThread,1000)){
      //TRACE(_T("*** Killing process %d(ctrl/break)\n"),m_idProcess);
      //::GenerateConsoleCtrlEvent( CTRL_BREAK_EVENT, m_idProcess);
      //if(WAIT_TIMEOUT==WaitForSingleObject(m_hThread,1000)){
        if(m_bVerbose){
          String strMsg;
          strMsg.Format(_T("*** Killing process %d (TerminateProcess)\n"),m_idProcess);
          Output(strMsg);
        }
        ::TerminateProcess(m_hProcess,PROCESS_KILL_EXIT_CODE);
      //}
    //}
  }
  return 0==m_idProcess;
}

void CSubprocess::Output (LPCTSTR psz)
{
  if(m_dwParentThreadId || m_hwndParent){
    TCHAR *pszCopy=new TCHAR[1+_tcslen(psz)];
    _tcscpy(pszCopy,psz);
    Post((LPARAM)pszCopy);
  } else if (m_pfnLogfunc) {
    m_pfnLogfunc(m_pLogparam,psz);
  } else {
    assert(NULL!=m_pstrOutput);
    (*m_pstrOutput)+=psz;
  }
}


void CSubprocess::Post(LPARAM lParam)
{
  if(m_hwndParent){
    ::PostMessage(m_hwndParent,WM_SUBPROCESS,(WPARAM)this,lParam);
  } else {
    assert(m_dwParentThreadId);
    ::PostThreadMessage(m_dwParentThreadId,WM_SUBPROCESS,(WPARAM)m_idProcess,lParam);
  }
}

void CSubprocess::CygKill()
{
  
  // Map win32 pids to cygwin format
  PtrArray map;
  
  int nKillCount;
  int nGid=-1;
  do {
    nKillCount=0;
    // Execute a "ps -l" to find out what's out there in Cygwin land
    CSubprocess sp;
    String strOutput;
    sp.Run(strOutput,_T("ps -l"));
    if(0==strOutput.GetLength()){
      if(m_bVerbose){
        Output(_T("*** Warning: could not run ps to effect a Cygkill()\n"));
      }
    } else {
      // Walk through the lines by line and extract what we need
      for(LPCTSTR psz=-1+(LPCTSTR)strOutput;psz;psz=_tcschr(psz,_TCHAR('\n'))){
        psz++; // move over the '\n'
        CygProcessInfo cpi;
        if(0==_tcsstr(psz,_T("ps.exe")) && 4==_stscanf(2+psz,_T("%d %d %d %d"),&cpi.nPid,&cpi.nPpid,&cpi.nPgid,&cpi.nWinpid) && cpi.nWinpid){
          for(unsigned int i=0;i<map.size();i++){
            if(((CygProcessInfo *)map[i])->nWinpid==cpi.nWinpid){
              break;
            }
          }

          if(i==map.size()){
            // No existing match - add:
            map.push_back(new CygProcessInfo(cpi));
            if((int)m_idProcess==cpi.nWinpid){
              nGid=cpi.nPgid;
            }
          }
        }
      }
      
      // Kill everything in the same group as our process
      if(-1!=nGid){
        for (unsigned int i=0;i<map.size();i++){
          CygProcessInfo *pcpi=(CygProcessInfo *)map[i];
          if(pcpi->nPgid==nGid){
            // Check that the process exists
            HANDLE hProcess=OpenProcess(PROCESS_TERMINATE,false,pcpi->nWinpid);
            if(hProcess){
              CloseHandle(hProcess);
              String strCmd,strOut;
              strCmd.Format(_T("kill %d"),pcpi->nPid);
              pcpi->nPgid=-pcpi->nPgid; // prevent this being done more than once
              CSubprocess::Run(strCmd);
              nKillCount++;
            }
          }
        }
      }

    }
  } while (nKillCount>0);

  // Now we've sent a kill to every process.  Go back and check they are really dead (use windows API this time)
  for(unsigned int i=0;i<map.size();i++){
    CygProcessInfo *pcpi=(CygProcessInfo *)map[i];
    if(abs(pcpi->nPgid)==nGid){
      HANDLE hProcess=OpenProcess(PROCESS_TERMINATE,false,pcpi->nWinpid);
      if(hProcess){
        ::TerminateProcess(hProcess,0x7fffffff);
        CloseHandle(hProcess);
      }
    }
  }

  for(i=0;i<map.size();i++){
    delete (CygProcessInfo *)map[i];
  }
}

void CSubprocess::Send(LPCTSTR str)
{
  int nLength=_tcslen(str);
  char *psz=new char[1+nLength];
#ifdef _UNICODE
  WideCharToMultiByte(CP_ACP, 0, str, -1, psz, 1+nLength, NULL, NULL);
#else
  strcpy(psz,str);
#endif
  
  DWORD dwWritten;
  ::WriteFile(m_hwPipe,psz,nLength,&dwWritten,0);
  delete [] psz;
}



void CSubprocess::CloseInput()
{
  ::CloseHandle(m_hwPipe);
}

