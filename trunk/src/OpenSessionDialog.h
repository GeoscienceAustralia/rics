/*
Author: Nariman Habili

Description: Dialog to open an existing session.

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

#ifndef OPENSESSIONDIALOG_H
#define OPENSESSIONDIALOG_H

#include "Camera.h"
#include "Session.h"
#include "Database.h"
#include <wx/wx.h>

namespace rics
{
    class Frame;

    class OpenSessionDialog
    {
    public:
        OpenSessionDialog(Frame *frame, Session* session, Cameras* cameras, Database* db);
        ~OpenSessionDialog();

        void open();

    private:
        bool checkDuplicateCameraNames();
        inline Camera& camera(size_t i) const;

    private:
        Frame* frame_;
        Session* session_;
        Cameras* cameras_;
        Database* db_;
        size_t numCameras_;
            
    };
} //namespace rics

#endif //OPENSESSIONDIALOG_H