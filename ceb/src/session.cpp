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
#include "system_widget.h"
#include "profile.h"
#include "token_script.h"

#include "session.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

Session::Session(QObject *parent)
    : QObject(parent),
      _socket(0)
{
}

Session::Session(const SessionConfig &config, QObject *parent) : QObject(parent)
{
    _config = config;
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
    _getGroupTicketID = -1;
}

Session::~Session()
{
    if (_socket)
    {
        stop();
        delete _socket;
    }
}

void Session::start(const QString &address, int port)
{
    // If already connected => disconnect it
    if (_socket->state() == QAbstractSocket::ConnectedState)
        stop();
    _tokenFactory.reset();

    // Get the next backup server
    BackupServer newBackupServer;

    if (_config.manageBackupServers())
        newBackupServer = _config.nextBackupServer(_serverAddress, _serverPort);

    if (address.isEmpty())
    {
        if (!newBackupServer.isNull())
            _serverAddress = newBackupServer.address();
        else
            _serverAddress = _config.address();
    }
    else
        _serverAddress = address;

    if (port == -1)
    {
        if (!newBackupServer.isNull())
            _serverPort = newBackupServer.port();
        else
            _serverPort = _config.port();
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
        // use personalized quit message
        sendCommand("quit " + config().quitMessage());
        _socket->waitForDisconnected(1000);
    } else
        _socket->disconnectFromHost();
}

QString Session::caption() const
{
    if (!_config.name().isEmpty())
        return _config.name();
    else
        return QString("%1:%2").arg(serverAddress()).arg(serverPort());
}

void Session::send(const QString &message, bool killIdle)
{
    QTextCodec *codec = QTextCodec::codecForMib(_config.encodingMib());
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
        _socket->write(_tokenFactory.serverCommand("set away off\n").toLatin1());
        _autoAway = false;
    }

    QString toSend = message;

    // Avoid TELNET command code and convert tab to space
	toSend = toSend.replace(255, QString("%1%1").arg(QChar(255))).replace('\t', ' ');

    // If message is multi-lines, split it before sending
    foreach (QString msg, _tokenFactory.split(toSend, codec))
    {
        // Ok, encode it and send!
        _socket->write(codec->fromUnicode(msg + '\n'));
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
    return QString("[%1:%2] ").arg(_config.address()).arg(_config.port());
}

void Session::logInfo(const QString &message)
{
    SystemWidget::info(logPrefix() + message);
}

void Session::logError(const QString &message)
{
    SystemWidget::error(logPrefix() + message);
}

void Session::logSuccess(const QString &message)
{
    SystemWidget::success(logPrefix() + message);
}

