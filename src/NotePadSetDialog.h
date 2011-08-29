/*
Author: Nariman Habili

Description: Dialog to control the properties of the notepad widget.

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

#ifndef NOTEPADSETDIALOG_H
#define NOTEPADSETDIALOG_H

#include "NotePad.h"
#include <wx/filepicker.h>
#include <wx/wx.h>

namespace rics
{
    class NotePadSetDialog : public wxDialog
    {
    public:
        NotePadSetDialog (wxWindow *parent, const wxString &title, NotePad* np);

        void createNotePadSettingsPage();

    private:
        void onCheckBoxLatLon(wxCommandEvent& WXUNUSED(event));
        void onCheckBoxDamageLevels(wxCommandEvent& WXUNUSED(event));
        void onFile(wxFileDirPickerEvent& event);

        void onClear(wxCommandEvent& WXUNUSED(event));
        void onOK(wxCommandEvent& WXUNUSED(event));
        void onClose(wxCloseEvent& event);

	private:
        NotePad* np_;

        wxPanel* panel_;
        wxButton* clearButton_;
        wxCheckBox* checkBoxLatLonBear_;
        wxCheckBox* checkBoxDamageLevels_;
        wxFilePickerCtrl* damageLevelsFile_;
        
        wxTextCtrl* FText_[8];

        enum
        {
            ID_LatLon = 1,
            ID_DamageLevels,
            ID_File,
            ID_Clear,
            ID_OK,

            ID_F1,
            ID_F2,
            ID_F3,
            ID_F4,
            ID_F5,
            ID_F6,
            ID_F7,
            ID_F8,
        };

        DECLARE_EVENT_TABLE()
    
    };
} //namespace rics

#endif //NOTEPADSETDIALOG_H