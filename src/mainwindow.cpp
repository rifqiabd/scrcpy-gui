#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <QSettings>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , appManager(new AppManager(this))
    , scrcpyProcess(nullptr)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/resources/icon.png"));
    
    // Set splitter initial sizes (60% left, 40% right)
    ui->splitter->setSizes(QList<int>() << 600 << 400);
    
    // Connect signals from UI elements
    connect(ui->appListWidget, &QListWidget::itemClicked, this, &MainWindow::onAppSelected);
    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
    connect(ui->manualAddButton, &QPushButton::clicked, this, &MainWindow::onManualAddClicked);
    
    // Connect scrcpy control buttons
    connect(ui->stopScrcpyButton, &QPushButton::clicked, this, &MainWindow::onStopScrcpyClicked);
    connect(ui->clearLogsButton, &QPushButton::clicked, this, &MainWindow::onClearLogsClicked);
    
    // Connect menu actions
    connect(ui->actionRefresh, &QAction::triggered, this, &MainWindow::onRefreshClicked);
    connect(ui->actionManualAdd, &QAction::triggered, this, &MainWindow::onManualAddClicked);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onExit);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);

    // Create Settings menu item programmatically since we can't edit .ui easily
    QAction *actionSettings = new QAction("Settings", this);
    ui->menuBar->addAction(actionSettings);
    connect(actionSettings, &QAction::triggered, this, &MainWindow::onSettings);

    // Connect app manager signals
    connect(appManager, &AppManager::appsLoaded, this, &MainWindow::onAppsLoaded);
    connect(appManager, &AppManager::loadError, this, &MainWindow::onLoadError);

    // Load apps on startup
    loadAppList();
}

MainWindow::~MainWindow()
{
    stopScrcpy();
    delete ui;
}

void MainWindow::setupUI()
{
    // No longer needed - UI is set up by ui->setupUi(this)
}

void MainWindow::loadAppList()
{
    ui->statusLabel->setText("Loading apps...");
    ui->appListWidget->clear();
    ui->refreshButton->setEnabled(false);

    appManager->loadApps();
}

void MainWindow::addAppToList(const AppInfo &appInfo)
{
    QListWidgetItem *item = new QListWidgetItem(appInfo.name);
    item->setData(Qt::UserRole, appInfo.packageName);
    ui->appListWidget->addItem(item);
}

void MainWindow::onAppSelected(QListWidgetItem *item)
{
    if (!item) return;

    QString packageName = item->data(Qt::UserRole).toString();
    QString appName = item->text();

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

    arguments << "--new-display";
    arguments << "--start-app=" + packageName;

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

    appendLog("========================================", "#4fc3f7");
    appendLog(QString("[%1] Launching scrcpy for %2")
              .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
              .arg(appName), "#4fc3f7");
    appendLog("Package: " + packageName, "#9e9e9e");
    appendLog("Command: scrcpy " + arguments.join(" "), "#9e9e9e");
    appendLog("========================================", "#4fc3f7");

    ui->scrcpyStatusLabel->setText("Starting scrcpy for " + appName + "...");
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
    QString coloredText = QString("<span style='color:%1;'>%2</span>")
                          .arg(color)
                          .arg(text.toHtmlEscaped());
    ui->logTextEdit->append(coloredText);
}

void MainWindow::onStopScrcpyClicked()
{
    stopScrcpy();
}

void MainWindow::onClearLogsClicked()
{
    ui->logTextEdit->clear();
    appendLog("Logs cleared", "#9e9e9e");
}

void MainWindow::onScrcpyStarted()
{
    qDebug() << "Scrcpy started successfully";
    appendLog("Scrcpy started successfully!", "#4caf50");
    ui->scrcpyStatusLabel->setText("Running: " + currentAppName);
    ui->scrcpyStatusLabel->setStyleSheet("color: #4caf50; padding: 5px;");
    ui->stopScrcpyButton->setEnabled(true);
}

