#include <QLabel>
#include <QHBoxLayout>

#include "filter_widget.h"

FilterWidget::FilterWidget(QWidget *parent) : QWidget(parent)
{
	m_treeView = 0;

	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->setMargin(0);	

	m_clearToolButton = new QToolButton;
	m_clearToolButton->setVisible(false);
	m_clearToolButton->setIcon(QIcon(":/images/clear.png"));
	m_clearToolButton->setAutoRaise(true);
	
	mainLayout->addWidget(new QLabel(tr("Filter: ")));
	mainLayout->addWidget(m_lineEditFilter = new QLineEdit);
	connect(m_lineEditFilter, SIGNAL(textChanged(const QString &)),
			this, SLOT(filterTextChanged(const QString &)));
	QHBoxLayout *layout = new QHBoxLayout(m_lineEditFilter);
	layout->setMargin(2);
	QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
	layout->addItem(spacerItem);
	layout->addWidget(m_clearToolButton);
	
	connect(m_clearToolButton, SIGNAL(clicked()),
			m_lineEditFilter, SLOT(clear()));

	mainLayout->addWidget(new QLabel(tr("Column: ")));
	mainLayout->addWidget(m_comboBoxColumn = new QComboBox);
	connect(m_comboBoxColumn, SIGNAL(currentIndexChanged(int)),
			this, SLOT(columnIndexChanged(int)));
}

void FilterWidget::filterTextChanged(const QString &text)
{
	m_clearToolButton->setVisible(text != "");
	GenericSortModel *proxyModel = getProxyModel();
	if (!proxyModel)
		return;

	proxyModel->setFilterWildcard(text);
}

void FilterWidget::columnIndexChanged(int index)
{
	GenericSortModel *proxyModel = getProxyModel();
	if (!proxyModel)
		return;

	proxyModel->setFilterAny(!index);

	if (index)
		proxyModel->setFilterKeyColumn(index - 1);
}

void FilterWidget::setTreeView(QTreeView *treeView)
{
	m_treeView = treeView;

	GenericSortModel *proxyModel = getProxyModel();	
	if (!proxyModel)
		return;

	m_comboBoxColumn->clear();

	m_comboBoxColumn->addItem(tr("<Any>"));
	
	for (int i = 0; i < proxyModel->columnCount(); ++i)
	{
		m_comboBoxColumn->addItem(proxyModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
	}   
}

GenericSortModel *FilterWidget::getProxyModel() const
{
	if (!m_treeView)
		return 0;
	
	return qobject_cast<GenericSortModel*>(m_treeView->model());	
}
