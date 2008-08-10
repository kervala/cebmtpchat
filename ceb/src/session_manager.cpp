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

#include <QMessageBox>

#include "profile.h"
#include "session_manager.h"
#include "logger.h"

SessionManager *SessionManager::_instance = 0;

SessionManager::SessionManager() : QObject()
{
    // Autoconnection timer
    connect(&timerAutoconnection, SIGNAL(timeout()), this, SLOT(timerAutoconnect()));
    if (Profile::instance().autoconnection)
    {
        timerAutoconnection.setInterval(Profile::instance().autoconnectionDelay * 1000);
        timerAutoconnection.start();
    }

    // Timer idle
    connect(&timerIdle, SIGNAL(timeout()), this, SLOT(doTimerIdle()));
    timerIdle.setInterval(1000); // One hot minute
    timerIdle.start();
}

SessionManager::~SessionManager()
{
    clearSessionsList();
    Logger::free();
}

SessionManager &SessionManager::instance()
{
    if (!_instance)
        _instance = new SessionManager;
    return *_instance;
}

void SessionManager::free()
{
    if (_instance)
    {
        delete _instance;
        _instance = 0;
    }
}

bool SessionManager::destroyed()
{
    return _instance == 0;
}

void SessionManager::clearSessionsList()
{
    while (_sessionsList.count() > 0)
    {
        Session *session = _sessionsList.value(0);
        _sessionsList.removeAt(0);
        delete session;
    }
}

Session *SessionManager::newSession(const SessionConfig &config)
{
    Session *session = new Session(config);
    _sessionsList << session;
    connect(session, SIGNAL(connecting()), this, SLOT(connecting()));
    connect(session, SIGNAL(connected()), this, SLOT(connected()));
    connect(session, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(session, SIGNAL(newToken(const Token&)),
            this, SLOT(newToken(const Token&)));
    connect(session, SIGNAL(loginChanged(const QString &, const QString &)),
            this, SLOT(loginChanged(const QString &, const QString &)));
    connect(session, SIGNAL(privateConversationIncoming(const QString&)),
            this, SLOT(privateConversationIncoming(const QString&)));
    return session;
}

void SessionManager::removeSession(Session *session)
{
    int index = _sessionsList.indexOf(session);
    if (index >= 0)
        _sessionsList.removeAt(index);
    delete session;
}

void SessionManager::newToken(const Token &token)
{
    Session *session = qobject_cast<Session*>(sender());
    emit newSessionToken(session, token);

    // Log it
    QString line = token.line();
    if (session->away())
        line = token.timeStamp().toString("hh:mm:ss ") + line;
    else if (Profile::instance().logsTimeStamp)
        line = token.timeStamp().toString("hh:mm:ss ") + line;

    if (token.ticketID() < 0)
        Logger::instance().log(session->config().name(), line);
}

void SessionManager::connecting()
{
    Session *session = qobject_cast<Session*>(sender());
    emit sessionConnecting(session);
}

void SessionManager::connected()
{
    Session *session = qobject_cast<Session*>(sender());
    emit sessionConnected(session);

    // Log it
    QString line = tr("Connected") + " [" + QDateTime::currentDateTime().toString() + "]";
    Logger::instance().logWithStarPadding(session->config().name(), line);
}

void SessionManager::disconnected()
{
    Session *session = qobject_cast<Session*>(sender());
    emit sessionDisconnected(session);

    // Log it
    QString line = tr("Disconnected") + " [" + QDateTime::currentDateTime().toString() + "]";
    Logger::instance().logWithStarPadding(session->config().name(), line);
}

void SessionManager::loginChanged(const QString &oldLogin, const QString &newLogin)
{
    Session *session = qobject_cast<Session*>(sender());
    emit sessionLoginChanged(session, oldLogin, newLogin);
}

void SessionManager::timerAutoconnect()
{
    foreach (Session *session, _sessionsList)
    {
        if (!session->isDisconnected() || session->cleanDisconnected())
            continue;

        // Launch the connection
        session->start();
    }
}

void SessionManager::doTimerIdle()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();

    foreach (Session *session, _sessionsList)
    {
        // Compute delta
        int minutes = session->idleStart().secsTo(currentDateTime) / 60;

        // Auto-away?
        if (Profile::instance().idleAway && minutes >= Profile::instance().idleAwayTimeout && session->isLogged())
            session->activateAutoAway();

        // Auto-quit?
        if (Profile::instance().idleQuit && minutes >= Profile::instance().idleQuitTimeout && session->isLogged())
            session->sendCommand("quit CeB autoquit", false);
    }
}

void SessionManager::privateConversationIncoming(const QString &login)
{
    Session *session = qobject_cast<Session*>(sender());
    emit sessionPrivateConversationIncoming(session, login);
}
