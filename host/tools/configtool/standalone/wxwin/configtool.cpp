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
// configtool.cpp :
//
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   julians
// Contact(s):  julians
// Date:        2000/08/24
// Version:     $Id: configtool.cpp,v 1.35 2001/06/18 14:41:13 julians Exp $
// Purpose:
// Description: Implementation file for the ConfigTool application class
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
    #pragma implementation "configtool.h"
#endif

// Includes other headers for precompiled compilation
#include "ecpch.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "eCosSocket.h"
#include "eCosTestPlatform.h"

#include "wx/splash.h"
#include "wx/cshelp.h"
#include "wx/image.h"
#include "wx/filesys.h"
#include "wx/fs_zip.h"
#include "wx/config.h"
#include "wx/cmdline.h"
#include "wx/process.h"
#include "wx/mimetype.h"
#include "wx/txtstrm.h"

#include "configtool.h"
#include "configtoolview.h"
#include "configtree.h"
#include "mainwin.h"
#include "outputwin.h"
#include "configtooldoc.h"
#include "aboutdlg.h"
#include "shortdescrwin.h"
#include "conflictwin.h"
#include "propertywin.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------
// the application icon
#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "bitmaps/configtool.xpm"
#endif

// Create a new application object.
IMPLEMENT_APP(ecApp)

BEGIN_EVENT_TABLE(ecApp, wxApp)
// Don't handle automatically, or it will bypass more specific processing.
//    EVT_MENU(ecID_WHATS_THIS, ecApp::OnWhatsThis)
END_EVENT_TABLE()

bool ecApp::sm_arMounted[26]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static const wxCmdLineEntryDesc sg_cmdLineDesc[] =
{
/*
    { wxCMD_LINE_SWITCH, "v", "verbose", "be verbose" },
    { wxCMD_LINE_SWITCH, "q", "quiet",   "be quiet" },

    { wxCMD_LINE_OPTION, "o", "output",  "output file" },
    { wxCMD_LINE_OPTION, "i", "input",   "input dir" },
    { wxCMD_LINE_OPTION, "s", "size",    "output block size", wxCMD_LINE_VAL_NUMBER },
    { wxCMD_LINE_OPTION, "d", "date",    "output file date", wxCMD_LINE_VAL_DATE },
*/

    { wxCMD_LINE_PARAM,  NULL, NULL, "input file 1", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_PARAM,  NULL, NULL, "input file 2", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

    { wxCMD_LINE_NONE }
};

ecApp::ecApp()
{
    m_docManager = NULL;
    m_mainFrame = NULL;
    m_currentDoc = NULL;
    m_whatsThisMenu = new wxMenu;
    m_whatsThisMenu->Append(ecID_WHATS_THIS, _("&What's This?"));
    m_helpFile = wxEmptyString;
    m_splashScreen = NULL;
    m_pipedProcess = NULL;
    m_valuesLocked = 0;
    m_helpController = NULL;
}

ecApp::~ecApp()
{
    delete m_whatsThisMenu;
}

// 'Main program' equivalent: the program execution "starts" here
bool ecApp::OnInit()
{
    wxLog::SetTimestamp(NULL);

    CeCosSocket::Init();
    CeCosTestPlatform::Load();
    
    wxHelpProvider::Set(new wxSimpleHelpProvider);
    //wxHelpProvider::Set(new wxHelpControllerHelpProvider(& m_helpController));

    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxGIFHandler);

    // Required for advanced HTML help
#if wxUSE_STREAMS && wxUSE_ZIPSTREAM && wxUSE_ZLIB
      wxFileSystem::AddHandler(new wxZipFSHandler);
#endif

    wxString currentDir = wxGetCwd();

    // Use argv to get current app directory
    m_appDir = wxFindAppPath(argv[0], currentDir, wxT("CONFIGTOOLDIR"));
    
    // If the development version, go up a directory.
#ifdef __WXMSW__
    if ((m_appDir.Right(5).CmpNoCase("DEBUG") == 0) ||
        (m_appDir.Right(11).CmpNoCase("DEBUGSTABLE") == 0) ||
        (m_appDir.Right(7).CmpNoCase("RELEASE") == 0) ||
        (m_appDir.Right(13).CmpNoCase("RELEASESTABLE") == 0)
        )
        m_appDir = wxPathOnly(m_appDir);
#endif

    wxGetEnv(wxT("PATH"), & m_strOriginalPath);

    // Create a document manager
    m_docManager = new wxDocManager;

    // Create a template relating documents to their views
    (void) new wxDocTemplate(m_docManager, "Configtool", "*.ecc", "", "ecc", "Configtool Doc", "Configtool View",
        CLASSINFO(ecConfigToolDoc), CLASSINFO(ecConfigToolView));

    // If we've only got one window, we only get to edit
    // one document at a time.
    m_docManager->SetMaxDocsOpen(1);
    
    // Initialize config settings
    m_settings.Init();

    // Let wxWindows know what the app name is
    SetAppName(m_settings.GetAppName());

    InitializeWindowSettings(TRUE /* beforeWindowConstruction */) ;

    // Load config settings
    m_settings.LoadConfig();

#ifdef __WXMSW__
    wxBitmap bitmap(wxBITMAP(splash));
#else
    wxBitmap bitmap;
    if (wxFileExists("splash16.png"))
        bitmap.LoadFile("splash16.png", wxBITMAP_TYPE_PNG);
#endif
    if (bitmap.Ok() && GetSettings().m_showSplashScreen)
    {
        m_splashScreen = new ecSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
            6000, NULL, -1, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxFRAME_FLOAT_ON_PARENT|wxSTAY_ON_TOP);
    }

    wxYieldIfNeeded();

    // create the main application window
    ecMainFrame *frame = new ecMainFrame(m_docManager, GetSettings().GetAppName(),
        wxPoint(GetSettings().m_frameSize.x, GetSettings().m_frameSize.y),
        wxSize(GetSettings().m_frameSize.width, GetSettings().m_frameSize.height));

