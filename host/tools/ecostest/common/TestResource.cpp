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
//        TestResource.cpp
//
//        Resource test class
//
//=================================================================
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     sdf
// Contributors:  sdf
// Date:          1999-04-01
// Description:   This class abstracts a test resource for use in the testing infrastructure
// Usage:
//
//####DESCRIPTIONEND####
#include "eCosStd.h"
#include "eCosTestUtils.h"
#include "eCosTrace.h"
#ifdef _WIN32
  #include "Subprocess.h"
#endif
#include "TestResource.h"

CTestResource *CTestResource::pFirstInstance=0;
unsigned int CTestResource::nCount=0;

String CTestResource::strResourceHost;
int CTestResource::nResourcePort;

CTestResource::CTestResource(LPCTSTR target, LPCTSTR  pszDownloadPort, int nBaud, LPCTSTR pszResetString):
  m_strReset(pszResetString),
  m_bInUse(false),
  m_nBaud(nBaud),
  m_strPort(pszDownloadPort),
  m_bLocked(false),
  m_Target(target),
  m_strHost(CeCosTestUtils::HostName()),
  m_nPort(0)
{
  VTRACE(_T("@@@ Created resource %08x %s\n"),(unsigned int)this,(LPCTSTR)Output());
  Chain();
}

CTestResource::~CTestResource()
{
  ENTERCRITICAL;
  VTRACE(_T("@@@ Destroy resource %08x %s\n"),this,(LPCTSTR)Output());
  if(m_pPrevInstance || m_pNextInstance){
    nCount--;
  }
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

// Find the resource matching the given host:port specification
// Returns 0 if no such host:port found
CTestResource * CTestResource::Lookup(LPCTSTR  pszHost, int nPort)
{
  CTestResource *pResource;
  ENTERCRITICAL;
  for(pResource=pFirstInstance;pResource;pResource=pResource->m_pNextInstance){
    if(nPort==pResource->TcpipPort() && 0==_tcsicmp(pszHost,pResource->Host())){
      break;
    }
  }
  LEAVECRITICAL;
  return pResource;
}

unsigned int CTestResource::GetMatchCount (const CeCosTest::ExecutionParameters &e,bool bIgnoreLocking)
{
  unsigned int i=0;
  ENTERCRITICAL;
  for(const CTestResource *pResource=pFirstInstance;pResource;pResource=pResource->m_pNextInstance){
    if(pResource->Matches(e,bIgnoreLocking)){
      i++;
    }
  }
  LEAVECRITICAL;
  return i;
}

bool CTestResource::GetMatches (const CeCosTest::ExecutionParameters &e, StringArray &arstr, bool bIgnoreLocking)
{
  bool rc=false;
  arstr.clear();
  if(LoadSocket()){
    ENTERCRITICAL;
    for(CTestResource *pResource=pFirstInstance;pResource;pResource=pResource->m_pNextInstance){
		    if(pResource->Matches(e,bIgnoreLocking)){
		        arstr.push_back(CeCosTestSocket::HostPort(pResource->Host(),pResource->TcpipPort()));
        }
    }
    LEAVECRITICAL;
    rc=true;
  }
  return rc;
}

void CTestResource::DeleteAllInstances()
{
  ENTERCRITICAL;
  while(pFirstInstance){
    delete pFirstInstance;
  }
  LEAVECRITICAL;
}

bool CTestResource::LoadFile (LPCTSTR psz,void *key)
{
  bool rc=true;
  ENTERCRITICAL;
  DeleteAllInstances();
#ifdef _WIN32
  if(key){
    // Find all the keys under "psz" and load from each of them
    HKEY hKey;
    if(ERROR_SUCCESS==RegOpenKeyEx ((HKEY)key, psz, 0L, KEY_ENUMERATE_SUB_KEYS, &hKey)){
		    TCHAR szName[256];
        DWORD dwSizeName=sizeof szName;
        FILETIME ftLastWriteTime;
        for(DWORD dwIndex=0;ERROR_SUCCESS==RegEnumKeyEx(hKey, dwIndex, szName, &dwSizeName, NULL, NULL, NULL, &ftLastWriteTime); dwIndex++){
          CTestResource *pResource=new CTestResource(_T(""));
          String strKey;
          strKey.Format(_T("%s\\%s"),psz,szName);
          CTestResourceProperties prop1(pResource,strKey,key);
          prop1.SetDefaults();
          prop1.Load();
          dwSizeName=sizeof szName;
        }
        RegCloseKey(hKey);
    }
  } else
#endif
  {
    // Find all the files in directory "psz" and load from each of them
    void *pHandle;
    TCHAR szOrigDir[256];
    _tgetcwd(szOrigDir,sizeof szOrigDir-1);
    if(0==_tchdir(psz)){
      String strFile;
      for(bool b=CeCosTestUtils::StartSearch(pHandle,strFile);b;b=CeCosTestUtils::NextFile(pHandle,strFile)){
        struct _stat buf;
        if(0==_tstat(strFile,&buf) && 0==(S_IFDIR&buf.st_mode)){
          CTestResource *pResource=new CTestResource(_T(""));
          CTestResourceProperties prop1(pResource,strFile);
          prop1.SetDefaults();
          prop1.Load();
        }
      }
      CeCosTestUtils::EndSearch(pHandle);
    } else {
      TRACE(_T("Failed to change to %s from %s\n"),psz,szOrigDir);
    }
    _tchdir(szOrigDir);
  }
  LEAVECRITICAL;
  
  return rc;
}

bool CTestResource::SaveFile (LPCTSTR psz,void *key)
{
  bool rc=false;    
  ENTERCRITICAL;
#ifdef _WIN32
  if(key){
    // Delete all the keys under "psz"
    HKEY hKey;
    if(ERROR_SUCCESS==RegOpenKeyEx ((HKEY)key, psz, 0L, KEY_ENUMERATE_SUB_KEYS, &hKey)){
		    TCHAR szName[256];
        DWORD dwSizeName=sizeof szName;
        FILETIME ftLastWriteTime;
        DWORD dwIndex;
        if(ERROR_SUCCESS==RegQueryInfoKey(hKey,0,0,0,&dwIndex,0,0,0,0,0,0,0)){
          while((signed)--dwIndex>=0){
            if(ERROR_SUCCESS!=RegEnumKeyEx(hKey, dwIndex, szName, &dwSizeName, NULL, NULL, NULL, &ftLastWriteTime) ||
              ERROR_SUCCESS!=RegDeleteKey(hKey,szName)){
              rc=false;
            }
            dwSizeName=sizeof szName;
		        }
        }
        RegCloseKey(hKey);
    }
  } else
#endif
  {
    // Delete all the files under directory "psz"
    void *pHandle;
    TCHAR szOrigDir[256];
    _tgetcwd(szOrigDir,sizeof szOrigDir-1);
    if(0==_tchdir(psz)){
      String strFile;
      for(bool b=CeCosTestUtils::StartSearch(pHandle,strFile);b;b=CeCosTestUtils::NextFile(pHandle,strFile)){
        struct _stat buf;
        if(0==_tstat(strFile,&buf) && 0==(S_IFDIR&buf.st_mode)){
          _tunlink(strFile);
        }
      }
      CeCosTestUtils::EndSearch(pHandle);
    } else {
      fprintf(stderr,"Failed to change to %s from %s\n",psz,szOrigDir);
    }
    _tchdir(szOrigDir);
  }
  
  rc=true;
  for(CTestResource *pResource=pFirstInstance;pResource;pResource=pResource->m_pNextInstance){
    String strName;
    if(key){
      strName.Format(_T("%s\\%s-%d"),psz,(LPCTSTR)pResource->m_strHost,pResource->m_nPort);
      CTestResourceProperties prop1(pResource,strName,key);
      rc&=prop1.Save();
    } else {
      strName.Format(_T("%s%c%s-%d"),psz,cPathsep,(LPCTSTR)pResource->m_strHost,pResource->m_nPort);
      CTestResourceProperties prop1(pResource,strName,key);
      rc&=prop1.Save();
    }
    
  }
  LEAVECRITICAL;
  
  return rc;
}

CTestResource::CTestResourceProperties::CTestResourceProperties(CTestResource *pResource,LPCTSTR psz,void *key):
  CProperties(psz,key)
{
  CProperties::Add(_T("Baud"),       pResource->m_nBaud,0);
               Add(_T("BoardId"),    pResource->m_strBoardID);
               Add(_T("Date"),       pResource->m_strDate);
               Add(_T("Email"),      pResource->m_strEmail);
               Add(_T("Host"),       pResource->m_strHost);
  CProperties::Add(_T("Locked"),     pResource->m_bLocked,0);
               Add(_T("Originator"), pResource->m_strUser);
  CProperties::Add(_T("Port"),       pResource->m_nPort);
               Add(_T("Reason"),     pResource->m_strReason);
               Add(_T("Reset"),      pResource->m_strReset);
               Add(_T("Serial"),     pResource->m_strPort);
               Add(_T("Target"),     pResource->m_Target);
               Add(_T("User"),       pResource->m_strUser);
}

bool CTestResource::Lock()
{
  if(!m_bLocked){
    m_bLocked=true;
    return true;
  } else {
    return false;
  }
}

bool CTestResource::Unlock()
{
  if(m_bLocked){
    m_bLocked=false;
    return true;
  } else {
    return false;
  }
}

bool CTestResource::LoadSocket(LPCTSTR pszResourceHost,int nResourcePort,Duration dTimeout/*=10*1000*/)
{
  bool rc=false;
  ENTERCRITICAL;
  CTestResource *pResource;
  // If the resource is in use, we don't dare delete it!
  for(pResource=CTestResource::First();pResource;pResource=pResource->Next()){
    pResource->m_bFlag=false;
  }
  CeCosTestSocket sock;
  if(sock.Connect(pszResourceHost,nResourcePort,dTimeout)){
    // Write the message to the socket
    int nRequest=0; // read
    if(!sock.sendInteger(nRequest)){
      ERROR(_T("Failed to write to socket\n"));
    } else {
      int nResources;
      if(sock.recvInteger(nResources,_T(""),dTimeout)){
        rc=true;
        while(nResources--){
          String strImage;
          if(sock.recvString(strImage,_T(""),dTimeout)){
            VTRACE(_T("Recv \"%s\"\n"),(LPCTSTR)strImage);
            CTestResource tmp;
            tmp.FromStr(strImage);
            CTestResource *pResource=Lookup(tmp.Host(),tmp.TcpipPort());
            if(0==pResource){
              pResource=new CTestResource(_T(""));
            }
            pResource->FromStr(strImage);
            pResource->m_bFlag=true;
          } else {
            rc=false;
            break;
          }
        }
      }
    }
  }
  // If the resource is in use, we don't dare delete it!
  CTestResource *pNext;
  for(pResource=CTestResource::First();pResource;pResource=pNext){
    pNext=pResource->Next();
    if(!pResource->m_bFlag && !pResource->m_bInUse){
      delete pResource;
    }
  }
  
  LEAVECRITICAL;
  return rc;
}

void CTestResource::Image(String &str)
{
  CTestResourceProperties prop(this);
  str=prop.MakeCommandString();
  VTRACE(_T("Make command string %s\n"),(LPCTSTR)str);
}

bool CTestResource::FromStr(LPCTSTR pszImage)
{
  CTestResourceProperties prop(this);
  prop.LoadFromCommandString(pszImage);
  VTRACE(_T("From command string %s\n"),pszImage);
  return true;
}

void CTestResource::Chain()
{
  ENTERCRITICAL;
  nCount++;
  m_pNextInstance=pFirstInstance;
  if(m_pNextInstance){
    m_pNextInstance->m_pPrevInstance=this;
  }
  m_pPrevInstance=0;
  pFirstInstance=this;
  LEAVECRITICAL;
}

bool CTestResource::Matches  (const CeCosTest::ExecutionParameters &e,bool bIgnoreLocking) const
{
  return (bIgnoreLocking||(!m_bLocked)) && (0==_tcsicmp(e.Target(),m_Target)); 
};

bool CTestResource::SaveSocket(LPCTSTR pszResourceServer,int nResourcePort,Duration dTimeout)
{
  bool rc=true;
  ENTERCRITICAL;
  CeCosTestSocket sock(pszResourceServer,nResourcePort, dTimeout);
  if(sock.Ok()){
    // Write the message to the socket
    int nRequest=1; //write
    if(!sock.sendInteger(nRequest, _T(""),dTimeout)){
      ERROR(_T("Failed to write to socket\n"));
      rc=false;
    } else {
      int nResources=0;
      CTestResource *pResource;
      for(pResource=CTestResource::First();pResource;pResource=pResource->Next()){
        nResources++;
      }
      if(sock.sendInteger(nResources,_T("resource count"),dTimeout)){
        for(pResource=CTestResource::First();pResource;pResource=pResource->Next()){
          String strImage;
          pResource->Image(strImage);
          TRACE(_T("Send \"%s\"\n"),(LPCTSTR)strImage);
          if(!sock.sendString (strImage, _T("reply"),dTimeout)){
            rc=false;
            break;
          }
        }
      } else {
        rc=false;
      }
    }
  } else {
    rc=false;
  }
  LEAVECRITICAL;
  return rc;
}

CeCosTest::ServerStatus CTestResource::Query() 
{
  CeCosTest::ExecutionParameters e(CeCosTest::ExecutionParameters::QUERY,m_Target);
  CeCosTestSocket *pSock=0;
  CeCosTest::ServerStatus s=CeCosTest::Connect(m_strHost,m_nPort,pSock,e,m_strInfo);
  delete pSock;
  return s;
}

int CTestResource::Count(const CeCosTest::ExecutionParameters &e)
{
  int nCount=0;
  for(const CTestResource *p=pFirstInstance;p;p=p->m_pNextInstance){
    if(p->Matches(e)){
      nCount++;
    }
  }
  return nCount;
}

bool CTestResource::Use()
{
  bool rc;
  ENTERCRITICAL;
  if(m_bInUse){
    rc=false;
  } else {
    m_bInUse=true;
    rc=true;
  }
  LEAVECRITICAL;
  return rc;
}

CTestResource *CTestResource::GetResource (const CeCosTest::ExecutionParameters &e)
{
  CTestResource *p=0;
  if(0==Count(e)){
    ERROR(_T("GetResource: no candidates available\n"));
  } else {
    ENTERCRITICAL;
    for(p=pFirstInstance;p;p=p->m_pNextInstance){
      if(p->Matches(e) && !p->m_bInUse){
        TRACE(_T("Acquired %s\n"),p->Serial());
				    p->Use();
            break;
      }
    }
    LEAVECRITICAL;
  }
  return p;
}

String CTestResource::Output() const
{
  String str;
  str.Format(
    _T("%10s:%d %20s %8s"),
    Host(),
    TcpipPort(),
    Target(),
    Serial());
  if(HasReset()){
    str+=_T(" Reset:");
    str+=m_strReset;
  }
  if(IsLocked()){
    str+=_T(" [RL]");
  }
  return str;
}

bool CTestResource::Matches(LPCTSTR pszHost, int nPort, const CeCosTest::ExecutionParameters &e)
{
  bool rc=false;
  ENTERCRITICAL;
  if(LoadSocket()){
    CTestResource *pResource=Lookup(pszHost,nPort);
    if(pResource){
      rc=pResource->Matches(e);
    }
  }
  LEAVECRITICAL;
  return rc;
}

CResetAttributes::ResetResult CTestResource::Reset(LogFunc *pfnLog, void *pfnLogparam)
{
  String strReset;
  strReset.Format(_T("port(%s,%d) %s"),Serial(),Baud(),ResetString());
  return CResetAttributes(strReset).Reset(pfnLog,pfnLogparam);
}

CResetAttributes::ResetResult CTestResource::Reset(String &str)
{
  return Reset(StringLogFunc,&str);
}

void CALLBACK CTestResource::StringLogFunc (void *pParam,LPCTSTR psz)
{
  *((String *)pParam)+=psz;
}


CResetAttributes::ResetResult CTestResource::RemoteReset(LogFunc *pfnLog, void *pfnLogparam)
{
  String strCmd;
  strCmd.Format(_T("rsh %s x10reset %s\n"),Host(),ResetString());
  pfnLog(pfnLogparam,strCmd);

#ifdef _WIN32
  // because this can be used from GUI tools
  CSubprocess sp;
  sp.Run(pfnLog,pfnLogparam,strCmd);
#else // UNIX
  FILE *f=POPEN(strCmd,"rt");
  if(f){
    char buf[80];
    while(fgets(buf,sizeof buf,f)){
      pfnLog(pfnLogparam,buf);
    }
    PCLOSE(f);
  } else {
    String strMsg;
    strMsg.Format(_T("Failed to execute '%s'\n"),(LPCTSTR)strCmd);
    pfnLog(pfnLogparam,strMsg);
  }
#endif
  return CResetAttributes::RESET_OK; // FIXME
}

