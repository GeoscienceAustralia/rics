/*
Author: Nariman Habili

Description: Creates a notepad widget for writing geo-coded notes.

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

#include "NotePad.h"
#include "Session.h"

namespace rics
{
	NotePad::NotePad():
	wxTextCtrl(),
    doWriteLatLonBearText_(true)
	{
	}
 
	NotePad::NotePad(wxWindow *parent,
					 wxWindowID id, 
                     const Session* session,
				     wxString* formatTime,
                     wxString* latitude,
                     wxString* longitude,
					 wxString* bearing,
					 const wxString &value, 
					 const wxPoint &pos, 
					 const wxSize &size, 
					 long style, 
					 const wxValidator &validator, 
					 const wxString &name):
	wxTextCtrl(parent, id, value, pos, size, style, validator, name),
	session_(session),
	formatTime_(formatTime),
    latitude_(latitude),
    longitude_(longitude),
	bearing_(bearing)
	{
	}

	NotePad::~NotePad()
	{
	}

    //Create the notepad.
	bool NotePad::create(wxWindow *parent,
						 wxWindowID id, 
                         const Session* session, 
						 wxString* formatTime,
                         wxString* latitude,
                         wxString* longitude,
						 wxString* bearing,
						 const wxString &value,
						 const wxPoint &pos, 
						 const wxSize &size, 
						 long style, 
						 const wxValidator &validator, 
						 const wxString &name)
	{
		session_ = session;
		formatTime_ = formatTime;
        latitude_ = latitude;
        longitude_ = longitude;
		bearing_ = bearing;
		bool code = Create(parent, id, value, pos, size, style, validator, name);
		Disable();
		return code;
	}

    //Open existing csv file.
	void NotePad::openFile()
	{ 
		if (session_->saveImages() || session_->createDB())
		{
			Enable();
			
			wxString filename = session_->path() + 
								"\\" + 
								session_->sessionName() + 
								".csv";

			Clear(); 

			if (wxFileExists(filename))//if file already exists
			{
				LoadFile(filename);
			}
		}
		else
		{
			Clear();
            Disable();
		}
	}

    //Save csv file.
	void NotePad::saveFile()
	{
		if (session_->saveImages() || session_->createDB())
		{
			wxString filename = session_->path() + 
								"\\" + 
								session_->sessionName() + 
								".csv";

			SaveFile(filename);
		}
	}

    //Formatted time, lat, lon, and bear text.
	inline wxString NotePad::timeLatLonBearText()
	{
        wxString timeLatLonBear;

        if (doWriteLatLonBearText())
        {
    		timeLatLonBear = "," + 
						     *formatTime_ + 
						     "," + 
						     *latitude_ + 
						     "," +
						     *longitude_ +
							 "," +
							 *bearing_;
        }
        else
        {
            timeLatLonBear = "";
        }
        
        return timeLatLonBear;
	}

    bool NotePad::doWriteLatLonBearText() const
    {
        return doWriteLatLonBearText_;
    }

    void NotePad::setDoWriteLatLonBearText(bool text)
    {
        doWriteLatLonBearText_ = text;
    }

    inline wxString NotePad::damageLevel(int i) const
    {
        return damageLevels_[i];
    }

    void NotePad::setDamageLevel(const wxString& dL, int i)
    {
        damageLevels_[i] = dL;
    }

    //Add damage level for F2-F9. Add time, lat, long and bear to the end of 
    //a sentence for return.
	void NotePad::onKeyDown(wxKeyEvent& event)
	{
        if (doWriteLatLonBearText() & IsEmpty())
        {
            WriteText("comment,time,lat,lon,bearing\n");
        }
        
        switch(event.GetKeyCode())
        {
            case WXK_F2:
                WriteText(damageLevels_[0] + timeLatLonBearText() + "\n");
                break;
            case WXK_F3:
                WriteText(damageLevels_[1] + timeLatLonBearText() + "\n");
                break;
            case WXK_F4:
                WriteText(damageLevels_[2] + timeLatLonBearText() + "\n");
                break;
            case WXK_F5:
                WriteText(damageLevels_[3] + timeLatLonBearText() + "\n");
                break;
            case WXK_F6:
                WriteText(damageLevels_[4] + timeLatLonBearText() + "\n");
                break;
            case WXK_F7:
                WriteText(damageLevels_[5] + timeLatLonBearText() + "\n");
                break;
            case WXK_F8:
                WriteText(damageLevels_[6] + timeLatLonBearText() + "\n");
                break;
            case WXK_F9:
                WriteText(damageLevels_[7] + timeLatLonBearText() + "\n");
                break;
            case WXK_RETURN:
                WriteText(timeLatLonBearText());
                break;
        }

        saveFile();
		event.Skip();
	}

	BEGIN_EVENT_TABLE(NotePad, wxTextCtrl)
		EVT_KEY_DOWN(NotePad::onKeyDown)
	END_EVENT_TABLE()

}