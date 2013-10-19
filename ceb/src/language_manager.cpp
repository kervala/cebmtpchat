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
QMap<QString, Language> LanguageManager::languageDisplayNames;

QStringList LanguageManager::getAvailableLanguages()
{
	init();

    QStringList languages;

	foreach(const Language &lang, languageDisplayNames)
	{
        languages << lang.code;
	}

    return languages;
}

QStringList LanguageManager::getLanguageFileNames(const QString &language)
{
    init();

	QStringList filenames;

	if (language.isEmpty()) return filenames;

    QDir languagesDir(Paths::translationsPath());

#ifdef Q_OS_LINUX
#ifdef USE_QT5
	int qtVersion = 5;
#else
	int qtVersion = 4;
#endif
    // Use system Qt translations
    filenames << QString("/usr/share/qt%1/translations/qt_%2.qm").arg(qtVersion).arg(language);
#else
    // Use local Qt translations
    filenames << languagesDir.filePath(QString("qt_%1.qm").arg(language));
#endif

	filenames << languageDisplayNames[language].filename;

    return filenames;
}

QString LanguageManager::getLanguageDisplayName(const QString &language)
{
    init();
	return languageDisplayNames[language].displayName;
}

void LanguageManager::init()
{
    if (languageDisplayNameInitialized)
        return;

    languageDisplayNameInitialized = true;

	{
		// add english as default language
		Language lang;
		lang.displayName = "English";
		languageDisplayNames[""] = lang;
	}

	QDir languagesDir(Paths::translationsPath());

    QStringList nameFilters;
    nameFilters << "*.qm";
    QStringList entryList = languagesDir.entryList(nameFilters, QDir::Files);

	foreach(const QString &entry, entryList)
    {
		QString filename(languagesDir.filePath(entry));

		QFileInfo fileInfo(filename);
		QString baseName = fileInfo.baseName();
		QRegExp regExp("ceb_(.+)$");
        if (regExp.lastIndexIn(baseName) >= 0)
		{
			Language lang;
			lang.filename = filename;
			lang.code = regExp.cap(1);

			QTranslator translator;
			if (translator.load(lang.filename))
			{
				lang.displayName = translator.translate("LanguageManager", "English");
			}

			languageDisplayNames[lang.code] = lang;
		}
	}
}
