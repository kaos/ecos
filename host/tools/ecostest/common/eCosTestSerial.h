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
//        eCosTestSerial.h
//
//        Serial test class
//
//=================================================================
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     sdf
// Contributors:  sdf
// Date:          1999-04-01
// Description:   This class abstracts the serial port for use in the testing infrastructure
// Usage:
//
//
//####DESCRIPTIONEND####

#ifndef _CECOSSERIAL_H
#define _CECOSSERIAL_H
#include "eCosStd.h"
#include "eCosTestSocket.h"
#include "Collections.h"

class CeCosTestSerial {
    friend int CeCosTestSocket::SSRead (CeCosTestSerial &serial,CeCosTestSocket &socket,void *pBuf,unsigned int nSize,unsigned int &nRead,bool *pbStop);

public:
    enum StopBitsType { ONE_STOP_BIT, ONE_POINT_FIVE_STOP_BITS, TWO_STOP_BITS };
    CeCosTestSerial(LPCTSTR pszPort,int nBaud); // ctor and open all in one go
    CeCosTestSerial(); // Call Open() later
    virtual ~CeCosTestSerial(); // Call Open() later

    bool Open(LPCTSTR pszPort,int nBaud);

    // Set various line characteristics.  This can be done with the line open or closed.
    bool SetBaud(unsigned int nBaud,bool bApplySettingsNow=true);
    bool SetParity(bool bParityOn,bool bApplySettingsNow=true);
    bool SetDataBits(int n,bool bApplySettingsNow=true);
    bool SetStopBits(StopBitsType n,bool bApplySettingsNow=true);
    bool SetReadTimeOuts(int nTotal,int nBetweenChars,bool bApplySettingsNow=true); // mSec
    bool SetWriteTimeOuts(int nTotal,int nBetweenChars,bool bApplySettingsNow=true); // mSec

    bool ApplySettings();
    
    // Get them:
    int  GetParity() const { return m_bParity; }
    int  GetDataBits() const { return m_nDataBits; }
    StopBitsType GetStopBits() const { return m_nStopBits; }
    unsigned int GetBaud() const { return m_nBaud; }
    bool GetReadTimeOuts(int &nTotal,int &nBetweenChars) const {nTotal=m_nTotalReadTimeout; nBetweenChars=m_nInterCharReadTimeout; return true; }// mSec
    bool GetWriteTimeOuts(int &nTotal,int &nBetweenChars) const {nTotal=m_nTotalWriteTimeout; nBetweenChars=m_nInterCharWriteTimeout; return true; }// mSec
    bool GetBlockingReads() const { return m_bBlockingReads; }
    bool Close();

    bool Flush (void);

    // Use to test success after open ctor
    bool Ok() { return 0!=m_pHandle; }

    // Will read up to the length provided:
    bool Read (void *pBuf,unsigned int nSize,unsigned int &nRead);
    bool Write(void *pBuf,unsigned int nSize,unsigned int &nWritten);

    //Removed, as dangerous when using UNICODE:
    //bool Read (String &str);
    //bool Write(const String &str);

    // Use in the event of an error that needs to be cleared before the next operation:
    bool ClearError();

    bool SetBlockingReads(bool b,bool bApplySettingsNow=true);

    int Error() { return m_nErr; }
    // Return last socket error, translated to a string
    String ErrString();

protected:
    int m_nErr;
    void SaveError() { 
        #ifdef _WIN32
        m_nErr=WSAGetLastError();
        #else // UNIX
        m_nErr=errno;
        #endif
    }

    void *m_pHandle;
    int m_nDataBits;
    StopBitsType m_nStopBits;
    bool m_bParity;
    unsigned int m_nBaud;
    int m_nTotalReadTimeout,m_nTotalWriteTimeout;
    int m_nInterCharReadTimeout,m_nInterCharWriteTimeout; 
    bool m_bBlockingReads;
    String m_strPort;
};
#endif
