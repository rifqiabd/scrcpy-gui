#include "scrcpywindow.h"
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
    arguments << "-Sw";
    arguments << "-t";
    arguments << "--start-app=" + packageName;
    arguments << "--window-title" << appName;

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
