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
// Properties.h: interface for the CProperties class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROPERTIES_H__DA938D29_135A_11D3_A50B_00A0C949ADAC__INCLUDED_)
#define AFX_PROPERTIES_H__DA938D29_135A_11D3_A50B_00A0C949ADAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "eCosStd.h"
#include "Collections.h"

class CProperties  
{
public:
//  bool Remove (LPCTSTR pszName);
  void RemoveAll ();
  // Declare various types of property.  The functions associate names with 
  // variables, but no values are assigned here.  That comes later when a
  // load function (such as LoadFromRegistry) is called.
  void Add (LPCTSTR pszName,int &n,int nDefault=0);
  void Add (LPCTSTR pszName,unsigned int &n,unsigned int nDefault=0);
  void Add (LPCTSTR pszName,bool &b,bool bDefault=false);
  void Add (LPCTSTR pszName,char &c,char cDefault='\0');
  void Add (LPCTSTR pszName,unsigned char &c,unsigned char cDefault='\0');
  void Add (LPCTSTR pszName,short&s,short sDefault=0);
  void Add (LPCTSTR pszName,unsigned short&s,unsigned short sDefault=0);
  void Add (LPCTSTR pszName,float&f,float fDefault=0.0);
  void Add (LPCTSTR pszName,double&f,double fDefault=0.0);
  void Add (LPCTSTR pszName,void *d,unsigned int nLength,void *pvDefault=0);
  void Add (LPCTSTR pszName,LPTSTR s,unsigned int nLength,LPCTSTR sDefault=_T(""));
  // String adding with caller defining get and put.  This is here to preserve the neutrality of
  // the class - cannot assume MFC's String, for example.  Implementation of these fns might be:
  //    static LPCTSTR CALLBACK GetFn (void *pObj) { return (LPCTSTR)*(String *)pObj; }
  //    static void    CALLBACK PutFn (void *pObj,LPCTSTR psz) { *(String *)pObj=psz; }
  
  typedef void (CALLBACK putFn)(void *,LPCTSTR);
  typedef LPCTSTR (CALLBACK getFn)(void *);
  void Add (LPCTSTR pszName,void *pObj,getFn *pgetFn,putFn *pputFn,LPCTSTR pszDefault=_T(""));
  
  // Set all Added variables to their default values
  void SetDefaults();
  // Load (from registry ro f
  bool Load(LPCTSTR pszPrefix=_T(""));
  bool Save(LPCTSTR pszPrefix=_T(""));
  
  // Load from and save to a command string.
  String MakeCommandString (LPCTSTR pszPrefix=_T("-")) const; // caller must delete []
  bool LoadFromCommandString (LPCTSTR psz,LPCTSTR pszPrefix=_T("-"));
  
  CProperties(LPCTSTR pszFile,void *hKey=0);
  virtual ~CProperties();
#ifdef _WIN32
  static bool CreateKey (LPCTSTR pszKey,HKEY hKey=HKEY_CURRENT_USER);
#endif
protected:
  static bool CreatePathToFile(LPCTSTR pszDir);
  String m_strName;
  
  // Load from and save to a given file.  The format is name=value, one per line.
  bool LoadFromFile(LPCTSTR pszFileName,LPCTSTR pszPrefix=_T(""));
  bool SaveToFile (LPCTSTR pszFileName,LPCTSTR pszPrefix=_T("")) const;
  // Load from and save to the registry
  void *m_hKey;
#ifdef _WIN32
  bool SaveToRegistry(HKEY,LPCTSTR,LPCTSTR pszPrefix=_T("")) const;
  bool LoadFromRegistry (HKEY,LPCTSTR,LPCTSTR pszPrefix=_T(""));
#endif
  
  PtrArray ar; // Holds declared properties
  class CProperty {
  public:
    enum Typetype {Integer, szString, GPString, Bool, Char, Short, Float, Double, Void};
    CProperty(LPCTSTR pszName,Typetype type,void *_pData);
    virtual ~CProperty();
    void SetDefault();
    friend class CProperties;
    bool  SetValue(int n);
    bool  SetValue(double n);
    bool  SetValue(void *d,int nLength);
    bool  SetValue(LPCTSTR psz);
    const String GetStringValue() const;
    unsigned long GetValue() const;
    
  protected:
    String strName;
    Typetype Type;
    void *pData;
    union {
      int    nDefault;
      double dDefault;
      void* pvDefault; 
    };
    String strDefault; // can't be in union - has copy ctor
    getFn *pgetFn;
    putFn *pputFn;
    unsigned int nLength; // for szString and Void
  };
  CProperty * Lookup (LPCTSTR pszName);
};

#endif // !defined(AFX_PROPERTIES_H__DA938D29_135A_11D3_A50B_00A0C949ADAC__INCLUDED_)
