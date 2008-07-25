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

#include <QScrollArea>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QProgressBar>
#include <QFileDialog>

#include "transfers_widget.h"
#include "transfers_manager.h"

TransfersWidget::TransfersWidget(Session *session, QWidget *parent) : SessionWidget(session, parent)
{
    connect(&TransfersManager::instance(), SIGNAL(newTransferAdded(Transfer *)),
            this, SLOT(transferAdded(Transfer *)));
    connect(&TransfersManager::instance(), SIGNAL(transferStateChanged(Transfer *)),
            this, SLOT(transferStateChanged(Transfer *)));
    connect(&TransfersManager::instance(), SIGNAL(transferRemoved(Transfer *)),
            this, SLOT(transferRemoved(Transfer *)));

    createTransfersWidgets();
}

void TransfersWidget::createTransfersWidgets()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(4);
    QScrollArea *mainArea = new QScrollArea;
    mainLayout->addWidget(mainArea);
    QWidget *mainWidget = new QWidget;
    QVBoxLayout *mainAreaLayout = new QVBoxLayout(mainWidget);
    mainAreaLayout->setMargin(0);
    mainAreaLayout->setSpacing(0);

    _widgetsLayout = new QVBoxLayout;
    _widgetsLayout->setMargin(0);
    _widgetsLayout->setSpacing(0);
    mainAreaLayout->addLayout(_widgetsLayout);

    foreach (Transfer *transfer, TransfersManager::instance().getTransfers(_session))
    {
        QWidget *newWidget = createTransferWidget(transfer);
        _widgetsLayout->addWidget(newWidget);
    }

    QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    mainAreaLayout->addItem(spacerItem);

    mainArea->setWidget(mainWidget);
    mainArea->setWidgetResizable(true);

    // Clean button
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    mainLayout->addLayout(bottomLayout);
    bottomLayout->addStretch();
    QPushButton *cleanButton = new QPushButton(tr("Clean all finished transfers"));
    bottomLayout->addWidget(cleanButton);
    connect(cleanButton, SIGNAL(clicked()),
            this, SLOT(cleanFinishedTransfers()));
}

void TransfersWidget::transferAdded(Transfer *transfer)
{
    if (transfer->session() != _session)
        return;

    QWidget *newWidget = createTransferWidget(transfer);
    _widgetsLayout->addWidget(newWidget);
}

QWidget *TransfersWidget::createTransferWidget(Transfer *transfer)
{
    TransferWidget *widget = new TransferWidget(transfer);

    fillWidget(widget, transfer);

    return widget;
}

