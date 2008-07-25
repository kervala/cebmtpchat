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

#include "multi_tab_widget.h"

MultiTabWidget::MultiTabWidget(QWidget *parent) : QWidget(parent)
{
    tabWidgetMain = 0;
    tabWidgetCentral = 0;

    _displayMode = AllInOneRow;
    _allInOneRowLocation = North;
    _superLocation = North;
    _subLocation = South;

    mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
}

void MultiTabWidget::setDisplayMode(DisplayMode displayMode)
{
    if (displayMode == _displayMode)
        return;

    this->setUpdatesEnabled(false);

    QWidget *oldFocusedWidget = focusedWidget();

    switch(displayMode)
    {
    case AllInOneRow:
        // Destroy all MyTabWidgets
        if (tabWidgetCentral)
        {
            disconnect(tabWidgetCentral, SIGNAL(currentChanged(int)),
                       this, SLOT(fatherCurrentTabChanged(int)));
            tabWidgetMain = new MyTabWidget;
            connect(tabWidgetMain, SIGNAL(currentChanged(int)),
                    this, SLOT(fatherCurrentTabChanged(int)));
            setTabWidgetPosition(tabWidgetMain, _allInOneRowLocation);

            while (tabWidgetCentral->count())
            {
                QWidget *widget = tabWidgetCentral->widget(0);
                QIcon tmpIcon = tabWidgetCentral->tabIcon(0);
                QColor tmpTextColor = tabWidgetCentral->tabTextColor(0);

                tabWidgetCentral->removeTab(0);
                widget->setParent(0);
                int index = tabWidgetMain->addTab(widget, tmpIcon, getCaption(widget2Info[widget]));
                tabWidgetMain->setTabTextColor(index, tmpTextColor);
            }
            mainLayout->removeWidget(tabWidgetCentral);
            delete tabWidgetCentral;
            tabWidgetCentral = 0;
            mainLayout->addWidget(tabWidgetMain);
        }
        else if (tabWidgetMain)
        {
            disconnect(tabWidgetMain, SIGNAL(currentChanged(int)), this, SLOT(superTabChanged(int)));
            int tabCount = tabWidgetMain->count();
            for (int i = 0; i < tabCount; i++)
            {
                MyTabWidget *tabWidget = qobject_cast<MyTabWidget*>(tabWidgetMain->widget(0));
                disconnect(tabWidget, SIGNAL(currentChanged(int)),
                           this, SLOT(fatherCurrentTabChanged(int)));
                tabWidgetMain->removeTab(0);
                while (tabWidget->count())
                {
                    QWidget *widget = tabWidget->widget(0);
                    QIcon tmpIcon = tabWidget->tabIcon(0);
                    QColor tmpTextColor = tabWidget->tabTextColor(0);
                    tabWidget->removeTab(0);
                    widget->setParent(0);
                    int index = tabWidgetMain->addTab(widget, tmpIcon, getCaption(widget2Info[widget]));
                    tabWidgetMain->setTabTextColor(index, tmpTextColor);
                }
                delete tabWidget;
            }
            setTabWidgetPosition(tabWidgetMain, _allInOneRowLocation);
            connect(tabWidgetMain, SIGNAL(currentChanged(int)),
                    this, SLOT(fatherCurrentTabChanged(int)));
        }
        break;
    case Hierarchical:
        if (_superLabels.count() == 1)
        {
            disconnect(tabWidgetMain, SIGNAL(currentChanged(int)),
                       this, SLOT(fatherCurrentTabChanged(int)));

            tabWidgetCentral = new MyTabWidget;

            setTabWidgetPosition(tabWidgetCentral, _subLocation);

            while (tabWidgetMain->count())
            {
                QWidget *widget = tabWidgetMain->widget(0);
                QIcon tmpIcon = tabWidgetMain->tabIcon(0);
                QColor tmpTextColor = tabWidgetMain->tabTextColor(0);
                tabWidgetMain->removeTab(0);
                widget->setParent(0);
                int index = tabWidgetCentral->addTab(widget, tmpIcon, widget2Info[widget].label);
                tabWidgetCentral->setTabTextColor(index, tmpTextColor);
            }
            mainLayout->removeWidget(tabWidgetMain);
            delete tabWidgetMain;
            tabWidgetMain = 0;
            mainLayout->addWidget(tabWidgetCentral);
            connect(tabWidgetCentral, SIGNAL(currentChanged(int)),
                    this, SLOT(fatherCurrentTabChanged(int)));
        }
        else if (_superLabels.count() > 1)
        {
            MyTabWidget *tabWidgetMainNew = new MyTabWidget;
            disconnect(tabWidgetMain, SIGNAL(currentChanged(int)),
                       this, SLOT(fatherCurrentTabChanged(int)));
            while (tabWidgetMain->count())
            {
                QWidget *widget = tabWidgetMain->widget(0);
                QIcon tmpIcon = tabWidgetMain->tabIcon(0);
                QColor tmpTextColor = tabWidgetMain->tabTextColor(0);
                tabWidgetMain->removeTab(0);

                QString superLabel = widget2Info[widget].superLabel;
                MyTabWidget *tabWidget = 0;
                // Existing tab widget for the superLabel
                for (int i = 0; i < tabWidgetMainNew->count(); i++)
                {
                    MyTabWidget *w = qobject_cast<MyTabWidget*>(tabWidgetMainNew->widget(i));
                    if (w && widget2Info[w->widget(0)].superLabel == superLabel)
                    {
                        tabWidget = w;
                        break;
                    }
                }
                if (!tabWidget)
                {
                    tabWidget = new MyTabWidget;
                    setTabWidgetPosition(tabWidget, _subLocation);
                    tabWidgetMainNew->addTab(tabWidget, superLabel);
                }
                widget->setParent(0);
                int index = tabWidget->addTab(widget, tmpIcon, widget2Info[widget].label);
                tabWidget->setTabTextColor(index, tmpTextColor);

                // Do connections
                for (int i = 0; i < tabWidgetMainNew->count(); i++)
                {
                    MyTabWidget *w = qobject_cast<MyTabWidget*>(tabWidgetMainNew->widget(i));
                    connect(w, SIGNAL(currentChanged(int)),
                            this, SLOT(fatherCurrentTabChanged(int)));
                }

            }
            setTabWidgetPosition(tabWidgetMainNew, _superLocation);
            mainLayout->removeWidget(tabWidgetMain);
            delete tabWidgetMain;
            mainLayout->addWidget(tabWidgetMainNew);
            connect(tabWidgetMainNew, SIGNAL(currentChanged(int)), this, SLOT(superTabChanged(int)));
            tabWidgetMain = tabWidgetMainNew;
        }

        break;
    }

    _displayMode = displayMode;

    this->setUpdatesEnabled(true);

    if (oldFocusedWidget != focusedWidget())
        focusWidget(oldFocusedWidget);
}

