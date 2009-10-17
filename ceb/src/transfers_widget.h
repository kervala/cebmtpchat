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

#ifndef TRANSFERS_WIDGET_H
#define TRANSFERS_WIDGET_H

#include "session_widget.h"
#include "transfers_manager.h"

class TransferWidget : public QWidget
{
    Q_OBJECT

public:
    TransferWidget(Transfer *transfer) : QWidget(0), _transfer(transfer) {}

    Transfer *transfer() { return _transfer; }

protected:
    void paintEvent(QPaintEvent *)
        {
            QPainter painter(this);
            QLinearGradient shade(0, 0, 0, height());
            shade.setColorAt(0, QColor(250, 250, 250));
            shade.setColorAt(1, QColor(220, 220, 220));
            painter.fillRect(rect(), shade);
        }

private:
    Transfer *_transfer;
};

class TransferPushButton : public QPushButton
{
    Q_OBJECT

public:
    TransferPushButton(const QString &label, Transfer *transfer) : QPushButton(label), _transfer(transfer) {}

    Transfer *transfer() const { return _transfer; }

private:
    Transfer *_transfer;
};


class TransfersWidget : public SessionWidget
{
    Q_OBJECT

public:
    TransfersWidget(Session *session, QWidget *parent = 0);

protected:
    QString widgetCaption() const { return tr("File transfers"); }

private:
    QVBoxLayout *_widgetsLayout;

    void createTransfersWidgets();
    QWidget *createTransferWidget(Transfer *transfer);
    void fillWidget(QWidget *widget, Transfer *transfer);
    QWidget *getWidget(Transfer *transfer);

private slots:
    void transferAdded(Transfer *transfer);
    void transferStateChanged(Transfer *transfer);
    void transferRemoved(Transfer *transfer);
    void acceptFile();
    void refuseFile();
    void cancelFile();
    void cleanFinishedTransfers();
};

#endif
