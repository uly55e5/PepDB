#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QtGui/QDialog>
#include "ui_newprojectdialog.h"

#include <QMap>

class InputPluginInterface;

namespace pepdb
{

/*!
 * @brief The New Project Dialog.
 */
class NewProjectDialog: public QDialog
{

Q_OBJECT

public:
    /*!
     * \brief Constructor.
     *
     * @param parent The parent widget for the dialog
     */
    NewProjectDialog(QWidget *parent = 0);

    /*!
     * @brief Destructor.
     */
    ~NewProjectDialog();

    QString fileName();
    QString fileType();
    QString projectName();
    QString folder();

public slots:
    /*!
     * @brief Change the InputFile.
     */
    void browseInputFile();

    /*!
     * @brief Change the project Directory.
     */
    void browseProjectDir();

    /*!
     * @brief Call if Project Name changes.
     *
     * @param projectName New project name
     */
    void onProjectNameChange(const QString & projectName);

    /*!
     * @brief Call if Project older changed.
     *
     * @param projectFolder Current project folder.
     */
    void onProjectFolderEdit(const QString & projectFolder);

    void accept();

private:

    Ui::NewProjectDialogClass ui; //!< @brief The Qt Designer form

    bool _changedProjectDir; //!< @brief True if project Dir was changed by user
};
}

#endif // NEWPROJECTDIALOG_H
