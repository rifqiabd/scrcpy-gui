#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include "mainwindow.h"

// Custom message handler to log to file and show alerts
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Get log file path
    QString logPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(logPath);
    if (!dir.exists()) dir.mkpath(".");
    QString logFile = logPath + "/scrcpy-gui.log";

    // Format message
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtInfoMsg:
        txt = QString("Info: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        break;
    }

    // Write to file
    QFile outFile(logFile);
    if (outFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream ts(&outFile);
        ts << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ") << txt << Qt::endl;
    }

    // Show alert for critical/fatal errors
    if (type == QtFatalMsg || type == QtCriticalMsg) {
        // Use native message box if possible, or simple stderr if GUI not ready
        fprintf(stderr, "%s\n", qPrintable(txt));
        // Note: Cannot safely use QMessageBox in all contexts, but helpful for startup errors
        // if QApplication is running.
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageHandler);

    QApplication app(argc, argv);

    // Set application metadata
    QApplication::setApplicationName("Qt GUI Scrcpy");
    QApplication::setApplicationVersion("1.0.6");
    QApplication::setOrganizationName("Qt GUI Scrcpy");

    MainWindow window;
    window.show();

    return app.exec();
}
