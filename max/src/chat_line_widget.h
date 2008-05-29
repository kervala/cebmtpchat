/*
    This file is part of Max.

    Max is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    Max is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Max; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef CHAT_LINE_WIDGET_H
#define CHAT_LINE_WIDGET_H

#include <QLineEdit>

class ChatLineWidget : public QLineEdit
{
	Q_OBJECT

private:
    void validateText();

protected:
    void keyPressEvent(QKeyEvent *e);

public:
	ChatLineWidget(QWidget *parent = 0);

signals:
    void textValidated(const QString &text);
    void moveLeft();
    void moveRight();
    void pageUp();
    void pageDown();
};

#endif
