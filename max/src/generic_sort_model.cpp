#include "generic_sort_model.h"

GenericSortModel::GenericSortModel(QObject *parent) : QSortFilterProxyModel(parent),
		m_filterAny(true)
{
	setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void GenericSortModel::setFilterAny(bool value)
{
	if (m_filterAny == value)
		return;

	m_filterAny = value;

	clear();
}

bool GenericSortModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	if (!m_filterAny)
		return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);

	for (int i = 0; i < sourceModel()->columnCount(); ++i)
	{
		QModelIndex index = sourceModel()->index(sourceRow, i, sourceParent);

		if (sourceModel()->data(index).toString().contains(filterRegExp()))
			return true;
	}
	return false;
}
