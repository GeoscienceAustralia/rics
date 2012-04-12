/*
Author: Nariman Habili

Description: A widget to display captured image.

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

#include "ImagePanel.h"
#include "wx/statline.h"
#include "wx/dcbuffer.h"
#include "boost/lexical_cast.hpp"

namespace rics
{

    ImagePanel::ImagePanel(wxPanel *parent, wxWindowID id, wxString &camera, const wxPoint &pos, const wxSize &size):
    wxPanel(parent,  id, pos, size),
    camera_(camera),
    font_(8, -1, wxNORMAL, wxLIGHT, false)
    {
        SetBackgroundStyle(wxBG_STYLE_CUSTOM);
        image_ = wxBitmap(306, 256, -1);
    }

    ImagePanel::~ImagePanel()
    {
    }

    //Draw a new camera image on a panel
    void ImagePanel::onPaint(wxPaintEvent &WXUNUSED(event))
    {
        wxBufferedPaintDC dc(this);
        dc.DrawBitmap(image_, 0, 0);
        dc.SetTextForeground(*wxRED);
        dc.SetFont(font_);
        dc.DrawText(camera_, 10, 10);
    }

    //Draw a new camera image on a panel - will envoke onPaint
    void ImagePanel::refreshImage(wxBitmap& image)
    {
        image_ = image;
        RefreshRect(wxRect(0, 0, 306, 256));
    }

    //To draw the camera's unique ID. This is done once.
    void ImagePanel::setCameraName(wxString& camera)
    {
        camera_ = camera;
        RefreshRect(wxRect(0, 0, 306, 256));
    }

    BEGIN_EVENT_TABLE(ImagePanel, wxPanel)
        EVT_PAINT(ImagePanel::onPaint)
    END_EVENT_TABLE()

} //namespace