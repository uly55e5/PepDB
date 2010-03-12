/*
 * Job.cpp
 *
 *  Created on: 17.02.2010
 *      Author: dave
 */

#include "job.h"

#include <QSettings>

namespace pepdb
{

int Job::s_nextId = 0;

Job::Job(const QString & description, QObject * parent) :
	QObject(parent)
{
	_description = description;
	_canPause = false;
	_status = Waiting;
	_statusMessage = tr("Warte auf Start...");

	if (s_nextId == 0)
	{
		// get ID from application settings
		QSettings settings;
		s_nextId = settings.value("general/nextJobId", 1).toInt();
	}
	_id = s_nextId++;
}

Job::~Job()
{
	// write ID to apllication settings
	QSettings settings;
	settings.setValue("general/nextJobId", s_nextId);
}

QString Job::description() const
{
	return _description;
}

int Job::id() const
{
	return _id;
}

void Job::start()
{
	emit startRequested();
}

void Job::cancel()
{
	emit cancelRequested();
}

void Job::continueJob()
{
	emit continueRequested();
}

void Job::pause()
{
	emit pauseRequested();
}

bool Job::canPause() const
{
	return _canPause;
}

Job::Status Job::status() const
{
	return _status;
}

void Job::setCanPause(const bool canContinue)
{
	_canPause = canContinue;
}

void Job::setStatus(const Job::Status status)
{
	_status = status;
	emit statusChanged(status);
}

void Job::setStatusMessage(const QString & statusMessage)
{
	_statusMessage = statusMessage;
	emit statusMessageChanged(statusMessage);
}
}
