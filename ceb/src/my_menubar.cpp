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

#include "common.h"
#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent>

#include "profile.h"
#include "my_menubar.h"

void MyMenuBar::paintEvent(QPaintEvent *event)
{
    QMenuBar::paintEvent(event);

#ifdef Q_OS_WIN32
    if (Profile::instance().checkForUpdate)
    {
        QPainter painter(this);
        int left = width() - pixmap.width() - 2;
		int top = (height() - pixmap.height()) / 2;
        if (updateIconPressed && mouseOverIcon)
        {
            left++;
            top++;
        }
        // Paint warning
        painter.drawPixmap(left, top, pixmap);
    }
#endif
}

bool MyMenuBar::updateAvailable() const
{
    return _updateAvailable;
}

void MyMenuBar::setUpdateAvailable(bool value)
{
    if (_updateAvailable != value)
    {
        _updateAvailable = value;

		if (_updateAvailable)
            pixmap = QPixmap(":/images/update.png");
        else
            pixmap = QPixmap(":/images/no_update.png");

		repaint();
    }
}

MyMenuBar::MyMenuBar(QWidget *parent) : QMenuBar(parent)
{
    _updateAvailable = false;
    updateIconPressed = false;
    mouseOverIcon = false;
    pixmap = QPixmap(":/images/no_update.png");
}

void MyMenuBar::mousePressEvent(QMouseEvent *event)
{
    QMenuBar::mousePressEvent(event);

#ifdef Q_OS_WIN32
    if (Profile::instance().checkForUpdate)
    {
        // Icon?
        int left = width() - pixmap.width() - 2;
		int top = (height() - pixmap.height()) / 2;

		updateIconPressed = (event->x() >= left) &&
            (event->x() < left + pixmap.width()) &&
            (event->y() >= top) &&
            (event->y() < top + pixmap.height());
        mouseOverIcon = updateIconPressed;

        if (updateIconPressed)
            repaint();
    }
#endif
}

void MyMenuBar::mouseReleaseEvent(QMouseEvent *event)
{
    QMenuBar::mouseReleaseEvent(event);

#ifdef Q_OS_WIN32
    if (Profile::instance().checkForUpdate)
    {
        // Icon?
        if (updateIconPressed)
        {
            updateIconPressed = false;
            repaint();

            if (mouseOverIcon)
            {
                // Launch
                emit iconClicked();
            }
        }
    }
#endif
}

void MyMenuBar::mouseMoveEvent(QMouseEvent *event)
{
    QMenuBar::mouseMoveEvent(event);

#ifdef Q_OS_WIN32
    if (Profile::instance().checkForUpdate)
    {
        int left = width() - pixmap.width() - 2;
		int top = (height() - pixmap.height()) / 2;

		bool overIcon = (event->x() >= left) &&
            (event->x() < left + pixmap.width()) &&
            (event->y() >= top) &&
            (event->y() < top + pixmap.height());

        if (overIcon != mouseOverIcon)
        {
            mouseOverIcon = overIcon;
            repaint();
        }
    }
#endif
}
