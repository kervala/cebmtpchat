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

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QFile>

class Logger : public QObject
{
    Q_OBJECT

public:
    static Logger &instance();
    static void free();

    // Prefix will be append to the file name
    // ex.
    // log("test", ...) will gives :
    // => test - 10.25.2005.log (daily)
    // => test - 10.20.2005-10.26.2005.log (weekly)
    // => test - 10.01.2005-10.31.2005.log (monthly)
    void log(const QString &prefix, const QString &line);
    // Same as above but with star padding
    void logWithStarPadding(const QString &prefix, const QString &line);

    static QString getDefaultLogsDir();

private:
    static Logger *_instance;

    Logger();
    ~Logger();

    static QString getLogsDir();
    QString getFileName(const QString &prefix);
    QString getCurrentDay();
    QString getCurrentDayWeekRange();
    QString getCurrentDayMonthRange();

    QMap<QString,QFile*> files;
    QString oldPostFix;
};


#endif
