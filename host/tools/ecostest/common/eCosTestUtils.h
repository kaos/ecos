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
//        eCosTestUtils.h
//
//        run test program
//
//=================================================================
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     sdf
// Contributors:  sdf
// Date:          1999-04-01
// Description:   Utiltities for test infra
// Usage:
//
//####DESCRIPTIONEND####

#ifndef _ECOSTESTUTILS_H
#define _ECOSTESTUTILS_H

#include "eCosStd.h"
#include "Collections.h"

class CeCosTestUtils {
public:
	static bool CommandLine (int &argc,TCHAR **argv);
  
  static LPCTSTR HostName();
  static LPCTSTR SimpleHostName();
  
  // Find the tail of a filename
  static LPCTSTR  const Tail (LPCTSTR  const pszFile);
  
  // File iterator.  Gets next file in directory, avoiding "." and ".."
  static bool CeCosTestUtils::NextFile (void *&pHandle,String &str);
  // Start file iteration and return first file.
  static bool CeCosTestUtils::StartSearch (void *&pHandle,String &str);
  // End file iteration
  static void CeCosTestUtils::EndSearch (void *&pHandle);

protected:
  
};

#endif
