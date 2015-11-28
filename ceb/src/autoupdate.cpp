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

#include "action.h"
#include "paths.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

AutoUpdate::AutoUpdate(QObject *parent) : QObject(parent)
{
	connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReply(QNetworkReply*)));
}

AutoUpdate::~AutoUpdate()
{
}

void AutoUpdate::onReply(QNetworkReply *reply)
{
	bool res = false;

	QString filename = reply->url().path();
	QByteArray data = reply->readAll();

	// check if it's not a HTML message
	if (!data.isEmpty() && data[0] != '<')
	{
		// if it's an executable, save it
		if (filename.indexOf(".exe") > -1)
		{
			// use a teporary name
			QString fileToSave = QDir::tempPath() + "/cebmtpchat_setup.exe";

			// delete previous version
			if (QFile::exists(fileToSave)) QFile::remove(fileToSave);

			QFile file(fileToSave);
		
			// create file
			if (file.open(QIODevice::WriteOnly))
			{
				// write data and check saved size is the same as data
				if (file.write(data) == data.size())
				{
					file.close();

					emit fileDownloadEnd(fileToSave);

					res = true;
				}
			}
		}
	}

	if (!res) emit fileDownloadError();

	reply->deleteLater();
}

void AutoUpdate::fileDataReadProgress(qint64 done, qint64 total)
{
	emit updateDataReadProgress((int)done, (int)total);
}

void AutoUpdate::getUpdate(const QString &url)
{
	QNetworkRequest req;
	req.setUrl(QUrl(url));

	QNetworkReply *reply = networkManager.get(req);

	connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(fileDataReadProgress(qint64, qint64)));
}
