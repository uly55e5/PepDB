/*
 * UniprotInputPlugin.cpp

 *
 *  Created on: 17.02.2010
 *      Author: dave
 */

#include "uniprotinputplugin.h"

#include "uniprotxmlreader.h"

namespace pepdb
{

UniprotInputPlugin::UniprotInputPlugin()
{
    // TODO Auto-generated constructor stub

}

UniprotInputPlugin::~UniprotInputPlugin()
{
    // TODO Auto-generated destructor stub
}

Job * UniprotInputPlugin::openFile(const QString & fileName)
{
    Job * job = new Job(QString("Open file \"%1\"").arg(fileName),this);
    UniprotXmlReader * reader = new UniprotXmlReader(fileName, this);
    _jobMap.insert(job->id(),reader);

    connect(reader, SIGNAL(statusChanged(const QString &)), job,
            SIGNAL(statusMessageChanged(const QString &)));
    connect(reader, SIGNAL(progressChanged(int)), job,
            SIGNAL(progressChanged(int)));
    connect(reader, SIGNAL(progressRangeChanged(int,int)), job,
            SIGNAL(progressRangeChanged(int,int)));
    connect(reader, SIGNAL(finished()),job,SIGNAL(finished()));
    connect(job, SIGNAL(startRequested()), reader, SLOT(start()));

    return job;
}

ProteinDataSet::ProteinDataList * UniprotInputPlugin::data(int jobId)
{
   return _jobMap.value(jobId)->data();

}

const QStringList UniprotInputPlugin::fileTypes()
{
    return QStringList("Uniprot XML");
}

Q_EXPORT_PLUGIN2(uniprotinput,pepdb::UniprotInputPlugin)



}
