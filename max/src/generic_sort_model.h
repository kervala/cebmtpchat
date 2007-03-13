#ifndef GENERIC_SORT_MODEL_H
#define GENERIC_SORT_MODEL_H

#include <QSortFilterProxyModel>

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
