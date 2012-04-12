/*
Author: Nariman Habili

Description: The camera thread is initialised here and run.

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
#ifndef CAMERA_THREAD_H
#define CAMERA_THREAD_H

#define USE_JPEG_TURBO 1

#include "SharedImageBuffer.h"
#include "Camera.h"
#include "Session.h"
#include <wx/wx.h>
#include <wx/thread.h>

namespace rics
{
    class CameraThread : public wxThread
    {
    public:
        CameraThread();
        CameraThread(wxWindow* canvas, 
                     Camera* camera, 
                     SharedImageBufferPtr buffer, 
                     int panel,
                     Session* const session);
        ~CameraThread();

        void* Entry();

        void OnExit();

    public:
        enum
        {
            CAMERA_EVENT = 0
        };
    
    private:
        wxWindow* canvas_;
        Camera* camera_;
        SharedImageBufferPtr buffer_;
        int panel_;
        Session* session_;

    };
} //namespace

#endif //CAMERA_THREAD_H