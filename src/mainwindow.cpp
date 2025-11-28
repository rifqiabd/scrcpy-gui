#include "mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <QSettings>
#include <QDateTime>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickWindow>

MainWindow::MainWindow(QObject *parent)
    : QObject(parent)
    , appManager(new AppManager(this))
    , appListModel(new AppListModel(this))
    , scrcpyProcess(nullptr)
    , showRunningOnly(false)
    , m_statusText("Ready")
    , m_scrcpyStatusText("No scrcpy running")
    , m_scrcpyStatusColor("gray")
    , m_scrcpyRunning(false)
{
    // Connect signals from app manager
    connect(appManager, &AppManager::appsLoaded, this, &MainWindow::onAppsLoaded);
    connect(appManager, &AppManager::loadError, this, &MainWindow::onLoadError);
    connect(appManager, &AppManager::runningAppsLoaded, this, &MainWindow::onRunningAppsLoaded);
}

MainWindow::~MainWindow()
{
    stopScrcpy();
}

void MainWindow::setStatusText(const QString &text)
{
    if (m_statusText != text) {
        m_statusText = text;
        emit statusTextChanged();
    }
}

void MainWindow::windowReady()
{
    qDebug() << "Window ready, loading app list";
    loadAppList();
}

void MainWindow::loadAppList()
{
    setStatusText("Loading apps...");
    appListModel->clear();
    appManager->loadApps();
}

void MainWindow::onAppSelected(const QString &packageName, const QString &appName)
{
    qDebug() << "Launching scrcpy for:" << packageName;

    // Stop current scrcpy if running
    if (scrcpyProcess && scrcpyProcess->state() == QProcess::Running) {
        appendLog("Stopping current scrcpy session...", "#ff9800");
        stopScrcpy();
    }

    launchScrcpy(packageName, appName);
}

void MainWindow::launchScrcpy(const QString &packageName, const QString &appName)
{
    currentPackageName = packageName;
    currentAppName = appName;
    
    if (scrcpyProcess) {
        delete scrcpyProcess;
    }

    scrcpyProcess = new QProcess(this);

    // Connect signals
    connect(scrcpyProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onScrcpyFinished);
    connect(scrcpyProcess, &QProcess::errorOccurred, this, &MainWindow::onScrcpyError);
    connect(scrcpyProcess, &QProcess::started, this, &MainWindow::onScrcpyStarted);
    connect(scrcpyProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::onScrcpyReadyReadStandardOutput);
    connect(scrcpyProcess, &QProcess::readyReadStandardError, this, &MainWindow::onScrcpyReadyReadStandardError);

    // Build scrcpy command
    QStringList arguments;

    // Only use --new-display and --start-app if launching specific app
    if (!packageName.isEmpty()) {
        arguments << "--new-display";
        arguments << "--start-app=" + packageName;
    }

    // Load settings
    QSettings settings("ScrcpyGUI", "Settings");

    // General
    if (settings.value("always-on-top", false).toBool()) arguments << "--always-on-top";
    if (settings.value("no-control", false).toBool()) arguments << "--no-control";
    if (settings.value("stay-awake", false).toBool()) arguments << "--stay-awake";
    if (settings.value("turn-screen-off", false).toBool()) arguments << "--turn-screen-off";
    if (settings.value("no-vd-destroy-content", false).toBool()) arguments << "--no-vd-destroy-content";
    if (settings.value("show-touches", false).toBool()) arguments << "--show-touches";
    if (settings.value("disable-screensaver", false).toBool()) arguments << "--disable-screensaver";

    // Video
    int maxSize = settings.value("max-size", 0).toInt();
    if (maxSize > 0) arguments << "--max-size" << QString::number(maxSize);

    QString bitRate = settings.value("bit-rate", "Default (8M)").toString();
    if (bitRate != "Default (8M)") arguments << "--bit-rate" << bitRate;

    int maxFps = settings.value("max-fps", 0).toInt();
    if (maxFps > 0) arguments << "--max-fps" << QString::number(maxFps);

    QString videoCodec = settings.value("video-codec", "Default (h264)").toString();
    if (videoCodec != "Default (h264)") arguments << "--video-codec" << videoCodec;

    int rotation = settings.value("rotation", 0).toInt();
    if (rotation > 0) arguments << "--rotation" << QString::number(rotation);

    if (settings.value("no-mipmaps", false).toBool()) arguments << "--no-mipmaps";

    // Audio
    if (settings.value("no-audio", false).toBool()) arguments << "--no-audio";
    
    QString audioCodec = settings.value("audio-codec", "Default (opus)").toString();
    if (audioCodec != "Default (opus)") arguments << "--audio-codec" << audioCodec;

    QString audioBitRate = settings.value("audio-bit-rate", "Default (128K)").toString();
    if (audioBitRate != "Default (128K)") arguments << "--audio-bit-rate" << audioBitRate;

    // Window
    if (settings.value("fullscreen", false).toBool()) arguments << "--fullscreen";
    if (settings.value("window-borderless", false).toBool()) arguments << "--window-borderless";
    
    QString windowTitle = settings.value("window-title", "").toString();
    if (!windowTitle.isEmpty()) {
        arguments << "--window-title" << windowTitle;
    } else {
        arguments << "--window-title" << appName;
    }

    // Advanced
    QString customArgs = settings.value("custom-args", "").toString();
    if (!customArgs.isEmpty()) {
        arguments << customArgs.split(" ", Qt::SkipEmptyParts);
    }

    qDebug() << "Launching scrcpy with args:" << arguments;

    m_logText += QString("<span style='color:#4fc3f7;'>========================================</span><br>");
    m_logText += QString("<span style='color:#4fc3f7;'>[%1] Launching scrcpy: %2</span><br>")
                  .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                  .arg(appName.toHtmlEscaped());
    if (!packageName.isEmpty()) {
        m_logText += QString("<span style='color:#9e9e9e;'>Package: %1</span><br>").arg(packageName.toHtmlEscaped());
    }
    m_logText += QString("<span style='color:#9e9e9e;'>Command: scrcpy %1</span><br>").arg(arguments.join(" ").toHtmlEscaped());
    m_logText += QString("<span style='color:#4fc3f7;'>========================================</span><br>");
    emit logTextChanged();

    m_scrcpyStatusText = "Starting " + appName + "...";
    emit scrcpyStatusTextChanged();
    
    scrcpyProcess->start("scrcpy", arguments);
}

