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
	window.LineNumberLabel->SetLabelText(wxString::Format(_("Line: %u%s"), state.currentLineNumber, state.executionFinished ? _(" (finished)") : _("")));
	window.highlightSourceCodeLine(state.currentLineNumber);
}

void WindowAndStatePair::setScriptFile(const std::filesystem::path& filePath) {
	if (!std::filesystem::exists(filePath)) {
		window.SourceCodeDisplay->SetValue(_("Error: Script file does not exist."));
		return;
	}

	state.script = filePath;
	state.pendingTrapLineNumber = 0;
	state.executionFinished = false;
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

void WindowAndStatePair::stepScript() {
	if (!state.executing) {
		state.beginExecution();
		(void)state.socket.receiveVariables(); // Discard the initial variables block, which is just the pre-first-line state.
		state.socket.sendContinueSignal();

		// The next reply from the script will *still* be pre-first line
		// Cache its line number so completion after the final continue can
		// still highlight the last executed line.
		auto initialDebugInfo = state.socket.receiveVariables();
		state.pendingTrapLineNumber = static_cast<std::uint32_t>(initialDebugInfo.lineNumber);
	}

	state.socket.sendContinueSignal();

	DebugInfo debugInfo;
	try {
		debugInfo = state.socket.receiveVariables();
	} catch (const std::runtime_error& e) {
		if (std::string_view(e.what()) == "No variables block found in received data") {
			state.executing = false;
			state.executionFinished = true;
			if (state.pendingTrapLineNumber > 0) {
				updateLineNumber(state.pendingTrapLineNumber);
			}
			return;
		}
		throw;
	}
	state.pendingTrapLineNumber = static_cast<std::uint32_t>(debugInfo.lineNumber);
	state.executionFinished = false;
	state.variables = std::move(debugInfo.variables);
	window.setDisplayedVariables(state.variables);
	updateLineNumber(debugInfo.lineNumber > 0 ? static_cast<std::uint32_t>(debugInfo.lineNumber - 1) : 0);
}

void WindowAndStatePair::killScript() {
	if (state.executing) {
		state.socket.sendDieSignal();
		state.executing = false;
	}
	state.variables.clear();
	state.pendingTrapLineNumber = 0;
	state.executionFinished = false;
	window.setDisplayedVariables({});
	updateLineNumber(0);
}

WindowAndStatePair operator|(MainWindow& window, State& state) { return {window, state}; }
WindowAndStatePair operator|(State& state, MainWindow& window) { return {window, state}; }
