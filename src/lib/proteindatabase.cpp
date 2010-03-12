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
  _fileName = folder + QDir::separator() + name + ".sqlite";
  //_fileName=name;
  if (!open())
    qDebug() << "could not open Database";
  else
  {
    qDebug() << "Datbase opened succesfully";
    createDatabase();
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
  _database = QSqlDatabase::addDatabase("QSQLITE");
  //_database=QSqlDatabase::addDatabase("QMYSQL");
  _database.setDatabaseName(_fileName);
  _database.setHostName("localhost");
  _database.setUserName("pepdb");
  _database.setPassword("sirtuin");
  return _database.open();
}

bool ProteinDatabase::createDatabase()
{
  qDebug() << "creating Database...";
  QFile sqlfile("/home/dave/Projekte/eclipse/PepDB/sql/createDatabase.sql");
  sqlfile.open(QIODevice::ReadOnly);
  QTextStream stream(&sqlfile);
  while (!stream.atEnd())
  {
    QSqlQuery query(stream.readLine());
    qDebug() << query.lastQuery();
    //query.next();
    qDebug() << query.value(0);
    qDebug() << "Fehler: " << query.lastError();
  }
  return true;
  //return query.exec();

}

bool ProteinDatabase::addDataSet(ProteinDataSet * data)
{
  QSqlQuery query("PRAGMA synchronous=OFF");
  query.exec(
      QString(
          "INSERT INTO uniprot ( dataset, created, modified, version ) VALUES ( \"%1\", \"%2\" , \"%3\" ,\"%4\");").arg(
          data->uniprot.dataset).arg(data->uniprot.created.toString()).arg(data->uniprot.modified.toString()).arg(
          data->uniprot.version));
  int uniprotid = query.lastInsertId().toInt();
  qDebug() << uniprotid;
  query.exec(
      QString(
          "INSERT INTO sequences ( sequence , checksum , modified , precursor , fragment , length , mass , version , uniprotid) VALUES (\"%1\",\"%2\",\"%3\",\"%4\",\"%5\",\"%6\",\"%7\",\"%8\",\"%9\");") .arg(
          data->sequence.sequence).arg(data->sequence.checksum).arg(
          data->sequence.modified.toString("yyyyMMdd")).arg(
          data->sequence.precursor).arg(data->sequence.fragment).arg(
          data->sequence.length).arg(data->sequence.mass) .arg(
          data->sequence.version).arg(uniprotid));
  foreach(QString acc, data->uniprot.accessionList)
  {
    query.exec(QString("INSERT INTO accession ( database, acc, uniprotid) VALUES (\"%1\", \"%2\", \"%3\");").arg(acc).arg(uniprotid));
  }
  foreach(QString name, data->uniprot.nameList)
  {
    query.exec(QString("INSERT INTO datasetnames ( name, uniprotid) VALUES (\"%1\",\"%2\");").arg(name).arg(uniprotid));
  }
  foreach(ProteinDataSet::ProteinName * name,data->proteinNameList)
  {
    query.exec(QString("INSERT INTO proteinnames (nameType, scope, ref, uniprotid) VALUES (\"%1\", \"%2\", \"%3\", \"%4\");")
        .arg(name->type).arg(name->scope).arg(name->ref).arg(uniprotid));

    int protnameid = query.lastInsertId().toInt();
//    query.exec(QString("INSERT INTO evidencednames (name, evidence, status, protNameId,length) VALUES (\"%1\", \"%2\", \"%3\", \"%4\", \"%5\");")
//        .arg(name->name.name).arg(name->fullName.evidence).arg(name->fullName.status).arg(protnameid).arg("full"));

//    query.exec(QString("INSERT INTO evidencednames (name, evidence, status, protNameId,length) VALUES (\"%1\", \"%2\", \"%3\", \"%4\", \"%5\");")
//        .arg(name->shortName.name).arg(name->shortName.evidence).arg(name->shortName.status).arg(protnameid).arg("short"));

  }
  foreach(ProteinDataSet::Feature * feature,data->featureList)
  {
    query.exec(QString("INSERT INTO positions (position,status) VALUES ( \"%1\",\"%2\");").arg(feature->location.begin.pos).arg(feature->location.begin.status));
    int beginid = query.lastInsertId().toInt();
    query.exec(QString("INSERT INTO positions (position,status) VALUES ( \"%1\",\"%2\");").arg(feature->location.end.pos).arg(feature->location.end.status));
    int endid = query.lastInsertId().toInt();
    query.exec(QString("INSERT INTO features (uniprotId , original ,  type , status , featureId , description , evidence , \"begin\", \"end\") VALUES (\"%1\", \"%2\", \"%3\", \"%4\", \"%5\",\"%6\", \"%7\", \"%8\", \"%9\");")
        .arg(uniprotid).arg(feature->original).arg(feature->type).arg(feature->status).arg(feature->id).arg(feature->description).arg(feature->evidence).arg(beginid).arg(endid));
    int featureid = query.lastInsertId().toInt();
    foreach(QString var, feature->variationList)
    {
      query.exec(QString("INSERT INTO variations (feature,variation) VALUES  ( \"%1\",\"%2\");").arg(featureid).arg(var));

    }

  }
  //qDebug() << query.lastQuery();
  //qDebug() << "Fehler: " << query.lastError();
  return true;
}

FeatureTableModel * ProteinDatabase::searchFeatures()
{
  QSqlQuery query("PRAGMA synchronous=OFF");
  query.exec("PRAGMA cache_size=2000000");
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
