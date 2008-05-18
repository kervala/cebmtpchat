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
#include "mtp_analyzer.h"
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

    // Send something
    // If <killIdle> is false, then idle time is not reset
    void send(const QString &message, bool killIdle = true);

    const SessionConfig &config() const { return *m_config; }
    void setConfig(const SessionConfig &config) { *m_config = config; }

    const QString &serverAddress() const { return m_serverAddress; }
    quint16 serverPort() const { return m_serverPort; }
    const QString &serverLogin() const { return m_serverLogin; }
    void setServerLogin(const QString &newLogin) { m_serverLogin = newLogin; }
    bool away() const { return m_analyzer.away(); }
    bool isConnected() const
        { return m_socket && m_socket->state() == QAbstractSocket::ConnectedState; }
    bool cleanDisconnected() const { return m_cleanDisconnected; }
    bool isDisconnected() const
        { return m_socket && m_socket->state() == QAbstractSocket::UnconnectedState; }
    bool isLogged() const;
    const QStringList &users() const { return m_users; }
    const QString &channel() const { return m_channel; }
    QRegExp regExpAboutMe() const;
    const QDateTime &idleStart() const { return m_idleStart; }
    bool autoAwayActivated() const { return m_autoAway; }
    void activateAutoAway();
    void deactivateAutoAway();
    const QList<MessageItem> myMessages() const
        { return m_myMessages; }
    const MtpAnalyzer &analyzer() const { return m_analyzer; }
    QString localAddress() const { return m_socket->localAddress().toString(); }

    int requestTicket(MtpAnalyzer::Command command);

    // Reset the backup servers ring
    void resetBackupServers();

private:
    SessionConfig *m_config;
    QTcpSocket *m_socket;
    bool m_cleanDisconnected;
    MtpAnalyzer m_analyzer;
    QString m_currentLine;
    QString m_serverAddress;
    int m_serverPort;
    QString m_serverLogin;
    QStringList m_users;
    QString m_channel;
    QList<MessageItem> m_myMessages;
    QDateTime m_idleStart;
    bool m_autoAway; // true when away was set automatically, false when session is considered as not auto away anymore

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
    void tokenAnalyzed(const TokenEvent &event);

signals:
    void newData(const QString &data);
    void newToken(const TokenEvent &event);
    void connecting();
    void connected();
    void disconnected();
    void logged();
    void socketError(const QString &errorStr);
    void loginChanged(const QString &oldLogin, const QString &newLogin);
};

#endif
