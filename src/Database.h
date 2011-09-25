/*
Author: Nariman Habili

Description: Creates, opens, adds and closes the sqlite database.

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

#ifndef Database_H
#define Database_H

#include "sqlite3.h"
#include <wx/string.h>
#include <boost/lexical_cast.hpp>
#include <boost/shared_array.hpp>

namespace rics
{
    static long int countFrames;
    static int callbackCount(void* maxFrame, int argc, char **argv, char **azColName)
    {  
        countFrames = boost::lexical_cast<long int>(argv[0] ? argv[0] : "NULL");
        return 0;
    }

    class Database
    {
    public:
        Database();
        ~Database();
        
        void openDatabase(const wxString& filename);
        void createTable(const wxString& table);
        void databaseEnterData(const wxString& table,
                               long frame, 
                               wxString& timeStamp,
                               wxString& lat,
                               wxString& lon,
                               wxString& speed,
                               wxString& bearing,
                               wxString& satellites,
                               wxString& fixQuality,
                               wxString& cameraID);
        long int maxFrame(const wxString& table);
        void beginTransaction();
        void endTransaction();
        void databaseClose();

    private:
        sqlite3 *db_;
    };

    typedef boost::shared_array<wxString> TableNames;

}// namespace rics

#endif //Database_H