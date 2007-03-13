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

#ifndef SOUND_SETTINGS_WIDGET_H
#define SOUND_SETTINGS_WIDGET_H

#include "settings_widget.h"

#include "ui_sound_settings.h"

class SoundSettingsWidget : public SettingsWidget, private Ui::SoundSettings
{
	Q_OBJECT

public:
	SoundSettingsWidget(QWidget *parent = 0);

	void applyProfile(const Profile &profile);
	void feedProfile(Profile &profile);	

private slots:
	void on_pushButtonBeepDefaultPlay_clicked();
	void on_pushButtonBeepCustomPlay_clicked();
	void on_toolButtonBeepCustom_clicked();
	void on_pushButtonAboutMeDefaultPlay_clicked();
	void on_pushButtonAboutMeCustomPlay_clicked();
	void on_toolButtonAboutMeCustom_clicked();
};

#endif
