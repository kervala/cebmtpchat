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

#include <QApplication>
#include <QMessageBox>
#include <QTranslator>

#include "my_application.h"
#include "main_window.h"
#include "profile_manager.h"
#include "general_config.h"
#include "session_manager.h"
#include "language_manager.h"
#include "token_display.h"
#include "modifier.h"

int main(int argc, char **argv)
{
	MyApplication a(argc, argv);

	TextSkin::createDefaultSkin(); // Used to create fonts after QApplication (otherwise, it failed and gives wrong fonts)
//	openModifiers(); // Load LUA modifiers

	a.setWindowIcon(QIcon(":/images/ceb.png"));
	ProfileManager &manager = ProfileManager::instance();
	if (!manager.currentProfile())
	{
		QMessageBox::critical(0, "Error", "profile file no specified and default file is missing. Aborting.");
		return 1;
	}

	Profile &profile = *ProfileManager::instance().currentProfile();
	QTranslator translator;

	if (!profile.language.isEmpty())
	{
		QString fileName = LanguageManager::getLanguageFileName(profile.language);
		if (!fileName.isEmpty())
		{
			translator.load(fileName);
			a.installTranslator(&translator);
		}
	}

	MainWindow mainWindow;
	mainWindow.show();

	int appRes = a.exec();

	closeModifiers(); // Close all lua chunks

	return appRes;
}
