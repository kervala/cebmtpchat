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
#include <QLabel>
#include <QKeyEvent>
#include <QtDebug>
#include <QMessageBox>

#include "dialog_shortcut.h"

DialogShortcut::DialogShortcut(QWidget *parent)
    : DialogBasic(parent),
      _closeIt(false)
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

        if (keyEvent->key() != Qt::Key_Shift &&
            keyEvent->key() != Qt::Key_Control &&
            keyEvent->key() != Qt::Key_Meta &&
            keyEvent->key() != Qt::Key_Alt)
        {
            _keySequence = QKeySequence(keyEvent->modifiers() + keyEvent->key());
            _closeIt = true;
        }
        else
            _keySequence = QKeySequence(keyEvent->modifiers());

        refreshButton();

        return true;
    } else if (event->type() == QEvent::KeyRelease)
    {
        if (_closeIt)
            accept();
        else
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            _keySequence = QKeySequence(keyEvent->modifiers());
            refreshButton();
        }
        return true;
    }

    return false;
}
