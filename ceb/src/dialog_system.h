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

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>

class DialogSystem : public QDialog
{
    Q_OBJECT

public:
    static void init(QWidget *parent = 0);

    static DialogSystem *instance();

    void sendInfo(const QString &log);
    void sendError(const QString &log);
    void sendWarning(const QString &log);
    void sendSuccess(const QString &log);

    static void info(const QString &log);
    static void error(const QString &log);
    static void warning(const QString &log);
    static void success(const QString &log);

signals:
    void hideSystemDialog();

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private:
    static DialogSystem *_instance;
    bool firstShow;
    int savedLeft, savedTop, savedWidth, savedHeight;
    QPushButton *buttonHide;
    QTextEdit *textEditMain;

    DialogSystem(QWidget *parent = 0);
    ~DialogSystem();
};

#endif
