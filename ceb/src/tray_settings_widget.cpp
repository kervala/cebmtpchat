#include "tray_settings_widget.h"

TraySettingsWidget::TraySettingsWidget(QWidget *parent)
    : SettingsWidget(parent)
{
    setupUi(this);
}

void TraySettingsWidget::applyProfile(const Profile &profile)
{
    checkBoxAlwaysVisible->setChecked(profile.trayAlwaysVisible);
    checkBoxHideFromTaskBar->setChecked(profile.trayHideFromTaskBar);
    groupBoxEnabled->setChecked(profile.trayEnabled);
}

void TraySettingsWidget::feedProfile(Profile &profile)
{
    profile.trayEnabled = groupBoxEnabled->isChecked();
    profile.trayAlwaysVisible = checkBoxAlwaysVisible->isChecked();
    profile.trayHideFromTaskBar = checkBoxHideFromTaskBar->isChecked();
}
