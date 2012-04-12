/*
Author: Nariman Habili

Description: Holds the current session state.

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

#ifndef SESSION_H
#define SESSION_H

#include <wx/string.h>
#include <vector>

namespace rics
{
    class Session
    {
    public:
        Session(size_t numCameras):
        numCameras_(numCameras),
        saveImages_(false),
        db_(false),
        currentFrame_(numCameras_, 0),
        sessionName_(""),
        path_("")
        {
        }

        ~Session()
        { 
        }

        bool saveImages() const
        {
            return saveImages_;
        }

        void setSaveImages(bool save)
        {
            saveImages_ = save;
        }

        bool createDB() const
        {
            return db_;
        }

        void setCreateDB(bool db)
        {
            db_ = db;
        }

        wxString sessionName() const
        {
            return sessionName_;
        } 

        void setSessionName(const wxString& sn)
        {
            sessionName_ = sn;
        }

        bool sessionNameIsEmpty()
        {
            if (sessionName() == "")
            {
                return true;
            }

            return false;
        }

        wxString path() const
        {
            return path_;
        }

        void setPath(const wxString& path)
        {
            path_ = path;
        }

        void setCurrentFrame(size_t camera, long int cf)
        {
            currentFrame_[camera] = cf;
        }

        unsigned long currentFrame(size_t camera) const
        {
            return currentFrame_[camera];
        }

        void initCurrentFrame()
        {
            for (size_t i = 0; i < numCameras_; ++i)
            {
                currentFrame_[i] = 0;
            }
        }

    private:
        size_t numCameras_;
        bool saveImages_;
        bool db_;
        std::vector<long int> currentFrame_;

        wxString sessionName_;
        wxString path_;
    };
}

#endif //SESSION_H