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
//#####DESCRIPTIONBEGIN####
//
// Author(s):     sdf
// Contributors:  sdf
// Date:          1999-04-01
// Description:   Holds information related to target reset
// Usage:
//
//####DESCRIPTIONEND####
#include "eCosStd.h"
#include "eCosThreadUtils.h"
#include "eCosTrace.h"
#include "ResetAttributes.h"

const CResetAttributes CResetAttributes::NoReset;

LPCTSTR CResetAttributes::Image(int nErr)
{
  switch(nErr){
    case RESET_OK:
      return _T("RESET_OK");
      break;
    case RESET_ILLEGAL_DEVICE_CODE:
      return _T("Illegal device code");
      break;
    case RESET_NO_REPLY:
      return _T("No reply from reset unit");
      break;
    case RESET_BAD_CHECKSUM:
      return _T("Bad checksum");
      break;
    case RESET_BAD_ACK:
      return _T("Bad ack");
      break;
    default:
      return _T("Unknown reset error");
      break;
  }
}

void CResetAttributes::SuckThreadFunc()
{
  m_strResetOutput.SetLength(0);
  
  // Board has apparently been powered on.  Suck initial output.
  String strMsg;
  strMsg.Format(_T(">>> Reading board startup output from %s with timeout of %d seconds...\n"),(LPCTSTR)m_strAuxPort,m_nReadTimeout/1000);
  ResetLog(strMsg);

  enum {BUFSIZE=512};
  TCHAR *buf=new TCHAR[BUFSIZE];
  memset(buf,0,BUFSIZE); // safety for string functions in IsValidReset
  TCHAR *c=buf;
  do {
    unsigned int dwRead=0;
    // We are working in non-blocking mode
    if(m_Socket.Ok()){
      if(!m_Socket.Peek(dwRead)||!m_Socket.recv(c,MIN(dwRead,(unsigned)BUFSIZE-(c-buf)))){
        break;
      }
    } else if (!m_Serial.Read(c,BUFSIZE-(c-buf),dwRead)){
      m_Serial.ClearError();
      c=buf;
    } 
    if(dwRead>0){
      c[dwRead]=_TCHAR('\0');
      {
        String str;
        for(const TCHAR *t=c;*t;t++){
          if(_istprint(*t)){
            str+=*t;
          }
        }
        ResetLog(str);
      }
      
      for(int i=dwRead-1;i>=0;--i){
        if(c[i]<0x20 || c[i]>=0x7f){
          // Control character - assume nothing up to this is of interest
          i++;
          dwRead-=i;
          memmove(c,c+i,dwRead);
          break;
        }
      }
      c+=dwRead;
      if(IsValidReset((void *)buf)){
        ResetLog(_T("\n>>> Valid reset determined\n"));
        break;
      }
    } else { // Nothing read
      Sleep(500);
    }
  } while (0==m_tResetOccurred || Now()-m_tResetOccurred<m_nReadTimeout);
  m_strResetOutput=buf;
  delete [] buf;
  if(0==m_strResetOutput.GetLength()){
    ResetLog(_T("\n>>> No response from board\n"));
  }
}

