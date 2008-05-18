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

#ifndef AUTOUPDATE_H
#define AUTOUPDATE_H

#include <QHttp>
#include <QDate>
#include <QFile>

class AutoUpdate : public QObject
{
    Q_OBJECT

private:
    QString siteUrl;
    QHttp httpCheck;
    QHttp httpFile;
    QString version;
    QString fileName;
    QString fileToSave;
    QFile *fileDownload;
    int fileDownloadID;

public:
    AutoUpdate(QObject * parent = 0);
    ~AutoUpdate();

    void checkForUpdate();
    void getUpdate(const QString &fileName);

signals:
    void newVersion(const QDate &date);
    void updateDataReadProgress(int done, int total);
    void fileDownloadEnd(const QString &fileName);

private slots:
    void checkDataReadProgress(int done, int total);
    void fileDataReadProgress(int done, int total);
    void fileRequestFinished(int id, bool error);
};

#endif
