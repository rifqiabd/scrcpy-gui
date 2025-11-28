#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QSet>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

struct AppInfo {
    QString packageName;
    QString name;
    bool isCustom = false;
};

class AppManager : public QObject
{
    Q_OBJECT

public:
    explicit AppManager(QObject *parent = nullptr);
    ~AppManager();

    void loadApps();
    void loadRunningApps();
    void saveCustomApp(const AppInfo &app);
    QList<AppInfo> getCustomApps();
    QSet<QString> getRunningPackages() const;

signals:
    void appsLoaded(const QList<AppInfo> &apps);
    void loadError(const QString &error);
    void runningAppsLoaded(const QSet<QString> &packages);

private slots:
    void onAdbFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onAdbError(QProcess::ProcessError error);
    void onRunningAppsFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void loadCustomApps();
    void saveCustomApps();
    QString getConfigFilePath();
    QString packageToName(const QString &packageName);

    QProcess *adbProcess;
    QProcess *runningAppsProcess;
    QList<AppInfo> customApps;
    QList<AppInfo> allApps;
    QSet<QString> runningPackages;
};

#endif // APPMANAGER_H
