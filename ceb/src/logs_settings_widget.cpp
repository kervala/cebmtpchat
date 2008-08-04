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

#include <QFileDialog>

#include "logger.h"

#include "logs_settings_widget.h"

LogsSettingsWidget::LogsSettingsWidget(QWidget *parent)
    : SettingsWidget(parent)
{
    setupUi(this);
    lineEditDefaultDirectory->setText(Logger::getDefaultLogsDir());
}

void LogsSettingsWidget::applyProfile(const Profile &profile)
{
    // Directory
    if (profile.logsDefaultDir)
        radioButtonDefaultDirectory->setChecked(true);
    else
        radioButtonCustomDirectory->setChecked(true);

    lineEditCustomDirectory->setText(profile.logsDir);

    // Policy
    switch (profile.logsFilePolicy)
    {
    case Profile::LogFilePolicy_Daily:
        radioButtonDaily->setChecked(true);
        break;
    case Profile::LogFilePolicy_Weekly:
        radioButtonWeekly->setChecked(true);
        break;
    case Profile::LogFilePolicy_Monthly:
        radioButtonMonthly->setChecked(true);
        break;
    case Profile::LogFilePolicy_Uniq:
        radioButtonUniq->setChecked(true);
        break;
    }

    // Miscellaneaous
    checkBoxTimestamps->setChecked(profile.logsTimeStamp);
}

void LogsSettingsWidget::feedProfile(Profile &profile)
{
    profile.logsEnabled = groupBoxLogs->isChecked();
    profile.logsDefaultDir = radioButtonDefaultDirectory->isChecked();
    profile.logsDir = lineEditCustomDirectory->text();
    if (radioButtonDaily->isChecked())
        profile.logsFilePolicy = Profile::LogFilePolicy_Daily;
    else if (radioButtonWeekly->isChecked())
        profile.logsFilePolicy = Profile::LogFilePolicy_Weekly;
    else if (radioButtonMonthly->isChecked())
        profile.logsFilePolicy = Profile::LogFilePolicy_Monthly;
    else if (radioButtonUniq->isChecked())
        profile.logsFilePolicy = Profile::LogFilePolicy_Uniq;
    profile.logsTimeStamp = checkBoxTimestamps->isChecked();
}

void LogsSettingsWidget::on_toolButtonCustomDirectory_clicked()
{
    QString dir = chooseDirectory();
    if (QDir(dir).exists())
        lineEditCustomDirectory->setText(dir);
}

QString LogsSettingsWidget::chooseDirectory()
{
    return QFileDialog::getExistingDirectory(this, tr("Choose a logs directory"));
}

void LogsSettingsWidget::on_radioButtonCustomDirectory_toggled(bool checked)
{
    if (checked && lineEditCustomDirectory->text() == "")
    {
        QString dir = chooseDirectory();
        if (QDir(dir).exists())
            lineEditCustomDirectory->setText(dir);
    }
}
