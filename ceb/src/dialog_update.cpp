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
#include "dialog_update.h"
#include "autoupdate.h"
#include "updater.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

DialogUpdate::DialogUpdate(QWidget *parent): QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint), m_total(0)
{
	m_autoUpdate = new AutoUpdate(this);
	m_updater = new Updater(this);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setMargin(4);

    // Top
    m_labelUpdate = new QLabel(tr("No newer version."));
    m_mainLayout->addWidget(m_labelUpdate);
    m_progressBarUpdate = new QProgressBar(this);
    m_progressBarUpdate->setAlignment(Qt::AlignHCenter);
    m_mainLayout->addWidget(m_progressBarUpdate);
    m_labelInfo = new QLabel("");
    QSizePolicy labelPolicy = m_labelInfo->sizePolicy();
    labelPolicy.setVerticalPolicy(QSizePolicy::Fixed);
    m_labelInfo->setSizePolicy(labelPolicy);
    m_mainLayout->addWidget(m_labelInfo);

    // Separator line
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    m_mainLayout->addWidget(line);

    // Bottom
    m_bottomLayout = new QHBoxLayout();
    m_mainLayout->addLayout(m_bottomLayout);

    // Update button
    m_bUpdate = new QPushButton(tr("&Update"), this);
    QSizePolicy policy = m_bUpdate->sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Minimum);
    policy.setVerticalPolicy(QSizePolicy::Fixed);
    m_bUpdate->setSizePolicy(policy);
    m_bUpdate->setEnabled(false);

    // Cancel button
    m_bClose = new QPushButton(tr("&Close"), this);
    policy = m_bClose->sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Minimum);
    policy.setVerticalPolicy(QSizePolicy::Fixed);
    m_bClose->setSizePolicy(policy);

    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding,
                                          QSizePolicy::Fixed);
    m_bottomLayout->addItem(spacer);
    m_bottomLayout->addWidget(m_bUpdate);
    m_bottomLayout->addWidget(m_bClose);

    resize(500, 120);

    connect(m_bUpdate, SIGNAL(clicked()), this, SLOT(update()));
    connect(m_bClose, SIGNAL(clicked()), this, SLOT(reject()));

    connect(m_updater, SIGNAL(newVersionDetected(QString, QString, uint, QString)), this, SLOT(onNewVersion(QString, QString, uint, QString)));
    connect(m_autoUpdate, SIGNAL(updateDataReadProgress(int, int)), this, SLOT(updateDataReadProgress(int, int)));
    connect(m_autoUpdate, SIGNAL(fileDownloadEnd(QString)), this, SLOT(updateDownloadEnd(QString)));
    connect(m_autoUpdate, SIGNAL(fileDownloadError()), this, SLOT(updateDownloadError()));

	m_updater->checkUpdates();

	setFixedHeight(height());
}

void DialogUpdate::onNewVersion(const QString &url, const QString &date, uint size, const QString &version)
{
    m_labelUpdate->setText(tr("New version found: %1 (%2)").arg(version).arg(date));
    QPalette palette = m_labelUpdate->palette();
    palette.setColor(QPalette::Foreground, Qt::blue);
    m_labelUpdate->setPalette(palette);

	m_fileName = url;
	m_total = size;

	m_bUpdate->setEnabled(true);
}

void DialogUpdate::update()
{
    m_bUpdate->setEnabled(false);
    m_autoUpdate->getUpdate(m_fileName);
}

void DialogUpdate::updateDataReadProgress(int done, int total)
{
	if (total <= 0) total = m_total;

    // Progress bar
    m_progressBarUpdate->setMinimum(0);
    m_progressBarUpdate->setMaximum(total);
    m_progressBarUpdate->setValue(done);

    // Label
    m_labelInfo->setText(tr("%1/%2 KiB").arg(done/1024).arg(total/1024));
}

void DialogUpdate::updateDownloadEnd(const QString &fileName)
{
    m_fileToLaunch = fileName;
    accept();
}

void DialogUpdate::updateDownloadError()
{
    m_labelInfo->setText(tr("Error"));
}
