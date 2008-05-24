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

#ifndef SESSION_WIDGET_H
#define SESSION_WIDGET_H

#include "session.h"

#include <QWidget>

class SessionWidget : public QWidget
{
    Q_OBJECT

public:
    SessionWidget(Session *session, QWidget *parent = 0);

    Session *session() { return _session; }

    QString caption() const;

    bool stared() const { return _stared; }
    void setStared(bool stared) { _stared = stared; }

    virtual void applyFirstShow() {}

    virtual void search() {} // Activate the search widget (in ChannelWidget and TellWidget for instance)

protected:
    Session *_session;
    bool _stared;

    virtual QString widgetCaption() const = 0;

    bool eventFilter(QObject *obj, QEvent *event); // catch some events

signals:
    void moveLeft();
    void moveRight();
    void closeMe();
};

#endif
