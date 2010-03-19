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
    _database = QSqlDatabase::addDatabase("QMYSQL");
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
    bindVar("uniDataset", data->uniprot.dataset);
    bindVar("uniCreated", data->uniprot.created.toString(Qt::ISODate));
    bindVar("uniModified", data->uniprot.modified.toString(Qt::ISODate));
    bindVar("uniVersion", QString().setNum(data->uniprot.version));
    bindVar("protExist", data->proteinExistence);
    int
            uniprotId =
                    sendQuery(
                            "EXECUTE insert_uniprot USING @uniDataset, @uniCreated, @uniModified, @uniVersion, @protExist;");
    bindVar("uniId", QString().setNum(uniprotId));
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
    bindVar("sequence", data->sequence.sequence);
    bindVar("seqLength", QString().setNum(data->sequence.length));
    bindVar("seqMass", QString().setNum(data->sequence.mass));
    bindVar("seqCheck", data->sequence.checksum);
    bindVar("seqModified", data->sequence.modified.toString(Qt::ISODate));
    bindVar("seqVersion", QString().setNum(data->sequence.version));
    bindVar("seqPrecursor", boolToString(data->sequence.precursor));
    bindVar("seqFragment", data->sequence.fragment);
    sendQuery(
            "EXECUTE insert_sequence USING @uniId, @sequence, @seqLength, @seqMass,@seqCheck,@seqModified,@seqVersion,@seqPrecursor,@seqFragment;");
foreach(ProteinDataSet::ProteinName * name, data->proteinNameList)
{
    bindVar("pnScope", name->scope);
    bindVar("pnType", name->type);
    bindVar("pnRef", name->ref);
    bindVar("fullName", name->fullName.string);
    bindVar("fullEvidence", name->fullName.evidence);
    bindVar("fullStatus", name->fullName.status);
    int fullId = sendQuery("EXECUTE insert_evidenced USING @fullName, @fullEvidence, @fullStatus;");
    bindVar("fullId",QString().setNum(fullId));
    int protNameId = sendQuery("EXECUTE insert_proteinnames USING @uniId,@pnScope, @pnType, @pnRef, @fullId;");
    bindVar("pnId", QString().setNum(protNameId));
    foreach(ProteinDataSet::EvidencedString * shortName, name->shortNameList)
    {
        bindVar("shortName", shortName->string);
        bindVar("shortEvidence", shortName->evidence);
        bindVar("shortStatus", shortName->status);
        int shortId = sendQuery("EXECUTE insert_evidenced USING @shortName, @shortEvidence, @shortStatus;");
        bindVar("shortId",QString().setNum(shortId));
        sendQuery("EXECUTE insert_shortName USING @pnId, @shortId;");
    }

}
foreach(ProteinDataSet::Gene * gene, data->geneList)
{
    int geneId = sendQuery("EXECUTE insert_gene USING @uniId;");
    bindVar("geneId",QString().setNum(geneId));
    foreach(ProteinDataSet::GeneName * name, gene->geneNameList)
    {
        bindVar("gnName",name->name);
        bindVar("gnEvidence",name->evidence);
        bindVar("gnType",name->type);
        sendQuery("EXECUTE insert_genename USING @geneId, @gnName, @gnEvidence, @gnType;");
    }
}
foreach(ProteinDataSet::Feature * feature, data->featureList)
{
    bindVar("beginPos",QString().setNum(feature->location.begin.pos));
    bindVar("beginStat",feature->location.begin.status);
    bindVar("beginEvidence",feature->location.begin.evidence);
    int beginId = sendQuery("EXECUTE insert_position USING @beginPos, @beginStatus, @beginEvidence;");
    bindVar("beginId", QString().setNum(beginId));

    bindVar("endPos",QString().setNum(feature->location.end.pos));
    bindVar("endStat",feature->location.end.status);
    bindVar("endEvidence",feature->location.end.evidence);
    int endId = sendQuery("EXECUTE insert_position USING @endPos, @endStatus, @endEvidence;");
    bindVar("endId", QString().setNum(endId));

    bindVar("fSequence",feature->location.sequence);
    int locId = sendQuery("EXECUTE insert_location USING @beginId, @endId, @fSequence;");
    bindVar("fLocId",QString().setNum(locId));

    bindVar("fOrignal",feature->original);
    bindVar("fType",feature->type);
    bindVar("fStatus",feature->status);
    bindVar("fId",feature->id);
    bindVar("fDesc",feature->description.remove('"'));
    bindVar("fEvidence",feature->evidence);
    bindVar("fRef",feature->ref);

    int featureId = sendQuery("EXECUTE insert_feature USING @uniId, @fOriginal, @fLocId, @fType, @fStatus, @fId, @fDesc, @fEvidence, @fRef;");
    bindVar("featId",QString().setNum(featureId));
    foreach(QString s, feature->variationList)
    {
        bindVar("fVar",s);
        sendQuery("EXECUTE insert_variation USING @featId, @fVar;");
    }
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
        "SELECT  d.name AS UniprotID,  e.name AS ProteinName, p1.position as Start, p2.position as Ende, s.sequence AS  Sequenz FROM  features f INNER JOIN locations l ON l.locationId=f.locationId INNER JOIN positions p1 ON p1.positionId=l.beginId INNER JOIN positions p2 ON p2.positionId=l.endId INNER JOIN  proteinNames p ON  p.uniprotid=f.uniprotid INNER JOIN evidencedStrings e ON e.evidencedstringId=p.fullNameId INNER JOIN sequences s ON s.uniprotid = f.uniprotid  INNER JOIN uniprotNames d ON d.uniprotid=f.uniprotid WHERE p.type=\"recommendedName\" AND p.scope=\"protein\" AND f.description =\"N6-acetyllysine\"");
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

