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

#include <QFrame>

#include "dialog_update.h"

DialogUpdate::DialogUpdate(QWidget *parent): QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(4);

    // Top
    labelUpdate = new QLabel("No newer version.");
    mainLayout->addWidget(labelUpdate);
    progressBarUpdate = new QProgressBar;
    progressBarUpdate->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(progressBarUpdate);
    labelInfo = new QLabel("");
    QSizePolicy labelPolicy = labelInfo->sizePolicy();
    labelPolicy.setVerticalPolicy(QSizePolicy::Fixed);
    labelInfo->setSizePolicy(labelPolicy);
    mainLayout->addWidget(labelInfo);

    // Separator line
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);

    // Bottom
    bottomLayout = new QHBoxLayout();
    mainLayout->addLayout(bottomLayout);

    // Update button
    bUpdate = new QPushButton(tr("&Update"), this);
    QSizePolicy policy = bUpdate->sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Minimum);
    policy.setVerticalPolicy(QSizePolicy::Fixed);
    bUpdate->setSizePolicy(policy);
    bUpdate->setEnabled(false);

    // Cancel button
    bClose = new QPushButton("&Close", this);
    policy = bClose->sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Minimum);
    policy.setVerticalPolicy(QSizePolicy::Fixed);
    bClose->setSizePolicy(policy);

    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding,
                                          QSizePolicy::Fixed);
    bottomLayout->addItem(spacer);
    bottomLayout->addWidget(bUpdate);
    bottomLayout->addWidget(bClose);

    resize(500, 120);

    connect(bUpdate, SIGNAL(clicked()), this, SLOT(update()));
    connect(bClose, SIGNAL(clicked()), this, SLOT(reject()));

    connect(&autoUpdate, SIGNAL(newVersion(const QDate &)), this, SLOT(newVersion(const QDate &)));
    connect(&autoUpdate, SIGNAL(updateDataReadProgress(int, int)), this, SLOT(updateDataReadProgress(int, int)));
    connect(&autoUpdate, SIGNAL(fileDownloadEnd(const QString &)), this, SLOT(updateDownloadEnd(const QString &)));
    autoUpdate.checkForUpdate();
}

void DialogUpdate::newVersion(const QDate &date)
{
    labelUpdate->setText("New version found: " + date.toString("MM/dd/yyyy"));
    QPalette palette = labelUpdate->palette();
    palette.setColor(QPalette::Foreground, Qt::blue);
    labelUpdate->setPalette(palette);
    fileName = "CeB_alpha_" + date.toString("MMddyyyy") + ".exe";
    bUpdate->setEnabled(true);
}

void DialogUpdate::update()
{
    bUpdate->setEnabled(false);
    autoUpdate.getUpdate(fileName);
}

void DialogUpdate::updateDataReadProgress(int done, int total)
{
    // Progress bar
    progressBarUpdate->setMinimum(0);
    progressBarUpdate->setMaximum(total);
    progressBarUpdate->setValue(done);

    // Label
    labelInfo->setText(QString::number(done/1024) + "/" + QString::number(total/1024) + "Kb");
}

void DialogUpdate::updateDownloadEnd(const QString &fileName)
{
    _fileToLaunch = fileName;
    accept();
}
