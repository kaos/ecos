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
// packages :
//
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   julians
// Contact(s):  julians
// Date:        2000/09/28
// Version:     $Id: packagesdlg.cpp,v 1.2 2001/03/23 13:38:04 julians Exp $
// Purpose:
// Description: Implementation file for ecPackagesDialog
// Requires:
// Provides:
// See also:
// Known bugs:
// Usage:
//
//####DESCRIPTIONEND####
//
//===========================================================================

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation "packagesdlg.h"
#endif

// Includes other headers for precompiled compilation
#include "ecpch.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/cshelp.h"
#include "wx/valgen.h"

#include "configtool.h"
#include "packagesdlg.h"
#include "configtooldoc.h"
#include "ecutils.h"

BEGIN_EVENT_TABLE(ecPackagesDialog, ecDialog)
    EVT_BUTTON(wxID_OK, ecPackagesDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, ecPackagesDialog::OnCancel)
    EVT_BUTTON(ecID_PACKAGES_DIALOG_ADD, ecPackagesDialog::OnAdd)
    EVT_BUTTON(ecID_PACKAGES_DIALOG_REMOVE, ecPackagesDialog::OnRemove)
	EVT_LISTBOX_DCLICK(ecID_PACKAGES_DIALOG_AVAILABLE_LIST, ecPackagesDialog::OnDblClickListBox1)
	EVT_LISTBOX_DCLICK(ecID_PACKAGES_DIALOG_USE_LIST, ecPackagesDialog::OnDblClickListBox2)
	EVT_LISTBOX(ecID_PACKAGES_DIALOG_AVAILABLE_LIST, ecPackagesDialog::OnClickListBox1)
	EVT_LISTBOX(ecID_PACKAGES_DIALOG_USE_LIST, ecPackagesDialog::OnClickListBox2)
    EVT_LISTBOX(ecID_PACKAGES_DIALOG_VERSION, ecPackagesDialog::OnSelectVersion)
    EVT_INIT_DIALOG(ecPackagesDialog::OnInitDialog)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// Frame constructor
ecPackagesDialog::ecPackagesDialog(wxWindow* parent)
{
    m_bHardwarePackageSelected = FALSE;

    SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);

    ecDialog::Create(parent, ecID_PACKAGES_DIALOG, _("Packages"),
        wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

    CreateControls(this);

    Centre(wxBOTH);
}

ecPackagesDialog::~ecPackagesDialog()
{
}

