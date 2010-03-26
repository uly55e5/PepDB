#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_mainwindow.h"

namespace pepdb
{

class Project;

/*!
 * \brief Mainwindow of the PepDB App.
 */
class MainWindow: public QMainWindow
{

Q_OBJECT

public:
    /*!
     * \brief Constructor.
     *
     * @param parent The parent widget.
     */
    MainWindow(QWidget *parent = 0);

    /*!
     * \brief Destructor.
     */
    ~MainWindow();

public slots:

    /*!
     * \brief Create a new Project.
     */
    void newProject();

    /*!
     * \brief Open a existing Project.
     */
    void openProject();

    /*!
     * \brief Close Current Project.
     */
    void closeProject();

    void settingsDialog();
    void applySettings();
    void featureSearch();
    void batchSearch();
    void newSearch();
private:
    Ui::MainWindowClass ui; //!< The Qt designer form.
    Project * _currentProject;
};
}

#endif // MAINWINDOW_H
