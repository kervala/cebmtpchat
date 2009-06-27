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
#include <QDesktopWidget>
#include <QApplication>

#include "warningo_settings_widget.h"

WarningoSettingsWidget::WarningoSettingsWidget(QWidget *parent) : SettingsWidget(parent)
{
    setupUi(this);
}

void WarningoSettingsWidget::applyProfile(const Profile &profile)
{
    groupBoxEnable->setChecked(profile.warningoEnabled);

    // General
    spinBoxLifeTime->setValue(profile.warningoLifeTime);

    // Location
    switch (profile.warningoLocation)
    {
    case Profile::WarningoLocation_TopLeft: radioButtonTopLeft->setChecked(true); break;
    case Profile::WarningoLocation_TopRight: radioButtonTopRight->setChecked(true); break;
    case Profile::WarningoLocation_BottomLeft: radioButtonBottomLeft->setChecked(true); break;
    case Profile::WarningoLocation_BottomRight: radioButtonBottomRight->setChecked(true); break;
    default:;
    }

    labelScreen->setVisible(qApp->desktop()->numScreens() > 1);
    comboBoxScreen->setVisible(qApp->desktop()->numScreens() > 1);

    for (int i = 0; i < qApp->desktop()->numScreens(); ++i)
        comboBoxScreen->addItem(QString::number(i + 1));

    comboBoxScreen->setCurrentIndex(profile.warningoScreen);

    // Events
    checkBoxPrivateSentences->setChecked(profile.warningoPrivate);
    checkBoxAboutMe->setChecked(profile.warningoHighlight);
}

void WarningoSettingsWidget::feedProfile(Profile &profile)
{
    profile.warningoEnabled = groupBoxEnable->isChecked();

    // General
    profile.warningoLifeTime = spinBoxLifeTime->value();

    // Location
    if (radioButtonTopLeft->isChecked())
        profile.warningoLocation = Profile::WarningoLocation_TopLeft;
    else if (radioButtonTopRight->isChecked())
        profile.warningoLocation = Profile::WarningoLocation_TopRight;
    else if (radioButtonBottomLeft->isChecked())
        profile.warningoLocation = Profile::WarningoLocation_BottomLeft;
    else if (radioButtonBottomRight->isChecked())
        profile.warningoLocation = Profile::WarningoLocation_BottomRight;

    profile.warningoScreen = comboBoxScreen->currentIndex();

    // Events
    profile.warningoPrivate = checkBoxPrivateSentences->isChecked();
    profile.warningoHighlight = checkBoxAboutMe->isChecked();
}
