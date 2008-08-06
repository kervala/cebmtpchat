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

#include "my_application.h"
#include "main_window.h"
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

    Profile::instance().load();

    for(int i = 1; i < argc; ++i)
    {
        QString arg = argv[i];

        QUrl url(arg);

        if (url.isValid() && url.scheme() == "mtpchat")
        {
            SessionConfig config;

            config.setName(QObject::tr("New connection"));
            config.setAddress(url.host());
            config.setPort(url.port(4000));
            config.setLogin(url.userName());
            config.setPassword(url.password());
            config.setAutoconnect(true);
            config.setEncodingMib(111);

            QList<SessionConfig *> configs = Profile::instance().sessionConfigs();

            bool exists = false;

            for (int j = 0; !exists && j < configs.count(); ++j)
                if (configs[j] && configs[j]->address() == config.address() && configs[j]->port() == config.port() && (configs[j]->login() == config.login() || config.login().isEmpty()))
                    exists = true;

            if (!exists)
                Profile::instance().addSessionConfig(config);
        }
    }

    MainWindow::instance()->show();

    int appRes = a.exec();

    Script::closeModifiers(); // Close all lua chunks

    Profile::instance().save();

    return appRes;
}
