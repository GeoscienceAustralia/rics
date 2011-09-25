/*
Author: Nariman Habili

Description: The main frame. Sets the menu and status bar. Help file in pdf
             format is launched here. The Canvas class is instantiated here.

Copyright (c) 2011 Commonwealth of Australia (Geoscience Australia)

This file is part of RICS.

RICS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RICS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RICS.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Frame.h"
#include "play_black.xpm"
#include "play_grey.xpm"
#include "stop_black.xpm"
#include "stop_grey.xpm"
#include "record_red.xpm"
#include "new.xpm"
#include "folder.xpm"
#include "camera.xpm"
#include "map.xpm"
#include "film.xpm"
#include "CameraPropDialog.h"
#include "CameraThread.h"
#include "version.h"
#include <wx/animate.h>
#include <wx/mimetype.h>
#include <boost/lexical_cast.hpp>
#include <windows.h> 
#include <tchar.h> 

namespace rics
{
    Frame::Frame(const wxString& title, const wxPoint& pos, const wxSize& size, Cameras* cameras):
    wxFrame((wxFrame*)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE),
    cameras_(cameras),
    numCameras_(cameras_->size()),
    session_(numCameras_),
    np_(new NotePad),
    cameraPropDialog_(this, wxT("Camera Properties"), cameras),
    gpsPropDialog_(this, wxT("GPS Properties"), &gps_),
    sessionPropDialog_(this, "New Session", &session_, cameras, &db_, &gps_),
    openSessionDialog_(this, &session_, cameras, &db_),
    notePadSetDialog_(this, "Damage Level Settings", np_),
    timer_(this, ID_Timer),
    play_(false)
    {
        //File item
        wxMenu *menuFile = new wxMenu;
        menuFile->Append(ID_NewSession, _T("&New Session\tCtrl-N"), _T("New Session"));
        menuFile->Append(ID_OpenSession, _T("&Open Session\tCtrl-O"), _T("Open Session"));
        menuFile->Append(ID_Test, _T("&Test\tCtrl-T"), _T("Test"));
        menuFile->AppendSeparator();
        menuFile->Append(ID_Quit, _T("E&xit\tCtrl-X"), _T("Exit"));

        //Play item
        wxMenu *playOptions = new wxMenu;
        menuPlay_ = new wxMenuItem(playOptions, ID_Text_Play, _T("&Play\tCtrl-P"), _T("Play"), wxITEM_NORMAL);
        menuStop_ = new wxMenuItem(playOptions, ID_Text_Stop, _T("&Stop\tCtrl-S"), _T("Stop"), wxITEM_NORMAL);
        playOptions->Append(menuPlay_);
        playOptions->Append(menuStop_);
        menuPlay_->Enable(true);
        menuStop_->Enable(false);

        //Option item
        wxMenu *menuOptions = new wxMenu;     
        menuOptions->Append(ID_CameraProperties, _T("&Camera Properties...\tCtrl-C"), _T("Camera Properties..."));
        menuOptions->Append(ID_GPSProperties, _T("&GPS Properties...\tCtrl-G"), _T("GPS Properties..."));
        menuOptions->AppendSeparator();
        menuOptions->Append(ID_NotePadSettings, _T("&Damage Level Settings...\tCtrl-D"), _T("Damage Level Settings..."));
 
        //Help item
        wxMenu *menuHelp = new wxMenu;
        menuHelp->Append(ID_Help, _T("&RICS Help\tF1"), _T("RICS Help"));
        menuHelp->Append(ID_About, _T("&About RICS..."), _T("About RICS..."));

        //Adding menu item to menu bar
        wxMenuBar *menuBar = new wxMenuBar;
        menuBar->Append(menuFile, _T("&File"));
        menuBar->Append(playOptions, _T("&Playback"));
        menuBar->Append(menuOptions, _T("&Options"));
        menuBar->Append(menuHelp, _T("&Help"));
        SetMenuBar(menuBar);

        wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    
        wxToolBar* toolBar = new wxToolBar(this, 
                                           wxID_ANY,
                                           wxDefaultPosition, 
                                           wxDefaultSize, 
                                           wxTB_FLAT | wxTB_HORIZONTAL);
        wxBitmap bmpNew(new_xpm);
        wxBitmap bmpOpen(folder_xpm);
        wxBitmap bmpCamera(camera_xpm);
        wxBitmap bmpGPS(map_xpm);
        wxBitmap bmpTest(film_xpm);
        toolBar->AddTool(ID_NewSession_Icon, bmpNew, wxT("New Session"), wxT("New Session"));
        toolBar->AddTool(ID_OpenSession_Icon, bmpOpen, wxT("Open Session"), wxT("Open Session"));
        toolBar->AddTool(ID_Test_Icon, bmpTest, wxT("Test"), wxT("Test"));
        toolBar->AddSeparator();
        toolBar->AddTool(ID_CameraProperties_Icon, bmpCamera, wxT("Camera Properties"), wxT("Camera Properties"));
        toolBar->AddTool(ID_GPSProperties_Icon, bmpGPS, wxT("GPS Properties"), wxT("GPS Properties"));
        toolBar->Realize();
        frameSizer->Add(toolBar, 0, wxEXPAND, 5);

        wxBitmap play(play_black_xpm);
        wxBitmap play_grey(play_grey_xpm);
        wxBitmap stop(stop_black_xpm);
        wxBitmap stop_grey(stop_grey_xpm);
        wxBitmap r1(record_red_xpm);

        //Set the play tool bar
        playToolBar_ = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTB_FLAT | wxTB_HORIZONTAL);
        playToolBar_->SetToolBitmapSize(wxSize(16, 16));
        playToolBar_->AddTool(ID_Icon_Stop, wxT("Stop"), stop, stop_grey, wxITEM_NORMAL, wxT("Stop"));
        playToolBar_->AddTool(ID_Icon_Play, wxT("Play"), play, play_grey, wxITEM_NORMAL, wxT("Play"));
        wxStaticBitmap* nullImage = new wxStaticBitmap(playToolBar_, wxID_ANY, wxNullBitmap, wxPoint(100, -1), wxSize(335, 100), 0);
        playToolBar_->AddControl(nullImage);
        playToolBar_->AddTool(ID_Record, wxT("Record"), r1, wxT("Stop"), wxITEM_NORMAL);
        playToolBar_->SetToolDisabledBitmap(ID_Record, wxBitmap(16, 16, 10));
        playToolBar_->Realize();
        playToolBar_->EnableTool(ID_Icon_Stop, false);
        playToolBar_->EnableTool(ID_Icon_Play, true);
        playToolBar_->EnableTool(ID_Record, false);
        playToolBar_->Realize();
        frameSizer->Add(playToolBar_, 0, wxEXPAND, 5);

        //Set the status bar
        statusBar_ = new wxStatusBar(this, wxID_ANY, wxFULL_REPAINT_ON_RESIZE, "statusBar");
        statusBar_->SetFieldsCount(3);
        int widths[] = {100, 200, -1};
        statusBar_->SetStatusWidths(3, widths);
        statusBar_->SetStatusText("Ready", 0);
        statusBar_->SetStatusText("Test Mode", 2);
        SetStatusBar(statusBar_);

        canvas_ = new Canvas(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, cameras_, &gps_, &session_, &db_, np_);

        frameSizer->Add(canvas_, 1, wxEXPAND);
        SetSizer(frameSizer);

        timer_.Start(500);//For displaying frame rates on the status bar

        helpController_.Initialize("RICS");
    }

    Frame::~Frame()
    {
    }
    
    //Calls onClose
    void Frame::onQuit(wxCommandEvent& WXUNUSED(event))
    {
        Close(TRUE);
    }

    //Called when the application is closed. It saves the notepad file,
    //deletes all threads, and stops the camera streams.
    void Frame::onClose(wxCloseEvent& WXUNUSED(event))
    {
        np_->saveFile();

        canvas_->deleteAllThreads();
        
        for (size_t i = 0; i < numCameras_; ++i)
        {
            camera(i).stopStream();
        }

        Destroy();
    }

    //Called if the cameras are unplugged before the application is closed.
    //Differences to onClose: (1) threads deleted after cameras closed; and
    //(2) all dialogs explicitly closed.
    void Frame::unpluggedClose()
    {
        np_->saveFile();

        for (size_t i = 0; i < numCameras_; ++i)
        {
            camera(i).stopStream();
        }

        canvas_->deleteAllThreads();

        //These dialogs are modal, therefore need to be closed before application is existed  by user.
        //However, in case cameras are unplugged, and any dialog is ShowModal, they need to be closed explicitly here. 
        if (sessionPropDialog_.IsModal())
        {
            sessionPropDialog_.Close();
        }
        if (cameraPropDialog_.IsModal())
        {
            cameraPropDialog_.Close();
        }
        if (gpsPropDialog_.IsModal())
        {
            gpsPropDialog_.Close();
        }
        if (notePadSetDialog_.IsModal())
        {
            notePadSetDialog_.Close();
        }

        Destroy();
    }

    //This is a wx timer event that executes the commands below
    //at set times (set by timer_)
    void Frame::onTimer(wxTimerEvent& WXUNUSED(event))
    {
        wxString frameRateText = "Frame rates: ";

        for (size_t i = 0; i < numCameras_; ++i)
        {
            wxString fr;
            fr.Printf("%.2f ", camera(i).actualFrameRate());
            frameRateText += fr;
        }
        
        statusBar_->SetStatusText(frameRateText, 1);

        if (play_ && session_.saveImages())
        {
            if (playToolBar_->GetToolEnabled(ID_Record))
            {
                playToolBar_->EnableTool(ID_Record, false);
            }
            else
            {
                playToolBar_->EnableTool(ID_Record, true);
            }
        }
        else
        {
            playToolBar_->EnableTool(ID_Record, false);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////Menu bar
    void Frame::onNewSession(wxCommandEvent& WXUNUSED(event))
    {
        sessionPropDialog_.ShowModal();

        np_->openFile();
 
        if (session_.sessionNameIsEmpty())
        {
            SetTitle("RICS (Test Mode)");
        }
        else
        {
            SetTitle(session_.sessionName() + " - RICS (Session Mode)");
            statusBar_->SetStatusText("Session Mode", 2);
        }
    }
    
    void Frame::onOpenSession(wxCommandEvent& WXUNUSED(event))
    {
        openSessionDialog_.open();

        np_->openFile();
 
        if (session_.sessionNameIsEmpty())
        {
            SetTitle("RICS (Test Mode)");
        }
        else
        {
            SetTitle(session_.sessionName() + " - RICS (Session Mode)");
            statusBar_->SetStatusText("Session Mode", 2);
        }
    }

    void Frame::onTest(wxCommandEvent& WXUNUSED(event))
    {
        if (!session_.sessionNameIsEmpty())
        {
            if (wxMessageBox("Revert to Test Mode?", "Test Mode", wxOK | wxCANCEL | wxICON_EXCLAMATION, this) == wxOK)
            { 
                session_.setSaveImages(false);
                session_.setCreateDB(false);
                session_.setSessionName("");
                session_.setPath("");
                np_->openFile();
                SetTitle("RICS (Test Mode)");
                statusBar_->SetStatusText("Test Mode", 2);
            }
            else
            {
                return;
            }
        }
    }

    void Frame::onTextPlay(wxCommandEvent& WXUNUSED(event))
    {
        doPlay();
    }
    
    void Frame::onIconPlay(wxCommandEvent& WXUNUSED(event))
    {
        doPlay();
    }

    void Frame::doPlay()
    {
        if (canvas_->play())
        {       
            menuPlay_->Enable(false);
            menuStop_->Enable(true);

            playToolBar_->EnableTool(ID_Icon_Play, false);
            playToolBar_->EnableTool(ID_Icon_Stop, true);

            play_ = true;
        }
    }

    void Frame::onTextStop(wxCommandEvent& WXUNUSED(event))
    {
        doStop();
    }

    void Frame::onIconStop(wxCommandEvent& WXUNUSED(event))
    {
        doStop();
    }

    void Frame::doStop()
    {
        if (canvas_->stop())
        {
            menuPlay_->Enable(true);
            menuStop_->Enable(false);

            playToolBar_->EnableTool(ID_Icon_Play, true);
            playToolBar_->EnableTool(ID_Icon_Stop, false);

            play_ = false;
        }
    }

    void Frame::onCameraProperties(wxCommandEvent& WXUNUSED(event))
    {
        cameraPropDialog_.setPlay(play());
        cameraPropDialog_.ShowModal();
        canvas_->setCameraNames();
    }

    void Frame::onGPSProperties(wxCommandEvent& WXUNUSED(event))
    {
        gpsPropDialog_.ShowModal();
    }

    void Frame::onNotePadSet(wxCommandEvent& WXUNUSED(event))
    {   
        notePadSetDialog_.ShowModal();
    }

    //Will open the pdf user manual 
    void Frame::onHelp(wxCommandEvent& WXUNUSED(event))
    {
        //helpController_.DisplayContents();
        wxFileType *fileType = wxTheMimeTypesManager->GetFileTypeFromExtension(wxT("pdf"));
        std::string user_manual_name = "rics_user_manual_" + 
                                       boost::lexical_cast<std::string>(RICS_VERSION) +
                                       ".pdf";
        wxString command = fileType->GetOpenCommand(user_manual_name);
        wxExecute(command);
        delete fileType;
    }

    //Open "About dialog. Will list licence terms.
    void Frame::onAbout(wxCommandEvent& WXUNUSED(event))
    {
        wxString msg = "RICS version " + boost::lexical_cast<std::string>(RICS_VERSION) +
                       "\n\nDeveloped by:" + 
                       "\nNariman Habili" +
                       "\n\nLicence:" +
                       "\n" + (wxChar)(169) + " 2011 Commonwealth of Australia (Geoscience Australia)" +
                       "\nGNU GPLv3, see http://www.gnu.org/licenses/";             
           
        wxMessageBox(msg, _T("About RICS"), wxOK | wxICON_INFORMATION, this);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////Tool bar icons
    void Frame::onNewSessionIcon(wxCommandEvent& event)
    {
        onNewSession(event);
    }

    void Frame::onOpenSessionIcon(wxCommandEvent& event)
    {
        onOpenSession(event);
    }

    void Frame::onTestIcon(wxCommandEvent& event)
    {
        onTest(event);
    }

    void Frame::onCameraPropertiesIcon(wxCommandEvent& event)
    {
        onCameraProperties(event);
    }

    void Frame::onGPSPropertiesIcon(wxCommandEvent& event)
    {
        onGPSProperties(event);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////Accessor method for attributes
    inline Camera& Frame::camera(size_t i) const
    {
        return (*cameras_)[i];
    }

    bool Frame::play() const
    {
        return play_;
    }

    BEGIN_EVENT_TABLE(Frame, wxFrame)
        EVT_MENU(ID_NewSession, Frame::onNewSession)
        EVT_MENU(ID_OpenSession, Frame::onOpenSession)
        EVT_MENU(ID_Test, Frame::onTest)
        EVT_MENU(ID_Quit, Frame::onQuit)
        EVT_MENU(ID_Help, Frame::onHelp)
        EVT_MENU(ID_About, Frame::onAbout)
        EVT_MENU(ID_Text_Play, Frame::onTextPlay)
        EVT_MENU(ID_Text_Stop, Frame::onTextStop)
        EVT_MENU(ID_Icon_Play, Frame::onIconPlay)
        EVT_MENU(ID_Icon_Stop, Frame::onIconStop)
        EVT_MENU(ID_CameraProperties, Frame::onCameraProperties)
        EVT_MENU(ID_GPSProperties, Frame::onGPSProperties)
        EVT_MENU(ID_NotePadSettings, Frame::onNotePadSet)

        EVT_CLOSE(Frame::onClose)

        EVT_TOOL(ID_NewSession_Icon, Frame::onNewSessionIcon)
        EVT_TOOL(ID_OpenSession_Icon, Frame::onOpenSessionIcon)
        EVT_TOOL(ID_Test_Icon, Frame::onTestIcon)
        EVT_TOOL(ID_CameraProperties_Icon, Frame::onCameraPropertiesIcon)
        EVT_TOOL(ID_GPSProperties_Icon, Frame::onGPSPropertiesIcon)

        EVT_TIMER(ID_Timer, Frame::onTimer)
    END_EVENT_TABLE()

} //namespace