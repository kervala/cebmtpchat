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
#include "autoupdate.h"
#include "version.h"

#include <QDir>
#include <QSettings>

#include "action.h"
#include "paths.h"

AutoUpdate::AutoUpdate(QObject *parent) : QObject(parent)
{
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, Paths::sharePath());

    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "ceb");

    settings.beginGroup("update");
    siteUrl = settings.value("site_url").toString();
    filePrefix = settings.value("file_prefix").toString();
    settings.endGroup();

    fileDownload = 0;
    connect(&httpCheck, SIGNAL(dataReadProgress(int, int)), this, SLOT(checkDataReadProgress(int, int)));
    connect(&httpFile, SIGNAL(dataReadProgress(int, int)), this, SLOT(fileDataReadProgress(int, int)));
    connect(&httpFile, SIGNAL(requestFinished(int, bool)), this, SLOT(fileRequestFinished(int, bool)));
}

AutoUpdate::~AutoUpdate()
{
    if (fileDownload)
        delete fileDownload;
}

void AutoUpdate::checkDataReadProgress(int done, int total)
{
    if (done == total && total < 21)
    {
        char version[20];
        httpCheck.read(version, total);
        QString oldVer = VERSION;
        QString newVer = QString::fromAscii(version, total).trimmed();
        if (newVer > oldVer)
        {
            emit newVersion(newVer);
        }
    }
}

void AutoUpdate::fileDataReadProgress(int done, int total)
{
    emit updateDataReadProgress(done, total);
}

void AutoUpdate::checkForUpdate()
{
    httpCheck.setHost(siteUrl);
    httpCheck.get("/ceb.vrn");
}

void AutoUpdate::getUpdate(const QString &fileName)
{
    fileToSave = QDir::tempPath() + "/" + fileName;
    fileDownload = new QFile(fileToSave);
    fileDownload->open(QIODevice::WriteOnly);
    httpFile.setHost(siteUrl);
    fileDownloadID = httpFile.get("/" + fileName, fileDownload);
}

void AutoUpdate::fileRequestFinished(int id, bool error)
{
    if (!error && fileDownloadID == id)
    {
        fileDownload->reset();
        char buffer;
        fileDownload->read(&buffer, 1);
        fileDownload->close();
        if (buffer != '<')
            emit fileDownloadEnd(fileToSave);
        else
        {
            QFile::remove(fileToSave); // error
            emit fileDownloadError();
        }
    }
}
