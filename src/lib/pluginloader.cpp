/*
 * pluginloader.cpp
 *
 *  Created on: 17.02.2010
 *      Author: dave
 */

#include "pluginloader.h"

#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>
#include <QSettings>
#include <QtDebug>

namespace pepdb
{

PluginLoader::PluginLoader() :
    QObject()
{
}

PluginLoader::~PluginLoader()
{
}

QList<QObject *> PluginLoader::findPlugins(QString pluginDir)
{
    if (pluginDir.isEmpty()) // read standard path
    {
        QSettings settings;
        pluginDir = settings.value("paths/pluginpath",QCoreApplication::applicationDirPath() + QDir::separator() + "plugins" ).toString();
    }
    qDebug() << pluginDir;
    QList<QObject *> plugins;
    QDir pluginsDir(pluginDir);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
        {
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (plugin)
            {
                plugins.append(plugin);
            }
        }
    qDebug() << plugins.size() << "plugins found";
    return plugins;
}

QList<InputPluginInterface *> PluginLoader::inputPlugins(
        const QString & fileType)
{
    QList<InputPluginInterface *> inputList;
    QSettings settings;
    QString path = settings.value("paths/pluginpath",QCoreApplication::applicationDirPath() + QDir::separator() + "plugins").toString();
    QList<QObject *> plugins = findPlugins(path);
    if (plugins.size() > 0)
    {
        foreach(QObject * plugin, plugins)
            {
                InputPluginInterface * input = qobject_cast<
                        InputPluginInterface *> (plugin);
                if (input && (fileType.isEmpty()
                        || input->fileTypes().contains(fileType)))
                    inputList.append(input);
            }

    }
    qDebug() << inputList.size() << "input plugins found";
    return inputList;
}

}
