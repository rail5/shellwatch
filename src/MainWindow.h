/**
 * Copyright (C) 2026 Andrew S. Rightenburg
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

class MainWindow : public wxFrame {
	public:
		explicit MainWindow(wxWindow* parent, wxWindowID id = -1);
		virtual ~MainWindow();

		MainWindow(const MainWindow&) = delete;
		MainWindow& operator=(const MainWindow&) = delete;
		MainWindow(MainWindow&&) = delete;
		MainWindow& operator=(MainWindow&&) = delete;

	private:
		void OnQuit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);

		const wxWindowID ID_BUTTON1       = wxNewId();
		const wxWindowID ID_STATICTEXT1   = wxNewId();
		const wxWindowID ID_BUTTON2       = wxNewId();
		const wxWindowID ID_CHECKBOX1     = wxNewId();
		const wxWindowID ID_RICHTEXTCTRL1 = wxNewId();
		const wxWindowID ID_LISTCTRL1     = wxNewId();
		const wxWindowID ID_RICHTEXTCTRL2 = wxNewId();

		wxButton* QuitButton;
		wxButton* StepButton;
		wxCheckBox* AutostepCheckbox;
		wxListCtrl* ShellVariableListCtrl;
		wxRichTextCtrl* AutostepTextCtrl;
		wxRichTextCtrl* SourceCodeDisplay;
		wxStaticText* StaticText1;
		wxBoxSizer* BottomSizer;
		wxBoxSizer* BoxSizer2;
		wxBoxSizer* BoxSizer3;
		wxBoxSizer* TopSizer;
		wxFlexGridSizer* FlexGridSizer1;

		DECLARE_EVENT_TABLE() // NOLINT (no point linting the expansion of a macro we can't control)
};