// TODO: implement wxLB_SORT style in wxGTK.
void ecPackagesDialog::CreateControls(wxWindow* parent)
{
    wxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxSizer *item2 = new wxBoxSizer( wxVERTICAL );

    wxStaticText *item3 = new wxStaticText( parent, wxID_STATIC, _("Available &packages:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs4 = (wxString*) NULL;
    wxListBox *item4 = new wxListBox( parent, ecID_PACKAGES_DIALOG_AVAILABLE_LIST, wxDefaultPosition, wxSize(180,190), 0, strs4, 0 );
    item2->Add( item4, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    item1->Add( item2, 1, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 0 );

    wxSizer *item5 = new wxBoxSizer( wxVERTICAL );

    wxButton *item6 = new wxButton( parent, ecID_PACKAGES_DIALOG_ADD, _("&Add >>"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item6, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxButton *item7 = new wxButton( parent, ecID_PACKAGES_DIALOG_REMOVE, _("<< &Remove"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item7, 0, wxALIGN_CENTRE|wxALL, 5 );

    item1->Add( item5, 0, wxALIGN_CENTRE|wxALL, 0 );

    wxSizer *item8 = new wxBoxSizer( wxVERTICAL );

    wxStaticText *item9 = new wxStaticText( parent, wxID_STATIC, _("&Use these packages:"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs10 = (wxString*) NULL;
    wxListBox *item10 = new wxListBox( parent, ecID_PACKAGES_DIALOG_USE_LIST, wxDefaultPosition, wxSize(180,190), 0, strs10, 0 );
    item8->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    item1->Add( item8, 1, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 0 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item11 = new wxStaticText( parent, wxID_STATIC, _("&Version:"), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item11, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 10 );

    wxString *strs12 = (wxString*) NULL;
    wxChoice *item12 = new wxChoice( parent, ecID_PACKAGES_DIALOG_VERSION, wxDefaultPosition, wxSize(100,-1), 0, strs12, 0 );
    item0->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 10 );

    wxTextCtrl *item13 = new wxTextCtrl( parent, ecID_PACKAGES_DIALOG_DESCRIPTION, _(""), wxDefaultPosition, wxSize(80,70), wxTE_MULTILINE );
    item0->Add( item13, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 10 );

    wxSizer *item14 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item15 = new wxButton( parent, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item15->SetDefault();
    item14->Add( item15, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxButton *item16 = new wxButton( parent, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item14->Add( item16, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item14, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

#if 0
    wxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxSizer *item2 = new wxBoxSizer( wxVERTICAL );

    wxStaticText *item3 = new wxStaticText( parent, wxID_STATIC, "Available &packages:", wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxListBox *item4 = new wxListBox( parent, ecID_PACKAGES_DIALOG_AVAILABLE_LIST, wxDefaultPosition, wxSize(180,190), 0, NULL, wxLB_EXTENDED|wxLB_HSCROLL|wxLB_SORT );
    item2->Add( item4, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    item1->Add( item2, 0, wxALIGN_CENTRE|wxALL, 0 );

    wxSizer *item5 = new wxBoxSizer( wxVERTICAL );

    wxButton *item6 = new wxButton( parent, ecID_PACKAGES_DIALOG_ADD, "&Add >>", wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item6, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxButton *item7 = new wxButton( parent, ecID_PACKAGES_DIALOG_REMOVE, "<< &Remove", wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item7, 0, wxALIGN_CENTRE|wxALL, 5 );

    item1->Add( item5, 0, wxALIGN_CENTRE|wxALL, 0 );

    wxSizer *item8 = new wxBoxSizer( wxVERTICAL );

    wxStaticText *item9 = new wxStaticText( parent, wxID_STATIC, "&Use these packages:", wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxListBox *item10 = new wxListBox( parent, ecID_PACKAGES_DIALOG_USE_LIST, wxDefaultPosition, wxSize(180,190), 0, NULL, wxLB_EXTENDED|wxLB_HSCROLL|wxLB_SORT );
    item8->Add( item10, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    item1->Add( item8, 0, wxALIGN_CENTRE|wxALL, 0 );

    item0->Add( item1, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxStaticText *item11 = new wxStaticText( parent, wxID_STATIC, "&Version:", wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item11, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 10 );

    wxChoice *item12 = new wxChoice( parent, ecID_PACKAGES_DIALOG_VERSION, wxDefaultPosition, wxSize(100,-1), 0, NULL, 0 );
    item0->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 10 );

    wxTextCtrl *item13 = new wxTextCtrl( parent, ecID_PACKAGES_DIALOG_DESCRIPTION, "", wxDefaultPosition, wxSize(80,70), wxTE_MULTILINE );
    item0->Add( item13, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 10 );

    wxSizer *item14 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item15 = new wxButton( parent, wxID_OK, "&OK", wxDefaultPosition, wxDefaultSize, 0 );
    item15->SetDefault();
    item14->Add( item15, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxButton *item16 = new wxButton( parent, wxID_CANCEL, "&Cancel", wxDefaultPosition, wxDefaultSize, 0 );
    item14->Add( item16, 0, wxALIGN_CENTRE|wxALL, 5 );

#ifdef __WXGTK__
    wxButton *contextButton = new wxContextHelpButton( parent );
    item14->Add( contextButton, 0, wxALIGN_CENTRE|wxALL, 5 );
#endif

    item0->Add( item14, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );
#endif

#ifdef __WXGTK__
    wxButton *contextButton = new wxContextHelpButton( parent );
    item14->Add( contextButton, 0, wxALIGN_CENTRE|wxALL, 5 );
#endif

    parent->SetAutoLayout( TRUE );
    parent->SetSizer( item0 );
    parent->Layout();
    item0->Fit( parent );
    //item0->SetSizeHints( parent );

    // Add context-sensitive help text
    parent->FindWindow( ecID_PACKAGES_DIALOG_AVAILABLE_LIST )->SetHelpText(_("Displays the list of packages available, but not currently loaded."));
    parent->FindWindow( ecID_PACKAGES_DIALOG_USE_LIST )->SetHelpText(_("Displays the list of packages currently loaded."));
    parent->FindWindow( ecID_PACKAGES_DIALOG_ADD )->SetHelpText(_("Add one or more packages to the list to be loaded."));
    parent->FindWindow( ecID_PACKAGES_DIALOG_REMOVE )->SetHelpText(_("Removes one or more packages from the list to be loaded."));
    parent->FindWindow( ecID_PACKAGES_DIALOG_VERSION )->SetHelpText(_("Displays the version of the selected packages."));
    parent->FindWindow( ecID_PACKAGES_DIALOG_DESCRIPTION )->SetHelpText(_("Displays a description of the selected package (blank if more than one package is selected)."));
    parent->FindWindow( wxID_OK )->SetHelpText(_("Closes the dialog and saves any changes you have made."));
    parent->FindWindow( wxID_CANCEL )->SetHelpText(_("Closes the dialog without saving any changes you have made."));

#if __WXGTK__
    parent->FindWindow( wxID_CONTEXT_HELP )->SetHelpText(_("Invokes context-sensitive help for the clicked-on window."));
#endif

    // Add validators
    parent->FindWindow( ecID_PACKAGES_DIALOG_DESCRIPTION )->SetValidator(wxGenericValidator(& m_packageDescription));
}

void ecPackagesDialog::OnInitDialog(wxInitDialogEvent& event)
{
    InitControls();
}

void ecPackagesDialog::InitControls()
{
	wxListBox* availableList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_AVAILABLE_LIST );
	wxListBox* useList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_USE_LIST );

    // Initialize the controls
    int i;
    for (i = 0; i < GetCount()-1; i++)
    {
        const wxString& str = m_items[i];
        wxListBox* lb = m_arnItems[i] ? useList : availableList;

        lb->Append(str, (void*) i);
    }

    UpdateAddRemoveButtons();
}

void ecPackagesDialog::OnCancel(wxCommandEvent& event)
{
    event.Skip();
}

void ecPackagesDialog::OnOK(wxCommandEvent& event)
{
    event.Skip();
}

void ecPackagesDialog::Add(wxListBox* from, wxListBox* to)
{
	wxArrayInt selections;
	wxStringList selectionsStrings;
	int n = from -> GetSelections( selections );

	if (n > 0)
	{
		int i;
		for (i = 0; i < selections.GetCount(); i++)
		{
			wxString str = from -> GetString(selections[i]);
			selectionsStrings.Add(str);
		}

		// Now delete from one list and remove from t'other
		for (i = 0; i < selectionsStrings.Number(); i++)
		{
			wxString str = selectionsStrings[i];

			// Remove
            int toDelete =  from -> FindString(str);
            int itemIndex = -1;
            if (toDelete > -1)
            {
                itemIndex = (int) from -> GetClientData(toDelete);
                from -> Delete(toDelete);
            }

            wxASSERT (itemIndex > -1);

			// Add
            to -> Append(str, (void*) itemIndex);

            // Select it
            int addedIndex = to->FindString(str);
            to->Select(addedIndex);

		}
        //ClearDescription();
        ClearSelections(* from);
        UpdateHardwareSelectionFlag();
        UpdatePackageDescription();
        to->SetFocus();
    }
}

void ecPackagesDialog::OnAdd(wxCommandEvent& event)
{
	if (m_bHardwarePackageSelected)
    {
		HardwarePackageMessageBox ();
        return;
    }
	wxListBox* availableList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_AVAILABLE_LIST );
	wxListBox* useList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_USE_LIST );

    Add(availableList, useList);

    UpdateAddRemoveButtons();
}

void ecPackagesDialog::OnRemove(wxCommandEvent& event)
{
	if (m_bHardwarePackageSelected)
    {
		HardwarePackageMessageBox ();
        return;
    }

	wxListBox* availableList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_AVAILABLE_LIST );
	wxListBox* useList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_USE_LIST );

    Add(useList, availableList);

    UpdateAddRemoveButtons();
}

void ecPackagesDialog::OnDblClickListBox1(wxCommandEvent& event)
{
	if (m_bHardwarePackageSelected)
    {
		HardwarePackageMessageBox ();
        return;
    }

	wxListBox* availableList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_AVAILABLE_LIST );
	wxListBox* useList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_USE_LIST );

    Add(availableList, useList);

    UpdateAddRemoveButtons();
}

void ecPackagesDialog::OnDblClickListBox2(wxCommandEvent& event)
{
	if (m_bHardwarePackageSelected)
    {
		HardwarePackageMessageBox ();
        return;
    }

	wxListBox* availableList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_AVAILABLE_LIST );
	wxListBox* useList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_USE_LIST );

    Add(useList, availableList);

    UpdateAddRemoveButtons();
}

void ecPackagesDialog::OnClickListBox1(wxCommandEvent& event)
{
	wxListBox* availableList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_AVAILABLE_LIST );
	wxListBox* useList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_USE_LIST );
    
#if 0    
    int sel = event.GetSelection();
    if (sel > -1)
    {
        // TODO: check that this works for multiple-selection listboxes
        DisplayDescription(availableList->GetString(sel));
    }
#endif

    ClearSelections(*useList);
	UpdatePackageDescription ();
	UpdateVersionList ();
	UpdateHardwareSelectionFlag ();
    UpdateAddRemoveButtons();
}

void ecPackagesDialog::OnClickListBox2(wxCommandEvent& event)
{
	wxListBox* availableList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_AVAILABLE_LIST );
	wxListBox* useList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_USE_LIST );

#if 0    
    int sel = event.GetSelection();
    if (sel > -1)
    {
        // TODO: check that this works for multiple-selection listboxes
        DisplayDescription(useList->GetString(sel));
    }
#endif

    ClearSelections(*availableList);
    UpdatePackageDescription ();
	UpdateVersionList ();
	UpdateHardwareSelectionFlag ();
    UpdateAddRemoveButtons();
}

void ecPackagesDialog::OnSelectVersion(wxCommandEvent& event)
{
	wxListBox* availableList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_AVAILABLE_LIST );
	wxListBox* useList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_USE_LIST );
    wxChoice* versionChoice = (wxChoice*) FindWindow( ecID_PACKAGES_DIALOG_VERSION );

    if (-1 == versionChoice->GetSelection ()) // if there is no version selection
		return; // do nothing

	wxListBox * pListBox = NULL;

    wxArrayInt selected1, selected2;
    availableList->GetSelections(selected1);
    useList->GetSelections(selected2);

	int nListSelCount = selected1.GetCount ();
	if (nListSelCount > 0)
	{
		pListBox = availableList;
	}
	else
	{
		nListSelCount = selected2.GetCount ();
		if (nListSelCount)
			pListBox = useList;
	}

	wxASSERT (pListBox);

    if (!pListBox)
        return;

	// retrieve the list box indices of the selected packages

    wxArrayInt* selected = (pListBox == availableList ? & selected1 : & selected2);

    int nIndex;
	for (nIndex = 0; nIndex < nListSelCount; nIndex++) // for each selected package
	{
		// set the package version to that specified in the version combo box
        wxString str = versionChoice->GetString(nIndex);

        // itemIndex is the index into the list of item names. It gets stored with all the listbox items.
        int itemIndex = (int) pListBox->GetClientData((*selected)[nIndex]);
        m_currentVersions[(size_t)itemIndex] = str;
	}
}

void ecPackagesDialog::Insert(const wxString& str, bool added, const wxString& descr, const wxString& version)
{
	wxListBox* availableList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_AVAILABLE_LIST );
	wxListBox* useList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_USE_LIST );

	m_items.Add(str);
	m_descriptions.Add(str);
    m_currentVersions.Add(version);
    m_arnItems.Add(added);

	//(added ? useList : availableList) -> Append(str);
}