bool CResetAttributes::Reset(Action action,bool bCheckOutput)
{
  m_strResetOutput=_T("");
  bool rc=false;
  CeCosTestSocket sock;
  time_t ltime;
  time(&ltime);
  struct tm *now=localtime( &ltime );
  String strStatus;

  if(bCheckOutput){
    strStatus.Format(_T(">>> Reset target using %s %s port=%s(%d) read timeout=%d delay=%d [%02d:%02d:%02d]\n"),
      (LPCTSTR)m_strHostPort,(LPCTSTR)m_strControl, (LPCTSTR)m_strAuxPort, m_nBaud, m_nReadTimeout, m_nDelay,
      now->tm_hour,now->tm_min,now->tm_sec);
  } else {
    strStatus.Format(_T(">>> Reset target using %s %s delay=%d [%02d:%02d:%02d]\n"),
      (LPCTSTR)m_strHostPort,(LPCTSTR)m_strControl, m_nDelay,
      now->tm_hour,now->tm_min,now->tm_sec);
  }
  ResetLog(strStatus);

  // Open up communication to port whence we read the board startup
  bool bThreadDone=false;
  bCheckOutput&=(m_strAuxPort.GetLength()>0);
  if(bCheckOutput){
    TRACE(_T("Opening %s\n"),(LPCTSTR)m_strAuxPort);
    String strHost;
    int nPort;
    if(CeCosTestSocket::ParseHostPort(m_strAuxPort,strHost,nPort)){
      // tcp/ip port
      if(!m_Socket.Connect(strHost,nPort,m_nReadTimeout)){
        String str;
        str.Format(_T("Failed to open %s - %s\n"),(LPCTSTR)m_strAuxPort,(LPCTSTR)m_Socket.SocketErrString());
        ResetLog(str);
        return false;
      }
    } else {
      // Comms device
      m_Serial.SetBlockingReads(false);
      if(m_Serial.Open(m_strAuxPort,m_nBaud)){
        m_Serial.Flush();
      } else {
        String str;
        str.Format(_T("Failed to open comms port %s - %s\n"),(LPCTSTR)m_strAuxPort,(LPCTSTR)m_Serial.ErrString());
        ResetLog(str);
        return false;
      }
    }
    CeCosThreadUtils::RunThread(SSuckThreadFunc,this,&bThreadDone,_T("SSuckThreadFunc"));
  } else {
    ResetLog(_T(">>> [not checking output]\n"));
  }
  
  String strHost;
  int nPort;
  // This will be true if we need to talk to a reset server, false to talk down a local port
  bool bRemote=CeCosTestSocket::ParseHostPort(m_strHostPort,strHost,nPort);
  if(bRemote){
    if(sock.Connect(strHost,nPort,10*1000)){
      m_tResetOccurred=0;
      // Write the message to the socket
      String strCmd;
      int nDelay=(action==ON_OFF || action==OFF_ON)?m_nDelay:0;
      strCmd.Format(_T("-Control=%s -Action=%d -Delay=%d"),(LPCTSTR)m_strControl,action,nDelay);
      TRACE(_T("-Control=%s -Action=%d -Delay=%d"),(LPCTSTR)m_strControl,action,0);
      if(sock.sendString(strCmd,_T("Reset control codes"), 10*1000)){
        // Wait for an acknowledgement
        String strResponse;
        if(sock.recvString(strResponse, _T("Response"), 20*1000)){
          rc=(0==strResponse.GetLength());
          if(!rc && m_pfnReset){
            String strMsg;
            strMsg.Format(_T("Reset server reports error '%s'\n"),(LPCTSTR)strResponse);
            ResetLog(strMsg);
          }
        } else {
          String str;
          str.Format(_T(">>> Failed to read response from reset server %s - %s\n"),(LPCTSTR)m_strHostPort,(LPCTSTR)sock.SocketErrString());
          ResetLog(str);
        }
      } else {
        String str;
        str.Format(_T(">>> Failed to contact reset server %s - %s\n"),(LPCTSTR)m_strHostPort,(LPCTSTR)sock.SocketErrString());
        ResetLog(str);
        ResetLog(_T(">>> Failed to contact reset server\n"));
      }
      m_tResetOccurred=Now();
      if(bCheckOutput){
        if(!rc){
          // force thread to time out
          m_tResetOccurred=Now()-m_nReadTimeout;
        }
        CeCosThreadUtils::WaitFor(bThreadDone); // do not apply a timeout - the thread has one
        rc=IsValidReset((void *)(LPCTSTR)m_strResetOutput);
        ResetLog(rc?_T(">>> Reset output valid\n"):_T("!!! Reset output invalid\n"));
      } 
    } else {
      String str;
      str.Format(_T(">>> Failed to contact reset server %s - %s\n"),(LPCTSTR)m_strHostPort,(LPCTSTR)sock.SocketErrString());
      ResetLog(str);
      ResetLog(_T(">>> Failed to contact reset server\n"));
    }
  } else {
    // Sending something locally
    m_tResetOccurred=Now();
    unsigned int nWritten;
    m_Serial.Write((void *)(LPCTSTR)m_strControl,1,nWritten);
    if(bCheckOutput){
      CeCosThreadUtils::WaitFor(bThreadDone);  // do not apply a timeout - the thread has one
      rc=IsValidReset((void *)(LPCTSTR)m_strResetOutput);
      ResetLog(rc?_T(">>> Reset output valid\n"):_T("!!! Reset output invalid\n"));
    }
  }

  if(m_Socket.Ok()){
    m_Socket.Close();
  } else {
    m_Serial.Close();
  }
  return rc && bCheckOutput;
}

