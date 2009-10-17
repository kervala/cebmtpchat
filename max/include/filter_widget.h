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

#ifndef FILTER_WIDGET_H
#define FILTER_WIDGET_H

#include <QLineEdit>
#include <QComboBox>
#include <QTreeView>
#include <QToolButton>

#include "generic_sort_model.h"

class FilterWidget : public QWidget
{
	Q_OBJECT

public:
	FilterWidget(QWidget *parent = 0);

	void setTreeView(QTreeView *treeView);

private:
	QLineEdit *m_lineEditFilter;
	QComboBox *m_comboBoxColumn;
	QToolButton *m_clearToolButton;

	QTreeView *m_treeView;

	GenericSortModel *getProxyModel() const;

private slots:
	void filterTextChanged(const QString &text);
	void columnIndexChanged(int index);
};

#endif