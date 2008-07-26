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

#include "server_group.h"

///////////////////////////////////////////////////

void ServerGroups::clear()
{
    _groups.clear();
    emit reset();
}

ServerGroup ServerGroups::groupForName(const QString &name) const
{
    foreach (const ServerGroup &group, _groups)
        if (!group.name().compare(name, Qt::CaseInsensitive))
            return group;
    return ServerGroup();
}

int ServerGroups::groupIndexForName(const QString &name) const
{
    for (int i = 0; i < _groups.count(); ++i)
        if (!_groups[i].name().compare(name, Qt::CaseInsensitive))
            return i;
    return -1;
}

void ServerGroups::addGroup(const ServerGroup &group)
{
    _groups << group;
    emit groupAdded(_groups.count() - 1);
    emit groupAdded(group);
}

void ServerGroups::changeGroup(const QString &name, const ServerGroup &group)
{
    int index = groupIndexForName(name);
    if (index >= 0)
    {
        ServerGroup &oldGroup = _groups[index];
        ServerGroup old = oldGroup;
        oldGroup = group;
        emit groupChanged(index);
        emit groupChanged(old, group);
    }
}

void ServerGroups::removeGroup(const QString &name)
{
    int index = groupIndexForName(name);
    if (index < 0)
        return;

    ServerGroup group = _groups[index];
    _groups.removeAt(index);
    emit groupRemoved(index);
    emit groupRemoved(group);
}
