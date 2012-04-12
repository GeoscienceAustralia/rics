/*
Author: Nariman Habili

Description: Main entry point for RICS. The cameras are initialised here.

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

#ifndef MYAPP_H
#define MYAPP_H

#include "Frame.h"
#include <wx/wx.h>
#include <wx/snglinst.h>
#include <wx/help.h>
#include <boost/shared_ptr.hpp>

namespace rics
{
    class Camera;
    
    class App: public wxApp
    {
    public:
        App();
        ~App();

        void closeApp();

    private:
        virtual bool OnInit();
        void initCameraHandlersUniqueID();
        void initCameraHandlersIP();

    private:
        unsigned long numCams_;
        std::vector<tPvHandle> hCamera_;
        Cameras cameras_;
        static const unsigned long maxStreamBytesPerSecond_ = 120000000;

        const wxString name_;
        boost::shared_ptr<wxSingleInstanceChecker> instanceChecker_;

        Frame *frame_;
    };

    IMPLEMENT_APP(App)

} //namespace

#endif //MYAPP_H