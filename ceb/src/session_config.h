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
#include <QDomElement>

class BackupServer
{
public:
    BackupServer() { m_address = ""; m_port = 0; }
    BackupServer(const QString &address, int port) { m_address = address; m_port = port; }

    const QString &address() const { return m_address; }
    int port() const { return m_port; }

    QString toString() const { return m_address + ":" + QString::number(m_port); }

    bool isNull() const { return m_address == ""; }

private:
    QString m_address;
    int m_port;
};

class SessionConfig
{
public:
    SessionConfig();
    SessionConfig(const SessionConfig &config);

    void load(const QDomElement &rootElem);
    void save(QDomElement &rootElem);

    const QString &name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }
    const QString &description() const { return m_description; }
    void setDescription(const QString &description) { m_description = description; }
    const QString &address() const { return m_address; }
    void setAddress(const QString &address) { m_address = address; }
    int port() const { return m_port; }
    void setPort(int value) { m_port = value; }
    const QString &login() const { return m_login; }
    void setLogin(const QString &value) { m_login = value; }
    const QString &password() const { return m_password; }
    void setPassword(const QString &value) { m_password = value; }
    bool furtiveMode() const { return m_furtiveMode; }
    void setFurtiveMode(bool value) { m_furtiveMode = value; }
    bool autoconnect() const { return m_autoconnect; }
    void setAutoconnect(bool value) { m_autoconnect = value; }
    bool manageBackupServers() const { return m_manageBackupServers; }
    void setManageBackupServers(bool value) { m_manageBackupServers = value; }
    int encodingMib() const { return m_encodingMib; }
    void setEncodingMib(int value) { m_encodingMib = value; }
    int entryHeight() const { return m_entryHeight; }
    void setEntryHeight(int value) { m_entryHeight = value; }
    int whoWidth() const { return m_whoWidth; }
    void setWhoWidth(int value) { m_whoWidth = value; }
    const QList<BackupServer> backupServers() const { return m_backupServers; }
    void setBackupServers(const QList<BackupServer> &servers) { m_backupServers = servers; }

    BackupServer nextBackupServer(const QString address, int port) const;

    static SessionConfig &getTemplate();

private:
    QString m_name;
    QString m_description;
    QString m_address;
    int m_port;
    QString m_login;
    QString m_password;
    bool m_furtiveMode;
    bool m_autoconnect;
    bool m_manageBackupServers;
    int m_encodingMib; // http://www.iana.org/assignments/character-sets
    int m_entryHeight;
    int m_whoWidth;

    QList<BackupServer> m_backupServers;
};

class SessionConfigVariant : public QVariant
{
public:
    SessionConfig *sessionConfig;

    SessionConfigVariant(SessionConfig *sessionConfig);
};


#endif
