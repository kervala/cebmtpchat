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

#ifndef MY_TABWIDGET_H
#define MY_TABWIDGET_H

#include <QTabWidget>

class MyTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    MyTabWidget(QWidget *parent = 0);

    QColor tabTextColor(int tabIndex) const;
    void setTabTextColor(int tabIndex, const QColor &color);
    QRect tabRect(int tabIndex) const;
    QWidget *widgetByTabPosition(const QPoint &pos) const; //!< returns a widget in function of a position supposed to be in a tabrect

    QPoint globalTabBarPos(const QPoint &pos) const;

signals:
    void tabBarCustomContextMenuRequested(const QPoint &pos);
};

#endif