void MultiTabWidget::setAllInOneRowLocation(TabLocation tabLocation)
{
    if (_displayMode != AllInOneRow)
        return;

    if (_allInOneRowLocation == tabLocation) // Value already set
        return;

    if (tabWidgetMain)
        setTabWidgetPosition(tabWidgetMain, tabLocation);

    _allInOneRowLocation = tabLocation;
}

void MultiTabWidget::setSuperLocation(TabLocation tabLocation)
{
    if (_displayMode != Hierarchical)
        return;

    if (_superLocation == tabLocation) // Value already set
        return;

    if (tabWidgetMain)
        setTabWidgetPosition(tabWidgetMain, tabLocation);

    _superLocation = tabLocation;
}

void MultiTabWidget::setSubLocation(TabLocation tabLocation)
{
    if (_displayMode != Hierarchical)
        return;

    if (_subLocation == tabLocation)
        return;

    // All subwidgets
    if (tabWidgetCentral)
        setTabWidgetPosition(tabWidgetCentral, tabLocation);
    else if (tabWidgetMain)
    {
        switch (tabLocation)
        {
        case North:
            for (int i = 0; i < tabWidgetMain->count(); i++)
            {
                MyTabWidget *tabWidget = qobject_cast<MyTabWidget*>(tabWidgetMain->widget(i));
                tabWidget->setTabPosition(QTabWidget::North);
            }
            break;
        case South:
            for (int i = 0; i < tabWidgetMain->count(); i++)
            {
                MyTabWidget *tabWidget = qobject_cast<MyTabWidget*>(tabWidgetMain->widget(i));
                tabWidget->setTabPosition(QTabWidget::South);
            }
            break;
        }
    }

    _subLocation = tabLocation;
}

void MultiTabWidget::addWidget(const QString &superLabel, QWidget *widget, const QString &label, CaptionMode captionMode)
{
    storeNewWidget(superLabel, widget, label, captionMode);
}

