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

QString Paths::sharePath()
{
    QDir appDir(QCoreApplication::applicationDirPath());

    if (Global::devMode())
        return QDir(appDir.filePath("../share/ceb")).canonicalPath();

    if (Global::localMode())
        return appDir.absolutePath();

#if defined(Q_OS_LINUX)
#ifdef SHARE_PREFIX
    return QDir(appDir.filePath(SHARE_PREFIX)).canonicalPath();
#else
    return QDir(appDir.filePath("/usr/share/ceb")).canonicalPath();
#endif
#elif defined(Q_OS_MAC)
    return QDir(appDir.filePath("../Resources")).canonicalPath();
#else
    return appDir.absolutePath();
#endif
}

QString Paths::profilePath()
{
    if (Global::devMode() || Global::localMode())
        return QDir(QCoreApplication::applicationDirPath()).filePath(qApp->applicationName());

    return QDesktopServices::storageLocation(QDesktopServices::DataLocation);
}

QString Paths::logPath()
{
    if (Global::localMode())
    {
        QDir appDir(QCoreApplication::applicationDirPath());
        return QDir(appDir.absolutePath()).filePath("CeB's logs");
    }

    return QDir(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)).filePath("CeB's logs");
}

