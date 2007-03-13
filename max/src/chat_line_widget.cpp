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

#include "chat_line_widget.h"

#include <QKeyEvent>

ChatLineWidget::ChatLineWidget(QWidget *parent) : QLineEdit(parent)
{
}

void ChatLineWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Return ||
		e->key() == Qt::Key_Enter)
		validateText();
    else if (e->key() == Qt::Key_PageUp)
    {
        if (e->modifiers() == Qt::NoModifier)
            emit pageUp();
        else
            QLineEdit::keyPressEvent(e);
    }
    else if (e->key() == Qt::Key_PageDown)
    {
        if (e->modifiers() == Qt::NoModifier)
            emit pageDown();
        else
            QLineEdit::keyPressEvent(e);
    }
    else
        QLineEdit::keyPressEvent(e);
}

void ChatLineWidget::validateText()
{
    if (text().isEmpty())
        return;

	QString textToSend = text();
	
	clear();
    emit textValidated(textToSend);
}
