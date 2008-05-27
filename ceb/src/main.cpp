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
#include "session_manager.h"
#include "language_manager.h"
#include "token_display.h"
#include "script.h"
#include "profile.h"

int main(int argc, char **argv)
{
    MyApplication a(argc, argv);

    a.setApplicationName("CeB");
    a.setOrganizationName("http://www.melting-pot.org");
    a.setOrganizationName("MeltingPot");

    TextSkin::createDefaultSkin(); // Used to create fonts after QApplication (otherwise, it failed and gives wrong fonts)

    a.setWindowIcon(QIcon(":/images/ceb.png"));

    QTranslator translator;

    Profile::instance().load();

    if (Profile::instance().language != "")
    {
        QString fileName = LanguageManager::getLanguageFileName(Profile::instance().language);
        if (!fileName.isEmpty())
        {
            translator.load(fileName);
            a.installTranslator(&translator);
        }
    }

    MainWindow::instance()->show();

    int appRes = a.exec();

    Script::closeModifiers(); // Close all lua chunks

    Profile::instance().save();

    return appRes;
}
