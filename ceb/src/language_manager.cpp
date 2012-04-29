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
#include "paths.h"

#include "language_manager.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

bool LanguageManager::languageDisplayNameInitialized = false;
QMap<QString, QString> LanguageManager::languageDisplayNames;

QStringList LanguageManager::getAvailableLanguages()
{
    QStringList languages;
    QDir languagesDir(QDir(Paths::sharePath()).filePath("translations"));

    QStringList nameFilters;
    nameFilters << "*.qm";
    QStringList entryList = languagesDir.entryList(nameFilters, QDir::Files);
    for (int i = 0; i < entryList.size(); i++)
    {
        QFileInfo fileInfo(languagesDir.filePath(entryList.at(i)));
        QString baseName = fileInfo.baseName();
        QRegExp regExp("ceb_(.+)$");
        if (regExp.lastIndexIn(baseName) >= 0)
            languages << regExp.cap(1);
    }

    return languages;
}

QStringList LanguageManager::getLanguageFileNames(const QString &language)
{
    QStringList filenames;
    QDir languagesDir(QDir(Paths::sharePath()).filePath("translations"));

#ifdef Q_OS_LINUX
    // Use system Qt translations
    filenames << QString("/usr/share/qt4/translations/qt_%1.qm").arg(language);
#else
    // Use local Qt translations
    filenames << languagesDir.filePath(QString("qt_%1.qm").arg(language));
#endif

    filenames << languagesDir.filePath(QString("ceb_%1.qm").arg(language));

    return filenames;
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
    languageDisplayNames.insert("us", QObject::tr("US English"));
    languageDisplayNames.insert("fr", QObject::tr("French"));
    languageDisplayNames.insert("de", QObject::tr("German"));
    languageDisplayNames.insert("pt-br", QObject::tr("Brazilian"));
}
