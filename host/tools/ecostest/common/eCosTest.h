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
//        eCosTest.h
//
//        run test header
//
//=================================================================
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     sdf
// Contributors:  sdf
// Date:          1999-04-01
// Description:   Run one or more tests
// Usage:
//
//####DESCRIPTIONEND####

#ifndef _ECOSTEST_H
#define _ECOSTEST_H
// eCos testing infrastructure
// This class represents a single eCos test [executable].
// It includes member functions to run the test and to manage
// related system resources.

#include "eCosStd.h"
#include "eCosTestUtils.h"
#include "eCosThreadUtils.h"
#include "Properties.h"
#include "ResetAttributes.h"
#include <vector>

class CTestResource;
class CeCosTestSocket;
class CeCosTest{
public:

  static bool Init();
  static void Term();

	static bool SaveTargetInfo();
  class TargetInfo {
    public:
      // Update arHwTypeImage if you add to this enum
      enum HwType {INVALID=-1, HARDWARE=0, SIM=1, SYNTHETIC=2, HARDWARE_NO_BP=3, REMOTE_SIM=4
      };
    protected:
      String pszImage;
      String pszPrefix;
      HwType nType;
      String pszGdbcmd;
    public:
      static LPCTSTR arHwTypeImage[];
      static HwType FromStr (LPCTSTR psz);
      bool IsValid()   const { return nType!=INVALID; }
      LPCTSTR Image()  const { return pszImage.c_str(); }
      LPCTSTR Prefix() const { return pszPrefix.c_str(); }
      LPCTSTR GdbCmd() const { return pszGdbcmd.c_str(); }
      HwType Type()     const { return nType; }
      TargetInfo();
      TargetInfo(LPCTSTR pszIm,LPCTSTR pszPre,LPCTSTR pszHwtype,LPCTSTR pszGdb=0);
      TargetInfo(LPCTSTR pszIm,LPCTSTR pszPre,int     nHwtype,LPCTSTR pszGdb=0); // FIXME: phase this out as soon as we can
  };
  
  static int AddPlatform (const TargetInfo &t);
  static int InitTargetInfoReg  (LPCTSTR szRegKey);
  static bool SaveTargetInfoReg (LPCTSTR szRegKey);
  static LPCTSTR pszFormat;
  
  ///////////////////////////////////////////////////////////////////////////
  // Representation of an elapsed time (units of milliseconds)
  enum {NOTIMEOUT=0}; // No timeout specified
  
  ///////////////////////////////////////////////////////////////////////////
  // ctors, dtors and their friends
  class ExecutionParameters;
  CeCosTest(const ExecutionParameters &e, LPCTSTR  const pszExecutable, LPCTSTR  const pszTitle=0);
  virtual ~CeCosTest();
  // Count of number of instances of this class:
  static int InstanceCount;
  // Delete all heap instances of this class (*must* be allocated on heap)
  static void DeleteAllInstances ();
  // Simply wait for instances to die a natural death
  static bool WaitForAllInstances (int nPoll=1000,Duration nTimeout=NOTIMEOUT);
  // Tap them on the shoulder (does not wait)
  static void CancelAllInstances ();
  ///////////////////////////////////////////////////////////////////////////
  
  ///////////////////////////////////////////////////////////////////////////
  // Representation of target:
  static unsigned int TargetTypeMax() { return (unsigned)arTargetInfo.size(); }
  static LPCTSTR  const Image(unsigned int i) { return arTargetInfo[i].Image(); }
  static const TargetInfo &Target(unsigned int i) { return arTargetInfo[i]; }
  static const TargetInfo &Target(LPCTSTR t);
  static void RemoveAllPlatforms();
  ///////////////////////////////////////////////////////////////////////////
  
  ///////////////////////////////////////////////////////////////////////////
  // Class used to represent execution parameters (to be passed with request to execute a test)
  ///////////////////////////////////////////////////////////////////////////
  class ExecutionParameters {
  public:
    
