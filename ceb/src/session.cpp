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

#include <QIODevice>
#include <QTextCodec>

#include "session.h"
#include "dialog_system.h"
#include "profile.h"

Session::Session(QObject *parent) : QObject(parent)
{
    m_config = 0;
    m_socket = 0;
}

Session::Session(const SessionConfig &config, QObject *parent) : QObject(parent)
{
    m_config = new SessionConfig(config);
    m_serverAddress = "";
    m_serverPort = -1;
    m_socket = new QTcpSocket(0);
    connect(m_socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(m_socket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyToRead()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(manageError(QAbstractSocket::SocketError)));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(m_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));

    connect(this, SIGNAL(newData(const QString &)), &m_analyzer, SLOT(dataReceived(const QString &)));
    connect(&m_analyzer, SIGNAL(tokenAnalyzed(const TokenEvent&)),
            this, SLOT(tokenAnalyzed(const TokenEvent &)));
    m_channel = "Hall";
    m_cleanDisconnected = false;
    m_autoAway = false;
}

Session::~Session()
{
    if (m_socket)
    {
        stop();
        delete m_socket;
    }
    if (m_config)
        delete m_config;
}

void Session::start(const QString &address, int port)
{
    // If already connected => disconnect it
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        stop();
    m_analyzer.reset();

    // Get the next backup server
    BackupServer newBackupServer;

    if (m_config->manageBackupServers())
        newBackupServer = m_config->nextBackupServer(m_serverAddress, m_serverPort);

    if (address == "")
    {
        if (!newBackupServer.isNull())
            m_serverAddress = newBackupServer.address();
        else
            m_serverAddress = m_config->address();
    }
    else
        m_serverAddress = address;

    if (port == -1)
    {
        if (!newBackupServer.isNull())
            m_serverPort = newBackupServer.port();
        else
            m_serverPort = m_config->port();
    }
    else
        m_serverPort = port;

    // Go!
    m_socket->connectToHost(m_serverAddress, m_serverPort, QIODevice::ReadWrite);
    logInfo(QObject::tr("attempt to connect..."));
    emit connecting();
    m_cleanDisconnected = false;
}

void Session::stop()
{
    if (isLogged())
    {
        send("quit");
        m_socket->waitForDisconnected(1000);
    }
    else
        m_socket->disconnectFromHost();
}

void Session::send(const QString &message, bool killIdle)
{
    QTextCodec *codec = QTextCodec::codecForMib(m_config->encodingMib());
    Q_ASSERT_X(codec, "send()", "bad codec mib!");

    if (m_socket->state() != QAbstractSocket::ConnectedState)
        return;

    // Don't sent autoaway off only for "quit"... cannot bypass aliases :/
    QRegExp r("^quit(| (.*))$");
    if (killIdle && m_autoAway && isLogged() && away() && !r.exactMatch(message) && !Profile::instance().matchIdleAwayBypassExpressions(message))
    {
        m_socket->write("set away off\n");
        m_autoAway = false;
    }

    // If message is multi-lines, split it before sending
    foreach (QString msg, m_analyzer.split(message))
    {
        // Avoid TELNET command code
        QString toSend = msg.replace(255, QString("%1%1").arg(QChar(255))) + '\n';

        // Ok, encode it and send!
        m_socket->write(codec->fromUnicode(toSend));
    }

    if (killIdle)
        resetIdle(); // Something has been sent => Idle time can be reset
}

QRegExp Session::regExpAboutMe() const
{
    return QRegExp("(^|\\W)" + m_serverLogin + "(\\W|$)", Qt::CaseInsensitive);
}

void Session::socketConnected()
{
    logSuccess("connection successful");
}

void Session::socketDisconnected()
{
    logInfo("Disconnected from peer");
}

QString Session::logPrefix()
{
    return "[" + m_config->address() + ":" + QString::number(m_config->port()) + "] ";
}

void Session::logInfo(const QString &message)
{
    DialogSystem::info(logPrefix() + message);
}

void Session::logError(const QString &message)
{
    DialogSystem::error(logPrefix() + message);
}

void Session::logSuccess(const QString &message)
{
    DialogSystem::success(logPrefix() + message);
}