const TCHAR *CResetAttributes::GetIdAndArg (LPCTSTR psz,String &strID,String &strArg)
{
  const TCHAR *cEnd=_tcschr(psz,_TCHAR('('));
  if(cEnd){
    strID=String(psz,cEnd-psz);
    int nNest=0;
    for(const TCHAR *c=cEnd;*c;c++){
      if(_TCHAR('(')==*c){
        nNest++;
      } else if(_TCHAR(')')==*c){
        nNest--;
        if(0==nNest){
          strArg=String(cEnd+1,c-(cEnd+1));
          return c+1;
        }
      }
    }
    assert(false);
  }
  return 0;
}

int CResetAttributes::GetArgs(LPCTSTR psz,StringArray &ar)
{
  ar.clear();
  if(0==*psz){
    return 0;
  } else {
    String str;
    for(const TCHAR *c=psz;*c;c++){
      if(_TCHAR(',')==*c){
        ar.push_back(str);
        str=_T("");
      } else {
        str+=*c;
      }
    }
    ar.push_back(str);
    return ar.size();
  }
}

// m_str will hold something like
//   3(off(ginga:5000,a1) delay(2000) on(,..com2,38400))
CResetAttributes::ResetResult CResetAttributes::Reset (LogFunc *pfnLog, void *pfnLogparam,bool bCheckOnly)
{
  m_pfnReset=pfnLog;
  m_pfnResetparam=pfnLogparam;

  const TCHAR *c;
  String str;
  // Remove spaces
  for(c=m_str;*c;c++){
    if(!_istspace(*c)){
      str+=*c;
    }
  }

  // Check paren matching:
  int nNest=0;
  for(c=str;*c;c++){
    if(_TCHAR('(')==*c){
      nNest++;
    } else if(_TCHAR(')')==*c){
      nNest--;
      if(nNest<0){
        ResetLog(_T("Too many right parentheses"));
        return INVALID_STRING;
      }
    }
  }
  if(nNest>0){
    ResetLog(_T("Too many left parentheses"));
    return INVALID_STRING;
  }

  m_nReadTimeout=10*1000;
  m_nBaud=38400;
  m_nDelay=1000;
  
  return Parse(str,bCheckOnly);
}

