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
//        eCosTestUtils.cpp
//
//        Utility functions
//
//=================================================================
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     sdf
// Contributors:  sdf
// Date:          1999-04-01
// Description:   This class contains utility functions for use in the testing infrastructure
// Usage:
//
//####DESCRIPTIONEND####

#include "eCosStd.h"
#include "eCosTestUtils.h"
#include "eCosThreadUtils.h"
#include "eCosTrace.h"

LPCTSTR  const CeCosTestUtils::Tail(LPCTSTR  const pszFile)
{
  LPCTSTR pszTail=_tcsrchr(pszFile,_TCHAR('/'));
  if(0==pszTail){
    pszTail=_tcsrchr(pszFile,_TCHAR('\\'));
  }
  return (0==pszTail)?pszFile:pszTail+1;
}

LPCTSTR CeCosTestUtils::HostName()
{
  static bool bFirstTime=true;
  static String str;
  static int nErr;
  if(bFirstTime){
    char szMyname[256];
    nErr=gethostname(szMyname,sizeof szMyname);
    bFirstTime=false;
    if(0==nErr){
      str=String::CStrToUnicodeStr(szMyname);
    } else {
      str=_T("");
    }
  }
  return str;
}

LPCTSTR CeCosTestUtils::SimpleHostName()
{
  static bool bFirstTime=true;
  static String str;
  if(bFirstTime){
    str=HostName();
    // Remove all after a '.'
    LPCTSTR c=_tcschr(str,_TCHAR('.'));
    if(c){
      str.SetLength(c-(LPCTSTR )str);
    }
    bFirstTime=false;
  }
  return str;
}



// File iterator.  Gets next file in directory, avoiding _T(".") and _T("..")
bool CeCosTestUtils::NextFile (void *&pHandle,String &str)
{
#ifdef _WIN32
  WIN32_FIND_DATA fd;
  while(FindNextFile((HANDLE)pHandle,&fd)){
    LPCTSTR pszName=fd.cFileName;
#else // UNIX
    struct dirent *d;
    while((d=readdir((DIR *)pHandle))){
      LPCTSTR pszName=d->d_name;
#endif
      if(pszName[0]!='.'){
        str=pszName;
        return true;
      }
    }
    return false;
  }
  
  // Start file iteration and return first file.
bool CeCosTestUtils::StartSearch (void *&pHandle,String &str)
  {
#ifdef _WIN32
    WIN32_FIND_DATA fd;
    pHandle=(void *)FindFirstFile (_T("*.*"), &fd);
    if(INVALID_HANDLE_VALUE==(HANDLE)pHandle){
      ERROR(_T("Failed to open dir\n"));
      return false;
    } else if (fd.cFileName[0]=='.') {
      return NextFile((HANDLE)pHandle,str);
    } else {
		str=String(fd.cFileName);
      return true;
    }
#else // UNIX
    pHandle=(void *)opendir(_T("."));
    if(0==pHandle){
      ERROR(_T("Failed to open dir\n"));
      return false;
    }
    return NextFile(pHandle,str);
#endif
  }
  
  // End file iteration
void CeCosTestUtils::EndSearch (void *&pHandle)
  {
#ifdef _WIN32
    FindClose((HANDLE)pHandle);
#else // UNIX
    closedir((DIR *)pHandle);
#endif
}
  

// deal with common command-line  actions
bool CeCosTestUtils::CommandLine(int &argc,TCHAR **argv)
{
  for(int i=1;i<argc;i++){
    if(_TCHAR('-')==*argv[i]){
      if(0==_tcscmp(argv[i],_T("-v"))){
        CeCosTrace ::EnableTracing(true);
        // Shuffle the command line down to remove that which we have just seen:
        for(TCHAR **a=argv+i;(a[0]=a[1]);a++);
        argc--;i--; // counteract the increment
      } else if(0==_tcscmp(argv[i],_T("-o"))){
        if(i+1<argc){
          if(!CeCosTrace::SetOutput(argv[i+1])){
            ERROR(_T("Failed to direct output to %s\n"),argv[i+1]);
          }
          // Shuffle the command line down to remove that which we have just seen:
          for(TCHAR **a=argv+i;(a[0]=a[2]);a++);
          argc-=2;i-=2; // counteract the increment
        } else {
          return false;
        }
      } else if(0==_tcscmp(argv[i],_T("-version"))){
        const TCHAR *pszTail=_tcsrchr(argv[0],_TCHAR('/'));
        if(0==pszTail){
          pszTail=_tcsrchr(argv[0],_TCHAR('\\'));
        }
  			_tprintf (_T("%s %s (%s %s)\n"), (0==pszTail)?argv[0]:pszTail+1,ECOS_VERSION,  __DATE__, __TIME__);
        exit(0);
      } 
    }
  }
  return true;
}

