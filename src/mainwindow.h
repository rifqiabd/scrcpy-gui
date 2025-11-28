#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "appmanager.h"
#include "scrcpywindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAppSelected(QListWidgetItem *item);
    void onRefreshClicked();
    void onManualAddClicked();
    void onAppsLoaded(const QList<AppInfo> &apps);
    void onLoadError(const QString &error);

private:
    void setupUI();
    void loadAppList();
    void addAppToList(const AppInfo &appInfo);

    // UI Components
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QListWidget *appListWidget;
    QHBoxLayout *buttonLayout;
    QPushButton *refreshButton;
    QPushButton *manualAddButton;
    QLabel *statusLabel;

    // Business Logic
    AppManager *appManager;

    // Track opened windows
    QList<ScrcpyWindow*> openWindows;
};

#endif // MAINWINDOW_H
