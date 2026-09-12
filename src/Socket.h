/**
 * Copyright (C) 2026 Andrew S. Rightenburg
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cerrno>
#include <cstring>
#include <set>
#include <stdexcept>
#include <system_error>
#include <string_view>
#include <unordered_map>

struct DebugInfo {
	std::size_t lineNumber = 0;
	std::unordered_map<std::string, std::string> variables;
};

class Socket {
	public:
		int getShellToGuiWriteFd() const { return shellToGuiWriteFd; }
		int getGuiToShellReadFd() const { return guiToShellReadFd; }
		void closeChildEndsInParent() {
			if (shellToGuiWriteFd >= 0) {
				close(shellToGuiWriteFd);
				shellToGuiWriteFd = -1;
			}
			if (guiToShellReadFd >= 0) {
				close(guiToShellReadFd);
				guiToShellReadFd = -1;
			}
		}
		void addInitialVariableName(const std::string& name) { initialVariableNames.insert(name); }

		void sendContinueSignal() const { sendData("C\n"); }
		void sendDieSignal() const { sendData("D\n"); }

		DebugInfo receiveVariables() {
			DebugInfo debugInfo;
			std::string data = receiveData();

			auto getLine = [&data](std::size_t& pos) -> std::string_view {
				if (pos >= data.size()) return {};
				std::size_t endPos = data.find('\n', pos);
				if (endPos == std::string::npos) endPos = data.size();
				std::string_view line(&data[pos], endPos - pos);
				pos = endPos + 1;
				return line;
			};

			std::size_t pos = 0;

			if (data.starts_with("__SHELLWATCH_INIT_BEGIN")) {
				pos = std::strlen("__SHELLWATCH_INIT_BEGIN") + 1;
				while (true) {
					std::string_view line = getLine(pos);
					if (line == "__SHELLWATCH_INIT_END") {
						break;
					} else if (line.empty()) {
						continue;
					}
					addInitialVariableName(std::string(line));
				}
			}

			pos = data.find("__SHELLWATCH_VARS_BEGIN", pos);
			if (pos == std::string::npos) throw std::runtime_error("No variables block found in received data");
			std::string_view line = getLine(pos);
			line.remove_prefix(std::strlen("__SHELLWATCH_VARS_BEGIN "));
			debugInfo.lineNumber = std::stoul(std::string(line));

			while (true) {
				std::string_view line = getLine(pos);
				if (line == "__SHELLWATCH_VARS_END") {
					break;
				} else if (line.empty()) {
					continue;
				}

				std::string_view declarePrefix = "declare ";
				if (!line.starts_with(declarePrefix)) {
					throw std::runtime_error("Invalid variable declaration format");
				}
				line.remove_prefix(declarePrefix.size());

				while (!line.empty() && line[0] == '-') {
					std::size_t spacePos = line.find(' ');
					if (spacePos == std::string::npos) throw std::runtime_error("Invalid variable declaration format");
					line.remove_prefix(spacePos + 1);
				}

				std::size_t equalPos = line.find('=');
				std::string varName;
				std::string varValue;

				if (equalPos != std::string::npos) {
					varName = std::string(line.substr(0, equalPos));
					varValue = std::string(line.substr(equalPos + 1));
				} else {
					varName = std::string(line);
					varValue = "";
				}

				if (!initialVariableNames.contains(varName)) debugInfo.variables[varName] = varValue;
			}

			return debugInfo;
		}
	private:
		int shellToGuiReadFd = -1;
		int shellToGuiWriteFd = -1;
		int guiToShellReadFd = -1;
		int guiToShellWriteFd = -1;

		std::set<std::string> initialVariableNames = {"_shellwatch_response", "FUNCNAME", "BASH_TRAPSIG"};

		void sendData(const std::string& data) const {
			if (guiToShellWriteFd < 0) throw std::runtime_error("GUI-to-shell socket is not connected");
			ssize_t bytesSent = write(guiToShellWriteFd, data.c_str(), data.size());
			if (bytesSent < 0) throw std::runtime_error("Failed to send data");
		}

		std::string receiveData() const {
			if (shellToGuiReadFd < 0) throw std::runtime_error("Shell-to-GUI socket is not connected");
			std::string buffer;
			std::array<char, 1024> tempBuffer{};
			while (true) {
				ssize_t bytesRead = read(shellToGuiReadFd, tempBuffer.data(), tempBuffer.size());
				if (bytesRead < 0) throw std::runtime_error("Failed to receive data");
				if (bytesRead == 0) break;
				buffer.append(tempBuffer.data(), static_cast<std::size_t>(bytesRead));
				if (buffer.contains("__SHELLWATCH_VARS_END")) break;
			}

			return buffer;
		}

	public:
		Socket() {
			std::array<int, 2> shellToGuiFds{};
			std::array<int, 2> guiToShellFds{};

			if (socketpair(AF_UNIX, SOCK_STREAM, 0, shellToGuiFds.data()) < 0) {
				std::error_code ec(errno, std::generic_category());
				throw std::runtime_error(std::string("Failed to create shell-to-GUI socket pair: ") + ec.message());
			}

			if (socketpair(AF_UNIX, SOCK_STREAM, 0, guiToShellFds.data()) < 0) {
				close(shellToGuiFds[0]);
				close(shellToGuiFds[1]);
				std::error_code ec(errno, std::generic_category());
				throw std::runtime_error(std::string("Failed to create GUI-to-shell socket pair: ") + ec.message());
			}

			shellToGuiReadFd = shellToGuiFds[0];
			shellToGuiWriteFd = shellToGuiFds[1];
			guiToShellReadFd = guiToShellFds[0];
			guiToShellWriteFd = guiToShellFds[1];
		}

		~Socket() {
			if (shellToGuiReadFd >= 0) close(shellToGuiReadFd);
			if (shellToGuiWriteFd >= 0) close(shellToGuiWriteFd);
			if (guiToShellReadFd >= 0) close(guiToShellReadFd);
			if (guiToShellWriteFd >= 0) close(guiToShellWriteFd);
		}

		Socket(const Socket&) = delete;
		Socket& operator=(const Socket&) = delete;
		Socket(Socket&& other) noexcept
			: shellToGuiReadFd(other.shellToGuiReadFd), shellToGuiWriteFd(other.shellToGuiWriteFd), guiToShellReadFd(other.guiToShellReadFd), guiToShellWriteFd(other.guiToShellWriteFd), initialVariableNames(std::move(other.initialVariableNames)) {
			other.shellToGuiReadFd = -1;
			other.shellToGuiWriteFd = -1;
			other.guiToShellReadFd = -1;
			other.guiToShellWriteFd = -1;
		}
		Socket& operator=(Socket&& other) noexcept {
			if (this != &other) {
				if (shellToGuiReadFd >= 0) close(shellToGuiReadFd);
				if (shellToGuiWriteFd >= 0) close(shellToGuiWriteFd);
				if (guiToShellReadFd >= 0) close(guiToShellReadFd);
				if (guiToShellWriteFd >= 0) close(guiToShellWriteFd);

				shellToGuiReadFd = other.shellToGuiReadFd;
				shellToGuiWriteFd = other.shellToGuiWriteFd;
				guiToShellReadFd = other.guiToShellReadFd;
				guiToShellWriteFd = other.guiToShellWriteFd;
				initialVariableNames = std::move(other.initialVariableNames);

				other.shellToGuiReadFd = -1;
				other.shellToGuiWriteFd = -1;
				other.guiToShellReadFd = -1;
				other.guiToShellWriteFd = -1;
			}
			return *this;
		}
};
