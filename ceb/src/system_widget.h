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

#ifndef DIALOG_SYSTEM_H
#define DIALOG_SYSTEM_H

#include <QTextEdit>
#include <QPushButton>

class SystemWidget : public QWidget
{
    Q_OBJECT

public:
    static SystemWidget *instance();
    static void free();

    void sendInfo(const QString &log);
    void sendError(const QString &log);
    void sendWarning(const QString &log);
    void sendSuccess(const QString &log);

    static void info(const QString &log);
    static void error(const QString &log);
    static void warning(const QString &log);
    static void success(const QString &log);

private:
    static SystemWidget *_instance;
    QTextEdit *_textEditMain;

    SystemWidget(QWidget *parent = 0);
    ~SystemWidget();
};

#endif
