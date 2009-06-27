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

#include "common.h"
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
