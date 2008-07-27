#ifndef MISC_SETTINGS_WIDGET_H
#define MISC_SETTINGS_WIDGET_H

#include "settings_widget.h"

#include "ui_misc_settings.h"

class MiscSettingsWidget : public SettingsWidget, private Ui::MiscSettings
{
    Q_OBJECT

public:
    MiscSettingsWidget(QWidget *parent = 0);

    void applyProfile(const Profile &profile);
    void feedProfile(Profile &profile);

private slots:
    void on_checkBoxKeepAlive_stateChanged(int state);
};

#endif
