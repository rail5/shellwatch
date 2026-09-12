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

#include <cstdint>
#include <expected>
#include <filesystem>

using Milliseconds = std::uint64_t;

class MainWindow : public wxFrame {
	public:
		explicit MainWindow(wxWindow* parent, wxWindowID id = -1);
		virtual ~MainWindow();

		MainWindow(const MainWindow&) = delete;
		MainWindow& operator=(const MainWindow&) = delete;
		MainWindow(MainWindow&&) = delete;
		MainWindow& operator=(MainWindow&&) = delete;

	private:
		std::filesystem::path script;

		std::uint32_t currentLineNumber = 0;

		Milliseconds autostepInterval = 100;
		bool autostepEnabled = false;

		/**
		 * @brief Parse a string representing an autostep interval in seconds (e.g., '0.1') and convert it to milliseconds (e.g., 100).
		 *
		 * @param text The input string representing the autostep interval in seconds.
		 * @return std::expected<Milliseconds, bool> The parsed autostep interval in milliseconds, or false if the input is invalid.
		 */
		static std::expected<Milliseconds, bool> parseAutostepInterval(const wxString& text);

		/**
		 * @brief Display a given interval in milliseconds as a string in seconds with up to 3 decimal places (e.g., 100 -> "0.1").
		 *
		 * @param ms The interval in milliseconds to be converted to seconds.
		 * @return std::string The formatted string representing the interval in seconds.
		 */
		static std::string displayAsSeconds(Milliseconds ms);

		/**
		 * @brief Highlight a specific line in the source code display based on the provided line number. The line number is 1-based.
		 *
		 * If called as highlightSourceCodeLine(0), it will clear any existing highlights without highlighting any line.
		 * 
		 * @param lineNumber The line number to highlight in the source code display (1-based).
		 */
		void highlightSourceCodeLine(std::uint32_t lineNumber);

		/**
		 * @brief Update both the currentLineNumber internal value and the displayed line number in the GUI.
		 * 
		 * @param lineNumber The new line number to set and display.
		 */
		void updateLineNumber(std::uint32_t lineNumber);

		/**
		 * @brief Update the internal script file path, set the line number to 0, and update the displayed source code to the contents of the new script file.
		 * 
		 * @param filePath The path to the new script file to be set.
		 */
		void setScriptFile(const std::filesystem::path& filePath);

		void OnOpen(wxCommandEvent& event);
		void OnAutostepTextChanged(wxCommandEvent& event);
		void OnStep(wxCommandEvent& event);

		void OnQuit(wxCommandEvent& /*event*/);
		void OnAbout(wxCommandEvent& /*event*/);

		const wxWindowID ID_BUTTON1       = wxNewId();
		const wxWindowID ID_STATICTEXT1   = wxNewId();
		const wxWindowID ID_BUTTON2       = wxNewId();
		const wxWindowID ID_CHECKBOX1     = wxNewId();
		const wxWindowID ID_RICHTEXTCTRL1 = wxNewId();
		const wxWindowID ID_LISTCTRL1     = wxNewId();
		const wxWindowID ID_RICHTEXTCTRL2 = wxNewId();

		wxMenuBar* menuBar;
		wxMenu* fileMenu;
		wxMenu* helpMenu;
		wxButton* QuitButton;
		wxButton* StepButton;
		wxCheckBox* AutostepCheckbox;
		wxListCtrl* ShellVariableListCtrl;
		wxTextCtrl* AutostepTextCtrl;
		wxRichTextCtrl* SourceCodeDisplay;
		wxStaticText* LineNumberLabel;
		wxBoxSizer* BottomSizer;
		wxBoxSizer* StepButtonContainer;
		wxBoxSizer* AutostepContainer;
		wxBoxSizer* TopSizer;
		wxFlexGridSizer* MainWindowContainer;

		DECLARE_EVENT_TABLE() // NOLINT (no point linting the expansion of a macro we can't control)
};