void TransfersWidget::fillWidget(QWidget *widget, Transfer *transfer)
{
    switch (transfer->dir())
    {
    case Transfer::Direction_In:
    {
        QHBoxLayout *layout = new QHBoxLayout(widget);
        layout->setMargin(2);

        switch (transfer->state())
        {
        case Transfer::Initiated:
        {
            QVBoxLayout *labelLayout = new QVBoxLayout;
            labelLayout->setMargin(0);
            labelLayout->setSpacing(1);
            layout->addLayout(labelLayout);
            QLabel *label = new QLabel(tr("%1 wants to send you:").arg(transfer->nickName()));
            labelLayout->addWidget(label);
            QLabel *fileLabel = new QLabel("<b>" + transfer->baseFileName() + "</b>");
            labelLayout->addWidget(fileLabel);

            QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
            layout->addItem(spacerItem);

            TransferPushButton *pushButtonAccept = new TransferPushButton(tr("Accept"), transfer);
            layout->addWidget(pushButtonAccept);
            connect(pushButtonAccept, SIGNAL(clicked()),
                    this, SLOT(acceptFile()));
            TransferPushButton *pushButtonRefuse = new TransferPushButton(tr("Refuse"), transfer);
            layout->addWidget(pushButtonRefuse);
            connect(pushButtonRefuse, SIGNAL(clicked()),
                    this, SLOT(refuseFile()));
        }
        break;
        case Transfer::Refused:
        {
            QVBoxLayout *labelLayout = new QVBoxLayout;
            labelLayout->setMargin(0);
            labelLayout->setSpacing(1);
            layout->addLayout(labelLayout);
            QLabel *label = new QLabel(tr("You have refused the sending from %1 of:").arg(transfer->nickName()));
            QPalette palette = label->palette();
            palette.setColor(QPalette::WindowText, QColor(100, 100, 100));
            label->setPalette(palette);
            labelLayout->addWidget(label);
            QLabel *fileLabel = new QLabel("<b>" + transfer->baseFileName() + "</b>");
            fileLabel->setPalette(palette);
            labelLayout->addWidget(fileLabel);
        }
        break;
        case Transfer::Canceled:
        {
            QVBoxLayout *labelLayout = new QVBoxLayout;
            labelLayout->setMargin(0);
            labelLayout->setSpacing(1);
            layout->addLayout(labelLayout);
            QLabel *label = new QLabel(tr("%1 canceled the sending of:").arg(transfer->nickName()));
            QPalette palette = label->palette();
            palette.setColor(QPalette::WindowText, QColor(100, 100, 100));
            label->setPalette(palette);
            labelLayout->addWidget(label);
            QLabel *fileLabel = new QLabel("<b>" + transfer->baseFileName() + "</b>");
            fileLabel->setPalette(palette);
            labelLayout->addWidget(fileLabel);
        }
        break;
        case Transfer::Accepted:
        {
            // Left
            QVBoxLayout *leftLayout = new QVBoxLayout;
            leftLayout->setMargin(0);
            leftLayout->setSpacing(6);
            layout->addLayout(leftLayout);

            QLabel *label = new QLabel(tr("Downloading <b>%1</b> from <b>%2</b>").arg(transfer->baseFileName()).arg(transfer->nickName()));
            leftLayout->addWidget(label);

            QProgressBar *progressBar = new QProgressBar;
            leftLayout->addWidget(progressBar);

            QLabel *progressLabel = new QLabel(tr("456 KiB/s"));
            leftLayout->addWidget(progressLabel);

            // Right (buttons)
            QVBoxLayout *rightLayout = new QVBoxLayout;
            rightLayout->setMargin(0);
            rightLayout->setSpacing(6);
            layout->addLayout(rightLayout);
            QPushButton *pushButtonPause = new QPushButton(tr("Pause"));
            rightLayout->addWidget(pushButtonPause);
            QPushButton *pushButtonCancel = new QPushButton(tr("Cancel"));
            rightLayout->addWidget(pushButtonCancel);
        }
        break;
        default: return;
        }
    }
    break;
    case Transfer::Direction_Out:
    {
        QHBoxLayout *layout = new QHBoxLayout(widget);
        layout->setMargin(2);

        switch (transfer->state())
        {
        case Transfer::Initiated:
        {
            QVBoxLayout *labelLayout = new QVBoxLayout;
            labelLayout->setMargin(0);
            labelLayout->setSpacing(1);
            layout->addLayout(labelLayout);
            QLabel *label = new QLabel(tr("You want to send to %1:").arg(transfer->nickName()));
            labelLayout->addWidget(label);
            QLabel *fileLabel = new QLabel("<b>" + transfer->baseFileName() + "</b>");
            labelLayout->addWidget(fileLabel);

            QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
            layout->addItem(spacerItem);

            TransferPushButton *pushButtonCancel = new TransferPushButton(tr("Cancel"), transfer);
            layout->addWidget(pushButtonCancel);
            connect(pushButtonCancel, SIGNAL(clicked()),
                    this, SLOT(cancelFile()));
        }
        break;
        case Transfer::Canceled:
        {
            QVBoxLayout *labelLayout = new QVBoxLayout;
            labelLayout->setMargin(0);
            labelLayout->setSpacing(1);
            layout->addLayout(labelLayout);
            QLabel *label = new QLabel(tr("You have canceled the sending to %1 of:").arg(transfer->nickName()));
            QPalette palette = label->palette();
            palette.setColor(QPalette::WindowText, QColor(100, 100, 100));
            label->setPalette(palette);
            labelLayout->addWidget(label);
            QLabel *fileLabel = new QLabel("<b>" + transfer->baseFileName() + "</b>");
            fileLabel->setPalette(palette);
            labelLayout->addWidget(fileLabel);
        }
        break;
        case Transfer::Refused:
        {
            QVBoxLayout *labelLayout = new QVBoxLayout;
            labelLayout->setMargin(0);
            labelLayout->setSpacing(1);
            layout->addLayout(labelLayout);
            QLabel *label = new QLabel(transfer->nickName() + " have refused the sending of:");
            QPalette palette = label->palette();
            palette.setColor(QPalette::WindowText, QColor(100, 100, 100));
            label->setPalette(palette);
            labelLayout->addWidget(label);
            QLabel *fileLabel = new QLabel("<b>" + transfer->baseFileName() + "</b>");
            fileLabel->setPalette(palette);
            labelLayout->addWidget(fileLabel);
        }
        break;
        case Transfer::Accepted:
        {
            // Left
            QVBoxLayout *leftLayout = new QVBoxLayout;
            leftLayout->setMargin(0);
            leftLayout->setSpacing(6);
            layout->addLayout(leftLayout);

            QLabel *label = new QLabel(tr("Uploading <b>%1</b> to <b>%2</b>").arg(transfer->baseFileName()).arg(transfer->nickName()));
            leftLayout->addWidget(label);

            QProgressBar *progressBar = new QProgressBar;
            leftLayout->addWidget(progressBar);

            QLabel *progressLabel = new QLabel(tr("456 KiB/s"));
            leftLayout->addWidget(progressLabel);

            // Right (buttons)
            QVBoxLayout *rightLayout = new QVBoxLayout;
            rightLayout->setMargin(0);
            rightLayout->setSpacing(6);
            layout->addLayout(rightLayout);
            QPushButton *pushButtonPause = new QPushButton(tr("Pause"));
            rightLayout->addWidget(pushButtonPause);
            QPushButton *pushButtonCancel = new QPushButton(tr("Cancel"));
            rightLayout->addWidget(pushButtonCancel);
        }
        break;
        default: return;
        }
    }
    break;
    default: return;
    }

    widget->update();
}

