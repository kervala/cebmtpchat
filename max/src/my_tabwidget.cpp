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

#include "common.h"
#include <QTabBar>

#include "my_tabbar.h"
#include "my_tabwidget.h"

MyTabWidget::MyTabWidget(QWidget *parent) : QTabWidget(parent)
{
    setTabBar(new MyTabBar(this));

    connect(tabBar(), SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SIGNAL(tabBarCustomContextMenuRequested(const QPoint &)));
}

void MyTabWidget::setTabTextColor(int tabIndex, const QColor &color)
{
    tabBar()->setTabTextColor(tabIndex, color);
}

QColor MyTabWidget::tabTextColor(int tabIndex) const
{
    return tabBar()->tabTextColor(tabIndex);
}

QRect MyTabWidget::tabRect(int tabIndex) const
{
    QRect r = tabBar()->tabRect(tabIndex);
    r.translate(tabBar()->pos());
    return r;
}

QWidget *MyTabWidget::widgetByTabPosition(const QPoint &pos) const
{
    return widget(tabBar()->tabAt(tabBar()->mapFromGlobal(pos)));
}

QPoint MyTabWidget::globalTabBarPos(const QPoint &pos) const
{
    return tabBar()->mapToGlobal(pos);
}