#if 0
    // TODO: eCos.hhp/hhc/hhk will eventually be generated as the app starts up.
    if ( !GetHelpController().Initialize(GetFullAppPath(wxT("eCos"))) )
    {
        //wxMessageBox(_("Cannot initialize the help system, aborting."));
        //return FALSE;
    }
#endif

    m_mainFrame = frame;
    SetTopWindow(frame);
    frame->Show(TRUE);
    SendIdleEvents(); // Otherwise UI updates aren't done, because it's busy loading the repository

    InitializeWindowSettings(FALSE /* beforeWindowConstruction */) ;

    if (m_splashScreen)
        m_splashScreen->Raise();

    wxYieldIfNeeded();

    // Parse the command-line parameters and options
    wxCmdLineParser parser(sg_cmdLineDesc, argc, argv);
    int res = parser.Parse();
    if (res == -1 || res > 0)
    {
        parser.Usage();
        return FALSE;
    }
    wxString filenameToOpen1, filenameToOpen2;
    if (parser.GetParamCount() > 0)
    {
        wxString tmpSaveFile;

        filenameToOpen1 = parser.GetParam(0);

        if (parser.GetParamCount() > 1)
            filenameToOpen2 = parser.GetParam(1);

        bool gotRepository = FALSE;
        bool gotSavefile = FALSE;

        wxString repositoryDir, saveFile;

        // Might be e.g. . or .. in path, or relative path
        filenameToOpen1 = wxGetRealPath(currentDir, filenameToOpen1);

        if (parser.GetParamCount() > 1)
            filenameToOpen2 = wxGetRealPath(currentDir, filenameToOpen2);

        wxString path1, name1, ext1;
        wxSplitPath(filenameToOpen1, & path1, & name1, & ext1);

        wxString path2, name2, ext2;
        wxSplitPath(filenameToOpen2, & path2, & name2, & ext2);

        // Look at the first file
        if (ext1 == "ecc" || ext1 == "ECC")
        {
            if (wxFileExists(filenameToOpen1))
            {
                gotSavefile = TRUE;
                saveFile = filenameToOpen1;
            }
            else
            {
                wxString msg;
                msg.Printf("%s is not a valid file.", (const wxChar*) filenameToOpen1);
                wxMessageBox(msg, wxGetApp().GetSettings().GetAppName(), wxICON_EXCLAMATION|wxOK);
                return FALSE;
            }
        }
        else if (wxDirExists(filenameToOpen1) && FindSaveFileInDir(filenameToOpen1, tmpSaveFile))
        {
            saveFile = tmpSaveFile;
            gotSavefile = TRUE;
        }
        else if ((name1 == wxT("ecos") && ext1 == wxT("db") && wxFileExists(filenameToOpen1)) || wxDirExists(filenameToOpen1))
        {
            // It's a repository (we hope).
            if (name1 == wxT("ecos") && ext1 == wxT("db"))
            {
                // Go two steps up
                filenameToOpen1 = wxPathOnly(filenameToOpen1);
                filenameToOpen1 = wxPathOnly(filenameToOpen1);
            }
            else
            {
                // If it's the packages directory, we need to strip off
                // a directory
                wxString eccPath(filenameToOpen1 + wxString(wxFILE_SEP_PATH) + wxT("ecc"));

                // Don't strip off ecc if it's the CVS repository (with ecc below it)
                if (name1 == wxT("packages") || (name1 == wxT("ecc") && !wxDirExists(eccPath)))
                    filenameToOpen1 = wxPathOnly(filenameToOpen1);
            }

            repositoryDir = filenameToOpen1;
            gotRepository = TRUE;
        }
        else
        {
            wxString msg;
            msg.Printf("%s is neither a project file nor a valid repository.", (const wxChar*) filenameToOpen1);
            wxMessageBox(msg, wxGetApp().GetSettings().GetAppName(), wxICON_EXCLAMATION|wxOK);
            return FALSE;
        }

        // Look at the second file, if any
        if (parser.GetParamCount() > 1)
        {
            if (ext2 == "ecc" || ext2 == "ECC")
            {
                if (wxFileExists(filenameToOpen2))
                {
                    if (gotSavefile)
                    {
                        wxString msg;
                        msg.Printf("%s is a second save file -- please supply only one.", (const wxChar*) filenameToOpen2);
                        wxMessageBox(msg, wxGetApp().GetSettings().GetAppName(), wxICON_EXCLAMATION|wxOK);
                        return FALSE;
                    }
                    
                    gotSavefile = TRUE;
                    saveFile = filenameToOpen2;
                }
                else
                {
                    wxString msg;
                    msg.Printf("%s is not a valid file.", (const wxChar*) filenameToOpen2);
                    wxMessageBox(msg, wxGetApp().GetSettings().GetAppName(), wxICON_EXCLAMATION|wxOK);
                    return FALSE;
                }
            }
            else if (!gotSavefile && wxDirExists(filenameToOpen2) && FindSaveFileInDir(filenameToOpen2, tmpSaveFile))
            {
                saveFile = tmpSaveFile;
                gotSavefile = TRUE;
            }
            else if ((name2 == wxT("ecos") && ext2 == wxT("db") && wxFileExists(filenameToOpen2)) || wxDirExists(filenameToOpen2))
            {
                if (gotRepository)
                {
                    wxString msg;
                    msg.Printf("%s is a second repository -- please supply only one.", (const wxChar*) filenameToOpen2);
                    wxMessageBox(msg, wxGetApp().GetSettings().GetAppName(), wxICON_EXCLAMATION|wxOK);
                    return FALSE;
                }           
                
                // It's a repository (we hope).
                if (name1 == wxT("ecos") && ext1 == wxT("db"))
                {
                    // Go two steps up
                    filenameToOpen2 = wxPathOnly(filenameToOpen2);
                    filenameToOpen2 = wxPathOnly(filenameToOpen2);
                }
                else
                {
                    // If it's the packages directory, we need to strip off
                    // a directory
                    wxString eccPath(filenameToOpen2 + wxString(wxFILE_SEP_PATH) + wxT("ecc"));
                    
                    // Don't strip off ecc if it's the CVS repository (with ecc below it)
                    if (name2 == wxT("packages") || (name2 == wxT("ecc") && !wxDirExists(eccPath)))
                        filenameToOpen2 = wxPathOnly(filenameToOpen2);
                }
                
                repositoryDir = filenameToOpen2;
                gotRepository = TRUE;
            }
            else
            {
                wxString msg;
                msg.Printf("%s is neither a project file nor a valid repository.", (const wxChar*) filenameToOpen2);
                wxMessageBox(msg, wxGetApp().GetSettings().GetAppName(), wxICON_EXCLAMATION|wxOK);
                return FALSE;
            }
        }
        
        // Now we have looked at the two files and decided what they are; let's
        // act on it.

        if (gotRepository)
        {
            GetSettings().m_strRepository = repositoryDir;
        }

        if (!gotSavefile)
        {
            // See if there's a save file in the current dir
            if (FindSaveFileInDir(currentDir, saveFile))
                gotSavefile = TRUE;
        }

        if (gotSavefile)
        {
            // The repository will be loaded from m_strRepository, possibly set above.
            m_docManager->CreateDocument(saveFile, wxDOC_SILENT);
        }
        else
        {
            // Create a new file
            m_docManager->CreateDocument(wxString(""), wxDOC_NEW);
        }
    }
    else
    {
        m_docManager->CreateDocument(wxString(""), wxDOC_NEW);
    }

    return TRUE;
}

