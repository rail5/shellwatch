/**
 * Copyright (C) 2026 Andrew S. Rightenburg
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "MainWindow.h"
#include "State.h"

#include <fstream>
#include <sstream>

void WindowAndStatePair::updateLineNumber(std::uint32_t lineNumber) {
	state.currentLineNumber = lineNumber;
	window.LineNumberLabel->SetLabelText(wxString::Format(_("Line: %u"), state.currentLineNumber));
	window.highlightSourceCodeLine(state.currentLineNumber);
}

void WindowAndStatePair::setScriptFile(const std::filesystem::path& filePath) {
	if (!std::filesystem::exists(filePath)) {
		window.SourceCodeDisplay->SetValue(_("Error: Script file does not exist."));
		return;
	}

	state.script = filePath;
	updateLineNumber(0);

	// Load the script file content into the SourceCodeDisplay
	std::ifstream scriptFile(state.script);
	if (!scriptFile) {
		window.SourceCodeDisplay->SetValue(_("Error: Unable to open script file."));
		return;
	}

	std::stringstream contents;
	contents << scriptFile.rdbuf();

	window.SourceCodeDisplay->SetValue(contents.str());
	scriptFile.close();
}

WindowAndStatePair operator|(MainWindow& window, State& state) { return {window, state}; }
WindowAndStatePair operator|(State& state, MainWindow& window) { return {window, state}; }
