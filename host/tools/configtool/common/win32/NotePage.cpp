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
// NotePage.cpp : implementation file
//

#include "stdafx.h"
#include "NotePage.h"
#include "ConfigTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNotePage property page

IMPLEMENT_DYNCREATE(CNotePage, CeCosPropertyPage)

CNotePage::CNotePage() : CeCosPropertyPage(CNotePage::IDD)
{
	//{{AFX_DATA_INIT(CNotePage)
	m_strNote = _T("");
	//}}AFX_DATA_INIT
}

CNotePage::~CNotePage()
{
}

void CNotePage::DoDataExchange(CDataExchange* pDX)
{
	CeCosPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNotePage)
	DDX_Text(pDX, IDC_EDIT_NOTE, m_strNote);
	DDV_MaxChars(pDX, m_strNote, 1023);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNotePage, CeCosPropertyPage)
	//{{AFX_MSG_MAP(CNotePage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

