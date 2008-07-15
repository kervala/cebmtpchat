#include <QIcon>

#include "who_model.h"

WhoModel::WhoModel(const Session &session, QObject *parent)
    : QAbstractListModel(parent),
      _session(session)
{
    connect(&session.whoPopulation(), SIGNAL(populationReset()),
            this, SIGNAL(modelReset()));
    connect(&session.whoPopulation(), SIGNAL(userAdded(int)),
            this, SLOT(userAdded(int)));
    connect(&session.whoPopulation(), SIGNAL(userChanged(int)),
            this, SLOT(userChanged(int)));
    connect(&session.whoPopulation(), SIGNAL(userRemoved(int)),
            this, SLOT(userRemoved(int)));
}

int WhoModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int WhoModel::rowCount(const QModelIndex &parent) const
{
    return _session.whoPopulation().users().count();
}

QVariant WhoModel::data(const QModelIndex &index, int role) const
{
    const WhoUser &user = _session.whoPopulation().users()[index.row()];
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
                if (user.channel() == _session.channel())
                    return QIcon(":/images/here.png");
                else
                    return QIcon(":/images/yellow-led.png");
            }
        }
        default:;
        }
        break;
    case Qt::ToolTipRole:
        switch (index.column())
        {
        case 0:
        {
            QString str;
            str = tr("Group: %1").arg(user.group() != "" ? user.group() : tr("unregistered"));
            str += tr("\nFrom: %1").arg(user.fromHost() != "" ? user.fromHost() : tr("unknown"));
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
