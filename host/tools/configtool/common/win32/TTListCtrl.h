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
#if !defined(AFX_TTLISTCTRL_H__2E80C132_9E6E_11D3_A538_00A0C949ADAC__INCLUDED_)
#define AFX_TTLISTCTRL_H__2E80C132_9E6E_11D3_A538_00A0C949ADAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ttlistctrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTTListCtrl window

class CTTListCtrl : public CListCtrl
{
// Construction
public:
	CTTListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTTListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTTListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTTListCtrl)
	//}}AFX_MSG
  int OnToolHitTest(CPoint point, TOOLINFO * pTI) const;
  BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TTLISTCTRL_H__2E80C132_9E6E_11D3_A538_00A0C949ADAC__INCLUDED_)
