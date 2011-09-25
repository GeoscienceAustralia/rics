/*
Author: Nariman Habili

Description: Dialog to create a new session.

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

#ifndef SessionPropDialog_H
#define SessionPropDialog_H

#include "Camera.h"
#include "Session.h"
#include "Database.h"
#include "GPS.h"
#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/filepicker.h>
#include <boost/lexical_cast.hpp>

namespace rics
{
    class Frame;

    class SessionPropDialog : public wxDialog
    {
    public:
        SessionPropDialog(Frame *frame, 
                          const wxString &title, 
                          Session* session, 
                          Cameras* cameras, 
                          Database* db,
                          GPS* gps);
        ~SessionPropDialog();

    private:
        void createSessionName();

        void onOK(wxCommandEvent& WXUNUSED(event));
        void onClose(wxCloseEvent& WXUNUSED(event));
        void onTxtSN(wxCommandEvent& WXUNUSED(event));
        void onBrowse(wxFileDirPickerEvent& WXUNUSED(event));

        bool checkDuplicateCameraNames();

        inline Camera& camera(size_t i) const;
    
    private:
        Frame* frame_;
        Session* session_;
        Cameras* cameras_;
        Database* db_;
        GPS* gps_;
        size_t numCameras_;

        wxStaticBoxSizer* sessionNameSizer_;

        wxButton* okButton_;

        wxTextCtrl* textCtrlSN_;
        wxCheckBox* checkBoxSN_;
        wxTextCtrl* textCtrlLoc_;

        wxDirPickerCtrl directory_;

        wxString sessionName_;
        wxString dir_;
               
        enum
        {
            ID_OK = 1,
            ID_Browse,
            ID_SN,
            ID_Camera_IDs
        };

        DECLARE_EVENT_TABLE()
    
    };
} //namespace rics

#endif //SessionPropDialog_H