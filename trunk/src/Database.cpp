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

#include "Database.h"

namespace rics
{
	Database::Database():
	db_(NULL)
	{
	}

	Database::~Database()
	{
		databaseClose();
	}

    void Database::openDatabase(const wxString& filename)
    {
		sqlite3_open(filename.ToAscii(), &db_);

		//The following 2 pragmas will speed up disk I/O. Please 
		char *errMsg1 = 0;
		int code = sqlite3_exec(db_, "PRAGMA synchronous=OFF", NULL, 0, &errMsg1);
		sqlite3_free(errMsg1);

		char *errMsg2 = 0;
		code = sqlite3_exec(db_, "PRAGMA journal_mode=OFF", NULL, 0, &errMsg2);
		sqlite3_free(errMsg2);

		//This avoids each new SQL statement having a new
		//transaction started for it, which is very expensive.
		//***This method is curently not in use as it was thought that 
		//writing to the DB in one big chunk is dangerous is the OS
		//crashes etc... ***
		/*char *errMsg = 0;
		int code = sqlite3_exec(db_, "BEGIN;", NULL, 0, &errMsg);
		sqlite3_free(errMsg);*/
    }
    
    void Database::createTable(const wxString& table)
    {
        wxString tb = "create table " +
                      table + 
                      "(frame, time, latitude, longitude, speed, bearing, satellites, fix_quality, camera_ID)";
        
		char *errMsg = 0;
        int code = sqlite3_exec(db_, tb.ToAscii(), NULL, 0, &errMsg);//can't create the same table twice!!
		sqlite3_free(errMsg);
    }
    
    void Database::databaseEnterData(const wxString& table,
                                     long frame, 
									 wxString& timeStamp,  
									 wxString& lat,
									 wxString& lon,
									 wxString& speed,
                                     wxString& bearing,
                                     wxString& satellites,
                                     wxString& fixQuality,
                                     wxString& cameraID)
    {
        wxString data = "insert into " + table + " values(" +
                        boost::lexical_cast<std::string>(frame) + "," +
                        timeStamp + "," +
                        lat + "," +
                        lon + "," +
                        speed + "," +
                        bearing + "," +
                        satellites + "," +
                        fixQuality  + "," +
                        cameraID +
                        ")";
        
		char *errMsg = 0;
        int code = sqlite3_exec(db_, data.ToAscii(), NULL, 0, &errMsg);
		sqlite3_free(errMsg);
    }
 
    //Maximum frame number recorded in the database.
	long int Database::maxFrame(const wxString& table)
	{
		wxString sql = "select count(frame) from " + table;
		char* errMsg = 0;
        int code = sqlite3_exec(db_, sql.ToAscii(), callbackCount, 0, &errMsg);//get number of rows in db.
		sqlite3_free(errMsg);

		if (!countFrames)
		{
			return 0;
		}
		else
		{
			return countFrames - 1;
		}
	}

    //Begin and transaction are the begin and end of a sqlite sentence used to write
    //to database. Although not currently used, by writing a complete sentence to the
    //database only once dramatically speeds up database writes.
	void Database::beginTransaction()
	{
		char *errMsg = 0;
		int code = sqlite3_exec(db_, "BEGIN;", NULL, 0, &errMsg);
		sqlite3_free(errMsg);
	}

	void Database::endTransaction()
	{
		char *errMsg = 0;
		int code = sqlite3_exec(db_, "END;", NULL, 0, &errMsg);//sync is off to speed up db write
		sqlite3_free(errMsg);
	}

    void Database::databaseClose()
    {	
		if (db_ != NULL)//Close database if it does not exist.
		{
			//This avoids each new SQL statement having a new
			//transaction started for it, which is very expensive.
			//***Please note that this is currently not in use.***
            /*char *errMsg = 0;
			int code = sqlite3_exec(db_, "END;", NULL, 0, &errMsg);//sync is off to speed up db write
			sqlite3_free(errMsg);*/

			sqlite3_close(db_);
		}
    }

}// namespace rics