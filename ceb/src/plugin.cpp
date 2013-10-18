/* This file is part of CeB.
 * Copyright (C) 2005  Guillaume Denry
 *
 * CeB is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * CeB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CeB; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "common.h"
#include "token_display.h"
#include "script.h"
#include "profile.h"
#include "main_window.h"
#include "plugin.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

QMainWindow *CeBPlugin::getMainWindow() const {
    return MainWindow::instance();
}

void CeBPlugin::init() const {
    TextSkin::createDefaultSkin(); // Used to create fonts after QApplication (otherwise, it failed and gives wrong fonts)
    Profile::instance().load();
}

void CeBPlugin::deinit() const {
    Script::closeModifiers(); // Close all lua chunks

    Profile::instance().save();
	Profile::free();

	MainWindow::free();
	TextSkin::freeDefaultSkin();
}

#ifndef USE_QT5
Q_EXPORT_PLUGIN2(cebplugin, CeBPlugin)
#endif
