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

#ifndef LANGUAGE_MANAGER_H
#define LANGUAGE_MANAGER_H

struct Language
{
	QString code;
	QString displayName;
	QString filename;
};

class LanguageManager
{
private:
    static bool languageDisplayNameInitialized;
    static QMap<QString, Language> languageDisplayNames;
    static void init();
public:
    static QString getLanguageDisplayName(const QString &language);
    static QStringList getAvailableLanguages();
    static QStringList getLanguageFileNames(const QString &language);
};

#endif