// Initialize window settings object
bool ecApp::InitializeWindowSettings(bool beforeWindowConstruction)
{
    wxWindowSettings& settings = GetSettings().m_windowSettings;
    ecMainFrame* frame = GetMainFrame();

    if (beforeWindowConstruction)
    {
        settings.Add(wxT("Configuration"));       
        settings.Add(wxT("Short Description"));
        settings.Add(wxT("Output"));
        settings.Add(wxT("Properties"));
        settings.Add(wxT("Conflicts"));
    }
    else
    {
        wxArrayPtrVoid arr;
        arr.Add(frame->GetTreeCtrl());
        arr.Add(frame->GetValueWindow());
        settings.SetWindows(wxT("Configuration"), arr);

        settings.SetWindow(wxT("Short Description"), frame->GetShortDescriptionWindow());
        settings.SetWindow(wxT("Output"), frame->GetOutputWindow());
        settings.SetWindow(wxT("Properties"), frame->GetPropertyListWindow());
        settings.SetWindow(wxT("Conflicts"), frame->GetConflictsWindow());
    }

    return TRUE;
}


bool ecApp::InitializeHelpController()
{
    if (m_helpController)
        delete m_helpController;
    m_helpController = new wxHelpController;

    if (!m_helpController->Initialize(GetHelpFile()))
    {
        // TODO
        return FALSE;
    }
    else
    {
        // Tell the help controller where the repository documentation is.
        // For now, just keep this to myself since it uses hacks to wxWin
        ecConfigToolDoc* doc = GetConfigToolDoc();

        // No longer using relative paths
#if 0
        if (doc)
        {
            wxString htmlDir = wxString(doc->GetRepository()) + wxString(wxT("/doc/html"));
            if (!wxDirExists(htmlDir))
                htmlDir = wxString(doc->GetRepository()) + wxString(wxT("/doc"));

            htmlDir += wxString(wxT("/"));

            wxGetApp().GetHelpController().SetBookBasePath(htmlDir);
        }
#endif
        return TRUE;
    }
}

