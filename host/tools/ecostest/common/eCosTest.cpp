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
//        eCosTest.cpp
//
//        Test class
//
//=================================================================
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     sdf
// Contributors:  sdf
// Date:          1999-04-01
// Description:   This class abstracts a test for use in the testing infrastructure
// Usage:
//
//####DESCRIPTIONEND####
///////////////////////////////////////////////////////////////////////////////
#include "eCosStd.h"
#include "eCosTest.h"
#include "eCosTrace.h"
#include "TestResource.h"
#include "eCosTestUtils.h"
#include "eCosTestSocket.h"
#include "eCosTestSerial.h"
#include "eCosTestSerialFilter.h"
#include "eCosTestDownloadFilter.h"

#define WF(n) (n+50)/1000,((n+50)%1000)/100     // Present n as whole and fractional part.  Round to nearest least significant digit
#define WFS   _T("%u.%u")                           // The format string to output the above

static int nAuxPort; //hack
static int nAuxListenSock; // hack

#ifdef _WIN32
  #include "Subprocess.h"
  #ifdef _DEBUG
    #define CloseHandle(x) try { ::CloseHandle(x); } catch(...) { TRACE(_T("!!! Exception caught closing handle %08x\n"),x); }
  #endif
#endif

std::vector<CeCosTest::TargetInfo> CeCosTest::arTargetInfo;

LPCTSTR  const CeCosTest::arResultImage[1+CeCosTest::StatusTypeMax]=
{_T("NotStarted"), _T("NoResult"), _T("Inapplicable"), _T("Pass"), _T("DTimeout"), _T("Timeout"), _T("Cancelled"), _T("Fail"), _T("AssertFail"), _T("Unknown")};

CeCosTest *CeCosTest::pFirstInstance=0;
int CeCosTest::InstanceCount=0;

LPCTSTR  const CeCosTest::arServerStatusImage[1+CeCosTest::ServerStatusMax]={
  _T("Busy"), _T("Ready"), _T("Can't run"), _T("Connection failed"), _T("Locked"), _T("Bad server status")};
LPCTSTR  CeCosTest::ExecutionParameters::arRequestImage [1+ExecutionParameters::RequestTypeMax]={
  _T("Run"), _T("Query"), _T("Lock"), _T("Unlock"), _T("Stop"), _T("Bad request") };
  
const CeCosTest::TargetInfo CeCosTest::tDefault(_T("Unknown"),_T(""),-1);

// Do not use spaces in image strings for consideration of _stscanf
// One day this can be loadable from some external resource.
int CeCosTest::InitTargetInfo(LPCTSTR pszFilename)
{
  TRACE(_T("InitTargetInfo %s\n"),pszFilename);
  FILE *f=_tfopen(pszFilename,_T("rt"));
  if(f){
    enum {BUFSIZE=512};
    int nLine=0;
    TCHAR buf[BUFSIZE];
    while(_fgetts(buf,sizeof(buf)-1,f)){
      nLine++;
      StringArray ar;
      String(buf).Chop(ar);
      switch(ar.size()){
        // <target> <prefix> <nType> [<gdbcmds>]
        case 0:
          continue;
        case 3:
          arTargetInfo.push_back(TargetInfo(ar[0],ar[1],ar[2],0));
          break;
        case 1:
        case 2:
          ERROR(_T("Illegal configuration at line %d\n"),nLine);
          continue;
        default:
          arTargetInfo.push_back(TargetInfo(ar[0],ar[1],ar[2],ar[3]));
          break;
      }
    }
    fclose(f);
  }
  return arTargetInfo.size();
}

int CeCosTest::InitTargetInfoReg(LPCTSTR pszRegKey)
{
  
#ifdef _WIN32
  HKEY hKey;
  bool rc=ERROR_SUCCESS==RegOpenKeyEx (HKEY_LOCAL_MACHINE, pszRegKey, 0L, KEY_READ, &hKey);
  DWORD dwSubKeys=0;
  if(rc){
    // Found the given key.
    // Subkeys' names are the target image names:
    // Subkeys's values are:
    //      Prefix  String
    //      Type    String 
    //      GdbCmd  String [optional]
    FILETIME ftLastWriteTime;
    DWORD dwMaxSubKeyLen;
    if(ERROR_SUCCESS==RegQueryInfoKey(hKey,NULL,NULL,NULL,&dwSubKeys,&dwMaxSubKeyLen,NULL,NULL,NULL,NULL,NULL,NULL)){
      TCHAR *szName=new TCHAR[1+dwMaxSubKeyLen];
      DWORD dwSizeName=dwMaxSubKeyLen;
      for(DWORD dwIndex=0;ERROR_SUCCESS==RegEnumKeyEx(hKey, dwIndex, szName, &dwSizeName, NULL, NULL, NULL, &ftLastWriteTime); dwIndex++){
        HKEY hKey2;
        if(ERROR_SUCCESS!=RegOpenKeyEx (hKey, szName, 0L, KEY_READ, &hKey2)){
          ERROR(_T("Failed to open %s\\%s\n"),pszRegKey,szName);
          rc=false;
        } else {
          DWORD dwMaxValueLen;
          if(ERROR_SUCCESS==RegQueryInfoKey(hKey2,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,&dwMaxValueLen,NULL,NULL)){
            DWORD dwSizePrefix=dwMaxValueLen;
            DWORD dwSizeGdbCmd=dwMaxValueLen;
            DWORD dwSizeHwType=dwMaxValueLen;
            TCHAR *szPrefix=new TCHAR[1+dwMaxValueLen];
            TCHAR *szGdbCmd=new TCHAR[1+dwMaxValueLen];
            TCHAR *szHwType=new TCHAR[1+dwMaxValueLen];
            if(ERROR_SUCCESS==RegQueryValueEx(hKey2, _T("Prefix"),NULL, NULL,(LPBYTE)szPrefix,&dwSizePrefix)){
              arTargetInfo.push_back(TargetInfo(szName,szPrefix,
                (ERROR_SUCCESS==RegQueryValueEx(hKey2,_T("HwType"),NULL, NULL,(LPBYTE)szHwType,&dwSizeHwType))?szHwType:_T(""),
                (ERROR_SUCCESS==RegQueryValueEx(hKey2,_T("GdbCmd"),NULL, NULL,(LPBYTE)szGdbCmd,&dwSizeGdbCmd))?szGdbCmd:0
                ));
            }
            delete [] szPrefix;
            delete [] szGdbCmd;
            delete [] szHwType;
          }
          RegCloseKey(hKey2);
        }
        dwSizeName=dwMaxSubKeyLen;
      }
      delete [] szName;
    }
    RegCloseKey(hKey);
  }
  return rc?dwSubKeys:0;
#else // UNIX
  return 0;
#endif
}

bool CeCosTest::Init()
{
  TRACE(_T("CeCosTest::Init\n"));
  srand( (unsigned)time( NULL ) );
  
#ifdef _WIN32
  WSADATA wsaData;
  WORD wVersionRequested = MAKEWORD( 2, 0 ); 
  WSAStartup( wVersionRequested, &wsaData );

  // get target info from the registry
  String strPlatformsKey = _T("Software\\Red Hat\\eCos\\");
  strPlatformsKey += GetGreatestSubkey (_T("Software\\Red Hat\\eCos"));
  strPlatformsKey += _T("\\Platforms");
  InitTargetInfoReg (strPlatformsKey);
  
  // add target info from .eCosrc
  LPCTSTR psz=_tgetenv(_T("HOMEDRIVE"));
  if(psz){
    String strFile(psz);
    psz=_tgetenv(_T("HOMEPATH"));
    if(psz){
      strFile+=psz;
    }
    if(_TCHAR('\\')!=strFile[strFile.GetLength()-1]){
      strFile+=_TCHAR('\\');
    }
    strFile+=_T(".eCosrc");
    InitTargetInfo(strFile);
  }
#else // UNIX
  sigset_t mask;
  
  // Clean out all the signals
  sigemptyset(&mask);
  
  // Add our sigpipe
  sigaddset(&mask, SIGPIPE);
  
  sigprocmask(SIG_SETMASK, &mask, NULL);

  // _WIN32 not defined so get target info from .eCosrc
  LPCTSTR psz=_tgetenv(_T("HOME"));
  if(psz){
    String strFile(psz);
    strFile+=_T("/.eCosrc");
    InitTargetInfo(strFile);
  }
#endif
  if(0==TargetTypeMax()){
    ERROR(_T("Failed to initialize any targets\n"));
  }
  return true;
}

void CeCosTest::Term()
{
  
  TRACE(_T("CeCosTest::Term\n"));

#ifdef _WIN32
  WSACleanup();
#endif
};

bool CeCosTest::SaveTargetInfo()
{
#ifdef _WIN32
  // save target info to the registry
  String strPlatformsKey = _T("Software\\Red Hat\\eCos\\");
  strPlatformsKey += GetGreatestSubkey (_T("Software\\Red Hat\\eCos"));
  strPlatformsKey += _T("\\Platforms");
  return SaveTargetInfoReg (strPlatformsKey);
#else // UNIX
  // get target info from .eCosrc
  String strFile=_tgetenv(_T("HOME"));
  if(strFile.GetLength()>0){
    strFile+=_T("/.eCosrc");
    return SaveTargetInfo(strFile);
  } else {
    return false;
  }
#endif
}

bool CeCosTest::SaveTargetInfo(LPCTSTR pszFilename)
{
  return false; //FIXME
}

