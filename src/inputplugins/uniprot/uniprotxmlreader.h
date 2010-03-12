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

        enum AttributeType {
            String,
            Int,
            Float,
            Date
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
        QVariant readAttribute(const QString & attName, AttributeType type = String );

        void countEntries();
        void readEntries();
        void readEntry();
        void readAccessions();
        void readNames();
        void readProtein();
        void readProteinNames(const QString & scope = "protein");

        ProteinDataSet::EvidencedString * readEvidencedString(
                QXmlStreamReader * reader);
        void readProteinName(QXmlStreamReader * reader,
                             ProteinDataSet::ProteinName * protName);
        void readFeatureEntry(ProteinDataSet* data, QXmlStreamReader * reader);
        void readSequenceEntry(ProteinDataSet * data, QXmlStreamReader * reader);
        bool toBool(const QString & s);

        QFile * _file;
        QXmlStreamReader * _reader;
        ProteinDataSet * _dataset;
        int _entryCount;
        ProteinDataSet::ProteinDataList * _dataList;
    };

}

#endif /* UNIPROTIXMLREADER_H_ */
