#include "jobstatuswidget.h"

#include "lib/job.h"

namespace pepdb
{
JobStatusWidget::JobStatusWidget(Job * job, QWidget *parent)
	: QWidget(parent)
{
	_job = job;
	ui.setupUi(this);

	ui.labelJobId->setText(QString().setNum(_job->id()));
	setToolTip(_job->description());

	ui.progressBar->setHidden(true);
	ui.labelCount->setHidden(true);

	connect(_job,SIGNAL(progressChanged(int)),SLOT(changeProgressValue(int)));
	connect(_job,SIGNAL(progressRangeChanged(int,int)),
			SLOT(changeProgressRange(int,int)));
	connect(_job,SIGNAL(statusMessageChanged(const QString &)),
			SLOT(changeStatusMessage(const QString & )));
}

JobStatusWidget::~JobStatusWidget()
{

}

void JobStatusWidget::changeProgressRange(int min, int max)
{
	ui.progressBar->setHidden(false);
	ui.labelCount->setHidden(true);
	ui.progressBar->setRange(min,max);

}

void JobStatusWidget::changeProgressValue(int val)
{
	if(ui.progressBar->isHidden())
	{	ui.labelCount->setHidden(false);
		ui.labelCount->setText(QString("%1").arg(val));
	}
	else
		ui.progressBar->setValue(val);
}

void JobStatusWidget::changeStatusMessage(const QString & status)
{
	ui.labelStatus->setText(status);
}

void JobStatusWidget::changeStatus(Job::Status status)
{

}
}
