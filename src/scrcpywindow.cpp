#include "scrcpywindow.h"
#include <QSettings>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>

ScrcpyWindow::ScrcpyWindow(const QString &packageName, const QString &appName, QWidget *parent)
    : QMainWindow(parent)
    , packageName(packageName)
    , appName(appName)
    , scrcpyProcess(nullptr)
{
    setupUI();
    setWindowTitle("Scrcpy - " + appName);
}

ScrcpyWindow::~ScrcpyWindow()
{
    if (scrcpyProcess && scrcpyProcess->state() == QProcess::Running) {
        scrcpyProcess->terminate();
        if (!scrcpyProcess->waitForFinished(3000)) {
            scrcpyProcess->kill();
        }
    }
}

void ScrcpyWindow::setupUI()
{
    resize(400, 300);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);

    statusLabel = new QLabel("Preparing to launch scrcpy...", this);
    statusLabel->setWordWrap(true);
    statusLabel->setStyleSheet("padding: 20px; font-size: 12pt;");
    mainLayout->addWidget(statusLabel);

    mainLayout->addStretch();

    stopButton = new QPushButton("Stop Scrcpy", this);
    stopButton->setEnabled(false);
    connect(stopButton, &QPushButton::clicked, this, &ScrcpyWindow::onStopClicked);
    mainLayout->addWidget(stopButton);
}

void ScrcpyWindow::launchScrcpy()
{
    if (scrcpyProcess) {
        delete scrcpyProcess;
    }

    scrcpyProcess = new QProcess(this);

    // Connect signals
    connect(scrcpyProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &ScrcpyWindow::onScrcpyFinished);
    connect(scrcpyProcess, &QProcess::errorOccurred, this, &ScrcpyWindow::onScrcpyError);
    connect(scrcpyProcess, &QProcess::started, this, &ScrcpyWindow::onScrcpyStarted);

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

    statusLabel->setText("Launching scrcpy for " + appName + "...\n\n"
                        "Package: " + packageName);

    scrcpyProcess->start("scrcpy", arguments);
}

void ScrcpyWindow::onScrcpyStarted()
{
    qDebug() << "Scrcpy started successfully";
    statusLabel->setText("Scrcpy is running for " + appName + "\n\n"
                        "The scrcpy window should appear separately.\n"
                        "You can close this control window or click Stop to terminate scrcpy.");
    stopButton->setEnabled(true);
}

void ScrcpyWindow::onScrcpyFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Scrcpy finished with exit code:" << exitCode;

    stopButton->setEnabled(false);

    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        statusLabel->setText("Scrcpy closed normally.");
    } else {
        QString errorOutput = scrcpyProcess->readAllStandardError();
        qCritical() << "Scrcpy error output:" << errorOutput;
        statusLabel->setText("Scrcpy exited with error code: " + QString::number(exitCode) + 
                           "\n\nError output:\n" + errorOutput);
    }

    // Auto-close window after 2 seconds
    QTimer::singleShot(2000, this, &QWidget::close);
}

void ScrcpyWindow::onScrcpyError(QProcess::ProcessError error)
{
    QString errorMsg;

    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Failed to start scrcpy.\n\n"
                      "Make sure scrcpy is installed and in your PATH.\n"
                      "You can install it from: https://github.com/Genymobile/scrcpy";
            break;
        case QProcess::Crashed:
            errorMsg = "Scrcpy crashed.";
            break;
        case QProcess::Timedout:
            errorMsg = "Scrcpy operation timed out.";
            break;
        default:
            errorMsg = "An error occurred with scrcpy: " + scrcpyProcess->errorString();
    }

    qDebug() << "Scrcpy error:" << errorMsg;
    statusLabel->setText(errorMsg);
    stopButton->setEnabled(false);

    QMessageBox::critical(this, "Scrcpy Error", errorMsg);
}

void ScrcpyWindow::onStopClicked()
{
    if (scrcpyProcess && scrcpyProcess->state() == QProcess::Running) {
        statusLabel->setText("Stopping scrcpy...");
        stopButton->setEnabled(false);
        scrcpyProcess->terminate();
    }
}
