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

#ifndef DIALOG_CONFIG_H
#define DIALOG_CONFIG_H

#include "dialog_basic.h"

class DialogConfig : public DialogBasic
{
    Q_OBJECT

    private:
    QMap<QTreeWidgetItem*, QWidget*> nodeAndWidgetMap;

protected:
    QTreeWidget *treeMain;
    QStackedWidget *mainStack;
    QLabel *labelTitle;

    QTreeWidgetItem *createNode(QTreeWidgetItem *father, QWidget *associatedWidget,
                                const QString &label, const QIcon &icon = QIcon());
    // Destroy the node and the associated widget
    void deleteNode(QTreeWidgetItem *item);

    QWidget *getWidgetByNode(QTreeWidgetItem *item);
    QTreeWidgetItem *getNodeByWidget(QWidget *widget);

private slots:
    void treeItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

public:
    DialogConfig(QWidget *parent = 0);
};

#endif // DIALOG_CONFIG_H