    enum RequestType { RUN, QUERY, LOCK, UNLOCK, STOP, RequestTypeMax};
    static RequestType RequestTypeValue(LPCTSTR );
    static LPCTSTR  Image (RequestType r) { return arRequestImage[r]; }
    
    Duration    ActiveTimeout() const { return m_nActiveTimeout; }
    Duration    DownloadTimeout() const { return m_nDownloadTimeout; }
    
    LPCTSTR  Target() const { return m_Target.c_str(); }
    bool        IsValid() const { return m_Request!=RUN || CeCosTest::IsValid(m_Target); }
    RequestType Request() const { return m_Request;}
    void SetActiveTimeout  (Duration t){m_nActiveTimeout=t;}
    void SetDownloadTimeout (Duration t){m_nDownloadTimeout=t;}
    
    ExecutionParameters (
      RequestType r=CeCosTest::ExecutionParameters::RUN,
      LPCTSTR  Target=_T(""),
      Duration    nActiveTimeout=NOTIMEOUT,
      Duration    nDownloadTimeout=NOTIMEOUT);
    bool FromStr(LPCTSTR psz);
    
    String Image() const;
    virtual ~ExecutionParameters(){}
    bool m_bUseFilter;
  protected:
    static LPCTSTR  arRequestImage [1+RequestTypeMax];
    String m_Target;
    Duration m_nActiveTimeout,m_nDownloadTimeout;
    RequestType m_Request;
    int m_nUnused1;
    int m_nUnused2;
    int m_nUnused3;
    bool m_bUnused2;
    bool m_bUnused3;
  };
  ///////////////////////////////////////////////////////////////////////////
  
  ///////////////////////////////////////////////////////////////////////////
  // Result status stuff.
  // Order is important - SetStatus can only change status in left-to-right direction
  void AnalyzeOutput();
  enum StatusType {NotStarted, NoResult, Inapplicable, Pass, DownloadTimeOut, TimeOut, Cancelled, Fail, 
    AssertFail, StatusTypeMax};
  static StatusType StatusTypeValue (LPCTSTR  const pszStr);
  static LPCTSTR  const Image(StatusType s) { return arResultImage[MIN(s,StatusTypeMax)]; }
  ///////////////////////////////////////////////////////////////////////////
  
  ///////////////////////////////////////////////////////////////////////////
  // Attributes
  LPCTSTR  const  Executable()             const { return m_strExecutable;}            // Executable name
  const           LPCTSTR Target()         const { return m_ep.Target();}              // Target
  static bool     IsSim(LPCTSTR t)               { 
    return TargetInfo::SIM==Target(t).Type() || 
           TargetInfo::REMOTE_SIM==Target(t).Type() || 
           TargetInfo::SYNTHETIC==Target(t).Type(); }

  static bool     ServerSideGdb(LPCTSTR t)       { return TargetInfo::SIM==Target(t).Type() || TargetInfo::SYNTHETIC==Target(t).Type(); }
  bool            ServerSideGdb()          const { return TargetInfo::SIM==Target(Target()).Type() || TargetInfo::SYNTHETIC==Target(Target()).Type(); }
  LPCTSTR  const  Title()                  const;                                      // Title
  
  Duration            ActiveTimeout()             const { return m_ep.ActiveTimeout(); }      // Active timeout
  Duration            DownloadTimeout()            const { return m_ep.DownloadTimeout(); }     // Total timeout
  
  StatusType          Status()                    const { return m_Status; }                  // Test status
  
  Duration            Download()                  const { return m_nDownloadTime; }           // Download time
  Duration            Total()                     const { return m_nTotalTime; }              // Total
  Duration            MaxInactive()               const { return m_nMaxInactiveTime; }        // Max. inactive
  
