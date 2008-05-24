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

#include "session_config.h"

SessionConfig::SessionConfig()
{
    _port = 4000;
    _furtiveMode = false;
    _autoconnect = false;
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
    _password = XmlHandler::read(rootElem, "password", "");
    _furtiveMode = XmlHandler::read(rootElem, "furtive_mode", false);
    _autoconnect = XmlHandler::read(rootElem, "autoconnect", false);
    _manageBackupServers = XmlHandler::read(rootElem, "manage_backup_servers", true);
    _encodingMib = XmlHandler::read(rootElem, "encoding_mib", 111);
    _entryHeight = XmlHandler::read(rootElem, "entry_height", 30);
    _whoWidth = XmlHandler::read(rootElem, "who_width", 80);

    // Backup servers
    const QDomElement backupServersElem = rootElem.firstChildElement("backup_servers");
    if (!backupServersElem.isNull())
    {
        QDomElement backupElem = backupServersElem.firstChildElement("backup_server");
        while (!backupElem.isNull())
        {
            _backupServers << BackupServer(XmlHandler::read(backupElem, "address", ""),
                                            XmlHandler::read(backupElem, "port", 0));
            backupElem = backupElem.nextSiblingElement("backup_server");
        }
    }
}

void SessionConfig::save(QDomElement &rootElem)
{
    XmlHandler::write(rootElem, "name", _name);
    XmlHandler::write(rootElem, "description", _description);
    XmlHandler::write(rootElem, "address", _address);
    XmlHandler::write(rootElem, "port", _port);
    XmlHandler::write(rootElem, "login", _login);
    XmlHandler::write(rootElem, "password", _password);
    XmlHandler::write(rootElem, "furtive_mode", _furtiveMode);
    XmlHandler::write(rootElem, "autoconnect", _autoconnect);
    XmlHandler::write(rootElem, "manage_backup_servers", _manageBackupServers);
    XmlHandler::write(rootElem, "encoding_mib", _encodingMib);
    XmlHandler::write(rootElem, "entry_height", _entryHeight);
    XmlHandler::write(rootElem, "who_width", _whoWidth);

    // Backup servers
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

SessionConfig &SessionConfig::getTemplate()
{
    SessionConfig *config = new SessionConfig;
    config->_address = "mtpchat.melting-pot.org";
    config->_port = 4000;
    config->_description = QObject::tr("Official mtp server");
    config->_autoconnect = false;
    config->_manageBackupServers = true;
    config->_encodingMib = 111; // ISO-8859-15
    config->_entryHeight = 30;
    config->_whoWidth = 80;

    // Default backup servers
    QList<BackupServer> backupServers;
    backupServers << BackupServer("mtpchat.melting-pot.org", 4000);
    backupServers << BackupServer("mtpchat.zeninc.net", 4000);

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

////////////////////////////

SessionConfigVariant::SessionConfigVariant(SessionConfig *sessionConfig) : QVariant()
{
    this->sessionConfig = sessionConfig;
}
