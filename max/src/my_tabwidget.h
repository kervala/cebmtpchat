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
