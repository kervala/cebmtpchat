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

#include "autoupdate.h"
#include "version.h"

#include <QDir>

AutoUpdate::AutoUpdate(QObject *parent) : QObject(parent)
{
	fileDownload = 0;
	siteUrl = "cebmtp.free.fr";
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
	if (done == total && total == 8)
	{
		char *version = new char[total + 1];
		version[total] = '\0';
		httpCheck.read(version, total);
		bool b;
		int oldVer = QString(VERSION).toInt(&b);
		int ver = QString(version).toInt(&b);
		if (b && ver > oldVer)
		{
			QDate date = QDate::fromString(QString(version), "yyyyMMdd");
			emit newVersion(date);
		}
	}
}

void AutoUpdate::fileDataReadProgress(int done, int total)
{
	emit updateDataReadProgress(done, total);
}

void AutoUpdate::checkForUpdate()
{
	version = "";
	httpCheck.setHost(siteUrl);
	httpCheck.get("/ceb.vrn");
}

void AutoUpdate::getUpdate(const QString &fileName)
{
	this->fileName = fileName;
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
		fileDownload->close();
		emit fileDownloadEnd(fileToSave);
	}
}
