#include <QObject>

#include "action.h"

Action::Action(ActionType actionType)
{
	_actionType = actionType;

	// Default QKeySequence
	switch (_actionType)
	{
	case Action_RefreshWhoColumn:
		_keySequence = QKeySequence(Qt::Key_F5);
		_caption = QObject::tr("Refresh the who column on the current server");
		break;
	case Action_ToggleAway:
		_keySequence = QKeySequence(Qt::Key_F4);
		_caption = QObject::tr("Toggle the away state on the current server");
		break;
	case Action_Reconnect:
		_keySequence = QKeySequence(Qt::Key_F9);
		_caption = QObject::tr("Reconnect on the current server");
		break;
	default:;
	}
}

// ActionManager ////////////////////////

ActionManager::ActionManager()
{
	for (int i = 0; i <= Action::Action_End; ++i)
		_actions << Action((Action::ActionType) i);
}
