#include "actions_model.h"

ActionsModel::ActionsModel(QObject *parent) : QAbstractListModel(parent)
{
	
}

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
	return QVariant();
}

int ActionsModel::rowCount(const QModelIndex &parent) const
{
	return 0;
}

