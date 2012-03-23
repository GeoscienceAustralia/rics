/*
Author: Nariman Habili

Description: Information on different versions.

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

#ifndef VERSION_H
#define VERSION_H

/****************************************************************************
1.0.0:
1.0.1: Car navigation image (on GUI) modified
1.0.2: Change to version 2.0.2 of mgc32.dll
1.1.0: In "New Session" dialog, changed label "Create Database" to "Create GPS Database"
       GPS device needs to be found on correct COM port only if GPS Database is created, eg in play mode, 
       image streaming can occur without locating the GPS device on the correct COM port.
1.1.1 Exposure time is now auto by default
      Camera name defaults changed so that N is now S
      ArcPad 7.0 to ArcPad 7.1
1.2.0 Hotkeys for controls (eg Ctrl-P and Ctrl-S for play and stop respectively)
      Test and open session icons added
      Open sessions via directory structure
      Test and session mode texts on status bar
      Circle icon indicating recording (play in session mode)
      Default camera names changed
      Car navigation image modified to reflect new camera names
1.3.0 Changed to version 1.20 of the Prosilica API
      Added camera gain
      Added camera ID (uniqueID) as a field to the SQLite database
      Removed automatic launch of ArcPad from RICS.
1.4.0 Changed to jpeg-turbo to speed up image compression.
      Changed sqlite settings to speed up database writes.
      Added max exposure time to counter the effect of cameras passing a dark region
      and causing image blur for a few images afterwards.
      Addition of an NMEA 0183 parser and open-source serial communication 
      library to replace the Marshallsoft GPS component. The codebase is now completely 
      open-source.
      GPS date/time now added to session names (optional).
      Added bearing (heading) to the time/lat/lon already recorded in the
      notepad spreadsheet.
1.4.1 Turned on SQLite journaling to save against database corruption.
****************************************************************************/

#define RICS_VERSION "1.4.1"
 
#endif //VERSION_H