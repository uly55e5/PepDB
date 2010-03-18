/*
 * Project.cpp
 *
 *  Created on: 18.02.2010
 *      Author: dave
 */

#include "project.h"

#include <QDir>
#include <QFile>
#include <QSettings>

#include "job.h"
#include "pluginloader.h"
#include "proteindatabase.h"

#include "interfaces/inputplugininterface.h"

#include <QtDebug>

namespace pepdb
{
Project::Project(const QString & name, const QString & folder, QObject * parent) :
  QObject(parent)
{
  _name = name;
  _path = folder;
  if (!QFile::exists(folder))
    QDir().mkpath(folder);
  _database = new ProteinDatabase(name, folder);
  _created = QDate::currentDate();
  _modified = QDate::currentDate();
  writeProjectFile();
}

Project::Project(const QString & projectFile, QObject * parent) :
  QObject(parent)
{
  readProjectFile(projectFile);
}

Project::~Project()
{
  writeProjectFile();
}

Job * Project::addFileToDatabase(const QString & fileName,
    const QString & fileType)
{
  QList<InputPluginInterface *> plugins =
      PluginLoader::inputPlugins(fileType);
  if (plugins.size() > 0)
  {
    Job * job = plugins.first()->openFile(fileName);
    connect(job, SIGNAL(finished()), SLOT(addDataToDatabase()));
    return job;
  }
  else
    return 0;
}

void Project::addDataToDatabase()
{
  Job * job = qobject_cast<Job *> (sender());
  QObject * object = job->parent();
  InputPluginInterface * plugin = qobject_cast<InputPluginInterface *> (
      object);
  if (plugin && job != 0)
  {

    ProteinDataSet::ProteinDataList * data = plugin->data(job->id());
    foreach(ProteinDataSet * set, *data)
      {
        _database->addDataSet(set);
      }
  }
}

FeatureTableModel * Project::searchFeatures()
{
  return _database->searchFeatures();
}

void Project::writeProjectFile()
{
  QString fileName = _path + QDir::separator() + _name + ".project";
  QSettings settings(fileName, QSettings::IniFormat, this);
  settings.setIniCodec("UTF-8");
  settings.setValue("name", _name);
  settings.setValue("database", _database->fileName());
  settings.setValue("files", _fileList);
  settings.setValue("created", _created);
  settings.setValue("modified", _modified);
}

void Project::readProjectFile(const QString & settingsFile)
{
  QSettings settings(settingsFile, QSettings::IniFormat, this);
  settings.setIniCodec("UTF-8");
  _name = settings.value("name").toString();
  QString databaseFile = settings.value("database").toString();
  _database = new ProteinDatabase(databaseFile, this);
  _fileList = settings.value("files").toStringList();
  _created = settings.value("created").toDate();
  _modified = settings.value("modified").toDate();
}

}
