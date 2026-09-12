/**
 * Copyright (C) 2026 Andrew S. Rightenburg
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <wx/app.h>

class App : public wxApp {
	public:
		bool OnInit() override;
};
