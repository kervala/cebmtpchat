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

#include <QIcon>

#include "who_model.h"
#include "event_script.h"

WhoModel::WhoModel(Session *session, QObject *parent)
    : QAbstractListModel(parent),
      _session(session)
{
    connect(&session->whoPopulation(), SIGNAL(populationReset()),
            this, SIGNAL(modelReset()));
    connect(&session->whoPopulation(), SIGNAL(userAdded(int)),
            this, SLOT(userAdded(int)));
    connect(&session->whoPopulation(), SIGNAL(userChanged(int)),
            this, SLOT(userChanged(int)));
    connect(&session->whoPopulation(), SIGNAL(userRemoved(int)),
            this, SLOT(userRemoved(int)));
}

int WhoModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int WhoModel::rowCount(const QModelIndex &parent) const
{
    return _session->whoPopulation().users().count();
}

QVariant WhoModel::data(const QModelIndex &index, int role) const
{
    const WhoUser &user = _session->whoPopulation().users()[index.row()];
    switch (role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
        case 0: return user.login();
        default:;
        }
        break;
    case Qt::DecorationRole:
        switch (index.column())
        {
        case 0:
        {
            if (user.isAway())
                return QIcon(":/images/away.png");
            else
            {
                if (user.channel() == _session->channel())
                    return QIcon(":/images/here.png");
                else
                    return QIcon(":/images/yellow-led.png");
            }
        }
        default:;
        }
        break;
    case Qt::BackgroundRole:
        return EventScript::getWhoUserBackgroundColor(_session, user);
    case Qt::ToolTipRole:
        switch (index.column())
        {
        case 0:
        {
            QString str;
            str = tr("User: %1").arg(user.login() != "" ? user.login() : tr("unregistered"));
            str += tr("\nGroup: %1").arg(user.group() != "" ? user.group() : tr("unregistered"));
            return str;
        }
        default:;
        }
        break;
    }

    return QVariant();
}

void WhoModel::userAdded(int userIndex)
{
    beginInsertRows(QModelIndex(), userIndex, userIndex);
    endInsertRows();
}

void WhoModel::userChanged(int userIndex)
{
    QModelIndex leftTop = index(userIndex, 0);
    QModelIndex bottomRight = index(userIndex, columnCount() - 1);
    emit dataChanged(leftTop, bottomRight);
}

void WhoModel::userRemoved(int userIndex)
{
    beginRemoveRows(QModelIndex(), userIndex, userIndex);
    endRemoveRows();
}

/////////////////////////////////////////////////

bool WhoSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const WhoUser &leftUser = _session->whoPopulation().users()[left.row()];
    const WhoUser &rightUser = _session->whoPopulation().users()[right.row()];

    ServerGroup leftGroup = _session->serverGroups().groupForName(leftUser.group());
    ServerGroup rightGroup = _session->serverGroups().groupForName(rightUser.group());

    // Not same levels?
    if (leftGroup.level() != rightGroup.level())
        return leftGroup.level() < rightGroup.level();

    // Same level => Group names comparison?
    if (leftUser.group() != rightUser.group())
        return _session->serverGroups().groupIndexForName(leftUser.group()) > _session->serverGroups().groupIndexForName(rightUser.group());

    // Same group => Alphanumeric comparison
    return leftUser.login() > rightUser.login();
}
