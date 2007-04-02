#include "profile_manager.h"
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
	ActionManager &actionManager = ProfileManager::instance().currentProfile()->actionManager;

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
	ActionManager &actionManager = ProfileManager::instance().currentProfile()->actionManager;

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
	return ProfileManager::instance().currentProfile()->actionManager.actions().count();
}

