/**
 * Copyright (C) 2026 Andrew S. Rightenburg
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "MainWindow.h"

#include <wx/app.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/settings.h>
#include <wx/string.h>

#include <utility>

// Unfortunately, wxWidgets relies on raw 'new' and 'delete'.
// NOLINTBEGIN(cppcoreguidelines-owning-memory)
// NOLINTBEGIN(cppcoreguidelines-prefer-member-initializer)

BEGIN_EVENT_TABLE(MainWindow,wxFrame) // NOLINT (no point linting the expansion of a macro we can't control)
EVT_MENU(wxID_OPEN, MainWindow::OnOpen)
EVT_MENU(wxID_ABOUT, MainWindow::OnAbout)
END_EVENT_TABLE()

MainWindow::MainWindow(wxWindow* parent,wxWindowID id) {
	Create(parent, id, _("Shellwatch"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));

	menuBar = new wxMenuBar();
	fileMenu = new wxMenu();

	fileMenu->Append(wxID_OPEN, _("Open"));
	fileMenu->AppendSeparator();
	fileMenu->Append(ID_MENU_QUIT, _("Quit"));
	menuBar->Append(fileMenu, _("File"));

	helpMenu = new wxMenu();
	helpMenu->Append(wxID_ABOUT, _("About"));
	menuBar->Append(helpMenu, _("Help"));

	SetMenuBar(menuBar);
	Bind(wxEVT_MENU, &MainWindow::OnQuit, this, ID_MENU_QUIT);

	MainWindowContainer = new wxFlexGridSizer(2, 1, 0, 0);
	MainWindowContainer->AddGrowableCol(0, 1);
	MainWindowContainer->AddGrowableRow(1, 1);

	TopSizer = new wxBoxSizer(wxHORIZONTAL);

	QuitButton = new wxButton(this, ID_BUTTON1, _("Terminate"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	QuitButton->Bind(wxEVT_BUTTON, &MainWindow::OnTerminate, this);

	TopSizer->Add(QuitButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

	StepButtonContainer = new wxBoxSizer(wxVERTICAL);
	LineNumberLabel = new wxStaticText(this, ID_STATICTEXT1, _("Line: 0"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	StepButton = new wxButton(this, ID_BUTTON2, _("Step"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	StepButton->Bind(wxEVT_BUTTON, &MainWindow::OnStep, this);
	StepButtonContainer->Add(LineNumberLabel, 0, wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL, 1);
	StepButtonContainer->Add(StepButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 1);

	TopSizer->Add(StepButtonContainer, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

	AutostepContainer = new wxBoxSizer(wxVERTICAL);
	AutostepCheckbox = new wxCheckBox(this, ID_CHECKBOX1, _("Auto-step"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	AutostepCheckbox->SetValue(false);
	AutostepCheckbox->Bind(wxEVT_CHECKBOX, &MainWindow::OnAutostepCheckboxChanged, this);
	AutostepTextCtrl = new wxTextCtrl(this, ID_RICHTEXTCTRL1, _T("0.1"), wxDefaultPosition, wxSize(85,32), 0, wxDefaultValidator, _T("ID_RICHTEXTCTRL1"));
	AutostepTextCtrl->Bind(wxEVT_TEXT, &MainWindow::OnAutostepTextChanged, this);
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
	AutoStepTimer.SetOwner(this, ID_TIMER_AUTOSTEP);
	Bind(wxEVT_TIMER, &MainWindow::OnAutostepTimer, this, ID_TIMER_AUTOSTEP);
	Centre();
}

MainWindow::~MainWindow() {}

std::expected<Milliseconds, bool> MainWindow::parseAutostepInterval(const wxString& text) {
	std::uint64_t seconds = 0;
	std::uint64_t milliseconds = 0;
	std::uint64_t decimalPosition = 3; // Max 3 decimal places (0.001 = decimal position 1, 0.01 = decimal position 2, 0.1 = decimal position 3)

	bool decimalPointEncountered = false;

	for (const char c : text) {
		switch (c) {
			case '0' ... '9':
				if (!decimalPointEncountered) {
					seconds *= 10;
					seconds += static_cast<std::uint64_t>(c - '0');
				} else {
					if (decimalPosition == 0) break; // Ignore digits beyond 3 decimal places
					milliseconds += static_cast<std::uint64_t>(c - '0') * static_cast<std::uint64_t>(std::pow(10, decimalPosition - 1));
					decimalPosition = (decimalPosition > 0) ? decimalPosition - 1 : 0;
				}
				break;
			case '.':
				if (decimalPointEncountered) {
					return std::unexpected(false); // Invalid input: multiple decimal points
				}
				decimalPointEncountered = true;
				break;
			default:
				return std::unexpected(false); // Invalid input: non-numeric character
		}
	}

	return {(seconds * 1000) + milliseconds};
}

std::string MainWindow::displayAsSeconds(Milliseconds ms) {
	std::uint64_t seconds = ms / 1000;
	std::uint64_t milliseconds = ms % 1000;

	std::string result = std::to_string(seconds) + '.';
	if (milliseconds < 10) result += '0';
	if (milliseconds < 100) result += '0';
	result += std::to_string(milliseconds);

	return result;
}

void MainWindow::highlightSourceCodeLine(std::uint32_t lineNumber) {
	// Reset all text to the control's normal colors before highlighting a new line.
	const wxColour normalTextColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
	const wxColour normalBackgroundColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	const auto lastPos = SourceCodeDisplay->GetLastPosition();
	SourceCodeDisplay->SetStyle(wxRichTextRange(0, lastPos + 1), wxTextAttr(normalTextColor, normalBackgroundColor));

	if (lineNumber == 0) return; // No line to highlight
	if (std::cmp_greater(lineNumber ,SourceCodeDisplay->GetNumberOfLines())) return;

	// Calculate the start and end positions of the specified line
	auto lineStartPos = SourceCodeDisplay->XYToPosition(0, lineNumber - 1);
	auto lineEndPos = SourceCodeDisplay->XYToPosition(SourceCodeDisplay->GetLineLength(lineNumber - 1), lineNumber - 1);
	wxRichTextRange lineRange(lineStartPos, lineEndPos);

	// Highlight the specified line
	SourceCodeDisplay->SetStyle(lineRange, wxTextAttr(wxColour(0, 0, 0), wxColour(255, 255, 0))); // Yellow background, black text
	SourceCodeDisplay->ShowPosition(lineStartPos); // Ensure the highlighted line is visible
}

void MainWindow::setDisplayedVariables(const std::unordered_map<std::string, std::string>& variables) {
	ShellVariableListCtrl->DeleteAllItems();
	for (const auto& [varName, varValue] : variables) {
		std::int64_t index = ShellVariableListCtrl->InsertItem(ShellVariableListCtrl->GetItemCount(), varName);
		ShellVariableListCtrl->SetItem(index, 1, varValue);
	}
}

void MainWindow::OnAutostepTextChanged(wxCommandEvent& event) {
	const std::expected<Milliseconds, bool> parsedInterval = parseAutostepInterval(event.GetString());
	if (parsedInterval) {
		state.setAutostepInterval(*parsedInterval);
		AutostepTextCtrl->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)); // Reset to default background color
		AutostepTextCtrl->Refresh(); // Refresh to apply the color change
		if (state.isAutostepEnabled()) {
			AutoStepTimer.Stop();
			AutoStepTimer.Start(static_cast<int>(state.getAutostepInterval()));
		}
	} else {
		// Highlight the text box in red to indicate an error
		AutostepTextCtrl->SetBackgroundColour(wxColour(255, 0, 0)); // Red background
		AutostepTextCtrl->Refresh(); // Refresh to apply the color change
		state.setAutostepEnabled(false);
		AutostepCheckbox->SetValue(false);
		AutoStepTimer.Stop();
	}
}

void MainWindow::OnAutostepCheckboxChanged(wxCommandEvent& event) {
	state.setAutostepEnabled(event.IsChecked());
	if (event.IsChecked()) {
		AutoStepTimer.Start(static_cast<int>(state.getAutostepInterval()));
	} else {
		AutoStepTimer.Stop();
	}
}

void MainWindow::OnAutostepTimer(wxTimerEvent& /*event*/) {
	if (!state.isAutostepEnabled()) {
		AutoStepTimer.Stop();
		return;
	}
	if (AutoStepTickInProgress) {
		return;
	}

	AutoStepTickInProgress = true;
	(*this|state).stepScript();
	AutoStepTickInProgress = false;

	if (state.isExecutionFinished()) {
		state.setAutostepEnabled(false);
		AutostepCheckbox->SetValue(false);
		AutoStepTimer.Stop();
	}
}

