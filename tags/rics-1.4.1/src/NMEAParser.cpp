/*
Author: Nariman Habili

Description: NMEA 0183 parser. It parses the GPRMC and GPGGA sentences.

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

#include "NMEAParser.h"

namespace rics
{
    NMEAParser::NMEAParser():
    time_("0"),
    lat_(0),
    lon_(0),
    quality_(0),
    sats_(0),
    speed_(0),
    bearing_(0),
    date_("0"),
    sentenceType_(NONE),
    checksumRMCPass_(false),
    checksumGGAPass_(false)
    {
    }

    NMEAParser::~NMEAParser()
    {
    }

    //NMEA 0183 parser
    void NMEAParser::parse(const unsigned char *nmeaBuf, const int bufSize)
    {
        wxString temp(nmeaBuf, bufSize);
        int index0;

        while ((index0 = temp.Find('$')) != wxNOT_FOUND)
        {
            nmeaString_ += temp.BeforeFirst('$');

            //First look for GPRMC sentences.
            int index1 = nmeaString_.Find("GPRMC");
            if (index1 != wxNOT_FOUND)
            {
                processGPRMC();
                sentenceType_ = RMC;
            }

            //And now look for GPGGA sentences. Note that the order is not important.
            index1 = nmeaString_.Find("GPGGA");
            if (index1 != wxNOT_FOUND)
            {
                processGPGGA();
                sentenceType_ = GGA;
            }

            temp = temp.Mid(index0 + 1);
            nmeaString_.Clear();
        }

        nmeaString_ += temp;
    }


    //Parse the GPRMC sentence
    void NMEAParser::processGPRMC()
    {
        //checksum. With checksum, the $ and *xx are stripped and the 
        //checksum value is calculated by X-ORing all the data 
        //characters together.
        wxString checksumString = nmeaString_.AfterFirst('*');
        wxString beforeCheckSum = nmeaString_.BeforeFirst('*');
        
        const char* dataPointer = beforeCheckSum.c_str();
        char checksumChar = 0;

        while (*dataPointer != '\0')
        {
            checksumChar ^= *dataPointer; //X-ORing here
            dataPointer++;
        }
    
        long csLong1;
        checksumString.ToLong(&csLong1, 16);

        wxString cs;
        long csLong2;
        cs.Printf("%x", checksumChar);
        cs.ToLong(&csLong2, 16);

        if (csLong1 == csLong2)
        {
            checksumRMCPass_ = true;
        }
        else
        {
            //the checksum fails. GGA used to populate parameters.
            checksumRMCPass_ = false;
            return;
        }

        //time
        wxString nmeaString = nmeaString_.Mid(7);
        time_ = nmeaString.BeforeFirst(',');

        //Status
        wxString nmea = nmeaString.AfterFirst(',');
        wxString status = nmea.BeforeFirst(',');

        //latitude
        nmea = nmea.AfterFirst(',');
        wxString lat0 = nmea.BeforeFirst(',');
        nmea = nmea.AfterFirst(',');
        wxString lat1 = nmea.BeforeFirst(',');
        double lat;
        lat0.ToDouble(&lat);
        if (lat1 == "N")
        {
            lat_ = lat;
        }
        else
        {
            lat_ = -lat;
        }

        //longitude
        nmea = nmea.AfterFirst(',');
        wxString lon0 = nmea.BeforeFirst(',');
        nmea = nmea.AfterFirst(',');
        wxString lon1 = nmea.BeforeFirst(',');
        double lon;
        lon0.ToDouble(&lon);
        if (lon1 == "E")
        {
            lon_ = lon;
        }
        else
        {
            lon_ = -lon;
        }

        //Speed
        nmea = nmea.AfterFirst(',');
        wxString speed = nmea.BeforeFirst(',');
        speed.ToDouble(&speed_);

        //Bearing
        nmea = nmea.AfterFirst(',');
        wxString bear = nmea.BeforeFirst(',');
        bear.ToDouble(&bearing_);

        //Date
        nmea = nmea.AfterFirst(',');
        date_ = nmea.BeforeFirst(',');
    }

    //Parse the GPGGA sentence
    void NMEAParser::processGPGGA()
    {
        //checksum. With checksum, the $ and *xx are stripped and the 
        //checksum value is calculated by X-ORing all the data 
        //characters together.      
        wxString checksumString = nmeaString_.AfterFirst('*');
        wxString beforeCheckSum = nmeaString_.BeforeFirst('*');
        
        const char* dataPointer = beforeCheckSum.c_str();
        char checksumChar = 0;

        while (*dataPointer != '\0')
        {
            checksumChar ^= *dataPointer;
            dataPointer++;
        }
    
        long csLong1;
        checksumString.ToLong(&csLong1, 16);

        wxString cs;
        long csLong2;
        cs.Printf("%x", checksumChar);
        cs.ToLong(&csLong2, 16);

        if (csLong1 == csLong2)
        {
            checksumGGAPass_ = true;
        }
        else
        {
            checksumGGAPass_ = false;
            return;
        }

        //time
        wxString nmeaString = nmeaString_.Mid(7);
        if (!checksumRMCPass_)
        {
            time_ = nmeaString.BeforeFirst(',');
        }

        //latitude
        wxString nmea = nmeaString.AfterFirst(',');
        wxString lat0 = nmea.BeforeFirst(',');
        nmea = nmea.AfterFirst(',');
        if (!checksumRMCPass_)
        {
            wxString lat1 = nmea.BeforeFirst(',');
            double lat;
            lat0.ToDouble(&lat);
            if (lat1 == "N")
            {
                lat_ = lat;
            }
            else
            {
                lat_ = -lat;
            }
        }

        //longitude
        nmea = nmea.AfterFirst(',');
        wxString lon0 = nmea.BeforeFirst(',');
        nmea = nmea.AfterFirst(',');
        if (!checksumRMCPass_)
        {
            wxString lon1 = nmea.BeforeFirst(',');
            double lon;
            lon0.ToDouble(&lon);
            if (lon1 == "E")
            {
                lon_ = lon;
            }
            else
            {
                lon_ = -lon;
            }
        }

        //Fix quality
        nmea = nmea.AfterFirst(',');
        wxString quality = nmea.BeforeFirst(',');
        quality.ToLong(&quality_);

        //Number of satellites
        nmea = nmea.AfterFirst(',');
        wxString sats = nmea.BeforeFirst(',');
        sats.ToLong(&sats_);
    }

}