bool ecPackagesDialog::IsAdded(const wxString& str)
{
	return (((wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_USE_LIST ))->FindString(str) > -1) ;
}

static int ecPositionInStringList(const wxStringList& list, const wxString& item)
{
    int i;
    for (i = 0 ; i < list.Number(); i++)
        if (list[i] == item)
            return i;
        else
            i ++;
    return -1;
}

void ecPackagesDialog::DisplayDescription(const wxString& item)
{
    wxTextCtrl* descrCtrl = (wxTextCtrl*) FindWindow( ecID_PACKAGES_DIALOG_DESCRIPTION ) ;

    int pos = ecPositionInStringList(m_items, item);
    if (pos > -1)
    {
        wxString descr = m_descriptions[pos];
        descrCtrl->SetValue(descr);
    }
}

void ecPackagesDialog::ClearDescription()
{
    wxTextCtrl* descrCtrl = (wxTextCtrl*) FindWindow( ecID_PACKAGES_DIALOG_DESCRIPTION ) ;
    descrCtrl->SetValue(wxEmptyString);
}

wxString ecPackagesDialog::GetVersion (const wxString& item)
{
    int nCount;
    for (nCount = GetCount() - 1; nCount >= 0; --nCount)
	{
        if (m_items [nCount] == item)
		{
            return m_currentVersions [nCount];
        }
    }
    wxASSERT (FALSE);
    return wxEmptyString;
}

