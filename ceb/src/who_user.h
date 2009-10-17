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

#ifndef WHO_USER_H
#define WHO_USER_H

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QObject>

class WhoUser
{
public:
    WhoUser() {} //!< constructs an invalid user
    WhoUser(const QString &login, const QString &group,
            const QString &channel, const QString &idle,
            const QString &onFor, const QString &client,
            const QString &fromHost)
        : _login(login), _group(group), _channel(channel),
          _idle(idle), _onFor(onFor), _client(client),
          _fromHost(fromHost) {}

    const QString &login() const { return _login; }
    void setLogin(const QString &value) { _login = value; }
    const QString &group() const { return _group; }
    void setGroup(const QString &value) { _group = value; }
    const QString &channel() const { return _channel; }
    void setChannel(const QString &value) { _channel = value; }
    const QString &idle() const { return _idle; }
    void setIdle(const QString &value) { _idle = value; }
    const QString &onFor() const { return _onFor; }
    void setOnFor(const QString &value) { _onFor = value; }
    const QString &client() const { return _client; }
    void setClient(const QString &value) { _client = value; }
    const QString &fromHost() const { return _fromHost; }
    void setFromHost(const QString &value) { _fromHost = value; }

    bool isAway() const { return !_idle.compare("*away*", Qt::CaseInsensitive); }

    bool isValid() const { return _login != ""; }

    QString propertyByName(const QString &name) const;

private:
    QString _login;
    QString _group;
    QString _channel;
    QString _idle;
    QString _onFor;
    QString _client;
    QString _fromHost;
};

class WhoPopulation : public QObject
{
    Q_OBJECT

public:
    WhoPopulation(QObject *parent = 0) : QObject(parent) {}

    const QList<WhoUser> &users() const { return _users; }

    void clear();

    WhoUser userForLogin(const QString &login) const;
    int userIndexForLogin(const QString &login) const;
    void addUser(const WhoUser &user);
    void changeUser(const QString &login, const WhoUser &user);
    void removeUser(const QString &login);

signals:
    void userAdded(int index);
    void userChanged(int index);
    void userRemoved(int index);

    void userAdded(const WhoUser &user);
    void userChanged(const WhoUser &oldUser, const WhoUser &newUser);
    void userRemoved(const WhoUser &user);
    void populationReset();

private:
    QList<WhoUser> _users;
};

#endif