void Session::readyToRead()
{
    QTextCodec *codec = QTextCodec::codecForMib(_config.encodingMib());
    Q_ASSERT_X(codec, "readyToRead()", "bad codec mib!");

    char buffer[1024];
    int readCount;

    readCount = _socket->readLine(buffer, 1024);
    while (readCount > 0)
    {
        _currentLine += codec->toUnicode(buffer);

        // Telnet codes
        static const char clearChar[]  = { 27, '[', '2', 'J', 27, '[', '0', ';', '0', 'H', '\0' };
        static QString clearMsg(clearChar);
        if (_currentLine == clearMsg)
        {
            emit cleared();
            _currentLine.clear();
        }

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
    _currentLine.clear(); // Re-init the current line

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
        _whoPopulation.clear();
        break;
    case Token::WhoLine:
        _whoPopulation.addUser(WhoUser(token.arguments()[1], token.arguments()[2],
                                       token.arguments()[3], token.arguments()[4],
                                       token.arguments()[5], token.arguments()[6],
                                       token.arguments()[7]));
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
    {
        _channel = token.arguments()[1];
        WhoUser user = _whoPopulation.userForLogin(_serverLogin);
        user.setChannel(token.arguments()[1]);
        _whoPopulation.changeUser(_serverLogin, user);
    }
    break;
    case Token::YouLeaveChannel:
    {
        _channel = "Hall";
        WhoUser user = _whoPopulation.userForLogin(_serverLogin);
        user.setChannel("Hall");
        _whoPopulation.changeUser(_serverLogin, user);
    }
    break;
    case Token::YouLeave:
    case Token::YouAreKicked:
        _cleanDisconnected = true;
        break;
    case Token::GroupsBegin:
        _serverGroups.clear();
        break;
    case Token::GroupsLine:
        _serverGroups.addGroup(ServerGroup(token.arguments()[1],
                                           token.arguments()[3].toInt(),
                                           token.arguments()[2],
                                           token.arguments()[4]));
        break;
    case Token::SomeoneAway:
    case Token::SomeoneAwayWarning:
    {
        WhoUser user = _whoPopulation.userForLogin(token.arguments()[1]);
        user.setIdle("*away*");
        _whoPopulation.changeUser(token.arguments()[1], user);
    }
    break;
    case Token::SomeoneBack:
    {
        WhoUser user = _whoPopulation.userForLogin(token.arguments()[1]);
        user.setIdle("");
        _whoPopulation.changeUser(token.arguments()[1], user);
    }
    break;
    case Token::YouAway:
    {
        WhoUser user = _whoPopulation.userForLogin(_serverLogin);
        user.setIdle("*away*");
        _whoPopulation.changeUser(_serverLogin, user);
    }
    break;
    case Token::YouBack:
    {
        WhoUser user = _whoPopulation.userForLogin(_serverLogin);
        user.setIdle("");
        _whoPopulation.changeUser(_serverLogin, user);
        _autoAway = false;
    }
    break;
    case Token::SomeoneJoinChannel:
    {
        WhoUser user = _whoPopulation.userForLogin(token.arguments()[1]);
        user.setChannel(token.arguments()[2]);
        _whoPopulation.changeUser(token.arguments()[1], user);
    }
    break;
    case Token::SomeoneFadesIntoTheShadows:
    {
        WhoUser user = _whoPopulation.userForLogin(token.arguments()[1]);
        user.setChannel("");
        _whoPopulation.changeUser(token.arguments()[1], user);
    }
    break;
    case Token::SomeoneLeaveChannel:
    case Token::SomeoneAppearsFromTheShadows:
    {
        WhoUser user = _whoPopulation.userForLogin(token.arguments()[1]);
        user.setChannel(_channel);
        _whoPopulation.changeUser(token.arguments()[1], user);
    }
    break;
    case Token::UserLoginRenamed:
    {
        WhoUser user = _whoPopulation.userForLogin(token.arguments()[1]);
        user.setLogin(token.arguments()[2]);
        _whoPopulation.changeUser(token.arguments()[1], user);
    }
    break;
    case Token::SomeoneComesIn:
        _whoPopulation.addUser(WhoUser(token.arguments()[1], "", "Hall", "", "", "", ""));
        // We want to know the user group
        _getGroupTicketID = requestTicket(TokenFactory::Command_GetGroup);
        sendCommand(QString("get %1 group").arg(token.arguments()[1]));
        break;
        break;
    case Token::SomeoneLeaves:
    case Token::SomeoneDisconnects:
    case Token::SomeoneIsKicked:
    case Token::YouKickSomeone:
        _whoPopulation.removeUser(token.arguments()[1]);
        break;
    case Token::SomeoneTellsYou:
    case Token::SomeoneAsksYou:
    case Token::SomeoneReplies:
    {
        QString login = token.arguments()[1];
        emit privateConversationIncoming(login);
        _privateConversations[login] << token;
    }
    break;
    case Token::YouTellToSomeone:
    case Token::YouAskToSomeone:
    case Token::YouReply:
    {
        QString login = token.arguments()[1];
        emit privateConversationIncoming(login);
        _privateConversations[login] << token;
    }
    break;
    case Token::SomeoneGroup:
        if (_getGroupTicketID == token.ticketID() && token.ticketID() >= 0)
        {
            WhoUser user = _whoPopulation.userForLogin(token.arguments()[1]);
            user.setGroup(token.arguments()[2]);
            _whoPopulation.changeUser(token.arguments()[1], user);
            _getGroupTicketID = -1;
        }
        break;
    case Token::UnregisteredUser:
        if (_getGroupTicketID == token.ticketID() && token.ticketID() >= 0)
            _getGroupTicketID = -1;
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
    _serverAddress.clear();
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

QString Session::socketStateCaption() const
{
    switch (_socket->state())
    {
    case QAbstractSocket::UnconnectedState: return tr("Unconnected.");
    case QAbstractSocket::ConnectingState: return tr("Connecting...");
    case QAbstractSocket::ConnectedState: return tr("Ready.");
    default: break;
    }

    return "";
}
