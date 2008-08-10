#include <QAction>

#include "my_tabbar.h"

MyTabBar::MyTabBar(QWidget *parent)
    : QTabBar(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
}

