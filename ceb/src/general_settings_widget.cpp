#include <QMessageBox>

#include "language_manager.h"

#include "general_settings_widget.h"

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
