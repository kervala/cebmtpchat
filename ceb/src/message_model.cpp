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
#include "message_model.h"

MessageModel::MessageModel(const QList<MessageItem> &myMessages)
{
    _myMessages = myMessages;
}

int MessageModel::columnCount(const QModelIndex &) const
{
    return 4;
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    switch(role)
    {
    case Qt::DisplayRole:
    {
        const MessageItem &messageItem = _myMessages[index.row()];
        if (index.column() == 0)
            return QVariant(index.row() + 1);
        else if (index.column() == 1)
            return QVariant(messageItem.message());
        else if (index.column() == 2)
            return QVariant(messageItem.sender());
        else if (index.column() == 3)
        {
            if (messageItem.dateTime().isEmpty())
                return QVariant("???");
            else
                return QVariant(messageItem.dateTime());
        }
    }
    break;
    default: return QVariant();
    }
    return QVariant();
}

QModelIndex MessageModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column < 0 || column >= 4 || row < 0 || row >= rowCount(parent)) // does lazy population
        return QModelIndex();

    return createIndex(row, column, 0);
}

QModelIndex MessageModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return _myMessages.count();
    return 0;
}

QVariant MessageModel::headerData(int section, Qt::Orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (section == 0)
            return QVariant("#");
        else if (section == 1)
            return tr("Message");
        else if (section == 2)
            return tr("Author");
        else if (section == 3)
            return tr("Date");
    }
    return QVariant();
}

const QList<MessageItem> &MessageModel::myMessages() const
{
    return _myMessages;
}

void MessageModel::setMyMessages(const QList<MessageItem> &myMessages)
{
    _myMessages = myMessages;
}


void MessageModel::refreshDatas()
{
    reset();
}