void MainWindow::stopScrcpy()
{
    if (scrcpyProcess && scrcpyProcess->state() == QProcess::Running) {
        appendLog("Terminating scrcpy...", "#ff9800");
        scrcpyProcess->terminate();
        if (!scrcpyProcess->waitForFinished(3000)) {
            appendLog("Force killing scrcpy...", "#f44336");
            scrcpyProcess->kill();
        }
    }
}

void MainWindow::appendLog(const QString &text, const QString &color)
{
    m_logText += QString("<span style='color:%1;'>%2</span><br>")
                  .arg(color)
                  .arg(text.toHtmlEscaped());
    emit logTextChanged();
}

void MainWindow::onStopScrcpyClicked()
{
    stopScrcpy();
}

void MainWindow::onClearLogsClicked()
{
    m_logText.clear();
    appendLog("Logs cleared", "#9e9e9e");
}

void MainWindow::onScrcpyStarted()
{
    qDebug() << "Scrcpy started successfully";
    appendLog("Scrcpy started successfully!", "#4caf50");
    m_scrcpyStatusText = "Running: " + currentAppName;
    m_scrcpyStatusColor = "#4caf50";
    m_scrcpyRunning = true;
    emit scrcpyStatusTextChanged();
    emit scrcpyStatusColorChanged();
    emit scrcpyRunningChanged();
}

void MainWindow::onScrcpyFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Scrcpy finished with exit code:" << exitCode;

    m_scrcpyRunning = false;
    emit scrcpyRunningChanged();

    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        appendLog("Scrcpy closed normally", "#4caf50");
        m_scrcpyStatusText = "No scrcpy running";
        m_scrcpyStatusColor = "gray";
    } else {
        appendLog(QString("Scrcpy exited with error code: %1").arg(exitCode), "#f44336");
        m_scrcpyStatusText = "Scrcpy error - see logs";
        m_scrcpyStatusColor = "#f44336";
    }
    
    emit scrcpyStatusTextChanged();
    emit scrcpyStatusColorChanged();
}

void MainWindow::onScrcpyError(QProcess::ProcessError error)
{
    QString errorMsg;

    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Failed to start scrcpy. Make sure scrcpy is installed and in your PATH.";
            appendLog("ERROR: " + errorMsg, "#f44336");
            // Note: QMessageBox in QML context requires special handling
            break;
        case QProcess::Crashed:
            errorMsg = "Scrcpy crashed";
            appendLog("ERROR: " + errorMsg, "#f44336");
            break;
        case QProcess::Timedout:
            errorMsg = "Scrcpy operation timed out";
            appendLog("ERROR: " + errorMsg, "#f44336");
            break;
        default:
            errorMsg = "An error occurred with scrcpy: " + scrcpyProcess->errorString();
            appendLog("ERROR: " + errorMsg, "#f44336");
    }

    qDebug() << "Scrcpy error:" << errorMsg;
    m_scrcpyStatusText = "Error - see logs";
    m_scrcpyStatusColor = "#f44336";
    m_scrcpyRunning = false;
    
    emit scrcpyStatusTextChanged();
    emit scrcpyStatusColorChanged();
    emit scrcpyRunningChanged();
}

void MainWindow::onScrcpyReadyReadStandardOutput()
{
    QString output = QString::fromLocal8Bit(scrcpyProcess->readAllStandardOutput());
    if (!output.trimmed().isEmpty()) {
        appendLog("[stdout] " + output.trimmed(), "#d4d4d4");
    }
}

