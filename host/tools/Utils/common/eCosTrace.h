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
// Description:   Standard include file for test infra
// Usage:
//
//####DESCRIPTIONEND####

#ifndef _ECOSTRACE_H
#define _ECOSTRACE_H
#include "eCosStd.h"
#include "Collections.h"

class CeCosTrace {
public:
  // Diagnostic output
  static void Out(LPCTSTR  psz) { pfnOut(pOutParam,psz); }
  static void Err(LPCTSTR  psz) { pfnError(pErrorParam,psz); }

  static void Trace (LPCTSTR  pszFormat,...);
  static void Error (LPCTSTR  pszFormat,...);

  static void EnableTracing(bool b) { bVerbose=b; }
  static bool IsTracingEnabled () { return bVerbose; }

  static void SetInteractive(bool b) { bInteractive=b; }
  static bool IsInteractive() { return bInteractive; }

  static void SetOutput (LogFunc *pFn,void *pParam) { pfnOut=pFn; pOutParam=pParam; }
  static void SetError  (LogFunc *pFn,void *pParam) { pfnError=pFn; pErrorParam=pParam; }

  static bool SetOutput (LPCTSTR pszFilename);
  static bool SetError  (LPCTSTR pszFilename);

  #ifndef TRACE
    #define TRACE CeCosTrace::Trace
  #endif
  #undef ERROR
  #define ERROR CeCosTrace::Error

  #ifdef VERBOSE
    #define VTRACE CeCosTrace::Trace
  #else
    #define VTRACE if(0) CeCosTrace::Trace
  #endif

  static void CALLBACK StreamLogFunc  (void *, LPCTSTR psz);

protected:
  struct StreamInfo {
    Time tLastReopen;
    String strFilename;
    FILE *f;
    StreamInfo(LPCTSTR pszFile,FILE *_f) : tLastReopen(Now()),strFilename(pszFile),f(_f) {}
    ~StreamInfo() { fclose(f); }
  };
  
  static StreamInfo OutInfo,ErrInfo;

  static void CALLBACK StreamInfoFunc  (void *, LPCTSTR psz);
  
  static LPCTSTR arpszDow[7];

  static bool bVerbose;
  static bool bInteractive;
  static LogFunc *pfnOut;    static void *pOutParam;
  static LogFunc *pfnError;  static void *pErrorParam;
};
#endif
