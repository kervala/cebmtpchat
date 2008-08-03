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

#ifndef DIALOG_SETTINGS_H
#define DIALOG_SETTINGS_H

#include <dialog_config.h>
#include <my_tabwidget.h>

#include "token_display.h"
#include "settings_widget.h"

class DialogSettings : public DialogConfig
{
    Q_OBJECT

public:
    DialogSettings(QWidget *parent = 0);

    void reject();

protected slots:
    void checkValues();

private:
    // Sub-widgets for settings
    QList<SettingsWidget*> _settingsWidgets;

    // Connections
    QTreeWidgetItem *_itemConnections;
    QPushButton *_pushButtonNewConnection;

    // Creation functions
    void createConnectionsNodes();
    QWidget *createConnectionsWidget();

    TextSkin _oldTextSkin;

    bool verifyControlsDatas();

    // Get datas
    void getConnectionsControlsDatas(); //!< Collect all datas concerning connections
    void getControlsDatas();

    QMap<QString, QString> displayToLanguage;
    QString getLanguageDisplay(const QString &language);

    QTreeWidgetItem *addSettingsWidget(QTreeWidgetItem *father, SettingsWidget *widget,
                                       const QString &label, const QIcon &icon = QIcon());

private slots:
    void newSessionConfig();
    void removeSessionConfig();
};


#endif // DIALOG_SETTINGS_H
