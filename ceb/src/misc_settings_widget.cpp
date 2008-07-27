#include "misc_settings_widget.h"

MiscSettingsWidget::MiscSettingsWidget(QWidget *parent)
    : SettingsWidget(parent)
{
    setupUi(this);
}

void MiscSettingsWidget::applyProfile(const Profile &profile)
{
    switch(profile.timeStampPolicy)
    {
    case Profile::Policy_Classic:
        radioButtonTimestampClassic->setChecked(true);
        break;
    case Profile::Policy_Always:
        radioButtonTimestampAlways->setChecked(true);
        break;
    case Profile::Policy_Never:
        radioButtonTimestampNever->setChecked(true);
        break;
    default:;
    }
    checkBoxTimestampInTells->setChecked(profile.timeStampInTellTabs);

    checkBoxKeepAlive->setChecked(profile.keepAlive);
    if (checkBoxKeepAlive->isChecked())
        spinBoxKeepAlive->setValue(profile.keepAlive);
    else
        spinBoxKeepAlive->setValue(60);

    checkBoxTabForWho->setChecked(profile.tabForWho);
    checkBoxTabForWall->setChecked(profile.tabForWall);
    checkBoxTabForFinger->setChecked(profile.tabForFinger);

    checkBoxCopyOnSelection->setChecked(profile.copyOnSelection);
}

void MiscSettingsWidget::feedProfile(Profile &profile)
{
    // Time stamp
    if (radioButtonTimestampClassic->isChecked())
        profile.timeStampPolicy = Profile::Policy_Classic;
    else if (radioButtonTimestampAlways->isChecked())
        profile.timeStampPolicy = Profile::Policy_Always;
    else if (radioButtonTimestampNever->isChecked())
        profile.timeStampPolicy = Profile::Policy_Never;

    profile.timeStampInTellTabs = checkBoxTimestampInTells->isChecked();

    // Keep alive
    if (checkBoxKeepAlive->isChecked())
        profile.keepAlive = spinBoxKeepAlive->value();
    else
        profile.keepAlive = 0;

    // Tabs for...
    profile.tabForWho = checkBoxTabForWho->isChecked();
    profile.tabForWall = checkBoxTabForWall->isChecked();
    profile.tabForFinger = checkBoxTabForFinger->isChecked();

    profile.copyOnSelection = checkBoxCopyOnSelection->isChecked();
}

void MiscSettingsWidget::on_checkBoxKeepAlive_stateChanged(int state)
{
    spinBoxKeepAlive->setEnabled(state == Qt::Checked);
    labelKeepAlive->setEnabled(state == Qt::Checked);
}
