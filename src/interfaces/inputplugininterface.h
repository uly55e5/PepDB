/*
 * inputplugininterface.h
 *
 *  Created on: 16.02.2010
 *      Author: dave
 */

#ifndef INPUTPLUGININTERFACE_H_
#define INPUTPLUGININTERFACE_H_

#include <QtPlugin>

#include "lib/job.h"
#include "lib/proteindataset.h"

namespace pepdb
{

class InputPluginInterface
{
public:
  virtual ~InputPluginInterface() {};
  virtual const QStringList fileTypes()=0;
  virtual Job * openFile(const QString & fileName) = 0;
  virtual ProteinDataSet::ProteinDataList * data(int jobId) = 0;

};

}

Q_DECLARE_INTERFACE(pepdb::InputPluginInterface,"sisyphos.pepdb.inputplugininterface/0.1")



#endif /* INPUTPLUGININTERFACE_H_ */
