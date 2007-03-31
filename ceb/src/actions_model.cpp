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
	Profile &profile = *ProfileManager::instance().currentProfile();
	ActionManager &actionManager = profile.actionManager;

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

int ActionsModel::rowCount(const QModelIndex &parent) const
{
	return ProfileManager::instance().currentProfile()->actionManager.actions().count();
}

