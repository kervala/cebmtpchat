#ifndef TRAY_SETTINGS_WIDGET_H
#define TRAY_SETTINGS_WIDGET_H

#include "settings_widget.h"

#include "ui_tray_settings.h"

class TraySettingsWidget : public SettingsWidget, private Ui::TraySettings
{
    Q_OBJECT

public:
    TraySettingsWidget(QWidget *parent = 0);

    void applyProfile(const Profile &profile);
    void feedProfile(Profile &profile);
};

#endif
