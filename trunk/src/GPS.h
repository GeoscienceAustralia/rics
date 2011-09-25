/*
Author: Nariman Habili

Description: The GPS is initiased here and its properties are set here. 
             The GPS values are also formatted here.

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

#ifndef GPS_H
#define GPS_H

#include "NMEAParser.h"
#include "Serial.h"
#include "wx/string.h"

namespace rics
{
    class GPS
    {
    public:
        GPS();
        ~GPS();

        bool openPort(int portNumber);
        void setCurrentPort(int port);
        int currentPort();
        void close();
        int bytesReceived();
        void setGPSActive(bool active);
        bool gpsActive() const;
        bool readBuffer(unsigned char* buffer, const int bufSize, unsigned long* bytesRead);
        void parse(const unsigned char *buffer, const int bufSize);
        wxString latitude();
        wxString longitude();
        wxString latSexagesimal();
        wxString lonSexagesimal();
        wxString bearing();
        wxString formatBear();
        wxString timeStamp();
        wxString formatTime();
        wxString date();
        wxString speed();
        wxString formatSpeed();
        wxString satellites();
        wxString altitude();
        wxString formatAltitude();
        wxString quality();
        NMEAParser::SentenceType sentenceType();

    private:
        NMEAParser nmeaParser_;
        CSerial serial_;
        int currentPort_;
        bool gpsActive_;
        int bearing_;
    };

} //namespace rics

#endif //GPS_H