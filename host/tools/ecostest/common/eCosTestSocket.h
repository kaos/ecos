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
//        eCosTestSocket.h
//
//        Socket test class
//
//=================================================================
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     sdf
// Contributors:  sdf
// Date:          1999-04-01
// Description:   This class abstracts tcp/ip sockets for use in the testing infrastructure
// Usage:
//
//####DESCRIPTIONEND####
#include "eCosStd.h"
#include "Collections.h"

#ifndef _SOCKETUTILS_H
#define _SOCKETUTILS_H

class CeCosTestSerial;

class CeCosTestSocket {
public:	
  static bool SameHost (LPCTSTR host1,LPCTSTR host2);
  
  typedef bool (CALLBACK FilterFunc)(void *&,unsigned int &,CeCosTestSerial&,CeCosTestSocket &,void *);
  static int CeCosTestSocket::SSRead (CeCosTestSerial &serial,CeCosTestSocket &socket,void *pBuf,unsigned int nSize,unsigned int &nRead,bool *pbStop);
  
  static bool ConnectSocketToSerial (int nListenSock,LPCTSTR pszPort, int nBaud,FilterFunc *pSerialToSocketFilterFunc=0,void *pSerialParam=0,FilterFunc *pSocketToSerialFilterFunc=0,void *pSocketParam=0,bool *pbStop=0);
  static bool ConnectSocketToSerial (CeCosTestSocket &socket,CeCosTestSerial &serial,FilterFunc *pSerialToSocketFilterFunc=0,void *pSerialParam=0, FilterFunc *pSocketToSerialFilterFunc=0,void *pSocketParam=0,bool *pbStop=0);
  bool ConnectSocketToSocket (CeCosTestSocket &o,FilterFunc *pSocketToSocketFilterFunc1,FilterFunc *pSocketToSocketFilterFunc2,void *pParam,bool *pbStop);
  
  typedef bool (CALLBACK StopFunc)(void *);
  
  static bool IsSocketError(int n);
  
  enum {NOTIMEOUT=-1,DEFAULTTIMEOUT=-2}; // No explicit timeout specified
  
  // Listen and this form of constructor used to act as server
  static int Listen(int nTcpPort);
  CeCosTestSocket (); // Caller promises to call Accept() or Connect() later
  
  // Accept-like ctor (act as server)
  CeCosTestSocket (int sock /*result of previous call of Listen*/, bool *pbStop=0);
  // Connect-like ctor (act as client)
  CeCosTestSocket (String strHost,int port,Duration dTimeout);
  
  bool Accept(int sock /*result of previous call of Listen*/, bool *pbStop=0);
  // This form of constructor used to act as client
  bool Connect(String strHost,int port,Duration dTimeout);
  ~CeCosTestSocket();
  
  int Client() const { return m_nClient; }
  static String ClientName(int nClient);
  
  int Sock() const { return m_nSock; }
  
  // Set the default timeout for all operations
  void SetTimeout (Duration dTimeout) { m_nDefaultTimeout=dTimeout; }
  
  bool send(const void *pData,unsigned int nLength,LPCTSTR pszMsg=_T(""),int dTimeout=DEFAULTTIMEOUT,StopFunc *pFunc=0,void *pParam=0){
    return sendrecv(true,pData,nLength,pszMsg,dTimeout,pFunc,pParam);
  }
  bool recv(const void *pData,unsigned int nLength,LPCTSTR pszMsg=_T(""),int dTimeout=DEFAULTTIMEOUT,StopFunc *pFunc=0,void *pParam=0){
    return sendrecv(false,pData,nLength,pszMsg,dTimeout,pFunc,pParam);
  }
  bool Ok() { return -1!=m_nSock; }
  // Close the given socket
  bool Close () { return CloseSocket(m_nSock); }
  
  int SocketError() { return m_nErr; }
  // Return last socket error, translated to a string
  String SocketErrString();
  static String SocketErrString(int nErr);
  
  bool recvInteger (int &n,LPCTSTR pszMsg=_T(""),Duration dTimeout=DEFAULTTIMEOUT);
  bool sendInteger (int n,LPCTSTR pszMsg=_T(""),Duration dTimeout=DEFAULTTIMEOUT);
  
  bool recvString  (String &str,LPCTSTR pszMsg=_T(""),Duration dTimeout=DEFAULTTIMEOUT);
  bool sendString  (const String &str,LPCTSTR pszMsg=_T(""),Duration dTimeout=DEFAULTTIMEOUT);
  
  static bool CloseSocket (int &sock);
  bool Peek (unsigned int &nAvail);
  static String HostPort(LPCTSTR pszHost,int nPort);
  static bool ParseHostPort (LPCTSTR pszHostPort, String &pszHost, int &nPort);
  static bool IsLegalHostPort (LPCTSTR pszHostPort);
  
protected:
  Duration m_nDefaultTimeout;
  Duration TimeoutDuration (Duration dTimeout);
  // Set appropriate socket options (most importantly, non-blocking mode)
  bool SetSocketOptions ();
  int m_nSock;
  int m_nClient;
  int m_nErr;
  void SaveError() { 
#ifdef _WIN32
  m_nErr=WSAGetLastError();
#else // UNIX
  m_nErr=errno;
#endif
  }
  bool sendrecv(bool bSend,const void *pData,unsigned int nLength,LPCTSTR pszMsg=_T(""),int dTimeout=DEFAULTTIMEOUT,StopFunc *pFunc=0,void *pParam=0);
  
};
#endif