// Check if there is a (unique) .ecc file in dir
bool ecApp::FindSaveFileInDir(const wxString& dir, wxString& saveFile)
{
    wxDir fileFind;
    
    if (!fileFind.Open(dir))
        return FALSE;
    
    wxString wildcard = wxT(".ecc");   
    wxString filename;

    bool found = fileFind.GetFirst (& filename, wildcard);
    if (found)
    {
        // Fail if there was more than one matching file.
        wxString filename2;
        if (fileFind.GetNext (& filename2))
            return FALSE;
        else
        {
            saveFile = dir + wxString(wxFILE_SEP_PATH) + filename;
            return TRUE;
        }
    }
    return FALSE;
}

int ecApp::OnExit(void)
{
/*
    if (m_helpController)
    {
        delete m_helpController;
        m_helpController = NULL;
    }
*/

    if (m_splashScreen)
    {
        m_splashScreen->Destroy();
        m_splashScreen = NULL;
    }

    m_settings.SaveConfig();

    {
        wxConfig config(wxGetApp().GetSettings().GetConfigAppName(), wxT("Red Hat"));
        m_docManager->FileHistorySave(config);
    }

    delete m_docManager;

    return 0;
}

// Prepend the current program directory to the name
wxString ecApp::GetFullAppPath(const wxString& filename) const
{
    wxString path(m_appDir);
    if (path.Last() != '\\' && path.Last() != '/' && filename[0] != '\\' && filename[0] != '/')
#ifdef __WXGTK__
        path += '/';
#else
        path += '\\';
#endif
    path += filename;
    
    return path;
}

// Are we running in 32K colours or more?
bool ecApp::GetHiColour() const
{
    static bool hiColour = (wxDisplayDepth() >= 16) ;
    return hiColour;
}

// General handler for 'What's this?'
void ecApp::OnWhatsThis(wxCommandEvent& event)
{
    wxObject* obj = event.GetEventObject();
    wxWindow* win = NULL;

    if (obj->IsKindOf(CLASSINFO(wxMenu)))
    {
        win = ((wxMenu*) obj)->GetInvokingWindow();
    }
    else if (obj->IsKindOf(CLASSINFO(wxWindow)))
    {
        win = (wxWindow*) obj;
    }

    wxWindow* subjectOfHelp = win;
    bool eventProcessed = FALSE;
    wxPoint pt = wxGetMousePosition();

    while (subjectOfHelp && !eventProcessed)
    {
        wxHelpEvent helpEvent(wxEVT_HELP, subjectOfHelp->GetId(), pt) ;
        helpEvent.SetEventObject(this);
        eventProcessed = win->GetEventHandler()->ProcessEvent(helpEvent);
        
        // Go up the window hierarchy until the event is handled (or not).
        // I.e. keep submitting ancestor windows until one is recognised
        // by the app code that processes the ids and displays help.
        subjectOfHelp = subjectOfHelp->GetParent();
    }
    // wxGetApp().GetHelpController().DisplayTextPopup(GetHelpText(), wxGetMousePosition());
}

// Log to output window
void ecApp::Log(const wxString& msg)
{
    ecMainFrame* frame = (ecMainFrame*) GetTopWindow();
    if (frame)
    {
        // TODO is SetInsertionPointEnd necessary?
        frame->GetOutputWindow()->SetInsertionPointEnd();
        frame->GetOutputWindow()->AppendText(msg /* + wxT("\n") */ );
        if ((msg == wxEmptyString) || (msg.Last() != wxT('\n')))
            frame->GetOutputWindow()->AppendText(wxT("\n"));
    }
}

void ecApp::SetStatusText(const wxString& text, bool clearFailingRulesPane)
{
    ecMainFrame* mainFrame = GetMainFrame();
    if(mainFrame)
    {
        mainFrame->GetStatusBar()->SetStatusText(text, ecStatusPane);
        if (clearFailingRulesPane)
            mainFrame->GetStatusBar()->SetStatusText(wxT(""), ecFailRulePane);
#ifdef __WXMSW__
        wxYield();
#endif
    }
}

// Config tree control
ecConfigTreeCtrl* ecApp::GetTreeCtrl() const
{
    return GetMainFrame()->GetTreeCtrl();
}

// MLT window
ecMemoryLayoutWindow* ecApp::GetMLTWindow() const
{
    return GetMainFrame()->GetMemoryLayoutWindow();
}

// Get active document
ecConfigToolDoc* ecApp::GetConfigToolDoc() const
{
    if (m_currentDoc)
        return m_currentDoc;

    if (!m_docManager)
        return NULL;

    return wxDynamicCast(m_docManager->GetCurrentDocument(), ecConfigToolDoc);
}

