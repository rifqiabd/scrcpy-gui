#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include "mainwindow.h"
#include "settingsmanager.h"

// Custom message handler to log to file
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
        fprintf(stderr, "%s\n", qPrintable(txt));
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageHandler);

    // Use QApplication for widget compatibility (dialogs)
    QApplication app(argc, argv);

    // Set application metadata
    QApplication::setApplicationName("Qt GUI Scrcpy");
    QApplication::setApplicationVersion("1.0.8");
    QApplication::setOrganizationName("Qt GUI Scrcpy");

    // Set platform-specific style
#ifdef Q_OS_WIN
    QQuickStyle::setStyle("FluentWinUI3");
    qDebug() << "Using FluentWinUI3 style for Windows";
#elif defined(Q_OS_MACOS)
    QQuickStyle::setStyle("macOS");
    qDebug() << "Using macOS native style";
#else
    QQuickStyle::setStyle("Material");
    qDebug() << "Using Material style for Linux";
#endif

    // Create QML engine
    QQmlApplicationEngine engine;

    // Create C++ backend objects
    MainWindow *mainWindow = new MainWindow(&app);
    SettingsManager *settingsManager = new SettingsManager(&app);

    // Expose C++ objects to QML
    engine.rootContext()->setContextProperty("mainWindow", mainWindow);
    engine.rootContext()->setContextProperty("appListModel", mainWindow->getAppListModel());
    engine.rootContext()->setContextProperty("settingsManager", settingsManager);

    // Load QML
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        qCritical() << "Failed to load QML";
        return -1;
    }

    qDebug() << "QML loaded successfully";

    return app.exec();
}
