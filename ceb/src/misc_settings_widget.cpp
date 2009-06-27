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
