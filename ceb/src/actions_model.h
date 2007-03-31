#ifndef ACTIONS_MODEL_H
#define ACTIONS_MODEL_H

#include <QAbstractListModel>

class ActionsModel : public QAbstractListModel
{
	Q_OBJECT
	
public:
	static const int columnAction = 0;
	static const int columnShortcut = 1;
	static const int columnsCount = 2;

	ActionsModel(QObject *parent = 0);

	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;	
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
};

#endif // ACTIONS_MODEL_H
