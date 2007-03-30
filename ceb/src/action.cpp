#include <QObject>

#include "action.h"

Action::Action(ActionType actionType)
{
	_actionType = actionType;

	// Default QKeySequence
	switch (_actionType)
	{
	case Action_RefreshWhoColumn: _keySequence = QKeySequence(Qt::Key_F5);
	case Action_ToggleAway: _keySequence = QKeySequence(Qt::Key_F4);
	case Action_Reconnect: _keySequence = QKeySequence(Qt::Key_F9);
	default:;
	}
}

QString Action::actionToString()
{
	switch (_actionType)
	{
	case Action_RefreshWhoColumn: return QObject::tr("Refresh the who column on the current server");
	case Action_ToggleAway: return QObject::tr("Toggle the away state on the current server");
	case Action_Reconnect: return QObject::tr("Reconnect on the current server");
	default: return "";
	}
}
