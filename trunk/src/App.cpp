/*
Author: Nariman Habili

Description: Main entry point for RICS. The cameras are initialised here.

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

#include "App.h"
#include "Camera.h"
#include <cassert>
#include <vector>

namespace rics
{
    static bool closed = false;
    
    void  __stdcall CameraEventCB(void* Context,
                                  tPvInterface Interface,
                                  tPvLinkEvent Event,
                                  unsigned long UniqueId);

    
    //For sorting the cameras based on their unique IDs
    bool uniqueIdSort(tPvCameraInfoEx& camInfo0, tPvCameraInfoEx& camInfo1)
    {
        return camInfo0.UniqueId < camInfo1.UniqueId;
    }
    
    
    App::App():
    numCams_(0),
    name_(wxString::Format(wxT("MyApp-%s"), wxGetUserId().c_str())),
    instanceChecker_(boost::shared_ptr<wxSingleInstanceChecker>(new wxSingleInstanceChecker(name_)))
    {
    }

    App::~App()
    {
        //uninitialise the cameras
        for (unsigned long i = 0; i < numCams_; ++i)
        {
			tPvErr returnCode = PvCameraClose(hCamera_[i]);
            assert(!returnCode);
        }
        
        PvLinkCallbackUnRegister(CameraEventCB, ePvLinkRemove);
		PvUnInitialize();
    }

    bool App::OnInit()
    {
        //make sure only one instance of the process is running
        if (instanceChecker_->IsAnotherRunning())
        {
            wxLogError(_("Program already running, aborting."));
            return false;
        }
        
		//Initialise API
        if(!PvInitialize())
		{        
            PvLinkCallbackRegister(CameraEventCB, ePvLinkRemove, this);
			
            initCameraHandlersUniqueID();

			//Abort if no cameras attached
			if (!numCams_)
			{
				wxLogError(_("No cameras attached, aborting."));
				return false; 
			}

            if (numCams_ > 4)
			{
				wxLogError(_("More than 4 cameras attached, aborting."));
				return false; 
			}

            //Calculate the stream bytes per second per camera
			unsigned long streamBytesPerSecond = maxStreamBytesPerSecond_/numCams_;
	        
            //Initialise the cameras
			for (unsigned long i = 0; i < numCams_; ++i)
			{
				cameras_.push_back(Camera(hCamera_[i], streamBytesPerSecond));
			}

            wxInitAllImageHandlers();

			//wxSize frameSize(619, 953); 
			wxSize frameSize(420, 720);//This is the size of the GUI
            frame_ = new Frame(_T("RICS (Test Mode)"), 
                               wxDefaultPosition, 
                               frameSize, 
                               &cameras_);
			frame_->SetSizeHints(frameSize, frameSize);
            frame_->Layout();
            frame_->SetIcon(wxIcon("aaaa"));
			frame_->Show(TRUE);

			return TRUE;
		}
		else
		{
			wxLogError(_("Failed to initialise the API, aborting."));
			return false;
		}
    }

    void App::closeApp()
    {
        frame_->unpluggedClose();
    }

	//This method searches for cameras that are attached to the computer and opens those.
    //This is preferred as it does not rely on any prior knowledge (ie IP addresses)
    //of the cameras.
    void App::initCameraHandlersUniqueID()
    {        
        tPvCameraInfoEx cams0[10]; //10 elements allocated in case more than 4 cameras attached.
        unsigned long numCameras;
        Sleep(1000);//Needs to sleep here otherwise it won't pick up any cameras.
        numCameras = PvCameraListEx(cams0, 10, NULL, sizeof(tPvCameraInfoEx));
        
        if (!numCameras)
        {
            return;//No camera detected
        }

        if (numCameras > 4)
        {
            return; //Software designed to work with up to 4 cameras only!
        }
        
        std::vector<tPvCameraInfoEx> cams1(cams0, cams0 + numCameras);

        //Cameras are ordered based on their unique IDs.
        std::sort(cams1.begin(), cams1.end(), uniqueIdSort);
        
        tPvHandle handle;

        for (unsigned long i = 0; i < cams1.size(); ++i)
        {
            PvCameraOpen(cams1[i].UniqueId, ePvAccessMaster, &handle);
            hCamera_.push_back(handle);
			numCams_++;
        }
    }

    //This method opens the cameras using their IP address. The drawback of this
    //method is that the camera's IP addresses need to be set before use.
	void App::initCameraHandlersIP()
    {     
		//The IP address of the cameras must be set to 169.254.1.x where x is in the range 
		//2 - 5. GigEPIConfig should be used to set the IP addresses.
		tPvHandle handle;
		
		if (!PvCameraOpenByAddr(inet_addr("169.254.1.2"), ePvAccessMaster, &handle))//Unique ID = 53044
		{
			hCamera_.push_back(handle);
			numCams_++;
		}
		
		if (!PvCameraOpenByAddr(inet_addr("169.254.1.3"), ePvAccessMaster, &handle))//Unique ID = 53045/
		{
			hCamera_.push_back(handle);
			numCams_++;
		}
		
		if (!PvCameraOpenByAddr(inet_addr("169.254.1.4"), ePvAccessMaster, &handle))//Unique ID = 53046
		{
			hCamera_.push_back(handle);
			numCams_++;
		}

        if (!PvCameraOpenByAddr(inet_addr("169.254.1.5"), ePvAccessMaster, &handle))//Unique ID = 53086
		{
			hCamera_.push_back(handle);
			numCams_++;
		}
    }

    //This callback function is called when the camera is unplugged.
    //The application should exit cleanly
    void __stdcall CameraEventCB(void* context,
                                 tPvInterface Interface,
                                 tPvLinkEvent Event,
                                 unsigned long UniqueId)
    {
        Sleep(100);//Needs to Sleep otherwise it won't quit properly.
        if (!closed)
        {
            wxLogError(_("Camera(s) unplugged, aborting."));
            App* app = (App*)context;
            app->closeApp();
            closed = true;
        }
    }

} //namespace