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
#include "links_settings_widget.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

LinksSettingsWidget::LinksSettingsWidget(QWidget *parent) : SettingsWidget(parent)
{
    setupUi(this);
}

void LinksSettingsWidget::applyProfile(const Profile &profile)
{
    if (profile.linksCustomBrowser.isEmpty())
        radioButtonDefault->setChecked(true);
    else
    {
        radioButtonForce->setChecked(true);
        lineEditCustomBrowser->setText(profile.linksCustomBrowser);
    }

    httpDownloadUrl->setText(profile.downloadUrl);
    ftpUploadUrl->setText(profile.uploadUrl);
    prefixDate->setChecked(profile.prefixDate);
}

void LinksSettingsWidget::feedProfile(Profile &profile)
{
    if (radioButtonDefault->isChecked())
        profile.linksCustomBrowser = "";
    else
        profile.linksCustomBrowser = lineEditCustomBrowser->text();

    profile.downloadUrl = httpDownloadUrl->text();
    profile.uploadUrl = ftpUploadUrl->text();
    profile.prefixDate = prefixDate->isChecked();
}

void LinksSettingsWidget::on_toolButtonCustomBrowser_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this);

    if (fileName.isEmpty())
        return;

    lineEditCustomBrowser->setText(fileName);
}
