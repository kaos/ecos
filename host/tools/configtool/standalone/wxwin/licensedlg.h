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
// Date:        2000/12/18
// Version:     $Id: licensedlg.h,v 1.2 2001/03/23 13:38:04 julians Exp $
// Purpose:
// Description: Header file for ecLicenseDialog
// Requires:
// Provides:
// See also:
// Known bugs:
// Usage:
//
//####DESCRIPTIONEND####
//
//===========================================================================

#ifndef _ECOS_LICENSEDLG_H_
#define _ECOS_LICENSEDLG_H_

#ifdef __GNUG__
    #pragma interface "licensedlg.cpp"
#endif

#include "ecutils.h"

//----------------------------------------------------------------------------
// ecLicenseDialog
//----------------------------------------------------------------------------

class ecLicenseDialog: public ecDialog
{
public:
    // constructors and destructors
    ecLicenseDialog( const wxString& licenseText, wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
    
//// Event handlers

    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

//// Operations
    void CreateControls();

private:
    DECLARE_EVENT_TABLE()

    wxString    m_licenseText;
};


#endif
    // _ECOS_LICENSEDLG_H_
