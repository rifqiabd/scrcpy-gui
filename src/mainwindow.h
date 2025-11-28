#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QProcess>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include "appmanager.h"

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
    void onSettings();
    
    // Scrcpy control slots
    void onStopScrcpyClicked();
    void onClearLogsClicked();
    void onScrcpyStarted();
    void onScrcpyFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onScrcpyError(QProcess::ProcessError error);
    void onScrcpyReadyReadStandardOutput();
    void onScrcpyReadyReadStandardError();

private:
    void setupUI();
    void loadAppList();
    void addAppToList(const AppInfo &appInfo);
    void launchScrcpy(const QString &packageName, const QString &appName);
    void stopScrcpy();
    void appendLog(const QString &text, const QString &color = "#d4d4d4");

    // UI from Qt Designer
    Ui::MainWindow *ui;

    // Business Logic
    AppManager *appManager;
    
    // Scrcpy process management
    QProcess *scrcpyProcess;
    QString currentPackageName;
    QString currentAppName;
};

#endif // MAINWINDOW_H
