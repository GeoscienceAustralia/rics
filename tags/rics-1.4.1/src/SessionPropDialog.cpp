/*
Author: Nariman Habili

Description: Dialog to create a new session.

Copyright (c) 2011-2012 Commonwealth of Australia (Geoscience Australia)

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

#include "SessionPropDialog.h"
#include "Frame.h"
#include "sqlite3.h"
#include "nav_car.xpm"

namespace rics 
{
    SessionPropDialog::SessionPropDialog(Frame* frame, 
                                         const wxString &title, 
                                         Session* session, 
                                         Cameras* cameras,
                                         Database* db,
                                         GPS* gps):
    wxDialog(frame, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    frame_(frame),
    session_(session),
    cameras_(cameras),
    db_(db),
    gps_(gps),
    numCameras_((*cameras_).size()),
    directory_(this, ID_Browse, "C:\\RICS Sessions", "Session Location", wxDefaultPosition, wxDefaultSize, 
               wxDIRP_USE_TEXTCTRL | wxDD_NEW_DIR_BUTTON),
    dir_("C:\\RICS Sessions")
    {   
        wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

        createSessionName();
        topSizer->Add(sessionNameSizer_, 0, wxALL, 10);

        //Button
        wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
        okButton_ = new wxButton(this, ID_OK, _T("OK"));
        okButton_->Enable(true);
        okButton_->SetDefault();
        //wxButton *cancelButton = new wxButton(this, wxID_CANCEL,  _T("Cancel"));
        buttonSizer->Add(okButton_, 0, wxALIGN_LEFT | wxALL, 10);
        //buttonSizer->Add(cancelButton, 0, wxALIGN_LEFT | wxALL, 10 );
        topSizer->Add(buttonSizer, 0, wxALIGN_RIGHT);

        SetSizer(topSizer);
        topSizer->Fit(this);
        topSizer->SetSizeHints(this);
    }

    SessionPropDialog::~SessionPropDialog()
    {
    }

    
    void SessionPropDialog::createSessionName()
    {
        //Session name
        wxStaticBox* sessionName = new wxStaticBox(this, wxID_STATIC, "Session Name");
        sessionNameSizer_ = new wxStaticBoxSizer(sessionName, wxVERTICAL);
        sessionNameSizer_->SetMinSize(330, 0);
        
        wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
        
        wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* textSN = new wxStaticText(this, wxID_ANY, "Session Name:", wxDefaultPosition, wxDefaultSize, 0);
        textCtrlSN_ = new wxTextCtrl(this,
                                     ID_SN,
                                     "",
                                     wxDefaultPosition,
                                     wxSize(100, -1),
                                     wxTE_PROCESS_ENTER);
        checkBoxSN_ = new wxCheckBox(this, wxID_ANY, wxT("Add GPS date/time?"), wxDefaultPosition, wxDefaultSize, 0);
        checkBoxSN_->SetValue(true);
        topSizer->Add(textSN, 0, wxALL | wxALIGN_LEFT, 10);
        topSizer->Add(textCtrlSN_, 0, wxALL | wxALIGN_LEFT, 10);
        topSizer->Add(checkBoxSN_, 0, wxALL | wxALIGN_LEFT, 10);

        wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* textLoc = new wxStaticText(this, wxID_ANY, "Session Location:", wxDefaultPosition, wxDefaultSize, 0);
        bottomSizer->Add(textLoc, 0, wxALL | wxALIGN_LEFT, 5);
        bottomSizer->Add(&directory_, 0, wxALL | wxALIGN_LEFT, 5);

        boxSizer->Add(topSizer, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 5);
        boxSizer->Add(bottomSizer, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 5);
 
        sessionNameSizer_->Add(boxSizer, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 5);
    }

    //////OK/Close Buttons
    void SessionPropDialog::onOK(wxCommandEvent& WXUNUSED(event))
    {
        //make sure camera names are not duplicates.
        if (!checkDuplicateCameraNames())
        {
            wxMessageDialog(this, "Duplicate camera names chosen. Please assign a unique camera name to each camera",
            "Duplicate Camera Name Error", wxICON_HAND)
            .ShowModal();
            return;
        }

        session_->setSaveImages(false);//initially have save image and create db off.
        session_->setCreateDB(false);
        session_->initCurrentFrame();//Set current frame number to 0. Only changed if database already exists.

        bool sessionExists = false;
        
        //make sure session name is not empty
        if (sessionName_ != "")
        {
            //make sure the first character of session name is not a space
            if (sessionName_[0] == ' ')
            {
                wxMessageDialog(this, "Invalid session name. Please enter a valid session name.", "Session Name Error", wxICON_HAND)
                .ShowModal();
                return;
            }

            //make sure GPS is active before creating a session
            if (!gps_->gpsActive())
            {
                wxMessageDialog(this, "No GPS data detected. Check COM port setting",
                "RICS GPS Error", wxICON_HAND)
                .ShowModal();
                return;
            }

            //If directory doesn't exist, then create
            if (!wxDirExists(dir_))
            {
                wxMkdir(dir_);
            }

            onTxtSN(wxCommandEvent());

            //If system date and time are required.
            //wxDateTime dateTime = wxDateTime::Now();
            //int year = dateTime.GetYear();
            //int month = dateTime.GetMonth();
            //int day = dateTime.GetDay();
            //int hour = dateTime.GetHour();
            //int minute = dateTime.GetMinute();
            //int second = dateTime.GetSecond();

            if (checkBoxSN_->IsChecked())
            {
            
                sessionName_ = sessionName_ + 
                               "_" + 
                               gps_->date() + 
                               "_" + 
                               gps_->timeStamp();
            }

            wxString sessionDir = dir_ + "\\" + sessionName_;
            if (!wxDirExists(sessionDir))
            {
                wxMkdir(sessionDir);
            }
            else
            {
                wxMessageDialog(this, "Session already exists. Session will be opened.", "Session Name Message", wxICON_EXCLAMATION)
                .ShowModal();
                sessionExists = true;
            }

            //A database is created, or if one already exists, the max frame number is retrieved.
            wxString filename = dir_ + 
                                "\\" + 
                                sessionName_ + 
                                "\\" + 
                                sessionName_ + 
                                ".sdb";

            if (wxFileExists(filename))//if database already exists
            {
                db_->openDatabase(filename);

                for (size_t i = 0; i < numCameras_; ++i)
                {
                    wxString cameraName = camera(i).cameraName();
                    db_->createTable(cameraName);//In case extra camera(s) are attached, new tables need to be created. 
                    long int currentFrame = db_->maxFrame(cameraName);              
                    if (!currentFrame)
                    {
                        session_->setCurrentFrame(i, currentFrame);//Table is empty
                    }
                    else
                    {
                        session_->setCurrentFrame(i, currentFrame + 1);//1 + max frame number in database
                    }
                }
            }
            else
            {
                if (sessionExists)//if session exists, check for <session>.sdb file. If not found, return.
                {
                    wxMessageDialog(frame_, "Database \"" + sessionName_ + ".sdb\" does not exist. Cannot open session.", "Session Database Error", wxICON_HAND)
                    .ShowModal();
                    EndModal(wxID_CANCEL);
                    return;
                }
                else
                {
                    db_->databaseClose();//Close previous database if it exists.
                    db_->openDatabase(filename);

                    for (size_t i = 0; i < numCameras_; ++i)
                    {   
                        wxString cameraName = boost::lexical_cast<std::string>(camera(i).cameraName()); 
                        db_->createTable(cameraName);
                    }
                }
            }

            //Stop cameras (and therefore delete threads) if playing to make sure no images are saved and no entries in
            //databse are written. MUST be called after database is created.
            bool play = frame_->play();
            if (play)
            {
                frame_->doStop();
            }

            //Create directories for saved images
            for (size_t i = 0; i < numCameras_; ++i)
            {
                wxString dir = dir_ + 
                               "\\" + 
                               sessionName_ + 
                               "\\" +
                               boost::lexical_cast<std::string>(camera(i).cameraName());

                if (!wxDirExists(dir))
                {
                    wxMkdir(dir);                   
                }

                camera(i).setSessionPath(dir);
            }

            session_->setSaveImages(true);
            session_->setCreateDB(true);
            session_->setSessionName(sessionName_);
            session_->setPath(dir_ + "\\" + sessionName_);

            for (size_t i = 0; i < numCameras_; ++i)
            {
                camera(i).setSessionName(session_->sessionName());
                camera(i).setFrameNumber(session_->currentFrame(i));
            }

            //Start cameras and threads again if cameras were originally in play mode.
            if (play)
            {
                frame_->doPlay();
            }
        }

        EndModal(wxID_CANCEL);
    }

    void SessionPropDialog::onClose(wxCloseEvent& WXUNUSED(event))
    {     
        //onOK(wxCommandEvent());
        EndModal(GetReturnCode());
    } 
    
    void SessionPropDialog::onTxtSN(wxCommandEvent& WXUNUSED(event))
    {
        okButton_->Enable(true);
        sessionName_ = textCtrlSN_->GetValue();

        if (sessionName_ == "")
        {
            okButton_->Enable(false);
        }
    }
    
    void SessionPropDialog::onBrowse(wxFileDirPickerEvent& WXUNUSED(event))
    {
        dir_ = directory_.GetPath();
    }

    //check for duplicate camera names
    bool SessionPropDialog::checkDuplicateCameraNames()
    {
        for (size_t j = 0; j < numCameras_ - 1; ++j)
        {
            for (size_t i = j + 1; i < numCameras_; ++i)
            {
                if (camera(j).cameraName() == camera(i).cameraName())
                {
                    return false;
                }
            }
        }

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////Accessor method for attributes
    inline Camera& SessionPropDialog::camera(size_t i) const
    {
        return (*cameras_)[i];
    }
    
   BEGIN_EVENT_TABLE(SessionPropDialog, wxDialog)
        EVT_BUTTON(ID_OK, SessionPropDialog::onOK)
        EVT_CLOSE(SessionPropDialog::onClose)
        EVT_DIRPICKER_CHANGED(ID_Browse, SessionPropDialog::onBrowse)
        EVT_TEXT(ID_SN, SessionPropDialog::onTxtSN)
   END_EVENT_TABLE()

} //namespace rics