#ifndef FILTER_WIDGET_H
#define FILTER_WIDGET_H

#include <QLineEdit>
#include <QComboBox>
#include <QTreeView>
#include <QToolButton>

#include <generic_sort_model.h>

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
