Rapid Inventory Collection System
=================================

Nariman Habili (nariman.habili@gmail.com)

29 August 2011

Version 1.4.0

Description
===========
The Rapid Inventory Collection System (RICS) is used to capture and save geo-tagged digital imagery of built environments. 
Up to four high resolution cameras capture images which are then displayed in the RICS Graphical User Interface (GUI) 
and saved in the JPEG format. It is typically used for building and infrastructure damage assessment following a natural 
disaster. The captured images are analysed in Geographical Information System (GIS). RICS was deployed following the 
2009 Victorian Bushfires, the 2010 Kalgoorlie and Christchurch Earthquakes, 2011 Brisbane floods and Tropical Cyclone 
Yasi to collect geo-tagged imagery of structures and buildings.


Platforms Supported
===================
The RICS software was developed for Microsoft Windows using Microsoft Visual 2008 C++ Express. It has been tested on Windows XP, Vista and 7 (32 and 64 bits).


Third Party Libraries
=====================
The software uses the following third party libraries:

wxWidgets (http://www.wxwidgets.org/)
------------------------------------
Download the latest stable version of wxWidgets (currently 2.8.12) and build wx.dsw in $(WX)/build/msw where $(WX) is the location of your wxWidgets folder. 
The built static libraries are located in $(WX)lib/vc_lib.

Boost C++ library (http://www.boost.org/)
-----------------------------------------
The Boost library can be simply downloaded and unzipped only as the classes used in RICS are 
template classes.

libjpeg-turbo library (http://www.libjpeg-turbo.org/)
-----------------------------------------------------
Build instructions are available at http://www.libjpeg-turbo.org/DeveloperInfo/BuildInstructions.

JPEGWriter C++ code (http://cs.nyu.edu/~ajsecord/downloads.html)
----------------------------------------------------------------
The JPEG.h and JPEGWriter.cpp/h files from JPEGLoader-1.0.1.zip are provided in the vendor folder and are included in rics.vcproj.

Visual Leak Detector library
----------------------------

SQLite (http://www.sqlite.org/)
-------------------------------
The sqlite amalgamation file is used in RICS. The sqlite3.c/h are provided in the vendor folder and are included in rics.vcproj.
 
Serial Library for C++ (http://www.codeproject.com/KB/system/serial.aspx)
-------------------------------------------------------------------------
Download Serial_demo.zip, open Serial.dsw and build. 

Allied Vision Technologies (AVT) SDK
------------------------------
Please refer to the AVT manual, http://www.alliedvisiontec.com/fileadmin/content/PDF/Products/Technical_Manual/GigE_Install_Manual/70-0062_GigE_Installation_Manual_V2.0.pdf

Installation
============
1) Define the following PATH environments: WX (wxWidgets), BOOST (Boost C++ libraries), AVT (Allied Vision Technologies SDK), 
VLD (Visual Leak Detector), JPEG_TURBO (libjpeg-turbo library) and SERIAL (serial library). 

2) Open rics.vcproj (this is in MS Visual 2008 format) and compile. Note that if you have a later version of MS Visual Studio 
(or MS Visual C++ Express), vcproj will be converted automatically.
