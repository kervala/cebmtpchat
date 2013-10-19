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
#include "global.h"
#include "paths.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

QString Paths::sharePath()
{
    QDir appDir(QCoreApplication::applicationDirPath());

	if (Global::devMode())
	{
		appDir.cdUp();
		appDir.cd("share");
		appDir.cd("ceb");
	}
	else if (!Global::localMode())
	{
#if defined(Q_OS_LINUX)
#ifdef SHARE_PREFIX
	    appDir.cd(SHARE_PREFIX);
#else
	    appDir.cd("/usr/local/share/ceb";
#endif
#elif defined(Q_OS_MAC)
		// application root directory
		appDir.cdUp();
		appDir.cd("Resources");
#elif defined(Q_OS_WIN) && defined(_DEBUG)
		appDir = QDir::current();

		appDir.cdUp();
		appDir.cdUp();
		appDir.cd("ceb");
		appDir.cd("share");
		appDir.cd("ceb");
#endif
	}

	return appDir.canonicalPath();
}

QString Paths::translationsPath()
{
#if defined(Q_OS_WIN) && defined(_DEBUG)
	// under Windows, when running from Visual C++, translations are located in "build" directory
	QDir appDir = QDir::current();

	// solution directory
	appDir.cdUp();
#else
    QDir appDir(sharePath());
#endif

	// translations directory
	appDir.cd("translations");
	return appDir.canonicalPath();
}

QString Paths::scriptsPath()
{
    QDir appDir(sharePath());

	// scripts directory
	appDir.cd("scripts");
	return appDir.canonicalPath();
}

QString Paths::modifiersPath()
{
    QDir appDir(sharePath());

	// modifiers directory
	appDir.cd("modifiers");
	return appDir.canonicalPath();
}

QString Paths::profilesPath()
{
    QDir appDir(sharePath());

	// profiles directory
	appDir.cd("profiles");
	return appDir.canonicalPath();
}

QString Paths::resourcesPath()
{
    QDir appDir(sharePath());

	// resources directory
	appDir.cd("resources");
	return appDir.canonicalPath();
}

QString Paths::profilePath()
{
    if (Global::devMode() || Global::localMode())
        return QDir(QCoreApplication::applicationDirPath()).filePath(qApp->applicationName());

#ifdef USE_QT5
	return QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    return QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif
}

QString Paths::logPath()
{
    if (Global::localMode())
    {
        QDir appDir(QCoreApplication::applicationDirPath());
        return QDir(appDir.absolutePath()).filePath("CeB's logs");
    }

	QString location;
#ifdef USE_QT5
	location = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
	location = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#endif

    return QDir(location).filePath("CeB's logs");
}

