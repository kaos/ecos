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
//#####DESCRIPTIONBEGIN####
//
// Author(s):     sdf
// Contributors:  sdf
// Date:          1999-04-01
// Description:   Holds information related to target reset.
// Usage:
//
//####DESCRIPTIONEND####

#ifndef _RESETATTRIBUTES_H
#define _RESETATTRIBUTES_H

#include "eCosStd.h"
#include "eCosTestSerial.h"
#include "eCosTestSocket.h"
#include "Properties.h"

class CTestResource;

class CResetAttributes {
public:
	bool IsValid();
  CResetAttributes(LPCTSTR psz=_T("")) : m_str(psz) {}
  const LPCTSTR Image() const { return m_str; }
  static bool IsError(int n) { return n>=RESET_ILLEGAL_DEVICE_CODE && n<=RESET_UNKNOWN_ERROR; }
  static LPCTSTR Image(int nErr);
  bool IsNull() const { return 0==m_str.GetLength(); }
  static const CResetAttributes NoReset;

  // Perform reset
  enum ResetResult {INVALID_STRING=-1,RESET_OK=0,NOT_RESET=1,
    RESET_ILLEGAL_DEVICE_CODE=30000,RESET_NO_REPLY, RESET_BAD_CHECKSUM, RESET_BAD_ACK, RESET_UNKNOWN_ERROR};

  enum Action {OFF=0,ON=1,OFF_ON=2,ON_OFF=3};

  ResetResult Reset (LogFunc *pfnLog=0, void *pfnLogparam=0,bool bCheckOnly=false);
  static bool CALLBACK IsValidReset (void *);

protected:

  Time m_tResetOccurred;
  void ResetLog(LPCTSTR psz);
  LogFunc *m_pfnReset;
  void *m_pfnResetparam;

  CeCosTestSerial m_Serial;
  CeCosTestSocket m_Socket;
  
  const TCHAR *GetIdAndArg (LPCTSTR psz,String &strID,String &strArg);
  ResetResult Parse (LPCTSTR psz,bool bCheckOnly=false);
  bool Reset (Action action,bool bCheckOutput);

  void SuckThreadFunc ();
  static void CALLBACK SSuckThreadFunc (void *pParam) { ((CResetAttributes*)pParam)->SuckThreadFunc(); }
  int GetArgs(LPCTSTR psz,StringArray &ar);

  String m_str; // Could be const, but avoid the assignment operator warnings

  String m_strHostPort;  // host we talk to
  String m_strControl;   // Control string
  int m_nDelay;                          // Delay between power off and power on
  String m_strAuxPort;   // Auxiliary port (serial port to listen on if primary port is TCP/IP)
  int m_nReadTimeout;                    // mSec to wait for board to say something
  int m_nBaud;

  String m_strResetOutput;
};

#endif
