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
// Properties.cpp: implementation of the CProperties class.
//
//////////////////////////////////////////////////////////////////////
#include "Properties.h"
#if defined (_AFXDLL) || defined(_AFXEXT)
    // MFC
    //#ifdef _DEBUG
    //#undef THIS_FILE
    //static char THIS_FILE[]=__FILE__;
    //#define new DEBUG_NEW
    //#endif
    #include <assert.h>
    #include <sys/types.h> 
    #include <sys/stat.h>
    #include <direct.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProperties::CProperties(LPCTSTR pszKey,void *hKey):
  m_strName(pszKey),
  m_hKey(hKey)
{
}

CProperties::~CProperties()
{
    RemoveAll();
}

void CProperties::RemoveAll()
{
    for(int i=ar.size()-1;i>=0;--i){
        delete (CProperties::CProperty *)ar[i];
    }
    ar.clear();
}

#ifdef _WIN32
bool CProperties::LoadFromRegistry(HKEY hTopKey,LPCTSTR szRegKey,LPCTSTR pszPrefix)
{
  int nPrefixlen=_tcslen(pszPrefix);
  HKEY hKey;
  LONG l=RegOpenKeyEx (hTopKey, szRegKey, 0L, KEY_QUERY_VALUE, &hKey);
  bool rc=(ERROR_SUCCESS==l);
  if(rc){
    TCHAR szName[256];
    DWORD dwSizeName=sizeof szName;
    DWORD dwMaxDatalen;
    DWORD dwType;
    if(ERROR_SUCCESS==RegQueryInfoKey(hKey,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,&dwMaxDatalen,NULL,NULL)){
      char *Data=new char[dwMaxDatalen];
      DWORD dwDatalen=dwMaxDatalen;
      for(DWORD dwIndex=0;ERROR_SUCCESS==RegEnumValue(hKey, dwIndex, szName, &dwSizeName, NULL, &dwType, (LPBYTE)Data, &dwDatalen);dwIndex++){ 
        if(0!=_tcsncmp(pszPrefix,szName,nPrefixlen)){
          continue;
        }
        
        CProperties::CProperty *p=Lookup(szName+nPrefixlen);
        if(p){
          switch(p->Type){
          case CProperty::Integer:
            if(REG_DWORD==dwType){
              p->SetValue(*(int *)Data);
            } else {
              //TRACE(_T("Type mismatch - %s: expected REG_DWORD, got %d\n"),(LPCTSTR)p->strName,dwType);
              rc=false;
            }
            break;
          case CProperty::Bool:
            if(REG_DWORD==dwType){
              p->SetValue((bool)0!=*(int *)Data);
            } else {
              //TRACE(_T("Type mismatch - %s: expected REG_DWORD, got %d\n"),(LPCTSTR)p->strName,dwType);
              rc=false;
            }
            break;
          case CProperty::Char:
            if(REG_DWORD==dwType){
              p->SetValue(*(char *)Data);
            } else {
              //TRACE(_T("Type mismatch - %s: expected REG_DWORD, got %d\n"),(LPCTSTR)p->strName,dwType);
              rc=false;
            }
            break;
          case CProperty::Short:
            if(REG_DWORD==dwType){
              p->SetValue(*(short *)Data);
            } else {
              //TRACE(_T("Type mismatch - %s: expected REG_DWORD, got %d\n"),(LPCTSTR)p->strName,dwType);
              rc=false;
            }
            break;
          case CProperty::Float:
          case CProperty::Double:
          case CProperty::szString: 
          case CProperty::GPString: 
            if(REG_SZ==dwType){
              rc&=p->SetValue((LPCTSTR)Data);
            } else {
              //TRACE(_T("Type mismatch - %s: expected REG_SZ, got %d\n"),(LPCTSTR)p->strName,dwType);
              rc=false;
            }
            break;
          case CProperty::Void:
            if(REG_BINARY==dwType){
              memcpy(p->pData,Data,min(dwDatalen,p->nLength));
            } else {
              //TRACE(_T("Type mismatch - %s: expected REG_BINARY, got %d\n"),(LPCTSTR)p->strName,dwType);
              rc=false;
            }
            break;
          }
        } else {
          //TRACE(_T("CProperties::LoadFromRegistry - unrecognized value %s\\%s\n"),szRegKey,szName);
          rc=false;
        }
        dwSizeName=sizeof szName;
        dwDatalen=dwMaxDatalen;
      }
      delete [] Data;
      dwSizeName=sizeof szName;
    }
    RegCloseKey(hKey);
  } else {
    //TRACE(_T("Failed to open %s\n"),szRegKey);
  }
  
  return rc;
}

