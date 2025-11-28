#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include "appmanager.h"
#include "scrcpywindow.h"

namespace Ui {
class MainWindow;
}

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
    void onExit();
    void onAbout();

private:
    void setupUI();
    void loadAppList();
    void addAppToList(const AppInfo &appInfo);

    // UI from Qt Designer
    Ui::MainWindow *ui;

    // Business Logic
    AppManager *appManager;

    // Track opened windows
    QList<ScrcpyWindow*> openWindows;
};

#endif // MAINWINDOW_H
