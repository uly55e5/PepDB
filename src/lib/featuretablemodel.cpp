/*
 * FeatureTableModel.cpp
 *
 *  Created on: 26.02.2010
 *      Author: dave
 */

#include "featuretablemodel.h"

#include <QModelIndex>
namespace pepdb {


FeatureTableModel::FeatureTableModel(QObject * parent) :
	QSqlQueryModel(parent)
{
	// TODO Auto-generated constructor stub

}

FeatureTableModel::~FeatureTableModel()
{
	// TODO Auto-generated destructor stub
}

QVariant FeatureTableModel::data(const QModelIndex &index, int role) const
{
	QVariant value = QSqlQueryModel::data(index, role);
	if (index.column() == 4 && role == Qt::DisplayRole)
	{
		int start = data(FeatureTableModel::index(index.row(),2),role).toInt();
		int end = data(FeatureTableModel::index(index.row(),3),role).toInt();
		QString sequence = value.toString().simplified();
		int pos = start-8 < 0 ? 0 : start-8 ;
		int length = end+7 < sequence.length() ? end+7-pos : sequence.length()-pos;
		value = sequence.mid(pos,length);
	}
	return value;
}
}
