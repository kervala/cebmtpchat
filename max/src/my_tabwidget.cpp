#include <QTabBar>

#include "my_tabbar.h"
#include "my_tabwidget.h"

MyTabWidget::MyTabWidget(QWidget *parent) : QTabWidget(parent)
{
    setTabBar(new MyTabBar(this));
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
