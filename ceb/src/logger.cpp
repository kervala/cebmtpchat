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

#include <QDate>
#include <QDir>
#include <QApplication>
#include <QTextStream>
#include <QDesktopServices>

#include "logger.h"
#include "profile.h"

Logger *Logger::_instance = 0;

Logger &Logger::instance()
{
    if (!_instance)
        _instance = new Logger;
    return *_instance;
}

void Logger::free()
{
    if (_instance)
    {
        delete _instance;
        _instance = 0;
    }
}

Logger::Logger(): QObject()
{

}

Logger::~Logger()
{
    for (QMap<QString,QFile*>::iterator it = files.begin(); it != files.end(); it++)
    {
        // Close file (if opened) and destroy the file
        QFile *file = it.value();
        file->close();
        delete file;
    }
}

QString Logger::getDefaultLogsDir()
{
    return QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)).filePath("logs");
}

QString Logger::getLogsDir()
{
    if (Profile::instance().logsDefaultDir)
        return getDefaultLogsDir();
    else
    {
        QString logsDir = Profile::instance().logsDir;
        if (!logsDir.isEmpty() && logsDir[logsDir.length() - 1] != '/')
            logsDir += "/";
        return logsDir;
    }
}

QString Logger::getFileName(const QString &prefix)
{
    QString fileName = QDir(getLogsDir()).filePath(prefix);

    switch (Profile::instance().logsFilePolicy)
    {
    case Profile::LogFilePolicy_Daily:
        return fileName + " - " + getCurrentDay() + ".log";
    case Profile::LogFilePolicy_Weekly:
        return fileName + " - " + getCurrentDayWeekRange() + ".log";
    case Profile::LogFilePolicy_Monthly:
        return fileName + " - " + getCurrentDayMonthRange() + ".log";
    default:
        return fileName + ".log";
    }
}

void Logger::log(const QString &prefix, const QString &line)
{
    if (!Profile::instance().logsEnabled)
        return;

    QDir dir(getLogsDir());
    if (!dir.exists())
        if (!dir.mkpath("."))
        {
            qDebug("Impossible to create log dir: %s", qPrintable(dir.absolutePath()));
            return;
        }

    QString fileName = getFileName(prefix);

    // Is fileName in <files>?
    QMap<QString,QFile*>::iterator it = files.find(prefix);

    QFile *file;
    if (it != files.end())
    {
        file = it.value();
        // Is old filename matches the new filename
        QString oldFileName = file->fileName();

        if (oldFileName != fileName)
        {
            // filename has changed => close current file and re-open
            file->close();
            delete file;
            file = new QFile(fileName);
            files.insert(prefix, file);

            // Open the file
            QIODevice::OpenMode openMode;
            if (file->exists())
                openMode = QIODevice::Append | QIODevice::Text;
            else
                openMode = QIODevice::WriteOnly | QIODevice::Text;

            if (!file->open(openMode))
                return;
        }
    }
    else
    {
        file = new QFile(fileName);
        files.insert(prefix, file);

        // Open the file
        QIODevice::OpenMode openMode;
        if (file->exists())
            openMode = QIODevice::Append | QIODevice::Text;
        else
            openMode = QIODevice::WriteOnly | QIODevice::Text;

        if (!file->open(openMode))
            return;
    }

    // Write the line
    QTextStream logOut(file);
    logOut << line << endl;
    file->flush();
}

void Logger::logWithStarPadding(const QString &prefix, const QString &line)
{
    QString str;

    // Padd to 80 chars
    if (line.length() < 79)
    {
        for (int i = 0; i < 79 - line.length() - 1; i++)
            str += "*";
        str = str + " ";
    }

    log(prefix, str + line);
}

QString Logger::getCurrentDay()
{
    return QDate::currentDate().toString("yyyy.MM.dd");
}

QString Logger::getCurrentDayWeekRange()
{
    QDate date = QDate::currentDate();
    QDate firstDate = date.addDays(1 - date.dayOfWeek());
    QDate lastDate = date.addDays(7 - date.dayOfWeek());

    return firstDate.toString("yyyy.MM.dd") + "-" +
        lastDate.toString("yyyy.MM.dd");
}

QString Logger::getCurrentDayMonthRange()
{
    QDate date = QDate::currentDate();
    QDate firstDate = date.addDays(1 - date.day());
    QDate lastDate = date.addDays(date.daysInMonth() - date.day());

    return firstDate.toString("yyyy.MM.dd") + "-" +
        lastDate.toString("yyyy.MM.dd");
}