void ecPackagesDialog::UpdateHardwareSelectionFlag()
{
	m_bHardwarePackageSelected = FALSE;

	wxListBox* availableList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_AVAILABLE_LIST );
	wxListBox* useList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_USE_LIST );
	wxListBox * pListBox = NULL;

    wxArrayInt selections1, selections2;
    wxArrayInt* selections = NULL;
    availableList->GetSelections(selections1);
    useList->GetSelections(selections2);

	int nListSelCount = selections1.GetCount ();
	if (nListSelCount)
	{
		pListBox = availableList;
        selections = & selections1;
	}
	else
	{
		nListSelCount = selections2.GetCount ();
		if (nListSelCount)
        {
			pListBox = useList;
            selections = & selections2;
        }
	}

	if (pListBox) // if there are packages selected
	{
		ecConfigToolDoc * pDoc = wxGetApp().GetConfigToolDoc ();

		// retrieve the list box indices of the selected packages

		//int * arnIndices = new int [nListSelCount];
		//pListBox->GetSelItems (nListSelCount, arnIndices);

        int nIndex;
		for (nIndex = 0; nIndex < nListSelCount; nIndex++) // for each selected package
		{
			wxString strPackageAlias = pListBox->GetString((*selections)[nIndex]);

			// check if the package is a hardware package

			//TRACE (_T("Checking '%s' for hardware status\n"), strPackageAlias);
			if (pDoc->GetCdlPkgData ()->is_hardware_package (ecUtils::UnicodeToStdStr (pDoc->GetPackageName (strPackageAlias))))
			{
				m_bHardwarePackageSelected = TRUE;
				break;
			}
		}
	}
}