void MultiTabWidget::addWidget(const QString &superLabel, QWidget *widget, const QIcon &icon, const QString &label, CaptionMode captionMode)
{
    MyTabWidget *tabWidget = storeNewWidget(superLabel, widget, label, captionMode);
    if (tabWidget)
        tabWidget->setTabIcon(tabWidget->indexOf(widget), icon);
}

MyTabWidget *MultiTabWidget::getTabWidgetBySuperLabel(const QString &superLabel) const
{
    if (_displayMode == Hierarchical)
    {
        if (tabWidgetCentral &&
            widget2Info[tabWidgetCentral->widget(0)].superLabel == superLabel)
            return tabWidgetCentral;
        else if (tabWidgetMain)
        {
            for (int i = 0; i < tabWidgetMain->count(); i++)
            {
                MyTabWidget *tabWidget = qobject_cast<MyTabWidget*>(tabWidgetMain->widget(i));
                if (widget2Info[tabWidget->widget(0)].superLabel == superLabel)
                    return tabWidget;
            }
        }
    }
    return 0;
}

MyTabWidget *MultiTabWidget::insertNewWidget(const QString &superLabel, QWidget *widget, const QString &label, CaptionMode captionMode)
{
    switch (_displayMode)
    {
    case AllInOneRow:
        if (!tabWidgetMain)
        {
            tabWidgetMain = new MyTabWidget;
            connect(tabWidgetMain, SIGNAL(currentChanged(int)),
                    this, SLOT(fatherCurrentTabChanged(int)));

            setTabWidgetPosition(tabWidgetMain, _allInOneRowLocation);
            mainLayout->addWidget(tabWidgetMain);
        }

        {
            WidgetInfo info = { superLabel, label, captionMode };
            widget2Info.insert(widget, info);
        }

        // Add the widget
        tabWidgetMain->addTab(widget, getCaption(widget2Info[widget]));
        return tabWidgetMain;
    case Hierarchical:
        if (!mainLayout->count())
        {
            // No elements
            tabWidgetCentral = new MyTabWidget;
            connect(tabWidgetCentral, SIGNAL(currentChanged(int)),
                    this, SLOT(fatherCurrentTabChanged(int)));

            setTabWidgetPosition(tabWidgetCentral, _subLocation);
            mainLayout->addWidget(tabWidgetCentral);
            tabWidgetCentral->addTab(widget, label);
            return tabWidgetCentral;
        }
        else
        {
            // Is super label already exists?
            MyTabWidget *tabWidget = getTabWidgetBySuperLabel(superLabel);
            if (tabWidget)
            {
                tabWidget->addTab(widget, label);
                return tabWidget;
            }
            else
            {
                if (tabWidgetCentral)
                {
                    // Move existing tabwidget
                    QString centralSuperLabel = widget2Info[tabWidgetCentral->widget(0)].superLabel;
                    mainLayout->removeWidget(tabWidgetCentral);

                    tabWidgetMain = new MyTabWidget;
                    connect(tabWidgetMain, SIGNAL(currentChanged(int)), this, SLOT(superTabChanged(int)));
                    setTabWidgetPosition(tabWidgetMain, _superLocation);
                    mainLayout->addWidget(tabWidgetMain);
                    tabWidgetCentral->setParent(0);
                    tabWidgetMain->addTab(tabWidgetCentral, centralSuperLabel);

                    // No need reference to central tab widget anymore
                    tabWidgetCentral = 0;
                }

                // Just add a new tab widget
                MyTabWidget *tabWidget = new MyTabWidget;
                connect(tabWidget, SIGNAL(currentChanged(int)),
                        this, SLOT(fatherCurrentTabChanged(int)));
                setTabWidgetPosition(tabWidget, _subLocation);
                tabWidgetMain->addTab(tabWidget, superLabel);
                tabWidget->addTab(widget, label);
                return tabWidget;
            }
        }
        break;
    }
    return 0;
}

MyTabWidget *MultiTabWidget::storeNewWidget(const QString &superLabel, QWidget *widget, const QString &label, CaptionMode captionMode)
{
    MyTabWidget *tabWidget = insertNewWidget(superLabel, widget, label, captionMode);
    if (tabWidget)
    {
        widgets << widget;
        if (_superLabels.indexOf(superLabel) < 0)
            _superLabels << superLabel;
        WidgetInfo info = { superLabel, label, captionMode };
        widget2Info.insert(widget, info);
    }
    return tabWidget;
}

