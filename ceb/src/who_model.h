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

#ifndef WHO_MODEL_H
#define WHO_MODEL_H

#include "session.h"

class WhoModel : public QAbstractListModel
{
    Q_OBJECT

public:
    WhoModel(Session *session, QObject *parent = 0);

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
    Session *_session;

private slots:
    void userAdded(int userIndex);
    void userChanged(int userIndex);
    void userRemoved(int userIndex);
};

class WhoSortModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    WhoSortModel(Session *session, QObject *parent = 0) : QSortFilterProxyModel(parent),
                                                          _session(session) {}

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    Session *_session;
};

#endif
