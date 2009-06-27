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
        QPixmap pixmap;

        // Paint warning
        if (_updateAvailable)
            pixmap = QPixmap(":/images/update.png");
        else
            pixmap = QPixmap(":/images/no_update.png");

        QPainter painter(this);
        int left = width() - 18;
        int top = 0;
        if (updateIconPressed && mouseOverIcon)
        {
            left++;
            top++;
        }
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
        repaint();
    }
}

MyMenuBar::MyMenuBar(QWidget *parent) : QMenuBar(parent)
{
    _updateAvailable = false;
    updateIconPressed = false;
}

void MyMenuBar::mousePressEvent(QMouseEvent *event)
{
    QMenuBar::mousePressEvent(event);

#ifdef Q_OS_WIN32
    if (Profile::instance().checkForUpdate)
    {
        // Icon?
        updateIconPressed = (event->x() >= width() - 18) &&
            (event->x() < width() - 2) &&
            (event->y() >= 0) &&
            (event->y() < 16);
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
        bool overIcon = (event->x() >= width() - 18) &&
            (event->x() < width() - 2) &&
            (event->y() >= 0) &&
            (event->y() < 16);

        if (overIcon != mouseOverIcon)
        {
            mouseOverIcon = overIcon;
            repaint();
        }
    }
#endif
}
