// ####ECOSHOSTGPLCOPYRIGHTBEGIN####                                        
// -------------------------------------------                              
// This file is part of the eCos host tools.                                
// Copyright (C) 1998, 1999, 2000 Free Software Foundation, Inc.            
//
// This program is free software; you can redistribute it and/or modify     
// it under the terms of the GNU General Public License as published by     
// the Free Software Foundation; either version 2 or (at your option) any   
// later version.                                                           
//
// This program is distributed in the hope that it will be useful, but      
// WITHOUT ANY WARRANTY; without even the implied warranty of               
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU        
// General Public License for more details.                                 
//
// You should have received a copy of the GNU General Public License        
// along with this program; if not, write to the                            
// Free Software Foundation, Inc., 51 Franklin Street,                      
// Fifth Floor, Boston, MA  02110-1301, USA.                                
// -------------------------------------------                              
// ####ECOSHOSTGPLCOPYRIGHTEND####                                          
// PkgAdminDlg.h : header file
//

#if !defined(AFX_PKGADMINDLG_H__9B4DD5E8_6113_11D3_8008_00A0C9554250__INCLUDED_)
#define AFX_PKGADMINDLG_H__9B4DD5E8_6113_11D3_8008_00A0C9554250__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning (push)
    #pragma warning(disable:4018) // signed/unsigned mismatch
    #pragma warning(disable:4100) // unreferenced formal parameter
    #pragma warning(disable:4146) // unary minus operator applied to unsigned type, result still unsigned
    #pragma warning(disable:4244) // conversion from 'unsigned int' to 'char', possible loss of data
    #pragma warning(disable:4503) // decorated name length exceeded, name was truncated
    #pragma warning(disable:4663) // C++ language change: to explicitly specialize class template...

    // EXTERN will be (re)defined in tcl.h via cdl.hxx
    #undef EXTERN 
	#include "cdl.hxx"
    #undef EXTERN
#pragma warning (pop)

#include "PkgAdminres.h"
#include "eCosDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CPkgAdminDlg dialog

class CPkgAdminDlg : public CeCosDialog
{
// Construction
public:
	CPkgAdminDlg(LPCTSTR pszRepository=NULL,LPCTSTR pszUserTools=NULL);

// Dialog Data
	//{{AFX_DATA(CPkgAdminDlg)
	enum { IDD = IDD_PKGADMIN_DIALOG };
	CButton	m_btnRemove;
	CTreeCtrl	m_ctrlPackageTree;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPkgAdminDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CString m_strRepository;
	HICON m_hIcon;
protected:
	bool FindUserToolsPath();
	static std::string UnicodeToStdStr (LPCTSTR str);
	CImageList m_ilTreeIcons;
	CString m_strUserTools;
	bool RemovePackageVersion (HTREEITEM hTreeItem);
	bool EvalTclFile (int nArgc, LPCTSTR pszArgv);
	CdlPackagesDatabase m_CdlPkgData;
	void ClearPackageTree ();
	bool PopulatePackageTree (LPCTSTR pszPackageDatabase);

	// Generated message map functions
	//{{AFX_MSG(CPkgAdminDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPkgadminRemove();
	afx_msg void OnDestroy();
	afx_msg void OnPkgadminAdd();
	afx_msg void OnPkgadminRepository();
	afx_msg void OnSelchangedPkgadminTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PKGADMINDLG_H__9B4DD5E8_6113_11D3_8008_00A0C9554250__INCLUDED_)