bool CProperties::SaveToRegistry(HKEY hTopKey,LPCTSTR szRegKey,LPCTSTR pszPrefix) const
{
  HKEY hKey;
  CreateKey(szRegKey);
  bool rc=(ERROR_SUCCESS==RegOpenKeyEx (hTopKey, szRegKey, 0L, KEY_SET_VALUE, &hKey));
  if(rc){
    for(int i=ar.size()-1;i>=0;--i){
      // Initializations are simply to avoid compiler warnings.
      DWORD dwDatalen=0; 
      DWORD dwType=REG_DWORD;
      BYTE *Data=0;
      // strValue and dw *must* be in scope for RegSetValueEx below.
      DWORD dw;
      String strValue;
      CProperties::CProperty *p=(CProperties::CProperty *)ar[i];
      switch(p->Type){
        case CProperties::CProperty::Integer:
        case CProperties::CProperty::Bool:
        case CProperties::CProperty::Char:
        case CProperties::CProperty::Short:
          dwType=REG_DWORD;
          dwDatalen=sizeof(DWORD);
          dw=p->GetValue();
          Data=(BYTE *)&dw;
          break;
        case CProperties::CProperty::Float:
        case CProperties::CProperty::Double:
        case CProperties::CProperty::szString:
        case CProperties::CProperty::GPString:
          strValue=p->GetStringValue();
          Data=(BYTE *)(LPCTSTR)strValue;
          dwType=REG_SZ;
          dwDatalen=(1+strValue.GetLength())*sizeof(_TCHAR);
          break;
        case CProperties::CProperty::Void:
          Data=(BYTE *)p->pData;
          dwType=REG_BINARY;
          dwDatalen=p->nLength;
          break;
        default:
          assert(false);
          break;
      }
      String strName(pszPrefix);
      strName+=p->strName;
      rc&=(ERROR_SUCCESS==RegSetValueEx(hKey,strName,0,dwType,Data,dwDatalen));
    }
  }   
  RegCloseKey(hKey);
  return rc;    
}

// Create all keys down to the one specified
bool CProperties::CreateKey(LPCTSTR pszKey,HKEY hKey/*=HKEY_CURRENT_USER*/)
{
    bool rc=true;
    LPCTSTR pcStart=pszKey;
    LPCTSTR pcEnd;
    do {
        HKEY hKey2;
        pcEnd=_tcschr(pcStart,_TCHAR('\\'));
        if(NULL==pcEnd){
            pcEnd=pcStart+_tcslen(pcStart);
        }
        String strKey(pcStart,pcEnd-pcStart);
        if(ERROR_SUCCESS!=RegCreateKeyEx(hKey,                // handle to an open key
              strKey,         // address of subkey name
              0,           // reserved
              0,           // address of class string
              REG_OPTION_NON_VOLATILE,          // special options flag
              KEY_ALL_ACCESS,        // desired security access
              NULL,
                                        // address of key security structure
              &hKey2,          // address of buffer for opened handle
              NULL// address of disposition value buffer);
              )){
            rc=false;
            break;
        }
        RegCloseKey(hKey);
        hKey=hKey2;
        pcStart=pcEnd+1;
    } while (_TCHAR('\0')!=*pcEnd);
    RegCloseKey(hKey);
    return rc;
}

#endif

bool CProperties::LoadFromCommandString(LPCTSTR psz,LPCTSTR pszPrefix/*=_T("-")*/)
{
  bool rc=true;
  const TCHAR *cNext;
  int nPrefixlen=_tcslen(pszPrefix);
  for(LPCTSTR c=_tcsstr(psz,pszPrefix);c;c=_tcsstr(cNext,pszPrefix)){
    c+=nPrefixlen;
    const TCHAR *pEq=_tcschr(c,_TCHAR('='));
    if(NULL==pEq){
      //TRACE(_T("Failed to find '=' after %s\n"),c);
      rc=false;
      break;
    }
    String strName(c,pEq-c);
    CProperties::CProperty *p=Lookup(strName);
    c=pEq+1;
    String str;
    if(_TCHAR('"')==*c){
      // Value is a quoted string
      for(cNext=c+1;_TCHAR('"')!=*cNext;cNext++){
        if(_TCHAR('\\')==*cNext){
          cNext++;
        }
        str+=*cNext;
      }
    } else {
      // Value is simply terminated by whitespace
      for(cNext=c;_TCHAR('\0')!=*cNext && !_istspace(*cNext);cNext++);
      str=String(c,cNext-c);
    }
    if(p){
      rc&=p->SetValue(str);
    } else {
      //TRACE(_T("Properties: unrecognized attribute %s in command string\n"),(LPCTSTR)strName);
      rc=false;
    }
  }
  return rc;
}

