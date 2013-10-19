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
#include "paths.h"

#include "sound_settings_widget.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

SoundSettingsWidget::SoundSettingsWidget(QWidget *parent) : SettingsWidget(parent)
{
    setupUi(this);
}

void SoundSettingsWidget::applyProfile(const Profile &profile)
{
    // Parameters
    groupBoxBeep->setChecked(profile.soundBeepEnabled);
    if (profile.soundBeepDefault)
        radioButtonBeepDefault->setChecked(true);
    else
        radioButtonBeepCustom->setChecked(true);
    lineEditBeepCustom->setText(profile.soundBeepFileName);

    groupBoxAboutMe->setChecked(profile.soundAboutMeEnabled);
    if (profile.soundAboutMeDefault)
        radioButtonAboutMeDefault->setChecked(true);
    else
        radioButtonAboutMeCustom->setChecked(true);
    lineEditAboutMeCustom->setText(profile.soundAboutMeFileName);
}

void SoundSettingsWidget::feedProfile(Profile &profile)
{
    profile.soundBeepEnabled = groupBoxBeep->isChecked();
    profile.soundBeepDefault = radioButtonBeepDefault->isChecked();
    profile.soundBeepFileName = lineEditBeepCustom->text();

    profile.soundAboutMeEnabled = groupBoxAboutMe->isChecked();
    profile.soundAboutMeDefault = radioButtonAboutMeDefault->isChecked();
    profile.soundAboutMeFileName = lineEditAboutMeCustom->text();
}

void SoundSettingsWidget::on_pushButtonBeepDefaultPlay_clicked()
{
    QString fileName = QDir(Paths::resourcesPath()).filePath("notify.wav");

	if (QFile::exists(fileName))
    {
        QSound s(fileName);
        s.play();
    }
}

void SoundSettingsWidget::on_pushButtonBeepCustomPlay_clicked()
{
    if (QFile::exists(lineEditBeepCustom->text()))
    {
        QSound s(lineEditBeepCustom->text());
        s.play();
    }
}

void SoundSettingsWidget::on_toolButtonBeepCustom_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose a wav file"),
                                                    "", "Wav (*.wav)");
    if (!filename.isNull())
        lineEditBeepCustom->setText(filename);
}

void SoundSettingsWidget::on_pushButtonAboutMeDefaultPlay_clicked()
{
    QString fileName = QDir(Paths::resourcesPath()).filePath("notify.wav");

    if (QFile::exists(fileName))
    {
        QSound s(fileName);
        s.play();
    }
}

void SoundSettingsWidget::on_pushButtonAboutMeCustomPlay_clicked()
{
    if (QFile::exists(lineEditAboutMeCustom->text()))
    {
        QSound s(lineEditAboutMeCustom->text());
        s.play();
    }
}

void SoundSettingsWidget::on_toolButtonAboutMeCustom_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose a wav file"),
                                                    "", "Wav (*.wav)");
    if (!filename.isNull())
        lineEditAboutMeCustom->setText(filename);
}
