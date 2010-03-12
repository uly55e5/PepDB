/*
 * main.cpp
 *
 *  Created on: 16.02.2010
 *      Author: dave
 */

#include "mainwindow.h"

#include <QTextCodec>

/*!
 * @mainpage PepDB App
 *
 * @brief PepDB App is a application using the PepDB libraries.
 * @author David Rauh
 *
 * PepDB App is used to search in Peptid Databases and output the results in a human readable presentation.
 *
 */

/*!
 * @namespace pepdb
 * @brief All classes related to PepDB.
 */
using namespace pepdb;

/*!
 * @brief Entry point of PepDB App.
 *
 * @param argc Argument count
 * @param argv Arguments
 * @return Result value, 0 on success
 */
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QCoreApplication::setOrganizationName("Sisyphos Soft");
	QCoreApplication::setApplicationName("PepDB App");
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	MainWindow mw;
	mw.show();

	return app.exec();
}

