/*
 * proteindatabase.h
 *
 *  Created on: 18.02.2010
 *      Author: dave
 */

#ifndef PROTEINDATABASE_H_
#define PROTEINDATABASE_H_

#include <QObject>

#include <QSqlDatabase>


#include "proteindataset.h"

namespace pepdb
{

class FeatureTableModel;

class ProteinDatabase: public QObject
{
    Q_OBJECT
public:
    ProteinDatabase(const QString & name, const QString & folder, QObject * parent=0);
    ProteinDatabase(const QString & dataBaseFile, QObject * parent = 0);
    virtual ~ProteinDatabase();
    bool open();
    bool addDataSet(ProteinDataSet * data);
    FeatureTableModel * searchFeatures();
    QString fileName();
    void setFileName(const QString & fileName);
private:
    bool createDatabase();
    QString _fileName;
    QSqlDatabase _database;
};

}

#endif /* PROTEINDATABASE_H_ */
