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
#include "language_manager.h"
#include "general_settings_widget.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

GeneralSettingsWidget::GeneralSettingsWidget(QWidget *parent)
    : SettingsWidget(parent)
{
    setupUi(this);

    QStringList languages = LanguageManager::getAvailableLanguages();
    comboBoxLanguages->addItem(tr("<Default language>"));
    foreach (QString language, languages)
    {
        comboBoxLanguages->addItem(getLanguageDisplay(language));
        displayToLanguage.insert(getLanguageDisplay(language), language);
    }

    // Keep above
    checkBoxAlwaysOnTop->setVisible(false);

#ifdef Q_OS_WIN32
    checkBoxAlwaysOnTop->setVisible(true);
#endif
}

void GeneralSettingsWidget::applyProfile(const Profile &profile)
{
    checkBoxCheckForUpdate->setChecked(profile.checkForUpdate);

    // Keep above
#ifdef Q_OS_WIN32
    checkBoxAlwaysOnTop->setChecked(profile.keepAboveOtherWindows);
#endif

    if (!Profile::instance().language.isEmpty())
    {
        int index = comboBoxLanguages->findText(getLanguageDisplay(Profile::instance().language));
        if (index >= 0)
            comboBoxLanguages->setCurrentIndex(index);
    }
}

void GeneralSettingsWidget::feedProfile(Profile &profile)
{
    QString oldLanguage = profile.language;
    if (comboBoxLanguages->currentIndex() == 0)
        profile.language = "";
    else
        profile.language = displayToLanguage[comboBoxLanguages->currentText()];

    if (profile.language != oldLanguage)
        QMessageBox::warning(this, tr("Warning"), tr("You must restart CeB to apply your language changes"));
    profile.checkForUpdate = checkBoxCheckForUpdate->isChecked();
#ifdef Q_OS_WIN32
    profile.keepAboveOtherWindows = checkBoxAlwaysOnTop->isChecked();
#endif
}

QString GeneralSettingsWidget::getLanguageDisplay(const QString &language)
{
    return LanguageManager::getLanguageDisplayName(language) + " (" + language + ")";
}
