#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QProcess>
#include "appmanager.h"
#include "applistmodel.h"

class MainWindow : public QObject
{
    Q_OBJECT
    
    // Properties exposed to QML
    Q_PROPERTY(QString statusText READ statusText WRITE setStatusText NOTIFY statusTextChanged)
    Q_PROPERTY(QString logText READ logText NOTIFY logTextChanged)
    Q_PROPERTY(QString scrcpyStatusText READ scrcpyStatusText NOTIFY scrcpyStatusTextChanged)
    Q_PROPERTY(QString scrcpyStatusColor READ scrcpyStatusColor NOTIFY scrcpyStatusColorChanged)
    Q_PROPERTY(bool scrcpyRunning READ scrcpyRunning NOTIFY scrcpyRunningChanged)

public:
    explicit MainWindow(QObject *parent = nullptr);
    ~MainWindow();

    // Property getters
    QString statusText() const { return m_statusText; }
    QString logText() const { return m_logText; }
    QString scrcpyStatusText() const { return m_scrcpyStatusText; }
    QString scrcpyStatusColor() const { return m_scrcpyStatusColor; }
    bool scrcpyRunning() const { return m_scrcpyRunning; }

    // Property setters
    void setStatusText(const QString &text);

    // Get models for QML
    AppListModel* getAppListModel() { return appListModel; }

    // Invokable methods for QML
    Q_INVOKABLE void windowReady();
    Q_INVOKABLE void onAppSelected(const QString &packageName, const QString &appName);
    Q_INVOKABLE void onRefreshClicked();
    Q_INVOKABLE void onManualAddClicked();
    Q_INVOKABLE void onMirrorDeviceClicked();
    Q_INVOKABLE void onStopScrcpyClicked();
    Q_INVOKABLE void onClearLogsClicked();
    Q_INVOKABLE void onAbout();
    Q_INVOKABLE void onSettings();
    Q_INVOKABLE void onFilterChanged(bool showRunningOnly);

signals:
    void statusTextChanged();
    void logTextChanged();
    void scrcpyStatusTextChanged();
    void scrcpyStatusColorChanged();
    void scrcpyRunningChanged();

private slots:
    void onAppsLoaded(const QList<AppInfo> &apps);
    void onLoadError(const QString &error);
    void onRunningAppsLoaded(const QSet<QString> &packages);
    
    // Scrcpy control slots
    void onScrcpyStarted();
    void onScrcpyFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onScrcpyError(QProcess::ProcessError error);
    void onScrcpyReadyReadStandardOutput();
    void onScrcpyReadyReadStandardError();

private:
    void loadAppList();
    void launchScrcpy(const QString &packageName, const QString &appName);
    void stopScrcpy();
    void appendLog(const QString &text, const QString &color = "#d4d4d4");
    void applyFilter();

    // Business Logic
    AppManager *appManager;
    AppListModel *appListModel;
    
    // Scrcpy process management
    QProcess *scrcpyProcess;
    QString currentPackageName;
    QString currentAppName;
    
    // Filter state
    bool showRunningOnly;
    QList<AppInfo> allLoadedApps;
    QSet<QString> runningPackages;

    // Properties
    QString m_statusText;
    QString m_logText;
    QString m_scrcpyStatusText;
    QString m_scrcpyStatusColor;
    bool m_scrcpyRunning;
};

#endif // MAINWINDOW_H