void Session::readyToRead()
{
    QTextCodec *codec = QTextCodec::codecForMib(m_config->encodingMib());
    Q_ASSERT_X(codec, "readyToRead()", "bad codec mib!");

    char buffer[1024];
    int readCount;

    readCount = m_socket->readLine(buffer, 1024);
    while (readCount > 0)
    {
        m_currentLine += codec->toUnicode(buffer);
        emit newData(m_currentLine);
        readCount = m_socket->readLine(buffer, 1024);
    }
}

void Session::manageError(QAbstractSocket::SocketError error)
{
    QString str;
    switch (error) {
    case QAbstractSocket::RemoteHostClosedError:
        return;
    case QAbstractSocket::HostNotFoundError:
        str = "Host not found.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        str = "Connection refused by the peer.";
        break;
    default:
        str = "Socket error: " + m_socket->errorString();
    }

    logError(str);
    emit socketError(str);
}

void Session::tokenAnalyzed(const TokenEvent &event)
{
    m_currentLine = ""; // Re-init the current line

    switch(event.token())
    {
    case Token_IndicatedActiveServer:
        logInfo(QString("Moving to the active server (%1:%2)...").arg(event.arguments()[3]).arg(event.arguments()[4]));
        break;
    case Token_Welcome:
        m_serverLogin = event.arguments()[1];
        m_channel = "Hall";
        m_myMessages.clear();
        m_autoAway = false;
        resetIdle();
        emit logged();
        break;
    case Token_YourLoginRenamed:
    {
        QString oldLogin = m_serverLogin;
        m_serverLogin = event.arguments()[1];
        emit loginChanged(oldLogin, m_serverLogin);
    }
    break;
    case Token_WhoBegin:
        m_users.clear();
        break;
    case Token_WhoLine:
        m_users << event.arguments()[1];
        break;
    case Token_MessageBegin:
        m_myMessages.clear();
        break;
    case Token_MessageLine:
        m_myMessages << MessageItem(event.arguments()[2],
                                    event.arguments()[3],
                                    event.arguments()[4]);
        break;
    case Token_NoMessage:
        m_myMessages.clear();
        break;
    case Token_MessageReceived:
        m_myMessages << MessageItem(QString(),
                                    event.arguments()[1],
                                    event.arguments()[2]);
        break;
    case Token_AllMessagesCleared:
        m_myMessages.clear();
        break;
    case Token_MessageCleared:
        m_myMessages.removeAt(event.arguments()[1].toInt() - 1);
        break;
    case Token_MessagesCleared:
    {
        int n1 = event.arguments()[1].toInt() - 1;
        int n2 = event.arguments()[2].toInt() - 1;
        for (int i = n1; i <= n2; i++)
            m_myMessages.removeAt(n1);
    }
    break;
    case Token_YouJoinChannel:
        m_channel = event.arguments()[1];
        break;
    case Token_YouLeaveChannel:
        m_channel = "Hall";
        break;
    case Token_YouLeave:
    case Token_YouAreKicked:
        m_cleanDisconnected = true;
        break;
    case Token_YouBack:
        m_autoAway = false;
        break;
    default:;
    }

    emit newToken(event);

    // After signal propagation, we treat some special tokens...
    switch(event.token())
    {
    case Token_IndicatedActiveServer:
        stop();
        start(event.arguments()[3], event.arguments()[4].toInt());
        break;
    default:;
    }
}

int Session::requestTicket(MtpAnalyzer::Command command)
{
    return m_analyzer.requestTicket(command);
}

bool Session::isLogged() const
{
    return m_socket &&
        m_socket->state() == QAbstractSocket::ConnectedState &&
        m_analyzer.logged();
}

void Session::resetIdle()
{
    m_idleStart = QDateTime::currentDateTime();
}

void Session::resetBackupServers()
{
    m_serverAddress = "";
    m_serverPort = -1;
}

void Session::activateAutoAway()
{
    if (m_autoAway)
        return;

    if (away())
        return;

    m_autoAway = true;
    send("set away on", false);
}

void Session::deactivateAutoAway()
{
    if (!m_autoAway)
        return;

    m_autoAway = false;
}
