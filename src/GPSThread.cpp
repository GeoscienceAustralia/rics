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

#include "GPSThread.h"
#include <limits>

namespace rics
{
    GPSThread::GPSThread()
    {
    }
    
    GPSThread::GPSThread(wxWindow* canvas, GPS* gps, SharedGPSDataPtr buffer):
    canvas_(canvas),
    gps_(gps),
    buffer_(buffer)
    {
    }

    GPSThread::~GPSThread()
    {
    }

    void* GPSThread::Entry()
    {
        unsigned long bytesRead = 0;
        unsigned char buffer[1000];

        while(!TestDestroy())
        {
            buffer_->writeLock();

            if (gps_->gpsActive())
            {
                // Read data from the COM-port
                bool error = gps_->readBuffer(buffer, sizeof(buffer) - 1, &bytesRead);
                if (error != ERROR_SUCCESS)
                {
                    //Not read data from COM port. 
                    //Best thing to do here I think is to re-issue
                    //the previous GPS data.
                }

                //If more than 0 bytes in the buffer, send buffer to NMEA parser.
                if (bytesRead > 0)
                {
                    gps_->parse(buffer, (int)bytesRead);
                }

                //Get GPS data and set in buffer.
                wxString latitude = gps_->latitude();
                buffer_->setLatitude(latitude);

                wxString longitude = gps_->longitude();
                buffer_->setLongitude(longitude);

                buffer_->setLatSexagesimal(gps_->latSexagesimal());
                buffer_->setLonSexagesimal(gps_->lonSexagesimal());

                wxString bearing = gps_->bearing();
                buffer_->setBearing(bearing);

                wxString formatBear = gps_->formatBear();
                buffer_->setFormatBear(formatBear);

                wxString timeStamp = gps_->timeStamp();
                buffer_->setTimeStamp(timeStamp);

                wxString formatTime = gps_->formatTime();
                buffer_->setFormatTime(formatTime);

                wxString speed = gps_->speed();
                buffer_->setSpeed(speed);

                wxString formatSpeed = gps_->formatSpeed();
                buffer_->setFormatSpeed(formatSpeed);

                wxString sats = gps_->satellites();
                buffer_->setSatellites(sats);

                wxString quality = gps_->quality();
                buffer_->setQuality(quality);
            }
            //If GPS isn't active.
            else
            {
                buffer_->setLatitude("--");
                buffer_->setLongitude("--");
                buffer_->setLatSexagesimal("--");
                buffer_->setLonSexagesimal("--");
                buffer_->setBearing("--");
                buffer_->setFormatBear("--");
                buffer_->setTimeStamp("--");
                buffer_->setFormatTime("--");
                buffer_->setSpeed("--");
                buffer_->setFormatSpeed("--");
                buffer_->setSatellites("--");
                buffer_->setAltitude("--");
                buffer_->setFormatAltitude("--");
                buffer_->setQuality("--");
            }
            
            wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, GPS_EVENT);
            wxPostEvent(canvas_, event);
            
            buffer_->writeUnlock();

            Sleep(100);//Thread is put to sleep to avoid thread starvation.
        }

        return NULL;
    }

    void GPSThread::OnExit()
    {
    }

}//namespace