void MainWindow::OnOpen(wxCommandEvent& /*event*/) {
	wxFileDialog dialog(this, _("Open script"), wxEmptyString, wxEmptyString, _("All files (*.*)|*.*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (dialog.ShowModal() == wxID_OK) {
		(*this|state).setScriptFile(std::filesystem::path(dialog.GetPath().ToStdString()));
	}
}

void MainWindow::OnStep(wxCommandEvent& /*event*/) {
	(*this|state).stepScript();
}

void MainWindow::OnTerminate(wxCommandEvent& /*event*/) {
	AutoStepTimer.Stop();
	state.setAutostepEnabled(false);
	AutostepCheckbox->SetValue(false);
	(*this|state).killScript();
}

void MainWindow::OnQuit(wxCommandEvent& /*event*/) { // NOLINT(readability-convert-member-functions-to-static) (wxWidgets requires this to be a member function)
	AutoStepTimer.Stop();
	state.setAutostepEnabled(false);
	AutostepCheckbox->SetValue(false);
	if (wxAppConsole* app = wxApp::GetInstance()) {
		app->ExitMainLoop();
	}
}

void MainWindow::OnAbout(wxCommandEvent& /*event*/) { // NOLINT(readability-convert-member-functions-to-static) (wxWidgets requires this to be a member function)
	wxMessageBox(_("About window"), _("About"));
}

// NOLINTEND(cppcoreguidelines-prefer-member-initializer)
// NOLINTEND(cppcoreguidelines-owning-memory)
