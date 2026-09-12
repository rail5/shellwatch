/**
 * Copyright (C) 2026 Andrew S. Rightenburg
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "MainWindow.h"

#include <wx/msgdlg.h>
#include <wx/intl.h>
#include <wx/string.h>

// Unfortunately, wxWidgets relies on raw 'new' and 'delete'.
// NOLINTBEGIN(cppcoreguidelines-owning-memory)
// NOLINTBEGIN(cppcoreguidelines-prefer-member-initializer)

BEGIN_EVENT_TABLE(MainWindow,wxFrame) // NOLINT (no point linting the expansion of a macro we can't control)
END_EVENT_TABLE()

MainWindow::MainWindow(wxWindow* parent,wxWindowID id) {
	Create(parent, id, _("wxWidgets app"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
	FlexGridSizer1 = new wxFlexGridSizer(2, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0, 1);
	FlexGridSizer1->AddGrowableRow(1, 1);
	TopSizer = new wxBoxSizer(wxHORIZONTAL);
	QuitButton = new wxButton(this, ID_BUTTON1, _("Terminate"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	TopSizer->Add(QuitButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Line: 0"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer2->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StepButton = new wxButton(this, ID_BUTTON2, _("Step"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	BoxSizer2->Add(StepButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TopSizer->Add(BoxSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3 = new wxBoxSizer(wxVERTICAL);
	AutostepCheckbox = new wxCheckBox(this, ID_CHECKBOX1, _("Auto-step"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	AutostepCheckbox->SetValue(false);
	BoxSizer3->Add(AutostepCheckbox, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	AutostepTextCtrl = new wxRichTextCtrl(this, ID_RICHTEXTCTRL1, _T("0.1"), wxDefaultPosition, wxSize(85,32), wxRE_MULTILINE, wxDefaultValidator, _T("ID_RICHTEXTCTRL1"));
	wxRichTextAttr rchtxtAttr_1;
	rchtxtAttr_1.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ALIGN_LEFT);
	BoxSizer3->Add(AutostepTextCtrl, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TopSizer->Add(BoxSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(TopSizer, 1, wxALL|wxEXPAND, 5);
	BottomSizer = new wxBoxSizer(wxHORIZONTAL);
	ShellVariableListCtrl = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_LIST, wxDefaultValidator, _T("ID_LISTCTRL1"));
	BottomSizer->Add(ShellVariableListCtrl, 1, wxALL|wxEXPAND, 5);
	SourceCodeDisplay = new wxRichTextCtrl(this, ID_RICHTEXTCTRL2, _("Text"), wxDefaultPosition, wxDefaultSize, wxRE_MULTILINE, wxDefaultValidator, _T("ID_RICHTEXTCTRL2"));
	wxRichTextAttr rchtxtAttr_2;
	rchtxtAttr_2.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ALIGN_LEFT);
	SourceCodeDisplay->SetMinSize(wxSize(250,350));
	BottomSizer->Add(SourceCodeDisplay, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer1->Add(BottomSizer, 1, wxALL|wxEXPAND, 5);
	SetSizerAndFit(FlexGridSizer1);
	Centre();
}

MainWindow::~MainWindow() {}

void MainWindow::OnQuit(wxCommandEvent& event) {
	Close();
}

void MainWindow::OnAbout(wxCommandEvent& event) {
	wxMessageBox(_("About window"), _("About"));
}

// NOLINTEND(cppcoreguidelines-prefer-member-initializer)
// NOLINTEND(cppcoreguidelines-owning-memory)
