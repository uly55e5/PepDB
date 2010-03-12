/*
 * pluginloader.h
 *
 *  Created on: 17.02.2010
 *      Author: dave
 */

#ifndef PLUGINLOADER_H_
#define PLUGINLOADER_H_

#include <QObject>
#include "interfaces/inputplugininterface.h"

namespace pepdb
{

/*!
 * @brief The class loads plugins for pepdb.
 */
class PluginLoader: public QObject
{
Q_OBJECT
public:
	/*!
	 * @brief The constructor.
	 */
	PluginLoader();

	/*!
	 * @brief The destructor.
	 */
	virtual ~PluginLoader();

	/*!
	 * @brief A list of all plugins implementing InputPluginInterface.
	 *
	 * If a file type is passed, then only input plugins for this file type are
	 * returned.
	 *
	 * @param fileType A string with the file type
	 * @return A list of pointers to root elements of plugins
	 */
	static QList<InputPluginInterface *> inputPlugins(const QString & fileType =
			QString());
private:
	/*!
	 * @brief Find All plugins in a directory.
	 *
	 * If no directory is passed, then _pluginDir is searched
	 *
	 * @param pluginDir A directory containing plugins
	 * @return A list of pointers to the root elements of all plugins
	 */
	static QList<QObject *> findPlugins(QString pluginDir = QString());
};

}

#endif /* PLUGINLOADER_H_ */