  LPCTSTR  const  Output()                    const { return m_strOutput; }               // Output generated by a test run [for report purposes]
  const CTestResource * const Port()                      const { return m_pPort; }                   // Port used for a test run [for report purposes]
  
  LPCTSTR  const  ResultString (bool bIncludeOutput=true)             const;
  ///////////////////////////////////////////////////////////////////////////
  
  ///////////////////////////////////////////////////////////////////////////
  // Running a test:
  
  // Run a test locally:
  bool RunLocal ();
  
  // Run a test remotely: 
  // If pszRemoteHostPort is given, it sends the test for execution on the given host:post.
  // Otherwise, a suitable host:port is determined from the test resource information.
  bool RunRemote (LPCTSTR  const pszRemoteHostPort);
  void Cancel (); // Stop the run
  ///////////////////////////////////////////////////////////////////////////
  
  ///////////////////////////////////////////////////////////////////////////
  // Resource functions
  
  static bool SetLogFile (LPCTSTR  pszFile);   // Define the log file
  
  // Run as a server on given TCP/IP port
  static bool RunAgent(int nTcpPort); 
  
  unsigned int RunCount() const { return m_nRunCount; }
  
  static bool IsValid (LPCTSTR pszTarget) { return Target(pszTarget).IsValid(); }

  static int  InitTargetInfo(LPCTSTR pszFilename);
  static bool SaveTargetInfo(LPCTSTR pszFilename);
  
  bool InteractiveGdb(const String &strHost,int nPort,TCHAR **argv);
  void SetTimeouts (Duration dActive,Duration dElapsed);
  void SetExecutable (LPCTSTR pszExecutable);
  static bool Value (
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
    int &nDownloadedSize);
  
  Time GdbCpuTime();
#ifndef _WIN32
  int ReadPipe (String &str,bool bBlock=false);             // Read from gdb process
#endif
  enum ServerStatus {SERVER_BUSY, SERVER_READY, SERVER_CANT_RUN, CONNECTION_FAILED, SERVER_LOCKED, ServerStatusMax};
  static LPCTSTR  const Image(ServerStatus s) { return arServerStatusImage[MIN(s,ServerStatusMax)]; }
  static ServerStatus ServerStatusValue(LPCTSTR psz);

  static ServerStatus CeCosTest::Connect (String strHost,int port, CeCosTestSocket *&pSock, const ExecutionParameters &e,String &strInfo,Duration dTimeout=10*1000);

  // Delay for given count of milliseconds
  // Record test result in log file:
  void LogResult();
  // Connect to m_strExecutionHostPort
  void ConnectForExecution ();
  
  // Log some output.  The accumulated output can be retrieved using Output()
  void Log (LPCTSTR  const pszFormat,...);
  void LogString (LPCTSTR psz);
  static bool GetSizes(LPCTSTR pszExecutable, LPCTSTR target,unsigned int &nFileSize, unsigned int &nStrippedSize);
  
protected:
  static String GetGreatestSubkey (LPCTSTR pszKey);
  bool GetTargetReady(String &strHostPort);
  
  unsigned int m_nRunCount;
  void Interactive(LPCTSTR  pszFormat, ...);
  
  bool m_bStopConnectSocketToSerial;
  
  static String CygPath (LPCTSTR pszPath);
  CResetAttributes::ResetResult Reset(bool bSendStatus);
  void LogTimeStampedOutput(LPCTSTR psz);
#ifndef _WIN32
  bool Suck(LPCTSTR pszPrompt,Duration d=1000);
#endif
  bool AtPrompt(LPCTSTR pszPrompt);
  bool BreakpointsOperational() const { return TargetInfo::HARDWARE_NO_BP!=Target(Target()).Type(); }
  void * m_wPipeHandle;
  void * m_rPipeHandle;
  
  // For Unix to limit calls to ps:
  Time m_tGdbCpuTime;
  Time m_tPrevSample;
  
