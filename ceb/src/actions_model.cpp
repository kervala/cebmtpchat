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
#include "profile.h"
#include "actions_model.h"

int ActionsModel::columnCount(const QModelIndex &parent) const
{
    return columnsCount;
}

QVariant ActionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (section)
        {
        case columnAction: return tr("Action");
        case columnShortcut: return tr("Shortcut");
        default:;
        }
    }
    return QVariant();
}

QVariant ActionsModel::data(const QModelIndex &index, int role) const
{
    ActionManager &actionManager = Profile::instance().actionManager;

    switch(role)
    {
    case Qt::DisplayRole:
    {
        const Action &action = actionManager.actions()[index.row()];
        switch (index.column())
        {
        case columnAction: return action.caption();
        case columnShortcut: return action.keySequence().toString();
        default:;
        }
    }
    break;
    default:;
    }
    return QVariant();
}

bool ActionsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    ActionManager &actionManager = Profile::instance().actionManager;

    switch(role)
    {
    case Qt::EditRole:
    {
        Action &action = actionManager.getAction(index.row());
        switch (index.column())
        {
        case columnShortcut:
            action.setKeySequence(QKeySequence::fromString(value.toString()));
            emit dataChanged(index, index);
            return true;
        default:;
        }
    }
    break;
    default:;
    }
    return false;
}

int ActionsModel::rowCount(const QModelIndex &parent) const
{
    return Profile::instance().actionManager.actions().count();
}

