#ifndef ACTION_H
#define ACTION_H

#include <QList>
#include <QKeySequence>

class Action
{
public:
	enum ActionType
	{
		Action_RefreshWhoColumn,
		Action_ToggleAway,
		Action_Reconnect,
		Action_End = Action_Reconnect
	};

	Action(ActionType actionType);
	
	QKeySequence keySequence() const { return _keySequence; }
	void setKeySequence(const QKeySequence value) { _keySequence = value; }

	const QString &caption() const { return _caption; }

private:
	ActionType _actionType;
	QKeySequence _keySequence;
	QString _caption;
};

class ActionManager
{
public:
	ActionManager();

	const QList<Action> &actions() const { return _actions; }

private:
	QList<Action> _actions;
};


#endif // ACTION_H
