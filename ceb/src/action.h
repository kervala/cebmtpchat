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

#ifndef ACTION_H
#define ACTION_H

#include <QList>
#include <QKeySequence>
#include <QDomElement>

class Action
{
public:
    enum ActionType
    {
        Action_ToggleMenuBar,
        Action_ToggleStatusBar,
        Action_RefreshWhoColumn,
        Action_ToggleAway,
        Action_Reconnect,
        Action_End = Action_Reconnect
    };

    Action(ActionType actionType);

    QKeySequence keySequence() const { return _keySequence; }
    void setKeySequence(const QKeySequence &value) { _keySequence = value; }

    QKeySequence defaultKeySequence() const { return _defaultKeySequence; }
    void resetKeySequence();

    static QString name(ActionType type);
    static QString caption(ActionType type);
    QString caption() const;
    static ActionType actionByName(const QString &actionName, bool *found = 0);

    ActionType actionType() const { return _actionType; }

private:
    ActionType _actionType;
    QKeySequence _keySequence;
    QKeySequence _defaultKeySequence;
};

class ActionManager
{
public:
    ActionManager();

    const QList<Action> &actions() const { return _actions; }

    Action &getAction(int index);
    Action &getAction(Action::ActionType actionType);

    void load(const QDomElement &root);
    void save(QDomElement &root) const;

private:
    QList<Action> _actions;
};


#endif // ACTION_H
