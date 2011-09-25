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

#include "GPS.h"
#include <boost/lexical_cast.hpp>

namespace rics
{
    GPS::GPS():
    currentPort_(0),
    gpsActive_(false),
    bearing_(0)
    {
    }

    GPS::~GPS()
    {
        close();
    }

    //Open COM port for GPS receiver
    bool GPS::openPort(int portNumber)
    {
        setCurrentPort(portNumber);
        wxString port = "COM" + boost::lexical_cast<std::string>(portNumber);

        long error = serial_.Open(port, 0, 0, false);
        if (error != ERROR_SUCCESS)//Could not open
        {
            return false;
        }

        //Current COM port defaults. These could be moved to the GUI as user set options.
        error = serial_.Setup(CSerial::EBaud4800, 
                              CSerial::EData8, 
                              CSerial::EParNone, 
                              CSerial::EStop1);
        if (error != ERROR_SUCCESS)
        {
            return false;
        }

        // Use 'non-blocking' reads, because we don't know how many bytes
        // will be received. This is normally the most convenient mode
        // (and also the default mode for reading data).
        error = serial_.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
        if (error != ERROR_SUCCESS)
        {
            return false;
        }

        return true;
    }

    void GPS::setCurrentPort(int port)
    {
        currentPort_ = port;
    }
    
    int GPS::currentPort()
    {
        return currentPort_;
    }

    //Close the COM port.
    void GPS::close()
    {
        serial_.Close();
    }

    //Currently not in use as WaitEvent() hangs if 
    //COM port can be opened but there is no data.
    int GPS::bytesReceived()
    {
        long error = serial_.WaitEvent();
        if (error != ERROR_SUCCESS)
        {
            return false;
        }

        // Save event
        const CSerial::EEvent eEvent = serial_.GetEventType();

        // Handle break event
        if (eEvent & CSerial::EEventBreak)
        {
            return false;
        }
        // Handle error event
        else if (eEvent & CSerial::EEventError)
        {
            return false;
        }
        // If received, return true
        else if (eEvent & CSerial::EEventRecv)
        {
            return true;
        }
        else //some other issue
        {
            return false;
        }
    }
    
    //Has GPS been successfully detected?
    void GPS::setGPSActive(bool active)
    {
        gpsActive_ = active;
    }

    bool GPS::gpsActive() const
    {
        return gpsActive_;
    }

    //Read GPS buffer.
    bool GPS::readBuffer(unsigned char* buffer, 
                         const int bufSize, 
                         unsigned long* bytesRead)
    {
        long error = serial_.Read((void *)buffer, bufSize, bytesRead);
                
        if (error == ERROR_SUCCESS)
        {
            return false;
        }

        return true;
    }

    //Send current buffer to NMEA parser.
    void GPS::parse(const unsigned char *buffer, const int bufSize)
    {
        nmeaParser_.parse(buffer, bufSize);
    }

    //Return current latitude in degrees.
    wxString GPS::latitude()
    {
        double gpsLat = nmeaParser_.latitute();

        int degs = (int)(gpsLat/100.0f);
        double lat = degs + (gpsLat - 100.0f*degs)/60.0f;

        wxString s;
        s.Printf("%.6f", lat);
        return s;   
    }

    //Return current longitude in degrees.
    wxString GPS::longitude()
    {
        double gpsLon = nmeaParser_.longitude();
        int deg = (int)(gpsLon/100.0f);
        double lon = deg + (gpsLon - 100.0f*deg)/60.0f;
        
        wxString s;
        s.Printf("%.6f", lon);
        return s;
    }

