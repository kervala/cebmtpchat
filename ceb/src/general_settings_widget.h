#ifndef GENERAL_SETTINGS_WIDGET_H
#define GENERAL_SETTINGS_WIDGET_H

#include "settings_widget.h"

#include "ui_general_settings.h"

class GeneralSettingsWidget : public SettingsWidget, private Ui::GeneralSettings
{
    Q_OBJECT

public:
    GeneralSettingsWidget(QWidget *parent = 0);

    void applyProfile(const Profile &profile);
    void feedProfile(Profile &profile);

private:
    QMap<QString, QString> displayToLanguage;

    QString getLanguageDisplay(const QString &language);
};

#endif
