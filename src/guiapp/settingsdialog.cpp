#include "settingsdialog.h"

#include <QSettings>
#include <QDir>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    readSettings();
}

SettingsDialog::~SettingsDialog()
{

}

void SettingsDialog::readSettings()
{
    QSettings settings;
    ui.editDbName->setText( settings.value("database/name","pepdb").toString() );
    ui.editDbUser->setText( settings.value("database/user","pepdb").toString() );
    ui.editDbHost->setText( settings.value("database/host","localhost").toString() );
    ui.editDbPassword->setText( settings.value("database/password","").toString() );
    ui.boxDatabase->setCurrentIndex(ui.boxDatabase->findText(settings.value("database/type","MySql").toString()));
    ui.boxPassword->setChecked( settings.value("database/storepwd",true).toBool());

    ui.editProjectRoot->setText( settings.value("paths/projectroot",QDir::homePath()).toString());
    ui.editPluginPath->setText( settings.value("paths/pluginpath",QCoreApplication::applicationDirPath() + QDir::separator() + "plugins").toString());
}

void SettingsDialog::writeSettings()
{
    QSettings settings;
    settings.setValue("database/name",ui.editDbName->text());
    settings.setValue("database/user",ui.editDbUser->text());
    settings.setValue("database/host",ui.editDbHost->text());
    settings.setValue("database/storepwd",ui.boxPassword->isChecked());
    settings.setValue("database/type",ui.boxDatabase->currentText());
    if(ui.boxPassword->isChecked())
        settings.setValue("database/password",ui.editDbPassword->text());
    else
        settings.setValue("database/password","");

    settings.setValue("paths/projectroot",ui.editProjectRoot->text());
}
