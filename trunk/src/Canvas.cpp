/*
Author: Nariman Habili

Description: Creates the image, gps and notepad panels. Controls
             camera play and stop.

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

#include "Canvas.h"
#include <wx/statline.h>
#include <wx/mstream.h>
#include <boost/lexical_cast.hpp>
#include <windows.h>

namespace rics
{

    Canvas::Canvas(wxWindow *parent, 
				   wxWindowID id, 
				   const wxPoint &pos, 
				   const wxSize &size, 
				   Cameras* cameras, 
				   GPS* gps, 
				   Session* session,
				   Database* db,
				   NotePad* np):
    wxPanel(parent, id, pos, size),
    cameras_(cameras),
    gps_(gps),
    session_(session),
    numCameras_((*cameras_).size()),
    cameraBuffers_(boost::shared_array<SharedImageBufferPtr>(new SharedImageBufferPtr[numCameras_])),
    cameraThreads_(boost::shared_array<CameraThread*>(new CameraThread*[numCameras_])),
    gpsThread_(boost::shared_ptr<GPSThread*>(new GPSThread*)),
    play_(false),
    createDB_(false),
    firstTime_(true),
	latSexagesimal_("--"),
    lonSexagesimal_("--"),
    bear_("--"),
	lat_("--"),
    lon_("--"),
    timeStamp_("--"),
    speed_("--"),
    satellites_("--"),
    altitude_("--"),
    quality_("--"),
	db_(db),
	np_(np)
    {
        wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

		//Assigning camera names for the display panel based on their unique ID.
		wxString cameraName[4] = {"Camera: None", "Camera: None", "Camera: None", "Camera: None"};
		for (size_t i = 0; i < numCameras_; ++i)
		{
			cameraName[i] = "Camera: " + 
                            boost::lexical_cast<std::string>((*cameras_)[i].uniqueID()) + 
                            " (" + (*cameras_)[i].cameraName() + ")";
		}
        
        //Panel where images are painted
        wxGridSizer* panelSizer = new wxGridSizer(2, 2, 0, 0); 
		panel1_ = new ImagePanel(this, wxID_ANY, cameraName[0], wxDefaultPosition, wxSize(204, 170));
        panel2_ = new ImagePanel(this, wxID_ANY, cameraName[1], wxDefaultPosition, wxSize(204, 170));
        panel3_ = new ImagePanel(this, wxID_ANY, cameraName[2], wxDefaultPosition, wxSize(204, 170));
		panel4_ = new ImagePanel(this, wxID_ANY, cameraName[3], wxDefaultPosition, wxSize(204, 170));		
		
		panels_.push_back(panel1_);
        panels_.push_back(panel2_);
        panels_.push_back(panel3_);
		panels_.push_back(panel4_);
        panelSizer->Add(panel1_, 0, wxFIXED_MINSIZE  | wxRIGHT | wxBOTTOM, 1);
        panelSizer->Add(panel2_, 0, wxFIXED_MINSIZE);
        panelSizer->Add(panel3_, 0, wxFIXED_MINSIZE);
		panelSizer->Add(panel4_, 0, wxFIXED_MINSIZE);

        //gps data display
        wxStaticText* gpsText = new wxStaticText(this, wxID_ANY, wxT("Current GPS Data"), wxDefaultPosition, wxDefaultSize, 0);
        gpsText->SetFont(wxFont(7, 74, 90, 90, false, wxT("Arial")));
        wxBoxSizer* gpsBoxSizer = new wxBoxSizer(wxVERTICAL);
        
	    wxPanel* mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxTAB_TRAVERSAL );
	    mainPanel->SetBackgroundColour(wxNullColour);
        wxBoxSizer* mainBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    	
	    wxGridSizer* leftGridSizer = new wxGridSizer(2, 2, 0, 0);
    	
	    wxStaticText* timeStampText = new wxStaticText(mainPanel, wxID_ANY, wxT("Time"), wxDefaultPosition, wxDefaultSize, 0);
        timeStampText->SetFont(wxFont(7, 74, 90, 92, false, wxT("Arial")));
        timeStampValue_ = new wxStaticText(mainPanel, wxID_STATIC, wxT("00:00:00"), wxDefaultPosition, wxDefaultSize, 0);
	    leftGridSizer->Add(timeStampText, 0, wxALL, 0);
        leftGridSizer->Add(timeStampValue_, 0, wxALIGN_LEFT | wxALL, 0);

        wxStaticText* latitudeText = new wxStaticText(mainPanel, wxID_STATIC, wxT("Latitude"), wxDefaultPosition, wxDefaultSize, 0);
        latitudeText->SetFont(wxFont(7, 74, 90, 92, false, wxT("Arial")));
        latitudeValue_ = new wxStaticText(mainPanel, wxID_STATIC, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
        leftGridSizer->Add(latitudeText, 0, wxALL, 0);
        leftGridSizer->Add(latitudeValue_, 0, wxALIGN_LEFT | wxALL, 0);

        wxStaticText* longitudeText = new wxStaticText(mainPanel, wxID_STATIC, wxT("Longitude"), wxDefaultPosition, wxDefaultSize, 0);
        longitudeText->SetFont(wxFont(7, 74, 90, 92, false, wxT("Arial")));
        longitudeValue_ = new wxStaticText(mainPanel, wxID_STATIC, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
        leftGridSizer->Add(longitudeText, 0, wxALL, 0);
        leftGridSizer->Add(longitudeValue_, 0, wxALIGN_LEFT | wxALL, 0);

		mainBoxSizer->Add(leftGridSizer, 1, wxEXPAND | wxLEFT, 0);
    	
	    wxBoxSizer* midBoxSizer = new wxBoxSizer(wxVERTICAL);
	    wxPanel* midPanel = new wxPanel(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize/*, wxDOUBLE_BORDER*/);
	    midPanel->SetBackgroundColour(wxColour(38, 111, 198));  	
	    wxBoxSizer* panelBoxSizer = new wxBoxSizer(wxVERTICAL);

        speedValue_ = new wxStaticText(midPanel, wxID_STATIC, wxT("--"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);        
        speedValue_->SetFont(wxFont(20, 70, 90, 90, false, wxT("Arial")));
        wxStaticText* kmhText = new wxStaticText(midPanel, wxID_ANY, wxT("km/h"), wxDefaultPosition, wxDefaultSize, 0);
        panelBoxSizer->Add(speedValue_, 0, wxALIGN_CENTER | wxTOP, 5);
        panelBoxSizer->Add(kmhText, 0, wxALIGN_CENTER);// | wxALL, 5 );

	    midPanel->SetSizer(panelBoxSizer);
	    midPanel->Layout();
	    panelBoxSizer->Fit(midPanel);
	    midBoxSizer->Add(midPanel, 1, wxEXPAND, 0);
    	
	    mainBoxSizer->Add(midBoxSizer, 1, wxEXPAND, 0);
    	
	    wxGridSizer* rightGridSizer = new wxGridSizer(2, 2, 0, 0);

	    wxStaticText* bearText = new wxStaticText(mainPanel, wxID_ANY, wxT("Bearing"), wxDefaultPosition, wxDefaultSize, 0);
        bearText->SetFont(wxFont(7, 74, 90, 92, false, wxT("Arial")));
        bearValue_ = new wxStaticText(mainPanel, wxID_STATIC, wxT("00:00:00"), wxDefaultPosition, wxDefaultSize, 0);
	    rightGridSizer->Add(bearText, 0, wxALL, 2);
        rightGridSizer->Add(bearValue_, 0, wxALIGN_LEFT | wxLEFT, 5);

	    wxStaticText* satText = new wxStaticText(mainPanel, wxID_ANY, wxT("Satellites"), wxDefaultPosition, wxDefaultSize, 0);
        satText->SetFont(wxFont(7, 74, 90, 92, false, wxT("Arial")));
        satValue_ = new wxStaticText(mainPanel, wxID_STATIC, wxT("00:00:00"), wxDefaultPosition, wxDefaultSize, 0);
	    rightGridSizer->Add(satText, 0, wxLEFT, 2);
        rightGridSizer->Add(satValue_, 0, wxALIGN_LEFT | wxLEFT, 5);

	    wxStaticText* qualityText = new wxStaticText(mainPanel, wxID_ANY, wxT("Fix Quality"), wxDefaultPosition, wxDefaultSize, 0);
        qualityText->SetFont(wxFont(7, 74, 90, 92, false, wxT("Arial")));
        qualityValue_ = new wxStaticText(mainPanel, wxID_STATIC, wxT("00:00:00"), wxDefaultPosition, wxDefaultSize, 0);
	    rightGridSizer->Add(qualityText, 0, wxLEFT, 2);
        rightGridSizer->Add(qualityValue_, 0, wxALIGN_LEFT | wxLEFT, 5);
    	
	    mainBoxSizer->Add(rightGridSizer, 1, wxEXPAND | wxLEFT , 0);
    	
	    mainPanel->SetSizer(mainBoxSizer);
	    mainPanel->Layout();
	    mainBoxSizer->Fit(mainPanel); 

        gpsBoxSizer->Add(gpsText, 0, wxALIGN_CENTER | wxTOP, 5);
        gpsBoxSizer->Add(mainPanel, 0, wxEXPAND | wxTOP, 2);
		gpsBoxSizer->SetMinSize(400, 85);
        
    	wxStaticLine* staticLine1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
       	wxStaticLine* staticLine2 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);

        //text Note Pad
        wxStaticText* npText = new wxStaticText(this, wxID_ANY, wxT("Note Pad"), wxDefaultPosition, wxDefaultSize, 0);
        npText->SetFont(wxFont(8, 74, 90, 90, false, wxT("Arial")));
        wxPanel* textPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxTAB_TRAVERSAL);
		wxBoxSizer* textPanelSizer = new wxBoxSizer(wxVERTICAL);
		np_->create(textPanel, 
                    ID_NotePad, 
                    session_, 
                    &formatTime_, 
                    &lat_, 
                    &lon_,
					&bear_,
                    wxEmptyString, 
                    wxDefaultPosition, 
					wxSize(400, 102),
				    wxTE_MULTILINE | wxTE_PROCESS_ENTER | wxNO_BORDER | wxHSCROLL);
        textPanelSizer->Add(np_, 0, wxEXPAND, 5);
        textPanel->SetSizer(textPanelSizer);
        
        topSizer->Add(panelSizer, 0, wxTOP, 0);
        topSizer->Add(staticLine2, 0, wxEXPAND | wxTOP, 5);
        topSizer->Add(gpsBoxSizer, 0, wxTOP, 5);
        topSizer->Add(npText, 0, wxALIGN_CENTER | wxTOP, 5);
        topSizer->Add(textPanel, 0, wxTOP, 5);
        
        SetSizer(topSizer);
        topSizer->Fit(this);
        topSizer->SetSizeHints(this);

        for (size_t i = 0; i < numCameras_; ++i)
        {
            cameraBuffers_[i] = SharedImageBufferPtr(new SharedImageBuffer());
        }

        gpsData_ = SharedGPSDataPtr(new SharedGPSData());

	    //GPS thread creation and Run.
		GPSThread* gpsThread = new GPSThread(this, gps_, gpsData_);
        wxThreadError threadError = gpsThread->Create();
        assert(threadError == wxTHREAD_NO_ERROR);
        *gpsThread_ = gpsThread;
		(*gpsThread_)->Run();
    }

    Canvas::~Canvas()
    {
    }

    //Delete all camera threads.
    inline void Canvas::deleteCameraThreads()
    {
        if (play_)
        {
            for (size_t i = 0; i < numCameras_; ++i)
            {
                cameraThreads_[i]->Delete();
            }
        }
    }

    //Delete the GPS thread.
	inline void Canvas::deleteGPSThread()
	{
		(*gpsThread_)->Delete();
	}

	void Canvas::deleteAllThreads()
	{
		deleteCameraThreads();
		deleteGPSThread();
	}

    //Refresh GUI image from streamed cameras and write GPS data to database
    void Canvas::onCameraEvent(wxCommandEvent& event)
    {
        //write GPS data to database
        if (session_->createDB())
        {
            wxString cameraName = (*cameras_)[event.GetInt()].cameraName();
            wxString cameraID = boost::lexical_cast<std::string>((*cameras_)[event.GetInt()].uniqueID());
			db_->databaseEnterData(cameraName, 
                                   event.GetExtraLong(),
                                   timeStamp_, 
                                   lat_, 
                                   lon_, 
                                   speed_,
                                   bear_,
                                   satellites_,
                                   quality_,
                                   cameraID);
        }

        cameraBuffers_[event.GetInt()]->readLock();
        UCArray bmpBuffer = cameraBuffers_[event.GetInt()]->data();
        wxImage image(204, 170, (unsigned char*)bmpBuffer.get(), true);
		panels_[event.GetInt()]->refreshImage(wxBitmap(image));
        cameraBuffers_[event.GetInt()]->readUnlock();
    }

    //Refresh GPS data on GUI
    void Canvas::onGPSEvent(wxCommandEvent& WXUNUSED(event))
    {
        gpsData_->readLock();

        lat_ = gpsData_->latitude();
        latSexagesimal_ = gpsData_->latSexagesimal();
		lon_ = gpsData_->longitude();
		lonSexagesimal_ = gpsData_->lonSexagesimal();
        bear_ = gpsData_->bearing();
        timeStamp_ = gpsData_->timeStamp();
		formatTime_ = gpsData_->formatTime();
		speed_ = gpsData_->speed();
        satellites_ = gpsData_->satellites();
        quality_ = gpsData_->quality();
        
        latitudeValue_->SetLabel(latSexagesimal_);
        longitudeValue_->SetLabel(lonSexagesimal_);
        bearValue_->SetLabel(gpsData_->formatBear());
        timeStampValue_->SetLabel(formatTime_);
        speedValue_->SetLabel(speed_);
        satValue_->SetLabel(satellites_);
        qualityValue_->SetLabel(quality_);

        gpsData_->readUnlock();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////    
    ////////Buttons appearing on canvas
    
    //Camera threads are created and images are streamed
    bool Canvas::play()
    {        
        if (!play_)
        {
		    if (!gps_->gpsActive() && session_->createDB())
		    {
			    wxMessageDialog(this, "No GPS data detected. Check COM port setting", "RICS GPS Error", wxOK | wxICON_ERROR)
			    .ShowModal() ;
			    return false;
		    }
    		
		    if (session_->sessionNameIsEmpty() && (session_->saveImages() || session_->createDB()))//allows preview also
            {
                wxMessageDialog(this, "Your session requires a unique name. Please create a new session with a unique name.",
                "RICS", wxICON_EXCLAMATION)
			    .ShowModal();
                return false;
            }
            
            for (size_t i = 0; i < numCameras_; ++i)
            {
                CameraThread* cameraThread = new CameraThread(this, &((*cameras_)[i]), cameraBuffers_[i], (int)i, session_);
                wxThreadError threadError = cameraThread->Create();
                assert(threadError == wxTHREAD_NO_ERROR);
                cameraThreads_[i] = cameraThread;
                (*cameras_)[i].startStream();
                cameraThreads_[i]->Run();
            }

            play_ = true;

            return true;
        } 

        return false;
    }

    //Camera threads are deleted and images are refreshed blank
    bool Canvas::stop()
    {
        if (play_)
        {
            deleteCameraThreads();
            
            panel1_->refreshImage(wxBitmap(204, 170, -1));
            panel2_->refreshImage(wxBitmap(204, 170, -1));
            panel3_->refreshImage(wxBitmap(204, 170, -1));
            panel4_->refreshImage(wxBitmap(204, 170, -1));

            for (size_t i = 0; i < numCameras_; ++i)
            {
                (*cameras_)[i].stopStream();
            }
            
            play_ = false;

            return true;
        }

        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////    
    ////////Camera Names
    void Canvas::setCameraNames()
    {
		for (size_t i = 0; i < numCameras_; ++i)
		{
			panels_[i]->setCameraName("Camera: " + 
                                      boost::lexical_cast<std::string>((*cameras_)[i].uniqueID()) + 
                                      " (" + (*cameras_)[i].cameraName() + ")");
		}        
    }

   BEGIN_EVENT_TABLE(Canvas, wxPanel)
        EVT_MENU(CameraThread::CAMERA_EVENT, Canvas::onCameraEvent)
        EVT_MENU(GPSThread::GPS_EVENT, Canvas::onGPSEvent )
   END_EVENT_TABLE()

} //namespace