void MultiTabWidget::setTabWidgetPosition(MyTabWidget *tabWidget, TabLocation tabLocation)
{
    switch(tabLocation)
    {
    case North:
        tabWidget->setTabPosition(QTabWidget::North);
        break;
    case South:
        tabWidget->setTabPosition(QTabWidget::South);
        break;
    }
}

void MultiTabWidget::removeWidget(QWidget *widget)
{
    // Record old focused widget to see if it changes at the end
    QWidget *oldFocusedWidget = focusedWidget();

    QString superLabel;
    switch (_displayMode)
    {
    case AllInOneRow:
        if (!tabWidgetMain || tabWidgetMain->indexOf(widget) < 0)
            return;
        tabWidgetMain->removeTab(tabWidgetMain->indexOf(widget));
        widget->deleteLater();
        widgets.removeAt(widgets.indexOf(widget));
        cleanSuperLabel(widget2Info[widget].superLabel);
        widget2Info.remove(widget);

        if (!tabWidgetMain->count())
        {
            mainLayout->removeWidget(tabWidgetMain);
            delete tabWidgetMain;
            tabWidgetMain = 0;
        }
        else
        {
            // Refocus the current widget
            QWidget *w = tabWidgetMain->currentWidget();
            if (w->focusWidget())
                w->focusWidget()->setFocus();
        }
        break;
    case Hierarchical:
        if (tabWidgetCentral)
        {
            if (tabWidgetCentral->indexOf(widget) < 0)
                return;
            tabWidgetCentral->removeTab(tabWidgetCentral->indexOf(widget));
            widget->deleteLater();
            widgets.removeAt(widgets.indexOf(widget));
            cleanSuperLabel(widget2Info[widget].superLabel);
            widget2Info.remove(widget);

            if (!tabWidgetCentral->count())
            {
                mainLayout->removeWidget(tabWidgetCentral);
                delete tabWidgetCentral;
                tabWidgetCentral = 0;
            }
            else
            {
                // Refocus the current widget
                QWidget *w = tabWidgetCentral->currentWidget();
                if (w->focusWidget())
                    w->focusWidget()->setFocus();
            }
        }
        else if (tabWidgetMain)
        {
            WidgetInfo info = widget2Info[widget];
            MyTabWidget *tabWidget = getTabWidgetBySuperLabel(widget2Info[widget].superLabel);
            if (tabWidget)
            {
                tabWidget->removeTab(tabWidget->indexOf(widget));
                widget->deleteLater();
                widgets.removeAt(widgets.indexOf(widget));
                cleanSuperLabel(widget2Info[widget].superLabel);
                widget2Info.remove(widget);

                if (!tabWidget->count())
                {
                    tabWidgetMain->removeTab(tabWidgetMain->indexOf(tabWidget));
                    delete tabWidget;
                }
                // One tabwidget? => tabWidgetCentral
                if (tabWidgetMain->count() == 1)
                {
                    MyTabWidget *tabWidget = qobject_cast<MyTabWidget*>(tabWidgetMain->widget(0));
                    tabWidgetMain->removeTab(0);
                    setTabWidgetPosition(tabWidget, _subLocation);
                    mainLayout->removeWidget(tabWidgetMain);
                    tabWidget->setParent(0);
                    mainLayout->addWidget(tabWidget);
                    delete tabWidgetMain;
                    tabWidgetMain = 0;
                    tabWidgetCentral = tabWidget;
                    tabWidget->setVisible(true);

                    // Refocus the current widget
                    QWidget *w = tabWidgetCentral->currentWidget();
                    if (w->focusWidget())
                        w->focusWidget()->setFocus();
                }
                else
                {
                    // Refocus the current widget
                    MyTabWidget *tabWidget = qobject_cast<MyTabWidget*>(tabWidgetMain->currentWidget());
                    QWidget *w = tabWidget->currentWidget();
                    if (w->focusWidget())
                        w->focusWidget()->setFocus();
                }
            }
        }
        break;
    }

    QWidget *newFocusedWidget = focusedWidget();
    if (newFocusedWidget != oldFocusedWidget)
        emit focusedWidgetChanged(newFocusedWidget);
}

void MultiTabWidget::removeSuperWidgets(const QString &superLabel)
{
    for (int i = widgets.count() - 1; i >= 0; i--)
        if (widget2Info[widgets[i]].superLabel == superLabel)
            removeWidget(widgets[i]);
}

void MultiTabWidget::cleanSuperLabel(const QString &superLabel)
{
    bool removeSuperLabel = true;
    foreach (QWidget *widget, widgets)
        if (widget2Info[widget].superLabel == superLabel)
        {
            removeSuperLabel = false;
            break;
        }
    if (removeSuperLabel)
        _superLabels.removeAt(_superLabels.indexOf(superLabel));
}

