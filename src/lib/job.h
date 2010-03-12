/*
 * Job.h
 *
 *  Created on: 17.02.2010
 *      Author: dave
 */

#ifndef JOB_H_
#define JOB_H_

#include <QObject>

namespace pepdb
{
/*!
 * @brief Job is a wrapper for processes running in background.
 *
 * Jobs are used to connect the application with background processes and should
 * be used as abstract layer between these. A job has a short description
 * for identifying running jobs by the user and an unique ID. The background
 * process can be started by calling the start(). Several signals and slots allow
 * communication with the process.
 */
class Job: public QObject
{
Q_OBJECT
public:
	/*!
	 * @brief Status of the background process
	 */
	enum Status
	{
		Waiting, //!<  Waiting for a request.
		Running, //!<  Process is running.
		Canceled, //!< Process is stopped and unfinished.
		Paused, //!< Process is paused.
		Finished //!<  Process is finished.
	};

	/*!
	 * @brief A Constructor.
	 *
	 * New Jobs should be constructed by the class responsible for the running
	 * the background process.
	 *
	 * @param description A short text describing the Job
	 * @param parent The parent class
	 */
	Job(const QString & description, QObject * parent = 0);

	/*!
	 * @brief The Destructor.
	 */
	virtual ~Job();

	/*!
	 * @brief The unique ID of the Job.
	 *
	 * @return Job ID
	 */
	int id() const;

	/*!
	 * @brief The description of the job.
	 *
	 * @return A string with the description
	 */
	QString description() const;

	/*!
	 * @brief The current status of the background process.
	 *
	 * @return The status
	 */
	Status status() const;

	/*!
	 * @brief Set the current status (by background process).
	 *
	 * @param status The status.
	 */
	void setStatus(Status status);

	/*!
	 * @brief The current status message.
	 *
	 * @return The message
	 */
	QString statusMessage() const;

	/*!
	 * @brief Set the current status message (by background process).
	 *
	 * @param statusMessage
	 */
	void setStatusMessage(const QString & statusMessage);

	/*!
	 * @brief True if the background can be paused and continued is possible.
	 *
	 * @return True if pausing is possible
	 */
	bool canPause() const;

	/*!
	 * @brief Set to true if pausing is possible (by background process).
	 *
	 * @param canContinue
	 */
	void setCanPause(bool canContinue);
public slots:
	/*!
	 * @brief Request start of process.
	 */
	void start();

	/*!
	 * @brief Request canceling of process.
	 */
	void cancel();

	/*!
	 * @brief Request pause of process.
	 */
	void pause();

	/*!
	 * @brief Request continue if paused.
	 */
	void continueJob();
signals:
	/*!
	 * @brief Process has started.
	 */
	void started();

	/*!
	 * @brief Process is finished.
	 */
	void finished();

	/*!
	 * @brief Process stopped unfinished.
	 */
	void canceled();

	/*!
	 * @brief The progress range for the process changed.
	 *
	 * @param min Minimum
	 * @param max Maximum
	 */
	void progressRangeChanged(int min, int max);

	/*!
	 * @brief Emitted whenever the progress of the process has changed.
	 *
	 * @param progress The new progress value
	 */
	void progressChanged(int progress);

	/*!
	 * @brief Emitted when the status message changes.
	 *
	 * @param statusMessage The new status Message
	 */
	void statusMessageChanged(const QString & statusMessage);

	/*!
	 * @brief Emitted when the status  of the background process changes.
	 *
	 * @param status The new status
	 */
	void statusChanged(Status status);

	/*!
	 * @brief Emitted when a process start is requested.
	 */
	void startRequested();

	/*!
	 * @brief Emitted when a process pause is requested.
	 */
	void pauseRequested();

	/*!
	 * @brief Emitted when a process cancel is requested.
	 */
	void cancelRequested();

	/*!
	 * @brief Emitted when a process continue is requested.
	 */
	void continueRequested();
private:
	int _id; //!<  The job ID
	QString _description; //!<  The job description.
	Status _status; //!<  The current status
	QString _statusMessage; //!<  The current status message
	bool _canPause; //!< True if the process can be paused

	static int s_nextId; //!< @brief The next free unique job ID
};

}

#endif /* JOB_H_ */
