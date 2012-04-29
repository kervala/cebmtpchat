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
#include "my_application.h"
#include "main_window.h"
#include "script.h"
#include "profile.h"
#include "mtpchat_handler.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define PRODUCT "CeB"
#define AUTHOR "MeltingPot"
#endif

#ifdef QT_STATICPLUGIN
#include <QtPlugin>
Q_IMPORT_PLUGIN(qsvgicon);
#endif

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

int main(int argc, char **argv)
{
#if defined(_MSC_VER) && defined(_DEBUG)
	_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    Q_INIT_RESOURCE(ceb);

    QApplication a(argc, argv);

    QApplication::setApplicationName(PRODUCT);
    QApplication::setOrganizationName(AUTHOR);

    TextSkin::createDefaultSkin(); // Used to create fonts after QApplication (otherwise, it failed and gives wrong fonts)

    QApplication::setWindowIcon(QIcon(":/images/ceb.svg"));

    Profile::instance().load();

    for(int i = 1; i < argc; ++i)
    {
        QString arg = argv[i];

        QUrl url(arg);

        if (url.isValid() && url.scheme() == "mtpchat")
            Profile::instance().addSessionUrl(url);
    }

	MtpChatHandler *handler = new MtpChatHandler();

    QDesktopServices::setUrlHandler("mtpchat", handler, "addSession");

    MainWindow::instance()->show();

    int appRes = a.exec();

	delete handler;

    Script::closeModifiers(); // Close all lua chunks

    Profile::instance().save();
	Profile::free();

	MainWindow::free();
	TextSkin::freeDefaultSkin();

    return appRes;
}
