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

    ProteinDataSet::EvidencedString readEvidencedString();
    ProteinDataSet::Location readLocation();
    ProteinDataSet::Position readPosition();

    void countEntries();
    void readEntries();

    void readEntry();
    void readAccession();
    void readName();
    void readProtein(const QString & scope = "protein");
    void readSequence();
    void readFeature();

    void readGene();
    void readOrganism();
    void readOrganismHost();
    void readGeneLocation();
    void readReference();
    void readComment();
    void readDbReference();
    void readProteinExistence();
    void readKeyword();
    void readEvidence();

    bool toBool(const QString & s);

    QFile * _file;
    QXmlStreamReader * _reader;
    ProteinDataSet * _currentDataset;
    ProteinDataSet::ProteinDataList * _dataList;
    int _entryCount;
};

}

#endif /* UNIPROTIXMLREADER_H_ */
