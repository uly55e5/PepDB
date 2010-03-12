#include "newprojectdialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include "interfaces/inputplugininterface.h"
#include "lib/pluginloader.h"

namespace pepdb
{

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);
    _changedProjectDir = false;

    QList<InputPluginInterface *> pluginsList = PluginLoader::inputPlugins();
    foreach( InputPluginInterface * plugin, pluginsList )
        {
            ui.boxFileType->addItems(plugin->fileTypes());
        }

    QSettings settings;
    ui.lineEditProjectFolder->setText(settings.value("paths/projectroot",
            QDir::homePath()).toString());

    connect(ui.buttonProjectFolder, SIGNAL(clicked()), SLOT(browseProjectDir()));
    connect(ui.buttonInputFileName, SIGNAL(clicked()), SLOT(browseInputFile()));
    connect(ui.lineEditProjectName, SIGNAL(textChanged(const QString& )),
            SLOT(onProjectNameChange(const QString&)));
    connect(ui.lineEditProjectFolder, SIGNAL(textEdited(const QString&)),
            SLOT(onProjectFolderEdit(const QString&)));
}

NewProjectDialog::~NewProjectDialog()
{

}

void NewProjectDialog::browseInputFile()
{
    ui.lineEditInputFile->setText(QFileDialog::getOpenFileName(this, tr(
            "Select Input File"), QDir::homePath()));

}

void NewProjectDialog::browseProjectDir()
{

    ui.lineEditProjectFolder->setText(QFileDialog::getExistingDirectory(this,
            tr("Select Project Directory"), QDir::homePath()));
    _changedProjectDir = true;
}

void NewProjectDialog::onProjectFolderEdit(const QString & folder)
{
    _changedProjectDir = true;
}

void NewProjectDialog::onProjectNameChange(const QString & projectName)
{
    if (!_changedProjectDir)
    {
        QSettings settings;
        ui.lineEditProjectFolder->setText(settings.value("paths/projectroot",
                QDir::homePath()).toString() + QDir::separator() + projectName);
    }
}

void NewProjectDialog::accept()
{
    if (ui.lineEditProjectName->text().isEmpty()
            || ui.lineEditProjectFolder->text().isEmpty()
            || (ui.lineEditInputFile->text().isEmpty()
                    && !ui.boxEmptyProject->isChecked()))
    {
        QMessageBox(
                QMessageBox::Warning,
                tr("Fill in fields"),
                tr(
                        "The fields for name and folder are mandatory. If a non-empty project is created fill in the iput file field."),
                QMessageBox::Close).exec();
        return;
    }
    QDialog::accept();
}

QString NewProjectDialog::projectName()
{
    return ui.lineEditProjectName->text();

}

QString NewProjectDialog::fileName()
{
    return ui.lineEditInputFile->text();
}

QString NewProjectDialog::fileType()
{
    return ui.boxFileType->currentText();
}

QString NewProjectDialog::folder()
{
    return ui.lineEditProjectFolder->text();
}
}
