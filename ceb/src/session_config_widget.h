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

#ifndef SESSION_CONFIG_WIDGET_H
#define SESSION_CONFIG_WIDGET_H

#include "session_config.h"
#include "ui_session_config_widget.h"

class SessionConfigWidget : public QWidget, private Ui::WidgetSessionConfig
{
    Q_OBJECT

public:
    SessionConfigWidget(QWidget *parent = 0, bool showRemoveButton = false);

    void init(const SessionConfig &config);
    bool check();
    void get(SessionConfig &config);
    void get(SessionConfig &config, QString &oldName);

    const QString &oldName() const { return m_oldName; }

private:
    QString m_oldName;
    QList<int> m_mibList;

private slots:
    void on_checkBoxFurtiveMode_toggled(bool state);

signals:
    void removeMe();
};

#endif
