Rapid Inventory Collection System
---------------------------------

29 August 2011

Version 1.4.0

Description
-----------
The Rapid Inventory Collection System (RICS) is used to capture and save geo-tagged digital imagery of built environments. Up to four high resolution cameras capture images which are then displayed in the RICS Graphical User Interface (GUI) and saved in the JPEG format. It is typically used for building and infrastructure damage assessment following a natural disaster. The captured images are analysed in Geographical Information System (GIS). RICS was deployed following the 2009 Victorian Bushfires, the 2010 Kalgoorlie and Christchurch Earthquakes, 2011 Brisbane floods and Tropical Cyclone Yasi to collect geo-tagged imagery of structures and buildings.

Platforms Supported
-------------------
The RICS software was developed for Microsoft Windows using Microsoft Visual 2008 C++ Express. It has been tested on Windows XP, Vista and 7 (32 and 64 bits).

Third Party Libraries
---------------------
The software uses the following third party libraries:

* wxWidgets
* Boost C++ library
* libjpeg-turbo library
* Adrian Secord's JPEGWriter C++ code
* Visual Leak Detector library
* SQLite
* Ramon de Klein's Serial Library for C++
* Allied Vision Technologies SDK
 
JPEGWriter and SQLite are provided in the vendor directory. The rest need to be downloaded and installed separately.

Installation
------------
1) Define the following PATH environments: WX (wxWidgets), BOOST (Boost C++ libraries), AVT (Allied Vision Technologies SDK), VLD (Visual Leak Detector), JPEG_TURBO (libjpeg-turbo library) and SERIAL (serial library). 

2) Open rics.vcproj (this is in MS Visual 2008 format) and compile. Note that if you have a later version of MS Visual Studio (or MS Visual C++ Express), vcproj will be converted automatically.
