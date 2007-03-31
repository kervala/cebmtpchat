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

#include <QKeyEvent>
#include <QShortcutEvent>

#include "session_widget.h"

SessionWidget::SessionWidget(Session *session, QWidget *parent) :
	QWidget(parent),
	m_session(session),
	m_stared(false),
	m_highlightType(NoHighlight)
{
	installEventFilter(this);
}

QString SessionWidget::caption() const
{
	if (m_stared)
		return widgetCaption() + " *";
	return widgetCaption();
}

bool SessionWidget::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
		if ((keyEvent->key() == Qt::Key_W && keyEvent->modifiers() == Qt::ControlModifier) ||
			(keyEvent->key() == Qt::Key_Escape))
		{
			emit closeMe();
			return true;
		}
		else
			return false;
	}
	else if (event->type() == QEvent::ShortcutOverride)
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
		if ((keyEvent->key() == Qt::Key_Left && keyEvent->modifiers() == Qt::AltModifier) ||
			keyEvent->key() == Qt::Key_F11)
		{
			emit moveLeft();
			return true;
		}
		else if ((keyEvent->key() == Qt::Key_Right && keyEvent->modifiers() == Qt::AltModifier) ||
				 keyEvent->key() == Qt::Key_F12 ||
				 (keyEvent->key() == Qt::Key_Tab && keyEvent->modifiers() == Qt::ControlModifier))
		{
			emit moveRight();
			return true;
		}
		else
			return false;
	}
	return QObject::eventFilter(obj, event);
}

void SessionWidget::sendHighlightSignal(HighlightType value)
{
	m_highlightType = value;
	emit highlightMe();
}

void SessionWidget::focusIt()
{
	m_highlightType = NoHighlight;
	emit highlightMe();
}
