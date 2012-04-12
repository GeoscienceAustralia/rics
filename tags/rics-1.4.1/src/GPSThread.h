/*
Author: Nariman Habili

Description: The GPS thread is initialised here and run.

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

#ifndef GPS_THREAD_H
#define GPS_THREAD_H

#include "SharedGPSData.h"
#include "GPS.h"
#include <wx/wx.h>
#include <wx/thread.h>
#include <windows.h>

namespace rics
{
    class GPSThread : public wxThread
    {
    public:
        GPSThread();
        GPSThread(wxWindow* canvas, GPS* gps, SharedGPSDataPtr buffer);
        ~GPSThread();

        void* Entry();

        void OnExit();

    public:
        enum
        {
            GPS_EVENT = 1 //must be a different number to CAMERA_EVENT
        };
    
    private:
        wxWindow* canvas_;
        GPS* gps_;
        SharedGPSDataPtr buffer_;
    };
} //namespace

#endif //GPS_THREAD_H