void ecPackagesDialog::HardwarePackageMessageBox()
{
    // TODO: could give the user the choice of going to the template dialog.
	wxMessageBox (wxT("Add and remove hardware packages by selecting a new hardware template."),
        wxGetApp().GetSettings().GetAppName(), wxICON_EXCLAMATION|wxOK, this);
}

void ecPackagesDialog::UpdatePackageDescription ()
{
    ecConfigToolDoc * pDoc = wxGetApp().GetConfigToolDoc ();

    wxListBox* availableList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_AVAILABLE_LIST );
	wxListBox* useList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_USE_LIST );

	wxListBox * pListBox = NULL;

    wxArrayInt selections1, selections2;
    wxArrayInt* selections = NULL;
    availableList->GetSelections(selections1);
    useList->GetSelections(selections2);

	if (1 == selections1.GetCount ())
    {
		pListBox = availableList;
        selections = & selections1;
    }
	else if (1 == selections2.GetCount ())
    {
		pListBox = useList;
        selections = & selections2;
    }

	if (pListBox && selections)
	{
		int nIndex = (*selections)[0];
		wxString strPackageAlias = pListBox->GetString(nIndex);
		m_packageDescription = pDoc->GetCdlPkgData ()->get_package_description (ecUtils::UnicodeToStdStr (pDoc->GetPackageName (strPackageAlias))).c_str ();
		m_packageDescription = ecUtils::StripExtraWhitespace (m_packageDescription);
	}
	else
	{
		m_packageDescription = wxEmptyString;
	}
    TransferDataToWindow ();
}

