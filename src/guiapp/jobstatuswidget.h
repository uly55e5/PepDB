#ifndef JOBSTATUSWIDGET_H
#define JOBSTATUSWIDGET_H

#include <QtGui/QWidget>
#include "ui_jobstatuswidget.h"

#include "lib/job.h"

namespace pepdb
{

class JobStatusWidget : public QWidget
{
    Q_OBJECT

public:
    JobStatusWidget(Job * job, QWidget *parent = 0);
    ~JobStatusWidget();
public slots:
	void changeProgressRange(int min, int max);
	void changeProgressValue(int val);
	void changeStatusMessage(const QString & status);
	void changeStatus(Job::Status status);
private:
    Ui::JobStatusWidgetClass ui;
    Job * _job;
};

}

#endif // JOBSTATUSWIDGET_H
