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

#ifndef MULTI_TAB_WIDGET_H
#define MULTI_TAB_WIDGET_H

#include <QVBoxLayout>
#include <QMap>

#include "my_tabwidget.h"

class MultiTabWidget : public QWidget
{
	Q_OBJECT

public:
	enum DisplayMode {
		DisplayMode_AllInOneRow,
		DisplayMode_Hierarchical,
	};

	enum TabLocation {
		TabLocation_North,
		TabLocation_South
	};

	MultiTabWidget(QWidget *parent = 0);

	// Configuration functions
	inline DisplayMode displayMode() const
	  { return _displayMode; }
	void setDisplayMode(DisplayMode displayMode);
	inline TabLocation allInOneRowLocation() const
	  { return _allInOneRowLocation; }
	void setAllInOneRowLocation(TabLocation tabLocation);
	inline TabLocation superLocation() const
	  { return _superLocation; }
	void setSuperLocation(TabLocation tabLocation);
	inline TabLocation subLocation() const
	  { return _subLocation; }
	void setSubLocation(TabLocation tabLocation);

	// Widget functions
	inline const QList<QString> superLabels() const
	  { return _superLabels; }
	QList<QWidget*> getWidgets(const QString &superLabel) const;
	QList<QWidget*> getWidgets() const { return widgets; }
	QWidget *widget(int index) const { return widgets[index]; }
	int count() const { return widgets.count(); }

	QWidget *focusedWidget() const;
	void focusWidget(QWidget *widget);
	QWidget *widgetByTabLocation(const QPoint &p) const;

	void addWidget(const QString &superLabel, QWidget *widget, const QString &label, bool richLabel = true);
	void addWidget(const QString &superLabel, QWidget *widget, const QIcon &icon, const QString &label, bool richLabel = true);
	void removeWidget(QWidget *widget);
	void removeSuperWidgets(const QString &superLabel);
	void renameSuperLabel(const QString &oldSuperLabel, const QString &newSuperLabel);
	void renameLabel(QWidget *widget, const QString &newLabel);
	void changeTabIcon(QWidget *widget, const QIcon &icon);
	QColor tabTextColor(QWidget *widget) const;
	void changeTabTextColor(QWidget *widget, const QColor &color);

public slots:
	void rotateCurrentPageToLeft();
	void rotateCurrentPageToRight();

private:
	struct WidgetInfo {
		QString superLabel;
		QString label;
		bool showSuperLabel;
	};

	QVBoxLayout *mainLayout;
	MyTabWidget *tabWidgetMain;
	MyTabWidget *tabWidgetCentral;

	QList<QString> _superLabels;
	QList<QWidget*> widgets;
	QMap<QWidget*, WidgetInfo> widget2Info;

	DisplayMode _displayMode;
	TabLocation _allInOneRowLocation;
	TabLocation _superLocation;
	TabLocation _subLocation;

	void setTabWidgetPosition(MyTabWidget *tabWidget, TabLocation tabLocation);
	MyTabWidget *getTabWidgetBySuperLabel(const QString &superLabel) const;
	MyTabWidget *getCurrentSubTabWidget();
	MyTabWidget *getFatherTabWidget(QWidget *widget) const;
	void cleanSuperLabel(const QString &superLabel);
	// Return the QTabWidget in which the new widget is stored
	MyTabWidget *storeNewWidget(const QString &superLabel, QWidget *widget, const QString &label, bool richLabel);
	MyTabWidget *insertNewWidget(const QString &superLabel, QWidget *widget, const QString &label, bool richLabel);

private slots:
	void fatherCurrentTabChanged(int index);
	void superTabChanged(int index);

signals:
	void focusedWidgetChanged(QWidget *widget);
};

#endif