// 3(off(ginga:5000,a1) delay(2000) on(,..com2,38400))
CResetAttributes::ResetResult CResetAttributes::Parse (LPCTSTR psz,bool bCheckOnly)
{
  bool bCheck=false;
  for(const TCHAR *c=psz;*c;){
    String strID,strArg;
    c=GetIdAndArg(c,strID,strArg);
    if(0==c){
      ResetLog(_T("Invalid reset string"));
      return INVALID_STRING;
    }

    if(isdigit(*(LPCTSTR)strID)){
      int nRepeat=_ttoi(strID);
      if(0==nRepeat){
        ResetLog(_T("Invalid reset string"));
        return INVALID_STRING;
      }
      if(bCheckOnly){
        return Parse(strArg,true);
      } else {
        while(nRepeat--){
          ResetResult r=Parse(strArg);
          if(RESET_OK==r||INVALID_STRING==r){
            return r;
          }
        }
      }
    } else if (_T("port")==strID) {
      //   0. Port
      //   1. Baud
      //   2. Timeout
      StringArray ar;
      int nArgs=GetArgs(strArg,ar);
      if(nArgs>0 && ar[0].GetLength()){
        m_strAuxPort=ar[0];
      }
      if(nArgs>1 && ar[1].GetLength()){
        m_nBaud=_ttoi(ar[1]);
      }
      if(nArgs>2 && ar[2].GetLength()){
        m_nReadTimeout=_ttoi(ar[2]);
      }
    } else if (_T("off")==strID || _T("on")==strID || _T("on_off")==strID || _T("off_on")==strID) {
      // args are:
      //   0. Reset host:port
      //   1. Control string
      //   2. Port
      //   3. Baud
      //   4. Read timeout
      //   5. Delay
      StringArray ar;
      int nArgs=GetArgs(strArg,ar);
      if(nArgs>0 && ar[0].GetLength()){
        m_strHostPort=ar[0];
      }
      if(nArgs>1 && ar[1].GetLength()){
        m_strControl=ar[1];
      }
      if(nArgs>2 && ar[2].GetLength()){
        m_strAuxPort=ar[2];
      }
      if(nArgs>3 && ar[3].GetLength()){
        m_nBaud=_ttoi(ar[3]);
      }
      if(nArgs>4 && ar[4].GetLength()){
        m_nReadTimeout=_ttoi(ar[4]);
      }
      if(nArgs>5 && ar[5].GetLength()){
        m_nDelay=_ttoi(ar[5]);
      }

      if(0==m_strHostPort.GetLength()){
        ResetLog(_T("Failed to specify reset host:port"));
        return INVALID_STRING;
      }

      Action action=ON; // prevent compiler warning
      if(_T("on")==strID){
        action=ON;
      } else if(_T("off")==strID){
        action=OFF;
      } else if(_T("on_off")==strID){
        action=ON_OFF;
      } else if(_T("off_on")==strID){
        action=OFF_ON;
      } 

      if(!bCheckOnly && Reset(action,bCheck||action==ON_OFF||action==OFF_ON)){
        return RESET_OK;
      }
      bCheck ^= 1;
    } else if (_T("delay")==strID) {
      TRACE(_T("Sleep %d\n"),_ttoi(strArg));
      if(!bCheckOnly){
        Sleep(_ttoi(strArg));
      }
    } else {
      ResetLog(_T("Unrecognized command"));
      return INVALID_STRING;
    }
  }
  ResetLog(_T("!!! Target reset not verified\n"));
  return NOT_RESET;
}

void CResetAttributes::ResetLog(LPCTSTR psz)
{
  if(m_pfnReset){
    ENTERCRITICAL;
    m_pfnReset(m_pfnResetparam,psz);
    TRACE(_T("%s"),psz);
    LEAVECRITICAL;
  }
}

// This function determines whether the board startup has all that is required
// It is a hack because it has hardwired knowledge of what boards say at startup time
bool CALLBACK CResetAttributes::IsValidReset (void *pParam)
{
  bool rc=false;
  LPCTSTR pszBuf=(LPCTSTR)pParam;
  
  // Look for $T or $S
  LPCTSTR pcTpkt=_tcsstr(pszBuf,_T("$T"));
  if(0==pcTpkt){
    pcTpkt=_tcsstr(pszBuf,_T("$S"));
  }
  
  if(pcTpkt){
    // T packet ends with #hh
    LPCTSTR d=_tcschr(pcTpkt,_TCHAR('#'));
    if(d && d[1] && d[2]){
      rc=true;
    }
  } else if (_tcsstr(pszBuf,_T("cygmon> "))) {
    rc=true;
  } else {
    LPCTSTR pBootp=_tcsstr(pszBuf,_T("BOOTP got "));
    if(pBootp){
      int i1,i2,i3,i4;
      rc=(4==_stscanf(pBootp+10,_T("%d.%d.%d.%d"),&i1,&i2,&i3,&i4));
    }
  } 
  return rc;
}

bool CResetAttributes::IsValid()
{
  return INVALID_STRING!=Reset (0,0,true);
}