void MultiTabWidget::renameSuperLabel(const QString &oldSuperLabel, const QString &newSuperLabel)
{
    if (oldSuperLabel == newSuperLabel)
        return;

    // Change in internal datas
    bool found = false;
    for (int i = 0; i < _superLabels.count(); i++)
        if (_superLabels[i] == oldSuperLabel)
        {
            found = true;
            _superLabels[i] = newSuperLabel;
            break;
        }

    if (!found)
        return;

    QList<QWidget*> widgetsToChange = getWidgets(oldSuperLabel);
    foreach (QWidget *widget, widgetsToChange)
    {
        // Re-insert a modified info
        WidgetInfo info = widget2Info[widget];
        info.superLabel = newSuperLabel;
        widget2Info.insert(widget, info);
    }

    // Propagate in visual controls
    switch (_displayMode)
    {
    case AllInOneRow:
        if (tabWidgetMain)
            for (int i = 0; i < tabWidgetMain->count(); i++)
            {
                QWidget *widget = tabWidgetMain->widget(i);
                if (widgetsToChange.indexOf(widget) >= 0)
                    tabWidgetMain->setTabText(i, getCaption(widget2Info[widget]));
            }
        break;
    case Hierarchical:
        if (tabWidgetMain)
        {
            MyTabWidget *tabWidget = getTabWidgetBySuperLabel(newSuperLabel);
            if (tabWidget)
                tabWidgetMain->setTabText(tabWidgetMain->indexOf(tabWidget), newSuperLabel);
        }
        break;
    }
}

void MultiTabWidget::renameLabel(QWidget *widget, const QString &newLabel)
{
    // Change in internal datas
    if (widget2Info.find(widget) == widget2Info.end())
        return;

    // Re-insert a new info
    WidgetInfo info = widget2Info[widget];
    info.label = newLabel;
    widget2Info.insert(widget, info);

    // In visual controls
    MyTabWidget *tabWidget = getFatherTabWidget(widget);
    if (_displayMode == AllInOneRow)
        tabWidget->setTabText(tabWidget->indexOf(widget), getCaption(widget2Info[widget]));
    else
        tabWidget->setTabText(tabWidget->indexOf(widget), newLabel);
}

void MultiTabWidget::rotateCurrentPageToLeft()
{
    MyTabWidget *tabWidget = 0;

    switch (_displayMode)
    {
    case AllInOneRow:
        tabWidget = tabWidgetMain;
        break;
    case Hierarchical:
        tabWidget = getCurrentSubTabWidget();
        break;
    }

    if (tabWidget && tabWidget->count() > 1)
    {
        if (tabWidget->currentIndex() == 0)
            tabWidget->setCurrentIndex(tabWidget->count() - 1);
        else
            tabWidget->setCurrentIndex(tabWidget->currentIndex() - 1);
    }
}

void MultiTabWidget::rotateCurrentPageToRight()
{
    MyTabWidget *tabWidget = 0;

    switch (_displayMode)
    {
    case AllInOneRow:
        tabWidget = tabWidgetMain;
        break;
    case Hierarchical:
        tabWidget = getCurrentSubTabWidget();
        break;
    }

    if (tabWidget && tabWidget->count() > 1)
    {
        if (tabWidget->currentIndex() == tabWidget->count() - 1)
            tabWidget->setCurrentIndex(0);
        else
            tabWidget->setCurrentIndex(tabWidget->currentIndex() + 1);
    }
}

MyTabWidget *MultiTabWidget::getCurrentSubTabWidget()
{
    if (_displayMode != Hierarchical)
        return 0;

    if (tabWidgetCentral)
        return tabWidgetCentral;
    else if (tabWidgetMain)
        return qobject_cast<MyTabWidget*>(tabWidgetMain->currentWidget());
    else
        return 0;
}

QList<QWidget*> MultiTabWidget::getWidgets(const QString &superLabel) const
{
    QList<QWidget*> list;

    foreach (QWidget *widget, widgets)
        if (widget2Info[widget].superLabel == superLabel)
            list << widget;

    return list;
}

void MultiTabWidget::focusWidget(QWidget *widget)
{
    MyTabWidget *tabWidget = getFatherTabWidget(widget);
    if (tabWidget)
    {
        if (_displayMode == Hierarchical && tabWidgetMain)
            tabWidgetMain->setCurrentWidget(tabWidget);

        tabWidget->setCurrentWidget(widget);
    }
}

