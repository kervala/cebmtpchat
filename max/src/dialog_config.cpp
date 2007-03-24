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

#include "dialog_config.h"

#include <QSpacerItem>
#include <QSplitter>
#include <QFrame>

DialogConfig::DialogConfig(QWidget *parent): DialogBasic(parent)
{
	// Main frame
	QSplitter *mainSplitter = new QSplitter(this);
	mainLayout->insertWidget(0, mainSplitter);
	treeMain = new QTreeWidget(mainSplitter);
	connect(treeMain, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
		this, SLOT(treeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
	treeMain->setMinimumWidth(60);
	mainSplitter->addWidget(treeMain);

	QWidget *rightWidget = new QWidget;
	mainSplitter->addWidget(rightWidget);
	QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
	rightLayout->setMargin(0);
	
	QFrame *frameTitle = new QFrame;
	frameTitle->setFrameShape(QFrame::Panel);
	frameTitle->setFrameShadow(QFrame::Sunken);
	rightLayout->addWidget(frameTitle);
	
	labelTitle = new QLabel("<Empty>");
	QVBoxLayout *titleLayout = new QVBoxLayout(frameTitle);
	titleLayout->setMargin(0);
	titleLayout->addWidget(labelTitle);
	QFont labelFont = labelTitle->font();
	labelFont.setPointSize(12);
	labelFont.setBold(true);
	labelTitle->setFont(labelFont);
	QPalette palette = labelTitle->palette();
	palette.setColor(QPalette::Background, QColor(200, 200, 200));
	labelTitle->setPalette(palette);
	
	mainStack = new QStackedWidget;
	
	QSizePolicy sizePolicy = rightWidget->sizePolicy();
	sizePolicy.setHorizontalStretch(1);
	rightWidget->setSizePolicy(sizePolicy);
	rightLayout->addWidget(mainStack);
	mainSplitter->setChildrenCollapsible(false);

	QList<int> list;
	list.append(130);
	mainSplitter->setSizes(list);
}

QTreeWidgetItem *DialogConfig::createNode(QTreeWidgetItem *father, QWidget *associatedWidget,
										  const QString &label, const QIcon &icon)
{
	// 1. Create node
	QTreeWidgetItem *item = new QTreeWidgetItem();

	if (!father)
		treeMain->addTopLevelItem(item);
	else
		father->addChild(item);

	item->setText(0, label);
	item->setIcon(0, icon);

	// 2. Associate it to the widget
	nodeAndWidgetMap.insert(item, associatedWidget);

	// 3. Insert widget in the stack widget
	mainStack->addWidget(associatedWidget);

	return item;
}

void DialogConfig::deleteNode(QTreeWidgetItem *item)
{
	QWidget *widget = getWidgetByNode(item);

	if (item->parent())
	{
		QTreeWidgetItem *parent = item->parent();
		parent->takeChild(parent->indexOfChild(item));
	}
	else
		treeMain->takeTopLevelItem(treeMain->indexOfTopLevelItem(item));

	// Delete the widget
	delete widget;
	
	// Delete the node
	delete item;
}

void DialogConfig::treeItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
	mainStack->setCurrentWidget(getWidgetByNode(current));
	labelTitle->setText(current->text(0));
}

QWidget *DialogConfig::getWidgetByNode(QTreeWidgetItem *item)
{
	return nodeAndWidgetMap.value(item);
}

QTreeWidgetItem *DialogConfig::getNodeByWidget(QWidget *widget)
{
	QList<QTreeWidgetItem*> list = nodeAndWidgetMap.keys(widget);

	if (list.count() > 0)
		return list[0];
	else
		return 0;
}
