/*
 * Copyright (C) 2025 Andrew S. Rightenburg
 * Bash++: Bash with classes
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <cerrno>
#include <string>
#include <vector>
#include <system_error>

#include <sys/wait.h>
#include <unistd.h>

inline void run_bash(const std::string& filename, int shellToGuiFd, int guiToShellFd) {
	std::vector<std::string> exec_args;
	exec_args.reserve(7);
	exec_args.emplace_back("bash");
	exec_args.emplace_back("-c");
	exec_args.emplace_back(R"(export SHELLWATCH_SOCKET_TO_GUI="$1"; export SHELLWATCH_SOCKET_FROM_GUI="$2"; source /usr/share/shellwatch/helper.sh; source "$3")");
	exec_args.emplace_back("shellwatch"); // Dummy $0
	exec_args.emplace_back(std::to_string(shellToGuiFd));
	exec_args.emplace_back(std::to_string(guiToShellFd));
	exec_args.emplace_back(filename);
	std::vector<char*> argv;
	argv.reserve(exec_args.size() + 1);
	for (auto& s : exec_args) {
		argv.push_back(s.data());
	}
	argv.push_back(nullptr);

	pid_t child = fork();
	if (child < 0) {
		std::error_code ec(errno, std::generic_category());
		throw std::runtime_error(std::string("fork() failed: ") + ec.message());
	}

	if (child == 0) {
		// We are now in the child process. Execute the compiled script.
		execvp(argv[0], argv.data());
		// If exec fails, we cannot throw (we're in the child) and we must not run
		// parent atexit handlers. Exit 127 is a widely-used convention for exec failure.
		_exit(127);
	}
}
