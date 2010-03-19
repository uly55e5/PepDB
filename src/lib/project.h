/*
 * Project.h
 *
 *  Created on: 18.02.2010
 *      Author: dave
 */

#ifndef PROJECT_H_
#define PROJECT_H_

#include <QObject>

#include <QDate>
#include <QMap>
#include <QStringList>


namespace pepdb
{
class FeatureTableModel;
class InputPluginInterface;
class Job;
class ProteinDatabase;

/*!
 * @brief This class manages pepdb projects
 *
 * A project contains preferences, database connections, input and result files.
 */
class Project: public QObject
{
    Q_OBJECT
public:
    /*!
     * @brief Constructor for a new Project.
     *
     * @param name Project name
     * @param path Project path
     * @param parent Parent object.
     */
    Project(const QString & name, const QString & path, QObject * parent=0);

    /*!
     * @brief Constructor for opening an existing project file.
     *
     * @param projectFile Project file
     * @param parent Parent object.
     */
    Project(const QString & projectFile, QObject * parent=0);

    /*!
     * @brief Destructor.
     */
    virtual ~Project();


    /*!
     *
     */
    Job * addFileToDatabase(const QString & fileName, const QString & fileType);
public slots:
    void addDataToDatabase();
    FeatureTableModel * searchFeatures(); //TODO
    void batchSearch();
private:
    void writeProjectFile();
    void readProjectFile(const QString & projectFile);
    QStringList _fileList;
    QString _name;
    QString _path;
    QDate _modified;
    QDate _created;
    ProteinDatabase * _database;
    InputPluginInterface * _currentInputPlugin;
};
}
#endif /* PROJECT_H_ */
