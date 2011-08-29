/*
Author: Nariman Habili

Description: Dialog to control the properties of the notepad widget.

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

#include "NotePadSetDialog.h"
#include <boost/lexical_cast.hpp>
#include <wx/textfile.h>

namespace rics 
{

    NotePadSetDialog::NotePadSetDialog(wxWindow *parent, const wxString &title, NotePad* np):
    wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    np_(np)
    {   
        wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

        panel_ = new wxPanel(this, wxID_ANY);
        createNotePadSettingsPage();
        topSizer->Add(panel_, 1, wxEXPAND);

        //Button
        wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
        clearButton_ = new wxButton(this, ID_Clear, _T("Clear Text"));
        clearButton_->Disable();
        wxButton *okButton = new wxButton(this, ID_OK, _T("OK"));
        //wxButton *cancelButton = new wxButton(this, wxID_CANCEL,  _T("Cancel"));
        buttonSizer->Add(clearButton_, 0, wxALIGN_LEFT | wxLEFT | wxUP | wxBOTTOM, 10 );
        buttonSizer->Add(okButton, 0, wxALIGN_LEFT | wxALL, 10 );
        //buttonSizer->Add(cancelButton, 0, wxALIGN_RIGHT | wxALL, 10 );
        topSizer->Add(buttonSizer, 0, wxALIGN_RIGHT);

        SetSizer(topSizer);
        topSizer->Fit(this);
        topSizer->SetSizeHints(this);
    }

    void NotePadSetDialog::createNotePadSettingsPage()
    {
        wxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);

        wxStaticBox* notePadBox = new wxStaticBox(panel_, wxID_STATIC, wxT("Damage Level Presets"));
        wxStaticBoxSizer* notePadBoxSizer = new wxStaticBoxSizer(notePadBox, wxVERTICAL);
        notePadBoxSizer->SetMinSize(200, 0);

        checkBoxLatLonBear_ = new wxCheckBox(panel_, 
							                 ID_LatLon, 
                                             wxT("Add time, latitude, longitude and bearing at the end of each line"), 
                                             wxDefaultPosition, 
                                             wxDefaultSize, 
                                             0);
        checkBoxLatLonBear_->SetValue(true);
        notePadBoxSizer->Add(checkBoxLatLonBear_, 0, wxALL, 5);

        wxBoxSizer* damageLevelsSizer = new wxBoxSizer(wxHORIZONTAL);
        damageLevelsSizer->SetMinSize(wxSize(5, 5)); 
        checkBoxDamageLevels_ = new wxCheckBox(panel_, 
											   ID_DamageLevels, 
								               wxT("Use predefined damage levels. File locaton: "), 
                                               wxDefaultPosition, 
                                               wxDefaultSize,
                                               0);
        checkBoxDamageLevels_->SetValue(false);

        damageLevelsSizer->Add(checkBoxDamageLevels_, 0, wxALL, 5);
        damageLevelsFile_ = new wxFilePickerCtrl(panel_, ID_File, "C:\\", "Session Location");
        damageLevelsFile_->Disable();
        damageLevelsSizer->Add(damageLevelsFile_, 0, wxALL, 0);
        notePadBoxSizer->Add(damageLevelsSizer, 1, wxSHAPED, 5);

        wxBoxSizer* hotKeysSizer = new wxBoxSizer(wxVERTICAL);

        wxBoxSizer* F1HotKeySizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* F1 = new wxStaticText(panel_, wxID_ANY, wxT("F2:"), wxDefaultPosition, wxDefaultSize, 0);
        FText_[0] = new wxTextCtrl(panel_, ID_F1, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0);
        FText_[0]->Disable();
        F1HotKeySizer->Add(F1, 0, wxLEFT | wxTOP | wxRIGHT, 5);
        F1HotKeySizer->Add(FText_[0]);
        hotKeysSizer->Add(F1HotKeySizer, 1, wxALL | wxALIGN_RIGHT, 5);

        wxBoxSizer* F2HotKeySizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* F2 = new wxStaticText(panel_, wxID_ANY, wxT("F3:"), wxDefaultPosition, wxDefaultSize, 0);
        FText_[1] = new wxTextCtrl(panel_, ID_F2, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0);
        FText_[1]->Disable();
        F2HotKeySizer->Add(F2, 0, wxLEFT | wxTOP | wxRIGHT, 5);
        F2HotKeySizer->Add(FText_[1]);
        hotKeysSizer->Add(F2HotKeySizer, 1, wxALL | wxALIGN_RIGHT, 5);

        wxBoxSizer* F3HotKeySizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* F3 = new wxStaticText(panel_, wxID_ANY, wxT("F4:"), wxDefaultPosition, wxDefaultSize, 0);
        FText_[2] = new wxTextCtrl(panel_, ID_F3, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0);
        FText_[2]->Disable();
        F3HotKeySizer->Add(F3, 0, wxLEFT | wxTOP | wxRIGHT, 5);
        F3HotKeySizer->Add(FText_[2]);
        hotKeysSizer->Add(F3HotKeySizer, 1, wxALL | wxALIGN_RIGHT, 5);

        wxBoxSizer* F4HotKeySizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* F4 = new wxStaticText(panel_, wxID_ANY, wxT("F5:"), wxDefaultPosition, wxDefaultSize, 0);
        FText_[3] = new wxTextCtrl(panel_, ID_F4, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0);
        FText_[3]->Disable();
        F4HotKeySizer->Add(F4, 0, wxLEFT | wxTOP | wxRIGHT, 5);
        F4HotKeySizer->Add(FText_[3]);
        hotKeysSizer->Add(F4HotKeySizer, 1, wxALL | wxALIGN_RIGHT, 5);

        wxBoxSizer* F5HotKeySizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* F5 = new wxStaticText(panel_, wxID_ANY, wxT("F6:"), wxDefaultPosition, wxDefaultSize, 0);
        FText_[4] = new wxTextCtrl(panel_, ID_F5, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0);
        FText_[4]->Disable();
        F5HotKeySizer->Add(F5, 0, wxLEFT | wxTOP | wxRIGHT, 5);
        F5HotKeySizer->Add(FText_[4]);
        hotKeysSizer->Add(F5HotKeySizer, 1, wxALL | wxALIGN_RIGHT, 5);

        wxBoxSizer* F6HotKeySizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* F6 = new wxStaticText(panel_, wxID_ANY, wxT("F7:"), wxDefaultPosition, wxDefaultSize, 0);
        FText_[5] = new wxTextCtrl(panel_, ID_F6, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0);
        FText_[5]->Disable();
        F6HotKeySizer->Add(F6, 0, wxLEFT | wxTOP | wxRIGHT, 5);
        F6HotKeySizer->Add(FText_[5]);
        hotKeysSizer->Add(F6HotKeySizer, 1, wxALL | wxALIGN_RIGHT, 5);

        wxBoxSizer* F7HotKeySizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* F7 = new wxStaticText(panel_, wxID_ANY, wxT("F8:"), wxDefaultPosition, wxDefaultSize, 0);
        FText_[6] = new wxTextCtrl(panel_, ID_F7, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0);
        FText_[6]->Disable();
        F7HotKeySizer->Add(F7, 0, wxLEFT | wxTOP | wxRIGHT, 5);
        F7HotKeySizer->Add(FText_[6]);
        hotKeysSizer->Add(F7HotKeySizer, 1, wxALL | wxALIGN_RIGHT, 5);

        wxBoxSizer* F8HotKeySizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* F8 = new wxStaticText(panel_, wxID_ANY, wxT("F9:"), wxDefaultPosition, wxDefaultSize, 0);
        FText_[7] = new wxTextCtrl(panel_, ID_F8, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0);
        FText_[7]->Disable();
        F8HotKeySizer->Add(F8, 0, wxLEFT | wxTOP | wxRIGHT, 5);
        F8HotKeySizer->Add(FText_[7]);
        hotKeysSizer->Add(F8HotKeySizer, 1, wxALL | wxALIGN_RIGHT, 5);

        notePadBoxSizer->Add(hotKeysSizer, 0, wxLEFT, 20);
        
        //Add to top level
        panelSizer->Add(notePadBoxSizer,
                        0,
                        wxALIGN_CENTER_HORIZONTAL | wxALL, 
                        10);
        
        panel_->SetSizer(panelSizer);
    }
    
    void NotePadSetDialog::onCheckBoxLatLon(wxCommandEvent& WXUNUSED(event)) //Enter time/lat/lon info
    {
        np_->setDoWriteLatLonBearText(checkBoxLatLonBear_->GetValue());
    }

    void NotePadSetDialog::onCheckBoxDamageLevels(wxCommandEvent& WXUNUSED(event))
    {
        if (checkBoxDamageLevels_->GetValue())
        {
            clearButton_->Enable();
            damageLevelsFile_->Enable();
            
            for (int i = 0; i < 8; ++i)
            {
                FText_[i]->Enable();
            }
        }
        else
        {
            clearButton_->Disable();
            damageLevelsFile_->Disable();
            
            for (int i = 0; i < 8; ++i)
            {
                FText_[i]->Clear();
                FText_[i]->Disable();
            }
            damageLevelsFile_->SetPath("C:\\");
        }
    }

    void NotePadSetDialog::onFile(wxFileDirPickerEvent& event)
    {
        wxString fileName = event.GetPath().c_str();
        wxTextFile file(fileName);
        bool success = file.Open();

        if (!success)
        {
            wxMessageDialog(panel_, "File could not be opened", "Open File", wxOK)
			.ShowModal();
        }

        for (size_t i = 0; i < file.GetLineCount(); ++i)
        {
            if (i == 8)
            {
                break; //If number of lines in file > 8, break
            }

            FText_[i]->ChangeValue(file[i]);
        }

        file.Close();
    }

    //////Clear/OK/Close Buttons
    void NotePadSetDialog::onClear(wxCommandEvent& WXUNUSED(event))
    {
        for (int i = 0; i < 8; ++i)
        {
            FText_[i]->Clear();
            np_->setDamageLevel(FText_[i]->GetValue(), i);
        }

        damageLevelsFile_->SetPath("C:\\");
    }
    
    void NotePadSetDialog::onOK(wxCommandEvent& WXUNUSED(event))
    {
        bool changed = false;
        
        for (int i = 0; i < 8; ++i)
        {
            if (FText_[i]->IsModified())
            {
                changed = true;
            }

            np_->setDamageLevel(FText_[i]->GetValue(), i);
        }

        if (changed)
        {
            wxMessageDialog msgDialog(this, 
                                      "The damage levels have changed.\nDo you want to save the changes?",
                                      "RICS",
                                      wxNO_DEFAULT | wxYES_NO | wxCANCEL | wxICON_INFORMATION);

            switch (msgDialog.ShowModal())
            {
                case wxID_YES:
                    {
                        wxFileDialog fileDialog(this, "Save As", "", "*.txt", "*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
                        
                        if (fileDialog.ShowModal() == wxID_OK)
                        {   
                            wxTextFile txtFile(fileDialog.GetPath().ToAscii());
                            txtFile.Create();

                            for (size_t i = 0; i < 8; ++i)
                            {
                                txtFile.AddLine(FText_[i]->GetValue());
                                FText_[i]->DiscardEdits();
                            }
                            txtFile.Write();
                            damageLevelsFile_->SetPath(fileDialog.GetPath().ToAscii());
                            EndModal(GetReturnCode());
                        }
                    }    
                    break;
                case wxID_NO:
                    for (size_t i = 0; i < 8; ++i)
                    {
                        FText_[i]->DiscardEdits();
                    }
                    
                    EndModal(GetReturnCode());
                    break;
                case wxID_CANCEL:
                    break;
                default:
                    wxLogError("Unexpected wxMessageDialog return code!");
            }
        }
        else
        {
            EndModal(GetReturnCode());
        }
    }

    void NotePadSetDialog::onClose(wxCloseEvent& WXUNUSED(event))
    {     
        onOK(wxCommandEvent());
    }
    
   BEGIN_EVENT_TABLE(NotePadSetDialog, wxDialog)
       EVT_CHECKBOX(ID_LatLon, NotePadSetDialog::onCheckBoxLatLon) 
       EVT_CHECKBOX(ID_DamageLevels, NotePadSetDialog::onCheckBoxDamageLevels) 
       EVT_FILEPICKER_CHANGED(ID_File, NotePadSetDialog::onFile)
       EVT_BUTTON(ID_OK, NotePadSetDialog::onOK)
       EVT_CLOSE(NotePadSetDialog::onClose)
       EVT_BUTTON(ID_Clear, NotePadSetDialog::onClear)
   END_EVENT_TABLE()


} //namespace rics