/*
Author: Nariman Habili

Description: The camera properties are controlled here. Images are also 
             saved here.

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

#ifndef CAMERA_H
#define CAMERA_H

#include "vld.h"
#include "JPEGWriter.h"

#include <windows.h>
#include <Winsock2.h>

#include <cassert>
#include <PvApi.h>
#include <PvRegIo.h>
#include <boost/shared_array.hpp>
#include <wx/wx.h>
#include <vector>

class wxString;

namespace rics
{
    typedef boost::shared_array<unsigned char> UCArray;

    class Camera
    {
    public:
        Camera(tPvHandle& hCamera, unsigned long streamBytesPerSecond);
        ~Camera();

        void startStream();
        void stopStream();

        UCArray getNextFrame();

        void saveImageWX();
        void saveImageTurbo();

        unsigned long height() const;
        void setHeight(unsigned long h);
        unsigned long width() const;
        void setWidth(unsigned long w);
        long frameNumber() const;
        void setFrameNumber(unsigned long fn);
        void incFrameNumber();
        void resetFrameNumber();

        void setROI(unsigned long left, unsigned long top, unsigned long height, unsigned long width);
        void setExposureTime(bool autoMode, unsigned long exposureTime);
        void setAutoMaxTime(unsigned long exposureMaxTime);
        float maxFrameRate();
        void setFrameRate(float frameRate);
        void setWhiteBalance(bool autoMode, char* colour, unsigned long value);
        void setGain(bool autoMode, unsigned long gain);
        void adjustPacketSize(unsigned long packetSize);
        wxString sessionName();
        void setSessionName(const wxString& sn);
        inline wxString sessionPath() const;
        void setSessionPath(const wxString& sessionPath);

        unsigned long exposureTime();
        int uniqueID();
        void setCameraName(const wxString& cameraName);
        wxString cameraName();
        float actualFrameRate();

        inline std::string frameName();

    private:
        HANDLE handle();

    private:
        HANDLE hCamera_;

        unsigned long height_;
        unsigned long width_;
        unsigned long resizeFactor_;
        unsigned long heightResized_;
        unsigned long widthResized_;
        unsigned long stepBytesResized_;
        unsigned long stepBytesOriginal_;

        long frameNumber_;
        wxString sessionPath_;

        UCArray frameBuffer_;
        UCArray resized_;
        UCArray imageBuffer_;
        tPvFrame image_;

        wxString sessionName_;
    };

    typedef std::vector<Camera> Cameras;

} //namespace

#endif //CAMERA_H
