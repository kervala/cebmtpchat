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
	m_port = 4000;
	m_furtiveMode = false;
	m_autoconnect = false;
	m_entryHeight = 30;
	m_whoWidth = 80;
}

SessionConfig::SessionConfig(const SessionConfig &config)
{
	*this = config;
}

void SessionConfig::load(const QDomElement &rootElem)
{
	m_name = XmlHandler::read(rootElem, "name", "<noname>");
	m_description = XmlHandler::read(rootElem, "description", "");
	m_address = XmlHandler::read(rootElem, "address", "");
	m_port = XmlHandler::read(rootElem, "port", 4000);
	m_login = XmlHandler::read(rootElem, "login", "");
	m_password = XmlHandler::read(rootElem, "password", "");
	m_furtiveMode = XmlHandler::read(rootElem, "furtive_mode", false);
	m_autoconnect = XmlHandler::read(rootElem, "autoconnect", false);
	m_manageBackupServers = XmlHandler::read(rootElem, "manage_backup_servers", true);
	m_entryHeight = XmlHandler::read(rootElem, "entry_height", 30);
	m_whoWidth = XmlHandler::read(rootElem, "who_width", 80);

	// Backup servers
	const QDomElement backupServersElem = rootElem.firstChildElement("backup_servers");
	if (!backupServersElem.isNull())
	{
		QDomElement backupElem = backupServersElem.firstChildElement("backup_server");
		while (!backupElem.isNull())
		{
			m_backupServers << BackupServer(XmlHandler::read(backupElem, "address", ""),
											XmlHandler::read(backupElem, "port", 0));
			backupElem = backupElem.nextSiblingElement("backup_server");
		}
	}
}

void SessionConfig::save(QDomElement &rootElem)
{
	XmlHandler::write(rootElem, "name", m_name);
	XmlHandler::write(rootElem, "description", m_description);
	XmlHandler::write(rootElem, "address", m_address);
	XmlHandler::write(rootElem, "port", m_port);
	XmlHandler::write(rootElem, "login", m_login);
	XmlHandler::write(rootElem, "password", m_password);
	XmlHandler::write(rootElem, "furtive_mode", m_furtiveMode);
	XmlHandler::write(rootElem, "autoconnect", m_autoconnect);
	XmlHandler::write(rootElem, "manage_backup_servers", m_manageBackupServers);
	XmlHandler::write(rootElem, "entry_height", m_entryHeight);
	XmlHandler::write(rootElem, "who_width", m_whoWidth);

	// Backup servers
	QDomElement serversElem = rootElem.ownerDocument().createElement("backup_servers");
	rootElem.appendChild(serversElem);
	foreach (const BackupServer &server, m_backupServers)
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
	config->m_address = "mtpchat.zeninc.net";
	config->m_port = 4000;
	config->m_description = QObject::tr("Official mtp server");
	config->m_autoconnect = false;
	config->m_manageBackupServers = true;
	config->m_entryHeight = 30;
	config->m_whoWidth = 80;

	// Default backup servers
	QList<BackupServer> backupServers;
	backupServers << BackupServer("mtpchat.zeninc.net", 4000);
	backupServers << BackupServer("mtpchat2.zeninc.net", 4000);	

	return *config;
}

BackupServer SessionConfig::nextBackupServer(const QString address, int port) const
{
	int i = 0;
	foreach (const BackupServer &server, m_backupServers)
		{
			if (server.address().toUpper() == address.toUpper() && server.port() == port)
				return m_backupServers[(i + 1) % m_backupServers.count()];

			i++;
		}
	if (address != "" && m_backupServers.count() > 0)
		return m_backupServers[0];

	return BackupServer();	
}

////////////////////////////

SessionConfigVariant::SessionConfigVariant(SessionConfig *sessionConfig) : QVariant()
{
	this->sessionConfig = sessionConfig;
}
