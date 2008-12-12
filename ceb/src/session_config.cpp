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

#include <xml_handler.h>

#include "profile.h"

#include "session_config.h"

SessionConfig::SessionConfig()
{
    _port = 4000;
    _furtiveMode = false;
    _autoconnect = false;
    _broadcast = true;
    _topicHeight = 30;
    _entryHeight = 30;
    _whoWidth = 80;
}

SessionConfig::SessionConfig(const SessionConfig &config)
{
    *this = config;
}

void SessionConfig::load(const QDomElement &rootElem)
{
    _name = XmlHandler::read(rootElem, "name", "<noname>");
    _description = XmlHandler::read(rootElem, "description", "");
    _address = XmlHandler::read(rootElem, "address", "");
    _port = XmlHandler::read(rootElem, "port", 4000);
    _login = XmlHandler::read(rootElem, "login", "");
    _password = decryptPassword(XmlHandler::read(rootElem, "password", ""));
    _furtiveMode = XmlHandler::read(rootElem, "furtive_mode", false);
    _autoconnect = XmlHandler::read(rootElem, "autoconnect", false);
    _broadcast = XmlHandler::read(rootElem, "broadcast", true);
    _manageBackupServers = XmlHandler::read(rootElem, "manage_backup_servers", true);
    _encodingMib = XmlHandler::read(rootElem, "encoding_mib", 111);
    _topicHeight = XmlHandler::read(rootElem, "topic_height", 30);
    _entryHeight = XmlHandler::read(rootElem, "entry_height", 30);
    _whoWidth = XmlHandler::read(rootElem, "who_width", 80);

    // Backup servers
    QDomElement backupElem = rootElem.firstChildElement("backup_servers").firstChildElement("backup_server");
    while (!backupElem.isNull())
    {
        _backupServers << BackupServer(XmlHandler::read(backupElem, "address", ""),
                                       XmlHandler::read(backupElem, "port", 0));
        backupElem = backupElem.nextSiblingElement("backup_server");
    }

    // Persistent properties
    QDomElement propertyElem = rootElem.firstChildElement("properties").firstChildElement("property");
    while (!propertyElem.isNull())
    {
        _properties << Property(propertyElem);
        propertyElem = propertyElem.nextSiblingElement("property");
    }
}

void SessionConfig::save(QDomElement &rootElem)
{
    XmlHandler::write(rootElem, "name", _name);
    XmlHandler::write(rootElem, "description", _description);
    XmlHandler::write(rootElem, "address", _address);
    XmlHandler::write(rootElem, "port", _port);
    XmlHandler::write(rootElem, "login", _login);
    XmlHandler::write(rootElem, "password", cryptPassword(_password));
    XmlHandler::write(rootElem, "furtive_mode", _furtiveMode);
    XmlHandler::write(rootElem, "autoconnect", _autoconnect);
    XmlHandler::write(rootElem, "broadcast", _autoconnect);
    XmlHandler::write(rootElem, "manage_backup_servers", _manageBackupServers);
    XmlHandler::write(rootElem, "encoding_mib", _encodingMib);
    XmlHandler::write(rootElem, "topic_height", _topicHeight);
    XmlHandler::write(rootElem, "entry_height", _entryHeight);
    XmlHandler::write(rootElem, "who_width", _whoWidth);

    // Backup servers
    if (_backupServers.count())
    {
        QDomElement serversElem = rootElem.ownerDocument().createElement("backup_servers");
        rootElem.appendChild(serversElem);
        foreach (const BackupServer &server, _backupServers)
        {
            QDomElement elem = rootElem.ownerDocument().createElement("backup_server");
            serversElem.appendChild(elem);
            XmlHandler::write(elem, "address", server.address());
            XmlHandler::write(elem, "port", server.port());
        }
    }

    // Properties
    QDomElement propertiesElem = rootElem.ownerDocument().createElement("properties");
    foreach (const Property &property, _properties)
    {
        if (Profile::instance().persistentSessionProperties.contains(property.name(), Qt::CaseInsensitive))
        {
            QDomElement propertyElem = rootElem.ownerDocument().createElement("property");
            propertiesElem.appendChild(propertyElem);
            property.save(propertyElem);
        }
    }
    if (!propertiesElem.firstChildElement().isNull())
        rootElem.appendChild(propertiesElem);
}

SessionConfig &SessionConfig::getTemplate()
{
    SessionConfig *config = new SessionConfig;
    config->_address = "";
    config->_port = 4000;
    config->_description = "";
    config->_autoconnect = false;
    config->_broadcast = true;
    config->_manageBackupServers = true;
    config->_encodingMib = 111; // ISO-8859-15
    config->_topicHeight = 30;
    config->_entryHeight = 30;
    config->_whoWidth = 80;

    return *config;
}

BackupServer SessionConfig::nextBackupServer(const QString address, int port) const
{
    int i = 0;
    foreach (const BackupServer &server, _backupServers)
    {
        if (server.address().toUpper() == address.toUpper() && server.port() == port)
            return _backupServers[(i + 1) % _backupServers.count()];

        i++;
    }
    if (address != "" && _backupServers.count() > 0)
        return _backupServers[0];

    return BackupServer();
}

QString SessionConfig::cryptPassword(const QString &value)
{
    QString val = rot13(value);
    QString res = "";

    for(int i = 0; i < val.length(); ++i)
        res += QString("%1").arg((int)val[i].toAscii(), 2, 16);

    return "*" + res;
}

QString SessionConfig::decryptPassword(const QString &value)
{
    if (value.left(1) != "*") return value;

    QString res = "";

    for(int i = 1; i < value.length(); i+=2)
    {
        bool ok;

        QChar tmp;

        res += tmp.fromAscii(value.mid(i, 2).toInt(&ok, 16));
    }

    return rot13(res);
}

QString SessionConfig::rot13(const QString &value)
{
    // adapted from "rot13.lua" by Benjamin "ben" Legros
    QString res = "";

    const char infLo = 'a';
    const char supLo = 'z';
    const char infHi = 'A';
    const char supHi = 'Z';

    for(int i = 0; i < value.length(); ++i)
    {
        char cbyte = value[i].toAscii();

        if (cbyte >= infLo && cbyte <= supLo)
            cbyte = (cbyte - infLo + 13) % 26 + infLo;
        else if (cbyte >= infHi && cbyte <= supHi)
            cbyte = (cbyte - infHi + 13) % 26 + infHi;

        res += cbyte;
    }

    return res;
}

////////////////////////////

SessionConfigVariant::SessionConfigVariant(SessionConfig *sessionConfig) : QVariant()
{
    this->sessionConfig = sessionConfig;
}
