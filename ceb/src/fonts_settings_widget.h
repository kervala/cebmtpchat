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

#ifndef FONTS_SETTINGS_WIDGET_H
#define FONTS_SETTINGS_WIDGET_H

#include "settings_widget.h"

#include "ui_fonts_settings.h"

class FontsSettingsWidget : public SettingsWidget, private Ui::FontsSettings
{
    Q_OBJECT

public:
    FontsSettingsWidget(QWidget *parent = 0);

    void applyProfile(const Profile &profile) {}
    void feedProfile(Profile &profile) {}

private:
    static const int outputRow = 0;
    static const int inputRow = 1;
    static const int whoRow = 2;
    static const int topicRow = 3;
    static const int timeStampRow = 4;

    void fillFontWidgets(int row);
    void refreshListWidgetItems();
    TextSkin::MtpFont &getMtpFontByRow(int row);

private slots:
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_checkBoxForceFont_toggled(bool b);
    void on_checkBoxForceColor_toggled(bool b);
    void on_pushButtonForcedFont_clicked();
    void on_pushButtonForcedColor_clicked();
    void on_checkBoxForceBackgroundColor_toggled(bool b);
    void on_checkBoxForceAwayBackgroundColor_toggled(bool b);
    void on_pushButtonBackgroundColor_clicked();
    void on_pushButtonAwayBackgroundColor_clicked();
};

#endif
