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

#ifndef GENERIC_SORT_MODEL_H
#define GENERIC_SORT_MODEL_H

class GenericSortModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	GenericSortModel(QObject *parent = 0);

	bool filterAny() const { return m_filterAny; }
	void setFilterAny(bool value);

protected:
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
	bool m_filterAny;
};

#endif
