#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
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
    void saveCustomApp(const AppInfo &app);
    QList<AppInfo> getCustomApps();

signals:
    void appsLoaded(const QList<AppInfo> &apps);
    void loadError(const QString &error);

private slots:
    void onAdbFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onAdbError(QProcess::ProcessError error);

private:
    void loadCustomApps();
    void saveCustomApps();
    QString getConfigFilePath();
    QString packageToName(const QString &packageName);

    QProcess *adbProcess;
    QList<AppInfo> customApps;
    QList<AppInfo> allApps;
};

#endif // APPMANAGER_H
