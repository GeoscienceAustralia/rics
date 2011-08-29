/*
Author: Nariman Habili

Description: This opens a tabbed widget where the camera properties 
             are set.

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

#ifndef CAMERAPROPDIALOG_H
#define CAMERAPROPDIALOG_H

#include "Camera.h"
#include <wx/wx.h>
#include <wx/notebook.h>
#include <vector>

namespace rics
{
    class CameraPropDialog : public wxDialog
    {
    public:
        CameraPropDialog(wxWindow *parent, const wxString &title, Cameras* cameras);

        void createCameraNamePage(wxNotebook* notebook);
        void createExposureTimePage(wxNotebook* notebook);
        void createGainPage(wxNotebook* notebook);
        void createWhiteBalancePage(wxNotebook* notebook);
		void createFrameRatePage(wxNotebook* notebook_);
        void createPacketSizePage(wxNotebook* notebook_);

		void disablePanelPS();
		void enablePanelPS();
    
		void setPlay(bool play);

	private:
        void onOK(wxCommandEvent& WXUNUSED(event));
        
        void setDefaultCameraNames();
        void onCameraIDs(wxCommandEvent& WXUNUSED(event));
        void onCameraPositions(wxCommandEvent& WXUNUSED(event));
        bool checkDuplicateCameraNames();
        
        void onCheckBoxCameraSelectET(wxCommandEvent& WXUNUSED(event));
		void onComboBoxET(wxCommandEvent& WXUNUSED(event));
        void onSliderET(wxCommandEvent& WXUNUSED(event));
		void onSliderAMT(wxCommandEvent& WXUNUSED(event));
        void onTxtCtrlET(wxCommandEvent& WXUNUSED(event));
		bool txtCtrlET();
		void onTxtCtrlAMT(wxCommandEvent& WXUNUSED(event));
		bool txtCtrlAMT();
        void onCheckBoxAutoET(wxCommandEvent& WXUNUSED(event));

        void onCheckBoxCameraSelectGain(wxCommandEvent& WXUNUSED(event));
		void onComboBoxGain(wxCommandEvent& WXUNUSED(event));
        void onSliderGain(wxCommandEvent& WXUNUSED(event));
        void onTxtCtrlGain(wxCommandEvent& WXUNUSED(event));
		bool txtCtrlGain();
        void onCheckBoxAutoGain(wxCommandEvent& WXUNUSED(event));

        void onCheckBoxCameraSelectWB(wxCommandEvent& WXUNUSED(event));
		void onComboBoxWB(wxCommandEvent& WXUNUSED(event));
		void onSliderWBRed(wxCommandEvent& WXUNUSED(event));
        void onSliderWBBlue(wxCommandEvent& WXUNUSED(event));
		void onTxtCtrlWBRed(wxCommandEvent& WXUNUSED(event));
		bool txtCtrlWBRed();
		void onTxtCtrlWBBlue(wxCommandEvent& WXUNUSED(event));
		bool txtCtrlWBBlue();
		void onCheckBoxAutoWB(wxCommandEvent& WXUNUSED(event));

   		void onSliderFR(wxCommandEvent& WXUNUSED(event));
		void onTxtCtrlFR(wxCommandEvent& WXUNUSED(event));
		bool txtCtrlFR();

		void onSliderPS(wxCommandEvent& WXUNUSED(event));
		void onTxtCtrlPS(wxCommandEvent& WXUNUSED(event));
		bool txtCtrlPS();

        void onClose(wxCloseEvent& WXUNUSED(event));

		void setExposureTime(double value);
		double exposureTime() const;
		void setAutoMaxTime(double value);
		double autoMaxTime() const;
		void setAutoET(bool value);
		bool autoET() const;
		void adjustCameraExposureTime();
		void adjustCameraAutoMaxTime();

		void setGain(double value);
		double gain() const;
		void setAutoGain(bool value);
		bool autoGain() const;
		void adjustCameraGain();

		void setWhiteBalance(double value, char* colour);
		double whiteBalance(char* colour);
		void setAutoWB(bool value);
		bool autoWB() const;
		void adjustCameraWB(char* colour);

		void setFrameRate(unsigned long);
		unsigned long frameRate() const;
		void adjustCameraFR();

		void setPacketSize(unsigned long);
		unsigned long packetSize() const;
		void adjustCameraPS();

		bool play() const;

	private:
        Cameras* cameras_;
        size_t numCameras_;
        
        wxListBox* cameraIDs_;
        wxComboBox* cameraPositionsComboBox_;
        
        bool cameraSelectET_;
        bool cameraSelectGain_;
		bool cameraSelectWB_;
		int selectComboBoxET_;
        int selectComboBoxGain_;
        int selectComboBoxWB_;
        std::vector<double> exposureTimes_;
		std::vector<double> autoMaxTimes_;
		std::vector<bool> autoET_;
        std::vector<int> gains_;
		std::vector<bool> autoGain_;
        std::vector<unsigned long> whiteBalanceRed_;
		std::vector<unsigned long> whiteBalanceBlue_;
		std::vector<bool> autoWB_;
		unsigned long frameRate_;
        unsigned long packetSize_;
		bool play_;
        wxNotebook* notebook_;

        wxComboBox* cameraSelectChoiceET_;
        wxComboBox* cameraSelectChoiceGain_;
        wxComboBox* cameraSelectChoiceWB_;

        wxSlider* sliderET_;
		wxSlider* sliderAMT_;
        wxSlider* sliderGain_;
        wxSlider* sliderWBRed_;
		wxSlider* sliderWBBlue_;
		wxSlider* sliderFR_;
        wxSlider* sliderPS_;
        
        wxTextCtrl* textCtrlET_;
		 wxTextCtrl* textCtrlAMT_;
        wxTextCtrl* textCtrlGain_;
        wxTextCtrl* textCtrlWBRed_;
		wxTextCtrl* textCtrlWBBlue_;
		wxTextCtrl* textCtrlFR_;
        wxTextCtrl* textCtrlPS_;

		wxCheckBox* checkBoxCameraSelectET_;
        wxCheckBox* checkBoxAutoET_;
   		wxCheckBox* checkBoxCameraSelectGain_;
        wxCheckBox* checkBoxAutoGain_;
		wxCheckBox* checkBoxCameraSelectWB_;
		wxCheckBox* checkBoxAutoWB_;

    	wxPanel* panelFR_;

        wxPanel* panelPS_;

        enum
        {
            ID_OK = 1,
            ID_Camera_IDs,
            ID_CameraPositions,
            ID_CheckBoxCameraSelectET,
			ID_ComboBoxET,
            ID_SliderET,
            ID_TxtCtrlET,
			ID_SliderAMT,
			ID_TxtCtrlAMT,
            ID_CheckBoxAutoET,
            ID_CheckBoxCameraSelectGain,
			ID_ComboBoxGain,
            ID_SliderGain,
            ID_TxtCtrlGain,
            ID_CheckBoxAutoGain,
            ID_CheckBoxCameraSelectWB,
			ID_ComboBoxWB,
            ID_SliderWBRed,
            ID_TxtCtrlWBRed,
			ID_SliderWBBlue,
			ID_TxtCtrlWBBlue,
			ID_CheckBoxWB,
   			ID_SliderFR,
			ID_TxtCtrlFR,
			ID_SliderPS,
			ID_TxtCtrlPS
        };

        DECLARE_EVENT_TABLE()

    };
} //namespace rics

#endif //CAMERAPROPDIALOG_H