CProperties::CProperty * CProperties::Lookup(LPCTSTR pszName)
{
    for(int i=ar.size()-1;i>=0;--i){
        CProperties::CProperty *p=(CProperties::CProperty *)ar[i];
        if(p->strName==pszName){
            return p;
        }
    }
    return NULL;
}

String CProperties::MakeCommandString(LPCTSTR pszPrefix/*=_T("-")*/) const
{
  String strResult;
  bool bFirst=true;
  for(int i=ar.size()-1;i>=0;--i){
    String str;    
    CProperties::CProperty *p=(CProperties::CProperty *)ar[i];
    switch(p->Type){
    case CProperties::CProperty::Integer:
    case CProperties::CProperty::Bool:
    case CProperties::CProperty::Char:
    case CProperties::CProperty::Short:
      str.Format(_T("%s%s=%u"),pszPrefix,(LPCTSTR)p->strName,p->GetValue());
      break;
    case CProperties::CProperty::szString:
    case CProperties::CProperty::GPString:
    case CProperties::CProperty::Float:
    case CProperties::CProperty::Double:
    case CProperties::CProperty::Void:
      str.Format(_T("%s%s=\"%s\""),pszPrefix,(LPCTSTR)p->strName,(LPCTSTR)p->GetStringValue());
      break;
    }
    if(!bFirst){
      strResult+=_TCHAR(' ');
    }
    bFirst=false;
    strResult+=str;
  }
  return strResult;
}       

bool CProperties::CreatePathToFile(LPCTSTR pszDir) 
{
    // Create intermediate directories
    #ifdef _WIN32
    const TCHAR cSep='\\';
    #else // UNIX
    const TCHAR cSep='/';
    #endif
    for(LPCTSTR c=_tcschr(pszDir,cSep);c;c=_tcschr(c+1,cSep)){
        #ifdef _WIN32
        if(c==pszDir+2 && _istalpha(pszDir[0]) && _TCHAR(':')==pszDir[1]){
            continue; // don't attempt to create "C:"
        }
        #endif
        String strDir(pszDir,c-pszDir);
        struct _stat buf;
        if(!(0==_tstat(strDir,&buf) && (S_IFDIR&buf.st_mode))){
            // Need to create directory
            bool b=(0==_tmkdir(strDir));
            //TRACE(_T("Create directory %s rc=%d\n"),(LPCTSTR)strDir,b);
            if(!b){
                return false;
            }
        }
    }
    return true;
}

bool CProperties::SaveToFile(LPCTSTR pszFileName,LPCTSTR pszPrefix) const
{
  CreatePathToFile(pszFileName);
  // If we have a prefix, we assume we're tagging on to an existing file
  FILE *f=_tfopen(pszFileName,(_TCHAR('\0')==*pszPrefix)?_T("wt"):_T("at"));
  if(f){
    for(int i=ar.size()-1;i>=0;--i){
      CProperties::CProperty *p=(CProperties::CProperty *)ar[i];
      String str(pszPrefix);
      str+=p->strName;
      str+=_TCHAR('=');
      switch(p->Type){
        case CProperties::CProperty::Integer:
        case CProperties::CProperty::Bool:
        case CProperties::CProperty::Char:
        case CProperties::CProperty::Short:
          {
            String strN;
            strN.Format(_T("%u"),p->GetValue());
            str+=strN;
          }
          break;
        case CProperties::CProperty::Float:
        case CProperties::CProperty::Double:
        case CProperties::CProperty::szString:
        case CProperties::CProperty::GPString:
        case CProperties::CProperty::Void:
          str+=p->GetStringValue();
          break;
      }
      str+=_TCHAR('\n');
      _fputts(str,f);
    }
    fclose(f);
  }
  return (0!=f);
}

bool CProperties::LoadFromFile(LPCTSTR pszFileName,LPCTSTR pszPrefix)
{
  int nPrefixlen=_tcslen(pszPrefix);
  FILE *f=_tfopen(pszFileName,_T("rt"));
  bool rc=(0!=f);
  if(rc){
    TCHAR c[256];
    while(_fgetts(c,sizeof(c)-1,f)){
      if(_TCHAR('\0')!=c[0]){
        c[_tcslen(c)-1]=_TCHAR('\0'); // remove trailing \n
      }
      if((signed)_tcslen(c)>nPrefixlen && 0==_tcsncmp(c,pszPrefix,nPrefixlen)){
        TCHAR *pEq=_tcschr(c,_TCHAR('='));
        if(pEq){
          const String strName(c+nPrefixlen,pEq-c-nPrefixlen);
          CProperties::CProperty *p=Lookup(strName);
          if(p){
            pEq++;
            rc&=p->SetValue(pEq);
          } else {
            //TRACE(_T("Failed to find %s\n"),(LPCTSTR)strName);
            rc=false;
          }
        }
      }
    }
    fclose(f);
  }
  return rc;
  
}

