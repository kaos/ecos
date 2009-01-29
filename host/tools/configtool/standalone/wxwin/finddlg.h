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
// appsettings.cpp :
//
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   julians
// Contact(s):  julians
// Date:        2000/09/11
// Version:     $Id: finddlg.h,v 1.2 2001/03/07 17:08:33 julians Exp $
// Purpose:
// Description: Header file for ecFindDialog
// Requires:
// Provides:
// See also:
// Known bugs:
// Usage:
//
//####DESCRIPTIONEND####
//
//===========================================================================

#ifndef _ECOS_FINDDLG_H_
#define _ECOS_FINDDLG_H_

#ifdef __GNUG__
    #pragma interface "finddlg.cpp"
#endif

//----------------------------------------------------------------------------
// ecFindDialog
//----------------------------------------------------------------------------

class ecFindDialog: public wxDialog
{
public:
    // constructors and destructors
    ecFindDialog( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE );

//// Operations    
    bool AddControls(wxWindow* parent);

    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();

//// Event handlers
    void OnFindNext(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

private:
    // Radiobox selection, used by validator
    int m_directionSelection;

    DECLARE_EVENT_TABLE()
};

#define ecID_FIND_DIALOG_WHAT           10002
#define ecID_FIND_DIALOG_MATCH_WHOLE    10003
#define ecID_FIND_DIALOG_MATCH_CASE     10004
#define ecID_FIND_DIALOG_DIRECTION      10005
#define ecID_FIND_DIALOG_SEARCH_IN      10006
#define ecID_FIND_DIALOG_NEXT           10007

#endif
    // _ECOS_FINDDLG_H_
