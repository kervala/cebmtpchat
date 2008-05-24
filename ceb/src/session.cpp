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

    connect(this, SIGNAL(newData(const QString &)), &_tokenFactory, SLOT(dataReceived(const QString &)));
    connect(&_tokenFactory, SIGNAL(newToken(const Token&)), this, SLOT(tokenAnalyzed(const Token &)));
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
    _tokenFactory.reset();

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
    foreach (QString msg, _tokenFactory.split(message))
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

void Session::tokenAnalyzed(const Token &token)
{
    m_currentLine = ""; // Re-init the current line

    switch(token.type())
    {
    case Token::IndicatedActiveServer:
        logInfo(QString("Moving to the active server (%1:%2)...").arg(token.arguments()[3]).arg(token.arguments()[4]));
        break;
    case Token::Welcome:
        m_serverLogin = token.arguments()[1];
        m_channel = "Hall";
        m_myMessages.clear();
        m_autoAway = false;
        resetIdle();
        emit logged();
        break;
    case Token::YourLoginRenamed:
    {
        QString oldLogin = m_serverLogin;
        m_serverLogin = token.arguments()[1];
        emit loginChanged(oldLogin, m_serverLogin);
    }
    break;
    case Token::WhoBegin:
        m_users.clear();
        break;
    case Token::WhoLine:
        m_users << token.arguments()[1];
        break;
    case Token::MessageBegin:
        m_myMessages.clear();
        break;
    case Token::MessageLine:
        m_myMessages << MessageItem(token.arguments()[2],
                                    token.arguments()[3],
                                    token.arguments()[4]);
        break;
    case Token::NoMessage:
        m_myMessages.clear();
        break;
    case Token::MessageReceived:
        m_myMessages << MessageItem(QString(),
                                    token.arguments()[1],
                                    token.arguments()[2]);
        break;
    case Token::AllMessagesCleared:
        m_myMessages.clear();
        break;
    case Token::MessageCleared:
        m_myMessages.removeAt(token.arguments()[1].toInt() - 1);
        break;
    case Token::MessagesCleared:
    {
        int n1 = token.arguments()[1].toInt() - 1;
        int n2 = token.arguments()[2].toInt() - 1;
        for (int i = n1; i <= n2; i++)
            m_myMessages.removeAt(n1);
    }
    break;
    case Token::YouJoinChannel:
        m_channel = token.arguments()[1];
        break;
    case Token::YouLeaveChannel:
        m_channel = "Hall";
        break;
    case Token::YouLeave:
    case Token::YouAreKicked:
        m_cleanDisconnected = true;
        break;
    case Token::YouBack:
        m_autoAway = false;
        break;
    default:;
    }

    emit newToken(token);

    // After signal propagation, we treat some special tokens...
    switch(token.type())
    {
    case Token::IndicatedActiveServer:
        stop();
        start(token.arguments()[3], token.arguments()[4].toInt());
        break;
    default:;
    }
}

int Session::requestTicket(TokenFactory::Command command)
{
    return _tokenFactory.requestTicket(command);
}

bool Session::isLogged() const
{
    return m_socket &&
        m_socket->state() == QAbstractSocket::ConnectedState &&
        _tokenFactory.logged();
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