CProperties::CProperty::CProperty(LPCTSTR pszName,Typetype type,void *_pData):
  strName(pszName),
  Type(type),
  pData(_pData)
{
}

CProperties::CProperty::~CProperty()
{
}

void CProperties::Add(LPCTSTR pszName,int &n,int _nDefault) 
{
    CProperty *p=new CProperty(pszName,CProperty::Integer,&n);
    p->nDefault=_nDefault;
    ar.push_back(p);
}

void CProperties::Add(LPCTSTR pszName,unsigned int &n,unsigned int _nDefault)
{
    CProperty *p=new CProperty(pszName,CProperty::Integer,&n);
    p->nDefault=_nDefault;
    ar.push_back(p);
}

void CProperties::Add(LPCTSTR pszName,bool &b,bool _bDefault)
{
    CProperty *p=new CProperty(pszName,CProperty::Bool,&b);
    p->nDefault=_bDefault;
    ar.push_back(p);
}

void CProperties::Add(LPCTSTR pszName,char &c,char _cDefault)
{
    CProperty *p=new CProperty(pszName,CProperty::Char,&c);
    p->nDefault=_cDefault;
    ar.push_back(p);
}

void CProperties::Add(LPCTSTR pszName,unsigned char &c,unsigned char _cDefault)
{
    CProperty *p=new CProperty(pszName,CProperty::Char,&c);
    p->nDefault=_cDefault;
    ar.push_back(p);
}

void CProperties::Add(LPCTSTR pszName,short &s,short _sDefault)
{
    CProperty *p=new CProperty(pszName,CProperty::Short,&s);
    p->nDefault=_sDefault;
    ar.push_back(p);
}

void CProperties::Add(LPCTSTR pszName,unsigned short &s,unsigned short _sDefault)
{
    CProperty *p=new CProperty(pszName,CProperty::Short,&s);
    p->nDefault=_sDefault;
    ar.push_back(p);
}

void CProperties::Add(LPCTSTR pszName,float &f,float _fDefault)
{
    CProperty *p=new CProperty(pszName,CProperty::Float,&f);
    p->dDefault=_fDefault;
    ar.push_back(p);
}

void CProperties::Add(LPCTSTR pszName,double &f,double dDefault) 
{
    CProperty *p=new CProperty(pszName,CProperty::Double,&f);
    p->dDefault=dDefault;
    ar.push_back(p);
}

void CProperties::Add(LPCTSTR pszName,void *pv,unsigned int _nLength,void *pvDefault)
{
    CProperty *p=new CProperty(pszName,CProperty::Void,pv);
    p->nLength=_nLength;
    p->pvDefault=pvDefault;
    ar.push_back(p);
}

void CProperties::Add(LPCTSTR pszName,LPTSTR s,unsigned int _nLength,LPCTSTR _pszDefault)
{
    CProperty *p=new CProperty(pszName,CProperty::szString,s);
    p->nLength=_nLength;
    p->strDefault=_pszDefault;
    ar.push_back(p);
}

void CProperties::Add (LPCTSTR pszName,void *pObj,getFn *getFn,putFn *putFn,LPCTSTR pszDefault)
{
    CProperty *p=new CProperty(pszName,CProperty::GPString,pObj);
    p->strDefault=pszDefault;
    p->pgetFn=getFn;
    p->pputFn=putFn;
    p->strDefault=pszDefault;
    ar.push_back(p);
}

unsigned long CProperties::CProperty::GetValue() const
{
    unsigned long dw;
    switch(Type){
        case Integer:
            dw=*(int *)pData;
            break;
        case Bool:
            dw=*(bool *)pData;
            break;
        case Char:
            dw=*(char *)pData;
            break;
        case Short:
            dw=*(short *)pData;
            break;
        default:
            dw=0;
            assert(false);
    }
    return dw;
}

