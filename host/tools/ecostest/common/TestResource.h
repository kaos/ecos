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
//        TestResource.h
//
//        Test resource class
//
//=================================================================
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     sdf
// Contributors:  sdf
// Date:          1999-04-01
// Description:   This class abstracts test resources for use in the testing infrastructure
// Usage:
//
//####DESCRIPTIONEND####

#ifndef _TESTRESOURCE_H
#define _TESTRESOURCE_H
#include "eCosStd.h"
#include "eCosTest.h"
#include "eCosTestSerial.h"
#include "Properties.h"
#include "ResetAttributes.h"

// This class is used to manipulate test resources.  A test resource is the means to execute
// a test (a test agent running on a tcp/ip port)
class CTestResource {
public:

  class CTestResourceProperties : public CProperties {
  public:
    CTestResourceProperties(CTestResource *pResource,LPCTSTR psz=0,void *key=0);
    virtual ~CTestResourceProperties(){}
    void Add(LPCTSTR pszName,String &str,LPCTSTR pszDefault=_T("")) {CProperties::Add(pszName,(void *)&str,GetFn,PutFn,pszDefault);}
  protected:
    static LPCTSTR CALLBACK GetFn (void *pObj) { return (LPCTSTR)*(String *)pObj; }
    static void    CALLBACK PutFn (void *pObj,LPCTSTR psz) { *(String *)pObj=psz; }
  };
  friend class CTestResourceProperties;

  // ctors/dtors
  CTestResource(
    LPCTSTR target, 
    // Physical port characteristics.  If pszPort is null, simulator run is meant (in which case nBaud is ignored)
    LPCTSTR pszPort=0, int nBaud=0,
    // Associated reset characteristics: pszPort may be a remote server (host:port) or a local port
    LPCTSTR pszResetString=_T("")
    );
  virtual ~CTestResource();
  
  bool Use();
  
  CResetAttributes::ResetResult RemoteReset(LogFunc *pfnLog, void *pfnLogparam=0);
  String Output() const;
  
  static void UnlockResources(LPCTSTR pszFile,int nLine);
  static void LockResources (LPCTSTR pszFile,int nLine);
  void Image (String &str);
  static bool SaveSocket (Duration dTimeout=10*1000) { return SaveSocket(strResourceHost,nResourcePort,dTimeout); }
  static bool LoadSocket (Duration dTimeout=10*1000) { return LoadSocket(strResourceHost,nResourcePort,dTimeout); }
  
  CeCosTest::ServerStatus Query();
  bool Unlock();
  bool Lock ();
  static CTestResource *First() { return pFirstInstance; }
  CTestResource *Next() const { return m_pNextInstance; }
  static CTestResource * Lookup(LPCTSTR  pszHost, int nPort);
  static unsigned int ResourceCount() { return nCount; }
  static void DeleteAllInstances();
  bool IsLocked() const { return m_bLocked; }
  static bool LoadFile (LPCTSTR psz,void *key=0); // Load information from the registry or a file
  static bool SaveFile (LPCTSTR psz,void *key=0); // Save information to the registry or a file
  
  void SetInfo (LPCTSTR pszInfo) { m_strInfo=pszInfo; }
  LPCTSTR  Info() const { return m_strInfo; }
  
  // Attributes
  LPCTSTR Host() const { return m_strHost; }
  int		    TcpipPort() const { return m_nPort; }
  LPCTSTR Serial() const { return m_strPort; }
  
  LPCTSTR Target() const { return m_Target; }

  bool HasReset() const { return m_strReset.GetLength()>0; }
  
  int Baud() const { return m_nBaud; }							// Baud rate
  
  void SetHostPort(LPCTSTR pszHost,int nPort) { m_strHost=pszHost;m_nPort=nPort; }
  
  void SetTarget	(LPCTSTR target) { m_Target=target; }
  void SetDownload(LPCTSTR pszDownloadPort,int nBaud) { m_strPort=pszDownloadPort; m_nBaud=nBaud; }
  void SetReset  (LPCTSTR pszReset) { m_strReset=pszReset; }
  void SetUser   (LPCTSTR pszUser,LPCTSTR pszEmail) {m_strUser=pszUser; m_strEmail=pszEmail; }
  void SetReason (LPCTSTR pszReason) {m_strReason=pszReason; }
  void SetBoardID(LPCTSTR pszBoardID){m_strBoardID=pszBoardID; }
  void SetDate   (LPCTSTR pszDate)   {m_strDate   =pszDate; }

  LPCTSTR Email  () { return m_strEmail; }
  LPCTSTR User   () { return m_strUser; }
  LPCTSTR Reason () { return m_strReason; }
  LPCTSTR BoardID() { return m_strBoardID; }
  LPCTSTR Date   () { return m_strDate; }
  
  // Scheduling functions
  // Get the array of resources capable of executing "e"
  // nCount gives the number of entries in ar on input
  // result is the number of entries required in ar (may exceed nCount, but this case does no damage)
  static unsigned int GetMatchCount (const CeCosTest::ExecutionParameters &e,bool bIgnoreLocking=false);
  static bool GetMatches (const CeCosTest::ExecutionParameters &e,StringArray &arstr,bool bIgnoreLocking=false); // as before, but callee allocates.  Deallocate using delete [].
  
  static void SetResourceServer (LPCTSTR pszHost,int nPort){ strResourceHost=pszHost;nResourcePort=nPort; }
  
  static CTestResource *GetResource(const CeCosTest::ExecutionParameters &e);
  void Release() {
    //TRACE(_T("Release %s\n"),Serial1());
    m_bInUse=false;
  }
  

  static int Count (const CeCosTest::ExecutionParameters &e);
  
  // Reset the hardware attached to this port.  Output goes to pfnLog
  CResetAttributes::ResetResult Reset(LogFunc *pfnLog=0, void *pfnLogparam=0);
  CResetAttributes::ResetResult Reset(String &str); // as above, output to string

  static bool Matches(LPCTSTR pszHost, int nPort, const CeCosTest::ExecutionParameters &e);
  bool FromStr(LPCTSTR pszImage);
  LPCTSTR ResetString() const { return m_strReset; }
protected:
  CTestResource():m_pNextInstance(0),m_pPrevInstance(0){} // not chained or initialized - for internal use only
  String m_strReason,m_strUser,m_strEmail,m_strBoardID, m_strDate;

  String m_strReset;

  static void CALLBACK StringLogFunc (void *pParam,LPCTSTR psz);

  static bool LoadSocket (LPCTSTR pszHost,int nPort,Duration dTimeout=10*1000);
  static bool SaveSocket (LPCTSTR pszHost,int nPort,Duration dTimeout=10*1000);
  static LPCTSTR szFormat;
  
  void Chain();
  
  unsigned static int nCount;
  static String strResourceHost;
  static int nResourcePort;
  static CTestResource *pFirstInstance;
  
  bool Matches  (const CeCosTest::ExecutionParameters &e,bool bIgnoreLocking=false) const;
  

  String m_strInfo;
  bool m_bFlag;
  bool m_bInUse;
  int  m_nBaud;
  String m_strPort;
  
  bool m_bLocked;
  
  String m_Target;
  String m_strHost;
  int m_nPort;
  CTestResource *m_pNextInstance;
  CTestResource *m_pPrevInstance;
};
#endif
