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

#ifndef DIALOG_UPDATE_H
#define DIALOG_UPDATE_H

#include "autoupdate.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>

class DialogUpdate: public QDialog
{
    Q_OBJECT

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *bottomLayout;
    QPushButton *bUpdate;
    QPushButton *bClose;
    QLabel *labelUpdate;
    QProgressBar *progressBarUpdate;
    QLabel *labelInfo;
    AutoUpdate autoUpdate;
    QString fileName;
    QString _fileToLaunch;

public:
    DialogUpdate(QWidget *parent = 0);

    const QString &fileToLaunch() const { return _fileToLaunch; }

private slots:
    void newVersion(const QString &string);
    void update();
    void updateDataReadProgress(int done, int total);
    void updateDownloadEnd(const QString &fileName);
};

#endif
