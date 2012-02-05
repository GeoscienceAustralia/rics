/*
Author: Nariman Habili

Description: Dialog to control GPS settings.

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

#include "GPSPropDialog.h"
#include <boost/lexical_cast.hpp>

namespace rics 
{

    GPSPropDialog::GPSPropDialog(wxWindow *parent, const wxString &title, GPS* gps):
    wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    gps_(gps)
    {   
        wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

        panel_ = new wxPanel(this, wxID_ANY);
        createGPSSettingsPage();
        topSizer->Add(panel_, 1, wxEXPAND);

        //Button
        wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
        wxButton *okButton = new wxButton(this, ID_OK, _T("OK"));
        buttonSizer->Add(okButton, 0, wxALIGN_LEFT | wxALL, 10 );
        topSizer->Add(buttonSizer, 0, wxALIGN_RIGHT);

        SetSizer(topSizer);
        topSizer->Fit(this);
        topSizer->SetSizeHints(this);
    }

    void GPSPropDialog::createGPSSettingsPage()
    {
        wxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);

        //COM port select
        wxStaticBox* portSelect = new wxStaticBox(panel_, wxID_STATIC, wxT("COM Port Setting"));
        wxStaticBoxSizer* portSelectSizer = new wxStaticBoxSizer(portSelect, wxHORIZONTAL);
        portSelectSizer->SetMinSize(200, 0);

        wxArrayString values;
        
        for (size_t i = 1; i < 65; ++i)
        {
            values.Add(boost::lexical_cast<std::string>(i));
        }
        
        wxStaticText* text = new wxStaticText(panel_, wxID_ANY, "Port", wxDefaultPosition, wxDefaultSize);
        
        port_ = new wxComboBox(panel_, 
                               ID_ComboBoxPort, 
                               "1", 
                               wxDefaultPosition, 
                               wxDefaultSize, 
                               values, 
                               wxCB_READONLY);
        
        wxButton* searchButton = new wxButton(panel_, ID_Button, "Search Ports", wxDefaultPosition, wxDefaultSize, 0);
        
        portSelectSizer->Add(text, 0, wxALIGN_LEFT | wxTOP, 13);
        portSelectSizer->Add(port_, 0, wxALIGN_LEFT | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, 10);
        portSelectSizer->Add(searchButton, 0, wxALIGN_LEFT | wxLEFT | wxRIGHT | wxTOP, 10);

        //Add to top level
        panelSizer->Add(portSelectSizer,
                        0,
                        wxALIGN_CENTER_HORIZONTAL|wxALL, 
                        5);

        panel_->SetSizer(panelSizer);
    }
    
    //////OK/Close Buttons
    void GPSPropDialog::onOK(wxCommandEvent& WXUNUSED(event))
    {
        EndModal(GetReturnCode());
    }

    void GPSPropDialog::onClose(wxCloseEvent& WXUNUSED(event))
    {    
        EndModal(GetReturnCode());
    }
    
    void GPSPropDialog::onComboBoxPort(wxCommandEvent& WXUNUSED(event))
    {
        selectPort_ = port_->GetCurrentSelection();

        gps_->close();
        bool code = gps_->openPort(selectPort_ + 1);

        if (code)
        {
            gps_->setGPSActive(true);
            wxString s = "COM" + boost::lexical_cast<std::string>(selectPort_ + 1);
            wxMessageDialog(panel_, "GPS device found on " + s, "Open COM Port", wxOK)
            .ShowModal();
            EndModal(GetReturnCode());
        }
        else
        {
            gps_->setGPSActive(false);
            wxString s = "COM" + boost::lexical_cast<std::string>(selectPort_ + 1);
            wxMessageDialog(panel_, "No GPS device detected on " + s, "Open COM Port", wxOK | wxICON_ERROR)
            .ShowModal();
        }
    }

    void GPSPropDialog::onSearchPorts(wxCommandEvent& WXUNUSED(event))
    {
        for (int i = 1; i < 65; ++i)
        {
            port_->SetValue(boost::lexical_cast<std::string>(i));
            gps_->close();
            int code = gps_->openPort(i);
            Sleep(250);

            if (code)
            {   
                gps_->setGPSActive(true);
                wxString s = "COM" + boost::lexical_cast<std::string>(i);
                wxMessageDialog(panel_, "GPS device found on " + s, "Search COM Ports", wxOK)
                .ShowModal();
                EndModal(GetReturnCode());
                return;
            }
        }  

        gps_->setGPSActive(false);
        wxMessageDialog(panel_, "No GPS device detected", "Search COM Ports", wxOK | wxICON_ERROR)
        .ShowModal();
    }

    void GPSPropDialog::searchPorts()
    {
        onSearchPorts(wxCommandEvent());
    }
    
   BEGIN_EVENT_TABLE(GPSPropDialog, wxDialog)
        EVT_BUTTON(ID_OK, GPSPropDialog::onOK)
        EVT_COMBOBOX(ID_ComboBoxPort, GPSPropDialog::onComboBoxPort)
        EVT_CLOSE(GPSPropDialog::onClose)
        EVT_BUTTON(ID_Button, GPSPropDialog::onSearchPorts)
   END_EVENT_TABLE()


} //namespace rics