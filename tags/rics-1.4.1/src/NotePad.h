/*
Author: Nariman Habili

Description: Creates a notepad widget for writing geo-coded notes.

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

#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <wx/wx.h>
#include <vector>

namespace rics
{
    class Session;

    class NotePad : public wxTextCtrl
    {
    public:
        NotePad();
        NotePad(wxWindow *parent,
                wxWindowID id,
                const Session* session,
                wxString* formatTime,
                wxString* latitude,
                wxString* longitude,
                wxString* bearing,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxTextCtrlNameStr);
        ~NotePad();

        bool create(wxWindow *parent,
                    wxWindowID id,
                    const Session* session,  
                    wxString* formatTime,
                    wxString* latitude,
                    wxString* longitude,
                    wxString* bearing,
                    const wxString& value = wxEmptyString,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxTextCtrlNameStr);

        void openFile();
        void saveFile();

        bool doWriteLatLonBearText() const;
        void setDoWriteLatLonBearText(bool text);

        void setDamageLevel(const wxString& dL, int i);
        
    private:
        inline wxString timeLatLonBearText();   
        void onKeyDown(wxKeyEvent& event);

        inline wxString damageLevel(int i) const;

    private:
        const Session* session_;
        wxString* formatTime_;
        wxString* latitude_;
        wxString* longitude_;
        wxString* bearing_;

        bool doWriteLatLonBearText_;

        wxString damageLevels_[8];

     DECLARE_EVENT_TABLE()
    };
}

#endif //NOTEPAD_H