void ProteinDatabase::batchSearch()
{
struct Result
{
    int count;
    QList<int> posList;
    QStringList seqList;
    QString name;
    QString originalSeq;
    QString uni;
};

sendQuery("PREPARE select_seqs FROM \"select DISTINCT un.name, s.sequence, e.name, p.type from sequences s INNER JOIN uniprot u ON u.uniprotId=s.uniprotId INNER JOIN uniprotnames un ON un.uniprotid=s.uniprotid INNER JOIN proteinNames p ON p.uniprotId=s.uniprotId INNER JOIN evidencedstrings e ON e.evidencedstringid=p.fullNameId where p.scope='protein' AND s.sequence like ?\";");
sendQuery("PREPARE select_seqs FROM \"select DISTINCT un.name, s.sequence from sequences s INNER JOIN uniprotnames un ON un.uniprotid=s.uniprotid  where s.sequence like ?\";");


QFile file("testdata/test.csv");
file.open(QIODevice::ReadOnly);
QTextStream stream(&file);
int i=1;
QFile data("out2");
data.open(QFile::WriteOnly | QFile::Truncate);
QTextStream outStream(&data);
while(!stream.atEnd())
{

    Result result;
    QString line = stream.readLine();
    QStringList list = line.split('|');
    if(list.size() == 3 )
    {
        result.originalSeq = list[2];
        result.name = list[1];
        result.count = i;
        QString seqStar = list[2].remove(QRegExp("[.\\-]"));
        QString name = list[1];
        while(seqStar.contains('*'))
        {
            int pos = seqStar.indexOf('*');
            result.posList.append(pos);
            seqStar.remove(pos,1);
        }
        bindVar("seq","%"+seqStar+"%");
        bindVar("name",name);
        QSqlQuery query(_database);
        if (! query.exec("EXECUTE select_seqs USING @seq"))
        {
            qDebug() << query.lastQuery();
            qDebug() << query.lastError().text();
        }
        else
        {

            if (query.size() < 1)
            {
                int j=0;
                QString inv;
                while (j<seqStar.size())
                {
                    inv.prepend(seqStar[j]);
                    j++;
                }

                bindVar("seq","%"+inv+"%");
                if (! query.exec("EXECUTE select_seqs USING @seq"))
                    qDebug() << "Fehler";
                if(query.size() < 1 );
                else
                {
                    seqStar = inv;
                    for(int i=0; i < result.posList.size(); i++)
                    {
                        result.posList[i] = seqStar.size()-result.posList[i]+1;
                    }

                }

            }
            while (query.next())
            {
                QString sequence = query.value(1).toString();
                int start = sequence.indexOf(seqStar);
                for(int i=0; i < result.posList.size(); i++)
                {
                    result.uni=query.value(0).toString();
                    int lys=start+result.posList[i];
                    int pos =lys - 7 < 0 ? 0 : lys - 7;
                    int length = lys + 6 < sequence.length() ? lys + 6 - pos
                    : sequence.length() - pos;
                    QString out = sequence.mid(pos, length);
                    if (pos == 0)
                    out = out.rightJustified(13, '.');
                    else
                    out = out.leftJustified(13, '.');
                    if(!result.seqList.contains(out))
                    {
                        result.seqList.append(out);

                        outStream << result.count << "|"  << result.name << "|"
                                << result.originalSeq << "|" << result.uni << "|"<< out;
                        qDebug() << result.count << "|"  << result.name << "|"
                               << result.originalSeq << "|" << result.uni << "|"<< out;
                    }

                }

            }
        }
        if(result.seqList.size() == 0 )
        {
            outStream << result.count << "|"  << result.name << "|"
                                            << result.originalSeq;
            qDebug() << result.count << "|"  << result.name << "|"
                                           << result.originalSeq;
        }


        i++;
    }
}

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

QString ProteinDatabase::boolToString(bool b)
{
if (b)
return "1";
else
return "0";
}

}