    //Return current latitude in formatted degrees, minutes and seconds format.
    wxString GPS::latSexagesimal()
    {
        double gpsLat = nmeaParser_.latitute(); 
        int deg = (int)(gpsLat/100.0f);
        int min = (int)(gpsLat - deg*100);
        int sec = 60*(gpsLat - (int)gpsLat);

        if (gpsLat >= 0)
        {
            return boost::lexical_cast<std::string>(deg) + wxString((wxChar)(-80)) + " " +
                   boost::lexical_cast<std::string>(min) + "' " +
                   boost::lexical_cast<std::string>(sec) + "'' N";
        }
        else
        {
            return boost::lexical_cast<std::string>(-deg) + wxString((wxChar)(-80)) + " " +
                   boost::lexical_cast<std::string>(-min) + "' " +
                   boost::lexical_cast<std::string>(-sec) + "'' S";
        }
    }

    //Return current longitude in formatted degrees, minutes and seconds format.
    wxString GPS::lonSexagesimal()
    {
        double gpsLon = nmeaParser_.longitude(); 
        int deg = (int)(gpsLon/100.0f);
        int min = (int)(gpsLon - deg*100);
        int sec = 60*(gpsLon - (int)gpsLon);

        if (gpsLon >= 0)
        {   //((wxChar)(-80) is superscript o.
            return boost::lexical_cast<std::string>(deg) + wxString((wxChar)(-80)) + " " +
                   boost::lexical_cast<std::string>(min) + "' " +
                   boost::lexical_cast<std::string>(sec) + "'' E";
        }
        else
        {
            return boost::lexical_cast<std::string>(-deg) + wxString((wxChar)(-80)) + " " +
                   boost::lexical_cast<std::string>(-min) + "' " +
                   boost::lexical_cast<std::string>(-sec) + "'' W";
        }
    }

    //Return current bearing or "heading".
    wxString GPS::bearing()
    {
        double bearing = nmeaParser_.bearing();
        wxString b;
        b.Printf("%.1f", bearing);
        return b;
    }

    //bearing() must be called first
    wxString GPS::formatBear()
    {
        return bearing() + wxString((wxChar)(-80));
    }
    
    //Current time from GPS receiver.
    wxString GPS::timeStamp()
    {
        return nmeaParser_.time();
    }

    //Re-format the timestamp for display.
    wxString GPS::formatTime()
    {
        wxString ts = timeStamp();
        wxString utcTime;
        
        switch (ts.Len()) 
        {
        case 1:
            utcTime = "00:00:0" + ts;
            break;
        case 2:
            utcTime = "00:00:" + ts;
            break;
        case 3:
            utcTime = "00:0" + (ts.Mid(0, 1) + (":" + ts.Mid(1)));
            break;
        case 4: 
            utcTime = "00:" + (ts.Mid(0, 2) + (":" + ts.Mid(2)));
            break;
        case 5:
            utcTime = "0" + (ts.Mid(0, 1) + (":" + (ts.Mid(1, 2) + (":" + ts.Mid(3)))));
            break;
        case 6:
            utcTime = ts.Mid(0, 2) + (":" + (ts.Mid(2, 2) + (":" + ts.Mid(4))));
            break;
        default:
            utcTime = "00:00:00";
            break;
        }

        return utcTime;
    }

    wxString GPS::date()
    {
        return nmeaParser_.date();
    }

    wxString GPS::speed()
    {
        double speed = nmeaParser_.speed();
        wxString s;
        s.Printf("%.1f", speed);
        return s;
    }

    //Return formatted speed with km/h added.
    wxString GPS::formatSpeed()
    {
        return speed() + " km/h";
    }

    //Return the number of satellites picked up by the GPS receiver.
    wxString GPS::satellites()
    {
        long sats = nmeaParser_.satellites();
        return boost::lexical_cast<std::string>(sats);
    }

    //Return the fix quality of the GPS receiver.
    wxString GPS::quality()
    {
        long quality = nmeaParser_.quality();
        return boost::lexical_cast<std::string>(quality);
    }

    NMEAParser::SentenceType GPS::sentenceType()
    {
        if (!gpsActive())
        {
            return NMEAParser::NONE;
        }

        return nmeaParser_.sentenceType();
    }

} //namespace rics