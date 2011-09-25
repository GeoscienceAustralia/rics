/*
Author: Nariman Habili

Description: A widget to display captured image.

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

#ifndef IMAGEPANEL_H
#define IMAGEPANEL_H

#include "vld.h"
#include <wx/wx.h>
#include <wx/file.h>

namespace rics
{
    class ImagePanel: public wxPanel
    {
    public:
        ImagePanel(wxPanel* parent,
                   wxWindowID id,
                   wxString& camera,
                   const wxPoint &pos = wxDefaultPosition,
                   const wxSize &size = wxDefaultSize);
        ~ImagePanel();

        void drawMyImage(wxPaintDC& dc);
        void refreshImage(wxBitmap& image);
        void setCameraName(wxString& camera);

    private:
        void onPaint(wxPaintEvent &WXUNUSED(event));

    private:
        wxBitmap image_;
        wxString camera_;
        wxFont font_;

        DECLARE_EVENT_TABLE()

    };

}

#endif //PANEL_H
