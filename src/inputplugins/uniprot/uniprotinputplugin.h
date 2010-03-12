/*
 * UniprotInputPlugin.h
 *
 *  Created on: 17.02.2010
 *      Author: dave
 */

#ifndef UNIPROTINPUTPLUGIN_H_
#define UNIPROTINPUTPLUGIN_H_

#include <QObject>
#include "../../interfaces/inputplugininterface.h"

namespace pepdb
{
class UniprotXmlReader;

class UniprotInputPlugin: public QObject, public InputPluginInterface
{
Q_OBJECT
Q_INTERFACES( pepdb::InputPluginInterface)

typedef QMap<int,UniprotXmlReader*> JobMap;

public:
    UniprotInputPlugin();
    virtual ~UniprotInputPlugin();

    const QStringList fileTypes();
    Job * openFile(const QString & fileName);
    ProteinDataSet::ProteinDataList * data(int jobId);
signals:
    void progressChanged(int val);
    void progressRangeChanged(int min, int max);
private:
    JobMap _jobMap;

};

}

#endif /* UNIPROTINPUTPLUGIN_H_ */
