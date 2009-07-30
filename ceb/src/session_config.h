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

#ifndef SESSION_CONFIG_H
#define SESSION_CONFIG_H

#include <QVariant>
#include <QString>
#include <QStringList>
#include <QDomElement>

#include "property.h"

class BackupServer
{
public:
    BackupServer() { _address = ""; _port = 0; }
    BackupServer(const QString &address, int port) { _address = address; _port = port; }

    const QString &address() const { return _address; }
    int port() const { return _port; }

    QString toString() const { return _address + ":" + QString::number(_port); }

    bool isNull() const { return _address == ""; }

private:
    QString _address;
    int _port;
};

class SessionConfig
{
public:
    SessionConfig();
    SessionConfig(const SessionConfig &config);

    void load(const QDomElement &rootElem);
    void save(QDomElement &rootElem);

    QString ID() const { return QString("%1:%2:%3").arg(_address).arg(_port).arg(_login).toLower(); }

    const QString &name() const { return _name; }
    void setName(const QString &name) { _name = name; }
    const QString &description() const { return _description; }
    void setDescription(const QString &description) { _description = description; }
    const QString &address() const { return _address; }
    void setAddress(const QString &address) { _address = address; }
    int port() const { return _port; }
    void setPort(int value) { _port = value; }
    const QString &login() const { return _login; }
    void setLogin(const QString &value) { _login = value; }
    const QString &password() const { return _password; }
    void setPassword(const QString &value) { _password = value; }
    bool furtiveMode() const { return _furtiveMode; }
    void setFurtiveMode(bool value) { _furtiveMode = value; }
    bool autoconnect() const { return _autoconnect; }
    void setAutoconnect(bool value) { _autoconnect = value; }
    bool manageBackupServers() const { return _manageBackupServers; }
    void setManageBackupServers(bool value) { _manageBackupServers = value; }
    int encodingMib() const { return _encodingMib; }
    void setEncodingMib(int value) { _encodingMib = value; }
    int topicHeight() const { return _topicHeight; }
    void setTopicHeight(int value) { _topicHeight = value; }
    int entryHeight() const { return _entryHeight; }
    void setEntryHeight(int value) { _entryHeight = value; }
    int whoWidth() const { return _whoWidth; }
    void setWhoWidth(int value) { _whoWidth = value; }
    const QList<BackupServer> backupServers() const { return _backupServers; }
    void setBackupServers(const QList<BackupServer> &servers) { _backupServers = servers; }
    bool broadcast() const { return _broadcast; }
    void setBroadcast(bool value) { _broadcast = value; }
    const QString &quitMessage() const { return _quitMessage; }
    void setQuitMessage(const QString &quitMessage) { _quitMessage = quitMessage; }
    const QString &performCommands() const { return _performCommands; }
    void setPerformCommands(const QString &performCommands) { _performCommands = performCommands; }

    static QString cryptPassword(const QString &value);
    static QString decryptPassword(const QString &value);

	static QString rot13(const QString &value);

    BackupServer nextBackupServer(const QString address, int port) const;

    Properties &properties() { return _properties; }

    static SessionConfig getTemplate();

private:
    QString _name;
    QString _description;
    QString _address;
    int _port;
    QString _login;
    QString _password;
    bool _furtiveMode;
    bool _autoconnect;
    bool _broadcast;
    bool _manageBackupServers;
    QString _quitMessage;
    QString _performCommands;
    int _encodingMib; // http://www.iana.org/assignments/character-sets
    int _topicHeight;
    int _entryHeight;
    int _whoWidth;
    Properties _properties;

    QList<BackupServer> _backupServers;
};

class SessionConfigVariant : public QVariant
{
public:
    SessionConfig *sessionConfig;

    SessionConfigVariant(SessionConfig *sessionConfig);
};


#endif
