/*
Author: Nariman Habili

Description: Dialog to open an existing session.

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

#include "OpenSessionDialog.h"
#include "Frame.h"

namespace rics 
{    

    OpenSessionDialog::OpenSessionDialog(Frame* frame, Session* session, Cameras* cameras, Database *db):
    frame_(frame),
    session_(session),
    cameras_(cameras),
    db_(db),
    numCameras_((*cameras_).size())
    {   
    }

    OpenSessionDialog::~OpenSessionDialog()
    {
    }

    void OpenSessionDialog::open()
    {
        wxDirDialog dialog(frame_, "Choose a Directory", "C:\\RICS Sessions", wxDD_DIR_MUST_EXIST);
        if (dialog.ShowModal() == wxID_OK)
        {
            //make sure camera names are not duplicates.
            if (!checkDuplicateCameraNames())
            {
                wxMessageDialog(frame_, "Duplicate camera names chosen. Please assign a unique camera name to each camera",
                    "Duplicate Camera Name Error", wxICON_HAND)
                    .ShowModal();
                return;
            }

            wxString sessionDir = dialog.GetPath();//session path
            wxString sessionName = sessionDir.Mid(sessionDir.Find('\\', true) + 1);//Find session name

            session_->setSaveImages(false);//initially have save image and create db off.
            session_->setCreateDB(false);

            //A database is created, or if one already exists, the max frame number is retrieved.
	        wxString filename = sessionDir + "\\" + sessionName + ".sdb";

            if (wxFileExists(filename))//if database  exists
            {
                db_->databaseClose();//Close previous database if it exists.
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
            else //if not, error
            {
			    wxMessageDialog(frame_, "Database \"" + sessionName + ".sdb\" does not exist. Cannot open session.", "Session Database Error", wxICON_HAND)
                .ShowModal();
			    return;            
            }
            
            //Stop cameras (and therefore delete threads) if playing to make sure no images are saved and no entries in
            //databse are written. MUST be called after database is created.
            bool play = frame_->play();
            if (play)
            {
                frame_->doStop();
            }

            //Create directories for saved image
            for (size_t i = 0; i < numCameras_; ++i)
            {
                wxString dir = sessionDir + 
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
            session_->setSessionName(sessionName);
            session_->setPath(sessionDir);

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
    }

    //check for duplicate camera names
    bool OpenSessionDialog::checkDuplicateCameraNames()
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
    inline Camera& OpenSessionDialog::camera(size_t i) const
    {
        return (*cameras_)[i];
    }

}