  unsigned int m_nStrippedSize;
  void SendKeepAlives(bool &b);
  void WaitForRemoteCompletion();
  Time GdbTime();
  String m_strPath;
  void GetPath(String &strPath);
  void SetPath(const String &strPath);
  
  unsigned int m_nFileSize;                   // Size of executable
  
  String m_strExecutionHostPort;
  
  ///////////////////////////////////////////////////////////////////////////
  // Stuff to manage running gdb as child subprocess
  bool CheckForTimeout();                     // Check for a timeout - set status and return false if it happens
  bool m_bDownloading;                        // Are we currently downloading executable?
  bool GdbProcessAlive ();                    // Is gdb still alive and kicking?
  Time m_tBase;                               // Base used for measurement of timeouts
  Time m_tBase0;                              // Base used for measurement of timeouts
  Time m_tWallClock0;                         // When the test was actually started
  void * m_pGdbProcesshandle;                 // Handle associated with gdb process
#ifndef _WIN32
  bool WritePipe (const String &str); // Write to gdb process
  void DriveGdb(LPCTSTR pszPrompt,const StringArray &arstrGdbCmds);  // Run gdb
#endif
  ///////////////////////////////////////////////////////////////////////////
  // Close the socket used by the current class instance
  void CloseSocket ();
  
  bool send(const void * const pData,unsigned int nLength,LPCTSTR  const pszMsg=_T(""),Duration dTimeout=10*1000);
  bool recv(const void *pData,unsigned int nLength,LPCTSTR  const pszMsg=_T(""),Duration dTimeout=10*1000);
  
  bool sendResult(Duration dTimeout=10*1000);
  bool recvResult(Duration dTimeout=10*1000);
  
  ///////////////////////////////////////////////////////////////////////////
  
  ///////////////////////////////////////////////////////////////////////////
  CeCosTestSocket *m_pSock;
  
  mutable String m_strResultString;
  
  LPCTSTR  ExecutableTail() const { return CeCosTestUtils::Tail(m_strExecutable); }
  
  ExecutionParameters m_ep;
  
  // Chaining to allow *AllInstances functions to work:
  static CeCosTest * pFirstInstance;
  CeCosTest * m_pPrevInstance;
  CeCosTest * m_pNextInstance;
  
  void RunGdb (LPCTSTR pszCmdline,LPCTSTR pszPrompt,const StringArray &arstrGdbCmds);
  
  bool OutputContains(LPCTSTR psz) const { return 0!=_tcsstr(m_strOutput,psz); }
  
  static void CALLBACK SAcceptThreadFunc (void *pParam) {((CeCosTest *)pParam)->AcceptThreadFunc(); }
  void AcceptThreadFunc();

  static void CALLBACK SConnectSocketToSerialThreadFunc(void *pParam) { ((CeCosTest *)pParam)->ConnectSocketToSerialThreadFunc(); }
  void ConnectSocketToSerialThreadFunc();
  
  // Stuff.
  // Thread function used by RunLocal to execute a non-blocking test locally
  void LocalThreadFunc ();
  // Thread function used by RunRemote to execute a non-blocking test remotely
  void RemoteThreadFunc ();
  
  String m_strExecutable;
  String m_strTitle;
  
  void SetStatus (StatusType status);
  StatusType m_Status;
  
  Duration  m_nDownloadTime;
  Duration  m_nTotalTime;
  Duration  m_nMaxInactiveTime;
  
  CTestResource * m_pPort;
  
  String m_strOutput;

  static std::vector<TargetInfo> arTargetInfo;
  static const TargetInfo tDefault;

  static LPCTSTR  const arResultImage[1+StatusTypeMax];
  static LPCTSTR  const arServerStatusImage[1+ServerStatusMax];
  bool m_bConnectSocketToSerialThreadDone;
  static void CALLBACK ResetLogFunc(void *pParam, LPCTSTR psz);
}; // class CeCosTest



#endif
