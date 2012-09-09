#include <QApplication>
#include <QtDebug>

#include "ServerExampleThreaded.h"

#include "Log.h"

void myMessageHandler( QtMsgType type, const char *msg )
{
	QString txt;

	switch (type)
	{
		case QtDebugMsg:
			txt = QString("%1").arg(msg);
			break;
		case QtWarningMsg:
			txt = QString("Warning: %1").arg(msg);
			break;
		case QtCriticalMsg:
			txt = QString("Critical: %1").arg(msg);
			break;
		case QtFatalMsg:
			txt = QString("Fatal: %1").arg(msg);
			abort();
			break;
	}

	Log::display(txt);
}


int main(int argc, char *argv[])
{
	qInstallMsgHandler(myMessageHandler);

	QApplication app(argc, argv);

	Log::display();

	Log::display( "main thread : " + QString::number((int)QThread::currentThreadId()) );

	ServerExampleThreaded myThreadedServer;

	return app.exec();
}
