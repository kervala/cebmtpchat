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

#ifndef SESSION_H
#define SESSION_H

#include <QTcpSocket>
#include <QStringList>
#include <QHostAddress>

#include "session_config.h"
#include "token_factory.h"
#include "message_item.h"

class Session : public QObject
{
    Q_OBJECT

public:
    Session(QObject *parent = 0);
    Session(const SessionConfig &config, QObject *parent = 0);
    ~Session();

    // Start the communication
    void start(const QString &address = "", int port = -1);

    // Stop the communication
    void stop();

    //! Send something
    // If <killIdle> is false, then idle time is not reset
    void send(const QString &message, bool killIdle = true);

    //! Send a command
    void sendCommand(const QString &command, bool killIdle = true);

    const SessionConfig &config() const { return *_config; }
    void setConfig(const SessionConfig &config) { *_config = config; }

    const QString &serverAddress() const { return _serverAddress; }
    quint16 serverPort() const { return _serverPort; }
    const QString &serverLogin() const { return _serverLogin; }
    void setServerLogin(const QString &newLogin) { _serverLogin = newLogin; }
    bool away() const { return _tokenFactory.away(); }
    bool isConnected() const
        { return _socket && _socket->state() == QAbstractSocket::ConnectedState; }
    bool cleanDisconnected() const { return _cleanDisconnected; }
    bool isDisconnected() const
        { return _socket && _socket->state() == QAbstractSocket::UnconnectedState; }
    bool isLogged() const;
    const QStringList &users() const { return _users; }
    const QString &channel() const { return _channel; }
    QRegExp regExpAboutMe() const;
    const QDateTime &idleStart() const { return _idleStart; }
    bool autoAwayActivated() const { return _autoAway; }
    void activateAutoAway();
    void deactivateAutoAway();
    const QList<MessageItem> myMessages() const
        { return _myMessages; }
    const TokenFactory &tokenFactory() const { return _tokenFactory; }
    QString localAddress() const { return _socket->localAddress().toString(); }
    Properties &properties() { return _properties; }
    const Properties &properties() const { return _properties; }

    int requestTicket(TokenFactory::Command command);

    // Reset the backup servers ring
    void resetBackupServers();

signals:
    void newData(const QString &data);
    void newToken(const Token &token);
    void connecting();
    void connected();
    void disconnected();
    void logged();
    void socketError(const QString &errorStr);
    void loginChanged(const QString &oldLogin, const QString &newLogin);
    void cleared();

private:
    SessionConfig *_config;
    QTcpSocket *_socket;
    bool _cleanDisconnected;
    TokenFactory _tokenFactory;
    QString _currentLine;
    QString _serverAddress;
    int _serverPort;
    QString _serverLogin;
    QStringList _users;
    QString _channel;
    QList<MessageItem> _myMessages;
    QDateTime _idleStart;
    bool _autoAway; // true when away was set automatically, false when session is considered as not auto away anymore
    Properties _properties;

    QString logPrefix();
    void logInfo(const QString &message);
    void logError(const QString &message);
    void logSuccess(const QString &message);
    void resetIdle();

private slots:
    void socketConnected();
    void socketDisconnected();
    void readyToRead();
    void manageError(QAbstractSocket::SocketError error);
    void tokenAnalyzed(const Token &token);
};

#endif
