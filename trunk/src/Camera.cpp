/*
Author: Nariman Habili

Description: The camera properties are controlled here. Images are also 
             saved here.

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

#include "Camera.h"
#include <boost/lexical_cast.hpp>

namespace rics
{
    Camera::Camera(tPvHandle& hCamera, unsigned long streamBytesPerSecond):
    hCamera_(hCamera),
    height_(2048),
    width_(2448),
    resizeFactor_(12),
    heightResized_(2048/resizeFactor_),
    widthResized_(2448/resizeFactor_),
    stepBytesResized_(widthResized_*3*sizeof(unsigned char)),
    stepBytesOriginal_(width_*3*sizeof(unsigned char)),
    frameNumber_(0),
    sessionPath_(""),
    frameBuffer_(UCArray(new unsigned char[height_*width_*3])),
    resized_(UCArray(new unsigned char[heightResized_*widthResized_*3]))//memory for resized image
    {
        //Set packet size. Maximum is 9014.
        PvAttrUint32Set(handle(), "PacketSize", 6000/*8228*/);

        //Calculating the "StreamBytesPerSecond". For a gigabit ethernet card, maximum stream
        //bytes per second is set to (maximum) 124000000. To find the "StreamBytesPerSecond" value, 
        //124000000 is divided by the number of cameras attached.
        tPvErr returnCode = PvAttrUint32Set(handle(), "StreamBytesPerSecond", streamBytesPerSecond);
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);

        returnCode = PvCaptureStart(handle());
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        returnCode = PvAttrEnumSet(handle(), "AcquisitionMode", "Continuous");
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        returnCode = PvAttrEnumSet(handle(), "PixelFormat", "Bayer8");
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        returnCode = PvAttrEnumSet(handle(), "FrameStartTriggerMode", /*"Freerun"*/"FixedRate");
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        
        //Set default camera parameters.
        setROI(0, 0, height(), width());
        setAutoMaxTime(5000);
        setExposureTime(true, 0);
        setFrameRate(4.0f);
        setWhiteBalance(false, "R", 181);
        setWhiteBalance(false, "B", 202);
        setGain(false, 0);

        unsigned long totalBytesPerFrame;
        returnCode = PvAttrUint32Get(handle(), "TotalBytesPerFrame", &totalBytesPerFrame);
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        image_.ImageBufferSize = totalBytesPerFrame;
        imageBuffer_ = UCArray(new unsigned char[totalBytesPerFrame]);
        image_.ImageBuffer = imageBuffer_.get();
    }

    Camera::~Camera()
    {
    }

    //get handle
    inline HANDLE Camera::handle()
    {
        return hCamera_;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////Set and get image properties
    inline unsigned long Camera::height() const
    {
        return height_;
    }

    inline void Camera::setHeight(unsigned long h)
    {
        height_ = h;
    }

    inline unsigned long Camera::width() const
    {
        return width_;
    }

    inline void Camera::setWidth(unsigned long w)
    {
        width_ = w;
    }

    inline long Camera::frameNumber() const
    {
        return frameNumber_;
    }

    void Camera::setFrameNumber(unsigned long fn)
    {
        frameNumber_ = fn;
    }

    //Increment the frame number
    void Camera::incFrameNumber()
    {
        ++frameNumber_;
    }

    //Reset the frame number to 0 
    void Camera::resetFrameNumber()
    {
        frameNumber_ = 0;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////Image streaming and saving
    void Camera::startStream()
    {
        unsigned long started;
        tPvErr returnCode = PvCaptureQuery(handle(), &started);
        if (!started)
        {
            returnCode = PvCaptureStart(handle());
            assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        }

        //start stream
        returnCode = PvCommandRun(handle(), "AcquisitionStart");
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
    }

    //Stop video stream.
    void Camera::stopStream()
    {
        unsigned long started;
        tPvErr returnCode = PvCaptureQuery(handle(), &started);
        if (started)
        {
            returnCode = PvCommandRun(handle(), "AcquisitionStop");
            assert(returnCode == 0 || returnCode == ePvErrUnplugged);
            returnCode = PvCaptureQueueClear(handle());
            assert(returnCode == 0 || returnCode == ePvErrUnplugged);         
            returnCode = PvCaptureEnd(handle());
            assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        }
    }

    //Grab next frame. Note: Pointer belongs to Camera.
    UCArray Camera::getNextFrame()
    {
        tPvErr returnCode = PvCaptureQueueFrame(handle(), &image_, NULL);
        if (returnCode)
        {
            return resized_;
        }
        
        returnCode = PvCaptureWaitForFrameDone(handle(), &image_, PVINFINITE);
        if (returnCode)
        {
            return resized_;
        }

        unsigned char *original = frameBuffer_.get();
        PvUtilityColorInterpolate(&image_, &(original[0]), &(original[1]), &(original[2]), 2, 0);

        //This section resizes the image. Saves wxWidgets doing it. 
        //Byte order of images in frameBuffer are in bmp format (GBR, left to right, bottom to top).
        //wxWidgets expects RGB (left to right, top to bottom)
        //byte order. The loop below reorders the byte order (and resizes) for wxWidgets.
        for (unsigned long j = 0; j < heightResized_; ++j)
        {     
            unsigned char* res = resized_.get() + j*stepBytesResized_;
            unsigned char* orig = original + resizeFactor_*j*stepBytesOriginal_;//resized resizeFactor_ times

            for (unsigned long i = 0; i < widthResized_; ++i)
            {    
                *res = *orig;
                *(res + 1) = *(orig + 1);
                *(res + 2) = *(orig + 2);

                res += 3;
                orig += 3*resizeFactor_;//resizeFactor_*3, resized resizeFactor_ times
           }
        }
        
        return resized_;
    }

    //save image as jpeg using the libjpeg library in wxWidgets
    void Camera::saveImageWX()
    {
        wxImage(width(), height(), frameBuffer_.get(), true)
        .SaveFile(frameName(), wxBITMAP_TYPE_JPEG);
    }

    //save image as jpeg using the libjpeg-turbo library
    void Camera::saveImageTurbo()
    {
        JPEGWriter writer;
        writer.header(width(), height(), 3, JPEG::COLOR_RGB);
        writer.setQuality(80);
        writer.write(frameName(), frameBuffer_.get());
    }

    inline wxString Camera::sessionName()
    {
        return sessionName_;
    }

    void Camera::setSessionName(const wxString& sn)
    {
        sessionName_ = sn;
    }

    inline wxString Camera::sessionPath() const
    {
        return sessionPath_;
    }

    void Camera::setSessionPath(const wxString& sessionPath)
    { 
        sessionPath_ = sessionPath;
    }
    
    std::string Camera::frameName()
    {
        std::string frameName;
        
        frameName = sessionPath();

        if (frameNumber() < 10)
        {
            frameName += "\\000000" + 
                         boost::lexical_cast<std::string>(frameNumber()) + 
                         ".jpg";
        }
        else if (frameNumber() < 100)
        {
            frameName += "\\00000" + 
                        boost::lexical_cast<std::string>(frameNumber()) + 
                        ".jpg";
        }
        else if (frameNumber() < 1000)
        {
            frameName += "\\0000" + 
                        boost::lexical_cast<std::string>(frameNumber()) + 
                        ".jpg";
        }
        else if (frameNumber() < 10000)
        {
            frameName += "\\000" + 
                        boost::lexical_cast<std::string>(frameNumber()) + 
                        ".jpg";
        }
        else if (frameNumber() < 100000)
        {
            frameName += "\\00" + 
                        boost::lexical_cast<std::string>(frameNumber()) + 
                        ".jpg";
        }
        else if (frameNumber() < 1000000)
        {
            frameName += "\\0" + 
                        boost::lexical_cast<std::string>(frameNumber()) + 
                        ".jpg";
        }
        else
        {
            frameName += "\\" + boost::lexical_cast<std::string>(frameNumber()) + 
                         ".jpg";
        }

        return frameName;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////set camera features
    //Setting the ROI (Region Of interest)
    void Camera::setROI(unsigned long left, unsigned long top, unsigned long height, unsigned long width)
    {
        setHeight(height);
        setWidth(width);

        tPvErr returnCode = PvAttrUint32Set(handle(), "Width", width);
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        returnCode = PvAttrUint32Set(handle(), "Height", height);
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        returnCode = PvAttrUint32Set(handle(), "RegionX", left);
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        returnCode = PvAttrUint32Set(handle(), "RegionY", top);
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
    }
    
    void Camera::setExposureTime(bool autoMode, unsigned long exposureTime)
    {
        tPvErr returnCode;

        if (autoMode)
        {
            returnCode = PvAttrEnumSet(handle(), "ExposureMode", "Auto");
            assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        }
        else
        {
            returnCode = PvAttrEnumSet(handle(), "ExposureMode", "Manual");
            assert(returnCode == 0 || returnCode == ePvErrUnplugged);
            returnCode = PvAttrUint32Set(handle(), "ExposureValue", exposureTime);            
            assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        }
    }

    unsigned long Camera::exposureTime()
    {   
        unsigned long exposureTime;
        tPvErr returnCode = PvAttrUint32Get(handle(), "ExposureValue", &exposureTime);
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        return exposureTime;
    }

    //Set the maximum exposure time when exposure mode is auto.
    //This is used to reduce image blur as the vehicle passes a dark scene.
    //The camera then needs to return to a lower exposure time and this takes a 
    //few frames hence the image blur.
    void Camera::setAutoMaxTime(unsigned long exposureMaxTime)
    {
        tPvErr returnCode;
        returnCode = PvAttrUint32Set(handle(), "ExposureAutoMax", exposureMaxTime);            
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
    }

    float Camera::maxFrameRate()
    {
        float min; 
        float max;
        tPvErr returnCode = PvAttrRangeFloat32(handle(), "FrameRate", &min, &max);
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        return max;
    }

    void Camera::setFrameRate(float frameRate)
    {
        tPvErr returnCode = PvAttrFloat32Set(handle(), "FrameRate", frameRate);
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
    }

    void Camera::setWhiteBalance(bool autoMode, char* colour, unsigned long value)
    {
        tPvErr returnCode;

        if (autoMode)
        {
            returnCode = PvAttrEnumSet(handle(), "WhitebalMode", "Auto");
            assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        }
        else
        {
            returnCode = PvAttrEnumSet(hCamera_, "WhitebalMode", "Manual");
            assert(returnCode == 0 || returnCode == ePvErrUnplugged);
            
            if (colour[0] == 'R')
            {
                returnCode = PvAttrUint32Set(handle(), "WhitebalValueRed", value);            
                assert(returnCode == 0 || returnCode == ePvErrUnplugged);    
            }
            else if (colour[0] == 'B')
            {
                returnCode = PvAttrUint32Set(handle(), "WhitebalValueBlue", value);            
                assert(returnCode == 0 || returnCode == ePvErrUnplugged);
            }
        }
    }

    void Camera::setGain(bool autoMode, unsigned long gain)
    {
        tPvErr returnCode;

        if (autoMode)
        {
            returnCode = PvAttrUint32Set(handle(), "GainAutoMax", 24);
            assert(returnCode == 0 || returnCode == ePvErrUnplugged);
            returnCode = PvAttrUint32Set(handle(), "GainAutoMin", 0);
            assert(returnCode == 0 || returnCode == ePvErrUnplugged);
            returnCode = PvAttrEnumSet(handle(), "GainMode", "Auto");
            assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        }
        else
        {
            returnCode = PvAttrEnumSet(handle(), "GainMode", "Manual");
            assert(returnCode == 0 || returnCode == ePvErrUnplugged);
            returnCode = PvAttrUint32Set(handle(), "GainValue", gain);            
            assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        }
    }

    void Camera::adjustPacketSize(unsigned long packetSize)
    {
        tPvErr returnCode = PvCaptureEnd(handle());
        returnCode = PvCaptureAdjustPacketSize(handle(), packetSize);//*/PvAttrUint32Set(handle(), "PacketSize", packetSize);
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        returnCode = PvCaptureStart(handle());
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////get camera information
    int Camera::uniqueID()
    {
        unsigned long id;
        tPvErr returnCode = PvAttrUint32Get(handle(), "UniqueId", &id);
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        return id;
    }

    void Camera::setCameraName(const wxString& cameraName)
    {
        tPvErr returnCode = PvAttrStringSet(handle(), "CameraName", cameraName.ToAscii());
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
    }

    wxString Camera::cameraName()
    {
        char buffer[256];
        tPvErr returnCode = PvAttrStringGet(handle(), "CameraName", buffer, 256, NULL);
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        size_t length = strlen(buffer);
        wxString name(buffer, length);
        return name;
    }
  
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////get camera statistics
    float Camera::actualFrameRate()
    {
        float frameRate;
        tPvErr returnCode = PvAttrFloat32Get(handle(), "StatFrameRate", &frameRate);
        assert(returnCode == 0 || returnCode == ePvErrUnplugged);
        return frameRate;
    }

} //namespace