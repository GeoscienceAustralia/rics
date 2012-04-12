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

#include "CameraThread.h"

namespace rics
{
    CameraThread::CameraThread()
    {
    }
    
    CameraThread::CameraThread(wxWindow* canvas, 
                               Camera* camera, 
                               SharedImageBufferPtr buffer, 
                               int panel,
                               Session* const session):
    canvas_(canvas),
    camera_(camera),
    buffer_(buffer),
    panel_(panel),
    session_(session)
    {
    }

    CameraThread::~CameraThread()
    {
    }

    void* CameraThread::Entry()
    {
        while (!TestDestroy())
        {
            buffer_->writeLock();
            buffer_->setData(camera_->getNextFrame()); 
            wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, CAMERA_EVENT);
            event.SetInt(panel_);
            event.SetExtraLong(camera_->frameNumber());
            wxPostEvent(canvas_, event);
            buffer_->writeUnlock();

            if (session_->saveImages())
            {
#if USE_JPEG_TURBO
                camera_->saveImageTurbo();
#else
                camera_->saveImageWX();
#endif
                camera_->incFrameNumber();
            }
        }

        return NULL;
    }

    void CameraThread::OnExit()
    {
    }

}//namespace