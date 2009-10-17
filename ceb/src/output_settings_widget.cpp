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
#include "output_settings_widget.h"

OutputSettingsWidget::OutputSettingsWidget(QWidget *parent)
    : SettingsWidget(parent)
{
    setupUi(this);

    QPalette palette = lineEditIdle->palette();
    palette.setColor(QPalette::Base, QColor(255, 255, 240));
    lineEditIdle->setPalette(palette);

    connect(spinBoxIdleLineLength, SIGNAL(valueChanged(int)),
            this, SLOT(refreshAwaySeparatorPreview()));
    connect(lineEditIdlePeriodicString, SIGNAL(textChanged(const QString &)),
            this, SLOT(refreshAwaySeparatorPreview()));
}

void OutputSettingsWidget::applyProfile(const Profile &profile)
{
    groupBoxIdleSeparatorLines->setChecked(profile.awaySeparatorLines);

    QPalette palette = lineEditIdle->palette();
    palette.setColor(QPalette::Text, profile.awaySeparatorColor);
    lineEditIdle->setPalette(palette);

    spinBoxIdleLineLength->setValue(profile.awaySeparatorLength);

    lineEditIdlePeriodicString->setText(profile.awaySeparatorPeriod);

//    refreshAwaySeparatorPreview();
}

void OutputSettingsWidget::feedProfile(Profile &profile)
{
    profile.awaySeparatorLines = groupBoxIdleSeparatorLines->isChecked();
    QPalette palette = lineEditIdle->palette();
    profile.awaySeparatorColor = palette.color(QPalette::Text);
    profile.awaySeparatorLength = spinBoxIdleLineLength->value();
    profile.awaySeparatorPeriod = lineEditIdlePeriodicString->text();
}

void OutputSettingsWidget::refreshAwaySeparatorPreview()
{
    QString period = lineEditIdlePeriodicString->text();
    lineEditIdle->setText(Profile::getAwaySeparator(period, spinBoxIdleLineLength->value()));
}

void OutputSettingsWidget::on_pushButtonIdleColor_clicked()
{
    QColor color = QColorDialog::getColor(lineEditIdle->palette().color(QPalette::Text));
    if (color.isValid())
    {
        QPalette palette = lineEditIdle->palette();
        palette.setColor(QPalette::Text, color);
        lineEditIdle->setPalette(palette);
    }
}