#ifdef _WIN32
bool CeCosTest::SaveTargetInfoReg(LPCTSTR pszRegKey)
{
  CProperties::CreateKey(pszRegKey,HKEY_LOCAL_MACHINE);
  HKEY hKey;
  bool rc=ERROR_SUCCESS==RegOpenKeyEx (HKEY_LOCAL_MACHINE, pszRegKey, 0L, KEY_ALL_ACCESS, &hKey);
  if(rc){
    for(int i=0;i<(signed)arTargetInfo.size();i++){
      HKEY hKey2;
      DWORD dwDisp;
      const TargetInfo &ti=arTargetInfo[i];
      rc&=(ERROR_SUCCESS==RegCreateKeyEx(hKey,ti.Image(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey2, &dwDisp));
      if(rc){
        LPCTSTR pszPrefix=ti.Prefix();
        LPCTSTR pszGdb   =ti.GdbCmd();
        LPCTSTR pszHwType=CeCosTest::TargetInfo::arHwTypeImage[ti.Type()];
        rc&=(ERROR_SUCCESS==RegSetValueEx(hKey2,_T("Prefix"),0,REG_SZ,   (CONST BYTE *)pszPrefix,(1+_tcslen(pszPrefix))*sizeof TCHAR)) &&
            (ERROR_SUCCESS==RegSetValueEx(hKey2,_T("HwType"),0,REG_SZ,   (CONST BYTE *)pszHwType,(1+_tcslen(pszHwType))*sizeof TCHAR)) &&
            (ERROR_SUCCESS==RegSetValueEx(hKey2,_T("GdbCmd"),0,REG_SZ,   (CONST BYTE *)pszGdb,(1+_tcslen(pszGdb))*sizeof TCHAR));
      }
      RegCloseKey(hKey2);
    }
    RegCloseKey(hKey);
  }
  return rc;
}
#endif

static bool CALLBACK IsCancelled(void *pThis)
{
  return CeCosTest::Cancelled==((CeCosTest *)pThis)->Status();
}

// Ctors and dtors:
CeCosTest::CeCosTest(const ExecutionParameters &e, LPCTSTR pszExecutable,LPCTSTR pszTitle):
m_nRunCount(0),
  m_nStrippedSize(0),
  m_nFileSize(0),
  m_bDownloading(false),
  m_pSock(0),
  m_ep(e),
  m_strTitle(pszTitle),
  m_Status(NotStarted),
  m_nDownloadTime(0),
  m_nTotalTime(0),
  m_nMaxInactiveTime(0),
  m_pPort(0)
{
  SetExecutable (pszExecutable);
  
  TRACE(_T("%%%% Create test instance %08x count:=%d\n"),this,InstanceCount+1);  
  
  GetPath(m_strPath);
  
  ENTERCRITICAL;
  InstanceCount++;
  m_pNextInstance=pFirstInstance;
  if(m_pNextInstance){
    m_pNextInstance->m_pPrevInstance=this;
  } 
  m_pPrevInstance=0;
  pFirstInstance=this;
  LEAVECRITICAL;
  
}

CeCosTest::~CeCosTest()
{
  TRACE(_T("%%%% Delete test instance %08x\n"),this);
  Cancel();
  CloseSocket();
  if(m_pPort){
    m_pPort->Release();
    //delete m_pPort;
    //m_pPort=0;
  }
  
  VTRACE(_T("~CeCosTest(): EnterCritical and decrease instance count\n"));
  ENTERCRITICAL;
  InstanceCount--;
  TRACE(_T("%%%% Destroy instance.  Instance count:=%d\n"),InstanceCount);
  if(pFirstInstance==this){
    pFirstInstance=m_pNextInstance;
  }
  if(m_pPrevInstance){
    m_pPrevInstance->m_pNextInstance=m_pNextInstance;
  }
  if(m_pNextInstance){
    m_pNextInstance->m_pPrevInstance=m_pPrevInstance;
  }
  LEAVECRITICAL;
}

bool CeCosTest::RunRemote (LPCTSTR pszRemoteHostPort)
{
  bool rc=false;
  m_nRunCount++;
  TRACE(_T("RunRemote\n"));
  m_strExecutionHostPort=pszRemoteHostPort;
  SetPath(m_strPath);
  m_Status=NotStarted;
  
  VTRACE(_T("RemoteThreadFunc()\n"));
  
  // Find a server.
  ConnectForExecution();
  if(Cancelled!=Status()){       
    if(ServerSideGdb()){
      // The executable is transmitted to the server for execution.
      // Send file size
      if(m_pSock->sendInteger(m_nFileSize,_T("file size"))&&m_nFileSize>0){
        int nBufSize=MIN(10000,m_nFileSize);
        Buffer b(nBufSize);
        TRACE(_T("Sending [%d bytes]\n"), m_nFileSize);
        int nToSend=m_nFileSize;
        FILE *f1=_tfopen(m_strExecutable,_T("rb"));
        if(0==f1){
          Log(_T("Failed to open %s - %s\n"),(LPCTSTR )m_strExecutable,strerror(errno));
        } else {
          while (nToSend>0){
            int nRead=fread( b.Data(), 1, nBufSize, f1);
            if(nRead<=0){
              Log(_T("Failure reading %s - %s\n"),(LPCTSTR )m_strExecutable,strerror(errno));
              break;
            }
            if(!send( b.Data(), nRead, _T("executable"))){
              Log(_T("Failure sending %s - %s\n"),(LPCTSTR )m_strExecutable,(LPCTSTR )m_pSock->SocketErrString());
              break;
            }
            nToSend-=nRead;
          }
          fclose(f1);
          f1=0;
          if(nToSend>0){
            TRACE(_T("done [%d bytes sent]\n"),m_nFileSize-nToSend);
            Log(_T("Failed to transmit %s - %d/%d bytes sent\n"),(LPCTSTR )m_strExecutable,m_nFileSize-nToSend,m_nFileSize);
          } else {
            TRACE(_T("done\n"));
            rc=true;
          }
        }
        if(!recvResult(9*1000*60)){ // nine minutes
          Log(_T("Failed to receive result from remote server\n"));
          rc=false;
        }
        m_pSock->sendInteger(1); // send an ack [n'importe quoi]
        CloseSocket();
      }
    } else {
      // The server sets up a connection between port and tcp/ip socket, and gdb is run locally
      String strHostPort,strOutput;
      // Big timeout here because we have to wait for the target to be reset
      // We do this:
      // do {
      //     target ready indicator (0==fail, 1==ready, 2==fail and will retry)
      //     any output so far
      // } while (2==target ready indicator)
      // read host:port
      if(GetTargetReady(strHostPort)){
        ENTERCRITICAL;
        m_pPort=new CTestResource(Target(), strHostPort, 0);
        m_pPort->Use();
        LEAVECRITICAL;
        RunLocal();
        delete m_pPort;
        m_pPort=0;
        m_pSock->sendInteger(123,_T("Terminating ack"));
        m_pSock->Close();
        rc=true;
      }
    }
  }
  TRACE(_T("RemoteThreadFunc(): Exiting\n"));
  return rc;
}

// Run the test locally
bool CeCosTest::RunLocal()
{
  m_nRunCount++;
  bool rc=false;
  if(0==CTestResource::Count(m_ep)){
    Log(_T("Cannot run a %s test\n"),m_ep.Target());
  } else {
    
    TRACE(_T("Run %s, timeouts: active=%d elapsed=%d\n"),(LPCTSTR )m_strExecutable, ActiveTimeout(), DownloadTimeout());
    m_Status=NotStarted;
    m_tPrevSample=Now();
    m_tGdbCpuTime=0;

    GetPath(m_strPath);
    
    TRACE(_T("LocalThreadFunc - target=%s\n"),Target());
    // Acquire a port (our caller may have done this for us)
    VTRACE(_T("LocalThreadFunc():Tring to acquire a port\n"));
    if(0==m_pPort){
      for(;;){
        m_pPort=CTestResource::GetResource(m_ep);
        if(m_pPort||Cancelled==Status()){
          break;
        }
        Sleep(2000);
        TRACE(_T("Waiting for a port\n"));
      }
    }
    VTRACE(_T("\nPort acquired!\n"));
    
    if(Cancelled!=Status()){
      // This means we have acquired a local port 
      bool bTargetReady=false;
      if(!m_pPort->HasReset()){
        bTargetReady=true;
      } else {
        bTargetReady=(CResetAttributes::RESET_OK==Reset(false));
      }
      // we may proceed to execute the test
      if(bTargetReady){
        SetStatus(NotStarted);
        if(_TCHAR('\0')!=*(m_pPort->Serial())){
          // No elapsed timeout given - calculate from knowledge of executable size and baud rate
          // 10 baud ~= 1 byte/sec, but we halve this to account for download in hex :-(
          // This means that a 200k executable is given ~100 seconds
          // In any case the whole thing is an overestimate [we should use the stripped size]
          // We use a minimum of 30 seconds and add 50% for safety
          int nSpeed=((0==m_pPort->Baud()?9600:m_pPort->Baud())/10)/2; // Hex
          nSpeed/=2; // Safety
          if(NOTIMEOUT==m_ep.DownloadTimeout()){
            m_ep.SetDownloadTimeout (1000*MAX(60,(m_nStrippedSize/nSpeed)));
          }
          TRACE(_T("Timeout=%d\n"),m_ep.DownloadTimeout());
        }
        if(NOTIMEOUT==m_ep.ActiveTimeout()){
          m_ep.SetActiveTimeout(1000*(_TCHAR('\0')==*(m_pPort->Serial())?300:30));
        }

        {
          // Construct commands for gdb
          const TargetInfo &t=Target(Target());
          TargetInfo::HwType hwt=t.Type();
          StringArray arstrGdbCmds;
          LPCTSTR pszPrompt;
          String strGdb;
            // running using gdb
            pszPrompt=_T("(gdb) ");
            strGdb.Format(_T("%s-gdb -nw %s"),t.Prefix(),(LPCTSTR)CygPath(m_strExecutable));
            // Tell gdb its paper size :-)
            arstrGdbCmds.push_back(_T("set height 0"));
            arstrGdbCmds.push_back(_T("set remotedebug 0"));
            
            if(_TCHAR('\0')!=*t.GdbCmd()){
              arstrGdbCmds.push_back(t.GdbCmd());
            }
            
            String str;
            if(_TCHAR('\0')!=*(m_pPort->Serial())){
              // Talking remotely
              if(CeCosTestSocket::IsLegalHostPort(m_pPort->Serial())){
                // Talking to a tcp/ip socket
                arstrGdbCmds.push_back(_T("set watchdog 0"));
              } else {
                // Talking to a serial port
                str.Format(_T("set remotebaud %d"),m_pPort->Baud());
                arstrGdbCmds.push_back(str);
              }
              str.Format(_T("target remote %s"),m_pPort->Serial());
#ifdef _WIN32
              // Serial names on windows must be in l.c. (gdb bug)
              for(TCHAR *c=str.GetBuffer();*c;c++){
                if(isalpha(*c)){
                  *c=(TCHAR)_totlower(*c);
                }
              }
              str.ReleaseBuffer();
#endif
              arstrGdbCmds.push_back(str);
            }
            
            if(TargetInfo::SYNTHETIC!=hwt){
              arstrGdbCmds.push_back(_T("load"));
            }
            
            arstrGdbCmds.push_back(_T("break cyg_test_exit"));
            arstrGdbCmds.push_back(_T("break cyg_assert_fail"));
            if(/*start hack*/BreakpointsOperational()/*end hack*/){
              arstrGdbCmds.push_back(_T("break cyg_test_init"));
            }
            
            switch(hwt){
            case TargetInfo::SYNTHETIC:
            case TargetInfo::SIM:
              arstrGdbCmds.push_back(_T("run"));
              break;
            case TargetInfo::HARDWARE:
            case TargetInfo::HARDWARE_NO_BP:
            case TargetInfo::REMOTE_SIM:
              arstrGdbCmds.push_back(_T("cont")); // run the program
              break;
            default:
              assert(false);
            }
            
            if(BreakpointsOperational()){
              str.Format(_T("set cyg_test_is_simulator=%d"),hwt);
              arstrGdbCmds.push_back(str);
              arstrGdbCmds.push_back(_T("cont")); // continue from cyg_test_init breakpoint
            }
            
            // run/cont command must be the last (logic in DriveGdb)
            
          TRACE(_T("Calling RunGdb\n"));
          RunGdb(strGdb,pszPrompt,arstrGdbCmds);
          rc=true;          
        }
      }
    }
    TRACE(_T("LocalThreadFunc - releasing resource\n"));
    if(m_pPort){
      m_pPort->Release();
      m_pPort=0;
    }
    TRACE(_T("LocalThreadFunc - exiting\n"));
  }

  return rc;
}

void CeCosTest::Cancel ()
{
  SetStatus(Cancelled);
}

CeCosTest::ServerStatus CeCosTest::Connect (String strHost,int port, CeCosTestSocket *&pSock, const ExecutionParameters &e,String &strInfo,Duration dTimeout)
{
  // Find out whether this host is receptive
  ServerStatus s=CONNECTION_FAILED;
  pSock=new CeCosTestSocket(strHost,port,dTimeout);
  int nStatus;    
  if(pSock->Ok() &&
    pSock->sendString(e.Image(), _T("execution parameters")) &&
    pSock->recvInteger(nStatus,_T("ready status")) &&
    pSock->recvString(strInfo)){
    s=(ServerStatus)MIN(nStatus,ServerStatusMax);
  }
  if(SERVER_READY!=s || ExecutionParameters::RUN!=e.Request()){
    delete pSock;
    pSock=0;
  }
  return s;
}

// Initiate a connection to hostName:nPort and acquire the ready status [retry until this is achieved]
// The socket (m_pSock) is left open.
// This function is either called with m_strExecutionHostPort already set to a desired server
// or else m_strExecutionHostPort empty (in which case the server is / dynamically)

void CeCosTest::ConnectForExecution ()
{
  bool bSchedule=(0==m_strExecutionHostPort.GetLength());
  Duration nDelay=2000;
  
  m_pSock=0;
  
  bool *arbHostTried=0;
  
  while(Cancelled!=Status()){
    StringArray arstrHostPort,arstrTries;
    int nChoices;
    
    if(bSchedule){
      if(!CTestResource::GetMatches(m_ep,arstrHostPort)){
        Log(_T("Could not establish matches\n"));
        continue;
      }
      nChoices=arstrHostPort.size();
      if(nChoices>0){
        TRACE(_T("ConnectForExecution: choices are:\n"));
        for(int i=0;i<nChoices;i++){
          TRACE(_T("\t%s\n"),(LPCTSTR )arstrHostPort[i]);
        }
      }
    } else {
      // Server has already been picked by caller
      nChoices=1;
      String str;
      arstrHostPort.push_back(m_strExecutionHostPort);
    }
    
    if(nChoices>0){
      delete [] arbHostTried;
      arbHostTried=new bool[nChoices];
      for(int i=0;i<nChoices;i++){
        arbHostTried[i]=false;
      }
      
      // Loop around the choices
      for(int nUntried=nChoices;nUntried>0;nUntried--) {
        // Select one we haven't tried already:
        int nChoice;        
        do {
          nChoice=rand() % nChoices;
        } while (arbHostTried[nChoice]);
        
        m_strExecutionHostPort=arstrHostPort[nChoice];
        
        TRACE(_T("ConnectForExecution: chosen %s\n"),(LPCTSTR )m_strExecutionHostPort);
        String strHost;
        int nPort;
        CeCosTestSocket::ParseHostPort(m_strExecutionHostPort, strHost, nPort);
        if(nPort){
          // If we're using the resource server we had better check that the host
          // we are about to lock has not been resource-locked (the other match checks
          // will of course always succeed)
          String strInfo;
          ServerStatus s=bSchedule && !CTestResource::Matches(strHost, nPort,m_ep)?
SERVER_LOCKED:  
          Connect(strHost,nPort,m_pSock,m_ep,strInfo);
          arbHostTried[nChoice]=true;        
          TRACE(_T("Connect: %s says %s %s\n"),(LPCTSTR )m_strExecutionHostPort,Image(s),(LPCTSTR )strInfo);
          CTestResource *pResource=CTestResource::Lookup(strHost,nPort);
          if(pResource){
            String str;
            str.Format(_T("%s %s %s"),(LPCTSTR )pResource->Output(),(LPCTSTR )strInfo,Image(s));
            arstrTries.push_back(str);
          }
          if(SERVER_READY==s){
            // So that's ok then.  We're outta here.
            Interactive(_T("Connected to %s\n"),(LPCTSTR )m_strExecutionHostPort);
            goto Done;
          } else {
            delete m_pSock;
            m_pSock=0;
          }
        }
      } 
    }
    
    Interactive(_T("Warning - could not connect to any test servers:\n"));
    if(arstrTries.size()>0){
      for(unsigned int i=0;i<arstrTries.size();i++){
        Interactive(_T("    %s\n"),(LPCTSTR )arstrTries[i]);
      }
    } else {
      Interactive(_T("No servers available to execute %s test:\n"),m_ep.Target());
      ENTERCRITICAL;
      for(CTestResource *pResource=CTestResource::First();pResource;pResource=pResource->Next()){
        Interactive(_T("    %s\n"),(LPCTSTR )pResource->Output());
      }
      LEAVECRITICAL;
    }
    Interactive(_T("Retry in %d seconds...\n"),nDelay/1000);
    
    // We have tried all possibilities - sleep before retrying
    Sleep(nDelay);
    
    if(Cancelled==m_Status){
      TRACE(_T("ConnectForExecution : cancelled\n"));
      goto Done;
    }
    if(nDelay<20*1000){
      nDelay+=rand() % 500;
    }
  }
Done:    
  delete [] arbHostTried;
}

void CeCosTest::LogResult()
{
  CeCosTrace::Out(ResultString());
  CeCosTrace::Out(_T("\n")  );
  CeCosTrace::Trace(_T("%s\n"),ResultString());
}

void CeCosTest::SetStatus (StatusType status)
{
  ENTERCRITICAL;
  if((int)status>(int)m_Status){
    TRACE(_T("Status <- %s\n"),Image(status));
    m_Status=status;
  }
  LEAVECRITICAL;
}

bool CeCosTest::WaitForAllInstances(int nPoll,Duration nTimeout)
{
  Time t0=Now();
  while(InstanceCount>0){
    Sleep(nPoll);
    if(NOTIMEOUT!=nTimeout && Now()-t0>nTimeout){
      return false;
    }
  }
  return true;
}

void CeCosTest::DeleteAllInstances()
{
  while(pFirstInstance){
    delete pFirstInstance;
  }
}

void CeCosTest::CancelAllInstances()
{
  ENTERCRITICAL;
  for(CeCosTest *pTest=pFirstInstance;pTest;pTest=pTest->m_pNextInstance){
    pTest->Cancel();
  }
  LEAVECRITICAL;
}

// The same format is used for _stscanf as for Format (which is like printf), so restrict to the format specifiers
// the former is happy with.  In particular, do not use %-3s etc...

LPCTSTR CeCosTest::pszFormat=
// 1999-01-15 17:24:36 Fireblade:5002 MN10300 sin.exe 219k/134k Pass sin download=106.3/117.0 Total=107.6 Max inactive=1.0/300.0    
_T("%04d-%02d-%02d %02d:%02d:%02d ")                   // Time
_T("%15s ")                                            // Execution host:port
_T("%16s ")                                             // Target
_T("%30s ")                                            // Executable tail
_T("%11s ")                                            // Result
_T("%dk/%dk ")                                         // Sizes
_T("D=") WFS _T("/") WFS _T(" Total=") WFS _T(" ")     // Times
_T("E=") WFS _T("/") WFS _T(" ")
_T("\"%s\"");

bool CeCosTest::Value (
  LPCTSTR pszStr, 
  struct tm &t,
  StatusType &status,
  String &target,
  String &strExecutionHostPort,
  String &strExecutableTail,
  String &strTitle,

  int &nFileSize,
  Duration &nTotalTime,
  Duration &nMaxInactiveTime,
  Duration &nDownloadTime,
  Duration &nDownloadTimeout,
  Duration &nActiveTimeout,

  int &nDownloadedSize)
{
  int nLen=_tcslen(pszStr);
  String strStatus;

  nFileSize=nTotalTime=nMaxInactiveTime=nDownloadTime=nDownloadTimeout=nActiveTimeout=nDownloadedSize=0;
  
  int nTotalTimeFrac=0;
  int nMaxInactiveTimeFrac=0;
  int nActiveTimeoutFrac=0;
  int nDownloadTimeFrac=0;
  int nDownloadTimeoutFrac=0;
  
  static String strFormat;
  if(0==strFormat.GetLength()){
    // Construct a version of the format string sans length attributes for %s items
    LPCTSTR c=pszFormat;
    TCHAR *d=strFormat.GetBuffer(_tcslen(pszFormat));
    while(_TCHAR('\0')!=*c){
      if(_TCHAR('%')==c[0] && _istdigit(c[1])){
        *d++=_TCHAR('%');
        do {
          c++;
        } while (_istdigit(*c));
      }
      *d++=*c++;
    }
    *d=_TCHAR('\0');
    strFormat.ReleaseBuffer();
  }
  
  _stscanf(pszStr,
    strFormat,
    &t.tm_year,&t.tm_mon,&t.tm_mday,
    &t.tm_hour,&t.tm_min,&t.tm_sec,         // Time of day
    strExecutionHostPort.GetBuffer(1+nLen),       // Execution host:port
    target.GetBuffer(1+nLen),                  // Target
    strExecutableTail.GetBuffer(1+nLen),          // Executable
    strStatus.GetBuffer(1+nLen),                  // Result
    &nDownloadedSize,&nFileSize,            // Sizes
    &nDownloadTime,&nDownloadTimeFrac,      // Times
    &nDownloadTimeout,&nDownloadTimeoutFrac,
    &nTotalTime,&nTotalTimeFrac,
    &nMaxInactiveTime,&nMaxInactiveTimeFrac,
    &nActiveTimeout,&nActiveTimeoutFrac,
    strTitle.GetBuffer(1+nLen)                    // Title
    );
  
  strExecutionHostPort.ReleaseBuffer();
  target.ReleaseBuffer();
  strExecutableTail.ReleaseBuffer();
  strStatus.ReleaseBuffer();
  strTitle.ReleaseBuffer();
  status=StatusTypeValue(strStatus);

  LPCTSTR c1=_tcschr(pszStr,_TCHAR('"'));
  if(c1){
    c1++;
    LPCTSTR c2=_tcschr(c1+1,_TCHAR('"'));
    if(c2){
      strTitle=String(c1,c2-c1);
    }
  }
  
  nTotalTime=nTotalTime*1000+nTotalTimeFrac*100;
  nMaxInactiveTime=nMaxInactiveTime*1000+nMaxInactiveTimeFrac*100;
  nActiveTimeout=nActiveTimeout*1000+nActiveTimeoutFrac*100;
  nDownloadTime=nDownloadTime*1000+nDownloadTimeFrac*100;
  nDownloadTimeout=nDownloadTimeout*1000+nDownloadTimeoutFrac*100;
  
  nFileSize*=1024;
  nDownloadedSize*=1024;
  t.tm_year-=1900;
  t.tm_mon--;
  return t.tm_year>=0 && t.tm_year<=200 && t.tm_mon>=0 && t.tm_mon<=11 && t.tm_mday>=1 && t.tm_mday<=31 && t.tm_hour>=0 && t.tm_hour<=23 && t.tm_min>=0 && t.tm_min<=59 && t.tm_sec>=0 && t.tm_sec<=59 &&
    status!=StatusTypeMax 
    //&& exetype!=ExecutionParameters::ExecutableTypeMax
    ;
}
  
LPCTSTR  const CeCosTest::ResultString(bool bIncludeOutput) const
{
  String strTitle(m_strTitle);
  String strExecutionHostPort(m_strExecutionHostPort);
  
  if(0==strTitle.GetLength()){
    strTitle=CeCosTestUtils::SimpleHostName();
    strTitle+=_TCHAR(':');
    strTitle+=m_strExecutable; 
  }
  
  if(0==strExecutionHostPort.GetLength()){
    strExecutionHostPort=CeCosTestUtils::SimpleHostName();
    strExecutionHostPort+=_T(":0");
  }
  
  ENTERCRITICAL;
  time_t ltime;
  time(&ltime);
  struct tm *now=localtime( &ltime );
  
  m_strResultString.Format(
    pszFormat,
    1900+now->tm_year,1+now->tm_mon,now->tm_mday,
    now->tm_hour,now->tm_min,now->tm_sec,               // Time of day
    (LPCTSTR )strExecutionHostPort,               // Execution host:port
    Target(),                                    // Target
    ExecutableTail(),                                   // Executable
    Image(Status()),                                    // Result
    m_nStrippedSize/1024,m_nFileSize/1024,          // Sizes
    WF(m_nDownloadTime),WF(DownloadTimeout()),WF(m_nTotalTime),// Times
    WF(m_nMaxInactiveTime),WF(ActiveTimeout()),
    (LPCTSTR )strTitle                              // Title
    );
  if(bIncludeOutput && m_strOutput.GetLength()>0){                            
    m_strResultString+=_TCHAR('\n');
    m_strResultString+=m_strOutput;
  }
  LEAVECRITICAL;
  return m_strResultString;
}

// Run as a server, listening on the port given as parameter
bool CeCosTest::RunAgent(int nTcpPort)
{
  bool bLocked=false;
  
  nAuxPort=nTcpPort+3000;//hack
  nAuxListenSock=CeCosTestSocket::Listen(nAuxPort);//hack
  if(-1!=nAuxListenSock){
    // Create socket
    int nSock = CeCosTestSocket::Listen(nTcpPort);
    int nLastClient=0;
    int nRejectionCount=0;
    if (-1!=nSock) {
      for (;;) {
        try {
          CeCosTestSocket *pSock=new CeCosTestSocket(nSock); // AcceptThreadFunc deletes if not deleted below
          String str;
          // Read the execution parameters
          if(!pSock->recvString(str)){
            // Socket error on the recv - nothing much we can do
            TRACE(_T("RunAgent : could not read execution parameters\n"));
            delete pSock;
            pSock=0;
          } else {
            ExecutionParameters e;
            e.FromStr(str);
            TRACE(_T("Execution parameters: %s\n"),(LPCTSTR)e.Image());
            bool bNuisance=false;
            ServerStatus s;
            CTestResource *pPort=0;
            String strInfo;
            if(!e.IsValid()){
              // Looks like a confused client ...
              strInfo.Format(_T("Bad target value %s read from client\n"),e.Target());
              s=SERVER_CANT_RUN;
            } else if(0==CTestResource::Count(e)){
              // No chance of running this test
              strInfo.Format(_T("Cannot run a %s test from this server\n"),e.Target());
              s=SERVER_CANT_RUN;
            } else {
              switch(e.Request()) {
              case ExecutionParameters::LOCK:
                if(bLocked){
                  s=SERVER_BUSY;
                } else {
                  WaitForAllInstances(1000,NOTIMEOUT);
                  bLocked=true;
                  s=SERVER_LOCKED;
                }
                break;
              case ExecutionParameters::UNLOCK:
                if(bLocked){
                  bLocked=false;
                  s=SERVER_READY;
                } else {
                  s=SERVER_BUSY;
                }
                break;
              case ExecutionParameters::QUERY:
                if (bLocked) {
                  s=SERVER_LOCKED;
                } else {
                  pPort=CTestResource::GetResource(e);
                  if(0==pPort){
                    s=SERVER_BUSY;
                    strInfo.Format(_T("serving %s"),(LPCTSTR )CeCosTestSocket::ClientName(nLastClient));
                  } else {
                    s=SERVER_READY;
                    pPort->Release();
                    pPort=0;
                  }
                }
                break;
              case ExecutionParameters::RUN:
                if (bLocked) {
                  s=SERVER_LOCKED;
                } else {
                  pPort=CTestResource::GetResource(e);
                  if(0==pPort){
                    // We must disappoint our client
                    nRejectionCount++;
                    strInfo.Format(_T("serving %s"),(LPCTSTR )CeCosTestSocket::ClientName(nLastClient));
                    s=SERVER_BUSY;
                    /*
                  } else if(nLastClient==pSock->Client() && nRejectionCount>10) {
                    // Don't answer the phone to a nuisance caller
                    s=SERVER_BUSY;
                    bNuisance=true;
                    nRejectionCount--;  
                    pPort->Release();
                    pPort=0;
                    */
                  } else {
                    s=SERVER_READY;
                    nRejectionCount=0;
                    nLastClient=pSock->Client();
                  }
                }
                break;
              case ExecutionParameters::STOP:
                s=SERVER_READY;
                break;
              default:
                s=SERVER_CANT_RUN;
              }
            }
            
#ifndef VERBOSE
            if(ExecutionParameters::QUERY!=e.Request())
#endif
              TRACE(_T("RunAgent : %s request tActive=%d tElapsed=%d Target=%s Reply status=%s %s Nuisance=%d\n"),
              e.Image(e.Request()),
              e.ActiveTimeout(),e.DownloadTimeout(),e.Target(),
              Image(s),
              (LPCTSTR )strInfo,
              bNuisance);
            
            bool bSendok=pSock->sendInteger(s) && pSock->sendString(strInfo);
            
            TRACE(_T("RunAgent(1)\n"));
            
            if(SERVER_READY==s && bSendok && ExecutionParameters::RUN==e.Request()){
              
              // Create a new class instance
              // AcceptThreadFunc deletes the instance and closes new_sock
              // RunLocal, called by AcceptThreadFunc, releases the port
              // We dream up a temporary name for the executable
              // No need for meaningful callback, but must run asynchronously
              String strTempFile;
              ENTERCRITICAL;
              strTempFile.Format(_T("%s-%s-%d"),_ttmpnam(0),e.Target(),nTcpPort);
              LEAVECRITICAL;
              
              CeCosTest *pTest=new CeCosTest(e,strTempFile);
              pTest->m_pSock=pSock;
              pTest->m_strExecutionHostPort.Format(_T("%s:%d"),CeCosTestUtils::HostName(),nTcpPort);
              pTest->m_pPort=pPort;
              
              CeCosThreadUtils::RunThread(SAcceptThreadFunc,pTest,_T("SAcceptThreadFunc"));
              // AcceptThreadFunc deletes pSock
              
            } else {
              TRACE(_T("RunAgent(2)\n"));
              delete pSock;
              pSock=0;
              if(pPort){
                pPort->Release();
                pPort=0;
              }
              TRACE(_T("RunAgent(3)\n"));
              if(CeCosTest::ExecutionParameters::STOP==e.Request()){
                CancelAllInstances();
                WaitForAllInstances(1000,20*1000);
                break;
              }
            }
                }
            }
            catch(...){
              TRACE(_T("!!! Exception caught in RunAgent()\n"));
            }
        }
        CeCosTestSocket::CloseSocket (nSock);
    }
    VTRACE(_T("RunAgent(): returning false\n"));
    CeCosTestSocket::CloseSocket(nAuxListenSock);
  }
  return false;
}

CeCosTest::StatusType CeCosTest::StatusTypeValue(LPCTSTR  pszStr)
{
  for(int i=0;i<StatusTypeMax;i++){
    StatusType t=(StatusType)i;
    if(0==_tcsicmp(Image(t),pszStr)){
      return t;
    }
  }
  return StatusTypeMax;
}

// Thread to run ConnectSocketToSerial
void CeCosTest::ConnectSocketToSerialThreadFunc()
{
  TRACE(_T("ConnectSocketToSerialThreadFunc sock=%d\n"),nAuxListenSock);
  {
    
    CeCosTestSerialFilter serial_filter;
    CeCosTestSocket::FilterFunc *serial_filter_function = 
      &SerialFilterFunction;
    
    CeCosTestDownloadFilter download_filter;
    CeCosTestSocket::FilterFunc *download_filter_function = 
      &DownloadFilterFunction;
    
    bool accept_connection = true;
    
    CeCosTestSerial serial;
    serial.SetBlockingReads(false);
    bool rc=false;
    // Open serial device.
    if (!serial.Open(m_pPort->Serial(),m_pPort->Baud())){
      ERROR(_T("Couldn't open port %s\n"),m_pPort->Serial());
    } else {
      while(accept_connection) {
        // Flush the serial buffer.
        serial.Flush();
        TRACE(_T("ConnectSocketToSerial: waiting for connection...\n"));
        CeCosTestSocket socket;
        if(!socket.Accept(nAuxListenSock,&m_bStopConnectSocketToSerial)){
          ERROR(_T("ConnectSocketToSerial - couldn't accept\n"));
          break;
        } else if (m_pSock->Client() != socket.Client()){    
          // Make sure the client is who we think it is...
          TRACE(_T("ConnectSocketToSerialThread - illegal connection attempted from %s\n"),(LPCTSTR )socket.ClientName(socket.Client()));
        } else {
          try {
            rc=CeCosTestSocket::ConnectSocketToSerial(socket,serial,m_ep.m_bUseFilter?serial_filter_function:NULL, (void*)&serial_filter, m_ep.m_bUseFilter?download_filter_function:NULL, (void*)&download_filter, &m_bStopConnectSocketToSerial);
            
            // If the download filter was just active, it may
            // allow the session to continue.
            accept_connection = download_filter.ContinueSession();
            
          }
          catch (LPCTSTR pszMsg){
            Log(_T("!!! ConnectSocketToSerial exception caught: %s!!!\n"),pszMsg);
            rc=false;
            break;
          }
          catch (...){
            Log(_T("!!! ConnectSocketToSerial exception caught!!!\n"));
            rc=false;
            break;
          }
        }
      }
    }
    TRACE(_T("ConnectSocketToSerial : done\n"));
  }
}

static bool CALLBACK DerefBool(void *pParam)
{
  return *(bool *)pParam;
}

// Function called (on a separate thread) to process a successful connection to the RunAgent loop
void CeCosTest::AcceptThreadFunc()
{
  SetPath(m_strPath);
  
  if(ServerSideGdb()){
    int n;
    if(m_pSock->recvInteger(n,_T("file size"))){
      m_nFileSize=n;
      // Read file from the socket
      bool bCanRun=true;
      TRACE(_T("AcceptThreadFunc file size=%d reading...\n"),m_nFileSize);
      FILE *f2;
      f2=_tfopen(m_strExecutable,_T("wb"));
      if(0==f2){
        Log(_T("Could not create %s - %s\n"),(LPCTSTR )m_strExecutable,strerror(errno));
        bCanRun=false;
      }
      unsigned int nBufSize=MIN(100000,m_nFileSize);
      Buffer b(nBufSize);
      unsigned int nWritten=0;
      unsigned int nRead=0;
      while(nRead<m_nFileSize){
        int nToRead=MIN(nBufSize,m_nFileSize-nRead);
        if(!recv( b.Data(), nToRead, _T("executable"))){
          break;
        }
        nRead+=nToRead;
        if(0!=f2){
          char *c=(char *)b.Data();
          while(nToRead>0){
            int w=fwrite(c,1,nToRead,f2);
            if(-1==w){
              Log(_T("Write error on %s - %s\n"),(LPCTSTR )m_strExecutable,strerror(errno));
              bCanRun=false;
              break;
            }
            nWritten+=w;
            c+=w;
            nToRead-=w;
          }
        }
      }
      TRACE(_T("Accept - done reading [%d bytes read, %d bytes written]\n"),nRead,nWritten);
      if(0!=f2){
        fclose(f2);
        _tchmod(m_strExecutable,00700); // user read, write and execute
      }
      if(0!=f2 && m_nFileSize!=nWritten){
        Log(_T("Failed to create %s correctly [%d/%d bytes written]\n"),(LPCTSTR )m_strExecutable, nWritten, m_nFileSize);
        bCanRun=false;
      }
      SetExecutable(m_strExecutable); // to set stripped length and title
      RunLocal();
      _tunlink(m_strExecutable);
    }
    sendResult();
    m_pSock->recvInteger(n); // receive an ack
  } else {
    bool bTargetReady;
    if(_TCHAR('\0')==*(m_pPort->ResetString())){
      bTargetReady=true;
      TRACE(_T("No reset possible\n"));
    } else {
      Log(_T("Resetting target using %s"),(LPCTSTR)m_pPort->ResetString());
      bTargetReady=(CResetAttributes::RESET_OK==Reset(true));
    }
    TRACE(_T("Send Target Ready indicator=%d\n"),bTargetReady);
    m_pSock->sendInteger(bTargetReady,_T("target ready indicator"));
    
    int nAck;
    int dTimeout=m_ep.DownloadTimeout()+MAX(3*m_ep.ActiveTimeout(),15*60*1000);
    
    if(bTargetReady){
      if(CeCosTestSocket::IsLegalHostPort(m_pPort->Serial())){
        TRACE(_T("Sending %s\n"),(LPCTSTR )m_pPort->Serial());
        m_pSock->sendString(m_pPort->Serial(),_T("Serial name"));
        m_pSock->recvInteger(nAck,_T("Terminating ack"),dTimeout);
        TRACE(_T("Terminating ack=%d\n"),nAck);
      } else {
        String strHostPort(CeCosTestSocket::HostPort(CeCosTestUtils::HostName(),nAuxPort));
        
        TRACE(_T("Using %s\n"),(LPCTSTR )strHostPort);
        
        if(m_pSock->sendString(strHostPort,_T("host:port"))){
          
          // This Boolean signifies that the serial<-->tcp/ip conversation is done.  It may be set
          // on completion of the ConnectSocketToSerial thread (which is why we pass it to runthread)
          // and also set by us to *cause* the thread to complete.
          
          bool bConnectSocketToSerialThreadDone=false; // Indication of termination of ConnectSocketToSerial thread
          m_bStopConnectSocketToSerial=false; // Used to tap ConnectSocketToSerial thread on the shoulder
          
          CeCosThreadUtils::RunThread(SConnectSocketToSerialThreadFunc,this,&bConnectSocketToSerialThreadDone,_T("SConnectSocketToSerialThreadFunc")); 
          
          // Wait for either client or the ConnectSocketToSerial thread to finish.
          m_pSock->recv(&nAck,1,_T("Terminating ack"),dTimeout,DerefBool,&bConnectSocketToSerialThreadDone);
          TRACE(_T("Terminating ack=%d\n"),nAck);
          if(!bConnectSocketToSerialThreadDone){
            // Tap ConnectSocketToSerial thread on the shoulder
            TRACE(_T("Waiting for ConnectSocketToSerial thread to terminate...\n"));
            m_bStopConnectSocketToSerial=true;
            CeCosThreadUtils::WaitFor(bConnectSocketToSerialThreadDone);
          }
          TRACE(_T("ConnectSocketToSerial thread terminated...\n"));
          
        }
      }
    }
  }
  delete this;
}

bool CeCosTest::send(const void *pData,unsigned int nLength,LPCTSTR pszMsg,Duration dTimeout)
{
  return m_pSock->send(pData,nLength,pszMsg,dTimeout,IsCancelled,this);
}

bool CeCosTest::recv(const void *pData,unsigned int nLength,LPCTSTR pszMsg,Duration dTimeout)
{
  return m_pSock->recv(pData,nLength,pszMsg,dTimeout,IsCancelled,this);
}

void CeCosTest::Log(LPCTSTR  pszFormat, ...)
{
  va_list args;
  va_start(args, pszFormat);
  String str;
  str.vFormat(pszFormat,args);
  va_end(args);
  LogString(str);
}

void CeCosTest::LogString(LPCTSTR psz)
{
  if(*psz){
    ENTERCRITICAL;
    m_strOutput+=psz;
    LEAVECRITICAL;
    if(CeCosTrace::IsInteractive()){
      CeCosTrace::Out(psz);
    } else {
      TRACE(_T("%s"),psz);
    }
  }
}

bool CeCosTest::sendResult(Duration dTimeout)
{
  bool rc=
    m_pSock->sendInteger(m_Status,_T("result"),dTimeout) &&
    m_pSock->sendInteger(m_nDownloadTime,_T("result"),dTimeout) &&
    m_pSock->sendInteger(m_nTotalTime,_T("result"),dTimeout) &&
    m_pSock->sendInteger(m_nMaxInactiveTime,_T("result"),dTimeout) &&
    m_pSock->sendString (m_strOutput,_T("result"),dTimeout);
  return rc;
}

bool CeCosTest::recvResult(Duration dTimeout)
{
  String strOutput;
  int nStatus=StatusTypeMax;
  bool rc=
    m_pSock->recvInteger(nStatus,_T("result"),dTimeout) &&
    m_pSock->recvInteger(m_nDownloadTime,_T("result"),dTimeout) &&
    m_pSock->recvInteger(m_nTotalTime,_T("result"),dTimeout) &&
    m_pSock->recvInteger(m_nMaxInactiveTime,_T("result"),dTimeout) &&
    m_pSock->recvString (strOutput,_T("result"),dTimeout);
  m_Status=(StatusType)MIN(nStatus,StatusTypeMax);
  LogString(strOutput);
  return rc;
}

#ifndef _WIN32
// This function may be run as a thread (win32) or called (unix).
// It runs gdb.
void CeCosTest::DriveGdb(LPCTSTR pszPrompt,const StringArray &arstrGdbCmds)
{
  unsigned int nCmdIndex=0;
  
  SetStatus(NotStarted);
  
  m_nMaxInactiveTime=0;
  m_nTotalTime=0;
  m_nDownloadTime=0;
  
  m_bDownloading=true;
  
  m_tBase=GdbTime();
  m_tBase0=GdbTime();
  m_tWallClock0=Now();
  
  TRACE(_T("DriveGdb()\n"));
  
  int nLastGdbInst=0;
  
  // Loop until 1 of:
  //      1. Timeout detected
  //      2. Gdb process is dead
  //      3. At a gdb prompt and no more commands to send to gdb
  //      4. Pipe read failure
  //      5. Pipe write failure
  do {
    String str;
    int readrc=ReadPipe(str,/*bBlockingReads=*/false);
    switch(readrc){
    case 0:
      Sleep(250); // only unix will execute this
      break;
    case -1:
      goto Done; // case 4
      break;
    default:
      LogTimeStampedOutput(str);
      
      if(m_strOutput.GetLength()>20000){
        LogString(_T("\n>>>> Infra FAIL\n*** too much output ***\n>>>>\n"));
        SetStatus(Fail);
        goto Done;
      }
      
      // Test for program loaded and started:
      // (remember SetStatus cannot downgrade the status if already > NoResult)
      if(OutputContains(_T("Starting program: /")) || (OutputContains(_T("Start address"))&&OutputContains(_T("Continuing.")))){
        SetStatus(NoResult);
      }
      
      m_tBase=GdbTime();
      //              // If can only hit a single breakpoint don't expect cyg_test_exit to stop us:
      //              if(!BreakpointsOperational() && (OutputContains(_T("EXIT:"))||OutputContains(_T("NOTAPPLICABLE:")))){
      //                  goto Done;
      //              }
      
      if(AtPrompt(pszPrompt)){
        m_tBase=GdbTime();
        TRACE(_T("DriveGdb(1)\n"));
        // gdb's output included one or more prompts
        // Send another command along
        if(nCmdIndex>=arstrGdbCmds.size()){
          // Nothing further to say to gdb - exit
          TRACE(_T("DriveGdb(2)\n"));
          goto Done; // case 3
        } else {
          String strCmd(arstrGdbCmds[nCmdIndex++]);
          TRACE(_T("DriveGdb(2a) - strCmd='%s' nLastGdbInst=%d\n"),(LPCTSTR)strCmd,nLastGdbInst);
          // If at a prompt and we can see a GDB instruction, send it down
          LPCTSTR pszGdbcmd=_tcsstr(nLastGdbInst+(LPCTSTR)m_strOutput,_T("GDB:"));
          if(pszGdbcmd){
            TRACE(_T("DriveGdb(2b)\n"));
            pszGdbcmd+=4;
            TCHAR cTerm;
            if(_TCHAR('<')==*pszGdbcmd){
              cTerm=_TCHAR('>');
              pszGdbcmd++;
            } else {
              cTerm=_TCHAR('\n');
            }
            TRACE(_T("DriveGdb(2c)\n"));
            LPCTSTR c=_tcschr(pszGdbcmd,cTerm);
            if(c){
              TRACE(_T("DriveGdb(2d)\n"));
              strCmd=String(pszGdbcmd,c-pszGdbcmd);
              nLastGdbInst=c+1-(LPCTSTR)m_strOutput;
              nCmdIndex--; // undo increment above
            }
          }
          strCmd+=_TCHAR('\n');
          LogString(strCmd);
          TRACE(_T("DriveGdb(3)\n"));
          if(!WritePipe(strCmd)){
            TRACE(_T("Writepipe returned error\n"));
            goto Done; // case 5
          }                        
          TRACE(_T("DriveGdb(4)\n"));
          if(0==_tcscmp(strCmd,_T("run\n"))||0==_tcscmp(strCmd,_T("cont\n"))){
            m_tBase=GdbTime();
            m_bDownloading=false;
          }
          TRACE(_T("DriveGdb(5)\n"));
        }
      }
      break;
    }
    
  } while (GdbProcessAlive() && CheckForTimeout()); // cases 2 and 1
  
Done:
  
  TRACE(_T("DriveGdb() - done\n"));
  
  Suck(pszPrompt);
  if(GdbProcessAlive() && AtPrompt(pszPrompt)){
    LogString(_T("bt\n"));
    WritePipe(_T("bt\n"));
    Suck(pszPrompt);
    LogString(_T("quit\n"));
    WritePipe(_T("quit\n"));
  }
  
  // Read anything gdb has to say [within limits]
  Suck(pszPrompt);
  AnalyzeOutput();
  
  m_nTotalTime=Now()-m_tWallClock0;
  TRACE(_T("Exiting DriveGdb()\n"));
  
}
#endif

// Return time used by inferior gdb process - CPU for sim, wallclock otherwise
Time CeCosTest::GdbTime()
{
  return _TCHAR('\0')==*(m_pPort->Serial())?GdbCpuTime():Now();
}

bool CeCosTest::CheckForTimeout()
{
  bool rc=false;
  if(TimeOut!=m_Status && DownloadTimeOut!=m_Status){
    Duration &dTime=m_bDownloading?m_nDownloadTime:m_nMaxInactiveTime;
    Time t=GdbTime();
    if(t){
      dTime=MAX(dTime,Duration(GdbTime()-m_tBase));
    }
    Duration dTimeout=m_bDownloading?DownloadTimeout():ActiveTimeout();
    if(dTimeout!=NOTIMEOUT && dTime>dTimeout) {
      Log(_T("\n*** Timeout - %s time ") WFS _T(" exceeds limit of ") WFS _T("\n"),
        m_bDownloading?_T("download"):_T("MAX inactive"),WF(dTime),WF(dTimeout));
      SetStatus(m_bDownloading?DownloadTimeOut:TimeOut);
    } else if(Now()-m_tWallClock0>MAX(3*dTimeout,15*60*1000)){
      Log(_T("\n*** Timeout - total time ") WFS _T(" exceeds limit of ") WFS _T("\n"),
        WF(Now()-m_tWallClock0),WF(MAX(3*dTimeout,15*60*1000)));
      SetStatus(m_bDownloading?DownloadTimeOut:TimeOut);
    } else {
      rc=true;
    }
  }
  return rc;
}

void CeCosTest::Interactive(LPCTSTR  pszFormat, ...)
{
  va_list marker;
  va_start (marker, pszFormat);
  String str;
  str.vFormat(pszFormat,marker);
  va_end (marker);
  
  if(CeCosTrace::IsInteractive()){
    CeCosTrace::Out(str);
  } else {
    CeCosTrace::Trace(_T("%s"),(LPCTSTR)str);
  }
}

LPCTSTR  const CeCosTest::Title() const { 
  return m_strTitle;
}

// Convert a path to something a cygwin tool will understand.  Used when invoking -size and -gdb
String CeCosTest::CygPath (LPCTSTR pszPath)
{
#ifdef _WIN32
  String str;
  TCHAR *buf=str.GetBuffer(2+MAX_PATH);
  TCHAR *pszFname;
  if(::GetFullPathName(pszPath,MAX_PATH,1+buf, &pszFname)){
    GetShortPathName(1+buf,1+buf,MAX_PATH); // ignore errors
    buf[0]=_TCHAR('/');
    buf[2]=buf[1];
    buf[1]=_TCHAR('/');
    for(int i=3;buf[i];i++){
      if(_TCHAR('\\')==buf[i]){
        buf[i]=_TCHAR('/');
      }
    }
    str.ReleaseBuffer();
    return str;
  } else {
    str.ReleaseBuffer();
    return pszPath;
  }
#endif
  return pszPath;
}

void CeCosTest::SetExecutable(LPCTSTR pszExecutable)
{
  m_strOutput=_T("");
  m_strResultString=_T("");
  m_strExecutable=pszExecutable;
  if(pszExecutable && !GetSizes(m_strExecutable, m_ep.Target(), m_nFileSize, m_nStrippedSize)){
    const TargetInfo &t=Target(Target());
    Log(_T("Failed to run %s-size to determine executable size of %s\n"),t.Prefix(),pszExecutable);
  }
}

// Calculate the sizes of the given file.  The target parameter is necessary in order to 
// determine which -size executable to use to do the job.
bool CeCosTest::GetSizes(LPCTSTR pszExecutable, LPCTSTR target,unsigned int &nFileSize, unsigned int &nStrippedSize)
{
  bool rc=false;
  nStrippedSize=nFileSize=0;
  struct _stat buf;
  const TargetInfo &t=Target(target);
  LPCTSTR pszPrefix=t.Prefix();
  if(0==_tstat(pszExecutable,&buf) && _TCHAR('\0')!=*pszPrefix){
    // File at least exists...
    nFileSize=buf.st_size;
    String strSize(pszPrefix);
    strSize+=_T("-size ");
    strSize+=CygPath(pszExecutable);
    const TCHAR *c=0;
#ifdef _WIN32
    CSubprocess sp;
    String strOut;
    if(!sp.Run(strOut,strSize,NULL)){
      return true;
    }
    c=_tcschr(strOut,_TCHAR('\n'));
    if(c){
      c++;
    }
#else // UNIX
    TCHAR buf[256];
    FILE *f=POPEN(strSize,_T("r"));
    if(f){
      _fgetts(buf,sizeof buf-1,f);
      _fgetts(buf,sizeof buf-1,f);
      PCLOSE(f);
      c=buf;
    }
#endif
    int s1=0;
    int s2=0;
    rc=(c && 2==_stscanf(c,_T(" %d %d"),&s1,&s2));
    nStrippedSize=s1+s2;
  }
  return rc;
}

void CeCosTest::SetTimeouts (Duration dActive,Duration dElapsed)
{
  m_ep.SetActiveTimeout (dActive);
  m_ep.SetDownloadTimeout(dElapsed);
}

void CeCosTest::CloseSocket (){
  delete m_pSock;
  m_pSock=0;
}

bool CeCosTest::AtPrompt(LPCTSTR pszPrompt)
{
  unsigned int nPromptLen=_tcslen(pszPrompt);
  return
    m_strOutput.GetLength()>=nPromptLen && 
    0==_tcscmp((LPCTSTR )m_strOutput+m_strOutput.GetLength()-nPromptLen,pszPrompt);
}

#ifndef _WIN32
bool CeCosTest::Suck(LPCTSTR pszPrompt,Duration d)
{
  TRACE(_T("Suck handle=%08x\n"),m_rPipeHandle);//sdf
  // Read until:
  //     8k read
  //     timeout elapsed
  //     gdb prompt reached
  //     pipe error
  Time t0=Now();
  int nLength=0;
  while(nLength<8192 && m_rPipeHandle && !AtPrompt(pszPrompt) && Now()-t0<d){
    String str;
    int n=ReadPipe(str);
    if(n>0){
      LogTimeStampedOutput(str);
      nLength+=n;
    } else if (n<0) {
      break;
    }
  }
  TRACE(_T("End suck\n"));//sdf
  return nLength>0;
}
#endif

void CeCosTest::LogTimeStampedOutput(LPCTSTR psz)
{
  LogString(psz);
  /*
  String str(psz);
  // Timestamp the output at each _TCHAR('\n')
  int nLen=m_strOutput.GetLength();
  TCHAR cPrev=(0==nLen?_TCHAR('\0'):((LPCTSTR )m_strOutput)[nLen-1]);
  
    TCHAR *c=str.GetBuffer();
    LPCTSTR d=c;
    while(*c){
    if(_TCHAR('\n')==cPrev){
    TCHAR cSav=*c;
    *c=_TCHAR('\0');
    LogString(d);
    Duration &dTime=m_bDownloading?m_nDownloadTime:m_nMaxInactiveTime;
    dTime=MAX(dTime,GdbTime()-m_tBase);
    
      String strTime;
      strTime.Format(_T("<") WFS _T("/") WFS _T(">\t"),WF(GdbTime()-m_tBase0), WF(GdbTime()-m_tBase));
      //strTime.Format(_T("<%03d.%d> "),t/1000,(t%1000)/100);
      LogString(strTime);
      *c=cSav;
      d=c;
      }
      cPrev=*c;
      c++;
      }
      LogString(d);
      str.ReleaseBuffer();
  */
}

#ifdef _WIN32
BOOL WINAPI HandlerRoutine(
                           DWORD dwCtrlType   //  control signal type
                           )
{
  dwCtrlType; // eliminate compiler warning
  return TRUE;
}
#endif


bool CeCosTest::InteractiveGdb(const String &strHost,int nPort,TCHAR **argv)
{
  bool rc=false;
  if(strHost.GetLength()>0){
    m_strExecutionHostPort=CeCosTestSocket::HostPort(strHost,nPort);
    Log(_T("Waiting to connect to %s...\n"),(LPCTSTR )m_strExecutionHostPort);
  } else {
    Log(_T("Waiting to connect to a server...\n"));
  }
  
  ConnectForExecution();
  
  Log(_T("Connected to %s - waiting for target reset\n"),(LPCTSTR )m_strExecutionHostPort);
  String strHostPort,strOutput;
  // We read:
  //     target ready indicator
  //     any output so far
  //     (if target ready) host:port
  if(GetTargetReady(strHostPort)){
    Log(_T("Use %s\n"),(LPCTSTR)strHostPort);
    const TargetInfo &t=Target(Target());
    String strGdb(t.Prefix());
    strGdb+=_T("-gdb");
#ifdef _WIN32
    SetConsoleCtrlHandler(HandlerRoutine,TRUE);
    int n=_tspawnvp(_P_WAIT,strGdb,argv);
    if(-1==n){
      Log(_T("Failed to spawn %s\n"),(LPCTSTR)strGdb);
    } else {
      rc=(0==n);
    }   
    SetConsoleCtrlHandler(HandlerRoutine,FALSE);
#else // UNIX
    
    int pid=fork();
    switch(pid){
    case -1:
      _ftprintf(stderr,_T("fork failed\n"));
      pid=0;
      break;  
    case 0:
      // Process is created (we're the child)
      execvp(strGdb,argv);
      Log(_T("Error invoking %s - %s\n"),(LPCTSTR)strGdb,strerror(errno));
      exit(1);
      break;
    default:
      // Process is created (we're the parent)
      {
        signal(SIGINT,SIG_IGN);
        int stat;
        waitpid(pid,&stat,0);
        rc=(0==stat);
        signal(SIGINT,SIG_DFL);
      }
      break;
    }
#endif
    Log(_T("Gdb terminated\n"));
    // Tell the server we're through
    m_pSock->sendInteger(123,_T("Terminating ack"));
  }
  
  return rc;
}

void CALLBACK CeCosTest::ResetLogFunc(void *pParam, LPCTSTR psz) 
{
  CeCosTest *pTest=(CeCosTest *)pParam;
  TRACE(_T("Send Target Ready indicator=2\n"));
  pTest->m_pSock->sendInteger(2,_T("target ready indicator"));
  TRACE(_T("Send %s\n"),psz);
  pTest->m_pSock->sendString(psz,_T("output so far"));
}

CResetAttributes::ResetResult CeCosTest::Reset(bool bSendStatus)
{
  return m_pPort->Reset(bSendStatus?ResetLogFunc:0,this);
}

CeCosTest::ExecutionParameters::RequestType CeCosTest::ExecutionParameters::RequestTypeValue(LPCTSTR psz)
{
  int r;
  for(r=0;r<RequestTypeMax;r++){
    if(0==_tcsicmp(psz,arRequestImage[r])){
      break;
    }
  }
  return (RequestType)r;
}

#ifdef _WIN32
void CeCosTest::RunGdb(LPCTSTR pszCmdline,LPCTSTR pszPrompt,const StringArray &arstrGdbCmds)
{
  SetPath(m_strPath);
  CSubprocess sp;

  unsigned int idProcess=sp.Run(GetCurrentThreadId(),pszCmdline);
  if(!idProcess){
    Log(_T("Failed to create gdb process: cmdline='%s'\n"),pszCmdline);
    TCHAR *pszMsg;
    
    FormatMessage( 
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL,
      sp.GetExitCode(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR)&pszMsg,
      0,
      NULL 
      );
    
    // Display the string.
    Log(_T("%s\n"),pszMsg);
    
    // Free the buffer.
    LocalFree(pszMsg);
  } else {

    unsigned int nCmdIndex=0;
  
    SetStatus(NotStarted);
  
    m_nMaxInactiveTime=0;
    m_nTotalTime=0;
    m_nDownloadTime=0;
  
    m_bDownloading=true;
  
    m_tBase=GdbTime();
    m_tBase0=GdbTime();
    m_tWallClock0=Now();
  
    TRACE(_T("RunGDb()\n"));
  
    int nLastGdbInst=0;
  
    while(Cancelled!=Status() && CheckForTimeout()){
      MSG msg;
      if(::PeekMessage(&msg,NULL,WM_SUBPROCESS,WM_SUBPROCESS+1,PM_NOREMOVE)){
        switch(::GetMessage(&msg,NULL,WM_SUBPROCESS,WM_SUBPROCESS+1)){
        case 0:  //WM_QUIT
        case -1: // error
          goto Done;
        case 1:
          if(WM_SUBPROCESS==msg.message && msg.wParam==idProcess){
            if(msg.lParam){
              LPTSTR pszMsg=(LPTSTR)msg.lParam;
              LogTimeStampedOutput(pszMsg);
            
              if(m_strOutput.GetLength()>20000){
                LogString(_T("\n>>>> Infra FAIL\n*** too much output ***\n>>>>\n"));
                SetStatus(Fail);
                goto Done;
              }
            
              // Test for program loaded and started:
              // (remember SetStatus cannot downgrade the status if already > NoResult)
              if(OutputContains(_T("Starting program: /")) || (OutputContains(_T("Start address"))&&OutputContains(_T("Continuing.")))){
                SetStatus(NoResult);
              }
            
              m_tBase=GdbTime();
              //              // If can only hit a single breakpoint don't expect cyg_test_exit to stop us:
              //              if(!BreakpointsOperational() && (OutputContains(_T("EXIT:"))||OutputContains(_T("NOTAPPLICABLE:")))){
              //                  goto Done;
              //              }
            
              if(AtPrompt(pszPrompt)){
                m_tBase=GdbTime();
                TRACE(_T("RunGDb(1)\n"));
                // gdb's output included one or more prompts
                // Send another command along
                if(nCmdIndex>=arstrGdbCmds.size()){
                  // Nothing further to say to gdb - exit
                  TRACE(_T("RunGDb(2)\n"));
                  goto Done; // case 3
                } else {
                  String strCmd(arstrGdbCmds[nCmdIndex++]);
                  TRACE(_T("RunGDb(2a) - strCmd='%s' nLastGdbInst=%d\n"),(LPCTSTR)strCmd,nLastGdbInst);
                  // If at a prompt and we can see a GDB instruction, send it down
                  LPCTSTR pszGdbcmd=_tcsstr(nLastGdbInst+(LPCTSTR)m_strOutput,_T("GDB:"));
                  if(pszGdbcmd){
                    TRACE(_T("RunGDb(2b)\n"));
                    pszGdbcmd+=4;
                    TCHAR cTerm;
                    if(_TCHAR('<')==*pszGdbcmd){
                      cTerm=_TCHAR('>');
                      pszGdbcmd++;
                    } else {
                      cTerm=_TCHAR('\n');
                    }
                    TRACE(_T("RunGDb(2c)\n"));
                    LPCTSTR c=_tcschr(pszGdbcmd,cTerm);
                    if(c){
                      TRACE(_T("RunGDb(2d)\n"));
                      strCmd=String(pszGdbcmd,c-pszGdbcmd);
                      nLastGdbInst=c+1-(LPCTSTR )m_strOutput;
                      nCmdIndex--; // undo increment above
                    }
                  }
                  strCmd+=_TCHAR('\n');
                  LogString(strCmd);
                  TRACE(_T("RunGDb(3)\n"));
                  sp.Send(strCmd);          
                  TRACE(_T("RunGDb(4)\n"));
                  if(0==_tcscmp(strCmd,_T("run\n"))||0==_tcscmp(strCmd,_T("cont\n"))){
                    m_tBase=GdbTime();
                    m_bDownloading=false;
                  }
                  TRACE(_T("RunGDb(5)\n"));
                }
              }
              delete [] pszMsg;
            } else {
              goto Done;
            }
          }
          break;
        }
      } else { // no message
        Sleep (500l); // sleep 500ms before the next peek
      }
    }
    
Done:  
    // Could use CygKill here
    sp.Kill();
    TRACE(_T("RunGDb - Done\n"));
    AnalyzeOutput();
    
    m_nTotalTime=Duration(Now()-m_tWallClock0);
  }
  TRACE(_T("Exiting RunGdb()\n"));
}

bool CeCosTest::GdbProcessAlive()
{
  DWORD dwExitRc;
  GetExitCodeProcess((HANDLE)m_pGdbProcesshandle,&dwExitRc);
  return STILL_ACTIVE==dwExitRc;
}

Time CeCosTest::GdbCpuTime()
{
  HANDLE hProcess=(HANDLE)m_pGdbProcesshandle;
  __int64 ftCreation,ftExit,ftKernel,ftUser;
  if(NULL!=hProcess && GetProcessTimes (hProcess,(FILETIME *)&ftCreation,(FILETIME *)&ftExit,(FILETIME *)&ftKernel,(FILETIME *)&ftUser)){
    return Time((int)((ftKernel+ftUser)/10000));
  } else {
    return 0;
  }
}

void CeCosTest::GetPath(String &strPath)
{
  int nSize=GetEnvironmentVariable(_T("PATH"), NULL, 0);
  if(nSize>0){
    GetEnvironmentVariable(_T("PATH"), strPath.GetBuffer(nSize), nSize);
    strPath.ReleaseBuffer();
  } else {
    strPath=_T("");
  }
}

void CeCosTest::SetPath(const String &strPath)
{
  SetEnvironmentVariable(_T("PATH"), strPath);
}

#else // UNIX

void CeCosTest::RunGdb(LPCTSTR pszCmdline,LPCTSTR pszPrompt,const StringArray &arstrGdbCmds)
{
  int pipe_ends_w[2];
  if (pipe(pipe_ends_w) < 0 ) { 
    Log(_T("Failed to create pipe_ends_w - %s\n"),strerror(errno));
  } else {
    int pipe_ends_r[2];
    if (pipe(pipe_ends_r) < 0 ) { 
      Log(_T("Failed to create pipe_ends_r - %s\n"),strerror(errno));
    } else {
      int new_pid;			
		  ENTERCRITICAL;
      // Ensure no one else has the lock such that the child might block in future
      new_pid = fork();
      // This leave is executed in *both* the child and parent
      LEAVECRITICAL;
      
      switch (new_pid) {
        // Fork failed
      case -1:
        Log(_T("Failed to create gdb process - %s\n"),strerror(errno));
        break;
      case 0:
        // Process is created (we're the child)
        // No point in calling Log in this process
        
        // Input to child process
        if (dup2(pipe_ends_w[0], 0) < 0) {
          TRACE(_T("dup2 error\n"));
          exit(1);
        }
        
        // Output from process
        if (dup2(pipe_ends_r[1], 2) < 0) {
          TRACE(_T("dup2 error\n"));
          exit(2);
        }
        if (dup2(pipe_ends_r[1], 1) < 0) {
          TRACE(_T("dup2 error\n"));
          exit(3);
        }
        setvbuf(stdout,0,_IONBF,0);
        setvbuf(stderr,0,_IONBF,0);
        {
          StringArray ar;
          int argc=String(pszCmdline).Chop(ar,_TCHAR(' '),true);
          char **argv=new char *[1+argc];
          int i;
          for(i=0;i<argc;i++){
            argv[i]=ar[i];  
          }
          argv[i]=0;
          execvp(argv[0], argv);
          delete [] argv;
        }
        TRACE(_T("exec error - %s\n"),strerror(errno));
        exit(4);
      default:
        // Process is created (we're the parent)
        
        TRACE(_T("Forked to create gdb process %s - pid is <%d>\n"), pszCmdline, new_pid);
        if (fcntl(pipe_ends_r[0], F_SETFL, O_NONBLOCK) <0) {
          Log(_T("Couldn't set pipe non-blocking - %s\n"),strerror(errno));
        } else {
          m_pGdbProcesshandle=(void *)new_pid;
          VTRACE(_T("RunGdb():Calling DriveGdb\n"));
          m_rPipeHandle=(void *)pipe_ends_r[0];
          m_wPipeHandle=(void *)pipe_ends_w[1];
          DriveGdb (pszPrompt,arstrGdbCmds);
          // Finished one way or another. Kill gdb now
          TRACE(_T("Finished processing this test.\n"));
          if(GdbProcessAlive()){
            TRACE(_T("Killing gdb\n"));

            // We need to kill gdb *and* its children
            FILE *f=popen(_T("ps -l"),_T("r"));
            if(f){
              TCHAR buf[100];
              while(_fgetts(buf,sizeof(buf)-1,f)){
                int F,UID,PID,PPID,C,PRI,NI,SZ,HH,MM,SS;
                TCHAR discard[100];
                // Output is in the form
                //  F S   UID   PID  PPID  C PRI  NI ADDR    SZ WCHAN  TTY          TIME CMD
                //100 S   490   877   876  0  70   0    -   368 wait4  pts/0    00:00:00 bash
                if(15==_stscanf(buf,
                  _T("%d %s %d %d %d %d %d %d %s %d %s %s %d:%d:%d"),&F,discard,&UID,&PID,&PPID,&C,&PRI,&NI,discard,&SZ,discard,discard,&HH,&MM,&SS) &&
                  (PID==new_pid || PPID==new_pid)){
                  kill(PID,SIGTERM);
                }
              }
              pclose(f);
              TRACE(_T("waitpid <%d>"), new_pid);
              int i;
              for(i=0;i<10;i++){
                int status;
                switch(waitpid(new_pid,&status,WNOHANG)){
                  case 0:
                    Sleep(1000);
                    continue;
                  case -1:
                    Log(_T("Failed to wait for gdb process to die\n"));
                    SetStatus(TimeOut);
                    break;
                  default:
                    break;
                }
                break;
              }
              if(10==i){
                Log(_T("Failed to wait for gdb process to die\n"));
                SetStatus(TimeOut);
              } else {
                TRACE(_T("Killed gdb\n"));
              }
            } else {
              Log(_T("Failed to run ps to kill gdb process %d\n"),new_pid);
            }
          }
          TRACE(_T("Total elapsed time is %d\n"), m_nTotalTime);
        }
        break;
      }
      VTRACE(_T("Closing pipe_ends_r[]\n"));
      close (pipe_ends_r[0]);
      close (pipe_ends_r[1]);
    }
    close (pipe_ends_w[0]);
    close (pipe_ends_w[1]);
  }
}

bool CeCosTest::GdbProcessAlive()
{
  int status;
  return 0==waitpid((int)m_pGdbProcesshandle,&status,WNOHANG);
}

Time CeCosTest::GdbCpuTime()
{
  if(GdbProcessAlive()){
    Time now=Now();
    if(now-m_tPrevSample>1000){
      
      m_tPrevSample=now;
      // Output is in the form
      //  F S   UID   PID  PPID  C PRI  NI ADDR    SZ WCHAN  TTY          TIME CMD
      //100 S   490   877   876  0  70   0    -   368 wait4  pts/0    00:00:00 bash
      FILE *f=popen(_T("ps -l"),_T("r"));
      if(f){
        TCHAR buf[100];
        int t=0;
        while(_fgetts(buf,sizeof(buf)-1,f)){
          int F,UID,PID,PPID,C,PRI,NI,SZ,HH,MM,SS;
          TCHAR discard[100];
TRACE(_T("ps -l : %s\n"),buf);
          if(15==_stscanf(buf,
            _T("%d %s %d %d %d %d %d %d %s %d %s %s %d:%d:%d"),&F,discard,&UID,&PID,&PPID,&C,&PRI,&NI,discard,&SZ,discard,discard,&HH,&MM,&SS) &&
            (PID==(int)m_pGdbProcesshandle || PPID==(int)m_pGdbProcesshandle)){
            t+=SS+60*(60*HH+MM);
          }
        }
        pclose(f);
TRACE(_T("---> t=%d [secs]\n"),t);
        m_tGdbCpuTime=Time(1000*t);
      }
    }
  } else {
    m_tGdbCpuTime=0;
  }
VTRACE(_T("GdbCpuTime rc=%d"),m_tGdbCpuTime/1000);
  return m_tGdbCpuTime;
}

bool CeCosTest::WritePipe (const String &str)
{
  const char *pszBuf=(const char *)str;
  int write_fd = (int)m_wPipeHandle;
  int dwWritten;
  int nToWrite=_tcslen(pszBuf);
  do {
    dwWritten = write(write_fd, pszBuf, nToWrite);
    if(-1==dwWritten){
      Log(_T("pipe write error - %s\n"),strerror(errno));
      return false;
    }
    nToWrite-=(int)dwWritten;
    pszBuf+=(int)dwWritten;
    if(!CheckForTimeout()){
      return false;
    }
  } while (nToWrite>0);
  return true;
}

int CeCosTest::ReadPipe (String &str,bool bBlocking /* This param ignored */)
{
  TCHAR buf[4096];
  int rc=read((int)m_rPipeHandle, buf, sizeof(buf)-1);
  if(-1==rc && EAGAIN==errno){
    rc=0;
  }
  buf[MAX(0,rc)]=_TCHAR('\0');
  str=buf;
  return rc;
}

void CeCosTest::GetPath(String &strPath)
{
  strPath=getenv(_T("PATH"));
}


void CeCosTest::SetPath(const String &strPath)
{
  String str;
  str.Format(_T("PATH=%s"),(LPCTSTR )strPath);
  putenv(str);
}
#endif

void CeCosTest::AnalyzeOutput()
{
  // This test is pulled out to allow ser_filter to simulate a test failure
  if(OutputContains(_T("FAIL:"))){
    SetStatus(Fail);
  }
  
  if(OutputContains(_T("EXIT:"))||OutputContains(_T("NOTAPPLICABLE:"))){
    static LPCTSTR arpszKeepAlive[]={_T("FAIL:"),_T("NOTAPPLICABLE:"), _T("PASS:")}; 
    static const StatusType arStatus[] ={Fail, Inapplicable, Pass};
    for(unsigned int i=0;i<sizeof arpszKeepAlive/sizeof arpszKeepAlive[0];i++){
      if(OutputContains(arpszKeepAlive[i])){
        TRACE(_T("DriveGdb: saw '%s'\n"),arpszKeepAlive[i]);
        SetStatus(arStatus[i]); // Do not break!
      }
    }
  }
  
  // Certain output spells failure...
  if(OutputContains(_T("cyg_assert_fail ("))){
    SetStatus(AssertFail);
  } else {
    static LPCTSTR arpszSignals[]={_T("SIGBUS"), _T("SIGSEGV"), _T("SIGILL"), _T("SIGFPE"), _T("SIGSYS"), _T("SIGTRAP")};
    for(unsigned int i=0;i<sizeof arpszSignals/sizeof arpszSignals[0];i++){
      String str1,str2;
      str1.Format(_T("signal %s"),arpszSignals[i]);
      str2.Format(_T("handle %s nostop"),arpszSignals[i]);
      if(OutputContains(str1)&&!OutputContains(str2)){
        SetStatus(Fail);
        break;
      }
    }
  }
  
  // Check for expect: strings
  static LPCTSTR szExpect=_T("EXPECT:");
  static const int nExpectLen=_tcslen(szExpect);
  for(const TCHAR*c=_tcsstr(m_strOutput,szExpect);c;c=_tcsstr(c,szExpect)){
    c+=nExpectLen;
    if(_TCHAR('<')==*c){
      c++;
      // Find the matching _TCHAR('>')
      for(LPCTSTR d=_tcschr(c,_TCHAR('>'));d;d=_tcschr(d+1,_TCHAR('>'))){
        if(d[-1]!=_TCHAR('\\')){
          
        /*
        // Skip whitespace immediately following the EXPECT:<...>
        do {
        d++;
        } while (_istspace(*d));
          */
          
          /*
          // Skip timestamp
          if(_TCHAR('<')==*d){
          d=_tcschr(d,_TCHAR('>'));
          if(0==d){
          continue;
          }
          do {
          d++;
          } while (_istspace(*d));
          }
          */
          
          // Now d points to the terminating _TCHAR('>') and c at the start of the expected string
          for(LPCTSTR e=d+1;c!=d;c++,e++){
            if(_TCHAR('\\')==*c){
              c++;
            }
            if(*c!=*e){
              LogString(_T("EXPECT:<> failure\n"));
              SetStatus(Fail);
              break;
            }
          }
          break;
        }
      }
    }
  }
}

bool CeCosTest::ExecutionParameters::FromStr(LPCTSTR psz)
{
  String str1,str2,str3,str4,str5;
  int nUseFilter,nUnused2,nUnused3;
  int nLen=_tcslen(psz);
  _stscanf(psz,_T("%s %s %d %d %d %d %d %d %d %d %s %s %s"),
    str1.GetBuffer(1+nLen),
    str2.GetBuffer(1+nLen),
    &m_nActiveTimeout,
    &m_nDownloadTimeout,
    &m_nUnused1,
    &m_nUnused2,
    &m_nUnused3,
    &nUseFilter,
    &nUnused2,
    &nUnused3,
    str3.GetBuffer(1+nLen),
    str4.GetBuffer(1+nLen),
    str5.GetBuffer(1+nLen)
    );
  m_bUseFilter=(0!=nUseFilter);            
  m_bUnused2=(0!=nUnused2);            
  m_bUnused3=(0!=nUnused3);            
  str1.ReleaseBuffer();
  str2.ReleaseBuffer();
  str3.ReleaseBuffer();
  str4.ReleaseBuffer();
  str5.ReleaseBuffer();
  m_Target=str1;
  int r;
  for(r=0;r<RequestTypeMax;r++){
    if(0==_tcscmp(arRequestImage[r],str2)){
      break;
    }
  }
  m_Request=(RequestType)r;
  return CeCosTest::IsValid(m_Target);
}

CeCosTest::ExecutionParameters::ExecutionParameters (RequestType r,
                                                     LPCTSTR  Target,
                                                     Duration    nActiveTimeout/*=NOTIMEOUT*/,
                                                     Duration    nDownloadTimeout/*=NOTIMEOUT*/):
  m_bUseFilter(true),
  m_Target(Target),
  m_nActiveTimeout(nActiveTimeout),
  m_nDownloadTimeout(nDownloadTimeout),
  m_Request(r),
  m_nUnused1(0),
  m_nUnused2(0),
  m_nUnused3(0),
  m_bUnused2(false),
  m_bUnused3(false)
{
}

String CeCosTest::ExecutionParameters::Image() const
{
  String str;
  str.Format(_T("%s %s %d %d %d %d %d %d %d %d"),Target(),Image(Request()),ActiveTimeout(),DownloadTimeout(),
    m_nUnused1,
    m_nUnused2,
    m_nUnused3,
    m_bUseFilter,
    m_bUnused2,
    m_bUnused3);
  return str;
}

CeCosTest::TargetInfo::TargetInfo()
{
};

LPCTSTR CeCosTest::TargetInfo::arHwTypeImage[]={_T("HARDWARE"), _T("SIM"), _T("SYNTHETIC"), _T("HARDWARE_NO_BP"), _T("REMOTE_SIM")
};

CeCosTest::TargetInfo::TargetInfo(LPCTSTR pszIm,LPCTSTR pszPre,int nHwtype,LPCTSTR pszGdb/*=_T("")*/):
  pszImage(pszIm),
  pszPrefix(pszPre),
  nType((HwType)nHwtype),
  pszGdbcmd(pszGdb)
{
}

CeCosTest::TargetInfo::TargetInfo(LPCTSTR pszIm,LPCTSTR pszPre,LPCTSTR pszHwtype,LPCTSTR pszGdb/*=_T("")*/):
  pszImage(pszIm),
  pszPrefix(pszPre),
  nType(INVALID),
  pszGdbcmd(pszGdb)
{
  if(_istdigit(pszHwtype[0])){
    nType=(HwType)_ttoi(pszHwtype);
    if((unsigned)nType>sizeof(arHwTypeImage)){
      nType=INVALID;
    }
  } else {
    for(unsigned int i=0;i<sizeof arHwTypeImage/sizeof arHwTypeImage[0];i++){
      if(0==_tcsicmp(pszHwtype,arHwTypeImage[i])){
        nType=(HwType)i;
        break;
      }
    }
  }
}

int CeCosTest::AddPlatform(const CeCosTest::TargetInfo &t)
{
  arTargetInfo.push_back(t);
  return arTargetInfo.size()-1;
}

bool CeCosTest::GetTargetReady(String &strHostPort)
{
  bool rc=false;
  int nTargetReady;
  do{
    if(!m_pSock->recvInteger(nTargetReady,_T("Target ready"),120*1000)){
      Log(_T("Failed to read target ready indicator from server - %s\n"),(LPCTSTR )m_pSock->SocketErrString());
      break;
    }
    switch(nTargetReady){
    case 0:
      LogString(_T("Failed to reset target"));
      break;
    case 1:
      if(m_pSock->recvString(strHostPort, _T("host:port"))){
        TRACE(_T("Instructed to use %s\n"),(LPCTSTR )strHostPort);
        rc=true;
      } else {
        Log(_T("Failed to read host:port - %s\n"),(LPCTSTR )m_pSock->SocketErrString());
      }
      break;
    case 2:
      {
        String strOutput;
        if(m_pSock->recvString(strOutput, _T("output"))){
          LogString(strOutput);               
        } else {
          Log(_T("Failed to read output\n"),(LPCTSTR )m_pSock->SocketErrString());
          return false;
        }
      }
      break;
    }
  } while(2==nTargetReady);
  return rc;
}

#ifdef _WIN32
String CeCosTest::GetGreatestSubkey (LPCTSTR pszKey)
{
  String strSubkey = _T("");
  HKEY hKey;
  
  if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, pszKey, 0L, KEY_READ, &hKey))
  {
    DWORD dwIndex = 0;
    TCHAR pszBuffer [MAX_PATH + 1];
    
    while (ERROR_SUCCESS == RegEnumKey (hKey, dwIndex++, (LPTSTR) pszBuffer, sizeof (pszBuffer)))
    {
      if (strSubkey.compare (pszBuffer) < 0)
        strSubkey = pszBuffer;
    }
    
    RegCloseKey (hKey);
  }
  
  TRACE (_T("CeCosTest::GetGreatestSubkey(\"%s\"): %s\n"), pszKey, (LPCTSTR)strSubkey);
  return strSubkey;
}
#endif


void CeCosTest::RemoveAllPlatforms()
{
  for(int i=TargetTypeMax()-1;i>=0;--i){
    delete &Target(i);
  }
  arTargetInfo.clear();
}

CeCosTest::ServerStatus CeCosTest::ServerStatusValue(LPCTSTR psz)
{
  int s;
  for(s=0;s<ServerStatusMax;s++){
    if(0==_tcsicmp(psz,arServerStatusImage[s])){
      break;
    }
  }
  return (ServerStatus)s;

}

const CeCosTest::TargetInfo &CeCosTest::Target (LPCTSTR psz) 
{ 
  for(int i=0;i<(signed)arTargetInfo.size();i++){
    const TargetInfo &t=arTargetInfo[i];
    if(0==_tcsicmp(t.Image(),psz)){
      return t;
    }
  }
  return tDefault;
}