void MainWindow::onScrcpyReadyReadStandardError()
{
    QString output = QString::fromLocal8Bit(scrcpyProcess->readAllStandardError());
    if (!output.trimmed().isEmpty()) {
        // stderr dari scrcpy biasanya info, bukan error
        appendLog("[stderr] " + output.trimmed(), "#ffc107");
    }
}

void MainWindow::onMirrorDeviceClicked()
{
    qDebug() << "Launching full device mirror";

    // Stop current scrcpy if running
    if (scrcpyProcess && scrcpyProcess->state() == QProcess::Running) {
        appendLog("Stopping current scrcpy session...", "#ff9800");
        stopScrcpy();
    }

    // Launch scrcpy in full device mirror mode (empty packageName)
    launchScrcpy("", "Full Device Mirror");
}

void MainWindow::onRefreshClicked()
{
    loadAppList();
}

void MainWindow::onManualAddClicked()
{
    // Note: QInputDialog in QML context - we'll keep using Qt Widgets dialogs
    bool ok;
    QString packageName = QInputDialog::getText(nullptr, "Manual Add App",
                                                "Enter package name (e.g., com.example.app):",
                                                QLineEdit::Normal, "", &ok);

    if (ok && !packageName.isEmpty()) {
        // Validate package name format
        if (!packageName.contains(".")) {
            return;
        }

        // Ask for display name
        QString displayName = QInputDialog::getText(nullptr, "App Display Name",
                                                   "Enter display name:",
                                                   QLineEdit::Normal, packageName, &ok);

        if (ok && !displayName.isEmpty()) {
            AppInfo appInfo;
            appInfo.packageName = packageName;
            appInfo.name = displayName;
            appInfo.isCustom = true;

            appManager->saveCustomApp(appInfo);
            setStatusText("Added: " + displayName);
            
            // Reload app list
            loadAppList();
        }
    }
}

void MainWindow::onAppsLoaded(const QList<AppInfo> &apps)
{
    allLoadedApps = apps;
    
    if (apps.isEmpty()) {
        setStatusText("No apps found. Make sure device is connected.");
    } else {
        // Apply current filter
        if (showRunningOnly) {
            // Reload running apps then apply filter
            appManager->loadRunningApps();
        } else {
            applyFilter();
        }
    }
}

void MainWindow::onLoadError(const QString &error)
{
    setStatusText("Error: " + error);
}

void MainWindow::onAbout()
{
    QMessageBox::about(nullptr, "About Qt GUI Scrcpy",
                      "<h2>Qt GUI Scrcpy</h2>"
                      "<p>Version 1.0.8</p>"
                      "<p>A cross-platform desktop GUI for scrcpy</p>"
                      "<p><a href='https://github.com/Genymobile/scrcpy'>scrcpy</a> "
                      "is a free and open source tool to display and control Android devices.</p>"
                      "<p>This application provides a native GUI interface for managing "
                      "and launching scrcpy instances.</p>"
                      "<p>Licensed under MIT License</p>");
}

void MainWindow::onSettings()
{
    // Load QML settings dialog
    QQmlEngine *engine = qmlEngine(this);
    if (!engine) {
        qWarning() << "Cannot create settings dialog: no QML engine";
        return;
    }

    QQmlComponent component(engine, QUrl("qrc:/qml/SettingsDialog.qml"));
    QObject *dialog = component.create();
    
    if (dialog) {
        QMetaObject::invokeMethod(dialog, "open");
    } else {
        qWarning() << "Failed to create settings dialog:" << component.errorString();
    }
}

void MainWindow::onFilterChanged(bool showRunningOnly)
{
    this->showRunningOnly = showRunningOnly;
    
    if (showRunningOnly) {
        // Load running apps from device
        setStatusText("Loading running apps...");
        appManager->loadRunningApps();
    } else {
        // Show all apps immediately
        applyFilter();
    }
}

void MainWindow::onRunningAppsLoaded(const QSet<QString> &packages)
{
    runningPackages = packages;
    qDebug() << "Running packages loaded:" << runningPackages.size();
    
    if (showRunningOnly) {
        applyFilter();
        setStatusText(QString("Showing %1 running apps").arg(runningPackages.size()));
    }
}

void MainWindow::applyFilter()
{
    QList<AppInfo> filteredApps;
    
    if (showRunningOnly) {
        // Show only running apps
        for (const AppInfo &app : allLoadedApps) {
            if (runningPackages.contains(app.packageName)) {
                filteredApps.append(app);
            }
        }
        
        if (filteredApps.isEmpty()) {
            setStatusText("No running apps found");
        } else {
            setStatusText(QString("%1 running apps").arg(filteredApps.size()));
        }
    } else {
        // Show all apps
        filteredApps = allLoadedApps;
        setStatusText(QString("Loaded %1 apps").arg(allLoadedApps.size()));
    }
    
    appListModel->setApps(filteredApps);
}
