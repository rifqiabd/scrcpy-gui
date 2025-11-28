#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , appManager(new AppManager(this))
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/resources/icon.png"));
    
    // Connect signals from UI elements
    connect(ui->appListWidget, &QListWidget::itemClicked, this, &MainWindow::onAppSelected);
    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
    connect(ui->manualAddButton, &QPushButton::clicked, this, &MainWindow::onManualAddClicked);
    
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
    // Clean up opened windows
    for (ScrcpyWindow *window : openWindows) {
        if (window) {
            window->close();
        }
    }
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

    // Create new window for scrcpy
    ScrcpyWindow *window = new ScrcpyWindow(packageName, appName, this);
    window->setAttribute(Qt::WA_DeleteOnClose);

    // Track the window
    openWindows.append(window);
    connect(window, &QObject::destroyed, this, [this, window]() {
        openWindows.removeOne(window);
    });

    window->show();
    window->launchScrcpy();
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

