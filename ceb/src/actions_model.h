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

#ifndef ACTIONS_MODEL_H
#define ACTIONS_MODEL_H

#include <QAbstractListModel>

class ActionsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    static const int columnAction = 0;
    static const int columnShortcut = 1;
    static const int columnsCount = 2;

    ActionsModel(QObject *parent = 0) : QAbstractListModel(parent) {}

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData (const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
};

#endif // ACTIONS_MODEL_H
