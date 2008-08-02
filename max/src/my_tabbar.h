#ifndef MY_TABBAR_H
#define MY_TABBAR_H

#include <QTabBar>

class MyTabBar : public QTabBar
{
    Q_OBJECT

    public:
    MyTabBar(QWidget *parent = 0) : QTabBar(parent) {}

protected:
//	QSize tabSizeHint(int index) const;
};

#endif // MY_TABBAR_H
