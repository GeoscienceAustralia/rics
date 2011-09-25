/*
Author: Nariman Habili

Description: NMEA 0183 parser.

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

#ifndef NMEAPARSER_H
#define NMEAPARSER_H

#include <iostream>
#include <windows.h>
#include <wx/string.h>

namespace rics
{
    class NMEAParser  
    {
    public:
        typedef enum
        {
          NONE,
          RMC,
          GGA,
        } SentenceType;

    public:
        NMEAParser();
        ~NMEAParser();

        void parse(const unsigned char *nmeaBuf, const int bufSize);

        wxString time()
        {
            return time_.BeforeFirst('.');
        }

        double latitute()
        {
            return lat_;
        }

        double longitude()
        {
            return lon_;
        }

        double quality()
        {
            return quality_;
        }

        long satellites()
        {
            return sats_;
        }

        double speed()
        {
            return speed_ * 1.852; //Speed in kmph, it's originally in knots.
        }

        double bearing()
        {
            return bearing_;
        }

        wxString date()
        {
            return date_;
        }

        SentenceType sentenceType()
        {
            return sentenceType_;
        }   

        void printGPRMC()
        {
            std::cout << "Speed: " << speed() << std::endl;
            std::cout << "Bearing: " << bearing() << std::endl << std::endl;
        }

        void printGPGGA()
        {
            std::cout << "Time: " << time() << std::endl;
            std::cout << "Latitude: " << latitute() 
                      << ", Longitude: "
                      << longitude() 
                      << std::endl;
            std::cout << "Quality: " << quality() << std::endl;
            std::cout << "Satellites: " << satellites() << std::endl << std::endl ;
        }



    private:
        void processGPGGA();
        void processGPRMC();

    public:

    private:
        wxString nmeaString_;
        
        wxString time_;
        double lat_;
        double lon_;
        long quality_;
        long sats_;
        double speed_;
        double bearing_;
        wxString date_;
        SentenceType sentenceType_;
        bool checksumRMCPass_;
        bool checksumGGAPass_;
    };
}
#endif 
