/*
Author: Nariman Habili

Description: This opens a tabbed widget where the camera properties 
             are set.  

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

#include "CameraPropDialog.h"
#include <boost/lexical_cast.hpp>
#include "nav_car.xpm"

namespace rics
{

    CameraPropDialog::CameraPropDialog(wxWindow *parent, const wxString &title, Cameras* cameras):
    wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxSize(800,850), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    cameras_(cameras),
    numCameras_((*cameras_).size()),
    cameraSelectET_(false),
    cameraSelectGain_(false),
    cameraSelectWB_(false),
    selectComboBoxET_(0),
    selectComboBoxGain_(0),
    selectComboBoxWB_(0),
    exposureTimes_(std::vector<double>(numCameras_, 10.0)),
    autoMaxTimes_(std::vector<double>(numCameras_, 5.0)),
    autoET_(std::vector<bool>(numCameras_, false)),
    gains_(std::vector<int>(numCameras_, 0)),
    autoGain_(std::vector<bool>(numCameras_, false)),
    whiteBalanceRed_(std::vector<unsigned long>(numCameras_, 181)),
    whiteBalanceBlue_(std::vector<unsigned long>(numCameras_, 202)),
    autoWB_(std::vector<bool>(numCameras_, false)),
    frameRate_(4),
    packetSize_(6000/*8228*/),
    play_(false),
    notebook_(new wxNotebook(this, wxID_ANY, wxDefaultPosition))
    {
        wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

        //Notebook
        createCameraNamePage(notebook_);
        createExposureTimePage(notebook_);
        createGainPage(notebook_);
        createWhiteBalancePage(notebook_);
        createFrameRatePage(notebook_);
        createPacketSizePage(notebook_);
        topSizer->Add(notebook_, 1, wxEXPAND);

        //Button
        wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
        wxButton *okButton = new wxButton(this, ID_OK, _T("OK"));
        //wxButton *cancelButton = new wxButton(this, wxID_CANCEL,  _T("Cancel"));
        buttonSizer->Add(okButton, 0, wxALIGN_LEFT | wxALL, 10 );
        //buttonSizer->Add(cancelButton, 0, wxALIGN_RIGHT | wxALL, 10 );
        topSizer->Add(buttonSizer, 0, wxALIGN_RIGHT);

        SetSizer(topSizer);
        topSizer->Fit(this);
        topSizer->SetSizeHints(this);
    }

    void CameraPropDialog::onClose(wxCloseEvent& WXUNUSED(event))
    {
        textCtrlET_->ChangeValue(boost::lexical_cast<std::string>(exposureTime()));
        textCtrlWBRed_->ChangeValue(boost::lexical_cast<std::string>(whiteBalance("R")));
        textCtrlWBBlue_->ChangeValue(boost::lexical_cast<std::string>(whiteBalance("B")));
        textCtrlFR_->ChangeValue(boost::lexical_cast<std::string>(frameRate()));
        textCtrlPS_->ChangeValue(boost::lexical_cast<std::string>(packetSize()));
        
        EndModal(wxID_CANCEL);
    }

    //////OK Buttons
    void CameraPropDialog::onOK(wxCommandEvent& WXUNUSED(event))
    {
        //make sure camera names are not duplicates.
        if (!checkDuplicateCameraNames())
        {
            wxMessageDialog(this, "Duplicate camera names chosen. Please assign a unique camera name to each camera",
            "Duplicate Camera Name Error", wxICON_HAND)
            .ShowModal();
            return;
        }        

        bool val = true;
        
        if (!checkBoxAutoET_->IsChecked())
        {
            val = txtCtrlET(); 
        }

        if (!checkBoxAutoWB_->IsChecked())
        {
            val = txtCtrlWBRed() && val; //"&& val" to make sure number is entered in text box
            val = txtCtrlWBBlue() && val;
        } 

        if (!play())
        {
            val = txtCtrlPS() && val;
        }

        if (val) //only end if valid number(s) entered in text box(es)
        {
            EndModal(wxID_OK);
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////
    //////Camera Names
    void CameraPropDialog::createCameraNamePage(wxNotebook* notebook_)
    {
        wxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
        wxPanel *panel = new wxPanel(notebook_, wxID_ANY);
        notebook_->AddPage(panel, _T("Camera Name"));

        //New session options
        wxStaticBox* cameraNames = new wxStaticBox(panel, wxID_STATIC, wxT("Camera Names"));
        wxStaticBoxSizer* cameraNamesSizer = new wxStaticBoxSizer(cameraNames, wxHORIZONTAL);
        cameraNamesSizer->SetMinSize(330, 0);

        wxGridSizer* gSizer = new wxFlexGridSizer(2, 2, 0, 0);

        wxStaticText* cameraIDText = new wxStaticText(panel, wxID_ANY, wxT("Camera ID:"), wxDefaultPosition, wxDefaultSize, 0);
        gSizer->Add(cameraIDText, 0, wxTOP, 10);

        wxStaticText* cameraNameText = new wxStaticText(panel, wxID_ANY, wxT("Camera Name:"), wxDefaultPosition, wxDefaultSize, 0);
        gSizer->Add(cameraNameText, 0, wxTOP | wxLEFT, 10);

        wxArrayString cameras;
        for (size_t i = 0; i < numCameras_; ++i)

        {
            cameras.Add("Camera: " + boost::lexical_cast<std::string>((*cameras_)[i].uniqueID()));
        }
        
        cameraIDs_ = new wxListBox(panel,
                                   ID_Camera_IDs,
                                   wxDefaultPosition, 
                                   wxSize(-1, 90),  
                                   cameras,
                                   wxLB_SINGLE);
        cameraIDs_->SetFirstItem(0);

        
        wxArrayString positions;
        positions.Add("L");
        positions.Add("LF");
        positions.Add("F");
        positions.Add("RF");
        positions.Add("R");
        positions.Add("RB");
        positions.Add("B");
        positions.Add("LB");

        cameraPositionsComboBox_ = new wxComboBox(panel, 
                                                  ID_CameraPositions, 
                                                  positions[0], //start with east
                                                  wxDefaultPosition, 
                                                  wxSize(60, -1), 
                                                  positions, 
                                                  wxCB_READONLY); 

        gSizer->Add(cameraIDs_, 0, wxALL, 0);
        gSizer->Add(cameraPositionsComboBox_, 0, wxLEFT, 10);
        cameraNamesSizer->Add(gSizer, 0, wxLEFT | wxBOTTOM, 10);
        
        wxStaticBitmap* carNavigation = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(nav_car));
        cameraNamesSizer->Add(carNavigation, 0, wxALL, 5);

        panelSizer->Add(cameraNamesSizer, 
                        0,
                        wxALIGN_CENTER_HORIZONTAL|wxALL, 
                        5);

        panel->SetSizer(panelSizer);

        setDefaultCameraNames();
    }

    void CameraPropDialog::setDefaultCameraNames()
    {
        switch (numCameras_)
        {
            case 1:
                (*cameras_)[0].setCameraName("L");
                break;
            case 2:
                (*cameras_)[0].setCameraName("L");
                (*cameras_)[1].setCameraName("R");
                break;
            case 3:
                (*cameras_)[0].setCameraName("L");
                (*cameras_)[1].setCameraName("B");
                (*cameras_)[2].setCameraName("R");
                break;
            case 4:
                (*cameras_)[0].setCameraName("L");
                (*cameras_)[1].setCameraName("B");
                (*cameras_)[2].setCameraName("R");
                (*cameras_)[3].setCameraName("F");
                break;
        }
    }

    void CameraPropDialog::onCameraIDs(wxCommandEvent& WXUNUSED(event))
    {
        int selection = cameraIDs_->GetSelection();

            if (selection != wxNOT_FOUND)
            {
                cameraPositionsComboBox_->SetValue((*cameras_)[selection].cameraName());
            }
    }

    void CameraPropDialog::onCameraPositions(wxCommandEvent& WXUNUSED(event))
    {
        wxString position = cameraPositionsComboBox_->GetStringSelection();
        int selection = cameraIDs_->GetSelection();

        if (selection != wxNOT_FOUND)
        {
            (*cameras_)[selection].setCameraName(position);
        }
    }

    //check for duplicate camera names
    bool CameraPropDialog::checkDuplicateCameraNames()
    {
        for (size_t j = 0; j < numCameras_ - 1; ++j)
        {
            for (size_t i = j + 1; i < numCameras_; ++i)
            {
                if ((*cameras_)[j].cameraName() == (*cameras_)[i].cameraName())
                {
                    return false;
                }
            }
        }

        return true;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////
    //////Exposure Time
    void CameraPropDialog::createExposureTimePage(wxNotebook* notebook_)
    {
        wxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
        wxPanel *panel = new wxPanel(notebook_, wxID_ANY);
        notebook_->AddPage(panel, _T("Exposure"));

        //Camera select
        wxStaticBox* cameraSelect = new wxStaticBox(panel, wxID_STATIC, wxT("Camera Select"));
        wxStaticBoxSizer* cameraSelectSizer = new wxStaticBoxSizer(cameraSelect, wxHORIZONTAL);
        cameraSelectSizer->SetMinSize(300, 0);

        checkBoxCameraSelectET_ = new wxCheckBox(panel, ID_CheckBoxCameraSelectET, 
                                                 "Apply to camera:", wxDefaultPosition, wxDefaultSize, 0);
        
        wxArrayString strings;
        for (size_t i = 0; i < numCameras_; ++i)
        {
            strings.Add("Camera: " + boost::lexical_cast<std::string>((*cameras_)[i].uniqueID()));
        }
        
        cameraSelectChoiceET_ = new wxComboBox(panel, 
                                               ID_ComboBoxET, 
                                               strings[0], 
                                               wxDefaultPosition, 
                                               wxDefaultSize, 
                                               strings, 
                                               wxCB_READONLY | wxCB_SORT);
        
        cameraSelectChoiceET_->Disable();
        
        cameraSelectSizer->Add(checkBoxCameraSelectET_, 0, wxALIGN_CENTER | wxALL, 10);
        cameraSelectSizer->Add(cameraSelectChoiceET_, 0, wxALIGN_CENTER | wxALL, 10);
        
        //Exposure Time
        wxStaticBox* shutterSpeed = new wxStaticBox(panel, wxID_STATIC, wxT("Exposure Time (ms)"));                                   
        wxStaticBoxSizer* shutterSpeedSizer = new wxStaticBoxSizer(shutterSpeed, wxVERTICAL);
        shutterSpeedSizer->SetMinSize(300, 0);

        checkBoxAutoET_ = new wxCheckBox(panel, ID_CheckBoxAutoET, wxT("Auto"), wxDefaultPosition, wxDefaultSize, 0);
        checkBoxAutoET_->SetValue(true);

        //Shutter speed
        wxSizer *sliderSizer = new wxBoxSizer(wxHORIZONTAL);
        sliderET_ = new wxSlider(panel,
                                 ID_SliderET, 
                                 static_cast<double>(exposureTime()), 
                                 0, 
                                 100, 
                                 wxDefaultPosition,
                                 wxSize(200, -1), 
                                 wxSL_HORIZONTAL | wxSL_AUTOTICKS);
        sliderET_->SetTickFreq(10, 10);
        textCtrlET_ = new wxTextCtrl(panel, 
                                     ID_TxtCtrlET, //allows number to be typed in text box followed by Enter.
                                     boost::lexical_cast<std::string>(exposureTime()), 
                                     wxDefaultPosition, 
                                     wxSize(50, -1), 
                                     wxTE_PROCESS_ENTER);

        wxBoxSizer* textSizerExposure = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* staticTextExposure = new wxStaticText(panel, wxID_ANY, wxT("Exposure Time"), wxDefaultPosition, wxDefaultSize);
        textSizerExposure->Add(staticTextExposure, 0, wxLEFT, 5);

        sliderSizer->Add(textSizerExposure, 0, wxLEFT, 5);
        sliderSizer->Add(sliderET_, 0, wxLEFT, 5);
        sliderSizer->Add(textCtrlET_, 0, wxLEFT, 5);
        sliderET_->Disable();
        textCtrlET_->Disable();

        //Exposure Auto Max Time (AMT)
        wxSizer *sliderSizerAMT = new wxBoxSizer(wxHORIZONTAL);
        sliderAMT_ = new wxSlider(panel,
                                  ID_SliderAMT, 
                                  static_cast<double>(autoMaxTime()), 
                                  0, 
                                  100, 
                                  wxDefaultPosition,
                                  wxSize(200, -1), 
                                  wxSL_HORIZONTAL | wxSL_AUTOTICKS);
        sliderAMT_->SetTickFreq(10, 10);
        textCtrlAMT_ = new wxTextCtrl(panel, 
                                      ID_TxtCtrlAMT, //allows number to be typed in text box followed by Enter.
                                      boost::lexical_cast<std::string>(autoMaxTime()), 
                                      wxDefaultPosition, 
                                      wxSize(50, -1), 
                                      wxTE_PROCESS_ENTER);

        wxBoxSizer* textSizerAMT = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* staticTextAMT = new wxStaticText(panel, wxID_ANY, wxT("Auto Max Time"), wxDefaultPosition, wxDefaultSize);
        textSizerAMT->Add(staticTextAMT, 0, wxLEFT, 5);

        sliderSizerAMT->Add(textSizerAMT, 0, wxLEFT, 5);
        sliderSizerAMT->Add(sliderAMT_, 0, wxLEFT, 5);
        sliderSizerAMT->Add(textCtrlAMT_, 0, wxLEFT, 5);

        //text showing min and max values
        wxBoxSizer* textSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* staticText1 = new wxStaticText(panel, wxID_ANY, wxT("0.025"), wxDefaultPosition, wxDefaultSize);
        wxStaticText* staticText2 = new wxStaticText(panel, wxID_STATIC, wxT("100"), wxDefaultPosition, wxDefaultSize);
        textSizer->Add(staticText1, 0, wxLEFT, 85);
        textSizer->Add(staticText2, 0, wxLEFT, 190);
     
        shutterSpeedSizer->Add(checkBoxAutoET_, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 10);
        shutterSpeedSizer->Add(sliderSizer, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 5);
        shutterSpeedSizer->Add(sliderSizerAMT, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 5);
        shutterSpeedSizer->Add(textSizer, 0, wxALIGN_LEFT);

        //Add to top level
        panelSizer->Add(cameraSelectSizer, 
                        0,
                        wxALIGN_CENTRE_VERTICAL|wxALL, 
                        5);

        panelSizer->Add(shutterSpeedSizer, 
                        0,
                        wxALIGN_CENTRE_VERTICAL|wxALL, 
                        5);

        panel->SetSizer(panelSizer);
    }
    
    //Either apply to all cameras to select a camera
    void CameraPropDialog::onCheckBoxCameraSelectET(wxCommandEvent& WXUNUSED(event))
    {
        if (checkBoxCameraSelectET_->IsChecked())
        {
            cameraSelectET_ = true;
            cameraSelectChoiceET_->Enable(true);
        }
        else
        {
            cameraSelectET_ = false;
            cameraSelectChoiceET_->Enable(false);
            setExposureTime(exposureTime());
            setAutoET(autoET());
            adjustCameraExposureTime();
        }
    }

    //Choose a camera
    void CameraPropDialog::onComboBoxET(wxCommandEvent& WXUNUSED(event))
    {
        selectComboBoxET_ = cameraSelectChoiceET_->GetCurrentSelection();
        sliderET_->SetValue(static_cast<int>(exposureTime()));
        textCtrlET_->ChangeValue(boost::lexical_cast<std::string>(exposureTime()));     
        sliderAMT_->SetValue(static_cast<int>(autoMaxTime()));
        textCtrlAMT_->ChangeValue(boost::lexical_cast<std::string>(autoMaxTime()));
        checkBoxAutoET_->SetValue(autoET());

        if (autoET())
        {
            sliderET_->Disable();
            textCtrlET_->Disable();
            sliderAMT_->Enable();
            textCtrlAMT_->Enable();
        }
        else
        {
            sliderET_->Enable();
            textCtrlET_->Enable();
            sliderAMT_->Disable();
            textCtrlAMT_->Disable();
        }
    }
    
    //Choose an exposure value using slider
    void CameraPropDialog::onSliderET(wxCommandEvent& WXUNUSED(event))
    {
        int et = sliderET_->GetValue();
        
        if (!et)
        {
            setExposureTime(0.025); //minimum exposure time
        }
        else
        {
            setExposureTime(static_cast<double>(et));
        }
    
        adjustCameraExposureTime();
        
        textCtrlET_->ChangeValue(boost::lexical_cast<std::string>(exposureTime()));
    }

    //Choose exposure value using text box
    void CameraPropDialog::onTxtCtrlET(wxCommandEvent& WXUNUSED(event))
    {
        txtCtrlET();
    }

    bool CameraPropDialog::txtCtrlET()
    {
        double et;

        if (textCtrlET_->GetValue().ToDouble(&et))
        {
            if (et > 100.0)
            {
                et = 100.0;
                textCtrlET_->ChangeValue(boost::lexical_cast<std::string>(et));
            }
            else if (et < 0.025)
            {
                et = 0.025;
                textCtrlET_->ChangeValue(boost::lexical_cast<std::string>(et));
            }

            setExposureTime(et);
            sliderET_->SetValue(static_cast<int>(et));

            adjustCameraExposureTime();

            return true;
        }
        else
        {
            wxMessageDialog(NULL, "Not a number!", "Exposure Time Error", wxOK | wxICON_ERROR)
            .ShowModal();
            textCtrlET_->ChangeValue(boost::lexical_cast<std::string>(exposureTime()));

            return false;
        }
    }

    void CameraPropDialog::onSliderAMT(wxCommandEvent& WXUNUSED(event))
    {
        int amt = sliderAMT_->GetValue();
        
        if (!amt)
        {
            setAutoMaxTime(0.025); //minimum exposure time
        }
        else
        {
            setAutoMaxTime(static_cast<double>(amt));
        }
    
        adjustCameraAutoMaxTime();
        
        textCtrlAMT_->ChangeValue(boost::lexical_cast<std::string>(autoMaxTime()));
    }

    //Choose exposure value using text box
    void CameraPropDialog::onTxtCtrlAMT(wxCommandEvent& WXUNUSED(event))
    {
        txtCtrlAMT();
    }

    bool CameraPropDialog::txtCtrlAMT()
    {
        double amt;

        if (textCtrlAMT_->GetValue().ToDouble(&amt))
        {
            if (amt > 100.0)
            {
                amt = 100.0;
                textCtrlAMT_->ChangeValue(boost::lexical_cast<std::string>(amt));
            }
            else if (amt < 0.025)
            {
                amt = 0.025;
                textCtrlET_->ChangeValue(boost::lexical_cast<std::string>(amt));
            }

            setAutoMaxTime(amt);
            sliderAMT_->SetValue(static_cast<int>(amt));

            adjustCameraAutoMaxTime();

            return true;
        }
        else
        {
            wxMessageDialog(NULL, "Not a number!", "Auto Max Time Error", wxOK | wxICON_ERROR)
            .ShowModal();
            textCtrlAMT_->ChangeValue(boost::lexical_cast<std::string>(autoMaxTime()));

            return false;
        }
    }

    void CameraPropDialog::onCheckBoxAutoET(wxCommandEvent& WXUNUSED(event))
    {
        if (checkBoxAutoET_->IsChecked())
        {
            setAutoET(true);
            adjustCameraExposureTime();//shutter speed is set automatically by the camera.
            textCtrlET_->Enable(false);
            sliderET_->Enable(false);
            textCtrlAMT_->Enable(true);
            sliderAMT_->Enable(true);
        }
        else
        {
            setAutoET(false);
            adjustCameraExposureTime();
            textCtrlET_->Enable(true);
            sliderET_->Enable(true);
            textCtrlAMT_->Enable(false);
            sliderAMT_->Enable(false);
        }
    }

    //Get/Set methods ET
    inline void CameraPropDialog::setExposureTime(double value)
    {
        if (!cameraSelectET_)
        { 
            exposureTimes_.assign(numCameras_, value);
        }
        else
        {
            exposureTimes_[selectComboBoxET_] = value;
        }
    }

    inline double CameraPropDialog::exposureTime() const
    {   
        return exposureTimes_[selectComboBoxET_];
    }


    inline void CameraPropDialog::setAutoMaxTime(double value)
    {
        if (!cameraSelectET_)
        { 
            autoMaxTimes_.assign(numCameras_, value);
        }
        else
        {
            autoMaxTimes_[selectComboBoxET_] = value;
        }
    }

    inline double CameraPropDialog::autoMaxTime() const
    {   
        return autoMaxTimes_[selectComboBoxET_];
    }

    inline void CameraPropDialog::setAutoET(bool value)
    {
        if (!cameraSelectET_)
        {
            autoET_.assign(numCameras_, value);
        }
        else
        {
            autoET_[selectComboBoxET_] = value;
        }
    }

    inline bool CameraPropDialog::autoET() const
    {   
        return autoET_[selectComboBoxET_];
    }

    inline void CameraPropDialog::adjustCameraExposureTime()
    {
        if (!cameraSelectET_)
        {
            //exposure time is in micros, therefore multiply by 1000
            for (size_t i = 0; i < numCameras_; ++i)
            {
                (*cameras_)[i].setExposureTime(autoET(), exposureTime()*1000);
            }
        }
        else
        {
            (*cameras_)[selectComboBoxET_].setExposureTime(autoET(), exposureTime()*1000);
        }
    }

    inline void CameraPropDialog::adjustCameraAutoMaxTime()
    {
        if (!cameraSelectET_)
        {
            //exposure time is in micros, therefore multiply by 1000
            for (size_t i = 0; i < numCameras_; ++i)
            {
                (*cameras_)[i].setAutoMaxTime(autoMaxTime()*1000);
            }
        }
        else
        {
            (*cameras_)[selectComboBoxET_].setAutoMaxTime(autoMaxTime()*1000);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    //////Gain
    void CameraPropDialog::createGainPage(wxNotebook* notebook_)
    {
        wxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
        wxPanel *panel = new wxPanel(notebook_, wxID_ANY);
        notebook_->AddPage(panel, _T("Gain"));

        //Camera select
        wxStaticBox* cameraSelect = new wxStaticBox(panel, wxID_STATIC, wxT("Camera Select"));
        wxStaticBoxSizer* cameraSelectSizer = new wxStaticBoxSizer(cameraSelect, wxHORIZONTAL);
        cameraSelectSizer->SetMinSize(300, 0);

        checkBoxCameraSelectGain_ = new wxCheckBox(panel, ID_CheckBoxCameraSelectGain, 
                                                 "Apply to camera:", wxDefaultPosition, wxDefaultSize, 0);
        
        wxArrayString strings;
        for (size_t i = 0; i < numCameras_; ++i)
        {
            strings.Add("Camera: " + boost::lexical_cast<std::string>((*cameras_)[i].uniqueID()));
        }
        
        cameraSelectChoiceGain_ = new wxComboBox(panel, 
                                                 ID_ComboBoxGain, 
                                                 strings[0], 
                                                 wxDefaultPosition, 
                                                 wxDefaultSize, 
                                                 strings, 
                                                 wxCB_READONLY | wxCB_SORT);
        
        cameraSelectChoiceGain_->Disable();
        
        cameraSelectSizer->Add(checkBoxCameraSelectGain_, 0, wxALIGN_CENTER | wxALL, 10);
        cameraSelectSizer->Add(cameraSelectChoiceGain_, 0, wxALIGN_CENTER | wxALL, 10);
        
        //Gain
        wxStaticBox* gainBox = new wxStaticBox(panel, wxID_STATIC, wxT("Gain (dB)"));                                   
        wxStaticBoxSizer* gainSizer = new wxStaticBoxSizer(gainBox, wxVERTICAL);
        gainSizer->SetMinSize(300, 0);

        checkBoxAutoGain_ = new wxCheckBox(panel, ID_CheckBoxAutoGain, wxT("Auto"), wxDefaultPosition, wxDefaultSize, 0);
        checkBoxAutoGain_->SetValue(false);

        //slider and text box
        wxSizer *sliderSizer = new wxBoxSizer(wxHORIZONTAL);
        sliderGain_ = new wxSlider(panel,
                                   ID_SliderGain, 
                                   static_cast<double>(gain()), 
                                   0, 
                                   24, 
                                   wxDefaultPosition,
                                   wxSize(200, -1), 
                                   wxSL_HORIZONTAL | wxSL_AUTOTICKS);
        sliderGain_->SetTickFreq(1, 1);
        textCtrlGain_ = new wxTextCtrl(panel, 
                                       ID_TxtCtrlGain, //allows number to be typed in text box followed by Enter.
                                       boost::lexical_cast<std::string>(gain()), 
                                       wxDefaultPosition, 
                                       wxSize(50, -1), 
                                       wxTE_PROCESS_ENTER);
        sliderSizer->Add(sliderGain_, 0, wxLEFT, 5);
        sliderSizer->Add(textCtrlGain_, 0, wxLEFT, 5);

        //text showing min and max values
        wxBoxSizer* textSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* staticText1 = new wxStaticText(panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize);
        wxStaticText* staticText2 = new wxStaticText(panel, wxID_STATIC, wxT("24"), wxDefaultPosition, wxDefaultSize);
        textSizer->Add(staticText1, 0, wxLEFT, 25);
        textSizer->Add(staticText2, 0, wxLEFT, 180);
     
        gainSizer->Add(checkBoxAutoGain_, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 10);
        gainSizer->Add(sliderSizer, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 5);
        gainSizer->Add(textSizer, 0, wxALIGN_LEFT);
        
        //Add to top level
        panelSizer->Add(cameraSelectSizer, 
                        0,
                        wxALIGN_CENTER_HORIZONTAL|wxALL, 
                        5);

        panelSizer->Add(gainSizer, 
                        0,
                        wxALIGN_CENTER_HORIZONTAL|wxALL, 
                        5);

        panel->SetSizer(panelSizer);
    }
    
    void CameraPropDialog::onCheckBoxCameraSelectGain(wxCommandEvent& WXUNUSED(event))
    {
        if (checkBoxCameraSelectGain_->IsChecked())
        {
            cameraSelectGain_ = true;
            cameraSelectChoiceGain_->Enable(true);
        }
        else
        {
            cameraSelectGain_ = false;
            cameraSelectChoiceGain_->Enable(false);
            setGain(gain());
            setAutoGain(autoGain());
            adjustCameraGain();
        }
    }

    void CameraPropDialog::onComboBoxGain(wxCommandEvent& WXUNUSED(event))
    {
        selectComboBoxGain_ = cameraSelectChoiceGain_->GetCurrentSelection();
        sliderGain_->SetValue(static_cast<int>(gain()));
        textCtrlGain_->ChangeValue(boost::lexical_cast<std::string>(gain()));
        checkBoxAutoGain_->SetValue(autoGain());

        if (autoGain())
        {
            sliderGain_->Disable();
            textCtrlGain_->Disable();
        }
        else
        {
            sliderGain_->Enable();
            textCtrlGain_->Enable();
        }
    }
    
    void CameraPropDialog::onSliderGain(wxCommandEvent& WXUNUSED(event))
    {
        int g = sliderGain_->GetValue();
        setGain(static_cast<double>(g));
        adjustCameraGain();
        textCtrlGain_->ChangeValue(boost::lexical_cast<std::string>(gain()));
    }

    void CameraPropDialog::onTxtCtrlGain(wxCommandEvent& WXUNUSED(event))
    {
        txtCtrlGain();
    }

    bool CameraPropDialog::txtCtrlGain()
    {
        long g;

        if (textCtrlGain_->GetValue().ToLong(&g))
        {
            if (g > 24)
            {
                g = 24;
                textCtrlGain_->ChangeValue(boost::lexical_cast<std::string>(g));
            }
            else if (g < 0)
            {
                g = 0;
                textCtrlGain_->ChangeValue(boost::lexical_cast<std::string>(g));
            }

            setGain(g);
            sliderGain_->SetValue(static_cast<int>(g));

            adjustCameraGain();

            return true;
        }
        else
        {
            wxMessageDialog(NULL, "Not a number!", "Gain Error", wxOK | wxICON_ERROR)
            .ShowModal();
            textCtrlGain_->ChangeValue(boost::lexical_cast<std::string>(gain()));

            return false;
        }
    }

    void CameraPropDialog::onCheckBoxAutoGain(wxCommandEvent& WXUNUSED(event))
    {
        if (checkBoxAutoGain_->IsChecked())
        {
            setAutoGain(true);
            adjustCameraGain();//gain is set automatically by the camera.
            textCtrlGain_->Enable(false);
            sliderGain_->Enable(false);
        }
        else
        {
            setAutoGain(false);
            adjustCameraGain();
            textCtrlGain_->Enable(true);
            sliderGain_->Enable(true);
        }
    }

    //Get/Set methods Gain
    inline void CameraPropDialog::setGain(double value)
    {
        if (!cameraSelectGain_)
        { 
            gains_.assign(numCameras_, value);
        }
        else
        {
            gains_[selectComboBoxGain_] = value;
        }
    }

    inline double CameraPropDialog::gain() const
    {   
        return gains_[selectComboBoxGain_];
    }

    inline void CameraPropDialog::setAutoGain(bool value)
    {
        if (!cameraSelectGain_)
        {
            autoGain_.assign(numCameras_, value);
        }
        else
        {
            autoGain_[selectComboBoxGain_] = value;
        }
    }

    inline bool CameraPropDialog::autoGain() const
    {   
        return autoGain_[selectComboBoxGain_];
    }

    inline void CameraPropDialog::adjustCameraGain()
    {
        if (!cameraSelectGain_)
        {
            for (size_t i = 0; i < numCameras_; ++i)
            {
                (*cameras_)[i].setGain(autoGain(), gain());
            }
        }
        else
        {
            (*cameras_)[selectComboBoxGain_].setGain(autoGain(), gain());
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    //////White Balance
    void CameraPropDialog::createWhiteBalancePage(wxNotebook* notebook_)
    {
        wxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
        wxPanel *panel = new wxPanel(notebook_, wxID_ANY);
        notebook_->AddPage(panel, _T("White Balance"));

        //Camera select
        wxStaticBox* cameraSelect = new wxStaticBox(panel, wxID_STATIC, wxT("Camera Select"));
        wxStaticBoxSizer* cameraSelectSizer = new wxStaticBoxSizer(cameraSelect, wxHORIZONTAL);
        cameraSelectSizer->SetMinSize(300, 0);

        checkBoxCameraSelectWB_ = new wxCheckBox(panel, ID_CheckBoxCameraSelectWB, 
                                                 "Apply to camera:", wxDefaultPosition, wxDefaultSize, 0);

        wxArrayString strings;

        for (size_t i = 0; i < numCameras_; ++i)
        {
            strings.Add("Camera: " + boost::lexical_cast<std::string>((*cameras_)[i].uniqueID()));
        }
        
        cameraSelectChoiceWB_ = new wxComboBox(panel, 
                                               ID_ComboBoxWB, 
                                               strings[0], 
                                               wxDefaultPosition, 
                                               wxDefaultSize, 
                                               strings, 
                                               wxCB_READONLY | wxCB_SORT);
    
        cameraSelectChoiceWB_->Disable();
        
        cameraSelectSizer->Add(checkBoxCameraSelectWB_, 0, wxALIGN_CENTER | wxALL, 10);
        cameraSelectSizer->Add(cameraSelectChoiceWB_, 0, wxALIGN_CENTER | wxALL, 10);

        //White balance
        wxStaticBox* staticBox = new wxStaticBox(panel, wxID_STATIC, wxT("White Balance"));
        wxStaticBoxSizer* staticBoxSizer = new wxStaticBoxSizer(staticBox, wxVERTICAL);
        staticBoxSizer->SetMinSize(300, 0);

        checkBoxAutoWB_ = new wxCheckBox(panel, ID_CheckBoxWB, wxT("Auto"), wxDefaultPosition, wxDefaultSize, 0);

        //Red
        wxSizer *sliderSizerRed = new wxBoxSizer(wxHORIZONTAL);
        sliderWBRed_ = new wxSlider(panel, 
                                    ID_SliderWBRed, 
                                    whiteBalance("R"),
                                    80, //min value
                                    300, //max value
                                    wxDefaultPosition,
                                    wxSize(200, -1),
                                    wxSL_HORIZONTAL | wxSL_AUTOTICKS);
        sliderWBRed_->SetTickFreq(10, 10);
        textCtrlWBRed_ = new wxTextCtrl(panel, 
                                        ID_TxtCtrlWBRed, 
                                        boost::lexical_cast<std::string>(whiteBalance("R")), 
                                        wxDefaultPosition, 
                                        wxSize(50,-1), 
                                        wxTE_PROCESS_ENTER);
        
        wxBoxSizer* textSizerRedLabel = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* staticTextRedLabel = new wxStaticText(panel, wxID_ANY, wxT("Red"), wxDefaultPosition, wxDefaultSize);
        textSizerRedLabel->Add(staticTextRedLabel, 0, wxLEFT, 5);

        sliderSizerRed->Add(textSizerRedLabel, 0, wxLEFT, 5);
        sliderSizerRed->Add(sliderWBRed_, 0, wxLEFT, 5);
        sliderSizerRed->Add(textCtrlWBRed_, 0, wxLEFT, 5);

        //Blue
        wxSizer *sliderSizerBlue = new wxBoxSizer(wxHORIZONTAL);
        sliderWBBlue_ = new wxSlider(panel, 
                                    ID_SliderWBBlue, 
                                    whiteBalance("B"),
                                    80, 
                                    300, 
                                    wxDefaultPosition,
                                    wxSize(200, -1),
                                    wxSL_HORIZONTAL | wxSL_AUTOTICKS);
        sliderWBBlue_->SetTickFreq(10, 10);
        textCtrlWBBlue_ = new wxTextCtrl(panel, 
                                        ID_TxtCtrlWBBlue, 
                                        boost::lexical_cast<std::string>(whiteBalance("B")), 
                                        wxDefaultPosition, 
                                        wxSize(50,-1), 
                                        wxTE_PROCESS_ENTER);
        
        wxBoxSizer* textSizerBlueLabel = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* staticTextBlueLabel = new wxStaticText(panel, wxID_ANY, wxT("Blue"), wxDefaultPosition, wxDefaultSize);
        textSizerBlueLabel->Add(staticTextBlueLabel, 0, wxLEFT, 5);

        sliderSizerBlue->Add(textSizerBlueLabel, 0, wxLEFT, 5);
        sliderSizerBlue->Add(sliderWBBlue_, 0, wxLEFT, 5);
        sliderSizerBlue->Add(textCtrlWBBlue_, 0, wxLEFT, 5);

        //text showing min and max values
        wxBoxSizer* textSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* staticText1 = new wxStaticText(panel, wxID_ANY, wxT("80"), wxDefaultPosition, wxDefaultSize);
        wxStaticText* staticText2 = new wxStaticText(panel, wxID_STATIC, wxT("300"), wxDefaultPosition, wxDefaultSize);
        textSizer->Add(staticText1, 0, wxLEFT, 45);
        textSizer->Add(staticText2, 0, wxLEFT, 170);
 
        staticBoxSizer->Add(checkBoxAutoWB_, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 10);
        staticBoxSizer->Add(sliderSizerRed, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 5);
        staticBoxSizer->Add(sliderSizerBlue, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 5);
        staticBoxSizer->Add(textSizer, 0, wxALIGN_LEFT);

        //Add to top level
        panelSizer->Add(cameraSelectSizer, 
                        0,
                        wxALIGN_CENTER_HORIZONTAL|wxALL, 
                        5);

        panelSizer->Add(staticBoxSizer, 
                        0,
                        wxALIGN_CENTER_HORIZONTAL|wxALL, 
                        5);

        panel->SetSizer(panelSizer); 
    }

    
    void CameraPropDialog::onCheckBoxCameraSelectWB(wxCommandEvent& WXUNUSED(event))
    {
        if (checkBoxCameraSelectWB_->IsChecked())
        {
            cameraSelectWB_ = true;
            cameraSelectChoiceWB_->Enable(true);
        }
        else
        {
            cameraSelectWB_ = false;
            cameraSelectChoiceWB_->Enable(false);
            setWhiteBalance(whiteBalance("R"), "R");
            setWhiteBalance(whiteBalance("B"), "B");
            setAutoWB(autoWB());
            adjustCameraWB("R");
            adjustCameraWB("B");
        }
    }
    
    void CameraPropDialog::onComboBoxWB(wxCommandEvent& WXUNUSED(event))
    {
        selectComboBoxWB_ = cameraSelectChoiceWB_->GetCurrentSelection();
        sliderWBRed_->SetValue(whiteBalance("R"));
        textCtrlWBRed_->ChangeValue(boost::lexical_cast<std::string>(whiteBalance("R")));
        sliderWBBlue_->SetValue(whiteBalance("B"));
        textCtrlWBBlue_->ChangeValue(boost::lexical_cast<std::string>(whiteBalance("B")));
        checkBoxAutoWB_->SetValue(autoWB());

        if (autoWB())
        {
            sliderWBRed_->Disable();
            sliderWBBlue_->Disable();
            textCtrlWBRed_->Disable();
            textCtrlWBBlue_->Disable();
        }
        else
        {
            sliderWBRed_->Enable();
            sliderWBBlue_->Enable();
            textCtrlWBRed_->Enable();
            textCtrlWBBlue_->Enable();
        }
    }

    void CameraPropDialog::onSliderWBRed(wxCommandEvent& WXUNUSED(event))
    {
        setWhiteBalance(sliderWBRed_->GetValue(), "R");
        textCtrlWBRed_->ChangeValue(boost::lexical_cast<std::string>(whiteBalance("R")));
        adjustCameraWB("R"); 
    }

    void CameraPropDialog::onTxtCtrlWBRed(wxCommandEvent& WXUNUSED(event))
    {
        txtCtrlWBRed();
    }

    bool CameraPropDialog::txtCtrlWBRed()
    {
        double wb;

        if (textCtrlWBRed_->GetValue().ToDouble(&wb))
        {
            if (wb > 300) //white balance cannot be greater than 300
            {
                wb = 300;
                textCtrlWBRed_->ChangeValue(boost::lexical_cast<std::string>(wb));
            }
            else if (wb < 80) //white balance cannot be less than 80
            {
                wb = 80;
                textCtrlWBRed_->ChangeValue(boost::lexical_cast<std::string>(wb));
            }
            
            setWhiteBalance(wb, "R");
            sliderWBRed_->SetValue(whiteBalance("R"));
            adjustCameraWB("R");

            return true;
        }
        else
        {
            wxMessageDialog(notebook_, "Not a number!", "White Balance Error (Red)", wxOK | wxICON_ERROR)
            .ShowModal();
            textCtrlWBRed_->ChangeValue(boost::lexical_cast<std::string>(whiteBalance("R")));

            return false;
        }
    }

    void CameraPropDialog::onSliderWBBlue(wxCommandEvent& WXUNUSED(event))
    {
        setWhiteBalance(sliderWBBlue_->GetValue(), "B");
        textCtrlWBBlue_->ChangeValue(boost::lexical_cast<std::string>(whiteBalance("B")));
        adjustCameraWB("B");
    }

    void CameraPropDialog::onTxtCtrlWBBlue(wxCommandEvent& WXUNUSED(event))
    {
        txtCtrlWBBlue();
    }

    bool CameraPropDialog::txtCtrlWBBlue()
    {
        double wb;

        if (textCtrlWBBlue_->GetValue().ToDouble(&wb))
        {
            if (wb > 300)
            {
                wb = 300;
                textCtrlWBBlue_->ChangeValue(boost::lexical_cast<std::string>(wb));
            }
            else if (wb < 0)
            {
                wb = 0;
                textCtrlWBBlue_->ChangeValue(boost::lexical_cast<std::string>(wb));
            }
            
            setWhiteBalance(wb, "B");
            sliderWBBlue_->SetValue(whiteBalance("B"));
            adjustCameraWB("B");

            return true;
        }
        else
        {
            wxMessageDialog(notebook_, "Not a number!", "White Balance Error (Blue)", wxOK | wxICON_ERROR)
            .ShowModal();
            textCtrlWBBlue_->ChangeValue(boost::lexical_cast<std::string>(whiteBalance("B")));

            return false;
        }
    }

    void CameraPropDialog::onCheckBoxAutoWB(wxCommandEvent& WXUNUSED(event))
    {
        if (checkBoxAutoWB_->IsChecked())
        {
            setAutoWB(true);
            adjustCameraWB("R");//shutter speed is set automatically by the camera.
            textCtrlWBRed_->Enable(false);
            sliderWBRed_->Enable(false);
            textCtrlWBBlue_->Enable(false);
            sliderWBBlue_->Enable(false);
        }
        else
        {
            setAutoWB(false);
            adjustCameraWB("R");
            adjustCameraWB("B");
            textCtrlWBRed_->Enable(true);
            sliderWBRed_->Enable(true);
            textCtrlWBBlue_->Enable(true);
            sliderWBBlue_->Enable(true);
        }
    }

    //Get/Set methods WB
    inline void CameraPropDialog::setWhiteBalance(double value, char* colour)
    {
        if (!cameraSelectWB_)
        { 
            if (colour[0] == 'R')
            {
                whiteBalanceRed_.assign(numCameras_, value);
            }
            else
            {
                whiteBalanceBlue_.assign(numCameras_, value);
            }
        }
        else
        {
            if (colour[0] == 'R')
            {
                whiteBalanceRed_[selectComboBoxWB_] = value;
            }
            else
            {
                whiteBalanceBlue_[selectComboBoxWB_] = value;
            }
        }
    }

    inline double CameraPropDialog::whiteBalance(char* colour) 
    {   
        if (colour[0] == 'R')
        {
            return whiteBalanceRed_[selectComboBoxWB_];
        }
        else
        {
            return whiteBalanceBlue_[selectComboBoxWB_];
        }
    }

    inline void CameraPropDialog::setAutoWB(bool value)
    {
        if (!cameraSelectWB_)
        {
            autoWB_.assign(numCameras_, value);
        }
        else
        {
            autoWB_[selectComboBoxWB_] = value;
        }
    }

    inline bool CameraPropDialog::autoWB() const
    {   
        return autoWB_[selectComboBoxWB_];
    }

    inline void CameraPropDialog::adjustCameraWB(char* colour)
    {
        if (!cameraSelectWB_)
        {
            //exposure time is in micros, therefore multiply by 1000
            for (size_t i = 0; i < numCameras_; ++i)
            {
                (*cameras_)[i].setWhiteBalance(autoWB(), colour, whiteBalance(colour));
            }
        }
        else
        {
            (*cameras_)[selectComboBoxWB_].setWhiteBalance(autoWB(), colour, whiteBalance(colour));
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    ////Frame Rate
    void CameraPropDialog::createFrameRatePage(wxNotebook* notebook_)
    {
        wxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
        panelFR_ = new wxPanel(notebook_, wxID_ANY);
        notebook_->AddPage(panelFR_, _T("Frame Rate"));

        wxStaticBox* frameRateSize = new wxStaticBox(panelFR_, wxID_STATIC, wxT("Frame Rate (frames/sec)"));                                   
        wxStaticBoxSizer* frameRateSizer = new wxStaticBoxSizer(frameRateSize, wxVERTICAL);
        frameRateSizer->SetMinSize(300, 0);
        
        //slider and text box
        wxSizer *sliderSizer = new wxBoxSizer(wxHORIZONTAL);
        sliderFR_ = new wxSlider(panelFR_,
                                ID_SliderFR, 
                                static_cast<double>(this->frameRate()), 
                                1, 
                                10, 
                                wxDefaultPosition,
                                wxSize(200, -1), 
                                wxSL_HORIZONTAL | wxSL_AUTOTICKS);
        sliderFR_->SetTickFreq(1, 1);
        textCtrlFR_ = new wxTextCtrl(panelFR_, 
                                     ID_TxtCtrlFR, //allows number to be typed in text box followed by Enter.
                                     boost::lexical_cast<std::string>(this->frameRate()), 
                                     wxDefaultPosition,
                                     wxSize(50, -1), 
                                     wxTE_PROCESS_ENTER);
        sliderSizer->Add(sliderFR_, 0, wxLEFT, 5);
        sliderSizer->Add(textCtrlFR_, 0, wxLEFT, 5);

        //text showing min and max values
        wxBoxSizer* textSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* staticText1 = new wxStaticText(panelFR_, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize);
        wxStaticText* staticText2 = new wxStaticText(panelFR_, wxID_STATIC, wxT("10"), wxDefaultPosition, wxDefaultSize);
        textSizer->Add(staticText1, 0, wxLEFT, 20);
        textSizer->Add(staticText2, 0, wxLEFT, 170);
     
        frameRateSizer->Add(sliderSizer, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 5);
        frameRateSizer->Add(textSizer, 0, wxALIGN_LEFT);
        
        //Add to top level
        panelSizer->Add(frameRateSizer, 
                        0,
                        wxALIGN_CENTER_HORIZONTAL|wxALL, 
                        5);

        panelFR_->SetSizer(panelSizer);
    }

    void CameraPropDialog::onSliderFR(wxCommandEvent& WXUNUSED(event))
    {
        unsigned long fr = sliderFR_->GetValue();
        setFrameRate(fr);
        adjustCameraFR();
        textCtrlFR_->ChangeValue(boost::lexical_cast<std::string>(frameRate()));
    }

    void CameraPropDialog::onTxtCtrlFR(wxCommandEvent& WXUNUSED(event))
    { 
        txtCtrlFR();
    }

    bool CameraPropDialog::txtCtrlFR()
    {
        double fr;

        if (textCtrlFR_->GetValue().ToDouble(&fr))
        {
            if (fr > 10)
            {
                fr = 10;
                textCtrlFR_->ChangeValue(boost::lexical_cast<std::string>(fr));
            }
            else if (fr < 1)
            {
                fr = 2;
                textCtrlFR_->ChangeValue(boost::lexical_cast<std::string>(fr));
            }
            
            setFrameRate(static_cast<unsigned long>(fr));
            adjustCameraFR();
            sliderFR_->SetValue(fr);

            return true;
        }
        else
        {
            wxMessageDialog(notebook_, "Not a number!", "Frame Rate Error", wxOK | wxICON_ERROR)
            .ShowModal();
            textCtrlFR_->ChangeValue(boost::lexical_cast<std::string>(frameRate()));

            return false;
        }
    }

    //Get/Set frameRate size
    inline void CameraPropDialog::setFrameRate(unsigned long value)
    {
        frameRate_ = value;
    }

    inline unsigned long CameraPropDialog::frameRate() const
    {   
        return frameRate_;
    }

    inline void CameraPropDialog::adjustCameraFR()
    {
        float fr = static_cast<float>(frameRate());

        for (size_t i = 0; i < numCameras_; ++i)
        {
            if (fr > (*cameras_)[i].maxFrameRate())
            {
                fr = (*cameras_)[i].maxFrameRate();
            }
            (*cameras_)[i].setFrameRate(fr);
        }       
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    ////Packet Size
    void CameraPropDialog::createPacketSizePage(wxNotebook* notebook_)
    {
        wxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
        panelPS_ = new wxPanel(notebook_, wxID_ANY);
        notebook_->AddPage(panelPS_, _T("Packet Size"));

        wxStaticBox* packetSize = new wxStaticBox(panelPS_, wxID_STATIC, wxT("Packet Size (Bytes)"));                                   
        wxStaticBoxSizer* packetSizeSizer = new wxStaticBoxSizer(packetSize, wxVERTICAL);
        packetSizeSizer->SetMinSize(300, 0);
        
        //slider and text box
        wxSizer *sliderSizer = new wxBoxSizer(wxHORIZONTAL);
        sliderPS_ = new wxSlider(panelPS_,
                                ID_SliderPS, 
                                static_cast<double>(this->packetSize()), 
                                500, 
                                9014, 
                                wxDefaultPosition,
                                wxSize(200, -1), 
                                wxSL_HORIZONTAL | wxSL_AUTOTICKS);
        sliderPS_->SetTickFreq(500, 500);
        textCtrlPS_ = new wxTextCtrl(panelPS_, 
                                     ID_TxtCtrlPS, //allows number to be typed in text box followed by Enter.
                                     boost::lexical_cast<std::string>(this->packetSize()), 
                                     wxDefaultPosition,
                                     wxSize(50, -1), 
                                     wxTE_PROCESS_ENTER);
        sliderSizer->Add(sliderPS_, 0, wxLEFT, 5);
        sliderSizer->Add(textCtrlPS_, 0, wxLEFT, 5);

        //text showing min and max values
        wxBoxSizer* textSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* staticText1 = new wxStaticText(panelPS_, wxID_ANY, wxT("500"), wxDefaultPosition, wxDefaultSize);
        wxStaticText* staticText2 = new wxStaticText(panelPS_, wxID_STATIC, wxT("9014"), wxDefaultPosition, wxDefaultSize);
        textSizer->Add(staticText1, 0, wxLEFT, 15);
        textSizer->Add(staticText2, 0, wxLEFT, 160);
     
        packetSizeSizer->Add(sliderSizer, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 5);
        packetSizeSizer->Add(textSizer, 0, wxALIGN_LEFT);
        
        //Add to top level
        panelSizer->Add(packetSizeSizer, 
                        0,
                        wxALIGN_CENTER_HORIZONTAL|wxALL, 
                        5);

        panelPS_->SetSizer(panelSizer);
    }

    void CameraPropDialog::onSliderPS(wxCommandEvent& WXUNUSED(event))
    {
        unsigned long ps = sliderPS_->GetValue();
        setPacketSize(ps);
        adjustCameraPS();
        textCtrlPS_->ChangeValue(boost::lexical_cast<std::string>(packetSize()));
    }

    void CameraPropDialog::onTxtCtrlPS(wxCommandEvent& WXUNUSED(event))
    {
        txtCtrlPS();
    }

    bool CameraPropDialog::txtCtrlPS()
    {
        double ps;

        if (textCtrlPS_->GetValue().ToDouble(&ps))
        {
            if (ps > 9014)
            {
                ps = 9014;
                textCtrlPS_->ChangeValue(boost::lexical_cast<std::string>(ps));
            }
            else if (ps < 500)
            {
                ps = 500;
                textCtrlPS_->ChangeValue(boost::lexical_cast<std::string>(ps));
            }
            
            setPacketSize(static_cast<unsigned long>(ps));
            adjustCameraPS();
            sliderPS_->SetValue(ps);

            return true;
        }
        else
        {
            wxMessageDialog(notebook_, "Not a number!", "Packet Size Error", wxOK | wxICON_ERROR)
            .ShowModal();
            textCtrlPS_->ChangeValue(boost::lexical_cast<std::string>(packetSize()));

            return false;
        }
    }

    //Get/Set packet size
    inline void CameraPropDialog::setPacketSize(unsigned long value)
    {
        packetSize_ = value;
    }

    inline unsigned long CameraPropDialog::packetSize() const
    {   
        return packetSize_;
    }

    inline void CameraPropDialog::adjustCameraPS()
    {
        for (size_t i = 0; i < numCameras_; ++i)
        {
            (*cameras_)[i].adjustPacketSize(packetSize());
        }       
    }

    void CameraPropDialog::disablePanelPS()
    {
        panelPS_->Disable();
    }

    void CameraPropDialog::enablePanelPS()
    {
        panelPS_->Enable();
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    ////Play mode
    void CameraPropDialog::setPlay(bool play)
    {
        play_ = play;

        if (play_)
        {
            disablePanelPS();
        }
        else
        {
            enablePanelPS();
        }
    }

    bool CameraPropDialog::play() const
    {
        return play_;
    }

   BEGIN_EVENT_TABLE(CameraPropDialog, wxDialog)
        EVT_BUTTON(ID_OK, CameraPropDialog::onOK) 
       
        EVT_LISTBOX(ID_Camera_IDs, CameraPropDialog::onCameraIDs)
        EVT_COMBOBOX(ID_CameraPositions, CameraPropDialog::onCameraPositions)

        EVT_CHECKBOX(ID_CheckBoxCameraSelectET, CameraPropDialog::onCheckBoxCameraSelectET)
        EVT_COMBOBOX(ID_ComboBoxET, CameraPropDialog::onComboBoxET)
        EVT_SLIDER(ID_SliderET, CameraPropDialog::onSliderET)
        EVT_TEXT_ENTER(ID_TxtCtrlET, CameraPropDialog::onTxtCtrlET)
        EVT_SLIDER(ID_SliderAMT, CameraPropDialog::onSliderAMT)
        EVT_TEXT_ENTER(ID_TxtCtrlAMT, CameraPropDialog::onTxtCtrlAMT)
        EVT_CHECKBOX(ID_CheckBoxAutoET, CameraPropDialog::onCheckBoxAutoET)
        
        EVT_CHECKBOX(ID_CheckBoxCameraSelectGain, CameraPropDialog::onCheckBoxCameraSelectGain)
        EVT_COMBOBOX(ID_ComboBoxGain, CameraPropDialog::onComboBoxGain)
        EVT_SLIDER(ID_SliderGain, CameraPropDialog::onSliderGain)
        EVT_TEXT_ENTER(ID_TxtCtrlGain, CameraPropDialog::onTxtCtrlGain)
        EVT_CHECKBOX(ID_CheckBoxAutoGain, CameraPropDialog::onCheckBoxAutoGain)

        EVT_CHECKBOX(ID_CheckBoxCameraSelectWB, CameraPropDialog::onCheckBoxCameraSelectWB)
        EVT_COMBOBOX(ID_ComboBoxWB, CameraPropDialog::onComboBoxWB)
        EVT_SLIDER(ID_SliderWBRed, CameraPropDialog::onSliderWBRed)
        EVT_TEXT_ENTER(ID_TxtCtrlWBRed, CameraPropDialog::onTxtCtrlWBRed)
        EVT_SLIDER(ID_SliderWBBlue, CameraPropDialog::onSliderWBBlue)
        EVT_TEXT_ENTER(ID_TxtCtrlWBBlue, CameraPropDialog::onTxtCtrlWBBlue)
        EVT_CHECKBOX(ID_CheckBoxWB, CameraPropDialog::onCheckBoxAutoWB)

        EVT_SLIDER(ID_SliderFR, CameraPropDialog::onSliderFR)
        EVT_TEXT_ENTER(ID_TxtCtrlFR, CameraPropDialog::onTxtCtrlFR)

        EVT_SLIDER(ID_SliderPS, CameraPropDialog::onSliderPS)
        EVT_TEXT_ENTER(ID_TxtCtrlPS, CameraPropDialog::onTxtCtrlPS)
  
        EVT_CLOSE(CameraPropDialog::onClose)
   END_EVENT_TABLE()


} //namespace rics