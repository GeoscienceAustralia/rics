/*
Author: Nariman Habili

Description: GPS data buffer.

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

#ifndef SHARED_GPS_DATA
#define SHARED_GPS_DATA

#include <wx/thread.h>
#include <wx/wx.h>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

namespace rics
{
    class SharedGPSData
    {
    public:
        SharedGPSData():
        cond_(mutex_)
        {
        }

        ~SharedGPSData()
        {
        }
        
        void readLock()
        {
            mutex_.Lock();
        }

        void readUnlock()
        {
            cond_.Signal();
            mutex_.Unlock();
        }

        void writeLock()
        {
            mutex_.Lock();
        }

        void writeUnlock()
        {
            cond_.Wait();
            mutex_.Unlock();
        }
    
        void setLatitude(const wxString& lat)
        {
            latitude_ = lat;
        }

        wxString latitude() const
        {
            return latitude_;
        }

		void setLatSexagesimal(const wxString& ls)
		{
			latSexagesimal_ = ls;
		}
		
		wxString latSexagesimal() const
		{
			return latSexagesimal_;
		}

        void setLongitude(const wxString& lon)
        {
            longitude_ = lon;
        }

        wxString longitude() const
        {
            return longitude_;
        }

		void setLonSexagesimal(const wxString& ls)
		{
			lonSexagesimal_ = ls;
		}
		
		wxString lonSexagesimal() const
		{
			return lonSexagesimal_;
		}

        void setBearing(const wxString& bear)
        {
            bearing_ = bear;
        }

        wxString bearing() const
        {
            return bearing_;
        }

        void setFormatBear(const wxString& fb)
        {
            formatBear_ = fb;
        }

        wxString formatBear() const
        {
            return formatBear_;
        }
        
        void setTimeStamp(const wxString& ts)
        {
            timeStamp_ = ts;
        }

        wxString timeStamp() const
        {
            return timeStamp_;
        }

		void setFormatTime(const wxString& ft)
		{
			formatTime_ = ft;
		}

		wxString formatTime() const
		{
			return formatTime_;
		}

        void setSpeed(const wxString& s)
        {
            speed_ = s;
        }

        wxString speed() const
        {
            return speed_;
        }

        void setFormatSpeed(const wxString& s)
        {
             formatSpeed_ = s;
        }

        wxString formatSpeed() const
        {
            return formatSpeed_;
        }

        void setSatellites(const wxString& s)
        {
            satellites_ = s;
        }
        
        wxString satellites() const
        {
            return satellites_;
        }

        void setAltitude(const wxString& a)
        {
            altitude_ = a;
        }

        wxString altitude() const
        {
            return altitude_;
        }

        void setFormatAltitude(const wxString& a)
        {
             formatAltitude_ = a;
        }

        wxString formatAltitude() const
        {
            return formatAltitude_;
        }

        void setQuality(const wxString& q)
        {
            quality_ = q;
        }

        wxString quality() const
        {
            return quality_;
        }


    private:
        wxMutex mutex_;
        wxCondition cond_;

        wxString latitude_;
        wxString longitude_;
		wxString latSexagesimal_;
		wxString lonSexagesimal_;
        wxString bearing_;
        wxString formatBear_;
        wxString timeStamp_;
		wxString formatTime_;
        wxString speed_;
        wxString formatSpeed_;
        wxString satellites_;
        wxString altitude_;
        wxString formatAltitude_;
        wxString quality_;
    };

    typedef boost::shared_ptr<SharedGPSData> SharedGPSDataPtr;

}//namespace

#endif //SHARED_GPS_DATA