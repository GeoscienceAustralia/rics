/*
Author: Nariman Habili

Description: Dialog to control GPS settings.

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

#ifndef GPSPROPDIALOG_H
#define GPSPROPDIALOG_H

#include "GPS.h"
#include <wx/wx.h>
#include <wx/notebook.h>

namespace rics
{
    class GPSPropDialog : public wxDialog
    {
    public:
        GPSPropDialog(wxWindow *parent, const wxString &title, GPS* gps);

        void createGPSSettingsPage();

        void searchPorts();

    private:
        void onOK(wxCommandEvent& WXUNUSED(event));
        
        void onComboBoxPort(wxCommandEvent& WXUNUSED(event));

        void onClose(wxCloseEvent& WXUNUSED(event));

        void onSearchPorts(wxCommandEvent& WXUNUSED(event));
    
    private:
        GPS* gps_;
        
        int selectPort_;
        wxPanel *panel_;
        wxComboBox* port_;
        
        enum
        {
            ID_OK = 1,
            ID_ComboBoxPort,
            ID_Button
        };

        DECLARE_EVENT_TABLE()
    
    };
} //namespace rics

#endif //GPSPROPDIALOG_H