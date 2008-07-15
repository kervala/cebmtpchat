#ifndef WHO_MODEL_H
#define WHO_MODEL_H

#include <QAbstractListModel>

#include "session.h"

class WhoModel : public QAbstractListModel
{
    Q_OBJECT

public:
    WhoModel(const Session &session, QObject *parent = 0);

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
    const Session &_session;

private slots:
    void userAdded(int userIndex);
    void userChanged(int userIndex);
    void userRemoved(int userIndex);
};

#endif
