#include "mainwindow.h"

#include <QFileDialog>
#include <QSettings>

#include "jobstatuswidget.h"
#include "newprojectdialog.h"
#include "resulttable.h"
#include "searchwidget.h"

#include "lib/job.h"
#include "lib/project.h"
#include "lib/featuretablemodel.h"

#include <QDebug>

namespace pepdb
{
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.actionNewProject,SIGNAL(activated()),SLOT(newProject()));
    connect(ui.actionCloseProject,SIGNAL(activated()),SLOT(closeProject()));
    connect(ui.actionOpenProject,SIGNAL(activated()),SLOT(openProject()));

    connect(ui.btnFeatureSeaurch,SIGNAL(clicked()),SLOT(featureSearch()));
    connect(ui.btnSearchBatch,SIGNAL(clicked()),SLOT(batchSearch()));
    connect(ui.btnSearch,SIGNAL(clicked()),SLOT(newSearch()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::newProject()
{
    NewProjectDialog npd(this);
    if ( npd.exec() == QDialog::Accepted )
    {
        Project * project = new Project(npd.projectName(), npd.folder(),this);
        _currentProject = project;
        Job * job = project->addFileToDatabase(npd.fileName(),npd.fileType());
        JobStatusWidget * status = new JobStatusWidget(job);
        ui.layoutJobs->insertWidget(ui.layoutJobs->count()-1, status);
        job->start();
    }

}

void MainWindow::openProject()
{
    QSettings settings;
    QString path = settings.value("paths/projectroot",QDir::homePath()).toString();
    QString fileName = QFileDialog::getOpenFileName(this,"Open Project",path,"Projects (*.project)");
    _currentProject = new Project(fileName,this);

}

void MainWindow::closeProject()
{
    delete _currentProject;

}

void MainWindow::featureSearch()
{
    ResultTable * result = new ResultTable();
    result->setModel(_currentProject->searchFeatures());
    ui.centralLayout->addWidget(result);

}

void MainWindow::batchSearch()
{
    _currentProject->batchSearch();
}

void MainWindow::newSearch()
{
    SearchWidget * sw = new SearchWidget(this);
    ui.centralLayout->addWidget(sw);
    sw->show();
}

}
