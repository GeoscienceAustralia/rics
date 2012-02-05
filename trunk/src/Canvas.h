/*
Author: Nariman Habili

Description: Creates the image, gps and notepad panels. Controls
             camera play and stop.

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

#ifndef CANVAS_H
#define CANVAS_H

#include "ImagePanel.h"
#include "Camera.h"
#include "CameraThread.h"
#include "GPSThread.h"
#include "SharedImageBuffer.h"
#include "SharedGPSData.h"
#include "Session.h"
#include "Database.h"
#include "NotePad.h"
#include <wx/wx.h>
#include <wx/file.h>
#include <wx/thread.h>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

namespace rics
{
    class CameraThread;

    class Canvas: public wxPanel
    {
    public:
        Canvas(wxWindow* parent, 
               wxWindowID id, 
               const wxPoint &pos, 
               const wxSize &size, 
               Cameras* cameras, 
               GPS* gps, 
               Session* session,
               Database* db,
               NotePad* np);
        ~Canvas();

        void deleteAllThreads();
        bool play();
        bool stop();

        void setCameraNames();
        
    public:
        enum
        {
            ID_Timer = 1000,
            ID_NotePad,
        };

    private:
        void onCameraEvent(wxCommandEvent& event);
        void onGPSEvent(wxCommandEvent& WXUNUSED(event));

        inline void deleteCameraThreads();
        inline void deleteGPSThread();
    
    private:        
        Cameras* cameras_;
        size_t numCameras_;

        GPS* gps_;
        Session* session_;

        boost::shared_array<SharedImageBufferPtr> cameraBuffers_;
        boost::shared_array<CameraThread*> cameraThreads_;

        SharedGPSDataPtr gpsData_;
        boost::shared_ptr<GPSThread*> gpsThread_;
        
        bool play_;
        
        bool createDB_;
        bool firstTime_;

        wxToolBar* playToolBar_;
        
        ImagePanel* panel1_;
        ImagePanel* panel2_;
        ImagePanel* panel3_;
        ImagePanel* panel4_;

        wxStaticText* latitudeValue_;
        wxStaticText* longitudeValue_;
        wxStaticText* bearValue_;
        wxStaticText* timeStampValue_;
        wxStaticText* speedValue_;
        wxStaticText* satValue_;
        wxStaticText* altValue_;
        wxStaticText* qualityValue_;

        wxString latSexagesimal_;
        wxString lonSexagesimal_;
        wxString lat_;
        wxString lon_;
        wxString bear_;
        wxString timeStamp_;
        wxString formatTime_;
        wxString speed_;
        wxString satellites_;
        wxString altitude_;
        wxString quality_;

        std::vector<ImagePanel*> panels_;

        wxTextCtrl* textCtrl_;

        Database* db_;

        NotePad* np_;

        DECLARE_EVENT_TABLE()

    };
}

#endif //CANVAS_H