MyTabWidget *MultiTabWidget::getFatherTabWidget(QWidget *widget) const
{
    if (widgets.indexOf(widget) < 0)
        return 0;

    switch (_displayMode)
    {
    case AllInOneRow:
        if (tabWidgetMain)
            return tabWidgetMain;
        break;
    case Hierarchical:
        if (tabWidgetCentral)
            return tabWidgetCentral;
        else
            return getTabWidgetBySuperLabel(widget2Info[widget].superLabel);
        break;
    }
    return 0;
}

void MultiTabWidget::changeTabIcon(QWidget *widget, const QIcon &icon)
{
    MyTabWidget *tabWidget = getFatherTabWidget(widget);
    if (tabWidget)
        tabWidget->setTabIcon(tabWidget->indexOf(widget), icon);
}

QColor MultiTabWidget::tabTextColor(QWidget *widget) const
{
    MyTabWidget *tabWidget = getFatherTabWidget(widget);
    if (tabWidget)
        return tabWidget->tabTextColor(tabWidget->indexOf(widget));
    else
        return QColor();
}

void MultiTabWidget::changeTabTextColor(QWidget *widget, const QColor &color)
{
    MyTabWidget *tabWidget = getFatherTabWidget(widget);
    if (tabWidget)
        tabWidget->setTabTextColor(tabWidget->indexOf(widget), color);
}

void MultiTabWidget::changeCaptionMode(QWidget *widget, CaptionMode captionMode)
{
    if (widgets.indexOf(widget) < 0)
        return;

    WidgetInfo &info = widget2Info[widget];
    info.captionMode = captionMode;

    if (_displayMode != AllInOneRow)
        return;

    MyTabWidget *tabWidget = getFatherTabWidget(widget);
    if (tabWidget)
        tabWidget->setTabText(tabWidget->indexOf(widget), getCaption(info));
}

QWidget *MultiTabWidget::focusedWidget() const
{
    switch (_displayMode)
    {
    case AllInOneRow:
        if (tabWidgetMain)
            return tabWidgetMain->currentWidget();
        break;
    case Hierarchical:
        if (tabWidgetCentral)
            return tabWidgetCentral->currentWidget();
        else if (tabWidgetMain)
        {
            MyTabWidget *tabWidget = qobject_cast<MyTabWidget*>(tabWidgetMain->currentWidget());
            if (tabWidget)
                return tabWidget->currentWidget();
        }
        break;
    }
    return 0;
}

void MultiTabWidget::fatherCurrentTabChanged(int index)
{
    MyTabWidget *tabWidget = qobject_cast<MyTabWidget*>(sender());
    if (index >= 0)
        emit focusedWidgetChanged(tabWidget->widget(index));
}

void MultiTabWidget::superTabChanged(int index)
{
    if (index < 0)
        return;
    MyTabWidget *tabWidget = qobject_cast<MyTabWidget*>(tabWidgetMain->widget(index));
    if (tabWidget && tabWidget->currentWidget())
        emit focusedWidgetChanged(tabWidget->currentWidget());
}

QWidget *MultiTabWidget::widgetByTabLocation(const QPoint &p) const
{
    MyTabWidget *tabWidget = 0;
    switch (_displayMode)
    {
    case AllInOneRow:
        tabWidget = tabWidgetMain;
        break;
    case Hierarchical:
        if (tabWidgetCentral)
            tabWidget = tabWidgetCentral;
        else if (tabWidgetMain)
            tabWidget = qobject_cast<MyTabWidget*>(tabWidgetMain->currentWidget());
        break;
    default: return 0;
    }

    if (!tabWidget)
        return 0;

    QPoint wP = tabWidget->mapFromGlobal(p);
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        QRect r = tabWidget->tabRect(i);
        if (wP.x() >= r.left() && wP.x() <= r.right() &&
            wP.y() >= r.top() && wP.y() <= r.bottom())
            return tabWidget->widget(i);
    }
    return 0;
}

QString MultiTabWidget::getCaption(const WidgetInfo &info) const
{
    switch (info.captionMode)
    {
    case LabelAndSuperLabel: return info.label + " (" + info.superLabel + ")";
    case LabelOnly: return info.label;
    case SuperLabelOnly: return info.superLabel;
    default: return "";
    }
}

void MultiTabWidget::clear()
{
    while (widgets.count())
        removeWidget(widgets[0]);
}
