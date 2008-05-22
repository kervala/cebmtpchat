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

#include <QLabel>
#include <QKeyEvent>
#include <QtDebug>
#include <QMessageBox>

#include "dialog_shortcut.h"

DialogShortcut::DialogShortcut(QWidget *parent) : DialogBasic(parent)
{
    QHBoxLayout *topLayout = new QHBoxLayout;
    mainLayout->insertLayout(0, topLayout);

    topLayout->addWidget(new QLabel(tr("Shortcut:")));

    topLayout->addWidget(_pushButtonChange = new QPushButton);

    topLayout->addStretch();

    adjustSize();

    installEventFilter(this);
}

void DialogShortcut::init(const QKeySequence &keySequence)
{
    _keySequence = keySequence;

    refreshButton();
}

void DialogShortcut::refreshButton()
{
    _pushButtonChange->setText(_keySequence.toString());
}

bool DialogShortcut::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (!keyEvent->text().isEmpty())
        {
            int k1 = 0, k2 = 0, k3 = 0, k4 = 0;
            if (keyEvent->modifiers() & Qt::AltModifier)
                k1 = Qt::ALT;
            if (keyEvent->modifiers() & Qt::ControlModifier)
            {
                if (k1 == 0)
                    k1 = Qt::CTRL;
                else
                    k2 = Qt::CTRL;
            }
            if (keyEvent->modifiers() && Qt::ShiftModifier)
            {
                if (k1 == 0)
                    k1 = Qt::SHIFT;
                else if (k2 == 0)
                    k2 = Qt::SHIFT;
                else
                    k3 = Qt::SHIFT;
            }

            if (k1 == 0)
                k1 = keyEvent->key();
            else if (k2 == 0)
                k2 = keyEvent->key();
            else if (k3 == 0)
                k3 = keyEvent->key();
            else
                k4 = keyEvent->key();

            _keySequence = QKeySequence(k1, k2, k3, k4);
            refreshButton();
        }
        return true;
    }

    return false;
}
