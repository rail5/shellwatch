/**
 * Copyright (C) 2026 Andrew S. Rightenburg
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <filesystem>
#include <cstdint>
#include <unordered_map>

#include "Socket.h"
#include "run_bash.h"

using Milliseconds = std::uint64_t;

class MainWindow;

class State {
	private:
		Socket socket;
		std::filesystem::path script;
		std::uint32_t pendingTrapLineNumber = 0;
		std::uint32_t currentLineNumber = 0;
		std::unordered_map<std::string, std::string> variables;
		Milliseconds autostepInterval = 100;
		bool autostepEnabled = false;
		bool executing = false;
		bool executionFinished = false;

		void beginExecution() {
			if (executing) return;
			socket = Socket{};
			run_bash(script.string(), socket.getShellToGuiWriteFd(), socket.getGuiToShellReadFd());
			socket.closeChildEndsInParent();
			executing = true;
			executionFinished = false;
		}
	public:
		friend class WindowAndStatePair;
		const std::filesystem::path& getScriptFile() const { return script; }
		std::uint32_t getCurrentLineNumber() const { return currentLineNumber; }
		Milliseconds getAutostepInterval() const { return autostepInterval; }
		bool isAutostepEnabled() const { return autostepEnabled; }

		void setAutostepInterval(Milliseconds interval) { autostepInterval = interval; }
		void setAutostepEnabled(bool enabled) { autostepEnabled = enabled; }
};

class WindowAndStatePair {
	private:
		MainWindow& window;
		State& state;
	public:
		WindowAndStatePair(MainWindow& win, State& st) : window(win), state(st) {}
		~WindowAndStatePair() = default;
		WindowAndStatePair() = delete;
		WindowAndStatePair(const WindowAndStatePair&) = delete;
		WindowAndStatePair& operator=(const WindowAndStatePair&) = delete;
		WindowAndStatePair(WindowAndStatePair&&) = delete;
		WindowAndStatePair& operator=(WindowAndStatePair&&) = delete;

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

		void stepScript();

		void killScript();
};

WindowAndStatePair operator|(MainWindow& window, State& state);
WindowAndStatePair operator|(State& state, MainWindow& window);
