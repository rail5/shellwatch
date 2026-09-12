/**
 * Copyright (C) 2026 Andrew S. Rightenburg
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "App.h"

#include "MainWindow.h"
#include <wx/image.h>

IMPLEMENT_APP(App); // NOLINT (no point linting the expansion of a macro we can't control)

bool App::OnInit() {
	wxInitAllImageHandlers();
	auto* dlg = new MainWindow(nullptr); // NOLINT(cppcoreguidelines-owning-memory) (wxWidgets relies on raw 'new' and 'delete')
	SetTopWindow(dlg);
	dlg->Show(true);
	return true;
}
