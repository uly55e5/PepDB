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
#include <QBitArray>


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
    int sendQuery(QString q);
    void batchSearch();
private:
    bool bindVar(QString varname, QString value);
    bool prepareStatements();
    QString boolToString(bool b);
    QString _fileName;
    QSqlDatabase _database;
    QString sequenceAroundPosition(int pos, QString seq, int flank);

    struct Result {
        QString uniprot;
        QString name;
        int pos;
        QString sequence;
        QBitArray mods;
    };
};

}

#endif /* PROTEINDATABASE_H_ */
