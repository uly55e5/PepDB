#ifndef UNIPROTXMLREADER_H_
#define UNIPROTXMLREADER_H_

#include <QThread>
#include <QVariant>

#include "lib/proteindataset.h"

class QFile;
class QXmlStreamReader;

namespace pepdb
{

class UniprotXmlReader: public QThread
{
Q_OBJECT

    enum AttributeType
    {
        String, Int, Float, Date, Bool
    };

public:
    UniprotXmlReader(const QString & filename, QObject * parent);
    ~UniprotXmlReader();
    void run();
    ProteinDataSet::ProteinDataList * data();
signals:
    void finished();
    void statusChanged(const QString & statusMessage);
    void progressChanged(int val);
    void progressRangeChanged(int min, int max);

private:
    QVariant
            readAttribute(const QString & attName, AttributeType type = String);
    ProteinDataSet::EvidencedString * readEvidencedString();
    ProteinDataSet::Location * readLocation();
    ProteinDataSet::Position * readPosition();

    void countEntries();
    void readEntries();

    void readEntry();
    void readAccessions();
    void readNames();
    void readProtein();
    void readProteinNames(const QString & scope = "protein");
    void readSequence();
    void readFeatures();
    void readGenes();
    void readOrganism();
    void readOrganismHosts();
    void readGeneLocations();
    void readReferences();
    void readComments();
    void readDbReferences();
    void readProteinExistence();
    void readKeywords();
    void readEvidences();

    bool toBool(const QString & s);

    QFile * _file;
    QXmlStreamReader * _reader;
    ProteinDataSet * _dataset;
    int _entryCount;
    ProteinDataSet::ProteinDataList * _dataList;
};

}

#endif /* UNIPROTIXMLREADER_H_ */
