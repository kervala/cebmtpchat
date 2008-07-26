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

#ifndef SERVER_GROUP_H
#define SERVER_GROUP_H

#include <QObject>
#include <QList>

class ServerGroup
{
public:
    ServerGroup() {}
    ServerGroup(const QString &name,
                int level,
                const QString &leader,
                const QString &description)
        : _name(name),
          _level(level),
          _leader(leader),
          _description(description) {}

    const QString &name() const { return _name; }
    void setName(const QString &value) { _name = value; }

    int level() const { return _level; }
    void setLevel(int value) { _level = value; }

    const QString &leader() const { return _leader; }
    void setLeader(const QString &value) { _leader = value; }

    const QString &description() const { return _description; }
    void setDescription(const QString &value) { _description = value; }

    bool isValid() const { return _name != ""; }

private:
    QString _name;
    int _level;
    QString _leader;
    QString _description;
};

class ServerGroups : public QObject
{
    Q_OBJECT

public:
    ServerGroups(QObject *parent = 0) : QObject(parent) {}

    const QList<ServerGroup> &groups() const { return _groups; }

    void clear();

    ServerGroup groupForName(const QString &name) const;
    int groupIndexForName(const QString &name) const;
    void addGroup(const ServerGroup &group);
    void changeGroup(const QString &name, const ServerGroup &group);
    void removeGroup(const QString &name);

signals:
    void groupAdded(int index);
    void groupChanged(int index);
    void groupRemoved(int index);

    void groupAdded(const ServerGroup &group);
    void groupChanged(const ServerGroup &oldGroup, const ServerGroup &newGroup);
    void groupRemoved(const ServerGroup &group);
    void reset();

private:
    QList<ServerGroup> _groups;
};

#endif
