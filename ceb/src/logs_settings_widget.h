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

#ifndef LOGS_SETTINGS_WIDGET_H
#define LOGS_SETTINGS_WIDGET_H

#include "settings_widget.h"

#include "ui_logs_settings.h"

class LogsSettingsWidget : public SettingsWidget, private Ui::LogsSettings
{
    Q_OBJECT

public:
    LogsSettingsWidget(QWidget *parent = 0);

    void applyProfile(const Profile &profile);
    void feedProfile(Profile &profile);

private:
	bool _interactive;

    static QString chooseDirectory(QWidget *parent = 0);

private slots:
    void on_toolButtonCustomDirectory_clicked();
    void on_radioButtonCustomDirectory_toggled(bool checked);
};

#endif
