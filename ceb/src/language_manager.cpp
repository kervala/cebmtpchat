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
#include <QDir>
#include <QFileInfo>
#include <QRegExp>

#include "language_manager.h"

bool LanguageManager::languageDisplayNameInitialized = false;
QMap<QString, QString> LanguageManager::languageDisplayNames;

QStringList LanguageManager::getAvailableLanguages()
{
    QStringList languages;
    QDir dir(QApplication::applicationDirPath());

    if (dir.cd("languages"))
    {
        QStringList nameFilters;
        nameFilters << "*.qm";
        QStringList entryList = dir.entryList(nameFilters, QDir::Files);
        for (int i = 0; i < entryList.size(); i++)
        {
            QFileInfo fileInfo(dir.absoluteFilePath(entryList.at(i)));
            QString baseName = fileInfo.baseName();
            QRegExp regExp("_(.+)$");
            if (regExp.lastIndexIn(baseName) >= 0)
                languages << regExp.cap(1);

        }

    }
    return languages;
}

QString LanguageManager::getLanguageFileName(const QString &language)
{
    QDir dir(QApplication::applicationDirPath());

    if (dir.cd("languages"))
    {
        QStringList nameFilters;
        nameFilters << "*.qm";
        QStringList entryList = dir.entryList(nameFilters, QDir::Files);
        for (int i = 0; i < entryList.size(); i++)
        {
            QString fileName = dir.absoluteFilePath(entryList.at(i));
            QString baseName = QFileInfo(fileName).baseName();
            QRegExp regExp("_(.+)$");
            if (regExp.lastIndexIn(baseName) >= 0 && regExp.cap(1) == language)
                return fileName;
        }

    }
    return "";
}

QString LanguageManager::getLanguageDisplayName(const QString &language)
{
    init();
    return languageDisplayNames[language];
}

void LanguageManager::init()
{
    if (languageDisplayNameInitialized)
        return;

    languageDisplayNameInitialized = true;
    languageDisplayNames.insert("us", "US English");
    languageDisplayNames.insert("fr", "French");
    languageDisplayNames.insert("pt-br", "Brazilian");
}
