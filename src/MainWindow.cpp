/**
 * Copyright (C) 2026 Andrew S. Rightenburg
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "MainWindow.h"

#include <wx/msgdlg.h>
#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/string.h>

// Unfortunately, wxWidgets relies on raw 'new' and 'delete'.
// NOLINTBEGIN(cppcoreguidelines-owning-memory)
// NOLINTBEGIN(cppcoreguidelines-prefer-member-initializer)

BEGIN_EVENT_TABLE(MainWindow,wxFrame) // NOLINT (no point linting the expansion of a macro we can't control)
EVT_MENU(wxID_EXIT, MainWindow::OnQuit)
EVT_MENU(wxID_ABOUT, MainWindow::OnAbout)
END_EVENT_TABLE()

MainWindow::MainWindow(wxWindow* parent,wxWindowID id) {
	Create(parent, id, _("Shellwatch"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));

	menuBar = new wxMenuBar();
	fileMenu = new wxMenu();

	fileMenu->Append(wxID_OPEN, _("Open"));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, _("Quit"));
	menuBar->Append(fileMenu, _("File"));

	helpMenu = new wxMenu();
	helpMenu->Append(wxID_ABOUT, _("About"));
	menuBar->Append(helpMenu, _("Help"));

	SetMenuBar(menuBar);

	MainWindowContainer = new wxFlexGridSizer(2, 1, 0, 0);
	MainWindowContainer->AddGrowableCol(0, 1);
	MainWindowContainer->AddGrowableRow(1, 1);

	TopSizer = new wxBoxSizer(wxHORIZONTAL);

	QuitButton = new wxButton(this, ID_BUTTON1, _("Terminate"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));

	TopSizer->Add(QuitButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

	StepButtonContainer = new wxBoxSizer(wxVERTICAL);
	LineNumberLabel = new wxStaticText(this, ID_STATICTEXT1, _("Line: 0"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	StepButton = new wxButton(this, ID_BUTTON2, _("Step"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	StepButtonContainer->Add(LineNumberLabel, 0, wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL, 1);
	StepButtonContainer->Add(StepButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 1);

	TopSizer->Add(StepButtonContainer, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

	AutostepContainer = new wxBoxSizer(wxVERTICAL);
	AutostepCheckbox = new wxCheckBox(this, ID_CHECKBOX1, _("Auto-step"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	AutostepCheckbox->SetValue(false);
	AutostepTextCtrl = new wxTextCtrl(this, ID_RICHTEXTCTRL1, _T("0.1"), wxDefaultPosition, wxSize(85,32), 0, wxDefaultValidator, _T("ID_RICHTEXTCTRL1"));
	AutostepContainer->Add(AutostepCheckbox, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	AutostepContainer->Add(AutostepTextCtrl, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);

	TopSizer->Add(AutostepContainer, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

	MainWindowContainer->Add(TopSizer, 1, wxALL|wxEXPAND, 5);

	BottomSizer = new wxBoxSizer(wxHORIZONTAL);

	ShellVariableListCtrl = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT, wxDefaultValidator, _T("ID_LISTCTRL1"));
	ShellVariableListCtrl->InsertColumn(0, _("Variable"), wxLIST_FORMAT_LEFT, 150);
	ShellVariableListCtrl->InsertColumn(1, _("Value"), wxLIST_FORMAT_LEFT, 150);

	BottomSizer->Add(ShellVariableListCtrl, 1, wxALL|wxEXPAND, 5);

	SourceCodeDisplay = new wxRichTextCtrl(this, ID_RICHTEXTCTRL2, _("Line 1\nLine 2\nLine 3\n"), wxDefaultPosition, wxDefaultSize, wxRE_MULTILINE, wxDefaultValidator, _T("ID_RICHTEXTCTRL2"));
	wxRichTextAttr rchtxtAttr_2;
	rchtxtAttr_2.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ALIGN_LEFT);

	SourceCodeDisplay->SetMinSize(wxSize(250,350));

	BottomSizer->Add(SourceCodeDisplay, 1, wxALL|wxEXPAND, 5);

	MainWindowContainer->Add(BottomSizer, 1, wxALL|wxEXPAND, 5);

	SetSizerAndFit(MainWindowContainer);
	Centre();
}

MainWindow::~MainWindow() {}

void MainWindow::OnQuit(wxCommandEvent& /*event*/) {
	Close();
}

void MainWindow::OnAbout(wxCommandEvent& /*event*/) { // NOLINT(readability-convert-member-functions-to-static) (wxWidgets requires this to be a member function)
	wxMessageBox(_("About window"), _("About"));
}

// NOLINTEND(cppcoreguidelines-prefer-member-initializer)
// NOLINTEND(cppcoreguidelines-owning-memory)
