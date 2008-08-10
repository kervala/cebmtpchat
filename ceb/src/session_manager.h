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

#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include "session.h"
#include "session_config.h"

#include <QList>
#include <QTimer>

class SessionManager : public QObject
{
    Q_OBJECT

public:
    static SessionManager &instance();
    static void free();
    static bool destroyed();

    Session *newSession(const SessionConfig &config);
    void removeSession(Session *session);

    QList<Session*> &sessionsList() { return _sessionsList; }

private:
    static SessionManager *_instance;

    QList<Session*> _sessionsList;
    QTimer timerAutoconnection;
    QTimer timerIdle;

    SessionManager();
    ~SessionManager();

    void clearSessionsList();

private slots:
    void newToken(const Token &token);
    void connecting();
    void connected();
    void disconnected();
    void loginChanged(const QString &oldLogin, const QString &newLogin);
    void timerAutoconnect();
    void doTimerIdle();

signals:
    void newSessionToken(Session *session, const Token &token);
    void sessionConnected(Session *session);
    void sessionConnecting(Session *session);
    void sessionDisconnected(Session *session);
    void sessionLoginChanged(Session *session, const QString &oldLogin, const QString &newLogin);
};

#endif
