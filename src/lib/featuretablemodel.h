/*
 * FeatureTableModel.h
 *
 *  Created on: 26.02.2010
 *      Author: dave
 */

#ifndef FEATURETABLEMODEL_H_
#define FEATURETABLEMODEL_H_

#include <QSqlQueryModel>
namespace pepdb
{

class FeatureTableModel : public QSqlQueryModel
{
	Q_OBJECT
public:
	FeatureTableModel(QObject * parent = 0 );
	virtual ~FeatureTableModel();
	QVariant data(const QModelIndex &item, int role) const;
};
}
#endif /* FEATURETABLEMODEL_H_ */