void MainWindow::onScrcpyFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Scrcpy finished with exit code:" << exitCode;

    ui->stopScrcpyButton->setEnabled(false);

    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        appendLog("Scrcpy closed normally", "#4caf50");
        ui->scrcpyStatusLabel->setText("No scrcpy running");
        ui->scrcpyStatusLabel->setStyleSheet("color: gray; padding: 5px;");
    } else {
        appendLog(QString("Scrcpy exited with error code: %1").arg(exitCode), "#f44336");
        ui->scrcpyStatusLabel->setText("Scrcpy error - see logs");
        ui->scrcpyStatusLabel->setStyleSheet("color: #f44336; padding: 5px;");
    }
}

void MainWindow::onScrcpyError(QProcess::ProcessError error)
{
    QString errorMsg;

    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Failed to start scrcpy. Make sure scrcpy is installed and in your PATH.";
            appendLog("ERROR: " + errorMsg, "#f44336");
            QMessageBox::critical(this, "Scrcpy Error", errorMsg + "\n\nYou can install it from: https://github.com/Genymobile/scrcpy");
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
    ui->scrcpyStatusLabel->setText("Error - see logs");
    ui->scrcpyStatusLabel->setStyleSheet("color: #f44336; padding: 5px;");
    ui->stopScrcpyButton->setEnabled(false);
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

void MainWindow::onRefreshClicked()
{
    loadAppList();
}

void MainWindow::onManualAddClicked()
{
    bool ok;
    QString packageName = QInputDialog::getText(this, "Manual Add App",
                                                "Enter package name (e.g., com.example.app):",
                                                QLineEdit::Normal, "", &ok);

    if (ok && !packageName.isEmpty()) {
        // Validate package name format
        if (!packageName.contains(".")) {
            QMessageBox::warning(this, "Invalid Package Name",
                               "Package name should be in format: com.example.app");
            return;
        }

        // Ask for display name
        QString displayName = QInputDialog::getText(this, "App Display Name",
                                                   "Enter display name:",
                                                   QLineEdit::Normal, packageName, &ok);

        if (ok && !displayName.isEmpty()) {
            AppInfo appInfo;
            appInfo.packageName = packageName;
            appInfo.name = displayName;
            appInfo.isCustom = true;

            addAppToList(appInfo);
            appManager->saveCustomApp(appInfo);

            ui->statusLabel->setText("Added: " + displayName);
        }
    }
}

void MainWindow::onAppsLoaded(const QList<AppInfo> &apps)
{
    ui->appListWidget->clear();

    if (apps.isEmpty()) {
        ui->statusLabel->setText("No apps found. Make sure device is connected.");
        QMessageBox::information(this, "No Apps Found",
                               "No Android apps found.\n\n"
                               "Make sure:\n"
                               "1. Android device is connected via USB\n"
                               "2. USB debugging is enabled\n"
                               "3. ADB is installed and in PATH");
    } else {
        for (const AppInfo &app : apps) {
            addAppToList(app);
        }
        ui->statusLabel->setText(QString("Loaded %1 apps").arg(apps.size()));
    }

    ui->refreshButton->setEnabled(true);
}

void MainWindow::onLoadError(const QString &error)
{
    ui->statusLabel->setText("Error: " + error);
    ui->refreshButton->setEnabled(true);

    QMessageBox::critical(this, "Error Loading Apps", error);
}

void MainWindow::onExit()
{
    close();
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "About Qt GUI Scrcpy",
                      "<h2>Qt GUI Scrcpy</h2>"
                      "<p>Version 1.0.0</p>"
                      "<p>A cross-platform desktop GUI for scrcpy</p>"
                      "<p><a href='https://github.com/Genymobile/scrcpy'>scrcpy</a> "
                      "is a free and open source tool to display and control Android devices.</p>"
                      "<p>This application provides a native GUI interface for managing "
                      "and launching scrcpy instances.</p>"
                      "<p>Licensed under MIT License</p>");
}

void MainWindow::onSettings()
{
    SettingsDialog dialog(this);
    dialog.exec();
}