bool ecApp::Launch(const wxString & strFileName,const wxString &strViewer)
{
    bool ok = FALSE;
    wxString cmd;

    if (!strViewer.IsEmpty())
    {
        cmd = strViewer + wxString(wxT(" ")) + strFileName ;
    }
    else
    {
        wxString path, filename, ext;
        wxSplitPath(strFileName, & path, & filename, & ext);
        
        wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
        if ( !ft )
        {
            wxLogError(_T("Impossible to determine the file type for extension '%s'"),
                ext.c_str());
            return FALSE;
        }
        
        bool ok = ft->GetOpenCommand(&cmd,
            wxFileType::MessageParameters(strFileName, _T("")));
        delete ft;
        
        if (!ok)
        {
            // TODO: some kind of configuration dialog here.
            wxMessageBox(_("Could not determine the command for opening this file."),
                wxGetApp().GetSettings().GetAppName(), wxOK|wxICON_EXCLAMATION);
            return FALSE;
        }
    }        

    ok = (wxExecute(cmd, FALSE) != 0);
    
    return ok;

#if 0    
    bool rc=false;
    
    if(!strViewer.IsEmpty())//use custom editor
    {
        CString strCmdline(strViewer);
        
        TCHAR *pszCmdLine=strCmdline.GetBuffer(strCmdline.GetLength());
        GetShortPathName(pszCmdLine,pszCmdLine,strCmdline.GetLength());
        strCmdline.ReleaseBuffer();
        
        strCmdline+=_TCHAR(' ');
        strCmdline+=strFileName;
        PROCESS_INFORMATION pi;
        STARTUPINFO si;
        
        si.cb = sizeof(STARTUPINFO); 
        si.lpReserved = NULL; 
        si.lpReserved2 = NULL; 
        si.cbReserved2 = 0; 
        si.lpDesktop = NULL; 
        si.dwFlags = 0; 
        si.lpTitle=NULL;
        
        if(CreateProcess(
            NULL, // app name
            //strCmdline.GetBuffer(strCmdline.GetLength()),    // command line
            strCmdline.GetBuffer(strCmdline.GetLength()),    // command line
            NULL, // process security
            NULL, // thread security
            TRUE, // inherit handles
            0,
            NULL, // environment
            NULL, // current dir
            &si, // startup info
            &pi)){
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            rc=true;
        } else {
            CUtils::MessageBoxF(_T("Failed to invoke %s.\n"),strCmdline);
        }
        strCmdline.ReleaseBuffer();
    } else {// Use association
        TCHAR szExe[MAX_PATH];
        HINSTANCE h=FindExecutable(strFileName,_T("."),szExe);
        if(int(h)<=32){
            CString str;
            switch(int(h)){
            case 0:  str=_T("The system is out of memory or resources.");break;
            case 31: str=_T("There is no association for the specified file type.");break;
            case ERROR_FILE_NOT_FOUND: str=_T("The specified file was not found.");break;
            case ERROR_PATH_NOT_FOUND: str=_T("The specified path was not found.");break;
            case ERROR_BAD_FORMAT:     str=_T("The .EXE file is invalid (non-Win32 .EXE or error in .EXE image).");break;
            default: break;
            }
            CUtils::MessageBoxF(_T("Failed to open document %s.\r\n%s"),strFileName,str);
        } else {
            
            SHELLEXECUTEINFO sei = {sizeof(sei), 0, AfxGetMainWnd()->GetSafeHwnd(), _T("open"),
                strFileName, NULL, NULL, SW_SHOWNORMAL, AfxGetInstanceHandle( )};
            
            sei.hInstApp=0;
            HINSTANCE hInst=ShellExecute(AfxGetMainWnd()->GetSafeHwnd(),_T("open"), strFileName, NULL, _T("."), 0)/*ShellExecuteEx(&sei)*/;
            if(int(hInst)<=32/*sei.hInstApp==0*/)
            {
                CString str;
                switch(int(hInst))
                {
                case 0 : str=_T("The operating system is out of memory or resources. ");break;
                case ERROR_FILE_NOT_FOUND : str=_T("The specified file was not found. ");break;
                case ERROR_PATH_NOT_FOUND : str=_T("The specified path was not found. ");break;
                case ERROR_BAD_FORMAT : str=_T("The .EXE file is invalid (non-Win32 .EXE or error in .EXE image). ");break;
                case SE_ERR_ACCESSDENIED : str=_T("The operating system denied access to the specified file. ");break;
                case SE_ERR_ASSOCINCOMPLETE : str=_T("The filename association is incomplete or invalid. ");break;
                case SE_ERR_DDEBUSY : str=_T("The DDE transaction could not be completed because other DDE transactions were being processed. ");break;
                case SE_ERR_DDEFAIL : str=_T("The DDE transaction failed. ");break;
                case SE_ERR_DDETIMEOUT : str=_T("The DDE transaction could not be completed because the request timed out. ");break;
                case SE_ERR_DLLNOTFOUND : str=_T("The specified dynamic-link library was not found. ");break;
                    //case SE_ERR_FNF : str=_T("The specified file was not found. ");break;
                case SE_ERR_NOASSOC : str=_T("There is no application associated with the given filename extension. ");break;
                case SE_ERR_OOM : str=_T("There was not enough memory to complete the operation. ");break;
                    //case SE_ERR_PNF : str=_T("The specified path was not found. ");break;
                case SE_ERR_SHARE : str=_T("A sharing violation occurred. ");break;
                default: str=_T("An unexpected error occurred");break;
                }
                CUtils::MessageBoxF(_T("Failed to open document %s using %s.\r\n%s"),strFileName,szExe,str);
            } else {
                rc=true;
            }
        }
    }
    return rc;
#endif
}

