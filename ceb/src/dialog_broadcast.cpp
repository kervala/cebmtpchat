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

#include "session_manager.h"
#include "dialog_broadcast.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

DialogBroadcast::DialogBroadcast(QWidget *parent)
    : QDialog(parent, Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
    setWindowTitle(tr("Message broadcasting"));
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(4);

    _historyWidget = new HistoryWidget;
    layout->addWidget(_historyWidget);

    QHBoxLayout *sendLayout = new QHBoxLayout;
    layout->addLayout(sendLayout);

    QVBoxLayout *serversLayout = new QVBoxLayout;
    serversLayout->setSpacing(0);

    _signalMapper = new QSignalMapper(this);

    QList<Session*> &sessions = SessionManager::instance().sessionsList();

    foreach (Session *session, sessions)
    {
        QCheckBox *checkbox = new QCheckBox(session->caption());
        checkbox->setChecked(session->config().broadcast());
        serversLayout->addWidget(checkbox);
        connect(checkbox, SIGNAL(clicked()), _signalMapper, SLOT (map()));
        _signalMapper->setMapping(checkbox, session);
    }

    sendLayout->addLayout(serversLayout);

    sendLayout->addStretch();
    QPushButton *sendButton = new QPushButton(tr("&broadcast to all servers"));
    sendLayout->addWidget(sendButton, 0, Qt::AlignBottom);
    resize(500, 60);
    connect(sendButton, SIGNAL(clicked()), this, SLOT(sendButtonClicked()));
    connect(_historyWidget, SIGNAL(textValidated(const QString&)), this, SLOT(textValidated(const QString&)));
    connect(_signalMapper, SIGNAL(mapped(QObject *)), this, SLOT(selectServer(QObject *)));
}

void DialogBroadcast::sendButtonClicked()
{
    _text = _historyWidget->toPlainText();
    accept();
}

void DialogBroadcast::textValidated(const QString &text)
{
    _text = text;
    accept();
}

void DialogBroadcast::selectServer(QObject *object)
{
    Session *session = qobject_cast<Session*>(object);
    if (!session) return;
    SessionConfig config = session->config();
    config.setBroadcast(!config.broadcast());
    session->setConfig(config);
}
