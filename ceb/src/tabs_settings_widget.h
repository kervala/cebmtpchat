#ifndef TABS_SETTINGS_WIDGET_H
#define TABS_SETTINGS_WIDGET_H

#include "settings_widget.h"

#include "ui_tabs_settings.h"

class TabsSettingsWidget : public SettingsWidget, Ui::TabsSettings
{
    Q_OBJECT

public:
    TabsSettingsWidget(QWidget *parent = 0);

    void applyProfile(const Profile &profile);
    void feedProfile(Profile &profile);
};

#endif
