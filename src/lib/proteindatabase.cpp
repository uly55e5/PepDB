/*
 * proteindatabase.cpp
 *
 *  Created on: 18.02.2010
 *      Author: dave
 */

#include "proteindatabase.h"

#include <QDir>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlResult>
#include <QSqlRecord>

#include "featuretablemodel.h"

#include <QtDebug>

namespace pepdb
{

ProteinDatabase::ProteinDatabase(const QString & name, const QString & folder,
    QObject * parent) :
  QObject(parent)
{
  //_fileName = folder + QDir::separator() + name + ".sqlite";
  //_fileName=name;
  if (!open())
  {
    qDebug() << _database.lastError().text();
    qDebug() << "could not open Database";
  }
  else
  {
    qDebug() << "Datbase opened succesfully";
    prepareStatements();
  }

}

ProteinDatabase::ProteinDatabase(const QString & dataBaseFile, QObject * parent) :
  QObject(parent)
{
  _fileName = dataBaseFile;
  open();

}

ProteinDatabase::~ProteinDatabase()
{
  // TODO Auto-generated destructor stub
}

bool ProteinDatabase::open()
{
  _database=QSqlDatabase::addDatabase("QMYSQL");
  _database.setDatabaseName("pepdb");
  _database.setHostName("localhost");
  _database.setUserName("pepdb");
  _database.setPassword("pepdb");
  return _database.open();
}

bool ProteinDatabase::prepareStatements()
{
  qDebug() << "preparing statements...";
  QFile sqlfile("sql/insertIntoPepDB.sql");
  sqlfile.open(QIODevice::ReadOnly);
  QTextStream stream(&sqlfile);
  while (!stream.atEnd())
  {
    sendQuery(stream.readLine());
  }
  return true;
}

bool ProteinDatabase::addDataSet(ProteinDataSet * data)
{
    bindVar("uniDataset",data->uniprot.dataset);
    bindVar("uniCreated",data->uniprot.created.toString(Qt::ISODate));
    bindVar("uniModified",data->uniprot.modified.toString(Qt::ISODate));
    bindVar("uniVersion",QString().setNum(data->uniprot.version));
    bindVar("protExist",data->proteinExistence);
    int uniprotId = sendQuery("EXECUTE insert_uniprot USING @uniDataset, @uniCreated, @uniModified, @uniVersion, @protExist;");
    bindVar("uniId",QString().setNum(uniprotId));
    foreach(QString s, data->uniprot.accessionList)
    {
        bindVar("uniAcc",s);
        sendQuery("EXECUTE insert_uniprotaccession USING @uniId, @uniAcc;");
    }
    foreach(QString s, data->uniprot.nameList)
    {
        bindVar("uniName",s);
        sendQuery("EXECUTE insert_uniprotname USING @uniId, @uniName;");
    }


}


bool ProteinDatabase::bindVar(QString varname, QString value)
{
    return sendQuery(QString("SET @%1 = \"%2\";").arg(varname).arg(value));
}




int ProteinDatabase::sendQuery(QString q)
{
    QSqlQuery query(_database);
    if (! query.exec(q))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError().text();
        return false;
    }
    else
        return query.lastInsertId().toInt();
}

FeatureTableModel * ProteinDatabase::searchFeatures()
{
  QSqlQuery query(_database);
  //query.exec("PRAGMA cache_size=2000000");
  query.exec(
      "SELECT  d.name AS \"Uniprot-ID\",  e.name AS \"Protein-Name\", p1.position as \"Start\", p2.position as \"Ende\",\"sequence\" AS  \"Sequenz\" FROM  features f INNER JOIN positions p1 ON p1.id=f.begin INNER JOIN positions p2 ON p2.id=f.end INNER JOIN  proteinNames p ON  p.uniprotid=f.uniprotid INNER JOIN evidencednames e ON e.protNameId=p.id INNER JOIN sequences s ON s.uniprotid = f.uniprotid  INNER JOIN datasetnames d ON d.uniprotid=f.uniprotid WHERE p.nameType=\"recommended\" AND p.scope=\"protein\" AND e.length=\"full\" AND f.description = \"N6-acetyllysine\";");
  QSqlRecord r = query.record();
  qDebug() << "Columns:" << r.count();
  QFile data("out");
  if (data.open(QFile::WriteOnly | QFile::Truncate))
  {
    QTextStream out(&data);
    int i = 0;
    while (query.next())
    {
      QString sequence = query.value(4).toString();
      int start = query.value(2).toInt();
      int end = query.value(3).toInt();
      int pos = start - 8 < 0 ? 0 : start - 8;
      int length = end + 7 < sequence.length() ? end + 7 - pos
          : sequence.length() - pos;
      sequence = sequence.mid(pos, length);
      if (pos == 0)
        sequence = sequence.rightJustified(15, '.');
      else
        sequence = sequence.leftJustified(15, '.');
      out << i << ";" << query.value(0).toString() << ";" << query.value(
          1).toString() << ";" << query.value(2).toInt() << ";"
          << sequence << "\n";
      qDebug() << query.value(0).toString() << query.value(1).toString()
          << query.value(2).toInt() << sequence;
      i++;
    }
  }
  FeatureTableModel * model = new FeatureTableModel();
  model->setQuery(
      "SELECT  d.name AS \"Uniprot-ID\",  e.name AS \"Protein-Name\", p1.position as \"Start\", p2.position as \"Ende\",\"sequence\" AS  \"Sequenz\" FROM  features f INNER JOIN positions p1 ON p1.id=f.begin INNER JOIN positions p2 ON p2.id=f.end INNER JOIN  proteinNames p ON  p.uniprotid=f.uniprotid INNER JOIN evidencednames e ON e.protNameId=p.id INNER JOIN sequences s ON s.uniprotid = f.uniprotid  INNER JOIN datasetnames d ON d.uniprotid=f.uniprotid WHERE p.nameType=\"recommended\" AND p.scope=\"protein\" AND e.length=\"full\" AND f.description = \"N6-acetyllysine\";");
  int i = 0;
  while (model->canFetchMore())
  {
    model->fetchMore();
    qDebug() << "fetching" << i;
    i++;
  }
  return model;
}

QString ProteinDatabase::fileName()
{
  return _fileName;
}

void ProteinDatabase::setFileName(const QString & fileName)
{
  if (_fileName != fileName)
  {
    _database.close();
    _fileName = fileName;
    open();
  }
}
}