void ecPackagesDialog::UpdateVersionList ()
{
    ecConfigToolDoc * pDoc = wxGetApp().GetConfigToolDoc ();

    wxListBox* availableList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_AVAILABLE_LIST );
	wxListBox* useList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_USE_LIST );
    wxChoice* versionChoice = (wxChoice*) FindWindow( ecID_PACKAGES_DIALOG_VERSION );

    versionChoice->Clear(); // clear the version combo box

    wxArrayInt selections1, selections2;
    wxArrayInt* selections = NULL;
    availableList->GetSelections(selections1);
    useList->GetSelections(selections2);
    wxListBox* pListBox = NULL;

	if (selections1.GetCount () > 0)
    {
		pListBox = availableList;
        selections = & selections1;
    }
	else if (selections2.GetCount () > 0)
    {
		pListBox = useList;
        selections = & selections2;
    }

	if (pListBox) // if there are packages selected
	{
		std::list<std::string> common_versions;
		bool bCommonSelectedVersion = true;
		int nCommonVersionIndex=-1;
        int nListSelCount = selections->GetCount();

		// retrieve the list box indices of the selected packages

		//int * arnIndices = new int [nListSelCount];
		//pListBox->GetSelItems (nListSelCount, arnIndices);
        int nIndex;
		for (nIndex = 0; nIndex < nListSelCount; nIndex++) // for each selected package
		{
			// retrieve the first package alias
			wxString strPackageAlias = pListBox->GetString ((*selections)[nIndex]);

			// retrieve the dialog item array index for use in
			// comparing current version strings
			const int nVersionIndex = (int) pListBox->GetClientData ((*selections)[nIndex]);

			// retrieve the installed version array

			//TRACE (_T("Retrieving versions for '%s'\n"), strPackageAlias);
			const std::vector<std::string>& versions = pDoc->GetCdlPkgData ()->get_package_versions (ecUtils::UnicodeToStdStr (pDoc->GetPackageName (strPackageAlias)));

			if (0 == nIndex) // if this is the first selected package
			{
				// use the version array to initialise a linked list of version
				// strings held in common between the selected packages
                unsigned int uCount;
				for (uCount = 0; uCount < versions.size (); uCount++)
				{
					//TRACE (_T("Adding common version '%s'\n"), wxString (versions [uCount].c_str ()));
					common_versions.push_back (versions [uCount]);
				}
				nCommonVersionIndex = nVersionIndex; // save the item array index
			}
			else // this is not the first selected package
			{
				std::list<std::string>::iterator i_common_versions = common_versions.begin ();
				while (i_common_versions != common_versions.end ()) // iterate through the common versions
				{
					if (versions.end () == std::find (versions.begin (), versions.end (), * i_common_versions)) // if the common version is not in the versions list
					{
						//TRACE (_T("Removing common version '%s'\n"), CString (i_common_versions->c_str ()));
						common_versions.erase (i_common_versions++); // remove the version from the common versions list
					}
					else
					{
						i_common_versions++;
					}
				}
				if (bCommonSelectedVersion) // if the selected versions of all preceding packages are identical
				{
					// check if the selected version of this package matches that of the preceding ones
					bCommonSelectedVersion = (m_currentVersions [nVersionIndex] == m_currentVersions [nCommonVersionIndex]);
				}
			}
		}

		// add the common versions to the version combo box

		std::list<std::string>::iterator i_common_versions;
		for (i_common_versions = common_versions.begin (); i_common_versions != common_versions.end (); i_common_versions++)
		{
			//TRACE (_T("Adding version '%s'\n"), CString (i_common_versions->c_str ()));
			versionChoice->Append(wxString (i_common_versions->c_str ()));
		}

		// select the common current version (if any) in the version combo box

		if (bCommonSelectedVersion)
		{
			//TRACE (_T("Selecting version '%s'\n"), m_arstrVersions [nCommonVersionIndex]);
			versionChoice->SetStringSelection (m_currentVersions [nCommonVersionIndex]);
		}

		// enable the version combo box only if there are multiple common versions

		versionChoice->Enable (common_versions.size () > 1);
	}
	else // there are no packages selected
	{
		versionChoice->Enable (FALSE); // disable the version combo box
	}
}

void ecPackagesDialog::UpdateAddRemoveButtons()
{
    wxListBox* availableList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_AVAILABLE_LIST );
	wxListBox* useList = (wxListBox*) FindWindow( ecID_PACKAGES_DIALOG_USE_LIST );

    wxArrayInt selections1, selections2;
    availableList->GetSelections(selections1);
    useList->GetSelections(selections2);

    FindWindow( ecID_PACKAGES_DIALOG_ADD )->Enable( selections1.GetCount() > 0 );
    FindWindow( ecID_PACKAGES_DIALOG_REMOVE )->Enable( selections2.GetCount() > 0 );
}

void ecPackagesDialog::ClearSelections(wxListBox& lbox)
{
    int i;
    for (i = 0; i < lbox.Number(); i++)
    {
        lbox.Deselect(i);
    }
}