const String CProperties::CProperty::GetStringValue() const 
{
    String str;
    switch(Type){
        case szString:
            str=(LPCTSTR)(pData); 
            break;
        case GPString:
            str=pgetFn(pData); 
            break;
        case CProperties::CProperty::Integer:
        case CProperties::CProperty::Bool:
        case CProperties::CProperty::Char:
        case CProperties::CProperty::Short:
            str.Format(_T("%u"),GetValue());
            break;
        case CProperties::CProperty::Float:
            str.Format(_T("%e"),*(float *)(pData));
            break;
        case CProperties::CProperty::Double:
            str.Format(_T("%e"),*(double *)(pData));
            break;
        case CProperties::CProperty::Void:
            {
                unsigned char *c=(unsigned char *)pData;
                for(unsigned int i=0;i<nLength;i++){
                    TCHAR buf[3];
                    _tprintf(buf,_T("%02x"),c[i]);
                    str+=buf;
                }
            }
            break;
        default:
            break;
    }
    return str;
}

bool CProperties::CProperty::SetValue(int n)
{
    bool rc=true;
    switch(Type){
        case Integer:
            *(int *)(pData)=n;
            break;
        case Bool:
            *(bool *)(pData)=(0!=n);
            break;
        case Char:
            *(char *)(pData)=(char)n; //FIXME: range checks
            break;
        case Short:
            *(short *)(pData)=(short)n;//FIXME: range checks
            break;
        default:
            //TRACE(_T("Failed to set '%s' to integer value '%d'\n"),(LPCTSTR)strName,n);
            break;
    }
    return rc;
}

bool CProperties::CProperty::SetValue(double n)
{
    bool rc=true;
    switch(Type){
        case Double:
            *(float *)(pData)=(float)n;//FIXME: range checks?
            break;
        case Float:
            *(double *)(pData)=n;
            break;
        default:
            //TRACE(_T("Failed to set '%s' to double value '%f'\n"),(LPCTSTR)strName,n);
            rc=false;
            break;
    }

    return rc;
}

bool CProperties::CProperty::SetValue(LPCTSTR psz)
{
    bool rc=false;
    TCHAR *pEnd;
    double d;
    long l;
    switch(Type){
        case szString:
            _tcsncpy((LPTSTR)pData,psz,nLength);
            if(nLength<=_tcslen(psz)){
                ((LPTSTR)pData)[nLength]=_TCHAR('\0');
            }
            rc=true;
            break;
        case GPString:
            pputFn(pData,psz);
            rc=true;
            break;             
        case Float:
            d=_tcstod(psz,&pEnd);
            rc=(_TCHAR('\0')==*pEnd);
            if(rc){
                SetValue((float)d);
            }
            break;
        case Double:
            d=_tcstod(psz,&pEnd);
            rc=(_TCHAR('\0')==*pEnd);
            if(rc){
                SetValue(d);
            }
            break;
        case Integer:
        case Bool:
        case Char:
        case Short:
            l=_tcstol(psz,&pEnd,10);
            rc=(_TCHAR('\0')==*pEnd);
            if(rc){
                SetValue((int)l);
            }
            break;
        default:
            //TRACE(_T("Failed to set '%s' to string value '%s'\n"),(LPCTSTR)strName,psz);
            break;
    }
    return rc;
}

void CProperties::CProperty::SetDefault()
{
    switch(Type){
        case Bool:
        case Integer:
        case Char:
        case Short:
            SetValue(nDefault);
            break;
        case Float:
        case Double:
            SetValue(dDefault);
            break;
        case szString:
        case GPString:
            SetValue(strDefault);
            break;
        case Void:
            if(pvDefault){
                memcpy(pData,pvDefault,nLength);
            }
            break;
        default:
            assert(false);
    }

}

void CProperties::SetDefaults()
{
    for(int i=ar.size()-1;i>=0;--i){
        ((CProperty*)ar[i])->SetDefault();
    }
}

bool CProperties::Load(LPCTSTR pszPrefix)
{
    #ifdef _WIN32
    return m_hKey?LoadFromRegistry((HKEY)m_hKey,m_strName,pszPrefix):LoadFromFile(m_strName,pszPrefix);
    #else // UNIX
    return LoadFromFile(m_strName,pszPrefix);
    #endif
}

bool CProperties::Save(LPCTSTR pszPrefix)
{
    #ifdef _WIN32
    return m_hKey?SaveToRegistry((HKEY)m_hKey,m_strName,pszPrefix):SaveToFile(m_strName,pszPrefix);
    #else // UNIX
    return SaveToFile(m_strName,pszPrefix);
    #endif
}
/*
bool CProperties::Remove(LPCTSTR pszName)
{
    for(int i=ar.size()-1;i>=0;--i){
        CProperties::CProperty *p=(CProperties::CProperty *)ar[i];
        if(p->strName==pszName){
            delete p;
            ar.RemoveAt(i);
            return true;
        }
    }
    return false;
}
*/