QWidget *TransfersWidget::getWidget(Transfer *transfer)
{
    for (int i = 0; i < _widgetsLayout->count(); ++i)
    {
        QLayoutItem *item = _widgetsLayout->itemAt(i);
        TransferWidget *widget = qobject_cast<TransferWidget*>(item->widget());
        if (widget && widget->transfer() == transfer)
            return widget;
        else if (widget)
            qDebug("PAS GLOP: %d", widget->transfer());

    }
    return 0;
}

void TransfersWidget::transferStateChanged(Transfer *transfer)
{
    if (transfer->session() != _session)
        return;

    TransferWidget *widget = qobject_cast<TransferWidget*>(getWidget(transfer));
    Q_ASSERT(widget);

    int insertionIndex = _widgetsLayout->indexOf(widget);
    bool insertIt = insertionIndex < _widgetsLayout->count() - 1;

    _widgetsLayout->removeWidget(widget);
    delete widget;
    widget = new TransferWidget(transfer);
    if (insertIt)
        _widgetsLayout->insertWidget(insertionIndex, widget);
    else
	_widgetsLayout->addWidget(widget);

    fillWidget(widget, transfer);
    widget->repaint();
}

void TransfersWidget::transferRemoved(Transfer *transfer)
{
    TransferWidget *widget = qobject_cast<TransferWidget*>(getWidget(transfer));
    Q_ASSERT(widget);

    _widgetsLayout->removeWidget(widget);
    delete widget;
}

void TransfersWidget::acceptFile()
{
    Transfer *transfer = qobject_cast<TransferPushButton*>(sender())->transfer();

    // Ask for a directory to save
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Choose the directory where you want to save the transfered file"));
    if (dirPath == "")
        return;

    transfer->setDstDirectory(dirPath);

    TransfersManager::instance().accept(transfer);
}

void TransfersWidget::refuseFile()
{
    TransfersManager::instance().refuse(qobject_cast<TransferPushButton*>(sender())->transfer());
}

void TransfersWidget::cancelFile()
{
    TransfersManager::instance().cancel(qobject_cast<TransferPushButton*>(sender())->transfer());
}

void TransfersWidget::cleanFinishedTransfers()
{
    TransfersManager::instance().cleanUpFinishedTransfers(_session);
}
