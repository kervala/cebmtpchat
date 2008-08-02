#include "tabs_settings_widget.h"

TabsSettingsWidget::TabsSettingsWidget(QWidget *parent)
    : SettingsWidget(parent)
{
    setupUi(this);
}

void TabsSettingsWidget::applyProfile(const Profile &profile)
{
    comboBoxTabsPosition->setCurrentIndex(profile.tabsPosition);
}

void TabsSettingsWidget::feedProfile(Profile &profile)
{
    profile.tabsPosition = (QTabWidget::TabPosition) comboBoxTabsPosition->currentIndex();
}
