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

#ifndef SHORTCUTS_SETTINGS_WIDGET_H
#define SHORTCUTS_SETTINGS_WIDGET_H

#include "settings_widget.h"
#include "actions_model.h"

#include "ui_shortcuts_settings.h"

class ShortcutsSettingsWidget : public SettingsWidget, private Ui::ShortcutsSettings
{
    Q_OBJECT

public:
    ShortcutsSettingsWidget(QWidget *parent = 0);

    void applyProfile(const Profile &profile);
    void feedProfile(Profile &profile);

private:
    ActionsModel *_model;

    void refreshShortcutSettings(const QModelIndex &index);

private slots:
    // Automatic signals
    void on_pushButtonChange_clicked();
    void on_radioButtonNone_toggled(bool checked);
    void on_radioButtonDefault_toggled(bool checked);
    void on_radioButtonCustom_toggled(bool checked);

    // Manual signals
    void treeActionFocusedChanged(const QModelIndex &current, const QModelIndex &previous);
    void actionRowChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
};

#endif // SHORTCUTS_SETTINGS_WIDGET_H
