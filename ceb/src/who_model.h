#ifndef WHO_MODEL_H
#define WHO_MODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

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
