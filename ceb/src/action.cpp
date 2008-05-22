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
    _defaultKeySequence = _keySequence;
}

void Action::resetKeySequence()
{
    if (_defaultKeySequence == _keySequence)
        return;

    _keySequence = _defaultKeySequence;
}

// ActionManager ////////////////////////

ActionManager::ActionManager()
{
    for (int i = 0; i <= Action::Action_End; ++i)
        _actions << Action((Action::ActionType) i);
}

Action &ActionManager::getAction(int index)
{
    Q_ASSERT_X(index >= 0 && index < _actions.count(), "ActionManager::getAction()", "<index> out of limits");

    return _actions[index];
}

Action &ActionManager::getAction(Action::ActionType actionType)
{
    Q_ASSERT_X(actionType >= Action::Action_RefreshWhoColumn && actionType <= Action::Action_End, "ActionManager::getAction()", "<actionType> out of limits");

    for (int i = 0; i < _actions.count(); ++i)
    {
        Action &action = _actions[i];
        if (action.actionType() == actionType)
            return action;
    }
    return _actions[0];
}
