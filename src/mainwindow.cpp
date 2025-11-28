#include "mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , appManager(new AppManager(this))
{
    setupUI();

    // Connect signals
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
}

void MainWindow::setupUI()
{
    setWindowTitle("Qt GUI Scrcpy");
    resize(600, 500);

    // Central widget
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);

    // Title
    titleLabel = new QLabel("Android Apps", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // App list
    appListWidget = new QListWidget(this);
    connect(appListWidget, &QListWidget::itemClicked, this, &MainWindow::onAppSelected);
    mainLayout->addWidget(appListWidget);

    // Buttons
    buttonLayout = new QHBoxLayout();

    refreshButton = new QPushButton("Refresh", this);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
    buttonLayout->addWidget(refreshButton);

    manualAddButton = new QPushButton("Manual Add", this);
    connect(manualAddButton, &QPushButton::clicked, this, &MainWindow::onManualAddClicked);
    buttonLayout->addWidget(manualAddButton);

    mainLayout->addLayout(buttonLayout);

    // Status label
    statusLabel = new QLabel("Ready", this);
    statusLabel->setStyleSheet("color: gray; padding: 5px;");
    mainLayout->addWidget(statusLabel);
}

void MainWindow::loadAppList()
{
    statusLabel->setText("Loading apps...");
    appListWidget->clear();
    refreshButton->setEnabled(false);

    appManager->loadApps();
}

void MainWindow::addAppToList(const AppInfo &appInfo)
{
    QListWidgetItem *item = new QListWidgetItem(appInfo.name);
    item->setData(Qt::UserRole, appInfo.packageName);
    appListWidget->addItem(item);
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

            statusLabel->setText("Added: " + displayName);
        }
    }
}

void MainWindow::onAppsLoaded(const QList<AppInfo> &apps)
{
    appListWidget->clear();

    if (apps.isEmpty()) {
        statusLabel->setText("No apps found. Make sure device is connected.");
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
        statusLabel->setText(QString("Loaded %1 apps").arg(apps.size()));
    }

    refreshButton->setEnabled(true);
}

void MainWindow::onLoadError(const QString &error)
{
    statusLabel->setText("Error: " + error);
    refreshButton->setEnabled(true);

    QMessageBox::critical(this, "Error Loading Apps", error);
}
