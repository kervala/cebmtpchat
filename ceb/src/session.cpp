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

#include "dialog_system.h"
#include "profile.h"
#include "token_script.h"

#include "session.h"

Session::Session(QObject *parent) : QObject(parent)
{
    _config = 0;
    _socket = 0;
}

Session::Session(const SessionConfig &config, QObject *parent) : QObject(parent)
{
    _config = new SessionConfig(config);
    _serverAddress = "";
    _serverPort = -1;
    _socket = new QTcpSocket(0);
    connect(_socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(_socket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(_socket, SIGNAL(readyRead()), this, SLOT(readyToRead()));
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(manageError(QAbstractSocket::SocketError)));
    connect(_socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));

    connect(this, SIGNAL(newData(const QString &)), &_tokenFactory, SLOT(dataReceived(const QString &)));
    connect(&_tokenFactory, SIGNAL(newToken(const Token&)), this, SLOT(tokenAnalyzed(const Token &)));
    _channel = "Hall";
    _cleanDisconnected = false;
    _autoAway = false;
}

Session::~Session()
{
    if (_socket)
    {
        stop();
        delete _socket;
    }
    if (_config)
        delete _config;
}

void Session::start(const QString &address, int port)
{
    // If already connected => disconnect it
    if (_socket->state() == QAbstractSocket::ConnectedState)
        stop();
    _tokenFactory.reset();

    // Get the next backup server
    BackupServer newBackupServer;

    if (_config->manageBackupServers())
        newBackupServer = _config->nextBackupServer(_serverAddress, _serverPort);

    if (address == "")
    {
        if (!newBackupServer.isNull())
            _serverAddress = newBackupServer.address();
        else
            _serverAddress = _config->address();
    }
    else
        _serverAddress = address;

    if (port == -1)
    {
        if (!newBackupServer.isNull())
            _serverPort = newBackupServer.port();
        else
            _serverPort = _config->port();
    }
    else
        _serverPort = port;

    // Go!
    _socket->connectToHost(_serverAddress, _serverPort, QIODevice::ReadWrite);
    logInfo(QObject::tr("attempt to connect..."));
    emit connecting();
    _cleanDisconnected = false;
}

void Session::stop()
{
    if (isLogged())
    {
        sendCommand("quit");
        _socket->waitForDisconnected(1000);
    } else
        _socket->disconnectFromHost();
}

void Session::send(const QString &message, bool killIdle)
{
    QTextCodec *codec = QTextCodec::codecForMib(_config->encodingMib());
    Q_ASSERT_X(codec, "send()", "bad codec mib!");

    if (_socket->state() != QAbstractSocket::ConnectedState)
        return;

    // Don't sent autoaway off only for "quit"... cannot bypass aliases :/
    QRegExp r("^" + QRegExp::escape(_tokenFactory.serverCommand("quit")) + "(| (.*))$");
    if (killIdle && _autoAway &&
		isLogged() && away() &&
		!r.exactMatch(message) &&
		!Profile::instance().matchIdleAwayBypassExpressions(message))
    {
        _socket->write(codec->fromUnicode(_tokenFactory.serverCommand("set away off\n")));
        _autoAway = false;
    }

    // If message is multi-lines, split it before sending
    foreach (QString msg, _tokenFactory.split(message))
    {
        // Avoid TELNET command code
        QString toSend = msg.replace(255, QString("%1%1").arg(QChar(255))) + '\n';

        // Ok, encode it and send!
        _socket->write(codec->fromUnicode(toSend));
    }

    if (killIdle)
        resetIdle(); // Something has been sent => Idle time can be reset
}

void Session::sendCommand(const QString &command, bool killIdle)
{
	send(_tokenFactory.serverCommand(command), killIdle);
}

QRegExp Session::regExpAboutMe() const
{
    return QRegExp("(^|\\W)" + _serverLogin + "(\\W|$)", Qt::CaseInsensitive);
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
    return "[" + _config->address() + ":" + QString::number(_config->port()) + "] ";
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
    QTextCodec *codec = QTextCodec::codecForMib(_config->encodingMib());
    Q_ASSERT_X(codec, "readyToRead()", "bad codec mib!");

    char buffer[1024];
    int readCount;

    readCount = _socket->readLine(buffer, 1024);
    while (readCount > 0)
    {
        _currentLine += codec->toUnicode(buffer);
        emit newData(_currentLine);
        readCount = _socket->readLine(buffer, 1024);
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
        str = "Socket error: " + _socket->errorString();
    }

    logError(str);
    emit socketError(str);
}

void Session::tokenAnalyzed(const Token &token)
{
    _currentLine = ""; // Re-init the current line

    switch(token.type())
    {
    case Token::IndicatedActiveServer:
        logInfo(QString("Moving to the active server (%1:%2)...").arg(token.arguments()[3]).arg(token.arguments()[4]));
        break;
    case Token::Welcome:
        _serverLogin = token.arguments()[2];
        _channel = "Hall";
        _myMessages.clear();
        _autoAway = false;
        resetIdle();
        emit logged();
        break;
    case Token::YourLoginRenamed:
    {
        QString oldLogin = _serverLogin;
        _serverLogin = token.arguments()[1];
        emit loginChanged(oldLogin, _serverLogin);
    }
    break;
    case Token::WhoBegin:
        _users.clear();
        break;
    case Token::WhoLine:
        _users << token.arguments()[1];
        break;
    case Token::MessageBegin:
        _myMessages.clear();
        break;
    case Token::MessageLine:
        _myMessages << MessageItem(token.arguments()[2],
                                    token.arguments()[3],
                                    token.arguments()[4]);
        break;
    case Token::NoMessage:
        _myMessages.clear();
        break;
    case Token::MessageReceived:
        _myMessages << MessageItem(QString(),
                                    token.arguments()[1],
                                    token.arguments()[2]);
        break;
    case Token::AllMessagesCleared:
        _myMessages.clear();
        break;
    case Token::MessageCleared:
        _myMessages.removeAt(token.arguments()[1].toInt() - 1);
        break;
    case Token::MessagesCleared:
    {
        int n1 = token.arguments()[1].toInt() - 1;
        int n2 = token.arguments()[2].toInt() - 1;
        for (int i = n1; i <= n2; i++)
            _myMessages.removeAt(n1);
    }
    break;
    case Token::YouJoinChannel:
        _channel = token.arguments()[1];
        break;
    case Token::YouLeaveChannel:
        _channel = "Hall";
        break;
    case Token::YouLeave:
    case Token::YouAreKicked:
        _cleanDisconnected = true;
        break;
    case Token::YouBack:
        _autoAway = false;
        break;
    default:;
    }

    emit newToken(token);

    // Execute modifier
    TokenScript::executeScript(this, token);

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
    return _socket &&
        _socket->state() == QAbstractSocket::ConnectedState &&
        _tokenFactory.logged();
}

void Session::resetIdle()
{
    _idleStart = QDateTime::currentDateTime();
}

void Session::resetBackupServers()
{
    _serverAddress = "";
    _serverPort = -1;
}

void Session::activateAutoAway()
{
    if (_autoAway)
        return;

    if (away())
        return;

    _autoAway = true;
    sendCommand("set away on", false);
}

void Session::deactivateAutoAway()
{
    if (!_autoAway)
        return;

    _autoAway = false;
}