bool ecApp::PrepareEnvironment(bool bWithBuildTools, wxString* cmdLine)
{
#ifdef __WXMSW__
    // Under Windows we can set variables.
    ecConfigToolDoc *pDoc = GetConfigToolDoc();
    
    wxSetEnv(wxT("PATH"), m_strOriginalPath);
    
    const wxString strPrefix(pDoc->GetCurrentTargetPrefix());
    ecFileName strBinDir;
    bool rc = FALSE;

    rc=(! bWithBuildTools) || GetSettings().m_arstrBinDirs.Find(strPrefix, strBinDir);
    if(!rc){
        wxCommandEvent event;
        GetMainFrame()->OnBuildToolsPath(event);
        rc = GetSettings().m_arstrBinDirs.Find(strPrefix, strBinDir);
    }
    
    if (rc)
    {
        ecFileName strUserBinDir(GetSettings().m_userToolsDir);
        if(strUserBinDir.IsEmpty())
        {
            if ( 1 == GetSettings().m_userToolPaths.GetCount() )
            {
                GetSettings().m_userToolsDir = GetSettings().m_userToolPaths[0];
            } else
            {
                wxCommandEvent event;
                GetMainFrame()->OnUserToolsPath(event);
            }
            strUserBinDir = GetSettings().m_userToolsDir;
        }
        if ( !strUserBinDir.IsEmpty() )
        {
            // calculate the directory of the host tools from this application's module name
            ecFileName strHostToolsBinDir(this->argv[0]);
            strHostToolsBinDir = strHostToolsBinDir.Head ();
            
            // tools directories are in the order host-tools, user-tools, comp-tools, install/bin (if present), contrib-tools (if present) on the path
            const ecFileName strContribBinDir(strUserBinDir, wxT("..\\contrib\\bin"));
            ecFileName strUsrBinDir(strUserBinDir, wxT("..\\usr\\bin"));
            const ecFileName strInstallBinDir(pDoc->GetInstallTree (), wxT("bin"));

            // In case strUserBinDir is e.g. c:\program files\red hat\cygwin-00r1\usertools\h-i686-pc-cygwin\bin
            if (!strUsrBinDir.IsDir ())
                strUsrBinDir = ecFileName(strUserBinDir + _T("..\\..\\..\\H-i686-pc-cygwin\\bin"));

            if (
                (strUsrBinDir.IsDir ()     && ! ecUtils::AddToPath (strUsrBinDir)) || 
                (strContribBinDir.IsDir () && ! ecUtils::AddToPath (strContribBinDir)) || 
                (strInstallBinDir.IsDir () && ! ecUtils::AddToPath (strInstallBinDir)) || 
                (bWithBuildTools && ! ecUtils::AddToPath (strBinDir)) || 
                ! ecUtils::AddToPath (strUserBinDir) || 
                ! ecUtils::AddToPath (strHostToolsBinDir))
            {
                wxString msg;
                msg.Printf(wxT("Failed to set PATH environment variable"));
                wxMessageBox(msg, wxGetApp().GetSettings().GetAppName(), wxICON_EXCLAMATION|wxOK);
                rc = FALSE;
            } else
            {
                if(!wxSetEnv(_T("MAKE_MODE"),_T("unix")))
                {
                    wxString msg;
                    msg.Printf(wxT("Failed to set MAKE_MODE environment variable"));
                    wxMessageBox(msg, wxGetApp().GetSettings().GetAppName(), wxICON_EXCLAMATION|wxOK);
                    rc = FALSE;
                } else
                {
                    // Remove from the environment
                    wxUnsetEnv(wxT("GDBTK_LIBRARY"));
                    wxUnsetEnv(wxT("GCC_EXEC_PREFIX"));

                    // Useful for ecosconfig
                    wxSetEnv(wxT("ECOS_REPOSITORY"), pDoc->GetPackagesDir());

                    if (! pDoc->GetBuildTree().IsEmpty())
                        CygMount(pDoc->GetBuildTree()[0]);
                    if (! pDoc->GetInstallTree().IsEmpty())
                        CygMount(pDoc->GetInstallTree()[0]);
                    if (! pDoc->GetRepository().IsEmpty())
                        CygMount(pDoc->GetRepository()[0]);
                }
            }
        }
    }
    return rc;
#else
    wxASSERT ( cmdLine != NULL );

    (* cmdLine) = wxEmptyString;

    // Under Unix we need to build up a command line to set variables and invoke make
    ecConfigToolDoc *pDoc = GetConfigToolDoc();
    
    const wxString strPrefix(pDoc->GetCurrentTargetPrefix());
    ecFileName strBinDir;
    bool rc = FALSE;

    rc=(! bWithBuildTools) || GetSettings().m_arstrBinDirs.Find(strPrefix, strBinDir);
    if(!rc){
        wxCommandEvent event;
        GetMainFrame()->OnBuildToolsPath(event);
        rc = GetSettings().m_arstrBinDirs.Find(strPrefix, strBinDir);
    }
    
    if (rc)
    {
        if (!strBinDir.IsEmpty())
        {
            (* cmdLine) += wxString(wxT("export PATH=")) + strBinDir + wxT(":$PATH; ");

            // Also set the path
            wxString oldPath(wxGetenv(wxT("PATH")));
            wxString path(strBinDir);
            if (!oldPath.IsEmpty())
                path += oldPath;
            wxSetEnv(wxT("PATH"), path);
        }
        (* cmdLine) += wxString(wxT("unset GDBTK_LIBRARY; ")) ;
        wxUnsetEnv(wxT("GDBTK_LIBRARY"));

        (* cmdLine) += wxString(wxT("unset GCC_EXEC_PREFIX; ")) ;
        wxUnsetEnv(wxT("GCC_EXEC_PREFIX"));

        (* cmdLine) += wxString(wxT("export ECOS_REPOSITORY=")) + pDoc->GetPackagesDir()+ wxT("; ");
        wxSetEnv(wxT("ECOS_REPOSITORY"), pDoc->GetPackagesDir());

#if 0
        ecFileName strUserBinDir(GetSettings().m_userToolsDir);
        if(strUserBinDir.IsEmpty())
        {
            if ( 1 == GetSettings().m_userToolPaths.GetCount() )
            {
                GetSettings().m_userToolsDir = GetSettings().m_userToolPaths[0];
            } else
            {
                wxCommandEvent event;
                GetMainFrame()->OnUserToolsPath(event);
            }
            strUserBinDir = GetSettings().m_userToolsDir;
        }
        if ( !strUserBinDir.IsEmpty() )
        {
            // calculate the directory of the host tools from this application's module name
            ecFileName strHostToolsBinDir(this->argv[0]);
            strHostToolsBinDir = strHostToolsBinDir.Head ();
            
            // tools directories are in the order host-tools, user-tools, comp-tools, install/bin (if present), contrib-tools (if present) on the path

            // TODO: is this right? Assuming that the user tools are already in the user's path.
            // const ecFileName strContribBinDir(strUserBinDir, wxT("..\\contrib\\bin"));
            // const ecFileName strUsrBinDir(strUserBinDir, wxT("..\\usr\\bin"));
            const ecFileName strInstallBinDir(pDoc->GetInstallTree (), wxT("bin"));

            (* cmdLine) += wxString(wxT("export PATH=")) + strInstallBinDir + wxT(":$PATH; ");
            (* cmdLine) += wxString(wxT("unset GDBTK_LIBRARY; ")) ;
            (* cmdLine) += wxString(wxT("unset GCC_EXEC_PREFIX; ")) ;
            (* cmdLine) += wxString(wxT("export ECOS_REPOSITORY=")) + pDoc->GetPackagesDir()+ wxT("; ");
        }
#endif
    }
    return rc;
#endif
}

