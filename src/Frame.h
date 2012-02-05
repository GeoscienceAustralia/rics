/*
Author: Nariman Habili

Description: The main frame. Sets the menu and status bar. Help file in pdf
             format is launched here. The Canvas class is instantiated here.

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

#ifndef FRAME_H
#define FRAME_H

#include "Canvas.h"
#include "CameraPropDialog.h"
#include "GPS.h"
#include "GPSPropDialog.h"
#include "SessionPropDialog.h"
#include "OpenSessionDialog.h"
#include "NotePadSetDialog.h"
#include "Camera.h"
#include "Session.h"
#include "Database.h"
#include <wx/wx.h>
#include <wx/help.h>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace rics
{
  
    class Frame: public wxFrame
    {
    public:
        Frame(const wxString& title, const wxPoint& pos, const wxSize& size, Cameras* cameras);
        ~Frame();

        void onNewSession(wxCommandEvent& WXUNUSED(event));
        void onOpenSession(wxCommandEvent& WXUNUSED(event));
        void onTest(wxCommandEvent& WXUNUSED(event));

        void onClose(wxCloseEvent& WXUNUSED(event));
        void unpluggedClose();

        void doPlay();
        void doStop();

        bool play() const;

    private:
        void onQuit(wxCommandEvent& event);
        void onHelp(wxCommandEvent& WXUNUSED(event));
        void onAbout(wxCommandEvent& event);
        void onCameraProperties(wxCommandEvent& event);
        void onGPSProperties(wxCommandEvent& event);
        void onNotePadSet(wxCommandEvent& WXUNUSED(event));

        void onNewSessionIcon(wxCommandEvent& WXUNUSED(event));
        void onOpenSessionIcon(wxCommandEvent& WXUNUSED(event));
        void onTestIcon(wxCommandEvent& WXUNUSED(event));
        void onCameraPropertiesIcon(wxCommandEvent& WXUNUSED(event));
        void onGPSPropertiesIcon(wxCommandEvent& WXUNUSED(event));

        void onTimer(wxTimerEvent& event);

        void onTextPlay(wxCommandEvent& WXUNUSED(event));
        void onTextStop(wxCommandEvent& WXUNUSED(event));
        void onIconPlay(wxCommandEvent& WXUNUSED(event));
        void onIconStop(wxCommandEvent& WXUNUSED(event));

        inline Camera& camera(size_t i) const;

    private:
        Cameras* cameras_;
        size_t numCameras_;

        Canvas* canvas_;

        GPS gps_;
        Session session_;
        Database db_;

        NotePad* np_;

        CameraPropDialog cameraPropDialog_;
        GPSPropDialog gpsPropDialog_;
        SessionPropDialog sessionPropDialog_;
        OpenSessionDialog openSessionDialog_;
        NotePadSetDialog notePadSetDialog_;
        wxToolBar* playToolBar_; 
        wxStatusBar* statusBar_;

        wxTimer timer_;

        bool play_;

        wxMenuItem* menuPlay_;
        wxMenuItem* menuStop_;

        wxHelpController helpController_;

        DECLARE_EVENT_TABLE()

        enum
        {
            ID_NewSession = 1,
            ID_OpenSession,
            ID_Test,
            ID_Quit,
            ID_Help,
            ID_About,
            ID_CameraProperties,
            ID_GPSProperties,
            ID_NotePadSettings,
            ID_Text_Play,
            ID_Text_Stop,
            ID_Icon_Play,
            ID_Icon_Stop,
            ID_Record,

            ID_NewSession_Icon,
            ID_OpenSession_Icon,
            ID_Test_Icon,
            ID_CameraProperties_Icon,
            ID_GPSProperties_Icon,

            ID_Timer
        };

    };

} //namespace

#endif //MYFRAME_H