void ecApp::CygMount(wxChar c)
{
    wxASSERT(wxIsalpha(c));
    c = wxTolower(c);
    if(!sm_arMounted[c-wxChar('a')])
    {
        sm_arMounted[c-wxChar('a')] = TRUE;
        wxString strCmd;
        
        strCmd.Printf(wxT("mount.exe %c: /%c"),c,c);

        wxExecute(strCmd, TRUE);
    }
}

void ecApp::Build(const wxString &strWhat /*=wxT("")*/ )
{
    if (m_pipedProcess)
        return;

    ecConfigToolDoc* pDoc = GetConfigToolDoc();
    if (!pDoc)
        return;

    if (!wxGetApp().GetMainFrame()->GetOutputWindow()->IsShown())
    {
        wxGetApp().GetMainFrame()->ToggleWindow(ecID_TOGGLE_OUTPUT);
    }
    if (!pDoc->GetDocumentSaved())
    {
        pDoc->SaveAs();
    }
/*
    if (pDoc->IsModified() && !wxDirExists(pDoc->GetBuildTree()))
    {
        pDoc->SaveAs();
    }
*/
//    if ( !(pDoc->IsModified() || !wxDirExists(pDoc->GetBuildTree())) ) // verify the save worked
    if ( pDoc->GetDocumentSaved() )
    {
        //wxString strCmd (wxT("c:\\bin\\testmake.bat"));
        wxString strCmd (wxT("make"));
        if(!strWhat.IsEmpty())
        {
            strCmd += wxT(' ');
            strCmd += strWhat;
        }

        if(!GetSettings().m_strMakeOptions.IsEmpty())
        {
            strCmd += wxT(' ');
            strCmd += GetSettings().m_strMakeOptions;
        }
        strCmd += wxT(" --directory ");
        strCmd += pDoc->GetBuildTree();

        wxString variableSettings;

        if (PrepareEnvironment(TRUE, & variableSettings))
        {
#ifdef __WXMSW__
            // strCmd is all we need
#else
            // strCmd has to invoke a shell with variables and make invocation
            strCmd = wxString(wxT("sh -c \"")) + variableSettings + strCmd + wxString(wxT("\""));
#endif
            // Output the command so we know what we're doing
            Log(strCmd);
            Log(wxT("\n"));

            // No: pass --directory
            // wxSetWorkingDirectory(pDoc->GetBuildTree());

            m_pipedProcess = new ecPipedProcess;
            long pid = wxExecute(strCmd, FALSE, m_pipedProcess);
            if ( pid )
            {
                m_pipedProcess->SetPid(pid);
                // wxLogStatus(_T("Process %ld (%s) launched."), pid, cmd.c_str());
            }
            else
            {
                wxLogError(_T("Execution of '%s' failed."), strCmd.c_str());
                
                delete m_pipedProcess;
                m_pipedProcess  = NULL;
            }
        }

#if 0        
        if(PrepareEnvironment())
        {
            m_strBuildTarget=strWhat;
            SetThermometerMax(250); // This is just a guess.  The thread we are about to spawn will work out the correct answer
            m_nLogicalLines=0;
            UpdateThermometer(0);
            CloseHandle(CreateThread(NULL, 0, ThreadFunc, this, 0 ,&m_dwThreadId));
            CString strMsg;
            strMsg.Format(_T("Building %s"),strWhat);
            SetIdleMessage(strMsg);
            
            SetTimer(42,1000,0); // This timer checks for process completion
            SetCurrentDirectory(pDoc->BuildTree());
            m_sp.Run(SubprocessOutputFunc, this, strCmd, false);
            SetIdleMessage();
        }
#endif
    }
#if 0
    
    if(pDoc->IsModified()||pDoc->BuildTree().IsEmpty()){
        SendMessage (WM_COMMAND, ID_FILE_SAVE);
    }
    
    if(!(pDoc->IsModified()||pDoc->BuildTree().IsEmpty())){ // verify the save worked
        CString strCmd (_T("make"));
        if(!strWhat.IsEmpty()){
            strCmd+=_TCHAR(' ');
            strCmd+=strWhat;
        }
        if(!GetApp()->m_strMakeOptions.IsEmpty()){
            strCmd+=_TCHAR(' ');
            strCmd+=GetApp()->m_strMakeOptions;
        }
        
        if(PrepareEnvironment()){
            m_strBuildTarget=strWhat;
            SetThermometerMax(250); // This is just a guess.  The thread we are about to spawn will work out the correct answer
            m_nLogicalLines=0;
            UpdateThermometer(0);
            CloseHandle(CreateThread(NULL, 0, ThreadFunc, this, 0 ,&m_dwThreadId));
            CString strMsg;
            strMsg.Format(_T("Building %s"),strWhat);
            SetIdleMessage(strMsg);
            
            SetTimer(42,1000,0); // This timer checks for process completion
            SetCurrentDirectory(pDoc->BuildTree());
            m_sp.Run(SubprocessOutputFunc, this, strCmd, false);
            SetIdleMessage();
        }
    }
#endif
}

void ecApp::OnProcessTerminated(wxProcess* process)
{
    m_pipedProcess = NULL;
}

// ----------------------------------------------------------------------------
// ecPipedProcess
// ----------------------------------------------------------------------------

bool ecPipedProcess::HasInput()
{
    bool hasInput = FALSE;

    wxInputStream& is = *GetInputStream();
    if ( !is.Eof() )
    {
        wxTextInputStream tis(is);

        // this assumes that the output is always line buffered
        wxString msg;
        msg << tis.ReadLine();

        wxGetApp().Log(msg);

        hasInput = TRUE;
    }

    wxInputStream& es = *GetErrorStream();
    if ( !es.Eof() )
    {
        wxTextInputStream tis(es);

        // this assumes that the output is always line buffered
        wxString msg;
        msg << tis.ReadLine();

        wxGetApp().Log(msg);

        hasInput = TRUE;
    }

    return hasInput;
}

void ecPipedProcess::OnTerminate(int pid, int status)
{
    // show the rest of the output
    while ( HasInput() )
        ;

    wxGetApp().OnProcessTerminated(this);

    //wxLogStatus(m_parent, _T("Process %u ('%s') terminated with exit code %d."),
    //            pid, m_cmd.c_str(), status);

    // we're not needed any more
    delete this;
}

void ecPingTimer::Notify()
{
    static bool s_inNotify = FALSE;

    if (s_inNotify)
        return;

    s_inNotify = TRUE;

    // On Windows, simply having the timer going will ping the message queue
    // and cause idle processing to happen.
    // On Unix, this doesn't happen so we have to do the processing explicitly.
#ifdef __WXMSW__
    // Nothing to do
#else
    // Get some input
    if ( wxGetApp().m_pipedProcess )
        while (wxGetApp().m_pipedProcess->HasInput())
        {
            // Loop while there is still input
        }
#endif

    s_inNotify = FALSE;
}

